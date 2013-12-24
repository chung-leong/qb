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

static int32_t qb_validate_operands_array_element(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count, qb_result_destination *result_destination) {
	qb_operand *container = &operands[0];
	qb_operand *index = &operands[1];

	if(container->type != QB_OPERAND_ADDRESS) {
		qb_report_not_an_array_exception(NULL, cxt->line_id);
		return FALSE;
	}

	if((index->type == QB_OPERAND_ADDRESS && !SCALAR(index->address)) || (index->type == QB_OPERAND_ARRAY_INITIALIZER)) {
		qb_report_illegal_array_index_exception(NULL, cxt->line_id);
		return FALSE;
	} else if(index->type == QB_OPERAND_ZVAL) {
		if(index->constant->type == IS_STRING) {
			qb_report_associative_array_exception(NULL, cxt->line_id);
			return FALSE;
		} else if(index->constant->type != IS_BOOL && index->constant->type != IS_LONG && index->constant->type != IS_DOUBLE) {
			qb_report_illegal_array_index_exception(NULL, cxt->line_id);
			return FALSE;
		}
	} else if(index->type == QB_OPERAND_NONE) {
		// an append operation
		if(!RESIZABLE(container->address)) {
			qb_report_fixed_length_array_exception(NULL, cxt->line_id);
			return FALSE;
		}
	}
	return TRUE;
}

static int32_t qb_validate_operands_object_property(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count, qb_result_destination *result_destination) {
	qb_operand *container = &operands[0];
	qb_operand *name = &operands[1];

	if(name->type != QB_OPERAND_ZVAL) {
		qb_report_variable_variable_exception(NULL, cxt->line_id);
		return FALSE;
	}
	if(container->type == QB_OPERAND_NONE) {
		if(!qb_get_instance_variable(cxt, name->constant)) {
			zend_class_entry *scope = cxt->zend_op_array->scope;
			qb_report_missing_property_exception(NULL, cxt->line_id, scope, Z_STRVAL_P(name->constant));
			return FALSE;
		}
	} else if(container->type == QB_OPERAND_ADDRESS) {
		uint32_t index;
		if(SCALAR(container->address)) {
			qb_report_not_an_array_exception(NULL, cxt->line_id);
			return FALSE;
		}
		if(!container->address->index_alias_schemes || !container->address->index_alias_schemes[0]) {
			qb_report_elements_not_named_exception(NULL, cxt->line_id);
			return FALSE;
		}
		index = qb_find_index_alias(cxt, container->address->index_alias_schemes[0], name->constant);
		if(index == INVALID_INDEX) {
			uint32_t swizzle_mask = 0;
			if(container->address->dimension_count == 1) {
				swizzle_mask = qb_get_swizzle_mask(cxt, container->address->index_alias_schemes[0], name->constant);
			}
			if(swizzle_mask == INVALID_INDEX) {
				qb_report_missing_named_element_exception(NULL, cxt->line_id, Z_STRVAL_P(name->constant));
				return FALSE;
			}
		}
	}
	return TRUE;
}

static int32_t qb_validate_operands_matching_type(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count, qb_result_destination *result_destination) {
	uint32_t i;
	for(i = 1; i < operand_count; i++) {
		if(operands[0].address->type != operands[i].address->type) {
			qb_report_type_mismatch_exception(NULL, cxt->line_id, operands[0].address->type, operands[i].address->type);
			return FALSE;
		}
	}
	return TRUE;
}

static int32_t qb_validate_operands_assign_return_value(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count, qb_result_destination *result_destination) {
	if(!cxt->return_variable->address) {
		qb_operand *value = &operands[0];
		if(value->type != QB_OPERAND_NONE && !(value->type == QB_OPERAND_ZVAL && value->constant->type == IS_NULL)) {
			qb_report_return_void_exception(NULL, cxt->line_id);
			return FALSE;
		}
	}
	return TRUE;
}

static int32_t qb_validate_operands_sent_value(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count, qb_result_destination *result_destination) {
	if(!cxt->sent_variable->address) {
		if(result_destination && result_destination->type != QB_RESULT_DESTINATION_FREE) {
			qb_report_missing_send_declaration(NULL, cxt->line_id);
			return FALSE;
		}
	}
	return TRUE;
}

static int32_t qb_validate_operands_rand(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count, qb_result_destination *result_destination) {
	if(operand_count != 0 && operand_count != 2) {
		qb_report_incorrect_argument_count_exception(NULL, cxt->line_id, cxt->intrinsic_function, operand_count);
		return FALSE;
	}
	return TRUE;
}

static int32_t qb_validate_operands_minmax_array(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count, qb_result_destination *result_destination) {
	qb_operand *container = &operands[0];

	if(operand_count == 1) {
		if(SCALAR(container->address)) {
			qb_report_unmet_intrinsic_condition_exception(NULL, cxt->line_id, cxt->intrinsic_function, "an array as parameter when only one parameter is given");
			return FALSE;
		}
	}
	return TRUE;
}

static int32_t qb_validate_array_append(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count, qb_result_destination *result_destination) {
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

static int32_t qb_validate_array_init(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count, qb_result_destination *result_destination) {
	if(operand_count > 0) {
		return qb_validate_array_append(cxt, f, expr_type, operands, operand_count, result_destination);
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

static int32_t qb_validate_operands_fetch_class_self(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count, qb_result_destination *result_destination) {
	zend_class_entry *ce = cxt->zend_op_array->scope;
	if(!ce) {
		qb_report_missing_scope_exception(NULL, cxt->line_id, "self");
		return FALSE;
	}
	return TRUE;
}

static int32_t qb_validate_operands_fetch_class_parent(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count, qb_result_destination *result_destination) {
	zend_class_entry *ce = cxt->zend_op_array->scope;
	if(!ce) {
		qb_report_missing_scope_exception(NULL, cxt->line_id, "parent");
		return FALSE;
	}
	if(!ce->parent) {
		qb_report_missing_parent_scope_exception(NULL, cxt->line_id, "parent");
		return FALSE;
	}
	return TRUE;
}

static int32_t qb_validate_operands_fetch_class_static(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count, qb_result_destination *result_destination) {
	zend_class_entry *ce = cxt->zend_op_array->scope;
	if(!ce) {
		qb_report_missing_scope_exception(NULL, cxt->line_id, "static");
		return FALSE;
	}
	return TRUE;
}

static int32_t qb_validate_operands_fetch_constant(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count, qb_result_destination *result_destination) {
	USE_TSRM
	qb_operand *scope = &operands[0], *name = &operands[1];
	if(scope->type == QB_OPERAND_ZEND_STATIC_CLASS) {
		// validation at runtime
	} else {
		zend_class_entry *ce = NULL;
		HashTable *ht;
		if(scope->type == QB_OPERAND_ZEND_CLASS) {
			ce = scope->zend_class;
		} else if(scope->type == QB_OPERAND_ZVAL) {
			ce = zend_fetch_class_by_name(Z_STRVAL_P(scope->constant), Z_STRLEN_P(scope->constant), NULL, 0 TSRMLS_CC);
		}
		if(ce) {
			ht = &ce->constants_table;
		} else {
			ht = EG(zend_constants);
		}
		if(!zend_hash_exists(ht, Z_STRVAL_P(name->constant), Z_STRLEN_P(name->constant) + 1)) {
			// see if it's a special QB constant
			if(!qb_get_special_constant(cxt, Z_STRVAL_P(name->constant), Z_STRLEN_P(name->constant))) {
				qb_report_undefined_constant_exception(NULL, cxt->line_id, ce, Z_STRVAL_P(name->constant));
				return FALSE;
			}
		}
	}
	return TRUE;
}

static int32_t qb_validate_operands_assign_ref(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count, qb_result_destination *result_destination) {
	qb_operand *variable = &operands[0], *value = &operands[1];
	// global and static assign ref to bring variables into the local scope
	if(variable->address != value->address) {
		qb_report_reference_exception(NULL, cxt->line_id);
		return FALSE;
	}
	return TRUE;
}

static int32_t qb_validate_operands_one_array(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count, qb_result_destination *result_destination) {
	qb_operand *operand = &operands[0];

	if(SCALAR(operand->address)) {
		qb_report_unexpected_intrinsic_argument_exception(NULL, cxt->line_id, cxt->intrinsic_function, 0, "array");
		return FALSE;
	}
	return TRUE;
}

static int32_t qb_validate_operands_referenceable(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count, qb_result_destination *result_destination) {
	qb_operand *operand = &operands[0];

	if(operand->type != QB_OPERAND_ADDRESS) {
		qb_report_unexpected_value_as_intrinsic_argument_exception(NULL, cxt->line_id, cxt->intrinsic_function, 0);
	}
	return TRUE;
}

static int32_t qb_validate_operands_one_array_variable(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count, qb_result_destination *result_destination) {
	if(!qb_validate_operands_referenceable(cxt, f, expr_type, operands, operand_count, result_destination)) {
		return FALSE;
	}
	if(!qb_validate_operands_one_array(cxt, f, expr_type, operands, operand_count, result_destination)) {
		return FALSE;
	}
	return TRUE;
}

static int32_t qb_validate_operands_two_arrays(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count, qb_result_destination *result_destination) {
	qb_operand *operand1 = &operands[0], *operand2 = &operands[1];

	if(SCALAR(operand1->address)) {
		qb_report_unexpected_intrinsic_argument_exception(NULL, cxt->line_id, cxt->intrinsic_function, 0, "array");
		return FALSE;
	}
	if(SCALAR(operand2->address)) {
		qb_report_unexpected_intrinsic_argument_exception(NULL, cxt->line_id, cxt->intrinsic_function, 1, "array");
		return FALSE;
	}
	return TRUE;
}

static int32_t qb_validate_operands_matching_vector_width(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count, qb_result_destination *result_destination) {
	qb_operand *operand1 = &operands[0], *operand2 = &operands[1];

	if(!qb_validate_operands_two_arrays(cxt, f, expr_type, operands, operand_count, result_destination)) {
		return FALSE;
	}

	if(CONSTANT_DIMENSION(operand1->address, -1) && CONSTANT_DIMENSION(operand2->address, -1)) {
		uint32_t vector_width1 = DIMENSION(operand1->address, -1);
		uint32_t vector_width2 = DIMENSION(operand2->address, -1);
		if(vector_width1 != vector_width2) {
			qb_report_vector_width_mismatch_exception(NULL, cxt->line_id, vector_width1, vector_width2);
			return FALSE;
		}
	} else {
		// TODO: runtime check
	}
	return TRUE;
}

static int32_t qb_validate_operands_refract(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count, qb_result_destination *result_destination) {
	qb_operand *operand3 = &operands[2];

	qb_validate_operands_matching_vector_width(cxt, f, expr_type, operands, operand_count, result_destination);

	if(!SCALAR(operand3->address)) {
		qb_report_unexpected_intrinsic_argument_exception(NULL, cxt->line_id, cxt->intrinsic_function, 2, "scalar");
		return FALSE;
	}
	return TRUE;
}

static int32_t qb_validate_operands_cross_product(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count, qb_result_destination *result_destination) {
	uint32_t i;

	for(i = 0; i < operand_count; i++) {
		qb_operand *operand = &operands[0];
		if(SCALAR(operand->address)) {
			qb_report_unexpected_intrinsic_argument_exception(NULL, cxt->line_id, cxt->intrinsic_function, i, "array");
			return FALSE;
		}
	}

	if(operand_count == 3) {
		qb_operand *operand1 = &operands[0], *operand2 = &operands[1], *operand3 = &operands[2];
		if(CONSTANT_DIMENSION(operand1->address, -1) && CONSTANT_DIMENSION(operand2->address, -1) && CONSTANT_DIMENSION(operand3->address, -1)) {
			uint32_t vector_width1 = DIMENSION(operand1->address, -1);
			uint32_t vector_width2 = DIMENSION(operand2->address, -1);
			uint32_t vector_width3 = DIMENSION(operand3->address, -1);

			if(!(vector_width1 == vector_width2 && vector_width2 == vector_width3 && 2 <= vector_width1 && vector_width1 <= 3)) {
				qb_report_invalid_4d_cross_product_exception(NULL, cxt->line_id, vector_width1, vector_width2, vector_width3);
				return FALSE;
			}
		} else {
			// TODO: runtime check
		} 
	} else {
		qb_operand *operand1 = &operands[0], *operand2 = &operands[1];
		if(CONSTANT_DIMENSION(operand1->address, -1) && CONSTANT_DIMENSION(operand2->address, -1)) {
			uint32_t vector_width1 = DIMENSION(operand1->address, -1);
			uint32_t vector_width2 = DIMENSION(operand2->address, -1);

			if(!(vector_width1 == vector_width2 && 2 <= vector_width1 && vector_width1 <= 3)) {
				qb_report_invalid_cross_product_exception(NULL, cxt->line_id, vector_width1, vector_width2);
				return FALSE;
			}
		} else {
			// TODO: runtime check
		} 
	}
	return TRUE;
}

static int32_t qb_validate_operands_one_matrix(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count, qb_result_destination *result_destination) {
	qb_operand *operand1 = &operands[0];

	if(operand1->address->dimension_count < 2) {
		qb_report_unexpected_intrinsic_argument_exception(NULL, cxt->line_id, cxt->intrinsic_function, 0, "matrix");
		return FALSE;
	}
	return TRUE;
}

static int32_t qb_validate_operands_square_matrix(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count, qb_result_destination *result_destination) {
	qb_operand *operand1 = &operands[0];

	qb_validate_operands_one_matrix(cxt, f, expr_type, operands, operand_count, result_destination);

	if(CONSTANT_DIMENSION(operand1->address, -1) && CONSTANT_DIMENSION(operand1->address, -2)) {
		uint32_t row = DIMENSION(operand1->address, -1);
		uint32_t col = DIMENSION(operand1->address, -2);
		if(row != col) {
			qb_report_unexpected_intrinsic_argument_exception(NULL, cxt->line_id, cxt->intrinsic_function, 0, "square matrix");
			return FALSE;
		}
	}
	return TRUE;
}

static int32_t qb_validate_operands_pixel(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count, qb_result_destination *result_destination) {
	qb_operand *operand1 = &operands[0], *operand2 = &operands[1];

	if(CONSTANT_DIMENSION(operand1->address, -1)) {
		uint32_t channel_count = DIMENSION(operand1->address, -1);
		if(!(3 <= channel_count && channel_count <= 4)) {
			qb_report_invalid_pixel_format_exception(NULL, cxt->line_id, cxt->intrinsic_function, channel_count);
			return FALSE;
		}
	} else {
		qb_report_variable_pixel_width_exception(NULL, cxt->line_id, cxt->intrinsic_function);
		return FALSE;
	}
	return TRUE;
}

static int32_t qb_validate_operands_rgba(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count, qb_result_destination *result_destination) {
	qb_operand *operand1 = &operands[0];

	if(CONSTANT_DIMENSION(operand1->address, -1)) {
		uint32_t channel_count = DIMENSION(operand1->address, -1);
		if(channel_count != 4) {
			qb_report_missing_alpha_channel_exception(NULL, cxt->line_id, cxt->intrinsic_function, channel_count);
			return FALSE;
		}
	} else {
		qb_report_variable_pixel_width_exception(NULL, cxt->line_id, cxt->intrinsic_function);
		return FALSE;
	}
	return TRUE;
}

static int32_t qb_validate_operands_sampling(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count, qb_result_destination *result_destination) {
	qb_operand *image = &operands[0];
	uint32_t channel_count;

	if(image->address->dimension_count != 3) {
		qb_report_unexpected_intrinsic_argument_exception(NULL, cxt->line_id, cxt->intrinsic_function, 0, "three-dimensional array");
	}
	if(CONSTANT_DIMENSION(image->address, -1)) {
		channel_count = DIMENSION(image->address, -1);
		if(!(3 <= channel_count && channel_count <= 4)) {
			qb_report_unexpected_intrinsic_argument_exception(NULL, cxt->line_id, cxt->intrinsic_function, 0, "array whose last dimension is 3 or 4");
			return FALSE;
		}
	} else {
		// TODO: do check at runtime
	}
	if(!(image->address->type >= QB_TYPE_F32)) {
		qb_report_unmet_intrinsic_condition_exception(NULL, cxt->line_id, cxt->intrinsic_function, "image data to be in floating-point representation");
		return FALSE;
	}
	return TRUE;
}

static int32_t qb_validate_operands_multidimensional_array(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count, qb_result_destination *result_destination) {
	qb_operand *container = &operands[0], *column_index = &operands[1];
	if(container->address->dimension_count < 2) {
		qb_report_unexpected_intrinsic_argument_exception(NULL, cxt->line_id, cxt->intrinsic_function, 0, "multidimensional array");
		return FALSE;
	}
	if(!SCALAR(column_index->address)) {
		qb_report_unexpected_intrinsic_argument_exception(NULL, cxt->line_id, cxt->intrinsic_function, 1, "scalar");
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
		return cxt->matrix_order;
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

static int32_t qb_validate_operands_mm_mult(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count, qb_result_destination *result_destination) {
	qb_operand *matrix1 = &operands[0], *matrix2 = &operands[1];
	qb_matrix_order order = qb_get_matrix_order(cxt, f);
	qb_address *m1_col_address = qb_obtain_matrix_column_address(cxt, matrix1->address, order);
	qb_address *m2_row_address = qb_obtain_matrix_row_address(cxt, matrix2->address, order);

	if(matrix1->address->dimension_count < 2) {
		qb_report_unexpected_intrinsic_argument_exception(NULL, cxt->line_id, cxt->intrinsic_function, 0, "matrix");
		return FALSE;
	}
	if(matrix2->address->dimension_count < 2) {
		qb_report_unexpected_intrinsic_argument_exception(NULL, cxt->line_id, cxt->intrinsic_function, 1, "matrix");
		return FALSE;
	}

	if(CONSTANT(m1_col_address) && CONSTANT(m2_row_address)) {
		uint32_t m1_col_count = VALUE(U32, m1_col_address);
		uint32_t m2_row_count = VALUE(U32, m2_row_address);

		if(!cxt->matrix_padding) {
			if(m1_col_count != m2_row_count) {
				qb_report_invalid_matrix_multiplication_exception(NULL, cxt->line_id, m1_col_count, m2_row_count, 1 | 2);
				return FALSE;
			}
		}
	} else {
		if(m1_col_address != m2_row_address) {
			// TODO: add run time checks
		}
	}
	return TRUE;
}

static int32_t qb_validate_operands_mv_mult(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count, qb_result_destination *result_destination) {
	qb_operand *matrix1 = &operands[0], *matrix2 = &operands[1];
	qb_matrix_order order = qb_get_matrix_order(cxt, f);
	qb_address *m1_col_address = qb_obtain_matrix_column_address(cxt, matrix1->address, order);
	qb_address *m2_row_address = DIMENSION_ADDRESS(matrix2->address, -1);

	if(matrix1->address->dimension_count < 2) {
		qb_report_unexpected_intrinsic_argument_exception(NULL, cxt->line_id, cxt->intrinsic_function, 0, "matrix");
		return FALSE;
	}
	if(matrix2->address->dimension_count < 1) {
		qb_report_unexpected_intrinsic_argument_exception(NULL, cxt->line_id, cxt->intrinsic_function, 1, "vector");
		return FALSE;
	}

	if(CONSTANT_DIMENSION(matrix1->address, -2) && CONSTANT_DIMENSION(matrix1->address, -1) && CONSTANT_DIMENSION(matrix2->address, -1)) {
		uint32_t m1_col_count = VALUE(U32, m1_col_address);
		uint32_t m2_row_count = VALUE(U32, m2_row_address);

		if(!cxt->matrix_padding) {
			if(m1_col_count != m2_row_count) {
				qb_report_invalid_matrix_multiplication_exception(NULL, cxt->line_id, m1_col_count, m2_row_count, 1);
				return FALSE;
			}
		}
	} else {
		if(m1_col_address != m2_row_address) {
			// TODO: add run time checks
		}
	}
	return TRUE;
}

static int32_t qb_validate_operands_vm_mult(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count, qb_result_destination *result_destination) {
	qb_operand *matrix1 = &operands[0], *matrix2 = &operands[1];
	qb_matrix_order order = qb_get_matrix_order(cxt, f);
	qb_address *m1_col_address = DIMENSION_ADDRESS(matrix1->address, -1);
	qb_address *m2_row_address = qb_obtain_matrix_row_address(cxt, matrix2->address, order);

	if(matrix1->address->dimension_count < 1) {
		qb_report_unexpected_intrinsic_argument_exception(NULL, cxt->line_id, cxt->intrinsic_function, 0, "vector");
		return FALSE;
	}
	if(matrix2->address->dimension_count < 2) {
		qb_report_unexpected_intrinsic_argument_exception(NULL, cxt->line_id, cxt->intrinsic_function, 1, "matrix");
		return FALSE;
	}

	if(CONSTANT_DIMENSION(matrix1->address, -1) && CONSTANT_DIMENSION(matrix2->address, -2) && CONSTANT_DIMENSION(matrix2->address, -1)) {
		uint32_t m1_col_count = VALUE(U32, m1_col_address);
		uint32_t m2_row_count = VALUE(U32, m2_row_address);

		if(!cxt->matrix_padding) {
			if(m1_col_count != m2_row_count) {
				qb_report_invalid_matrix_multiplication_exception(NULL, cxt->line_id, m1_col_count, m2_row_count, 2);
				return FALSE;
			}
		}
	} else {
		if(m1_col_address != m2_row_address) {
			// TODO: add run time checks
		}
	}
	return TRUE;
}

static int32_t qb_validate_operands_matrix_current_mode(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count, qb_result_destination *result_destination) {
	qb_matrix_op_factory_selector *s = (qb_matrix_op_factory_selector *) f;
	if(cxt->matrix_order == QB_MATRIX_ORDER_COLUMN_MAJOR) {
		f = s->cm_factory;
	} else {
		f = s->rm_factory;
	}
	return f->validate_operands(cxt, f, expr_type, operands, operand_count, result_destination);
}

static int32_t qb_validate_operands_transform(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count, qb_result_destination *result_destination) {
	qb_operand *matrix1 = &operands[0], *matrix2 = &operands[1];

	if(matrix1->address->dimension_count < 2) {
		qb_report_unexpected_intrinsic_argument_exception(NULL, cxt->line_id, cxt->intrinsic_function, 0, "matrix");
		return FALSE;
	}
	if(matrix2->address->dimension_count < 1) {
		qb_report_unexpected_intrinsic_argument_exception(NULL, cxt->line_id, cxt->intrinsic_function, 1, "vector");
		return FALSE;
	}

	if(CONSTANT_DIMENSION(matrix1->address, -2) && CONSTANT_DIMENSION(matrix1->address, -1) && CONSTANT_DIMENSION(matrix2->address, -1)) {
		qb_matrix_order order = qb_get_matrix_order(cxt, f);
		qb_address *m1_col_address = qb_obtain_matrix_column_address(cxt, matrix1->address, order);
		qb_address *m1_row_address = qb_obtain_matrix_row_address(cxt, matrix1->address, order);
		qb_address *m2_row_address = DIMENSION_ADDRESS(matrix2->address, -1);
		uint32_t m1_col_count = VALUE(U32, m1_col_address);
		uint32_t m1_row_count = VALUE(U32, m1_row_address);
		uint32_t m2_row_count = VALUE(U32, m2_row_address);

		if(!(m2_row_count >= 2 && m2_row_count <= 4 && m1_col_count == m2_row_count + 1 && m1_row_count == m2_row_count)) {
			qb_report_invalid_transform_exception(NULL, cxt->line_id, m1_col_count, m1_row_count, m2_row_count);
			return FALSE;
		}
	} else {
		// TODO: do runtime check
	}
	return TRUE;
}

static int32_t qb_validate_operands_array_push(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count, qb_result_destination *result_destination) {
	qb_operand *container = &operands[0], *value;
	qb_address *container_element_size_address;
	uint32_t i, container_element_size = 0;

	if(TEMPORARY(container->address)) {
		qb_report_unexpected_value_as_intrinsic_argument_exception(NULL, cxt->line_id, cxt->intrinsic_function, 0);
		return FALSE;
	}
	if(SCALAR(container->address)) {
		qb_report_unexpected_intrinsic_argument_exception(NULL, cxt->line_id, cxt->intrinsic_function, 0, "array");
		return FALSE;
	}
	if(FIXED_LENGTH(container->address)) {
		qb_report_unexpected_intrinsic_argument_exception(NULL, cxt->line_id, cxt->intrinsic_function, 0, "variable-length array");
		return FALSE;
	}

	if(container->address->dimension_count > 1) {
		container_element_size_address = container->address->array_size_addresses[1];
		if(CONSTANT(container_element_size_address)) {
			container_element_size = VALUE(U32, container_element_size_address);
		}
	} else {
		container_element_size = 1;
	}

	if(container_element_size != 0) {
		// the size of the container's elements is known
		// make sure the values pushed in aren't too big
		for(i = 1; i < operand_count; i++) {
			uint32_t value_size = 0;
			value = &operands[i];
			if(value->address->dimension_count > 0) {
				if(FIXED_LENGTH(value->address)) {
					value_size = ARRAY_SIZE(value->address);
				}
			} else {
				value_size = 1;
			}
			if(value_size != 0) {
				if(value_size > container_element_size) {
					qb_report_out_of_bound_exception(NULL, cxt->line_id, value_size, container_element_size, TRUE);
					return FALSE;
				}
			}
		}
	}
	return TRUE;
}

static int32_t qb_validate_operands_array_diff(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count, qb_result_destination *result_destination) {
	qb_operand *value = &operands[0];
	if(cxt->argument_offset == 0) {
		if(SCALAR(value->address)) {
			qb_report_unexpected_intrinsic_argument_exception(NULL, cxt->line_id, cxt->intrinsic_function, 0, "array");
			return FALSE;
		} 
	}
	return TRUE;
}

static int32_t qb_validate_operands_array_fill(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count, qb_result_destination *result_destination) {
	qb_operand *start_index = &operands[0], *number = &operands[1], *value = &operands[2];
	if(!SCALAR(start_index->address)) {
		qb_report_unexpected_intrinsic_argument_exception(NULL, cxt->line_id, cxt->intrinsic_function, 0, "scalar");
		return FALSE;
	}
	if(!SCALAR(number->address)) {
		qb_report_unexpected_intrinsic_argument_exception(NULL, cxt->line_id, cxt->intrinsic_function, 1, "scalar");
		return FALSE;
	}
	return TRUE;
}

static int32_t qb_validate_operands_array_pad(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count, qb_result_destination *result_destination) {
	qb_operand *input = &operands[0], *size = &operands[1], *value = &operands[2];

	if(SCALAR(input->address)) {
		qb_report_unexpected_intrinsic_argument_exception(NULL, cxt->line_id, cxt->intrinsic_function, 0, "array");
		return FALSE;
	}

	if(input->address->dimension_count > 1) {
		qb_address *element_size_address = input->address->array_size_addresses[1];
		qb_address *value_size_address = value->address->array_size_address;
		if(CONSTANT(element_size_address) && CONSTANT(value_size_address)) {
			uint32_t element_size = VALUE(U32, element_size_address);
			uint32_t value_size = VALUE(U32, value_size_address);
			if(element_size != value_size) {
				qb_report_unmet_intrinsic_condition_exception(NULL, cxt->line_id, cxt->intrinsic_function, "the third parameter to have the same size as the elements in the input array");
				return FALSE;
			}
		} else {
			// TODO: add runtime  check
		}
	} else {
		if(!SCALAR(value->address)) {
			qb_report_unmet_intrinsic_condition_exception(NULL, cxt->line_id, cxt->intrinsic_function, "the third parameter to be a scalar when the input array is not multidimensional");
			return FALSE;
		}
	}
	return TRUE;
}

static int32_t qb_validate_operands_array_pos(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count, qb_result_destination *result_destination) {
	qb_operand *operand = &operands[0];

	if(SCALAR(operand->address)) {
		qb_report_unexpected_intrinsic_argument_exception(NULL, cxt->line_id, cxt->intrinsic_function, 0, "array");
		return FALSE;
	}
	return TRUE;
}

static int32_t qb_validate_operands_array_rand(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count, qb_result_destination *result_destination) {
	qb_operand *container = &operands[0], *count = &operands[1];

	if(SCALAR(container->address)) {
		qb_report_unexpected_intrinsic_argument_exception(NULL, cxt->line_id, cxt->intrinsic_function, 0, "array");
		return FALSE;
	}
	if(count->type == QB_OPERAND_ADDRESS) {
		if(!SCALAR(count->address)) {
			qb_report_unexpected_intrinsic_argument_exception(NULL, cxt->line_id, cxt->intrinsic_function, 1, "scalar");
			return FALSE;
		}
	}
	return TRUE;
}

static int32_t qb_validate_operands_array_resize(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count, qb_result_destination *result_destination) {
	return TRUE;
}

static int32_t qb_validate_operands_array_slice(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count, qb_result_destination *result_destination) {
	qb_operand *container = &operands[0], *offset = &operands[1], *length = &operands[2];

	if(SCALAR(container->address)) {
		qb_report_unexpected_intrinsic_argument_exception(NULL, cxt->line_id, cxt->intrinsic_function, 0, "array");
		return FALSE;
	} 
	if(!SCALAR(offset->address)) {
		qb_report_unexpected_intrinsic_argument_exception(NULL, cxt->line_id, cxt->intrinsic_function, 1, "scalar");
		return FALSE;
	}
	if(length->type == QB_OPERAND_ADDRESS) {
		if(!SCALAR(length->address)) {
			qb_report_unexpected_intrinsic_argument_exception(NULL, cxt->line_id, cxt->intrinsic_function, 2, "scalar");
			return FALSE;
		}
	}
	return TRUE;
}

static int32_t qb_validate_operands_range(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count, qb_result_destination *result_destination) {
	qb_operand *start = &operands[0], *end = &operands[1], *interval = &operands[2];

	if(!SCALAR(start->address)) {
		qb_report_unexpected_intrinsic_argument_exception(NULL, cxt->line_id, cxt->intrinsic_function, 0, "scalar");
		return FALSE;
	}
	if(!SCALAR(end->address)) {
		qb_report_unexpected_intrinsic_argument_exception(NULL, cxt->line_id, cxt->intrinsic_function, 1, "scalar");
		return FALSE;
	}
	if(interval->type == QB_OPERAND_ADDRESS && !SCALAR(interval->address)) {
		qb_report_unexpected_intrinsic_argument_exception(NULL, cxt->line_id, cxt->intrinsic_function, 2, "scalar");
		return FALSE;
	}
	return TRUE;
}

static int32_t qb_validate_operands_utf8_decode(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count, qb_result_destination *result_destination) {
	qb_validate_operands_one_array(cxt, f, expr_type, operands, operand_count, result_destination);
	return TRUE;
}

static int32_t qb_validate_operands_utf8_encode(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count, qb_result_destination *result_destination) {
	// nothing
	return TRUE;
}

static int32_t qb_validate_operands_pack(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count, qb_result_destination *result_destination) {
	qb_operand *value = &operands[0], *index = &operands[1];
	if(value->type != QB_OPERAND_ADDRESS) {
		// type coercion had failed earlier
		if(index->type != QB_OPERAND_NONE) {
			qb_report_unexpected_intrinsic_argument_exception(NULL, cxt->line_id, cxt->intrinsic_function, 1, "constant indicating the type");
		} else {
			qb_report_missing_intrinsic_argument_exception(NULL, cxt->line_id, cxt->intrinsic_function, 1, "the type cannot be determined from context");
		}
		return FALSE;
	}
	if(!SCALAR(value->address)) {
		qb_report_unexpected_intrinsic_argument_exception(NULL, cxt->line_id, cxt->intrinsic_function, 0, "scalar");
		return FALSE;
	}
	return TRUE;
}

static int32_t qb_validate_operands_unpack(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count, qb_result_destination *result_destination) {
	qb_operand *value = &operands[0], *index = &operands[1], *type = &operands[2];
	if(SCALAR(value->address)) {
		qb_report_unexpected_intrinsic_argument_exception(NULL, cxt->line_id, cxt->intrinsic_function, 0, "array of bytes");
		return FALSE;
	}
	if(index->type == QB_OPERAND_ADDRESS) {
		if(!SCALAR(index->address)) {
			qb_report_unexpected_intrinsic_argument_exception(NULL, cxt->line_id, cxt->intrinsic_function, 0, "index value");
			return FALSE;
		}
	}
	if(expr_type == QB_TYPE_UNKNOWN || expr_type == QB_TYPE_ANY) {
		if(type->type != QB_OPERAND_NONE) {
			qb_report_unexpected_intrinsic_argument_exception(NULL, cxt->line_id, cxt->intrinsic_function, 1, "constant indicating the type");
			return FALSE;
		} else {
			qb_report_missing_intrinsic_argument_exception(NULL, cxt->line_id, cxt->intrinsic_function, 2, "the type cannot be determined from context");
			return FALSE;
		}
	}
	return TRUE;
}

static int32_t qb_validate_operands_defined(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count, qb_result_destination *result_destination) {
	qb_operand *name = &operands[0];
	if(name->type != QB_OPERAND_ZVAL || Z_TYPE_P(name->constant) != IS_STRING) {
		qb_report_unexpected_intrinsic_argument_exception(NULL, cxt->line_id, cxt->intrinsic_function, 0, "constant string");
		return FALSE;
	}
	return TRUE;
}

static int32_t qb_validate_operands_define(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count, qb_result_destination *result_destination) {
	qb_operand *name = &operands[0], *value = &operands[1];

	if(name->type != QB_OPERAND_ZVAL || Z_TYPE_P(name->constant) != IS_STRING) {
		qb_report_unexpected_intrinsic_argument_exception(NULL, cxt->line_id, cxt->intrinsic_function, 0, "constant string");
		return FALSE;
	}
	if(!(value->type == QB_OPERAND_ZVAL || (value->type == QB_OPERAND_ADDRESS && CONSTANT(value->address) && SCALAR(value->address)))) {
		qb_report_unexpected_intrinsic_argument_exception(NULL, cxt->line_id, cxt->intrinsic_function, 0, "constant expression");
		return FALSE;
	}
	return TRUE;
}

static int32_t qb_validate_operands_function_call(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count, qb_result_destination *result_destination) {
	qb_operand *func = &operands[0], *arguments = &operands[1], *argument_count = &operands[2];
	qb_function *qfunc = qb_find_compiled_function(func->zend_function);
	uint32_t i;

	for(i = 0; i < (uint32_t) argument_count->number; i++) {
		if(i < qfunc->argument_count) {
			qb_variable *arg = qfunc->variables[i];
			qb_operand *val = &arguments->arguments[i];
			if(arg->flags & QB_VARIABLE_BY_REF) {
				// TODO: check size
				if(val->type != QB_OPERAND_ADDRESS || TEMPORARY(val->address)) {
					qb_report_unexpected_value_as_function_argument_exception(NULL, cxt->line_id, qfunc, i);
					return FALSE;
				}
				if(!STORAGE_TYPE_MATCH(val->address->type, arg->address->type)) {
					qb_report_unexpected_function_argument_type_exception(NULL, cxt->line_id, qfunc, i, val->address->type, arg->address->type);
					return FALSE;
				}
			}
			if(SCALAR(arg->address) && !SCALAR(val->address)) {
				qb_report_unexpected_intrinsic_argument_exception(NULL, cxt->line_id, cxt->intrinsic_function, i, "scalar");
				return FALSE;
			}
		}
	}

	if(!qfunc->return_variable->address) {
		if(result_destination) {
			switch(result_destination->type) {
				case QB_RESULT_DESTINATION_RETURN:
				case QB_RESULT_DESTINATION_VARIABLE:
				case QB_RESULT_DESTINATION_ELEMENT:
				case QB_RESULT_DESTINATION_PROPERTY:
				case QB_RESULT_DESTINATION_PRINT:
					// TODO: warn about void
					break;
				default: {
				}	break;
			}
		}
	}
	return TRUE;
}

static int32_t qb_validate_operands_zend_function_call(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count, qb_result_destination *result_destination) {
	// TODO: check for by-ref argument
	if(result_destination) {
		switch(result_destination->type) {
			case QB_RESULT_DESTINATION_RETURN:
			case QB_RESULT_DESTINATION_VARIABLE:
			case QB_RESULT_DESTINATION_ELEMENT:
			case QB_RESULT_DESTINATION_PROPERTY: break;
			default: {
				qb_report_ambiguous_return_value_exception(NULL, cxt->line_id);
				return FALSE;
			}
		}
	}
	return TRUE;
}
