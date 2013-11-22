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
	qb_address_substitution *substitution;
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
	substitution = &cxt->substitutions[cxt->substitution_count++];
	substitution->original = callee_address;
	substitution->current = caller_address;
}

static void	qb_copy_callee_address_dimensions(qb_function_inliner_context *cxt, qb_address *callee_address, qb_variable_dimensions *dim) {
	uint32_t i;
	dim->dimension_count = callee_address->dimension_count;
	dim->array_size_address = qb_find_substitution_address(cxt, callee_address);
	for(i = 0; i < dim->dimension_count; i++) {
		dim->dimension_addresses[i] = qb_find_substitution_address(cxt, callee_address->dimension_addresses[i]);
		dim->array_size_addresses[i] = qb_find_substitution_address(cxt, callee_address->array_size_addresses[i]);
	}
	dim->array_size_address = dim->array_size_addresses[0];
}

static void qb_add_writable_substitution(qb_function_inliner_context *cxt, qb_address *callee_address) {
	int32_t need_new_address;
	qb_address *caller_address;
	qb_variable *callee_var = qb_find_variable_with_address(cxt, callee_address);
	qb_primitive_type type = callee_address->type;
	qb_address_substitution *substitution;

	if(callee_var) {
		if(callee_var->flags & QB_VARIABLE_ARGUMENT) {
			uint32_t argument_index = qb_get_variable_index(cxt->callee_context, callee_var->address);
			if(argument_index < cxt->argument_count) {
				caller_address = cxt->arguments[argument_index].address;
			} else {
				caller_address = qb_obtain_constant_zval(cxt->caller_context, callee_var->default_value, callee_var->address->type);
			}
			need_new_address = FALSE;
		} else if(callee_var->flags & QB_VARIABLE_RETURN_VALUE) {
			if(cxt->result->type == QB_OPERAND_NONE) {
				// the result isn't used--don't create an address for the return value
				need_new_address = FALSE;
				caller_address = NULL;
			} else {
				need_new_address = TRUE;
			}
		} else {
			need_new_address = TRUE;
		}
	} else {
		need_new_address = TRUE;
	}

	if(need_new_address) {
		qb_variable_dimensions dim;
		qb_copy_callee_address_dimensions(cxt, callee_address, &dim);
		if(callee_var && !(callee_var->flags & QB_VARIABLE_RETURN_VALUE)) {
			qb_variable *new_var = qb_allocate_variable(cxt->caller_context->pool);
			new_var->name = callee_var->name;
			new_var->name_length = callee_var->name_length;
			new_var->zend_class = callee_var->zend_class;
			new_var->flags = callee_var->flags;
			new_var->hash_value = callee_var->hash_value;

			if(dim.dimension_count == 0) {
				caller_address = qb_create_writable_scalar(cxt->caller_context, type);
			} else {
				// can't use qb_create_writable_array() here, since we want to use a specific address for the array length
				caller_address = qb_allocate_address(cxt->caller_context->pool);
				caller_address->mode = QB_ADDRESS_MODE_ARR;
				caller_address->type = type;
				caller_address->flags = QB_ADDRESS_READ_ONLY | QB_ADDRESS_ALWAYS_IN_BOUND;
				caller_address->segment_selector = QB_SELECTOR_INVALID;
				caller_address->segment_offset = QB_OFFSET_INVALID;
				caller_address->array_index_address = cxt->caller_context->zero_address;
				caller_address->array_size_address = qb_find_substitution_address(cxt, callee_address->array_size_address);
				caller_address->array_size_addresses = caller_address->dimension_addresses = &caller_address->array_size_address;
				caller_address->dimension_count = 1;
				qb_add_writable_array(cxt->caller_context, caller_address);
			}
			caller_address->flags = callee_address->flags;
			new_var->address = caller_address;
		} else {
			if(callee_var && (callee_var->flags & QB_VARIABLE_RETURN_VALUE)) {
				caller_address = qb_obtain_write_target(cxt->caller_context, callee_address->type, &dim, callee_address->flags, cxt->result_prototype, TRUE);
				cxt->result->address = caller_address;
				cxt->result->type = QB_OPERAND_ADDRESS;
			} else {
				caller_address = qb_obtain_temporary_variable(cxt->caller_context, callee_address->type, &dim);
			}
		}
	}
	if(caller_address) {
		//if(!READ_ONLY(callee_address)) {
			qb_mark_as_writable(cxt->caller_context, caller_address);
		//}
	}

	substitution = &cxt->substitutions[cxt->substitution_count++];
	substitution->original = callee_address;
	substitution->current = caller_address;
}

static void qb_add_alias_substitution(qb_function_inliner_context *cxt, qb_address *callee_alias) {
	qb_address *caller_alias = qb_allocate_address(cxt->caller_context->pool);
	qb_address **p_caller_alias = qb_enlarge_array((void **) &cxt->caller_context->address_aliases, 1);
	qb_address *source_address = qb_find_substitution_address(cxt, callee_alias->source_address);
	qb_address_substitution *substitution;

	*caller_alias = *source_address;
	caller_alias->source_address = source_address;
	caller_alias->flags = callee_alias->flags;
	*p_caller_alias = caller_alias;
	if(callee_alias->dimension_count > 1) {
		// copy the dimensions over 
		uint32_t i;
		caller_alias->dimension_count = callee_alias->dimension_count;
		caller_alias->dimension_addresses = qb_allocate_address_pointers(cxt->caller_context->pool, caller_alias->dimension_count);
		caller_alias->array_size_addresses = qb_allocate_address_pointers(cxt->caller_context->pool, caller_alias->dimension_count);
		for(i = 0; i < caller_alias->dimension_count; i++) {
			caller_alias->dimension_addresses[i] = qb_find_substitution_address(cxt, callee_alias->dimension_addresses[i]);
			caller_alias->array_size_addresses[i] = qb_find_substitution_address(cxt, callee_alias->array_size_addresses[i]);
		}
	}
	substitution = &cxt->substitutions[cxt->substitution_count++];
	substitution->original = callee_alias;
	substitution->current = caller_alias;
}

void qb_transfer_inlined_function_ops(qb_function_inliner_context *cxt) {
	uint32_t i, j = 0;

	// perform type coercion
	for(i = 0; i < cxt->argument_count; i++) {
		qb_primitive_type arg_type;
		if(i < cxt->callee_context->argument_count) {
			qb_variable *qvar = cxt->callee_context->variables[i];
			arg_type = qvar->address->type;
		} else {
			arg_type = QB_TYPE_ANY;
		}
		qb_perform_type_coercion(cxt->caller_context, &cxt->arguments[i], arg_type, 0);
	}

	// create the constants used by the function 
	for(i = 0; i < cxt->callee_context->constant_scalar_count; i++) {
		qb_add_constant_substitution(cxt, cxt->callee_context->constant_scalars[i]);
	}
	for(i = 0; i < cxt->callee_context->constant_array_count; i++) {
		qb_add_constant_substitution(cxt, cxt->callee_context->constant_arrays[i]);
	}

	// see how writable addresses should be mapped
	for(i = 0; i < cxt->callee_context->writable_scalar_count; i++) {
		qb_add_writable_substitution(cxt, cxt->callee_context->writable_scalars[i]);
	}
	for(i = 0; i < cxt->callee_context->writable_array_count; i++) {
		qb_add_writable_substitution(cxt, cxt->callee_context->writable_arrays[i]);
	}

	// create copies of aliases as well
	for(i = 0; i < cxt->callee_context->address_alias_count; i++) {
		qb_add_alias_substitution(cxt, cxt->callee_context->address_aliases[i]);
	}

	// copy the ops from the callee into the caller
	for(i = 0; i < cxt->callee_context->op_count; i++) {
		qb_op *callee_op = cxt->callee_context->ops[i];
		if(callee_op->opcode == QB_RET) {
			// don't need to worry about multiple return for now, since functions with branching are flagged as not-inlineable
			break;
		} else {
			int32_t omit = FALSE;
			qb_op *caller_op = qb_allocate_op(cxt->caller_context->pool);
			caller_op->flags = callee_op->flags;
			caller_op->line_number = callee_op->line_number;
			caller_op->opcode = callee_op->opcode;
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
			if(!omit) {
				qb_add_op(cxt->caller_context, caller_op);
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