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
	result->address = qb_obtain_write_target(cxt, expr_type, &dim, result_prototype);
}

static void qb_set_result_non_reusable_temporary_value(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_result_prototype *result_prototype) {
	if(result->type != QB_OPERAND_ADDRESS) {
		qb_variable_dimensions dim = { NULL, 0, cxt->one_address };
		if(f->set_dimensions) {
			f->set_dimensions(cxt, f, operands, operand_count, &dim);
		}
		result->type = QB_OPERAND_ADDRESS;
		result->address = qb_create_temporary_variable(cxt, expr_type, &dim);
		result->address->flags |= QB_ADDRESS_NON_REUSABLE;
	}
}

static void qb_set_result_first_operand(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_result_prototype *result_prototype) {
	*result = operands[0];
}

static void qb_set_result_second_operand(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_result_prototype *result_prototype) {
	*result = operands[1];
}

static void qb_set_result_true(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_result_prototype *result_prototype) {
	result->type = QB_OPERAND_ADDRESS;
	result->address = cxt->true_address;
}

static void qb_set_result_none(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_result_prototype *result_prototype) {
	result->type = QB_OPERAND_NONE;
}

static qb_address *qb_find_predicate_address(qb_compiler_context *cxt, qb_address *container_address) {
	while(container_address) {
		if(container_address->array_index_address) {
			if(container_address->array_index_address->expression) {
				qb_expression *expr = container_address->array_index_address->expression;
				if(expr->op_factory == &factory_check_array_index_add || expr->op_factory == &factory_check_array_index_multiply || expr->op_factory == &factory_check_array_index_multiply_add) {
					return expr->operands[2].address;
				}
			}
		}
		container_address = container_address->source_address;
	}
	return NULL;
}

static void qb_set_result_check_array_index(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_result_prototype *result_prototype) {
	qb_operand *container = &operands[0], *predicate = &operands[2];

	if(predicate->type == QB_OPERAND_EMPTY) {
		// see if there's a check done on higher dimension 
		// use the same predicate so that if one check fails, the whole chain fails
		qb_address *predicate_address = qb_find_predicate_address(cxt, container->address);
		if(!predicate_address) {
			// allocate a new variable
			predicate_address = qb_create_temporary_variable(cxt, QB_TYPE_I32, NULL);
			predicate_address->flags |= QB_ADDRESS_NON_REUSABLE;
		}
		predicate->address = predicate_address;
		predicate->type = QB_OPERAND_ADDRESS;
	}

	qb_set_result_non_reusable_temporary_value(cxt, f, expr_type, operands, operand_count, result, result_prototype);
}

static void qb_set_result_assign(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_result_prototype *result_prototype) {
	qb_operand *variable = &operands[0], *value = &operands[1];

	// if the expression type was set to void, then an earlier op has used the r-value as write target
	// so there's no need to perform the assignment
	if(expr_type != QB_TYPE_VOID) {
		qb_address *result_address = variable->address;
		result->address = qb_obtain_bound_checked_address(cxt, value->address->array_size_address, result_address);
		result->type = QB_OPERAND_ADDRESS;
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
		result->address = qb_obtain_bound_checked_address(cxt, value->address->array_size_address, result_address);
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
		qb_address *result_address;
		if(container->type == QB_OPERAND_NONE) {
			result_address = qb_obtain_instance_variable(cxt, name->constant);
		} else if(container->type == QB_OPERAND_ADDRESS) {
			result_address = qb_obtain_named_element(cxt, container->address, name->constant, QB_ARRAY_BOUND_CHECK_WRITE);
		}
		result->address = qb_obtain_bound_checked_address(cxt, value->address->array_size_address, result_address);
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

static void qb_set_result_increment(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_result_prototype *result_prototype) {
	// set result to a temporary variable unless it'll end up just getting freed
	if(!result_prototype->destination || result_prototype->destination->type != QB_RESULT_DESTINATION_FREE) {
		// use the destination's type
		qb_primitive_type destination_type = qb_get_result_destination_type(cxt, result_prototype->destination);
		if(destination_type != QB_TYPE_ANY) {
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
	if(result->type != QB_OPERAND_ADDRESS) {
		qb_variable_dimensions dim = { NULL, 1, NULL };
		qb_address *address = qb_create_temporary_variable(cxt, QB_TYPE_U08, &dim);
		address->flags |= QB_ADDRESS_NON_REUSABLE | QB_ADDRESS_STRING;
		result->address = address;
		result->type = QB_OPERAND_ADDRESS;
	}
}

static void qb_set_result_add_string(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_result_prototype *result_prototype) {
	qb_operand *string = &operands[0], *addend = &operands[1];
	if(string->type == QB_OPERAND_ADDRESS) {
		*result = *string;
	} else {
		qb_variable_dimensions dim = { NULL, 1, NULL };
		string->address = qb_obtain_on_demand_value(cxt, &factory_empty_string, NULL, 0);
		string->type = QB_OPERAND_ADDRESS;
		result->address = string->address->expression->result->address;
		result->type = QB_OPERAND_ADDRESS;
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
	result->address = qb_obtain_array_element(cxt, container->address, cxt->foreach_index.address, FALSE);
}

static void qb_set_result_fetch_class_self(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_result_prototype *result_prototype) {
	zend_class_entry *ce = cxt->zend_function->common.scope;
	result->type = QB_OPERAND_ZEND_CLASS;
	result->zend_class = ce;
}

static void qb_set_result_fetch_class_parent(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_result_prototype *result_prototype) {
	zend_class_entry *ce = cxt->zend_function->common.scope;
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
		qvar->default_value_address = qb_obtain_constant_zval(cxt, default_value->constant, qvar->address->type);
	}
}

static void qb_set_result_send_argument(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_result_prototype *result_prototype) {
	qb_operand *argument = &operands[0];
	qb_operand *stack_item = qb_push_stack_item(cxt);
	*stack_item = *argument;
}

static void qb_set_preliminary_result_intrinsic(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_result_prototype *result_prototype) {
	qb_operand *func = &operands[0], *arguments = &operands[1], *argument_count = &operands[2];
	f = func->intrinsic_function->extra;
	f->set_preliminary_result(cxt, f, expr_type, arguments->arguments, argument_count->number, result, result_prototype);
}

static void qb_set_final_result_intrinsic(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_result_prototype *result_prototype) {
	qb_operand *func = &operands[0], *arguments = &operands[1], *argument_count = &operands[2];
	f = func->intrinsic_function->extra;
	if(f->set_final_result) {
		f->set_final_result(cxt, f, expr_type, arguments->arguments, argument_count->number, result, result_prototype);
	}
}