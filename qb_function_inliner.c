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

static void qb_add_substitution(qb_function_inliner_context *cxt, qb_address *callee_address, qb_address *caller_address) {
	qb_address_substitution *substitution = &cxt->substitutions[cxt->substitution_count++];
	substitution->original = callee_address;
	substitution->current = caller_address;
	if(caller_address) {
		if(!READ_ONLY(callee_address)) {
			qb_mark_as_writable(cxt->caller_context, caller_address);
		}
	}
}

static qb_address * qb_find_substitution_address(qb_function_inliner_context *cxt, qb_address *callee_address) {
	uint32_t i;
	for(i = 0; i < cxt->substitution_count; i++) {
		qb_address_substitution *substitution = &cxt->substitutions[i];
		if(substitution->original == callee_address) {
			return substitution->current;
		}
	}
	return NULL;
}

static qb_variable * qb_find_variable_with_address(qb_function_inliner_context *cxt, qb_address *callee_address) {
	uint32_t i;
	for(i = 0; i < cxt->callee_context->variable_count; i++) {
		qb_variable *qvar = cxt->callee_context->variables[i];
		if(qvar->address == callee_address) {
			return qvar;
		}
	}
	return NULL;
}

static void qb_add_constant_substitution(qb_function_inliner_context *cxt, qb_address *callee_address) {
	qb_address *caller_address;
	qb_primitive_type type = callee_address->type;
	if(SCALAR(callee_address)) {
		switch(type) {
			case QB_TYPE_S08: {
				CTYPE(S08) value = VALUE_IN(cxt->callee_context->storage, S08, callee_address);
				caller_address = qb_obtain_constant_S08(cxt->caller_context, value);
			}	break;
			case QB_TYPE_U08: {
				CTYPE(U08) value = VALUE_IN(cxt->callee_context->storage, U08, callee_address);
				caller_address = qb_obtain_constant_U08(cxt->caller_context, value);
			}	break;
			case QB_TYPE_S16: {
				CTYPE(S16) value = VALUE_IN(cxt->callee_context->storage, S16, callee_address);
				caller_address = qb_obtain_constant_S16(cxt->caller_context, value);
			}	break;
			case QB_TYPE_U16: {
				CTYPE(U16) value = VALUE_IN(cxt->callee_context->storage, U16, callee_address);
				caller_address = qb_obtain_constant_U16(cxt->caller_context, value);
			}	break;
			case QB_TYPE_S32: {
				CTYPE(S32) value = VALUE_IN(cxt->callee_context->storage, S32, callee_address);
				caller_address = qb_obtain_constant_S32(cxt->caller_context, value);
			}	break;
			case QB_TYPE_U32: {
				CTYPE(U32) value = VALUE_IN(cxt->callee_context->storage, U32, callee_address);
				caller_address = qb_obtain_constant_U32(cxt->caller_context, value);
			}	break;
			case QB_TYPE_S64: {
				CTYPE(S64) value = VALUE_IN(cxt->callee_context->storage, S64, callee_address);
				caller_address = qb_obtain_constant_S64(cxt->caller_context, value);
			}	break;
			case QB_TYPE_U64: {
				CTYPE(U64) value = VALUE_IN(cxt->callee_context->storage, U64, callee_address);
				caller_address = qb_obtain_constant_U64(cxt->caller_context, value);
			}	break;
			case QB_TYPE_F32: {
				CTYPE(F32) value = VALUE_IN(cxt->callee_context->storage, F32, callee_address);
				caller_address = qb_obtain_constant_F32(cxt->caller_context, value);
			}	break;
			case QB_TYPE_F64: {
				CTYPE(F64) value = VALUE_IN(cxt->callee_context->storage, F64, callee_address);
				caller_address = qb_obtain_constant_F64(cxt->caller_context, value);
			}	break;
		}
	} else {
		uint32_t dimension = ARRAY_SIZE_IN(cxt->callee_context->storage, callee_address);
		caller_address = qb_create_constant_array(cxt->caller_context, type, &dimension, 1);
		memcpy(ARRAY_IN(cxt->caller_context->storage, I08, caller_address), ARRAY_IN(cxt->callee_context->storage, I08, callee_address), BYTE_COUNT(dimension, type));
	}
	qb_add_substitution(cxt, callee_address, caller_address);
}

static void qb_perform_assignment(qb_function_inliner_context *cxt, qb_address *dst_address, qb_address *src_address) {
	qb_operand operands[2] = { { QB_OPERAND_ADDRESS, { dst_address } }, { QB_OPERAND_ADDRESS, { src_address } } };
	qb_operand result = { QB_OPERAND_EMPTY, { NULL } };
	qb_produce_op(cxt->caller_context, &factory_assign, operands, 2, &result, NULL, 0, NULL);
}

static void qb_add_writable_substitution(qb_function_inliner_context *cxt, qb_address *callee_address) {
	USE_TSRM
	int32_t need_new_address = TRUE;
	qb_address *caller_address;
	qb_address *argument_address = NULL;
	qb_variable *callee_var = qb_find_variable_with_address(cxt, callee_address);
	qb_primitive_type type = callee_address->type;

	if(callee_var) {
		if(callee_var->flags & QB_VARIABLE_ARGUMENT) {
			uint32_t argument_index = qb_get_variable_index(cxt->callee_context, callee_var->address);
			if(argument_index < cxt->argument_count) {
				// use the argument directly if the argument is passed by ref or read-only
				// and if the size and type matches
				qb_operand *argument = &cxt->arguments[argument_index];
				if(argument->type != QB_OPERAND_ADDRESS) {
					qb_perform_type_coercion(cxt->caller_context, argument, callee_address->type, 0);
				}
				argument_address = argument->address;				
				if((callee_var->flags & QB_VARIABLE_BY_REF) || READ_ONLY(callee_address)) {
					if(argument_address->type == callee_address->type) {
						int32_t compatible = FALSE;
						if(FIXED_LENGTH(argument_address)) {
							if(FIXED_LENGTH(callee_address)) {
								uint32_t length1 = ARRAY_SIZE_IN(cxt->caller_context->storage, argument_address);
								uint32_t length2 = ARRAY_SIZE_IN(cxt->callee_context->storage, callee_address);
								if(length1 == length2) {
									compatible = TRUE;
								}
							} else if(READ_ONLY(callee_address->array_size_address)) {
								// TODO: check granularity
								// the function doesn't change the length
								compatible = TRUE;
							}
						} else if(VARIABLE_LENGTH(argument_address) && VARIABLE_LENGTH(callee_address)) {
							compatible = TRUE;
						}
						if(compatible) {
							caller_address = argument_address;
							need_new_address = FALSE;
							argument_address = NULL;
						}
					}
				}
			} else {
				caller_address = qb_obtain_constant_zval(cxt->caller_context, callee_var->default_value, callee_var->address->type);
				need_new_address = FALSE;
			}
		} else if(callee_var->flags & QB_VARIABLE_RETURN_VALUE) {
			if(cxt->result->type == QB_OPERAND_NONE) {
				// the result isn't used--don't create an address for the return value
				need_new_address = FALSE;
				caller_address = NULL;
			}
		} else if(callee_var->flags & QB_VARIABLE_CLASS_INSTANCE) {
			zval *name = qb_string_to_zend_literal(callee_var->name, callee_var->name_length TSRMLS_CC);
			qb_variable *caller_var = qb_get_instance_variable(cxt->caller_context, name);
			need_new_address = FALSE;
			caller_address = caller_var->address;
		}
	}

	if(need_new_address) {
		if(callee_var && !(callee_var->flags & (QB_VARIABLE_ARGUMENT | QB_VARIABLE_RETURN_VALUE))) {
			qb_variable *new_var = qb_allocate_variable(cxt->caller_context->pool);
			new_var->name = callee_var->name;
			new_var->name_length = callee_var->name_length;
			new_var->zend_class = callee_var->zend_class;
			new_var->flags = callee_var->flags;
			new_var->hash_value = callee_var->hash_value;

			if(callee_address->dimension_count == 0) {
				caller_address = qb_create_writable_scalar(cxt->caller_context, type);
			} else {
				uint32_t dimension = FIXED_LENGTH(callee_address) ? ARRAY_SIZE_IN(cxt->callee_context->storage, callee_address) : 0;
				caller_address = qb_create_writable_array(cxt->caller_context, type, &dimension, 1);
			}
			caller_address->flags = callee_address->flags;
			new_var->address = caller_address;
		} else {
			qb_variable_dimensions dim;
			if(SCALAR(callee_address)) {
				dim.array_size_address =  NULL;
				dim.dimension_count = 0;
			} else {
				// base address of arrays always point to linear arrays
				dim.dimension_count = 1;
				if(FIXED_LENGTH(callee_address)) {
					// find equivalent constant address in the caller storage
					dim.array_size_address = qb_find_substitution_address(cxt, callee_address->array_size_address);
				} else {
					// it's variable length
					dim.array_size_address = NULL;
				}
			}
			caller_address = qb_obtain_temporary_variable(cxt->caller_context, callee_address->type, &dim);
			if(callee_var && (callee_var->flags & QB_VARIABLE_RETURN_VALUE)) {
				cxt->result->address = caller_address;
				cxt->result->type = QB_OPERAND_ADDRESS;
			}
		}
	}
	if(caller_address) {
		if(VARIABLE_LENGTH(caller_address)) {
			// add a substitution for the address of the array length as well
			qb_add_substitution(cxt, callee_address->array_size_address, caller_address->array_size_address);
		}
		if(argument_address) {
			// copy the argument into the temporary variable
			qb_perform_assignment(cxt, caller_address, argument_address);
		}
	}
	qb_add_substitution(cxt, callee_address, caller_address);
}

static void qb_add_alias_substitution(qb_function_inliner_context *cxt, qb_address *callee_alias) {
	qb_address *caller_alias = qb_allocate_address(cxt->caller_context->pool);
	qb_address **p_caller_alias = qb_enlarge_array((void **) &cxt->caller_context->address_aliases, 1);
	qb_address *source_address = qb_find_substitution_address(cxt, callee_alias->source_address);

	*p_caller_alias = caller_alias;
	caller_alias->type = source_address->type;
	caller_alias->segment_selector = INVALID_INDEX;
	caller_alias->segment_offset = INVALID_INDEX;
	caller_alias->source_address = source_address;
	caller_alias->flags = callee_alias->flags;
	caller_alias->dimension_count = callee_alias->dimension_count;
	caller_alias->mode = callee_alias->mode;

	if(SCALAR(callee_alias)) {
		caller_alias->array_size_address = cxt->caller_context->one_address;
	} else {
		if(callee_alias->dimension_count > 1) {
			// copy the dimensions over 
			uint32_t i;
			caller_alias->dimension_addresses = qb_allocate_address_pointers(cxt->caller_context->pool, caller_alias->dimension_count);
			caller_alias->array_size_addresses = qb_allocate_address_pointers(cxt->caller_context->pool, caller_alias->dimension_count);
			for(i = 0; i < caller_alias->dimension_count; i++) {
				caller_alias->dimension_addresses[i] = qb_find_substitution_address(cxt, callee_alias->dimension_addresses[i]);
				caller_alias->array_size_addresses[i] = qb_find_substitution_address(cxt, callee_alias->array_size_addresses[i]);
			}
		}
		caller_alias->array_size_address = qb_find_substitution_address(cxt, callee_alias->array_size_address);
	}
	if(callee_alias->array_index_address == cxt->callee_context->zero_address) {
		if(callee_alias->segment_offset == callee_alias->source_address->segment_offset) {
			caller_alias->array_index_address = cxt->caller_context->zero_address;
		} else {
			// it's a constant index, which has been resolved
			uint32_t offset = callee_alias->segment_offset - callee_alias->source_address->segment_offset;
			uint32_t index = ELEMENT_COUNT(offset, callee_alias->type);
			caller_alias->array_index_address = qb_obtain_constant_U32(cxt->caller_context, index);
		}
	} else {
		caller_alias->array_index_address = qb_find_substitution_address(cxt, callee_alias->array_index_address);
	}
	qb_add_substitution(cxt, callee_alias, caller_alias);
}

void qb_transfer_inlined_function_ops(qb_function_inliner_context *cxt) {
	uint32_t i, j = 0;
	uint32_t caller_op_offset = cxt->caller_context->op_count;
	int32_t multiple_returns = FALSE;

	// create the constants used by the function 
	for(i = 0; i < cxt->callee_context->constant_scalar_count; i++) {
		qb_address *callee_address = cxt->callee_context->constant_scalars[i];
		qb_add_constant_substitution(cxt, callee_address);
	}
	for(i = 0; i < cxt->callee_context->constant_array_count; i++) {
		qb_address *callee_address = cxt->callee_context->constant_arrays[i];
		qb_add_constant_substitution(cxt, callee_address);
	}

	// add the writable array first
	for(i = 0; i < cxt->callee_context->writable_array_count; i++) {
		qb_address *callee_address = cxt->callee_context->writable_arrays[i];
		qb_add_writable_substitution(cxt, callee_address);
	}
	for(i = 0; i < cxt->callee_context->writable_scalar_count; i++) {
		// add the scalar if it hasn't already been added as the length of an array
		qb_address *callee_address = cxt->callee_context->writable_scalars[i];
		if(callee_address->type != QB_TYPE_U32 || !qb_find_substitution_address(cxt, callee_address)) {
			qb_add_writable_substitution(cxt, callee_address);
		}
	}

	// create copies of aliases as well
	for(i = 0; i < cxt->callee_context->address_alias_count; i++) {
		qb_address *callee_alias = cxt->callee_context->address_aliases[i];
		qb_add_alias_substitution(cxt, callee_alias);
	}

	// copy the ops from the callee into the caller
	for(i = 0; i < cxt->callee_context->op_count; i++) {
		qb_op *callee_op = cxt->callee_context->ops[i];
		qb_op *caller_op = qb_allocate_op(cxt->caller_context->pool);
		if(callee_op->opcode == QB_RET) {
			if(i == cxt->callee_context->op_count - 1) {
				// the last op--insert a nop, 
				caller_op->opcode = QB_NOP;
				if(multiple_returns || callee_op->flags & QB_OP_JUMP_TARGET) {
					caller_op->flags |= QB_OP_JUMP_TARGET;
				}
			} else {
				// jump to the end, assuming the callee's last op is a RET
				caller_op->opcode = QB_JMP;
				caller_op->jump_target_count = 1;
				caller_op->jump_target_indices = qb_allocate_indices(cxt->caller_context->pool, caller_op->jump_target_count);
				caller_op->jump_target_indices[0] = caller_op_offset + cxt->callee_context->op_count - 1;
				caller_op->flags = QB_OP_JUMP | QB_OP_JUMP_TARGETS_RESOLVED;

				// marking it as a jump target is the RET was one
				if(callee_op->flags & QB_OP_JUMP_TARGET) {
					caller_op->flags |= QB_OP_JUMP_TARGET;
				}

				// need to mark the final NOP as a jump target
				multiple_returns = TRUE;
			}
		} else {
			int32_t omit = FALSE;
			caller_op->flags = callee_op->flags;
			caller_op->line_number = callee_op->line_number;
			caller_op->opcode = callee_op->opcode;

			if(callee_op->operand_count > 0) {
				caller_op->operand_count = callee_op->operand_count;
				caller_op->operands = qb_allocate_operands(cxt->caller_context->pool, caller_op->operand_count);
				for(j = 0; j < caller_op->operand_count; j++) {
					qb_operand *callee_operand = &callee_op->operands[j];
					qb_operand *caller_operand = &caller_op->operands[j];
					if(callee_operand->type == QB_OPERAND_ADDRESS || callee_operand->type == QB_OPERAND_SEGMENT_SELECTOR || callee_operand->type == QB_OPERAND_ELEMENT_SIZE) {
						caller_operand->address = qb_find_substitution_address(cxt, callee_operand->address);
						caller_operand->type = callee_operand->type;
						if(!caller_operand->address) {
							// address is NULL--probably because the return value isn't used
							omit = TRUE;
						}
					} else {
						*caller_operand = *callee_operand;
					}
				}
			}
			if(callee_op->jump_target_count > 0) {
				caller_op->jump_target_count = callee_op->jump_target_count;
				caller_op->jump_target_indices = qb_allocate_indices(cxt->caller_context->pool, caller_op->jump_target_count);
				for(j = 0; j < caller_op->jump_target_count; j++) {
					uint32_t jump_target_index = callee_op->jump_target_indices[j];
					caller_op->jump_target_indices[j] = caller_op_offset + jump_target_index;
				}
			}
			if(omit) {
				caller_op->opcode = QB_NOP;
				caller_op->operand_count = 0;
				caller_op->operands = NULL;
			}
		}
		qb_add_op(cxt->caller_context, caller_op);
	}

	// copy values into arguments where direct substitution wasn't possible
	for(i = 0; i < cxt->callee_context->argument_count; i++) {
		if(i < cxt->argument_count) {
			qb_address *argument_address = cxt->arguments[i].address;
			qb_variable *callee_var = cxt->callee_context->variables[i];
			if(callee_var->flags & QB_VARIABLE_BY_REF) {
				qb_address *caller_address = qb_find_substitution_address(cxt, callee_var->address);
				if(caller_address != argument_address) {
					qb_perform_assignment(cxt, argument_address, caller_address);
				}
			}
		}
	}

	// release temporary variables
	for(i = cxt->callee_context->constant_scalar_count + cxt->callee_context->constant_array_count; i < cxt->substitution_count; i++) {
		qb_address_substitution *substitution = &cxt->substitutions[i];
		if(substitution->current && TEMPORARY(substitution->current)) {
			qb_unlock_address(cxt->caller_context, substitution->current);
		}
	}
}

void qb_initialize_function_inliner_context(qb_function_inliner_context *cxt, qb_compiler_context *caller_cxt, qb_compiler_context *callee_cxt, qb_operand *arguments, uint32_t argument_count, qb_operand *result, qb_result_prototype *result_prototype TSRMLS_DC) {
	uint32_t substitution_count;

	cxt->caller_context = caller_cxt;
	cxt->callee_context = callee_cxt;
	cxt->arguments = arguments;
	cxt->argument_count = argument_count;
	cxt->result = result;
	cxt->result_prototype = result_prototype;

	substitution_count = callee_cxt->constant_scalar_count + callee_cxt->constant_array_count + callee_cxt->writable_scalar_count + callee_cxt->writable_array_count + callee_cxt->address_alias_count;
	cxt->substitutions = emalloc(sizeof(qb_address_substitution) * substitution_count);
	memset(cxt->substitutions, 0, sizeof(qb_address_substitution) * substitution_count);
	cxt->substitution_count = 0;

	SAVE_TSRMLS
}

void qb_free_function_inliner_context(qb_function_inliner_context *cxt) {
	efree(cxt->substitutions);
}