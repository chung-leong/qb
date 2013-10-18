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

static void qb_set_result_prototype(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_result_prototype *result_prototype) {
	result->type = QB_OPERAND_RESULT_PROTOTYPE;
	result->result_prototype = result_prototype;
}

static void qb_set_result_temporary_value(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_result_prototype *result_prototype) {
	qb_variable_dimensions dim = { NULL, 0, cxt->one_address };

	// figure out the result size (it's a scalar if set_dimensions is null)
	if(f->set_dimensions) {
		f->set_dimensions(cxt, f, operands, operand_count, &dim);
	}
	result->type = QB_OPERAND_ADDRESS;
	result->address = qb_obtain_write_target(cxt, expr_type, &dim, f->address_flags, result_prototype, TRUE);
}

static void qb_set_result_first_operand(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_result_prototype *result_prototype) {
	*result = operands[0];
}

static void qb_set_result_second_operand(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_result_prototype *result_prototype) {
	*result = operands[1];
}

static void qb_set_result_third_operand(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_result_prototype *result_prototype) {
	*result = operands[2];
}

static void qb_set_result_true(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_result_prototype *result_prototype) {
	result->type = QB_OPERAND_ADDRESS;
	result->address = cxt->true_address;
}

static void qb_set_result_none(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_result_prototype *result_prototype) {
	result->type = QB_OPERAND_NONE;
}

static void qb_set_result_assign(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_result_prototype *result_prototype) {
	qb_operand *variable = &operands[0], *value = &operands[1];

	// if the expression type was set to void, then an earlier op has used the r-value as write target
	// so there's no need to perform the assignment
	if(expr_type != QB_TYPE_VOID) {
		qb_attach_bound_checking_expression(cxt, value->address->array_size_address, variable->address, TRUE);
		result->address = variable->address;
		result->type = QB_OPERAND_ADDRESS;
	}
}
static void qb_set_preliminary_result_assign_branching(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_result_prototype *result_prototype) {
	if(result->type == QB_OPERAND_RESULT_PROTOTYPE) {
		// link to the prototype in the other branch and keep it as the result 
		// so that type coercion on both branches is performed on that prototype 
		// ensuring the highest rank is obtained
		result_prototype->parent = result->result_prototype;
	} else {
		qb_set_result_prototype(cxt, f, expr_type, operands, operand_count, result, result_prototype);
	}
}

static void qb_set_final_result_assign_branching(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_result_prototype *result_prototype) {
	if(result->type == QB_OPERAND_ADDRESS) {
		// write to the same address as the other branch
	} else {
		qb_set_result_temporary_value(cxt, f, expr_type, operands, operand_count, result, result_prototype);
	}
}

static void qb_set_result_fetch_array_element(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_result_prototype *result_prototype) {
	qb_fetch_op_factory *ff = (qb_fetch_op_factory *) f;
	qb_operand *container = &operands[0], *index = &operands[1];
	result->address = qb_obtain_array_element(cxt, container->address, index->address, ff->bound_check_flags);
	result->type = QB_OPERAND_ADDRESS;
}

static void qb_set_result_fetch_array_size(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_result_prototype *result_prototype) {
	qb_operand *container = &operands[0], *recursive = &operands[1];
	qb_address *dimension_address = container->address->dimension_addresses[0];
	qb_address *array_size_address = container->address->array_size_address;
	if(recursive->type == QB_OPERAND_ADDRESS) {
		if(CONSTANT(recursive->address)) {
			result->address = VALUE(I32, recursive->address) ? array_size_address : dimension_address;
		} else {
			result->address = qb_obtain_larger_of_two(cxt, cxt->zero_address, dimension_address, recursive->address, array_size_address);
		}
	} else {
		result->address = dimension_address;
	}
	result->type = QB_OPERAND_ADDRESS;
}

static void qb_set_result_assign_array_element(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_result_prototype *result_prototype) {
	qb_operand *container = &operands[0], *index = &operands[1], *value = &operands[2];

	if(expr_type != QB_TYPE_VOID) {
		qb_address *index_address = (index->type == QB_OPERAND_NONE) ? container->address->dimension_addresses[0] : index->address;
		qb_address *result_address = qb_obtain_array_element(cxt, container->address, index_address, QB_ARRAY_BOUND_CHECK_WRITE);
		qb_attach_bound_checking_expression(cxt, value->address->array_size_address, result_address, TRUE);
		result->address = result_address;
		result->type = QB_OPERAND_ADDRESS;
	}
}

static void qb_set_result_fetch_object_property(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_result_prototype *result_prototype) {
	qb_fetch_op_factory *ff = (qb_fetch_op_factory *) f;
	qb_operand *container = &operands[0], *name = &operands[1];
	qb_address *result_address;
	if(container->type == QB_OPERAND_NONE) {
		result_address = qb_obtain_instance_variable(cxt, name->constant);
	} else if(container->type == QB_OPERAND_ADDRESS) {
		result_address = qb_obtain_named_element(cxt, container->address, name->constant, ff->bound_check_flags);
	}
	result->address = result_address;
	result->type = QB_OPERAND_ADDRESS;
}

static void qb_set_result_assign_object_property(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_result_prototype *result_prototype) {
	qb_operand *container = &operands[0], *name = &operands[1], *value = &operands[2];

	if(expr_type != QB_TYPE_VOID) {
		qb_address *result_address = qb_obtain_object_property(cxt, container, name, QB_ARRAY_BOUND_CHECK_WRITE);
		qb_attach_bound_checking_expression(cxt, value->address->array_size_address, result_address, TRUE);
		result->address = result_address;
		result->type = QB_OPERAND_ADDRESS;
	}
}

static void qb_set_result_fetch_local(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_result_prototype *result_prototype) {
	qb_operand *name = &operands[0];
	result->address = qb_obtain_local_variable(cxt, name->constant);
	result->type = QB_OPERAND_ADDRESS;
}

static void qb_set_result_fetch_global(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_result_prototype *result_prototype) {
	qb_operand *name = &operands[0];
	result->address = qb_obtain_global_variable(cxt, name->constant);
	result->type = QB_OPERAND_ADDRESS;
}

static void qb_set_result_fetch_static(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_result_prototype *result_prototype) {
	qb_operand *name = &operands[0];
	result->address = qb_obtain_static_variable(cxt, name->constant);
	result->type = QB_OPERAND_ADDRESS;
}

static void qb_set_result_fetch_class(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_result_prototype *result_prototype) {
	qb_operand *name = &operands[0], *scope = &operands[1];
	result->address = qb_obtain_class_variable(cxt, scope->zend_class, name->constant);
	result->type = QB_OPERAND_ADDRESS;
}

static void qb_set_result_increment_post(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_result_prototype *result_prototype) {
	// set result to a temporary variable unless it'll end up just getting freed
	if(!result_prototype->destination || result_prototype->destination->type != QB_RESULT_DESTINATION_FREE) {
		// use the destination's type
		qb_primitive_type destination_type = qb_get_result_destination_type(cxt, result_prototype->destination);
		if(destination_type != QB_TYPE_ANY && destination_type != QB_TYPE_UNKNOWN) {
			expr_type = destination_type;
		}
		qb_set_result_temporary_value(cxt, f, expr_type, operands, operand_count, result, result_prototype);
	}
}

static void qb_set_result_array_append(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_result_prototype *result_prototype) {
	qb_operand *value = &operands[0], *index = &operands[1];
	qb_operand *element;
	qb_array_initializer *initializer = result->array_initializer;
	uint32_t element_index;
	if(index->type == QB_OPERAND_NONE) {
		element_index = initializer->element_count;
	} else {
		switch(Z_TYPE_P(index->constant)) {
			case IS_BOOL:
			case IS_LONG: element_index = Z_LVAL_P(index->constant); break;
			case IS_DOUBLE: element_index = (long) Z_DVAL_P(index->constant); break;
			default: element_index = 0; break;
		}
	}
	element = qb_expand_array_initializer(cxt, initializer, element_index);
	*element = *value;
	if(value->type == QB_OPERAND_ADDRESS) {
		initializer->flags |= QB_ARRAY_INITIALIZER_VARIABLE_ELEMENTS;
	} else if(value->type == QB_OPERAND_ARRAY_INITIALIZER) {
		if(value->array_initializer->flags & QB_ARRAY_INITIALIZER_VARIABLE_ELEMENTS) {
			initializer->flags |= QB_ARRAY_INITIALIZER_VARIABLE_ELEMENTS;
		}
		if(value->array_initializer->flags & QB_ARRAY_INITIALIZER_CONSTANT_ELEMENTS) {
			initializer->flags |= QB_ARRAY_INITIALIZER_CONSTANT_ELEMENTS;
		}
	} else if(value->type == QB_OPERAND_ZVAL) {
		initializer->flags |= QB_ARRAY_INITIALIZER_CONSTANT_ELEMENTS;
	}
}

static void qb_set_result_array_init(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_result_prototype *result_prototype) {
	result->type = QB_OPERAND_ARRAY_INITIALIZER;
	result->array_initializer = qb_allocate_array_initializer(cxt->pool);
	if(operand_count > 0) {
		qb_set_result_array_append(cxt, f, expr_type, operands, operand_count, result, result_prototype);
	}
}

static void qb_set_result_empty_string(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_result_prototype *result_prototype) {
	qb_variable_dimensions dim = { NULL, 1, NULL };
	qb_address *address = qb_obtain_temporary_variable(cxt, QB_TYPE_U08, &dim);
	address = qb_obtain_string_alias(cxt, address);
	result->address = address;
	result->type = QB_OPERAND_ADDRESS;
}

static void qb_set_result_add_string(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_result_prototype *result_prototype) {
	qb_operand *augend = &operands[0], *addend = &operands[1];
	if(augend->type == QB_OPERAND_ADDRESS) {
		*result = *augend;
	} else {
		qb_operand es_result = { QB_OPERAND_EMPTY, NULL };
		qb_produce_op(cxt, &factory_empty_string, NULL, 0, &es_result, NULL, 0, NULL);
		result->address = es_result.address;
		result->type = QB_OPERAND_ADDRESS;
	}
}

static void qb_set_result_concat(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_result_prototype *result_prototype) {
	qb_operand *augend = &operands[0], *addend = &operands[1];
	if(augend->type == QB_OPERAND_ADDRESS && TEMPORARY(augend->address) && (augend->address->flags & QB_ADDRESS_STRING)) {
		// append to the augend
		*result = *augend;
	} else {
		qb_operand av_operands[2] = { { QB_OPERAND_EMPTY, NULL }, { QB_OPERAND_ADDRESS, augend->address } };
		qb_operand av_result = { QB_OPERAND_EMPTY, NULL };
		qb_produce_op(cxt, &factory_add_variable, av_operands, 2, &av_result, NULL, 0, NULL);
		*result = av_result;
	}
}

static void qb_set_result_array_cast(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_result_prototype *result_prototype) {
	qb_operand *value = &operands[0];
	if(SCALAR(value->address)) {
		result->address = qb_obtain_array_alias(cxt, value->address);
	} else {
		result->address = value->address;
	}
	result->type = QB_OPERAND_ADDRESS;
}

static void qb_set_result_string_cast(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_result_prototype *result_prototype) {
	qb_operand *value = &operands[0];
	if(value->address->flags & QB_ADDRESS_STRING) {
		result->address = value->address;
	} else {
		qb_operand av_operands[2] = { { QB_OPERAND_EMPTY, NULL }, { QB_OPERAND_ADDRESS, value->address } };
		qb_operand av_result = { QB_OPERAND_EMPTY, NULL };
		qb_produce_op(cxt, &factory_add_variable, av_operands, 2, &av_result, NULL, 0, NULL);
		*result = av_result;
	}
}

static void qb_set_result_foreach_reset(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_result_prototype *result_prototype) {
	qb_operand *container = &operands[0];
	cxt->foreach_index.address = qb_create_writable_scalar(cxt, QB_TYPE_U32);
	cxt->foreach_index.type = QB_OPERAND_ADDRESS;
	*result = *container;
}

static void qb_set_result_foreach_fetch(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_result_prototype *result_prototype) {
	qb_operand *container = &operands[0], *extra_result = &result[1];
	if(extra_result->type != QB_OPERAND_NONE) {
		*extra_result = cxt->foreach_index;
	}
	result->type = QB_OPERAND_ADDRESS;
	result->address = qb_obtain_array_element(cxt, container->address, cxt->foreach_index.address, QB_ARRAY_BOUND_CHECK_NONE);
}

static void qb_set_result_fetch_class_self(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_result_prototype *result_prototype) {
	zend_class_entry *ce = cxt->zend_op_array->scope;
	result->type = QB_OPERAND_ZEND_CLASS;
	result->zend_class = ce;
}

static void qb_set_result_fetch_class_parent(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_result_prototype *result_prototype) {
	zend_class_entry *ce = cxt->zend_op_array->scope;
	result->type = QB_OPERAND_ZEND_CLASS;
	result->zend_class = ce->parent;
}

static void qb_set_result_fetch_class_static(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_result_prototype *result_prototype) {
	result->type = QB_OPERAND_ZEND_STATIC_CLASS;
	result->zend_class = NULL;
}

static void qb_set_result_fetch_constant(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_result_prototype *result_prototype) {
	USE_TSRM
	qb_operand *scope = &operands[0], *name = &operands[1];
	if(scope->type == QB_OPERAND_ZEND_STATIC_CLASS) {
		// since we don't know what class the function is in until it's called
		// the constant actually has to be a variable 
		if(expr_type == QB_TYPE_ANY) {
			// use F64 if we don't know what it is
			expr_type = QB_TYPE_F64;
		}
		result->type = QB_OPERAND_ADDRESS;
		result->address = qb_obtain_class_static_constant(cxt, name->constant, expr_type);
	} else {
		zend_class_entry *ce = NULL;
		zval *value;
		if(scope->type == QB_OPERAND_ZEND_CLASS) {
			ce = scope->zend_class;
		} else if(scope->type == QB_OPERAND_ZVAL) {
			ce = zend_fetch_class_by_name(Z_STRVAL_P(scope->constant), Z_STRLEN_P(scope->constant), NULL, 0 TSRMLS_CC);
		}
		if(ce) {
			zval **p_value;
			zend_hash_find(&ce->constants_table, Z_STRVAL_P(name->constant), Z_STRLEN_P(name->constant) + 1, (void **) &p_value);
			value = *p_value;
		} else {
			zend_constant *zconst;
			if(zend_hash_find(EG(zend_constants), Z_STRVAL_P(name->constant), Z_STRLEN_P(name->constant) + 1, (void **) &zconst) == SUCCESS) {
				value = &zconst->value;
			} else {
				value = qb_get_special_constant(cxt, Z_STRVAL_P(name->constant), Z_STRLEN_P(name->constant));
			}
		}
		result->type = QB_OPERAND_ZVAL;
		result->constant = value;
	}
}

static void qb_set_result_receive_argument(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_result_prototype *result_prototype) {
	qb_operand *argument = &operands[0], *default_value = &operands[1];
	if(default_value->type == QB_OPERAND_ZVAL) {
		qb_variable *qvar = cxt->variables[argument->number - 1];
		qvar->default_value = default_value->constant;
	}
}

static void qb_set_result_send_argument(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_result_prototype *result_prototype) {
	qb_operand *argument = &operands[0];
	qb_operand *stack_item = qb_push_stack_item(cxt);
	*stack_item = *argument;
}

static void qb_set_result_array_push(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_result_prototype *result_prototype) {
	qb_operand *container = &operands[0];
	qb_address *dimension_address = container->address->dimension_addresses[0];
	qb_address *variable_address = qb_obtain_array_element(cxt, container->address, dimension_address, QB_ARRAY_BOUND_CHECK_WRITE);
	uint32_t i;

	for(i = 1; i < operand_count; i++) {
		qb_operand assign_operands[2] = { { QB_OPERAND_ADDRESS, variable_address }, { QB_OPERAND_ADDRESS, operands[i].address } };
		qb_operand assign_result = { QB_OPERAND_ADDRESS, variable_address };
		qb_create_op(cxt, &factory_assign, assign_operands, 2, &assign_result, NULL, 0, FALSE);
	}
	result->address = dimension_address;
	result->type = QB_OPERAND_ADDRESS;
}

static void qb_set_result_array_slice(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_result_prototype *result_prototype) {
	qb_operand *container = &operands[0], *offset = &operands[1], *length = &operands[2];
	result->address = qb_obtain_array_slice(cxt, container->address, offset->address, (length->type == QB_OPERAND_ADDRESS) ? length->address : NULL, QB_ARRAY_BOUND_CHECK_READ);
	result->type = QB_OPERAND_ADDRESS;
}

static void qb_set_result_array_unshift(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_result_prototype *result_prototype) {
	qb_operand *container = &operands[0];
	qb_address *dimension_address = container->address->dimension_addresses[0];
	qb_address *variable_address = qb_obtain_array_element(cxt, container->address, dimension_address, QB_ARRAY_BOUND_CHECK_WRITE);
	uint32_t i;

	/*
	for(i = 1; i < operand_count; i++) {
		qb_operand assign_operands[2] = { { QB_OPERAND_ADDRESS, variable_address }, { QB_OPERAND_ADDRESS, operands[i].address } };
		qb_operand assign_result = { QB_OPERAND_ADDRESS, variable_address };
		qb_create_op(cxt, &factory_array_insert, assign_operands, 2, &assign_result, NULL, 0, FALSE);
	}
	*/
	result->address = dimension_address;
	result->type = QB_OPERAND_ADDRESS;
}

static void qb_set_result_utf8_decode(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_result_prototype *result_prototype) {
	qb_variable_dimensions dim;
	if(expr_type != QB_TYPE_U32 && expr_type != QB_TYPE_U16) {
		if(STORAGE_TYPE_MATCH(expr_type, QB_TYPE_U16)) {
			expr_type = QB_TYPE_U16;
		} else {
			expr_type = QB_TYPE_U32;
		}
	}
	f->set_dimensions(cxt, f, operands, operand_count, &dim);
	result->type = QB_OPERAND_ADDRESS;
	result->address = qb_obtain_write_target(cxt, expr_type, &dim, f->address_flags, result_prototype, TRUE);
}

static void qb_set_preliminary_result_unpack(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_result_prototype *result_prototype) {
	if(result_prototype->preliminary_type != QB_TYPE_ANY) {
		// if we have the type from parameter 3, then that's the final type
		result_prototype->final_type = result_prototype->preliminary_type;
	}
	qb_set_result_prototype(cxt, f, expr_type, operands, operand_count, result, result_prototype);
}

static void qb_set_result_defined(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_result_prototype *result_prototype) {
	USE_TSRM
	qb_operand *name = &operands[0];
	zval c;
	char *name_str = Z_STRVAL_P(name->constant);
	uint32_t name_len = Z_STRLEN_P(name->constant);
	int32_t constant_exists = FALSE;
#if !ZEND_ENGINE_2_2 && !ZEND_ENGINE_2_1
	if(zend_get_constant_ex(name_str, name_len, &c, NULL, ZEND_FETCH_CLASS_SILENT TSRMLS_CC)) {
#else
	if(zend_get_constant(name_str, name_len, &c TSRMLS_CC)) {
#endif
		zval_dtor(&c);
		constant_exists = TRUE;
	} else {
		zval *special_constant = qb_get_special_constant(cxt, name_str, name_len);
		if(special_constant) {
			constant_exists = TRUE;
		}
	}
	result->type = QB_OPERAND_ADDRESS;
	result->address = (constant_exists) ? cxt->true_address : cxt->false_address;
}

static void qb_set_result_define(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_result_prototype *result_prototype) {
	USE_TSRM
	qb_operand *name = &operands[0], *value = &operands[1];
	int registration_result;
	zend_constant c;
	const char *name_str = Z_STRVAL_P(name->constant);
	uint32_t name_len = Z_STRLEN_P(name->constant);

	if(value->type == QB_OPERAND_ZVAL) {
		c.value = *value->constant;
		zval_copy_ctor(&c.value);
	} else if(value->type == QB_OPERAND_ADDRESS) {
		if(value->address->type >= QB_TYPE_F32) {
			double dval;
			switch(value->address->type) {
				case QB_TYPE_F32: dval = VALUE(F32, value->address); break;
				case QB_TYPE_F64: dval = VALUE(F64, value->address); break;
				default: break;
			}
			ZVAL_DOUBLE(&c.value, dval);
		} else {
			long lval;
			switch(value->address->type) {
				case QB_TYPE_S08: lval = VALUE(S08, value->address); break;
				case QB_TYPE_U08: lval = VALUE(U08, value->address); break;
				case QB_TYPE_S16: lval = VALUE(S16, value->address); break;
				case QB_TYPE_U16: lval = VALUE(U16, value->address); break;
				case QB_TYPE_S32: lval = VALUE(S32, value->address); break;
				case QB_TYPE_U32: lval = VALUE(U32, value->address); break;
				case QB_TYPE_S64: lval = (long) VALUE(S64, value->address); break;
				case QB_TYPE_U64: lval = (long) VALUE(U64, value->address); break;
				default: break;
			}
			ZVAL_LONG(&c.value, lval);
		}
	}
	c.flags = CONST_CS;
	c.name = zend_strndup(name_str, name_len);
	c.name_len = name_len + 1;
	c.module_number = PHP_USER_CONSTANT;
	registration_result = zend_register_constant(&c TSRMLS_CC);
	result->address = (registration_result == SUCCESS) ? cxt->true_address : cxt->false_address;
	result->type = QB_OPERAND_ADDRESS;
}

static void qb_set_result_function_call(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_result_prototype *result_prototype) {
	qb_operand *func = &operands[0];
	qb_function *qfunc = qb_find_compiled_function(func->zend_function);
	if(qfunc->return_variable->address) {
		qb_address *src_address = qfunc->return_variable->address;
		qb_variable_dimensions _dim, *dim = &_dim;

		// address of qfunc->return_variable is in a different storage object
		// so we can't use qb_copy_address_dimensions() here
		dim->dimension_count = src_address->dimension_count;	
		if(dim->dimension_count > 0) {
			qb_storage *src_storage = qfunc->local_storage;
			uint32_t i;
			for(i = 0; i < src_address->dimension_count; i++) {
				qb_address *dimension_address = src_address->dimension_addresses[i];
				if(CONSTANT(dimension_address)) {
					uint32_t dimension = VALUE_IN(src_storage, U32, dimension_address);
					dim->dimension_addresses[i] = qb_obtain_constant_U32(cxt, dimension);
				} else {
					dim->dimension_addresses[i] = qb_create_writable_scalar(cxt, QB_TYPE_U32);
				}
			}
			for(i = dim->dimension_count - 1; (int32_t) i >= 0; i--) {
				if(i == dim->dimension_count - 1) {
					dim->array_size_addresses[i] = dim->dimension_addresses[i];
				} else {
					dim->array_size_addresses[i] = qb_obtain_on_demand_product(cxt, dim->dimension_addresses[i], dim->dimension_addresses[i + 1]);
				}
			}
		} else {
			dim->array_size_addresses[0] = dim->dimension_addresses[0] = cxt->one_address;
		}
		dim->array_size_address = dim->array_size_addresses[0];
		
		// no resizing here--the callee will perform that if necessary
		result->address = qb_obtain_write_target(cxt, expr_type, dim, f->address_flags, result_prototype, FALSE);
	} else {
		result->address = cxt->zero_address;
	}
	result->type = QB_OPERAND_ADDRESS;
}

static void qb_set_result_zend_function_call(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_result_prototype *result_prototype) {
	if(result_prototype->destination) {
		qb_address *destination_address = qb_obtain_result_destination_address(cxt, result_prototype->destination);
		int32_t direct_assignment = FALSE;
		if(destination_address->mode == QB_ADDRESS_MODE_SCA) {
			direct_assignment = TRUE;
		} else if(destination_address->mode == QB_ADDRESS_MODE_ARR && !destination_address->source_address) {
			direct_assignment = TRUE;
		}
		if(direct_assignment) {
			if(destination_address) {
				result->address = destination_address;
			}
		} else {
			// use a temporary variable
			qb_variable_dimensions dim;
			qb_copy_address_dimensions(cxt, destination_address, 0, &dim);
			result->address = qb_obtain_temporary_variable(cxt, expr_type, &dim);
		}
		result->type = QB_OPERAND_ADDRESS;
	}
}