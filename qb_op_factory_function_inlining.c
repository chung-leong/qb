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

typedef struct qb_address_substitution		qb_address_substitution;

struct qb_address_substitution {
	qb_address *original;
	qb_address *current;
};

static void qb_set_constant_scalar_substitution(qb_compiler_context *cxt, qb_compiler_context *other_cxt, qb_address *other_address, qb_address_substitution *substitution) {
	qb_address *address;
	switch(other_address->type) {
		case QB_TYPE_S08: {
			CTYPE(S08) value = VALUE_IN(other_cxt->storage, S08, other_address);
			address = qb_obtain_constant_S08(cxt, value);
		}	break;
		case QB_TYPE_U08: {
			CTYPE(U08) value = VALUE_IN(other_cxt->storage, U08, other_address);
			address = qb_obtain_constant_U08(cxt, value);
		}	break;
		case QB_TYPE_S16: {
			CTYPE(S16) value = VALUE_IN(other_cxt->storage, S16, other_address);
			address = qb_obtain_constant_S16(cxt, value);
		}	break;
		case QB_TYPE_U16: {
			CTYPE(U16) value = VALUE_IN(other_cxt->storage, U16, other_address);
			address = qb_obtain_constant_U16(cxt, value);
		}	break;
		case QB_TYPE_S32: {
			CTYPE(S32) value = VALUE_IN(other_cxt->storage, S32, other_address);
			address = qb_obtain_constant_S32(cxt, value);
		}	break;
		case QB_TYPE_U32: {
			CTYPE(U32) value = VALUE_IN(other_cxt->storage, U32, other_address);
			address = qb_obtain_constant_U32(cxt, value);
		}	break;
		case QB_TYPE_S64: {
			CTYPE(S64) value = VALUE_IN(other_cxt->storage, S64, other_address);
			address = qb_obtain_constant_S64(cxt, value);
		}	break;
		case QB_TYPE_U64: {
			CTYPE(U64) value = VALUE_IN(other_cxt->storage, U64, other_address);
			address = qb_obtain_constant_U64(cxt, value);
		}	break;
		case QB_TYPE_F32: {
			CTYPE(F32) value = VALUE_IN(other_cxt->storage, F32, other_address);
			address = qb_obtain_constant_F32(cxt, value);
		}	break;
		case QB_TYPE_F64: {
			CTYPE(F64) value = VALUE_IN(other_cxt->storage, F64, other_address);
			address = qb_obtain_constant_F64(cxt, value);
		}	break;
	}
	substitution->original = other_address;
	substitution->current = address;
}

static void qb_set_constant_array_substitution(qb_compiler_context *cxt, qb_compiler_context *other_cxt, qb_address *other_address, qb_address_substitution *substitution) {
	qb_address *address;
	qb_primitive_type type = other_address->type;
	uint32_t dimensions[MAX_DIMENSION], dimension_count = other_address->dimension_count;
	uint32_t i, element_count;
	for(i = 0; i < dimension_count; i++) {
		dimensions[i] = VALUE_IN(other_cxt->storage, U32, other_address->dimension_addresses[i]);
	}
	address = qb_create_constant_array(cxt, type, dimensions, dimension_count);
	element_count = ARRAY_SIZE(address);
	memcpy(ARRAY(I08, address), ARRAY_IN(other_cxt->storage, I08, other_address), BYTE_COUNT(element_count, type));
	substitution->original = other_address;
	substitution->current = address;
}

static qb_variable * qb_find_variable_with_address(qb_compiler_context *cxt, qb_address *address) {
	uint32_t i;
	for(i = 0; i < cxt->variable_count; i++) {
		qb_variable *qvar = cxt->variables[i];
		if(qvar->address == address) {
			return qvar;
		}
	}
	return NULL;
}

static qb_variable * qb_find_variable_with_dimension_address(qb_compiler_context *cxt, qb_address *address, uint32_t *p_index) {
	uint32_t i, j;
	for(i = 0; i < cxt->variable_count; i++) {
		qb_variable *qvar = cxt->variables[i];
		if(qvar->address) {
			for(j = 0; j < qvar->address->dimension_count; j++) {
				if(qvar->address->dimension_addresses[j] == address) {
					*p_index = j;
					return qvar;
				}
			}
		}
	}
	return NULL;
}

static qb_variable * qb_find_variable_with_array_size_address(qb_compiler_context *cxt, qb_address *address, uint32_t *p_index) {
	uint32_t i, j;
	for(i = 0; i < cxt->variable_count; i++) {
		qb_variable *qvar = cxt->variables[i];
		if(qvar->address) {
			for(j = 0; j < qvar->address->dimension_count; j++) {
				if(qvar->address->array_size_addresses[j] == address) {
					*p_index = j;
					return qvar;
				}
			}
		}
	}
	return NULL;
}

static void qb_set_writable_scalar_substitution(qb_compiler_context *cxt, qb_compiler_context *other_cxt, qb_address *other_address, qb_operand *arguments, uint32_t argument_count, qb_operand *result, qb_address_substitution *substitution) {
	qb_address *address;
	qb_variable *other_var = qb_find_variable_with_address(other_cxt, other_address);
	if(other_var) {
		if(other_var->flags & QB_VARIABLE_ARGUMENT) {
			uint32_t argument_index = qb_get_variable_index(other_cxt, other_var->address);
			if(argument_index < argument_count) {
				address = arguments[argument_index].address;
			} else {
				address = qb_obtain_constant_zval(cxt, other_var->default_value, other_var->address->type);
			}
		} else if(other_var->flags & QB_VARIABLE_RETURN_VALUE) {
			if(result->type == QB_OPERAND_ADDRESS) {
				address = result->address;
			} else {
				address = NULL;
			}
		} else {
			if(other_var->flags & QB_VARIABLE_GLOBAL) {

			} else if(other_var->flags & QB_VARIABLE_STATIC) {

			} else if(other_var->flags & QB_VARIABLE_CLASS) {

			} else if(other_var->flags & QB_VARIABLE_CLASS_INSTANCE) {

			} else if(other_var->flags & QB_VARIABLE_CLASS_CONSTANT) {

			}
		}
	} else {
		address = qb_obtain_temporary_variable(cxt, other_address->type, NULL);
	}
	substitution->original = other_address;
	substitution->current = address;
}

static qb_address * qb_find_substitution_address(qb_compiler_context *cxt, qb_address *address, qb_address_substitution *substitutions, uint32_t substitution_count) {
	uint32_t i;
	for(i = 0; i < substitution_count; i++) {
		qb_address_substitution *substitution = &substitutions[i];
		if(substitution->original == address) {
			return substitution->current;
		}
	}
	return NULL;
}

static void qb_inline_function(qb_compiler_context *cxt, void *factory, qb_operand *operands, uint32_t operand_count, qb_operand *result, uint32_t *jump_target_indices, uint32_t jump_target_count, qb_result_prototype *result_prototype) {
	USE_TSRM
	zend_function *zfunc = operands[0].zend_function;
	qb_operand *arguments = operands[1].arguments;
	uint32_t argument_count = operands[2].number;
	qb_function *qfunc = qb_find_compiled_function(zfunc);
	qb_compiler_context *other_cxt = qb_find_compiler_context(QB_G(build_context), qfunc);
	qb_address_substitution *substitutions;
	uint32_t substitution_count;
	uint32_t i, j = 0;
	ALLOCA_FLAG(use_heap);

	// perform type coercion
	for(i = 0; i < argument_count; i++) {
		qb_primitive_type arg_type;
		if(i < other_cxt->argument_count) {
			qb_variable *qvar = other_cxt->variables[i];
			arg_type = qvar->address->type;
		} else {
			arg_type = QB_TYPE_ANY;
		}
		qb_perform_type_coercion(cxt, &arguments[i], arg_type, 0);
	}

	// get write target for return value
	if(result->type != QB_OPERAND_NONE) {
		qb_address *ret_address = other_cxt->return_variable->address;
		if(ret_address) {
			qb_variable_dimensions dim;
			qb_copy_address_dimensions(other_cxt, ret_address, 0, &dim);
			result->address = qb_obtain_write_target(cxt, ret_address->type, &dim, ret_address->flags, result_prototype, TRUE);
			result->type = QB_OPERAND_ADDRESS;
		}
	}

	substitution_count = other_cxt->constant_scalar_count + other_cxt->constant_array_count + other_cxt->writable_scalar_count + other_cxt->writable_array_count;
	substitutions = do_alloca(sizeof(qb_address_substitution) * substitution_count, use_heap);

	// create the constants used by the function 
	for(i = 0; i < other_cxt->constant_scalar_count; i++, j++) {
		qb_address *other_address = other_cxt->constant_scalars[i];
		qb_set_constant_scalar_substitution(cxt, other_cxt, other_address, &substitutions[j]);
	}
	for(i = 0; i < other_cxt->constant_array_count; i++, j++) {
		qb_address *other_address = other_cxt->constant_arrays[i];
		qb_set_constant_array_substitution(cxt, other_cxt, other_address, &substitutions[j]);
	}

	// see how writable addresses should be mapped
	for(i = 0; i < other_cxt->writable_scalar_count; i++, j++) {
		qb_address *other_address = other_cxt->writable_scalars[i];
		qb_set_writable_scalar_substitution(cxt, other_cxt, other_address, arguments, argument_count, result, &substitutions[j]);
	}

	// copy the ops from the callee into the caller
	for(i = 0; i < other_cxt->op_count; i++) {
		qb_op *other_op = other_cxt->ops[i];
		if(other_op->opcode == QB_RET) {
			// don't need to worry about multiple return for now, since functions with branching are flagged as not-inlineable
			break;
		} else {
			int32_t omit = FALSE;
			qb_op *qop = qb_allocate_op(cxt->pool);
			qop->flags = other_op->flags;
			qop->line_number = cxt->line_number;
			qop->opcode = other_op->opcode;
			qop->operand_count = other_op->operand_count;
			qop->operands = qb_allocate_operands(cxt->pool, qop->operand_count);

			for(j = 0; j < qop->operand_count; j++) {
				qb_operand *other_operand = &other_op->operands[j];
				qb_operand *operand = &qop->operands[j];
				if(other_operand->type == QB_OPERAND_ADDRESS) {
					operand->address = qb_find_substitution_address(cxt, other_operand->address, substitutions, substitution_count);
					operand->type = other_operand->type;
					if(operand->address) {
						if(!READ_ONLY(other_operand->address)) {
							qb_mark_as_writable(cxt, operand->address);
						}
					} else {
						// address is NULL--probably because the return value isn't used
						omit = TRUE;
					}
				} else {
					*operand = *other_operand;
				}
			}
			if(!omit) {
				qb_add_op(cxt, qop);
			}
		}
	}

	// release temporary variables
	for(i = other_cxt->constant_scalar_count + other_cxt->constant_array_count; i < substitution_count; i++) {
		qb_address_substitution *substitution = &substitutions[i];
		if(TEMPORARY(substitution->current)) {
			qb_unlock_address(cxt, substitution->current);
		}
	}

	free_alloca(substitutions, use_heap);
}
