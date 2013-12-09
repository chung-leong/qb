/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2012 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: Chung Leong <cleong@cal.berkeley.edu>                        |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#include "qb.h"

static int32_t qb_retrieve_operand(qb_php_translator_context *cxt, uint32_t zoperand_type, znode_op *zoperand, qb_operand *operand) {
	switch(zoperand_type) {
		case Z_OPERAND_CV: {
			uint32_t var_index = Z_OPERAND_INFO(*zoperand, var);
			qb_variable *qvar = cxt->compiler_context->variables[var_index];
			if(!qvar->address) {
				// the variable type hasn't been set yet
				qvar->flags |= QB_VARIABLE_LOCAL;
				qb_apply_type_declaration(cxt->compiler_context, qvar);
			}
			operand->address = qvar->address;
			operand->type = QB_OPERAND_ADDRESS;
		}	break;
		case Z_OPERAND_CONST: {
			operand->constant = Z_OPERAND_ZV(*zoperand);
			operand->type = QB_OPERAND_ZVAL;
		}	break;
		case Z_OPERAND_TMP_VAR:
		case Z_OPERAND_VAR: {
			uint32_t temp_var_index = Z_OPERAND_TMP_INDEX(zoperand);
			if(temp_var_index < cxt->temp_variable_count) {
				qb_temporary_variable *temp_variable = &cxt->temp_variables[temp_var_index];
				*operand = temp_variable->operand;
				if(cxt->compiler_context->stage == QB_STAGE_CONSTANT_EXPRESSION_EVALUATION) {
					temp_variable->last_access_op_index = cxt->zend_op_index;
				} else if(cxt->compiler_context->stage == QB_STAGE_OPCODE_TRANSLATION) {
#ifdef ZEND_DEBUG
					if(temp_variable->freed) {
						qb_abort("Op %d is accessing temporary varaible #%d which was freed by op %d", cxt->zend_op_index, temp_var_index, temp_variable->last_access_op_index);
					}
#endif
				}
				break;
			}
		}	// fall through in case of invalid index
		default: {
			operand->type = QB_OPERAND_NONE;
			operand->generic_pointer = NULL;
			return FALSE;
		}
	}
	return TRUE;
}

static void qb_retire_operand(qb_php_translator_context *cxt, uint32_t zoperand_type, znode_op *zoperand, qb_operand *operand) {
	switch(zoperand_type) {
		case Z_OPERAND_TMP_VAR:
		case Z_OPERAND_VAR: {
			uint32_t temp_var_index = Z_OPERAND_TMP_INDEX(zoperand);
			if(temp_var_index < cxt->temp_variable_count) {
				qb_temporary_variable *temp_variable = &cxt->temp_variables[temp_var_index];
				if(cxt->compiler_context->stage == QB_STAGE_OPCODE_TRANSLATION) {
					if(temp_variable->last_access_op_index == cxt->zend_op_index) {
						// unlock the operand if the current op is the last to access it
						qb_unlock_operand(cxt->compiler_context, &temp_variable->operand);
						temp_variable->operand.type = QB_OPERAND_EMPTY;
						temp_variable->operand.generic_pointer = NULL;
#ifdef ZEND_DEBUG
						temp_variable->freed = TRUE;
#endif
					} else {
						if(temp_variable->operand.type != operand->type || temp_variable->operand.address != operand->address) {
							// unlock what was there before if it's different
							qb_unlock_operand(cxt->compiler_context, &temp_variable->operand);
							temp_variable->operand = *operand;
							qb_lock_operand(cxt->compiler_context, &temp_variable->operand);
						}
					}
				} else {
					temp_variable->operand = *operand;
				}
			}
		}	break;
	}
}

static qb_operand * qb_push_stack_item(qb_php_translator_context *cxt) {
	if(cxt->stack_item_offset + cxt->stack_item_count + 1 > cxt->stack_item_buffer_size) {
		uint32_t i;
		cxt->stack_item_buffer_size += 2;		// TODO: make it bigger
		cxt->stack_items = erealloc(cxt->stack_items, cxt->stack_item_buffer_size * sizeof(qb_operand *));
		for(i = 0; i < 2; i++) {
			cxt->stack_items[cxt->stack_item_offset + cxt->stack_item_count + i] = qb_allocate_operands(cxt->pool, 1);
		}
	}
	return cxt->stack_items[cxt->stack_item_offset + cxt->stack_item_count++];
}

static qb_operand ** qb_get_stack_items(qb_php_translator_context *cxt, int32_t index) {
	if(index < 0) {
		qb_abort("stack underflow");
	}
	return &cxt->stack_items[cxt->stack_item_offset + index];
}

static qb_operand ** qb_pop_stack_items(qb_php_translator_context *cxt, int32_t count) {
	int32_t index = cxt->stack_item_count - count;
	cxt->stack_item_count -= count;
	return qb_get_stack_items(cxt, index);
}

static void qb_lock_temporary_variables(qb_php_translator_context *cxt) {
	uint32_t i, j;
	for(i = 0; i < cxt->temp_variable_count; i++) {
		qb_temporary_variable *temp_variable = &cxt->temp_variables[i];
		qb_lock_operand(cxt->compiler_context, &temp_variable->operand);
	}
	for(i = cxt->stack_item_offset, j = 0; j < cxt->stack_item_count; i++, j++) {
		qb_operand *stack_item = cxt->stack_items[i];
		qb_lock_operand(cxt->compiler_context, stack_item);
	}
	for(i = 0; i < cxt->compiler_context->on_demand_expression_count; i++) {
		qb_expression *expr = cxt->compiler_context->on_demand_expressions[i];
		if(expr->flags & QB_EXPR_RESULT_IS_STILL_VALID) {
			for(j = 0; j < expr->operand_count; j++) {
				qb_operand *operand = &expr->operands[j];
				qb_lock_operand(cxt->compiler_context, operand);
			}
			qb_lock_operand(cxt->compiler_context, expr->result);
		}
	}
}

static void qb_process_basic_op(qb_php_translator_context *cxt, void *op_factory, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_result_prototype *result_prototype) {
	qb_produce_op(cxt->compiler_context, op_factory, operands, operand_count, result, NULL, 0, result_prototype);
}

static void qb_process_combo_op(qb_php_translator_context *cxt, void *op_factories, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_result_prototype *result_prototype) {
	void **list = op_factories, *op_factory;

	if(cxt->zend_op->extended_value == ZEND_ASSIGN_DIM) {
		op_factory = list[QB_OP_FACTORY_DIM];
	} else if(cxt->zend_op->extended_value == ZEND_ASSIGN_OBJ) {
		op_factory = list[QB_OP_FACTORY_OBJ];
	} else {
		op_factory = list[QB_OP_FACTORY_VAR];
	}
	qb_produce_op(cxt->compiler_context, op_factory, operands, operand_count, result, NULL, 0, result_prototype);
}

static void qb_process_cast_op(qb_php_translator_context *cxt, void *op_factories, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_result_prototype *result_prototype) {
	void **list = op_factories, *op_factory;
	uint32_t zend_type = cxt->zend_op->extended_value;

	switch(zend_type) {
#if SIZEOF_LONG == 8
		case IS_LONG:	op_factory = list[QB_OP_FACTORY_INT64]; break;
#else
		case IS_LONG:	op_factory = list[QB_OP_FACTORY_INT32]; break;
#endif
		case IS_DOUBLE:	op_factory = list[QB_OP_FACTORY_DOUBLE]; break;
		case IS_BOOL:	op_factory = list[QB_OP_FACTORY_BOOLEAN]; break;
		case IS_ARRAY:	op_factory = list[QB_OP_FACTORY_ARRAY]; break;
		case IS_STRING: op_factory = list[QB_OP_FACTORY_STRING]; break;
		default:		
			qb_abort("Illegal cast");
	}
	qb_produce_op(cxt->compiler_context, op_factory, operands, operand_count, result, NULL, 0, result_prototype);
}

static void qb_process_fetch_class(qb_php_translator_context *cxt, void *op_factories, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_result_prototype *result_prototype) {
	int32_t fetch_type = cxt->zend_op->extended_value & ZEND_FETCH_CLASS_MASK;
	void **list = op_factories, *op_factory;

	if(fetch_type == ZEND_FETCH_CLASS_SELF) {
		op_factory = list[QB_OP_FACTORY_SELF];
	} else if(fetch_type == ZEND_FETCH_CLASS_PARENT) {
		op_factory = list[QB_OP_FACTORY_PARENT];
#if defined(ZEND_FETCH_CLASS_STATIC)
	} else if(fetch_type == ZEND_FETCH_CLASS_STATIC) {
		op_factory = list[QB_OP_FACTORY_STATIC];
#endif
	}
	qb_produce_op(cxt->compiler_context, op_factory, operands, operand_count, result, NULL, 0, result_prototype);
}

static void qb_process_fetch(qb_php_translator_context *cxt, void *op_factories, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_result_prototype *result_prototype) {
	USE_TSRM
	qb_operand *name = &operands[0], *scope = &operands[1];
	uint32_t fetch_type = FETCH_TYPE(cxt->zend_op);
	void **list = op_factories, *op_factory;

	if(fetch_type == ZEND_FETCH_LOCAL) {
		op_factory = list[QB_OP_FACTORY_LOCAL];
	} else if(fetch_type == ZEND_FETCH_GLOBAL || fetch_type == ZEND_FETCH_GLOBAL_LOCK) {
		op_factory = list[QB_OP_FACTORY_GLOBAL];
	} else if(fetch_type == ZEND_FETCH_STATIC) {
		op_factory = list[QB_OP_FACTORY_STATIC];
	} else if(fetch_type == ZEND_FETCH_STATIC_MEMBER) {
		op_factory = list[QB_OP_FACTORY_STATIC_MEMBER];
	}
	qb_produce_op(cxt->compiler_context, op_factory, operands, operand_count, result, NULL, 0, result_prototype);
}

static void qb_process_function_call_ex(qb_php_translator_context *cxt, void *op_factories, qb_operand *name, qb_operand *object, qb_operand **stack_pointer, uint32_t argument_count, qb_operand *result, qb_result_prototype *result_prototype) {
	qb_intrinsic_function *ifunc = NULL;
	zend_function *zfunc = NULL;
	qb_function *qfunc = NULL;

	qb_operand *arguments;
	qb_operand func_operands[4];
	int32_t external_call = FALSE;
	uint32_t max_operand_count = argument_count;
	uint32_t i;
	void **list = op_factories, *op_factory;
	ALLOCA_FLAG(use_heap);

	if(object->type == QB_OPERAND_NONE) {
		ifunc = qb_find_intrinsic_function(cxt, name->constant);
	}
	if(ifunc) {
		if(ifunc->argument_count_max != INVALID_INDEX &&  ifunc->argument_count_max > max_operand_count) {
			max_operand_count = ifunc->argument_count_max;
		}
	} else {
		USE_TSRM
		zval *class_name = NULL;
		if(object->type == QB_OPERAND_ZEND_CLASS || object->type == QB_OPERAND_ZEND_STATIC_CLASS) {
			zend_class_entry *ce;
			if(object->type == QB_OPERAND_ZEND_STATIC_CLASS) {
				// static:: qualifier--use the base class for now 
				// since we need to know whether it's going to be a zend call or not
				ce = cxt->zend_op_array->scope;
			} else {
				ce = object->zend_class;
			}
			if(ce) {
				class_name = qb_string_to_zval(ce->name, ce->name_length TSRMLS_CC);
			}
		} else if(object->type == QB_OPERAND_ZVAL) {
			class_name = object->constant;
		}
		zfunc = qb_find_zend_function(class_name, name->constant TSRMLS_CC);
		if(zfunc) {
			qfunc = qb_find_compiled_function(zfunc);
		} else {
			qb_abort("Missing function");
		}
	}

	// copy the arguments
	arguments = do_alloca(sizeof(qb_operand *) * max_operand_count, use_heap);
	for(i = 0; i < max_operand_count; i++) {
		if(i < argument_count) {
			arguments[i] = *stack_pointer[i];
		} else {
			// set unused arguments to none to simplify things
			arguments[i].type = QB_OPERAND_NONE;
			arguments[i].generic_pointer = NULL;
		}
	}

	if(ifunc) {
		op_factory = list[QB_OP_FACTORY_INTRINSIC];
		func_operands[0].intrinsic_function = ifunc;
		func_operands[0].type = QB_OPERAND_INTRINSIC_FUNCTION;
	} else {
		if(qfunc) {
			int32_t use_inlining = FALSE;
			if(cxt->compiler_context->stage == QB_STAGE_OPCODE_TRANSLATION) {
				USE_TSRM
				qb_compiler_context *other_compiler_cxt = qb_find_compiler_context(QB_G(build_context), qfunc);
				if(other_compiler_cxt) {
					if(other_compiler_cxt->function_flags & QB_FUNCTION_INLINEABLE) {
						if(!(other_compiler_cxt->function_flags & QB_FUNCTION_NEVER_INLINE)) {
							use_inlining = TRUE;
						}
					} else {
						if(other_compiler_cxt->function_flags & QB_FUNCTION_INLINE_ALWAYS) {
							// TODO: throw a warning
						}
					}
				}
			}
			if(use_inlining) {
				op_factory = list[QB_OP_FACTORY_INLINE];
			} else {
				op_factory = list[QB_OP_FACTORY_NORMAL];
				external_call = TRUE;
			}
		} else {
			op_factory = list[QB_OP_FACTORY_ZEND];
			external_call = TRUE;
		}
		func_operands[0].zend_function = zfunc;
		func_operands[0].type = (object->type == QB_OPERAND_ZEND_STATIC_CLASS) ? QB_OPERAND_STATIC_ZEND_FUNCTION : QB_OPERAND_ZEND_FUNCTION;
	}
	func_operands[1].arguments = arguments;
	func_operands[1].type = QB_OPERAND_ARGUMENTS;
	func_operands[2].number = argument_count;
	func_operands[2].type = QB_OPERAND_NUMBER;

	if(cxt->compiler_context->stage == QB_STAGE_RESULT_TYPE_RESOLUTION) {
		if(qfunc && cxt->compiler_context->dependencies) {
			USE_TSRM
			// note that this function is dependent on the target function
			qb_compiler_context *other_compiler_cxt = qb_find_compiler_context(QB_G(build_context), qfunc);
			if(other_compiler_cxt) {
				cxt->compiler_context->dependencies[other_compiler_cxt->dependency_index] = TRUE;
			}
		}
	} else if(cxt->compiler_context->stage == QB_STAGE_OPCODE_TRANSLATION) {
		if(external_call) {
			// don't inline functions that make calls
			cxt->compiler_context->function_flags &= ~QB_FUNCTION_INLINEABLE;
		}
	}

	qb_produce_op(cxt->compiler_context, op_factory, func_operands, 3, result, NULL, 0, result_prototype);
	free_alloca(arguments, use_heap);
}

static void qb_process_function_call(qb_php_translator_context *cxt, void *op_factories, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_result_prototype *result_prototype) {
	qb_operand *name = &operands[0], *object = &operands[1];
	uint32_t argument_count = cxt->zend_op->extended_value;
	qb_operand **stack_pointer = qb_pop_stack_items(cxt, argument_count);
	qb_process_function_call_ex(cxt, op_factories, name, object, stack_pointer, argument_count, result, result_prototype);
}

static void qb_process_init_method_call(qb_php_translator_context *cxt, void *op_factory, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_result_prototype *result_prototype) {
	qb_operand *object = &operands[0], *name = &operands[1], *stack_item;
	stack_item = qb_push_stack_item(cxt);	// function name
	*stack_item = *name;

	stack_item = qb_push_stack_item(cxt);	// object
#if ZEND_ENGINE_2_2 || ZEND_ENGINE_2_1
	if(object->type == QB_OPERAND_ZVAL) {
		qb_retrieve_operand(cxt, Z_OPERAND_TMP_VAR, &cxt->zend_op->op1, object);
	}
#endif
	if(object->type == QB_OPERAND_NONE) {
		stack_item->zend_class = cxt->zend_op_array->scope;
		stack_item->type = QB_OPERAND_ZEND_CLASS;
	} else {
		*stack_item = *object;
	}
}

static void qb_process_init_function_call(qb_php_translator_context *cxt, void *op_factory, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_result_prototype *result_prototype) {
	qb_operand *object = &operands[0], *name = &operands[1], *stack_item;
	stack_item = qb_push_stack_item(cxt);	// function name
	*stack_item = *name;

	stack_item = qb_push_stack_item(cxt);	// object
#if !ZEND_ENGINE_2_3
	*stack_item = *object;
#else
	// for some reason in Zend Engine 2.3, the name of the function would show up here
	if(object->type == QB_OPERAND_ZVAL && object->constant->type == IS_OBJECT) {
		*stack_item = *object;
	} else {
		stack_item->type = QB_OPERAND_NONE;
		stack_item->address = NULL;
	}
#endif
}

static void qb_process_function_call_by_name(qb_php_translator_context *cxt, void *op_factories, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_result_prototype *result_prototype) {
	uint32_t argument_count = cxt->zend_op->extended_value;
	qb_operand **stack_pointer = qb_pop_stack_items(cxt, argument_count + 2);
	qb_operand *name = stack_pointer[0];
	qb_operand *object = stack_pointer[1];
	qb_process_function_call_ex(cxt, op_factories, name, object, stack_pointer + 2, argument_count, result, result_prototype);
}

static void qb_process_send_argument(qb_php_translator_context *cxt, void *op_factory, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_result_prototype *result_prototype) {
	qb_operand *argument = &operands[0];
	qb_operand *stack_item = qb_push_stack_item(cxt);
	*stack_item = *argument;
}

static void qb_process_receive_argument(qb_php_translator_context *cxt, void *op_factory, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_result_prototype *result_prototype) {
	qb_operand *default_value = &operands[1];
	uint32_t argument_number;

#if !ZEND_ENGINE_2_3 && !ZEND_ENGINE_2_2 && !ZEND_ENGINE_2_1
	argument_number = Z_OPERAND_INFO(cxt->zend_op->op1, num);
#else
	argument_number = Z_LVAL_P(Z_OPERAND_ZV(cxt->zend_op->op1));
#endif
	
	if(default_value->type == QB_OPERAND_ZVAL) {
		qb_variable *qvar = cxt->compiler_context->variables[argument_number - 1];
		qvar->default_value = default_value->constant;
	}
}

static void qb_process_return(qb_php_translator_context *cxt, void *op_factory, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_result_prototype *result_prototype) {
	qb_produce_op(cxt->compiler_context, op_factory, operands, operand_count, result, NULL, 0, result_prototype);

	cxt->next_op_index1 = INVALID_INDEX;
}

static void qb_process_exit(qb_php_translator_context *cxt, void *op_factory, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_result_prototype *result_prototype) {
	qb_produce_op(cxt->compiler_context, op_factory, operands, operand_count, result, NULL, 0, result_prototype);

	cxt->next_op_index1 = INVALID_INDEX;
}

static void qb_process_jump(qb_php_translator_context *cxt, void *op_factory, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_result_prototype *result_prototype) {
	zend_op *target_op = Z_OPERAND_INFO(cxt->zend_op->op1, jmp_addr);
	uint32_t target_indices[1];

	if(cxt->compiler_context->stage == QB_STAGE_OPCODE_TRANSLATION) {
		// no inlining when a jump occurs
		cxt->compiler_context->function_flags &= ~QB_FUNCTION_INLINEABLE;
	}

	cxt->next_op_index1 = ZEND_OP_INDEX(target_op);

	target_indices[0] = JUMP_TARGET_INDEX(cxt->next_op_index1, 0);
	qb_produce_op(cxt->compiler_context, op_factory, operands, operand_count, result, target_indices, 1, result_prototype);
}

static void qb_process_jump_set(qb_php_translator_context *cxt, void *op_factory, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_result_prototype *result_prototype) {
	zend_op *target_op1 = Z_OPERAND_INFO(cxt->zend_op->op2, jmp_addr);
	uint32_t target_indices[2];

	if(cxt->compiler_context->stage == QB_STAGE_OPCODE_TRANSLATION) {
		// no inlining when a jump occurs
		cxt->compiler_context->function_flags &= ~QB_FUNCTION_INLINEABLE;
	}

	// process the zend ops beyond the ternary expression first
	// before we process the false clause
	cxt->next_op_index1 = ZEND_OP_INDEX(target_op1);
	cxt->next_op_index2 = cxt->zend_op_index + 1;

	// this might seem slightly weird
	// basically, if the value is true, we go to the second op created from the translation 
	// of this op--that is, the op following the IF_T, which assign the comparison value to the 
	// if the value is false, we go to the op created through the translation of the next zend op
	// that is, the second index points to the expression on the right side of ?:
	target_indices[0] = JUMP_TARGET_INDEX(cxt->zend_op_index, 1);
	target_indices[1] = JUMP_TARGET_INDEX(cxt->next_op_index2, 0);
	qb_produce_op(cxt->compiler_context, op_factory, operands, operand_count, result, target_indices, 2, result_prototype);
}

static void qb_process_branch(qb_php_translator_context *cxt, void *op_factory, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_result_prototype *result_prototype) {
	qb_operand *condition = &operands[0];
	zend_op *target_op1 = Z_OPERAND_INFO(cxt->zend_op->op2, jmp_addr);
	uint32_t target_indices[2];

	// start down the next instruction first before going down the branch
	cxt->next_op_index1 = ZEND_OP_INDEX(target_op1);
	cxt->next_op_index2 = cxt->zend_op_index + 1;

	// coerce the condition to boolean here so we can check whether a branch can be eliminated
	if(cxt->compiler_context->stage == QB_STAGE_CONSTANT_EXPRESSION_EVALUATION) {
		qb_perform_boolean_coercion(cxt->compiler_context, condition);
	}

	if(cxt->compiler_context->stage == QB_STAGE_OPCODE_TRANSLATION) {
		// no inlining when there's branching
		cxt->compiler_context->function_flags &= ~QB_FUNCTION_INLINEABLE;
	}

	if(condition->type == QB_OPERAND_ADDRESS && CONSTANT(condition->address)) {
		// the condition is constant--eliminate the branch
		int32_t is_true = VALUE_IN(cxt->compiler_context->storage, I32, condition->address);
		if((is_true && op_factory == &factory_branch_on_true) || (!is_true && op_factory == &factory_branch_on_false)) {
			// the branch always occurs--jump to it
			target_indices[0] = JUMP_TARGET_INDEX(cxt->next_op_index1, 0);
			qb_produce_op(cxt->compiler_context, &factory_jump, NULL, 0, result, target_indices, 1, result_prototype);
			cxt->next_op_index2 = 0;
		} else {
			// just go to the next op
			cxt->next_op_index1 = 0;
		}
	} else {
		target_indices[0] = JUMP_TARGET_INDEX(cxt->next_op_index1, 0);
		target_indices[1] = JUMP_TARGET_INDEX(cxt->next_op_index2, 0);
		qb_produce_op(cxt->compiler_context, op_factory, operands, operand_count, result, target_indices, 2, result_prototype);
	}
}

static void qb_process_for_loop(qb_php_translator_context *cxt, void *op_factory, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_result_prototype *result_prototype) {
	qb_operand *condition = &operands[0];
	uint32_t target_indices[2];
	
	if(cxt->compiler_context->stage == QB_STAGE_OPCODE_TRANSLATION) {
		// no inlining when there's a loop
		cxt->compiler_context->function_flags &= ~QB_FUNCTION_INLINEABLE;
	}

	cxt->next_op_index1 = cxt->zend_op->extended_value;
	cxt->next_op_index2 = Z_OPERAND_INFO(cxt->zend_op->op2, opline_num);

	target_indices[0] = JUMP_TARGET_INDEX(cxt->next_op_index1, 0);
	target_indices[1] = JUMP_TARGET_INDEX(cxt->next_op_index2, 0);
	qb_produce_op(cxt->compiler_context, op_factory, operands, operand_count, result, target_indices, 2, result_prototype);
}

static zend_brk_cont_element * qb_find_break_continue_element(qb_php_translator_context *cxt, int32_t nest_levels, int32_t array_offset) {
	zend_brk_cont_element *brk_cont_array = cxt->zend_op_array->brk_cont_array, *jmp_to;
	int32_t i;
	for(i = 0; i < nest_levels; i++) {
		if(array_offset == -1) {
			qb_abort("Cannot break/continue %d level%s", nest_levels, (nest_levels == 1) ? "" : "s");
		}
		jmp_to = &brk_cont_array[array_offset];
		array_offset = jmp_to->parent;
	}
	return jmp_to;
}


static void qb_process_break(qb_php_translator_context *cxt, void *op_factory, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_result_prototype *result_prototype) {
	zval *nest_level = Z_OPERAND_ZV(cxt->zend_op->op2);
	zend_brk_cont_element *jmp_to = qb_find_break_continue_element(cxt, Z_LVAL_P(nest_level), Z_OPERAND_INFO(cxt->zend_op->op1, opline_num));
	uint32_t target_indices[1];

	if(cxt->compiler_context->stage == QB_STAGE_OPCODE_TRANSLATION) {
		// no inlining when there's jumping
		cxt->compiler_context->function_flags &= ~QB_FUNCTION_INLINEABLE;
	}

	cxt->next_op_index1 = jmp_to->brk;

	target_indices[0] = JUMP_TARGET_INDEX(cxt->next_op_index1, 0);
	qb_produce_op(cxt->compiler_context, op_factory, operands, operand_count, result, target_indices, 1, result_prototype);
}

static void qb_process_continue(qb_php_translator_context *cxt, void *op_factory, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_result_prototype *result_prototype) {
	zval *nest_level = Z_OPERAND_ZV(cxt->zend_op->op2);
	zend_brk_cont_element *jmp_to = qb_find_break_continue_element(cxt, Z_LVAL_P(nest_level), Z_OPERAND_INFO(cxt->zend_op->op1, opline_num));
	uint32_t target_indices[1];

	if(cxt->compiler_context->stage == QB_STAGE_OPCODE_TRANSLATION) {
		// no inlining when there's jumping
		cxt->compiler_context->function_flags &= ~QB_FUNCTION_INLINEABLE;
	}

	cxt->next_op_index1 = jmp_to->cont;

	target_indices[0] = JUMP_TARGET_INDEX(cxt->next_op_index1, 0);
	qb_produce_op(cxt->compiler_context, op_factory, operands, operand_count, result, target_indices, 1, result_prototype);
}

static void qb_process_foreach_fetch(qb_php_translator_context *cxt, void *op_factory, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_result_prototype *result_prototype) {
	qb_operand *container = &operands[0];
	uint32_t target_indices[2];

	if(cxt->compiler_context->stage == QB_STAGE_OPCODE_TRANSLATION) {
		// no inlining when there's a loop
		cxt->compiler_context->function_flags &= ~QB_FUNCTION_INLINEABLE;
	}

	cxt->next_op_index1 = cxt->zend_op_index + 2;
	cxt->next_op_index2 = Z_OPERAND_INFO(cxt->zend_op->op2, opline_num);

	if(cxt->zend_op->extended_value & ZEND_FE_FETCH_BYREF) {
		qb_abort("reference is currently not supported");
	}

	target_indices[0] = JUMP_TARGET_INDEX(cxt->next_op_index1, 0);
	target_indices[1] = JUMP_TARGET_INDEX(cxt->next_op_index2, 0);
	qb_produce_op(cxt->compiler_context, op_factory, operands, operand_count, result, target_indices, 2, result_prototype);
}

static void qb_process_user_opcode(qb_php_translator_context *cxt, void *op_factory, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_result_prototype *result_prototype) {
}

static void qb_process_extension_op(qb_php_translator_context *cxt, void *op_factory, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_result_prototype *result_prototype) {
}

static qb_php_op_translator op_translators[] = {
	{	qb_process_basic_op,				&factory_nop								},	// ZEND_NOP
	{	qb_process_basic_op,				&factory_add								},	// ZEND_ADD
	{	qb_process_basic_op,				&factory_subtract							},	// ZEND_SUB
	{	qb_process_basic_op,				&factory_multiply							},	// ZEND_MUL
	{	qb_process_basic_op,				&factory_divide								},	// ZEND_DIV
	{	qb_process_basic_op,				&factory_modulo								},	// ZEND_MOD
	{	qb_process_basic_op,				&factory_shift_left							},	// ZEND_SL
	{	qb_process_basic_op,				&factory_shift_right						},	// ZEND_SR
	{	qb_process_basic_op,				&factory_concat								},	// ZEND_CONCAT
	{	qb_process_basic_op,				&factory_bitwise_or							},	// ZEND_BW_OR
	{	qb_process_basic_op,				&factory_bitwise_and						},	// ZEND_BW_AND
	{	qb_process_basic_op,				&factory_bitwise_xor						},	// ZEND_BW_XOR
	{	qb_process_basic_op,				&factory_bitwise_not						},	// ZEND_BW_NOT
	{	qb_process_basic_op,				&factory_logical_not						},	// ZEND_BOOL_NOT
	{	qb_process_basic_op,				&factory_logical_xor						},	// ZEND_BOOL_XOR
	{	qb_process_basic_op,				&factory_identical							},	// ZEND_IS_IDENTICAL
	{	qb_process_basic_op,				&factory_not_identical						},	// ZEND_IS_NOT_IDENTICAL
	{	qb_process_basic_op,				&factory_equal								},	// ZEND_IS_EQUAL
	{	qb_process_basic_op,				&factory_not_equal							},	// ZEND_IS_NOT_EQUAL
	{	qb_process_basic_op,				&factory_less_than							},	// ZEND_IS_SMALLER
	{	qb_process_basic_op,				&factory_less_equal							},	// ZEND_IS_SMALLER_OR_EQUAL
	{	qb_process_cast_op,					factories_cast								},	// ZEND_CAST
	{	qb_process_basic_op,				&factory_assign_branching					},	// ZEND_QM_ASSIGN
	{	qb_process_combo_op,				factories_add_assign						},	// ZEND_ASSIGN_ADD
	{	qb_process_combo_op,				factories_subtract_assign					},	// ZEND_ASSIGN_SUB
	{	qb_process_combo_op,				factories_multiply_assign					},	// ZEND_ASSIGN_MUL
	{	qb_process_combo_op,				factories_divide_assign						},	// ZEND_ASSIGN_DIV
	{	qb_process_combo_op,				factories_modulo_assign						},	// ZEND_ASSIGN_MOD
	{	qb_process_combo_op,				factories_shift_left_assign					},	// ZEND_ASSIGN_SL
	{	qb_process_combo_op,				factories_shift_right_assign				},	// ZEND_ASSIGN_SR
	{	qb_process_basic_op,				&factory_concat_assign						},	// ZEND_ASSIGN_CONCAT
	{	qb_process_combo_op,				factories_bitwise_or_assign					},	// ZEND_ASSIGN_BW_OR
	{	qb_process_combo_op,				factories_bitwise_and_assign				},	// ZEND_ASSIGN_BW_AND
	{	qb_process_combo_op,				factories_bitwise_xor_assign				},	// ZEND_ASSIGN_BW_XOR
	{	qb_process_basic_op,				&factory_increment_pre						},	// ZEND_PRE_INC
	{	qb_process_basic_op,				&factory_decrement_pre						},	// ZEND_PRE_DEC
	{	qb_process_basic_op,				&factory_increment_post						},	// ZEND_POST_INC
	{	qb_process_basic_op,				&factory_decrement_post						},	// ZEND_POST_DEC
	{	qb_process_basic_op,				&factory_assign								},	// ZEND_ASSIGN
	{	qb_process_basic_op,				&factory_assign_ref							},	// ZEND_ASSIGN_REF
	{	qb_process_basic_op,				&factory_echo								},	// ZEND_ECHO
	{	qb_process_basic_op,				&factory_print								},	// ZEND_PRINT
	{	qb_process_jump,					&factory_jump								},	// ZEND_JMP
	{	qb_process_branch,					&factory_branch_on_false					},	// ZEND_JMPZ
	{	qb_process_branch,					&factory_branch_on_true						},	// ZEND_JMPNZ
	{	qb_process_for_loop,				&factory_branch_on_true						},	// ZEND_JMPZNZ
	{	qb_process_branch,					&factory_branch_on_false					},	// ZEND_JMPZ_EX
	{	qb_process_branch,					&factory_branch_on_true						},	// ZEND_JMPNZ_EX
	{	qb_process_basic_op,				&factory_case								},	// ZEND_CASE
	{	qb_process_basic_op,				&factory_free								},	// ZEND_SWITCH_FREE
	{	qb_process_break,					&factory_jump								},	// ZEND_BRK
	{	qb_process_continue,				&factory_jump								},	// ZEND_CONT
	{	qb_process_basic_op,				&factory_boolean_cast						},	// ZEND_BOOL
	{	qb_process_basic_op,				&factory_empty_string						},	// ZEND_INIT_STRING
	{	qb_process_basic_op,				&factory_add_string							},	// ZEND_ADD_CHAR
	{	qb_process_basic_op,				&factory_add_string							},	// ZEND_ADD_STRING
	{	qb_process_basic_op,				&factory_add_variable						},	// ZEND_ADD_VAR
	{	qb_process_basic_op,				NULL						},	// ZEND_BEGIN_SILENCE
	{	qb_process_basic_op,				NULL						},	// ZEND_END_SILENCE
	{	qb_process_init_function_call,		NULL										},	// ZEND_INIT_FCALL_BY_NAME
	{	qb_process_function_call,			factories_fcall								},	// ZEND_DO_FCALL
	{	qb_process_function_call_by_name,	factories_fcall								},	// ZEND_DO_FCALL_BY_NAME
	{	qb_process_return,					&factory_return								},	// ZEND_RETURN
	{	qb_process_receive_argument,		NULL										},	// ZEND_RECV
	{	qb_process_receive_argument,		NULL										},	// ZEND_RECV_INIT
	{	qb_process_send_argument,			NULL										},	// ZEND_SEND_VAL
	{	qb_process_send_argument,			NULL										},	// ZEND_SEND_VAR
	{	qb_process_send_argument,			NULL										},	// ZEND_SEND_REF
	{	NULL,								NULL										},	// ZEND_NEW
	{	NULL,								NULL										},	// ZEND_INIT_NS_FCALL_BY_NAME
	{	qb_process_basic_op,				&factory_free								},	// ZEND_FREE
	{	qb_process_basic_op,				&factory_array_init							},	// ZEND_INIT_ARRAY
	{	qb_process_basic_op,				&factory_array_append						},	// ZEND_ADD_ARRAY_ELEMENT
	{	NULL,								NULL										},	// ZEND_INCLUDE_OR_EVAL
	{	qb_process_basic_op,				&factory_unset								},	// ZEND_UNSET_VAR
	{	qb_process_basic_op,				&factory_unset_array_element				},	// ZEND_UNSET_DIM
	{	qb_process_basic_op,				&factory_unset_object_property				},	// ZEND_UNSET_OBJ
	{	qb_process_basic_op,				&factory_foreach_reset						},	// ZEND_FE_RESET
	{	qb_process_foreach_fetch,			&factory_foreach_fetch						},	// ZEND_FE_FETCH
	{	qb_process_exit,					&factory_exit								},	// ZEND_EXIT
	{	qb_process_fetch,					factories_fetch_variable					},	// ZEND_FETCH_R
	{	qb_process_basic_op,				&factory_fetch_array_element_read			},	// ZEND_FETCH_DIM_R
	{	qb_process_basic_op,				&factory_fetch_object_property_read			},	// ZEND_FETCH_OBJ_R
	{	qb_process_fetch,					factories_fetch_variable					},	// ZEND_FETCH_W
	{	qb_process_basic_op,				&factory_fetch_array_element_write			},	// ZEND_FETCH_DIM_W
	{	qb_process_basic_op,				&factory_fetch_object_property_write		},	// ZEND_FETCH_OBJ_W
	{	qb_process_fetch,					factories_fetch_variable					},	// ZEND_FETCH_RW
	{	qb_process_basic_op,				&factory_fetch_array_element_write			},	// ZEND_FETCH_DIM_RW
	{	qb_process_basic_op,				&factory_fetch_object_property_write		},	// ZEND_FETCH_OBJ_RW
	{	qb_process_fetch,					factories_fetch_variable					},	// ZEND_FETCH_IS
	{	qb_process_basic_op,				&factory_fetch_array_element_isset			},	// ZEND_FETCH_DIM_IS
	{	qb_process_basic_op,				&factory_fetch_object_property_isset		},	// ZEND_FETCH_OBJ_IS
	{	qb_process_fetch,					factories_fetch_variable					},	// ZEND_FETCH_FUNC_ARG

	// TODO: fix this so the correct factory is used when the function accepts reference
	{	qb_process_basic_op,				&factory_fetch_array_element_read			},	// ZEND_FETCH_DIM_FUNC_ARG
	{	qb_process_basic_op,				&factory_fetch_object_property_read			},	// ZEND_FETCH_OBJ_FUNC_ARG

	{	qb_process_fetch,					factories_fetch_variable					},	// ZEND_FETCH_UNSET
	{	qb_process_basic_op,				&factory_fetch_array_element_isset			},	// ZEND_FETCH_DIM_UNSET
	{	qb_process_basic_op,				&factory_fetch_object_property_isset		},	// ZEND_FETCH_OBJ_UNSET
	{	qb_process_basic_op,				&factory_fetch_array_element_read			},	// ZEND_FETCH_DIM_TMP_VAR
	{	qb_process_basic_op,				&factory_fetch_constant						},	// ZEND_FETCH_CONSTANT
	{	NULL,								NULL										},	// ZEND_GOTO
	{	qb_process_extension_op,			&factory_ext				},	// ZEND_EXT_STMT
	{	qb_process_extension_op,			&factory_ext				},	// ZEND_EXT_FCALL_BEGIN
	{	qb_process_extension_op,			&factory_ext				},	// ZEND_EXT_FCALL_END
	{	qb_process_extension_op,			&factory_ext				},	// ZEND_EXT_NOP
	{	qb_process_basic_op,				&factory_nop								},	// ZEND_TICKS
	{	qb_process_send_argument,			NULL										},	// ZEND_SEND_VAR_NO_REF
	{	NULL,								NULL										},	// ZEND_CATCH
	{	NULL,								NULL										},	// ZEND_THROW
	{	qb_process_fetch_class,				factories_fetch_class						},	// ZEND_FETCH_CLASS
	{	NULL,								NULL										},	// ZEND_CLONE
	{	NULL,								NULL										},	// ZEND_RETURN_BY_REF
	{	qb_process_init_method_call,		NULL										},	// ZEND_INIT_METHOD_CALL
	{	qb_process_init_method_call,		NULL										},	// ZEND_INIT_STATIC_METHOD_CALL
	{	qb_process_basic_op,				&factory_boolean_cast						},	// ZEND_ISSET_ISEMPTY_VAR
	{	qb_process_basic_op,				&factory_array_element_isset				},	// ZEND_ISSET_ISEMPTY_DIM_OBJ
	{	NULL,								NULL										},	// 116
	{	NULL,								NULL										},	// 117
	{	NULL,								NULL										},	// 118
	{	NULL,								NULL										},	// 119
	{	NULL,								NULL										},	// 120
	{	NULL,								NULL										},	// 121
	{	NULL,								NULL										},	// 122
	{	NULL,								NULL										},	// 123
	{	NULL,								NULL										},	// 124
	{	NULL,								NULL										},	// 125
	{	NULL,								NULL										},	// 126
	{	NULL,								NULL										},	// 127
	{	NULL,								NULL										},	// 128
	{	NULL,								NULL										},	// 129
	{	NULL,								NULL										},	// 130
	{	NULL,								NULL										},	// 131
	{	qb_process_basic_op,				&factory_increment_object_property_pre		},	// ZEND_PRE_INC_OBJ
	{	qb_process_basic_op,				&factory_decrement_object_property_pre		},	// ZEND_PRE_DEC_OBJ
	{	qb_process_basic_op,				&factory_increment_object_property_post		},	// ZEND_POST_INC_OBJ
	{	qb_process_basic_op,				&factory_decrement_object_property_post		},	// ZEND_POST_DEC_OBJ
	{	qb_process_basic_op,				&factory_assign_object_property				},	// ZEND_ASSIGN_OBJ
	{	NULL,								NULL										},	// ZEND_OP_DATA
	{	NULL,								NULL										},	// ZEND_INSTANCEOF
	{	NULL,								NULL										},	// ZEND_DECLARE_CLASS
	{	NULL,								NULL										},	// ZEND_DECLARE_INHERITED_CLASS
	{	NULL,								NULL										},	// ZEND_DECLARE_FUNCTION
	{	NULL,								NULL										},	// ZEND_RAISE_ABSTRACT_ERROR
	{	NULL,								NULL										},	// ZEND_DECLARE_CONST
	{	NULL,								NULL										},	// ZEND_ADD_INTERFACE
	{	NULL,								NULL										},	// ZEND_DECLARE_INHERITED_CLASS_DELAYED
	{	NULL,								NULL										},	// ZEND_VERIFY_ABSTRACT_CLASS
	{	qb_process_basic_op,				&factory_assign_array_element				},	// ZEND_ASSIGN_DIM
	{	qb_process_basic_op,				&factory_object_property_isset				},	// ZEND_ISSET_ISEMPTY_PROP_OBJ
	{	NULL,								NULL										},	// ZEND_HANDLE_EXCEPTION
	{	qb_process_user_opcode,				NULL						},	// ZEND_USER_OPCODE
	{	NULL,								NULL										},	// 151
	{	qb_process_jump_set,				NULL						},	// ZEND_JMP_SET
	{	NULL,								NULL										},	// ZEND_DECLARE_LAMBDA_FUNCTION
	{	NULL,								NULL										},	// ZEND_ADD_TRAIT
	{	NULL,								NULL										},	// ZEND_BIND_TRAITS
	{	NULL,								NULL										},	// ZEND_SEPARATE
	{	qb_process_basic_op,				&factory_assign_branching					},	// ZEND_QM_ASSIGN_VAR
	{	qb_process_jump_set,				&factory_branch_on_true_set					},	// ZEND_JMP_SET_VAR
	{	NULL,								NULL,										},	// ZEND_DISCARD_EXCEPTION
	{	qb_process_basic_op,				&factory_yield,								},	// ZEND_YIELD
	{	qb_process_basic_op,				&factory_leave,								},	// ZEND_GENERATOR_RETURN
	{	NULL,								NULL,										},	// ZEND_FAST_CALL
	{	NULL,								NULL,										},	// ZEND_FAST_RET
};

static void qb_process_current_instruction(qb_php_translator_context *cxt) {
	if(cxt->zend_op->opcode != ZEND_OP_DATA && cxt->zend_op->opcode != qb_user_opcode) {
		qb_operand operands[3], results[2];
		qb_result_prototype *result_prototype = &cxt->result_prototypes[cxt->zend_op_index];
		qb_php_op_translator *t;
		uint32_t operand_count = 0;
		int32_t result_count = RETURN_VALUE_USED(cxt->zend_op);
		uint32_t zend_opcode = cxt->zend_op->opcode;

		// retrieve operands 
		if(qb_retrieve_operand(cxt, Z_OPERAND_TYPE(cxt->zend_op->op1), &cxt->zend_op->op1, &operands[0])) {
			operand_count = 1;
		}
		if(qb_retrieve_operand(cxt, Z_OPERAND_TYPE(cxt->zend_op->op2), &cxt->zend_op->op2, &operands[1])) {
			operand_count = 2;
		}

		// see whether the op returns a value
		if(result_count) {
			qb_retrieve_operand(cxt, Z_OPERAND_TYPE(cxt->zend_op->result), &cxt->zend_op->result, &results[0]);
		} else {
			results[0].type = QB_OPERAND_NONE;
			results[0].address = NULL;
		}

		if(cxt->zend_op[1].opcode == ZEND_OP_DATA) {
			// retrieve the extra data
			if(qb_retrieve_operand(cxt, Z_OPERAND_TYPE(cxt->zend_op[1].op1), &cxt->zend_op[1].op1, &operands[2])) {
				operand_count = 3;
			}
			if(qb_retrieve_operand(cxt, Z_OPERAND_TYPE(cxt->zend_op[1].result), &cxt->zend_op[1].result, &results[1])) {
				result_count = 2;
			}
		}

		// look up the translator for this opcode
		if(EXPECTED(zend_opcode < sizeof(op_translators) / sizeof(op_translators[0]))) {
			t = &op_translators[zend_opcode];
		} else {
			t = NULL;
		}
		if(t && t->translate) {
			t->translate(cxt, t->extra, operands, operand_count, &results[0], result_prototype);

#ifdef ZEND_DEBUG
			result_prototype->extra = t->extra;
#endif

			if(operand_count >= 1) {
				qb_retire_operand(cxt, Z_OPERAND_TYPE(cxt->zend_op->op1), &cxt->zend_op->op1, &operands[0]);
				if(operand_count >= 2) {
					qb_retire_operand(cxt, Z_OPERAND_TYPE(cxt->zend_op->op2), &cxt->zend_op->op2, &operands[1]);
					if(operand_count >= 3) {
						qb_retire_operand(cxt, Z_OPERAND_TYPE(cxt->zend_op[1].op1), &cxt->zend_op[1].op1, &operands[0]);
					}
				}
			}
			if(result_count >= 1) {
				qb_retire_operand(cxt, Z_OPERAND_TYPE(cxt->zend_op->result), &cxt->zend_op->result, &results[0]);
				if(result_count >= 2) {
					qb_retire_operand(cxt, Z_OPERAND_TYPE(cxt->zend_op[1].result), &cxt->zend_op[1].result, &results[1]);
				}
			}

			// lock operands kept as temporary variables
			qb_lock_temporary_variables(cxt);
		} else {
			qb_abort("Unsupported language feature");
		}
	}
}

static qb_intrinsic_function intrinsic_functions[] = {
	{	0,	"count",				1,		2,		&factory_fetch_array_size	},
	{	0,	"sizeof",				1,		2,		&factory_fetch_array_size	},
	{	0,	"strlen",				1,		2,		&factory_fetch_array_size	},
	{	0,	"int8",					1,		1,		&factory_cast_S08			},
	{	0,	"uint8",				1,		1,		&factory_cast_U08			},
	{	0,	"int16",				1,		1,		&factory_cast_S16			},
	{	0,	"uint16",				1,		1,		&factory_cast_U16			},
	{	0,	"int32",				1,		1,		&factory_cast_S32			},
	{	0,	"uint32",				1,		1,		&factory_cast_U32			},
	{	0,	"int64",				1,		2,		&factory_cast_S64			},
	{	0,	"uint64",				1,		2,		&factory_cast_U64			},
	{	0,	"float32",				1,		1,		&factory_cast_F32			},
	{	0,	"float64",				1,		1,		&factory_cast_F64			},
	{	0,	"defined",				1,		1,		&factory_defined			},
	{	0,	"define",				2,		2,		&factory_define				},
	{	0,	"fork",					0,		1,		&factory_fork				},
	{	0,	"spoon",				0,		0,		&factory_spoon				},
	{	0,	"equal",				2,		2,		&factory_set_equal			},
	{	0,	"not_equal",			2,		2,		&factory_set_not_equal		},
	{	0,	"less_than",			2,		2,		&factory_set_less_than		},
	{	0,	"less_than_equal",		2,		2,		&factory_set_less_equal		},
	{	0,	"greater_than",			2,		2,		&factory_set_greater_than	},
	{	0,	"greater_than_equal",	2,		2,		&factory_set_greater_equal	},
	{	0,	"min",					1,		-1,		&factory_min				},
	{	0,	"max",					1,		-1,		&factory_max				},
	{	0,	"any",					1,		1,		&factory_any				},
	{	0,	"all",					1,		1,		&factory_all				},
	{	0,	"not",					1,		1,		&factory_set_not			},
	{	0,	"abs",					1,		1,		&factory_abs				},
	{	0,	"sin",					1,		1,		&factory_sin				},
	{	0,	"asin",					1,		1,		&factory_asin				},
	{	0,	"cos",					1,		1,		&factory_cos				},
	{	0,	"acos",					1,		1,		&factory_acos				},
	{	0,	"tan",					1,		1,		&factory_tan				},
	{	0,	"atan",					1,		1,		&factory_atan				},
	{	0,	"sinh",					1,		1,		&factory_sinh				},
	{	0,	"asinh",				1,		1,		&factory_asinh				},
	{	0,	"cosh",					1,		1,		&factory_cosh				},
	{	0,	"acosh",				1,		1,		&factory_acosh				},
	{	0,	"tanh",					1,		1,		&factory_tanh				},
	{	0,	"atanh",				1,		1,		&factory_atanh				},
	{	0,	"ceil",					1,		1,		&factory_ceil				},
	{	0,	"floor",				1,		1,		&factory_floor				},
	{	0,	"round",				1,		3,		&factory_round				},
	{	0,	"log",					1,		1,		&factory_log				},
	{	0,	"log1p",				1,		1,		&factory_log1p				},
	{	0,	"log10",				1,		1,		&factory_log10				},
	{	0,	"log2",					1,		1,		&factory_log2				},
	{	0,	"exp",					1,		1,		&factory_exp				},
	{	0,	"exp2",					1,		1,		&factory_exp2				},
	{	0,	"expm1",				1,		1,		&factory_expm1				},
	{	0,	"sqrt",					1,		1,		&factory_sqrt				},
	{	0,	"pow",					2,		2,		&factory_pow				},
	{	0,	"fmod",					2,		2,		&factory_modulo				},
	{	0,	"mod",					2,		2,		&factory_floor_modulo		},
	{	0,	"lcg_value",			0,		0,		&factory_lcg				},
	{	0,	"is_finite",			1,		1,		&factory_is_finite			},
	{	0,	"is_infinite",			1,		1,		&factory_is_infinite		},
	{	0,	"is_nan",				1,		1,		&factory_is_nan				},
	{	0,	"rand",					0,		2,		&factory_rand				},
	{	0,	"mt_rand",				0,		2,		&factory_mt_rand			},
	{	0,	"hypot",				2,		2,		&factory_hypot				},
	{	0,	"deg2rad",				1,		1,		&factory_deg2rad			},
	{	0,	"rad2deg",				1,		1,		&factory_rad2deg			},
	{	0,	"sign",					1,		1,		&factory_sign				},
	{	0,	"rsqrt",				1,		1,		&factory_rsqrt				},
	{	0,	"fract",				1,		1,		&factory_fract				},
	{	0,	"step",					2,		2,		&factory_step				},
	{	0,	"clamp",				3,		3,		&factory_clamp				},
	{	0,	"mix",					3,		3,		&factory_mix				},
	{	0,	"smooth_step",			3,		3,		&factory_smooth_step		},
	{	0,	"normalize",			1,		1,		&factory_normalize			},
	{	0,	"length",				1,		1,		&factory_length				},
	{	0,	"distance",				2,		2,		&factory_distance			},
	{	0,	"dot",					2,		2,		&factory_dot_product		},
	{	0,	"cross",				2,		3,		&factory_cross_product		},
	{	0,	"faceforward",			2,		2,		&factory_faceforward		},
	{	0,	"reflect",				2,		2,		&factory_reflect			},
	{	0,	"refract",				3,		3,		&factory_refract			},
	{	0,	"mm_mult",				2,		2,		&factory_mm_mult			},
	{	0,	"mv_mult",				2,		2,		&factory_mv_mult			},
	{	0,	"vm_mult",				2,		2,		&factory_vm_mult			},
	{	0,	"transpose",			1,		1,		&factory_transpose			},
	{	0,	"det",					1,		1,		&factory_determinant		},
	{	0,	"inverse",				1,		1,		&factory_inverse			},
	{	0,	"mm_mult_cm",			2,		2,		&factory_mm_mult_cm			},
	{	0,	"mv_mult_cm",			2,		2,		&factory_mv_mult_cm			},
	{	0,	"vm_mult_cm",			2,		2,		&factory_vm_mult_cm			},
	{	0,	"transpose_cm",			1,		1,		&factory_transpose			},
	{	0,	"det_cm",				1,		1,		&factory_determinant		},
	{	0,	"inverse_cm",			1,		1,		&factory_inverse			},
	{	0,	"mm_mult_rm",			2,		2,		&factory_mm_mult_rm			},
	{	0,	"mv_mult_rm",			2,		2,		&factory_mv_mult_rm			},
	{	0,	"vm_mult_rm",			2,		2,		&factory_vm_mult_rm			},
	{	0,	"transpose_rm",			1,		1,		&factory_transpose			},
	{	0,	"det_rm",				1,		1,		&factory_determinant		},
	{	0,	"inverse_rm",			1,		1,		&factory_inverse			},
	{	0,	"transform_cm",			2,		2,		&factory_transform_cm		},
	{	0,	"transform_rm",			2,		2,		&factory_transform_rm		},
	{	0,	"transform",			2,		2,		&factory_transform			},
	{	0,	"sample_nearest",		3,		3,		&factory_sample_nearest		},
	{	0,	"sample_bilinear",		3,		3,		&factory_sample_bilinear	},
	{	0,	"blend",				2,		2,		&factory_alpha_blend		},

	{	0,	"array_column",			2,		2,		&factory_array_column		},
	{	0,	"array_diff",			2,		-1,		&factory_array_diff			},
	{	0,	"array_fill",			3,		3,		&factory_array_fill			},
/*
	{	0,	"array_filter",			1,		1,		&factory_array_diff			},
*/
	{	0,	"array_intersect",		2,		-1,		&factory_array_intersect	},
	{	0,	"array_merge",			2,		-1,		&factory_array_merge		},
	{	0,	"array_pad",			3,		3,		&factory_array_pad			},
	{	0,	"array_pop",			1,		1,		&factory_array_pop			},
	{	0,	"array_pos",			2,		3,		&factory_array_pos			},
	{	0,	"array_product",		1,		1,		&factory_array_product		},
	{	0,	"array_push",			2,		-1,		&factory_array_push			},
	{	0,	"array_rand",			1,		2,		&factory_array_rand			},
	{	0,	"array_resize",			2,		-1,		&factory_array_resize		},
	{	0,	"array_reverse",		1,		1,		&factory_array_reverse		},
	{	0,	"array_rpos",			2,		3,		&factory_array_rpos			},
	{	0,	"array_search",			2,		2,		&factory_array_search		},
	{	0,	"array_shift",			1,		1,		&factory_array_shift		},
	{	0,	"array_slice",			2,		3,		&factory_array_slice		},
	{	0,	"array_splice",			2,		4,		&factory_array_splice		},
	{	0,	"array_sum",			1,		1,		&factory_array_sum			},
	{	0,	"array_unique",			1,		1,		&factory_array_unique		},
	{	0,	"array_unshift",		2,		-1,		&factory_array_unshift		},
	{	0,	"in_array",				2,		2,		&factory_in_array			},
	{	0,	"range",				2,		3,		&factory_range				},
	{	0,	"rsort",				1,		1,		&factory_rsort				},
	{	0,	"shuffle",				1,		1,		&factory_shuffle			},
	{	0,	"sort",					1,		1,		&factory_sort				},
	{	0,	"substr",				2,		3,		&factory_substr				},
	{	0,	"strpos",				2,		3,		&factory_array_pos			},
	{	0,	"strrpos",				2,		3,		&factory_array_rpos			},
	{	0,	"pack_le",				1,		2,		&factory_pack_le			},
	{	0,	"pack_be",				1,		2,		&factory_pack_be			},
	{	0,	"unpack_le",			1,		3,		&factory_unpack_le			},
	{	0,	"unpack_be",			1,		3,		&factory_unpack_be			},
	{	0,	"utf8_decode",			1,		1,		&factory_utf8_decode		},
	{	0,	"utf8_encode",			1,		1,		&factory_utf8_encode		},
	{	0,	"cabs",					1,		1,		&factory_complex_abs		},
	{	0,	"carg",					1,		1,		&factory_complex_arg		},
	{	0,	"cmult",				2,		2,		&factory_complex_multiply	},
	{	0,	"cdiv",					2,		2,		&factory_complex_divide		},
	{	0,	"cexp",					1,		1,		&factory_complex_exp		},
	{	0,	"clog",					1,		1,		&factory_complex_log		},
	{	0,	"csqrt",				1,		1,		&factory_complex_sqrt		},
	{	0,	"cpow",					2,		2,		&factory_complex_pow		},
	{	0,	"csin",					1,		1,		&factory_complex_sin		},
	{	0,	"ccos",					1,		1,		&factory_complex_cos		},
	{	0,	"ctan",					1,		1,		&factory_complex_tan		},
	{	0,	"csinh",				1,		1,		&factory_complex_sinh		},
	{	0,	"ccosh",				1,		1,		&factory_complex_cosh		},
	{	0,	"ctanh",				1,		1,		&factory_complex_tanh		},
	{	0,	"rgb2hsv",				1,		1,		&factory_rgb2hsv			},
	{	0,	"hsv2rgb",				1,		1,		&factory_hsv2rgb			},
	{	0,	"rgb2hsl",				1,		1,		&factory_rgb2hsl			},
	{	0,	"hsl2rgb",				1,		1,		&factory_hsl2rgb			},
	{	0,	"rgb_premult",			1,		1,		&factory_apply_premult		},
	{	0,	"rgb_demult",			1,		1,		&factory_remove_premult		},

	// unsupported functions
	{	0,	"compact",				0,		-1,		NULL						},
	{	0,	"extract",				0,		-1,		NULL						},
	{	0,	"each",					0,		-1,		NULL						},
	{	0,	"pos"	,				0,		-1,		NULL						},
	{	0,	"key"	,				0,		-1,		NULL						},
	{	0,	"current",				0,		-1,		NULL						},
	{	0,	"prev",					0,		-1,		NULL						},
	{	0,	"next",					0,		-1,		NULL						},
	{	0,	"reset",				0,		-1,		NULL						},
	{	0,	"end",					0,		-1,		NULL						},
	{	0,	"array_map",			0,		-1,		NULL						},
	{	0,	"array_reduce",			0,		-1,		NULL						},
	{	0,	"array_walk",			0,		-1,		NULL						},
};

#define MAX_INLINE_FUNCTION_NAME_LEN		32

qb_intrinsic_function * qb_find_intrinsic_function(qb_php_translator_context *cxt, zval *callable) {
	const char *name = Z_STRVAL_P(callable);
	uint32_t len = Z_STRLEN_P(callable);

	if(len < MAX_INLINE_FUNCTION_NAME_LEN) {
		// make it lower case
		char name_buffer[MAX_INLINE_FUNCTION_NAME_LEN];
		uint32_t i;
		ulong hash_value;

		for(i = 0; i < len; i++) {
			name_buffer[i] = tolower(name[i]);
		}
		name_buffer[i] = '\0';

		// calculate the hash value and look up the function
		hash_value = zend_hash_func(name_buffer, len + 1);
		for(i = 0; i < sizeof(intrinsic_functions) / sizeof(qb_intrinsic_function); i++) {
			qb_intrinsic_function *f = &intrinsic_functions[i];
			if(f->hash_value == hash_value && strcmp(name_buffer, f->name) == 0) {
				return f;
			}
		}
	}
	return NULL;
}

static void qb_process_instruction_range(qb_php_translator_context *cxt, uint32_t start_index, uint32_t end_index) {
	uint32_t zend_op_index;

	// save states
	zend_op_index = cxt->zend_op_index;

	// process zend instructions until we reach the end 
	// or if an instruction is already translated
	cxt->zend_op_index = start_index;
	cxt->zend_op = ZEND_OP(start_index);
	for(;;) {
		if(cxt->op_processed[cxt->zend_op_index]) {
			// instruction has already been translated--do a jump there and exit
			qb_produce_op(cxt->compiler_context, &factory_jump, NULL, 0, NULL, &cxt->zend_op_index, 1, NULL);
			break;
		}

		if(cxt->compiler_context->stage == QB_STAGE_OPCODE_TRANSLATION) {
			// see if the next op is going to be a jump target
			if(qb_is_jump_target(cxt->compiler_context, cxt->zend_op_index)) {
				// the result of on-demand expressions might no longer be valid
				qb_invalidate_all_on_demand_expressions(cxt->compiler_context);
			}
		}

		// translate the current instruction
		qb_set_source_op_index(cxt->compiler_context, cxt->zend_op_index, cxt->zend_op->lineno);
		qb_process_current_instruction(cxt);
		cxt->op_processed[cxt->zend_op_index] = TRUE;

		// see if it was a branch or a jump
		if(cxt->next_op_index1 || cxt->next_op_index2) {
			uint32_t op_index1 = cxt->next_op_index1;
			uint32_t op_index2 = cxt->next_op_index2;
			cxt->next_op_index1 = 0;
			cxt->next_op_index2 = 0;
			if(op_index1 == INVALID_INDEX) {
				break;
			} else if(op_index2) {
				if(op_index1) {
					qb_process_instruction_range(cxt, op_index1, op_index2);
				}
				cxt->zend_op = ZEND_OP(op_index2);
				cxt->zend_op_index = op_index2;
			} else {
				cxt->zend_op = ZEND_OP(op_index1);
				cxt->zend_op_index = op_index1;
			}
		} else {
			cxt->zend_op++;
			cxt->zend_op_index++;
		}
		if(cxt->zend_op_index == end_index) {
			break;
		}
	}

	// restore the state
	cxt->zend_op = ZEND_OP(zend_op_index);
	cxt->zend_op_index = zend_op_index;
}

static void qb_clear_temporary_variables(qb_php_translator_context *cxt) {
	uint32_t i;
	for(i = 0; i < cxt->temp_variable_count; i++) {
		qb_temporary_variable *temp_variable = &cxt->temp_variables[i];
		temp_variable->operand.type = QB_OPERAND_EMPTY;
#ifdef ZEND_DEBUG
		temp_variable->freed = FALSE;
#endif
	}
}

static void qb_clear_result_prototypes(qb_php_translator_context *cxt) {
	uint32_t i;
	for(i = 0; i < cxt->result_prototype_count; i++) {
		qb_result_prototype *prototype = &cxt->result_prototypes[i];
		prototype->preliminary_type = QB_TYPE_UNKNOWN;
		prototype->final_type = QB_TYPE_UNKNOWN;
#ifdef ZEND_DEBUG
		prototype->zend_op = &cxt->compiler_context->zend_op_array->opcodes[i];
#endif
	}
}

static void qb_clear_completion_flags(qb_php_translator_context *cxt) {
	memset(cxt->op_processed, 0, cxt->zend_op_array->last);
}

static void qb_process_instructions(qb_php_translator_context *cxt) {
	qb_process_instruction_range(cxt, 0, cxt->zend_op_array->last);
}

void qb_survey_instructions(qb_php_translator_context *cxt) {
	// scan through the opcodes to determine the type of each expression
	cxt->compiler_context->stage = QB_STAGE_RESULT_TYPE_RESOLUTION;
	qb_clear_result_prototypes(cxt);
	qb_clear_temporary_variables(cxt);
	qb_clear_completion_flags(cxt);
	qb_process_instructions(cxt);

	// evaluate any constant expressions
	cxt->compiler_context->stage = QB_STAGE_CONSTANT_EXPRESSION_EVALUATION;
	qb_clear_temporary_variables(cxt);
	qb_clear_completion_flags(cxt);
	qb_process_instructions(cxt);
}

void qb_translate_instructions(qb_php_translator_context *cxt) {

	cxt->compiler_context->stage = QB_STAGE_OPCODE_TRANSLATION;
	qb_clear_temporary_variables(cxt);
	qb_clear_completion_flags(cxt);
	qb_process_instructions(cxt);

	// make sure there's always a RET at the end
	if(cxt->compiler_context->op_count == 0 || cxt->compiler_context->ops[cxt->compiler_context->op_count - 1]->opcode != QB_RET) {
		qb_produce_op(cxt->compiler_context, &factory_leave, NULL, 0, NULL, NULL, 0, NULL);
	}
}

void qb_initialize_php_translator_context(qb_php_translator_context *cxt, qb_compiler_context *compiler_cxt TSRMLS_DC) {
	static int hash_initialized = FALSE;
	if(!hash_initialized) {
		// calculate the hash of intrinsic function names for quicker look-ups
		uint32_t i;
		for(i = 0; i < sizeof(intrinsic_functions) / sizeof(qb_intrinsic_function); i++) {
			qb_intrinsic_function *f = &intrinsic_functions[i];
			f->hash_value = zend_hash_func(f->name, strlen(f->name) + 1);
		}
		hash_initialized = TRUE;
	}

	cxt->pool = compiler_cxt->pool;
	cxt->compiler_context = compiler_cxt;
	cxt->zend_op_array = compiler_cxt->zend_op_array;
	cxt->zend_op = NULL;
	cxt->zend_op_index = 0;
	cxt->next_op_index1 = 0;
	cxt->next_op_index2 = 0;
	cxt->silence = 0;
	cxt->foreach_index_address = NULL;
	cxt->stack_item_count = 0;
	cxt->stack_item_offset = 0;
	cxt->stack_item_buffer_size = 0;
	cxt->stack_items = NULL;

	if(cxt->zend_op_array->T > 0) {
		qb_attach_new_array(cxt->pool, (void **) &cxt->temp_variables, &cxt->temp_variable_count, sizeof(qb_temporary_variable), cxt->zend_op_array->T);
		qb_enlarge_array((void **) &cxt->temp_variables, cxt->zend_op_array->T);
	} else {
		cxt->temp_variables = NULL;
		cxt->temp_variable_count = 0;
	}
	qb_attach_new_array(cxt->pool, (void **) &cxt->result_prototypes, &cxt->result_prototype_count, sizeof(qb_result_prototype), cxt->zend_op_array->last);
	qb_enlarge_array((void **) &cxt->result_prototypes, cxt->zend_op_array->last);

	cxt->op_processed = emalloc(cxt->zend_op_array->last);

	SAVE_TSRMLS
}

void qb_free_php_translator_context(qb_php_translator_context *cxt) {
	if(cxt->stack_items) {
		efree(cxt->stack_items);
	}
	efree(cxt->op_processed);
}
