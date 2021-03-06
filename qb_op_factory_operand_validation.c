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

static int32_t qb_validate_operands_real_or_complex(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, uint32_t flags, qb_operand *operands, uint32_t operand_count, qb_result_destination *result_destination) {
	f = qb_select_complex_op_factory(f, flags);
	return (f->validate_operands) ? f->validate_operands(cxt, f, expr_type, flags, operands, operand_count, result_destination) : TRUE;
}

static int32_t qb_validate_operands_multiply(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, uint32_t flags, qb_operand *operands, uint32_t operand_count, qb_result_destination *result_destination) {
	f = qb_select_multiply_factory(f, flags);
	return (f->validate_operands) ? f->validate_operands(cxt, f, expr_type, flags, operands, operand_count, result_destination) : TRUE;
}

static int32_t qb_validate_operands_array_element(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, uint32_t flags, qb_operand *operands, uint32_t operand_count, qb_result_destination *result_destination) {
	qb_operand *container = &operands[0];
	qb_operand *index = &operands[1];

	if(container->type != QB_OPERAND_ADDRESS || IS_SCALAR(container->address)) {
		qb_report_not_an_array_exception(cxt->line_id);
		return FALSE;
	}

	if((index->type == QB_OPERAND_ADDRESS && !IS_SCALAR(index->address)) || (index->type == QB_OPERAND_ARRAY_INITIALIZER)) {
		qb_report_illegal_array_index_exception(cxt->line_id);
		return FALSE;
	} else if(index->type == QB_OPERAND_ZVAL) {
		if(index->constant->type == IS_STRING) {
			qb_report_associative_array_exception(cxt->line_id);
			return FALSE;
		} else if(index->constant->type != IS_BOOL && index->constant->type != IS_LONG && index->constant->type != IS_DOUBLE) {
			qb_report_illegal_array_index_exception(cxt->line_id);
			return FALSE;
		}
	} else if(index->type == QB_OPERAND_NONE) {
		// an append operation
		if(!IS_RESIZABLE(container->address)) {
			qb_report_fixed_length_array_exception(cxt->line_id);
			return FALSE;
		}
	}
	return TRUE;
}

static int32_t qb_validate_operands_object_property(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, uint32_t flags, qb_operand *operands, uint32_t operand_count, qb_result_destination *result_destination) {
	qb_operand *container = &operands[0];
	qb_operand *name = &operands[1];

	if(name->type != QB_OPERAND_ZVAL) {
		qb_report_variable_variable_exception(cxt->line_id);
		return FALSE;
	}
	if(container->type == QB_OPERAND_NONE) {
		if(!qb_get_instance_variable(cxt, name->constant)) {
			zend_class_entry *scope = cxt->zend_op_array->scope;
			qb_report_missing_property_exception(cxt->line_id, scope, Z_STRVAL_P(name->constant));
			return FALSE;
		}
	} else if(container->type == QB_OPERAND_ADDRESS) {
		uint32_t index, swizzle_mask;
		if(IS_SCALAR(container->address)) {
			qb_report_not_an_array_exception(cxt->line_id);
			return FALSE;
		}
		if(container->address->dimension_count == 1 && ((container->address->flags & QB_ADDRESS_VECTOR) || (container->address->flags & QB_ADDRESS_COMPLEX))) {
			index = qb_find_vector_index_alias(cxt, container->address, name->constant);
			if(index == INVALID_INDEX) {
				swizzle_mask = qb_get_vector_swizzle_mask(cxt, container->address, name->constant);
				if(swizzle_mask == INVALID_INDEX) {
					qb_report_missing_named_element_exception(cxt->line_id, Z_STRVAL_P(name->constant));
					return FALSE;
				}
			}
		} else {
			if(!container->address->index_alias_schemes || !container->address->index_alias_schemes[0]) {
				qb_report_elements_not_named_exception(cxt->line_id);
				return FALSE;
			}
			index = qb_find_index_alias(cxt, container->address->index_alias_schemes[0], name->constant);
			if(index == INVALID_INDEX) {
				swizzle_mask = (container->address->dimension_count == 1) ? qb_get_swizzle_mask(cxt, container->address->index_alias_schemes[0], name->constant) : INVALID_INDEX;
				if(swizzle_mask == INVALID_INDEX) {
					qb_report_missing_named_element_exception(cxt->line_id, Z_STRVAL_P(name->constant));
					return FALSE;
				}
			}
		}
	}
	return TRUE;
}

static int32_t qb_validate_operands_matching_type(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, uint32_t flags, qb_operand *operands, uint32_t operand_count, qb_result_destination *result_destination) {
	uint32_t i;
	for(i = 1; i < operand_count; i++) {
		if(operands[0].address->type != operands[i].address->type) {
			qb_report_type_mismatch_exception(cxt->line_id, operands[0].address->type, operands[i].address->type);
			return FALSE;
		}
	}
	return TRUE;
}

static int32_t qb_validate_operands_assign_return_value(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, uint32_t flags, qb_operand *operands, uint32_t operand_count, qb_result_destination *result_destination) {
	if(!cxt->return_variable->address) {
		qb_operand *value = &operands[0];
		if(value->type != QB_OPERAND_NONE && !(value->type == QB_OPERAND_ZVAL && value->constant->type == IS_NULL)) {
			qb_report_return_void_exception(cxt->line_id);
			return FALSE;
		}
	}
	return TRUE;
}

static int32_t qb_validate_operands_sent_value(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, uint32_t flags, qb_operand *operands, uint32_t operand_count, qb_result_destination *result_destination) {
	if(!cxt->sent_variable->address) {
		if(result_destination && result_destination->type != QB_RESULT_DESTINATION_FREE) {
			qb_report_missing_send_declaration_exception(cxt->line_id);
			return FALSE;
		}
	}
	return TRUE;
}

static int32_t qb_validate_operands_rand(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, uint32_t flags, qb_operand *operands, uint32_t operand_count, qb_result_destination *result_destination) {
	if(operand_count != 0 && operand_count != 2) {
		qb_report_incorrect_argument_count_exception(cxt->line_id, cxt->intrinsic_function, operand_count);
		return FALSE;
	}
	return TRUE;
}

static int32_t qb_validate_operands_minmax_array(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, uint32_t flags, qb_operand *operands, uint32_t operand_count, qb_result_destination *result_destination) {
	qb_operand *container = &operands[0];

	if(operand_count == 1) {
		if(IS_SCALAR(container->address)) {
			qb_report_unmet_intrinsic_condition_exception(cxt->line_id, cxt->intrinsic_function, "an array as parameter when only one parameter is given");
			return FALSE;
		}
	}
	return TRUE;
}

static int32_t qb_validate_array_append(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, uint32_t flags, qb_operand *operands, uint32_t operand_count, qb_result_destination *result_destination) {
	qb_operand *index = &operands[1];

	if(index->type == QB_OPERAND_ZVAL) {
		switch(Z_TYPE_P(index->constant)) {
			case IS_BOOL:
			case IS_LONG:
			case IS_DOUBLE: {
			}	break;
			case IS_STRING: {
			}	break;
		}
	}
	return TRUE;
}

static int32_t qb_validate_array_init(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, uint32_t flags, qb_operand *operands, uint32_t operand_count, qb_result_destination *result_destination) {
	if(operand_count > 0) {
		return qb_validate_array_append(cxt, f, expr_type, flags, operands, operand_count, result_destination);
	}
	return TRUE;
}

static zval * qb_get_special_constant(qb_compiler_context *cxt, const char *name, uint32_t length) {
	static zval type_constants[QB_TYPE_COUNT];
	static zval qb_indicator;

	if(strcmp(name, "__QB__") == 0) {
		Z_TYPE(qb_indicator) = IS_LONG;
		Z_LVAL(qb_indicator) = 1;
		return &qb_indicator;
	} else {
		uint32_t i;
		for(i = 0; i < QB_TYPE_COUNT; i++) {
			const char *type = type_names[i];
			if(strcmp(name, type) == 0) {
				Z_TYPE(type_constants[i]) = IS_LONG;
				Z_LVAL(type_constants[i]) = i;
				return &type_constants[i];
			}
		}
	}
	return NULL;
}

static int32_t qb_validate_operands_fetch_class_self(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, uint32_t flags, qb_operand *operands, uint32_t operand_count, qb_result_destination *result_destination) {
	zend_class_entry *ce = cxt->zend_op_array->scope;
	if(!ce) {
		qb_report_missing_scope_exception(cxt->line_id, "self");
		return FALSE;
	}
	return TRUE;
}

static int32_t qb_validate_operands_fetch_class_parent(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, uint32_t flags, qb_operand *operands, uint32_t operand_count, qb_result_destination *result_destination) {
	zend_class_entry *ce = cxt->zend_op_array->scope;
	if(!ce) {
		qb_report_missing_scope_exception(cxt->line_id, "parent");
		return FALSE;
	}
	if(!ce->parent) {
		qb_report_missing_parent_scope_exception(cxt->line_id, "parent");
		return FALSE;
	}
	return TRUE;
}

static int32_t qb_validate_operands_fetch_class_static(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, uint32_t flags, qb_operand *operands, uint32_t operand_count, qb_result_destination *result_destination) {
	zend_class_entry *ce = cxt->zend_op_array->scope;
	if(!ce) {
		qb_report_missing_scope_exception(cxt->line_id, "static");
		return FALSE;
	}
	return TRUE;
}

static int32_t qb_validate_operands_fetch_class_global(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, uint32_t flags, qb_operand *operands, uint32_t operand_count, qb_result_destination *result_destination) {
	USE_TSRM
	qb_operand *name = &operands[1];
	zend_class_entry *ce = zend_fetch_class_by_name(Z_STRVAL_P(name->constant), Z_STRLEN_P(name->constant), NULL, ZEND_FETCH_CLASS_SILENT TSRMLS_CC);
	if(!ce) {
		qb_report_missing_class_exception(cxt->line_id, Z_STRVAL_P(name->constant));
		return FALSE;
	}
	return TRUE;
}

static int32_t qb_validate_operands_fetch_constant(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, uint32_t flags, qb_operand *operands, uint32_t operand_count, qb_result_destination *result_destination) {
	USE_TSRM
	qb_operand *scope = &operands[0], *name = &operands[1];
	if(scope->type == QB_OPERAND_ZEND_STATIC_CLASS) {
		// validation at runtime
	} else {
		zend_class_entry *ce = NULL;
		HashTable *ht;
		const char *name_str = Z_STRVAL_P(name->constant);
		uint32_t name_len = Z_STRLEN_P(name->constant);
		if(scope->type == QB_OPERAND_ZEND_CLASS) {
			ce = scope->zend_class;
		} else if(scope->type == QB_OPERAND_ZVAL) {
			if(Z_TYPE_P(scope->constant) == IS_STRING) {
				ce = zend_fetch_class_by_name(Z_STRVAL_P(scope->constant), Z_STRLEN_P(scope->constant), NULL, 0 TSRMLS_CC);
			} else {
				ce = cxt->zend_op_array->scope;
			}
		}
		if(ce) {
			ht = &ce->constants_table;
		} else {
			ht = EG(zend_constants);
		}
		if(zend_hash_exists(ht, name_str, name_len + 1)) {
			return TRUE;
		}
		// see if the name has a namespace attached
		if(qb_strip_namespace(&name_str, &name_len)) {
			if(zend_hash_exists(ht, name_str, name_len + 1)) {
				return TRUE;
			}
		}
		// see if it's a special QB constant
		if(qb_get_special_constant(cxt, name_str, name_len)) {
			return TRUE;
		} else {
			qb_report_undefined_constant_exception(cxt->line_id, ce, Z_STRVAL_P(name->constant));
		}
	}
	return FALSE;
}

static int32_t qb_validate_operands_assign_ref(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, uint32_t flags, qb_operand *operands, uint32_t operand_count, qb_result_destination *result_destination) {
	qb_operand *variable = &operands[0], *value = &operands[1];
	// global and static assign ref to bring variables into the local scope
	if(variable->address != value->address) {
		qb_report_reference_exception(cxt->line_id);
		return FALSE;
	}
	return TRUE;
}

static int32_t qb_validate_operands_one_array(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, uint32_t flags, qb_operand *operands, uint32_t operand_count, qb_result_destination *result_destination) {
	qb_operand *operand = &operands[0];

	if(IS_SCALAR(operand->address)) {
		qb_report_unexpected_intrinsic_argument_exception(cxt->line_id, cxt->intrinsic_function, 0, "array");
		return FALSE;
	}
	return TRUE;
}

static int32_t qb_validate_operands_referenceable(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, uint32_t flags, qb_operand *operands, uint32_t operand_count, qb_result_destination *result_destination) {
	qb_operand *operand = &operands[0];

	if(operand->type != QB_OPERAND_ADDRESS) {
		qb_report_unexpected_value_as_intrinsic_argument_exception(cxt->line_id, cxt->intrinsic_function, 0);
	}
	return TRUE;
}

static int32_t qb_validate_operands_one_array_variable(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, uint32_t flags, qb_operand *operands, uint32_t operand_count, qb_result_destination *result_destination) {
	if(!qb_validate_operands_referenceable(cxt, f, expr_type, flags, operands, operand_count, result_destination)) {
		return FALSE;
	}
	if(!qb_validate_operands_one_array(cxt, f, expr_type, flags, operands, operand_count, result_destination)) {
		return FALSE;
	}
	return TRUE;
}

static int32_t qb_validate_operands_two_arrays(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, uint32_t flags, qb_operand *operands, uint32_t operand_count, qb_result_destination *result_destination) {
	qb_operand *operand1 = &operands[0], *operand2 = &operands[1];

	if(IS_SCALAR(operand1->address)) {
		qb_report_unexpected_intrinsic_argument_exception(cxt->line_id, cxt->intrinsic_function, 0, "array");
		return FALSE;
	}
	if(IS_SCALAR(operand2->address)) {
		qb_report_unexpected_intrinsic_argument_exception(cxt->line_id, cxt->intrinsic_function, 1, "array");
		return FALSE;
	}
	return TRUE;
}

static int32_t qb_validate_operands_matching_vector_width(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, uint32_t flags, qb_operand *operands, uint32_t operand_count, qb_result_destination *result_destination) {
	qb_operand *operand1 = &operands[0], *operand2 = &operands[1];
	qb_address *v1_width_address = DIMENSION_ADDRESS(operand1->address, -1);
	qb_address *v2_width_address = DIMENSION_ADDRESS(operand2->address, -1);

	if(!qb_validate_operands_two_arrays(cxt, f, expr_type, flags, operands, operand_count, result_destination)) {
		return FALSE;
	}

	if(IS_IMMUTABLE(v1_width_address) && IS_IMMUTABLE(v2_width_address)) {
		uint32_t v1_width = VALUE(U32, v1_width_address);
		uint32_t v2_width = VALUE(U32, v2_width_address);
		if(v1_width != v2_width) {
			qb_report_vector_width_mismatch_exception(cxt->line_id, v1_width, v2_width);
			return FALSE;
		}
	} else {
		qb_operand operands[2] = { { QB_OPERAND_ADDRESS, { v1_width_address } }, { QB_OPERAND_ADDRESS, { v2_width_address } } };
		qb_produce_op(cxt, &factory_validate_vector_width, operands, 2, NULL, NULL, 0, NULL);
	}
	return TRUE;
}

static int32_t qb_validate_operands_refract(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, uint32_t flags, qb_operand *operands, uint32_t operand_count, qb_result_destination *result_destination) {
	qb_operand *operand3 = &operands[2];

	qb_validate_operands_matching_vector_width(cxt, f, expr_type, flags, operands, operand_count, result_destination);

	if(!IS_SCALAR(operand3->address)) {
		qb_report_unexpected_intrinsic_argument_exception(cxt->line_id, cxt->intrinsic_function, 2, "scalar");
		return FALSE;
	}
	return TRUE;
}

static int32_t qb_validate_operands_cross_product(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, uint32_t flags, qb_operand *operands, uint32_t operand_count, qb_result_destination *result_destination) {
	uint32_t i;

	for(i = 0; i < operand_count; i++) {
		qb_operand *operand = &operands[0];
		if(IS_SCALAR(operand->address)) {
			qb_report_unexpected_intrinsic_argument_exception(cxt->line_id, cxt->intrinsic_function, i, "array");
			return FALSE;
		}
	}

	if(operand_count == 3) {
		qb_operand *operand1 = &operands[0], *operand2 = &operands[1], *operand3 = &operands[2];
		qb_address *v1_width_address = DIMENSION_ADDRESS(operand1->address, -1);
		qb_address *v2_width_address = DIMENSION_ADDRESS(operand2->address, -1);
		qb_address *v3_width_address = DIMENSION_ADDRESS(operand3->address, -1);
		if(IS_IMMUTABLE(v1_width_address) && IS_IMMUTABLE(v2_width_address) && IS_IMMUTABLE(v3_width_address)) {
			uint32_t v1_width = VALUE(U32, v1_width_address);
			uint32_t v2_width = VALUE(U32, v2_width_address);
			uint32_t v3_width = VALUE(U32, v3_width_address);

			if(!(v1_width == v2_width && v2_width == v3_width && v1_width == 4)) {
				qb_report_invalid_4d_cross_product_exception(cxt->line_id, v1_width, v2_width, v3_width);
				return FALSE;
			}
		} else {
			qb_operand operands[3] = { { QB_OPERAND_ADDRESS, { v1_width_address } }, { QB_OPERAND_ADDRESS, { v2_width_address } }, { QB_OPERAND_ADDRESS, { v3_width_address } } };
			qb_produce_op(cxt, &factory_validate_cross_product_4d, operands, 3, NULL, NULL, 0, NULL);
		} 
	} else {
		qb_operand *operand1 = &operands[0], *operand2 = &operands[1];
		qb_address *v1_width_address = DIMENSION_ADDRESS(operand1->address, -1);
		qb_address *v2_width_address = DIMENSION_ADDRESS(operand2->address, -1);
		if(IS_IMMUTABLE(v1_width_address) && IS_IMMUTABLE(v2_width_address)) {
			uint32_t v1_width = VALUE(U32, v1_width_address);
			uint32_t v2_width = VALUE(U32, v2_width_address);

			if(!(v1_width == v2_width && 2 <= v1_width && v2_width <= 3)) {
				qb_report_invalid_cross_product_exception(cxt->line_id, v1_width, v2_width);
				return FALSE;
			}
		} else {
			qb_operand operands[2] = { { QB_OPERAND_ADDRESS, { v1_width_address } }, { QB_OPERAND_ADDRESS, { v2_width_address } } };
			uint32_t width = 3;
			if(IS_IMMUTABLE(v1_width_address)) {
				width = VALUE(U32, v1_width_address);
			}
			if(IS_IMMUTABLE(v2_width_address)) {
				width = VALUE(U32, v2_width_address);
			}
			if(width == 2) {
				qb_produce_op(cxt, &factory_validate_cross_product_2d, operands, 2, NULL, NULL, 0, NULL);
			} else {
				qb_produce_op(cxt, &factory_validate_cross_product_3d, operands, 2, NULL, NULL, 0, NULL);
			}
		} 
	}
	return TRUE;
}

static int32_t qb_validate_operands_one_matrix(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, uint32_t flags, qb_operand *operands, uint32_t operand_count, qb_result_destination *result_destination) {
	qb_operand *operand1 = &operands[0];

	if(operand1->address->dimension_count < 2) {
		qb_report_unexpected_intrinsic_argument_exception(cxt->line_id, cxt->intrinsic_function, 0, "matrix");
		return FALSE;
	}
	return TRUE;
}

static qb_matrix_order qb_get_matrix_order(qb_compiler_context *cxt, qb_op_factory *f) {
	if(f->result_flags & QB_RESULT_IS_COLUMN_MAJOR) {
		return QB_MATRIX_ORDER_COLUMN_MAJOR;
	} else if(f->result_flags & QB_RESULT_IS_ROW_MAJOR) {
		return QB_MATRIX_ORDER_ROW_MAJOR;
	} else {
		USE_TSRM
		if(QB_G(column_major_matrix)) { 
			return QB_MATRIX_ORDER_COLUMN_MAJOR;
		} else {
			return QB_MATRIX_ORDER_ROW_MAJOR;
		}
	}
}

static qb_address *qb_obtain_matrix_row_address(qb_compiler_context *cxt, qb_address *address, qb_matrix_order order) {
	int32_t row_offset = (order == QB_MATRIX_ORDER_ROW_MAJOR) ? -2 : -1;
	return address->dimension_addresses[address->dimension_count + row_offset];
}

static qb_address *qb_obtain_matrix_column_address(qb_compiler_context *cxt, qb_address *address, qb_matrix_order order) {
	int32_t col_offset = (order == QB_MATRIX_ORDER_ROW_MAJOR) ? -1 : -2;
	return address->dimension_addresses[address->dimension_count + col_offset];
}

static int32_t qb_validate_operands_square_matrix(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, uint32_t flags, qb_operand *operands, uint32_t operand_count, qb_result_destination *result_destination) {
	qb_operand *matrix = &operands[0];
	qb_matrix_order order = qb_get_matrix_order(cxt, f);
	qb_address *m_col_address = qb_obtain_matrix_column_address(cxt, matrix->address, order);
	qb_address *m_row_address = qb_obtain_matrix_row_address(cxt, matrix->address, order);

	qb_validate_operands_one_matrix(cxt, f, expr_type, flags, operands, operand_count, result_destination);

	if(IS_IMMUTABLE(m_col_address) && IS_IMMUTABLE(m_row_address)) {
		uint32_t row = VALUE(U32, m_col_address);
		uint32_t col = VALUE(U32, m_row_address);
		if(row != col) {
			qb_report_not_square_matrix_exception(cxt->line_id, col, row);
			return FALSE;
		}
	} else {
		qb_operand operands[2] = { { QB_OPERAND_ADDRESS, { m_col_address } }, { QB_OPERAND_ADDRESS, { m_row_address } } };
		qb_produce_op(cxt, &factory_validate_square_matrix, operands, 2, NULL, NULL, 0, NULL);
	}
	return TRUE;
}

static int32_t qb_validate_operands_pixel(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, uint32_t flags, qb_operand *operands, uint32_t operand_count, qb_result_destination *result_destination) {
	qb_operand *operand1 = &operands[0];

	if(HAS_CONSTANT_DIMENSION(operand1->address, -1)) {
		uint32_t channel_count = DIMENSION(operand1->address, -1);
		if(!(3 <= channel_count && channel_count <= 4)) {
			qb_report_invalid_pixel_format_exception(cxt->line_id, cxt->intrinsic_function, channel_count);
			return FALSE;
		}
	} else {
		qb_report_variable_pixel_width_exception(cxt->line_id, cxt->intrinsic_function);
		return FALSE;
	}
	return TRUE;
}

static int32_t qb_validate_operands_rgba(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, uint32_t flags, qb_operand *operands, uint32_t operand_count, qb_result_destination *result_destination) {
	qb_operand *operand1 = &operands[0];

	if(HAS_CONSTANT_DIMENSION(operand1->address, -1)) {
		uint32_t channel_count = DIMENSION(operand1->address, -1);
		if(channel_count != 4) {
			qb_report_missing_alpha_channel_exception(cxt->line_id, cxt->intrinsic_function, channel_count);
			return FALSE;
		}
	} else {
		qb_report_variable_pixel_width_exception(cxt->line_id, cxt->intrinsic_function);
		return FALSE;
	}
	return TRUE;
}

static int32_t qb_validate_operands_blend(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, uint32_t flags, qb_operand *operands, uint32_t operand_count, qb_result_destination *result_destination) {
	qb_operand *operand1 = &operands[0];

	if(HAS_CONSTANT_DIMENSION(operand1->address, -1)) {
		uint32_t channel_count = DIMENSION(operand1->address, -1);
		if(channel_count != 2 && channel_count != 4) {
			qb_report_missing_alpha_channel_exception(cxt->line_id, cxt->intrinsic_function, channel_count);
			return FALSE;
		}
	} else {
		qb_report_variable_pixel_width_exception(cxt->line_id, cxt->intrinsic_function);
		return FALSE;
	}
	return TRUE;
}

static int32_t qb_validate_operands_sampling(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, uint32_t flags, qb_operand *operands, uint32_t operand_count, qb_result_destination *result_destination) {
	qb_operand *image = &operands[0];
	uint32_t channel_count = 0;

	if(image->address->dimension_count != 3) {
		qb_report_unexpected_intrinsic_argument_exception(cxt->line_id, cxt->intrinsic_function, 0, "three-dimensional array");
	}
	if(HAS_CONSTANT_DIMENSION(image->address, -1)) {
		channel_count = DIMENSION(image->address, -1);
	}
	if(!(1 <= channel_count && channel_count <= 4)) {
		qb_report_unexpected_intrinsic_argument_exception(cxt->line_id, cxt->intrinsic_function, 0, "array whose last dimension is between 1 and 4");
		return FALSE;
	}
	if(!(image->address->type >= QB_TYPE_F32)) {
		qb_report_unmet_intrinsic_condition_exception(cxt->line_id, cxt->intrinsic_function, "array in floating-point representation");
		return FALSE;
	}
	return TRUE;
}

static int32_t qb_validate_operands_sample_convolution(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, uint32_t flags, qb_operand *operands, uint32_t operand_count, qb_result_destination *result_destination) {
	qb_operand *matrix = &operands[3], *divider = &operands[4], *offset = &operands[5];

	if(!qb_validate_operands_sampling(cxt, f, expr_type, flags, operands, operand_count, result_destination)) {
		return FALSE;
	}
	if(matrix->address->dimension_count != 2) {
		qb_report_unexpected_intrinsic_argument_exception(cxt->line_id, cxt->intrinsic_function, 3, "2x2 matrix");
		return FALSE;
	}
	if(divider->type == QB_OPERAND_ADDRESS && !IS_SCALAR(divider->address)) {
		qb_report_unexpected_intrinsic_argument_exception(cxt->line_id, cxt->intrinsic_function, 4, "scalar");
		return FALSE;
	}
	if(offset->type == QB_OPERAND_ADDRESS && !IS_SCALAR(offset->address)) {
		qb_report_unexpected_intrinsic_argument_exception(cxt->line_id, cxt->intrinsic_function, 5, "scalar");
		return FALSE;
	}
	return TRUE;
}

static int32_t qb_validate_operands_multidimensional_array(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, uint32_t flags, qb_operand *operands, uint32_t operand_count, qb_result_destination *result_destination) {
	qb_operand *container = &operands[0], *column_index = &operands[1];
	if(container->address->dimension_count < 2) {
		qb_report_unexpected_intrinsic_argument_exception(cxt->line_id, cxt->intrinsic_function, 0, "multidimensional array");
		return FALSE;
	}
	if(!IS_SCALAR(column_index->address)) {
		qb_report_unexpected_intrinsic_argument_exception(cxt->line_id, cxt->intrinsic_function, 1, "scalar");
		return FALSE;
	}
	return TRUE;
}

static int32_t qb_validate_operands_mm_mult(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, uint32_t flags, qb_operand *operands, uint32_t operand_count, qb_result_destination *result_destination) {
	qb_operand *matrix1 = &operands[0], *matrix2 = &operands[1];
	qb_matrix_order order = qb_get_matrix_order(cxt, f);
	qb_address *m1_col_address = qb_obtain_matrix_column_address(cxt, matrix1->address, order);
	qb_address *m2_row_address = qb_obtain_matrix_row_address(cxt, matrix2->address, order);

	if(matrix1->address->dimension_count < 2) {
		qb_report_unexpected_intrinsic_argument_exception(cxt->line_id, cxt->intrinsic_function, 0, "matrix");
		return FALSE;
	}
	if(matrix2->address->dimension_count < 2) {
		qb_report_unexpected_intrinsic_argument_exception(cxt->line_id, cxt->intrinsic_function, 1, "matrix");
		return FALSE;
	}

	if(m1_col_address != m2_row_address) {
		if(IS_IMMUTABLE(m1_col_address) && IS_IMMUTABLE(m2_row_address)) {
			uint32_t m1_col_count = VALUE(U32, m1_col_address);
			uint32_t m2_row_count = VALUE(U32, m2_row_address);

			if(!cxt->matrix_padding) {
				if(m1_col_count != m2_row_count) {
					qb_report_invalid_matrix_multiplication_exception(cxt->line_id, m1_col_count, m2_row_count, 1 | 2);
					return FALSE;
				}
			}
		} else {
			qb_operand operands[2] = { { QB_OPERAND_ADDRESS, { m1_col_address } }, { QB_OPERAND_ADDRESS, { m2_row_address } } };
			qb_produce_op(cxt, &factory_validate_mm_mult, operands, 2, NULL, NULL, 0, NULL);
		}
	}
	return TRUE;
}

static int32_t qb_validate_operands_mv_mult(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, uint32_t flags, qb_operand *operands, uint32_t operand_count, qb_result_destination *result_destination) {
	qb_operand *matrix1 = &operands[0], *matrix2 = &operands[1];
	qb_matrix_order order = qb_get_matrix_order(cxt, f);
	qb_address *m1_col_address = qb_obtain_matrix_column_address(cxt, matrix1->address, order);
	qb_address *m2_row_address = DIMENSION_ADDRESS(matrix2->address, -1);

	if(matrix1->address->dimension_count < 2) {
		qb_report_unexpected_intrinsic_argument_exception(cxt->line_id, cxt->intrinsic_function, 0, "matrix");
		return FALSE;
	}
	if(matrix2->address->dimension_count < 1) {
		qb_report_unexpected_intrinsic_argument_exception(cxt->line_id, cxt->intrinsic_function, 1, "vector");
		return FALSE;
	}

	if(m1_col_address != m2_row_address) {
		if(IS_IMMUTABLE(m1_col_address) && IS_IMMUTABLE(m2_row_address)) {
			uint32_t m1_col_count = VALUE(U32, m1_col_address);
			uint32_t m2_row_count = VALUE(U32, m2_row_address);

			if(!cxt->matrix_padding) {
				if(m1_col_count != m2_row_count) {
					qb_report_invalid_matrix_multiplication_exception(cxt->line_id, m1_col_count, m2_row_count, 1);
					return FALSE;
				}
			}
		} else {
			qb_operand operands[2] = { { QB_OPERAND_ADDRESS, { m1_col_address } }, { QB_OPERAND_ADDRESS, { m2_row_address } } };
			qb_produce_op(cxt, &factory_validate_mv_mult, operands, 2, NULL, NULL, 0, NULL);
		}
	}
	return TRUE;
}

static int32_t qb_validate_operands_vm_mult(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, uint32_t flags, qb_operand *operands, uint32_t operand_count, qb_result_destination *result_destination) {
	qb_operand *matrix1 = &operands[0], *matrix2 = &operands[1];
	qb_matrix_order order = qb_get_matrix_order(cxt, f);
	qb_address *m1_col_address = DIMENSION_ADDRESS(matrix1->address, -1);
	qb_address *m2_row_address = qb_obtain_matrix_row_address(cxt, matrix2->address, order);

	if(matrix1->address->dimension_count < 1) {
		qb_report_unexpected_intrinsic_argument_exception(cxt->line_id, cxt->intrinsic_function, 0, "vector");
		return FALSE;
	}
	if(matrix2->address->dimension_count < 2) {
		qb_report_unexpected_intrinsic_argument_exception(cxt->line_id, cxt->intrinsic_function, 1, "matrix");
		return FALSE;
	}

	if(m1_col_address != m2_row_address) {
		if(IS_IMMUTABLE(m1_col_address) && IS_IMMUTABLE(m2_row_address)) {
			uint32_t m1_col_count = VALUE(U32, m1_col_address);
			uint32_t m2_row_count = VALUE(U32, m2_row_address);

			if(!cxt->matrix_padding) {
				if(m1_col_count != m2_row_count) {
					qb_report_invalid_matrix_multiplication_exception(cxt->line_id, m1_col_count, m2_row_count, 2);
					return FALSE;
				}
			}
		} else {
			qb_operand operands[2] = { { QB_OPERAND_ADDRESS, { m1_col_address } }, { QB_OPERAND_ADDRESS, { m2_row_address } } };
			qb_produce_op(cxt, &factory_validate_vm_mult, operands, 2, NULL, NULL, 0, NULL);
		}
	}
	return TRUE;
}

static int32_t qb_validate_operands_matrix_current_mode(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, uint32_t flags, qb_operand *operands, uint32_t operand_count, qb_result_destination *result_destination) {
	USE_TSRM
	qb_matrix_op_factory_selector *s = (qb_matrix_op_factory_selector *) f;
	if(QB_G(column_major_matrix)) {
		f = s->cm_factory;
	} else {
		f = s->rm_factory;
	}
	return f->validate_operands(cxt, f, expr_type, flags, operands, operand_count, result_destination);
}

static int32_t qb_validate_operands_transform(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, uint32_t flags, qb_operand *operands, uint32_t operand_count, qb_result_destination *result_destination) {
	qb_operand *matrix1 = &operands[0], *matrix2 = &operands[1];

	if(matrix1->address->dimension_count < 2) {
		qb_report_unexpected_intrinsic_argument_exception(cxt->line_id, cxt->intrinsic_function, 0, "matrix");
		return FALSE;
	}
	if(matrix2->address->dimension_count < 1) {
		qb_report_unexpected_intrinsic_argument_exception(cxt->line_id, cxt->intrinsic_function, 1, "vector");
		return FALSE;
	}

	if(HAS_CONSTANT_DIMENSION(matrix1->address, -2) && HAS_CONSTANT_DIMENSION(matrix1->address, -1) && HAS_CONSTANT_DIMENSION(matrix2->address, -1)) {
		qb_matrix_order order = qb_get_matrix_order(cxt, f);
		qb_address *m1_col_address = qb_obtain_matrix_column_address(cxt, matrix1->address, order);
		qb_address *m1_row_address = qb_obtain_matrix_row_address(cxt, matrix1->address, order);
		qb_address *m2_row_address = DIMENSION_ADDRESS(matrix2->address, -1);
		uint32_t m1_col_count = VALUE(U32, m1_col_address);
		uint32_t m1_row_count = VALUE(U32, m1_row_address);
		uint32_t m2_row_count = VALUE(U32, m2_row_address);

		if(!(m2_row_count >= 2 && m2_row_count <= 4 && m1_col_count == m2_row_count + 1 && m1_row_count == m2_row_count)) {
			qb_report_invalid_transform_exception(cxt->line_id, m1_col_count, m1_row_count, m2_row_count);
			return FALSE;
		}
	} else {
		qb_report_unexpected_intrinsic_argument_exception(cxt->line_id, cxt->intrinsic_function, 0, "matrix with fixed dimensions");
	}
	return TRUE;
}

static int32_t qb_validate_operands_array_push(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, uint32_t flags, qb_operand *operands, uint32_t operand_count, qb_result_destination *result_destination) {
	qb_operand *container = &operands[0];
	qb_address *container_element_size_address;
	uint32_t i;

	if(IS_TEMPORARY(container->address)) {
		qb_report_unexpected_value_as_intrinsic_argument_exception(cxt->line_id, cxt->intrinsic_function, 0);
		return FALSE;
	}
	if(IS_SCALAR(container->address)) {
		qb_report_unexpected_intrinsic_argument_exception(cxt->line_id, cxt->intrinsic_function, 0, "array");
		return FALSE;
	}
	if(IS_FIXED_LENGTH(container->address)) {
		qb_report_unexpected_intrinsic_argument_exception(cxt->line_id, cxt->intrinsic_function, 0, "variable-length array");
		return FALSE;
	}

	if(container->address->dimension_count > 1) {
		container_element_size_address = container->address->array_size_addresses[1];
	} else {
		container_element_size_address = cxt->one_address;
	}

	// make sure the elements being pushed in match the size of the elements in the container
	for(i = 1; i < operand_count; i++) {
		qb_address *address = operands[i].address;
		qb_address *element_size_address = address->array_size_address;

		if(container_element_size_address != element_size_address) {
			if(IS_IMMUTABLE(container_element_size_address) && IS_IMMUTABLE(element_size_address)) {
				uint32_t container_element_size = VALUE(U32, container_element_size_address);
				uint32_t element_size = VALUE(U32, element_size_address);
				if(container_element_size != element_size) {
					qb_report_unmet_intrinsic_condition_exception(cxt->line_id, cxt->intrinsic_function, "values of the same size as the elements in the container");
					return FALSE;
				}
			} else {
				qb_operand guard_operands[2] = { { QB_OPERAND_ADDRESS, { element_size_address } }, { QB_OPERAND_ADDRESS, { container_element_size_address } } };
				qb_operand guard_result = { QB_OPERAND_EMPTY, { NULL } };
				qb_produce_op(cxt, &factory_guard_array_size_exact, guard_operands, 2, &guard_result, NULL, 0, NULL);
			}
		}
	}
	return TRUE;
}

static int32_t qb_validate_operands_array_merge(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, uint32_t flags, qb_operand *operands, uint32_t operand_count, qb_result_destination *result_destination) {
	uint32_t i;
	uint32_t result_dimension_count = 1;
	qb_address *result_element_size_address = cxt->one_address;

	// the argument with the largest number of dimensions determines
	// how many there are in the result and the element size
	for(i = 0; i < operand_count; i++) {
		qb_address *address = operands[i].address;
		if(address->dimension_count > result_dimension_count) {
			result_dimension_count = address->dimension_count;
			result_element_size_address = address->array_size_addresses[1];
		}
	}

	// make sure the elements are of the same size
	for(i = 0; i < operand_count; i++) {
		qb_address *address = operands[i].address;
		qb_address *element_size_address;
		if(address->dimension_count == result_dimension_count) {
			element_size_address = (address->dimension_count > 1) ? address->array_size_addresses[1] : cxt->one_address;
		} else {
			// if the number of dimensions is less, then treat the whole array as the element
			element_size_address = address->array_size_address;
		}

		if(result_element_size_address != element_size_address) {
			if(IS_IMMUTABLE(result_element_size_address) && IS_IMMUTABLE(element_size_address)) {
				uint32_t result_element_size = VALUE(U32, result_element_size_address);
				uint32_t element_size = VALUE(U32, element_size_address);
				if(result_element_size != element_size) {
					qb_report_unmet_intrinsic_condition_exception(cxt->line_id, cxt->intrinsic_function, "arrays with elements with of the same size");
					return FALSE;
				}
			} else {
				qb_operand guard_operands[2] = { { QB_OPERAND_ADDRESS, { element_size_address } }, { QB_OPERAND_ADDRESS, { result_element_size_address } } };
				qb_operand guard_result = { QB_OPERAND_EMPTY, { NULL } };
				qb_produce_op(cxt, &factory_guard_array_size_exact, guard_operands, 2, &guard_result, NULL, 0, NULL);
			}
		}
	}
	return TRUE;
}

static int32_t qb_validate_operands_array_diff(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, uint32_t flags, qb_operand *operands, uint32_t operand_count, qb_result_destination *result_destination) {
	qb_operand *value = &operands[0];
	if(cxt->argument_offset == 0) {
		if(IS_SCALAR(value->address)) {
			qb_report_unexpected_intrinsic_argument_exception(cxt->line_id, cxt->intrinsic_function, 0, "array");
			return FALSE;
		} 
	}
	return TRUE;
}

static int32_t qb_validate_operands_array_fill(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, uint32_t flags, qb_operand *operands, uint32_t operand_count, qb_result_destination *result_destination) {
	qb_operand *start_index = &operands[0], *number = &operands[1];
	if(!IS_SCALAR(start_index->address)) {
		qb_report_unexpected_intrinsic_argument_exception(cxt->line_id, cxt->intrinsic_function, 0, "scalar");
		return FALSE;
	}
	if(!IS_SCALAR(number->address)) {
		qb_report_unexpected_intrinsic_argument_exception(cxt->line_id, cxt->intrinsic_function, 1, "scalar");
		return FALSE;
	}
	return TRUE;
}

static int32_t qb_validate_operands_array_pad(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, uint32_t flags, qb_operand *operands, uint32_t operand_count, qb_result_destination *result_destination) {
	qb_operand *input = &operands[0], *size = &operands[1], *value = &operands[2];

	if(IS_SCALAR(input->address)) {
		qb_report_unexpected_intrinsic_argument_exception(cxt->line_id, cxt->intrinsic_function, 0, "array");
		return FALSE;
	}
	if(!IS_SCALAR(size->address)) {
		qb_report_unexpected_intrinsic_argument_exception(cxt->line_id, cxt->intrinsic_function, 0, "scalar");
		return FALSE;
	}

	if(input->address->dimension_count > 1) {
		qb_address *element_size_address = input->address->array_size_addresses[1];
		qb_address *value_size_address = value->address->array_size_address;
		if(IS_IMMUTABLE(element_size_address) && IS_IMMUTABLE(value_size_address)) {
			uint32_t element_size = VALUE(U32, element_size_address);
			uint32_t value_size = VALUE(U32, value_size_address);
			if(element_size != value_size) {
				qb_report_unmet_intrinsic_condition_exception(cxt->line_id, cxt->intrinsic_function, "the third parameter to have the same size as the elements in the input array");
				return FALSE;
			}
		} else {
			qb_operand guard_operands[2] = { { QB_OPERAND_ADDRESS, { value_size_address } }, { QB_OPERAND_ADDRESS, { element_size_address } } };
			qb_operand guard_result = { QB_OPERAND_EMPTY, { NULL } };
			qb_produce_op(cxt, &factory_guard_array_size_exact, guard_operands, 2, &guard_result, NULL, 0, NULL);
		}
	} else {
		if(!IS_SCALAR(value->address)) {
			qb_report_unmet_intrinsic_condition_exception(cxt->line_id, cxt->intrinsic_function, "the third parameter to be a scalar when the input array is not multidimensional");
			return FALSE;
		}
	}
	return TRUE;
}

static int32_t qb_validate_operands_array_pos(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, uint32_t flags, qb_operand *operands, uint32_t operand_count, qb_result_destination *result_destination) {
	qb_operand *operand = &operands[0];

	if(IS_SCALAR(operand->address)) {
		qb_report_unexpected_intrinsic_argument_exception(cxt->line_id, cxt->intrinsic_function, 0, "array");
		return FALSE;
	}
	return TRUE;
}

static int32_t qb_validate_operands_array_rand(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, uint32_t flags, qb_operand *operands, uint32_t operand_count, qb_result_destination *result_destination) {
	qb_operand *container = &operands[0], *count = &operands[1];

	if(IS_SCALAR(container->address)) {
		qb_report_unexpected_intrinsic_argument_exception(cxt->line_id, cxt->intrinsic_function, 0, "array");
		return FALSE;
	}
	if(count->type == QB_OPERAND_ADDRESS) {
		if(!IS_SCALAR(count->address)) {
			qb_report_unexpected_intrinsic_argument_exception(cxt->line_id, cxt->intrinsic_function, 1, "scalar");
			return FALSE;
		}
	}
	return TRUE;
}

static int32_t qb_validate_operands_array_resize(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, uint32_t flags, qb_operand *operands, uint32_t operand_count, qb_result_destination *result_destination) {
	return TRUE;
}

static int32_t qb_validate_operands_array_slice(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, uint32_t flags, qb_operand *operands, uint32_t operand_count, qb_result_destination *result_destination) {
	qb_operand *container = &operands[0], *offset = &operands[1], *length = &operands[2];

	if(IS_SCALAR(container->address)) {
		qb_report_unexpected_intrinsic_argument_exception(cxt->line_id, cxt->intrinsic_function, 0, "array");
		return FALSE;
	} 
	if(!IS_SCALAR(offset->address)) {
		qb_report_unexpected_intrinsic_argument_exception(cxt->line_id, cxt->intrinsic_function, 1, "scalar");
		return FALSE;
	}
	if(length->type == QB_OPERAND_ADDRESS) {
		if(!IS_SCALAR(length->address)) {
			qb_report_unexpected_intrinsic_argument_exception(cxt->line_id, cxt->intrinsic_function, 2, "scalar");
			return FALSE;
		}
	}
	return TRUE;
}

static int32_t qb_validate_operands_range(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, uint32_t flags, qb_operand *operands, uint32_t operand_count, qb_result_destination *result_destination) {
	qb_operand *start = &operands[0], *end = &operands[1], *interval = &operands[2];

	if(!IS_SCALAR(start->address)) {
		qb_report_unexpected_intrinsic_argument_exception(cxt->line_id, cxt->intrinsic_function, 0, "scalar");
		return FALSE;
	}
	if(!IS_SCALAR(end->address)) {
		qb_report_unexpected_intrinsic_argument_exception(cxt->line_id, cxt->intrinsic_function, 1, "scalar");
		return FALSE;
	}
	if(interval->type == QB_OPERAND_ADDRESS && !IS_SCALAR(interval->address)) {
		qb_report_unexpected_intrinsic_argument_exception(cxt->line_id, cxt->intrinsic_function, 2, "scalar");
		return FALSE;
	}
	return TRUE;
}

static int32_t qb_validate_operands_utf8_decode(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, uint32_t flags, qb_operand *operands, uint32_t operand_count, qb_result_destination *result_destination) {
	qb_validate_operands_one_array(cxt, f, expr_type, flags, operands, operand_count, result_destination);
	return TRUE;
}

static int32_t qb_validate_operands_utf8_encode(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, uint32_t flags, qb_operand *operands, uint32_t operand_count, qb_result_destination *result_destination) {
	// nothing
	return TRUE;
}

static int32_t qb_validate_operands_pack(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, uint32_t flags, qb_operand *operands, uint32_t operand_count, qb_result_destination *result_destination) {
	qb_operand *value = &operands[0], *index = &operands[1];
	if(value->type != QB_OPERAND_ADDRESS) {
		// type coercion had failed earlier
		if(index->type != QB_OPERAND_NONE) {
			qb_report_unexpected_intrinsic_argument_exception(cxt->line_id, cxt->intrinsic_function, 1, "constant indicating the type");
		} else {
			qb_report_missing_intrinsic_argument_exception(cxt->line_id, cxt->intrinsic_function, 1, "the type cannot be determined from context");
		}
		return FALSE;
	}
	if(!IS_SCALAR(value->address)) {
		qb_report_unexpected_intrinsic_argument_exception(cxt->line_id, cxt->intrinsic_function, 0, "scalar");
		return FALSE;
	}
	return TRUE;
}

static int32_t qb_validate_operands_unpack(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, uint32_t flags, qb_operand *operands, uint32_t operand_count, qb_result_destination *result_destination) {
	qb_operand *value = &operands[0], *index = &operands[1], *type = &operands[2];
	if(IS_SCALAR(value->address)) {
		qb_report_unexpected_intrinsic_argument_exception(cxt->line_id, cxt->intrinsic_function, 0, "array of bytes");
		return FALSE;
	}
	if(index->type == QB_OPERAND_ADDRESS) {
		if(!IS_SCALAR(index->address)) {
			qb_report_unexpected_intrinsic_argument_exception(cxt->line_id, cxt->intrinsic_function, 0, "index value");
			return FALSE;
		}
	}
	if(expr_type == QB_TYPE_UNKNOWN || expr_type == QB_TYPE_ANY) {
		if(type->type != QB_OPERAND_NONE) {
			qb_report_unexpected_intrinsic_argument_exception(cxt->line_id, cxt->intrinsic_function, 1, "constant indicating the type");
			return FALSE;
		} else {
			qb_report_missing_intrinsic_argument_exception(cxt->line_id, cxt->intrinsic_function, 2, "the type cannot be determined from context");
			return FALSE;
		}
	}
	return TRUE;
}

static int32_t qb_validate_operands_function_call(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, uint32_t flags, qb_operand *operands, uint32_t operand_count, qb_result_destination *result_destination) {
	USE_TSRM
	qb_operand *func = &operands[0], *arguments = &operands[1], *argument_count = &operands[2];
	qb_function *qfunc = qb_find_compiled_function(func->zend_function TSRMLS_CC);
#if !ZEND_ENGINE_2_2 && !ZEND_ENGINE_2_1
	zend_class_entry *ce = EG(called_scope);
#else
	zend_class_entry *ce = EG(scope);
#endif
	const char *class_name = (ce) ? ce->name : NULL;
	uint32_t i;

	for(i = 0; i < (uint32_t) argument_count->number; i++) {
		if(i < qfunc->argument_count) {
			qb_variable *arg = qfunc->variables[i];
			qb_operand *val = &arguments->arguments[i];
			if(arg->flags & QB_VARIABLE_BY_REF) {
				if(val->type != QB_OPERAND_ADDRESS || IS_TEMPORARY(val->address)) {
					qb_report_unexpected_value_as_function_argument_exception(cxt->line_id, class_name, qfunc->name, i);
					return FALSE;
				}
				if(!STORAGE_TYPE_MATCH(val->address->type, arg->address->type)) {
					qb_report_unexpected_function_argument_type_exception(cxt->line_id, class_name, qfunc->name, i, val->address->type, arg->address->type);
					return FALSE;
				}
			}
		}
	}

	if(!qfunc->return_variable->address) {
		if(result_destination) {
			switch(result_destination->type) {
				case QB_RESULT_DESTINATION_VARIABLE:
				case QB_RESULT_DESTINATION_ELEMENT:
				case QB_RESULT_DESTINATION_PROPERTY:
				case QB_RESULT_DESTINATION_PRINT:
					qb_report_void_return_value_exception(cxt->line_id, class_name, qfunc->name);
					break;
				case QB_RESULT_DESTINATION_RETURN:
					if(cxt->return_variable->address) {
						qb_report_void_return_value_exception(cxt->line_id, class_name, qfunc->name);
					}
					break;
				default: {
				}	break;
			}
		}
	}
	return TRUE;
}

static int32_t qb_validate_operands_zend_function_call(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, uint32_t flags, qb_operand *operands, uint32_t operand_count, qb_result_destination *result_destination) {
	if(result_destination) {
		switch(result_destination->type) {
			case QB_RESULT_DESTINATION_RETURN:
			case QB_RESULT_DESTINATION_VARIABLE:
			case QB_RESULT_DESTINATION_ELEMENT:
			case QB_RESULT_DESTINATION_PROPERTY: break;
			default: {
				qb_report_ambiguous_return_value_exception(cxt->line_id);
				return FALSE;
			}
		}
	}
	return TRUE;
}

static int32_t qb_validate_operands_compile_time_function(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, uint32_t flags, qb_operand *operands, uint32_t operand_count, qb_result_destination *result_destination) {
	uint32_t i;
	qb_php_function_result_factory *pf = (qb_php_function_result_factory *) f;
	for(i = 0; i < operand_count; i++) {
		qb_operand *operand = &operands[i];
		int32_t type = pf->argument_types[i];
		if(type == IS_STRING) {
			if(operand->type != QB_OPERAND_ZVAL || Z_TYPE_P(operand->constant) != IS_STRING) {
				qb_report_unexpected_intrinsic_argument_exception(cxt->line_id, cxt->intrinsic_function, i, "literal string");
				return FALSE;
			} 
		} else if(type == IS_BOOL) {
			if(operand->type != QB_OPERAND_ZVAL || Z_TYPE_P(operand->constant) != IS_BOOL) {
				qb_report_unexpected_intrinsic_argument_exception(cxt->line_id, cxt->intrinsic_function, i, "boolean");
				return FALSE;
			} 
		} else if(type == IS_CONSTANT) {
			if(operand->type != QB_OPERAND_ZVAL) {
				qb_report_unexpected_intrinsic_argument_exception(cxt->line_id, cxt->intrinsic_function, i, "constant expression");
				return FALSE;
			} 
		} else if(type == IS_OBJECT) {
			if(operand->type != QB_OPERAND_THIS) {
				if(operand->type != QB_OPERAND_ZVAL || Z_TYPE_P(operand->constant) != IS_STRING) {
					qb_report_unexpected_intrinsic_argument_exception(cxt->line_id, cxt->intrinsic_function, i, "class name or $this");
					return FALSE;
				} 
			}
		}
	}
	return TRUE;
}

static int32_t qb_validate_operands_define(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, uint32_t flags, qb_operand *operands, uint32_t operand_count, qb_result_destination *result_destination) {
	qb_operand *name = &operands[0], *value = &operands[1];

	if(name->type != QB_OPERAND_ZVAL || Z_TYPE_P(name->constant) != IS_STRING) {
		qb_report_unexpected_intrinsic_argument_exception(cxt->line_id, cxt->intrinsic_function, 0, "constant string");
		return FALSE;
	}
	if(!(value->type == QB_OPERAND_ZVAL || (value->type == QB_OPERAND_ADDRESS && IS_IMMUTABLE(value->address) && IS_SCALAR(value->address)))) {
		qb_report_unexpected_intrinsic_argument_exception(cxt->line_id, cxt->intrinsic_function, 0, "constant expression");
		return FALSE;
	}
	return TRUE;
}


