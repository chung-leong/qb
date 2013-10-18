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

static void qb_copy_address_dimensions(qb_compiler_context *cxt, qb_address *address, int32_t offset, qb_variable_dimensions *dim) {
	uint32_t i;
	if(offset < 0) {
		dim->dimension_count = address->dimension_count + offset;
		offset = 0;
	} else {
		dim->dimension_count = address->dimension_count - offset;		
	}
	if(dim->dimension_count > 0) {
		for(i = 0; i < dim->dimension_count; i++) {
			dim->dimension_addresses[i] = address->dimension_addresses[i + offset];
		}
		if(dim->dimension_count == address->dimension_count) {
			// the array sizes are the same
			for(i = 0; i < dim->dimension_count; i++) {
				dim->array_size_addresses[i] = address->array_size_addresses[i];
			}
		} else {
			// need to be recalculated
			for(i = dim->dimension_count - 1; (int32_t) i >= 0; i--) {
				if(i == dim->dimension_count - 1) {
					dim->array_size_addresses[i] = dim->dimension_addresses[i];
				} else {
					dim->array_size_addresses[i] = qb_obtain_on_demand_product(cxt, dim->dimension_addresses[i], dim->array_size_addresses[i + 1]);
				}
			}
		}
	} else {
		dim->array_size_addresses[0] = dim->dimension_addresses[0] = cxt->one_address;
	}
	dim->array_size_address = dim->array_size_addresses[0];
	dim->source_address = address;
}

static int32_t qb_compare_addresses(qb_address **addresses1, uint32_t count1, qb_address **addresses2, uint32_t count2) {
	uint32_t i;
	if(count1 != count2) {
		return FALSE;
	}
	for(i = 0; i < count1; i++) {
		if(addresses1[i] != addresses2[i]) {
			return FALSE;
		}
	}
	return TRUE;
}

static void qb_merge_address_dimensions(qb_compiler_context *cxt, qb_address *address1, int32_t offset1, qb_address *address2, int32_t offset2, qb_variable_dimensions *dim) {
	uint32_t i, count1, count2;
	if(offset1 < 0) {
		count1 = address1->dimension_count + offset1;
		offset1 = 0;
	} else {
		count1 = address1->dimension_count - offset1;		
	}
	if(offset2 < 0) {
		count2 = address2->dimension_count + offset2;
		offset2 = 0;
	} else {
		count2 = address2->dimension_count - offset2;		
	}
	dim->dimension_count = count1 + count2;
	if(dim->dimension_count > 0) {
		for(i = 0; i < dim->dimension_count; i++) {
			if(i < count1) {
				dim->dimension_addresses[i] = address1->dimension_addresses[i + offset1];
			} else {
				dim->dimension_addresses[i] = address2->dimension_addresses[(i - count1) + offset2];
			}
		}
		if(qb_compare_addresses(dim->dimension_addresses, dim->dimension_count, address1->dimension_addresses, address1->dimension_count)) {
			// the array sizes are the same as those of address1 
			for(i = 0; i < dim->dimension_count; i++) {
				dim->array_size_addresses[i] = address1->array_size_addresses[i];
			}
		} else if(qb_compare_addresses(dim->dimension_addresses, dim->dimension_count, address2->dimension_addresses, address2->dimension_count)) {
			// the array sizes are the same as those of address2 
			for(i = 0; i < dim->dimension_count; i++) {
				dim->array_size_addresses[i] = address2->array_size_addresses[i];
			}
		} else {
			// need to be recalculated
			for(i = dim->dimension_count - 1; (int32_t) i >= 0; i--) {
				if(i == dim->dimension_count - 1) {
					dim->array_size_addresses[i] = dim->dimension_addresses[i];
				} else {
					dim->array_size_addresses[i] = qb_obtain_on_demand_product(cxt, dim->dimension_addresses[i], dim->array_size_addresses[i + 1]);
				}
			}
		}
	} else {
		dim->array_size_addresses[0] = dim->dimension_addresses[0] = cxt->one_address;
	}
	dim->array_size_address = dim->array_size_addresses[0];
	dim->source_address = NULL;
}

static qb_address * qb_obtain_larger_of_two(qb_compiler_context *cxt, qb_address *size_address1, qb_address *value_address1, qb_address *size_address2, qb_address *value_address2) {
	if(size_address1 == value_address1 && size_address2 == value_address2) {
		qb_operand operands[2] = { { QB_OPERAND_ADDRESS, size_address1 }, { QB_OPERAND_ADDRESS, size_address2 } };
		return qb_obtain_on_demand_value(cxt, &factory_choose_size_of_larger_array_top_level, operands, 2);
	} else {
		qb_operand operands[4] = { { QB_OPERAND_ADDRESS, size_address1 }, { QB_OPERAND_ADDRESS, value_address1 }, { QB_OPERAND_ADDRESS, size_address2 }, { QB_OPERAND_ADDRESS, value_address2 } };
		return qb_obtain_on_demand_value(cxt, &factory_choose_size_of_larger_array, operands, 4);
	}
}

static uint32_t qb_get_minimum_size_address(qb_compiler_context *cxt, qb_variable_dimensions *dim) {
	uint32_t i;
	for(i = 0; i < dim->dimension_count; i++) {
		qb_address *size_address = dim->array_size_addresses[i];
		if(CONSTANT(size_address)) {
			return VALUE(U32, size_address);
		}
	}
	return 1;
}

static void qb_choose_dimensions_from_two(qb_compiler_context *cxt, qb_variable_dimensions *dim1, qb_variable_dimensions *dim2, qb_variable_dimensions *dim) {
	qb_variable_dimensions *dim_chosen = NULL;
	if(SCALAR(dim1)) {
		// use the second if the first is a scalar
		dim_chosen = dim2;
	} else if(SCALAR(dim2)) {
		// use the first if the second is a scalar
		dim_chosen = dim1;
	} else if(FIXED_LENGTH(dim1) && FIXED_LENGTH(dim2)) {
		if(ARRAY_SIZE(dim1) > ARRAY_SIZE(dim2)) {
			// use the first if it's bigger
			dim_chosen = dim1;
		} else if(ARRAY_SIZE(dim1) < ARRAY_SIZE(dim2)) {
			// use the second if it's bigger
			dim_chosen = dim2;
		} else if(dim2->dimension_count > dim1->dimension_count) {
			// use the second if it has more dimensions
			dim_chosen = dim2;
		} else {
			// use the first otherwise
			dim_chosen = dim1;
		}
	} else if(FIXED_LENGTH(dim1)) {
		uint32_t min_width2 = qb_get_minimum_size_address(cxt, dim2);
		if(min_width2 >= ARRAY_SIZE(dim1)) {
			// two is either zero-length (which results in a zero-length result) or it's bigger than one
			dim_chosen = dim2;
		}
	} else if(FIXED_LENGTH(dim2)) {
		uint32_t min_width1 = qb_get_minimum_size_address(cxt, dim1);
		if(min_width1 >= ARRAY_SIZE(dim2)) {
			dim_chosen = dim1;
		}
	}
	if(dim_chosen) {
		// we can figure one which to use now
		uint32_t i;
		dim->dimension_count = dim_chosen->dimension_count;
		dim->source_address = dim_chosen->source_address;
		for(i = 0; i < dim->dimension_count; i++) {
			dim->dimension_addresses[i] = dim_chosen->dimension_addresses[i];
			dim->array_size_addresses[i] = dim_chosen->array_size_addresses[i];
		}
	} else {
		// have to choose at runtime
		uint32_t i;
		if(dim1->dimension_count == dim2->dimension_count) {
			// keep all the dimensions
			dim->dimension_count = dim1->dimension_count;
		} else {
			// impossible to resolve--just leave it flat
			dim->dimension_count = 1;
		}
		for(i = 0; i < dim->dimension_count; i++) {
			dim->dimension_addresses[i] = qb_obtain_larger_of_two(cxt, dim1->array_size_address, dim1->dimension_addresses[i], dim2->array_size_address, dim2->dimension_addresses[i]);
			dim->array_size_addresses[i] = qb_obtain_larger_of_two(cxt, dim1->array_size_address, dim1->array_size_addresses[i], dim2->array_size_address, dim2->array_size_addresses[i]);
		}
	}
	dim->array_size_address = dim->array_size_addresses[0];
}

static void qb_choose_dimensions_from_two_addresses(qb_compiler_context *cxt, qb_address *address1, int32_t offset1, qb_address *address2, int32_t offset2, qb_variable_dimensions *dim) {
	if(SCALAR(address1)) {
		qb_copy_address_dimensions(cxt, address2, offset2, dim);
	} else if(SCALAR(address2)) {
		qb_copy_address_dimensions(cxt, address1, offset1, dim);
	} else {
		qb_variable_dimensions dim1, dim2;
		qb_copy_address_dimensions(cxt, address1, offset1, &dim1);
		qb_copy_address_dimensions(cxt, address2, offset2, &dim2);
		qb_choose_dimensions_from_two(cxt, &dim1, &dim2, dim);
	}
}

static qb_address * qb_obtain_largest_of_three(qb_compiler_context *cxt, qb_address *size_address1, qb_address *value_address1, qb_address *size_address2, qb_address *value_address2, qb_address *size_address3, qb_address *value_address3) {
	if(size_address1 == value_address1 && size_address2 == value_address2 && size_address3 == value_address3) {
		qb_operand operands[3] = { { QB_OPERAND_ADDRESS, size_address1 }, { QB_OPERAND_ADDRESS, size_address2 }, { QB_OPERAND_ADDRESS, size_address3 } };
		return qb_obtain_on_demand_value(cxt, &factory_choose_size_of_largest_of_three_arrays_top_level, operands, 3);
	} else {
		qb_operand operands[6] = { { QB_OPERAND_ADDRESS, size_address1 }, { QB_OPERAND_ADDRESS, value_address1 }, { QB_OPERAND_ADDRESS, size_address2 }, { QB_OPERAND_ADDRESS, value_address2 }, { QB_OPERAND_ADDRESS, size_address3 }, { QB_OPERAND_ADDRESS, value_address3 } };
		return qb_obtain_on_demand_value(cxt, &factory_choose_size_of_largest_of_three_arrays, operands, 6);
	}
}

static void qb_choose_dimensions_from_three(qb_compiler_context *cxt, qb_variable_dimensions *dim1, qb_variable_dimensions *dim2, qb_variable_dimensions *dim3, qb_variable_dimensions *dim) {
	qb_variable_dimensions *dim_chosen;
	if(SCALAR(dim1) && SCALAR(dim2)) {
		// use the third if the first and second are scalars
		dim_chosen = dim3;
	} else if(SCALAR(dim1) && SCALAR(dim3)) {
		// use the second if the first and third are scalars
		dim_chosen = dim2;
	} else if(SCALAR(dim2) && SCALAR(dim3)) {
		// use the first if the second and third are scalars
		dim_chosen = dim1;
	} else if(FIXED_LENGTH(dim1) && FIXED_LENGTH(dim2) && FIXED_LENGTH(dim3)) {
		if(ARRAY_SIZE(dim1) > ARRAY_SIZE(dim2) && ARRAY_SIZE(dim1) > ARRAY_SIZE(dim3)) {
			// use the first if it's bigger
			dim_chosen = dim1;
		} else if(ARRAY_SIZE(dim2) > ARRAY_SIZE(dim1) && ARRAY_SIZE(dim2) > ARRAY_SIZE(dim3)) {
			// use the second if it's bigger
			dim_chosen = dim2;
		} else if(ARRAY_SIZE(dim3) > ARRAY_SIZE(dim1) && ARRAY_SIZE(dim3) > ARRAY_SIZE(dim2)) {
			// use the third if it's bigger
			dim_chosen = dim3;
		} else if(dim3->dimension_count > dim2->dimension_count && dim3->dimension_count > dim2->dimension_count) {
			// use the third if it has more dimensions
			dim_chosen = dim3;
		} else if(dim2->dimension_count > dim1->dimension_count && dim2->dimension_count > dim3->dimension_count) {
			// use the second if it has more dimensions
			dim_chosen = dim2;
		} else {
			// use the first otherwise
			dim_chosen = dim1;
		}
	} else if(FIXED_LENGTH(dim1) && FIXED_LENGTH(dim2)) {
		uint32_t min_width3 = qb_get_minimum_size_address(cxt, dim3);
		if(min_width3 >= ARRAY_SIZE(dim1) && min_width3 >= ARRAY_SIZE(dim2)) {
			dim_chosen = dim3;
		}
	} else if(FIXED_LENGTH(dim1) && FIXED_LENGTH(dim3)) {
		uint32_t min_width2 = qb_get_minimum_size_address(cxt, dim2);
		if(min_width2 >= ARRAY_SIZE(dim1) && min_width2 >= ARRAY_SIZE(dim3)) {
			dim_chosen = dim2;
		}
	} else if(FIXED_LENGTH(dim2) && FIXED_LENGTH(dim3)) {
		uint32_t min_width1 = qb_get_minimum_size_address(cxt, dim1);
		if(min_width1 >= ARRAY_SIZE(dim2) && min_width1 >= ARRAY_SIZE(dim3)) {
			dim_chosen = dim1;
		}
	}
	if(dim_chosen) {
		// we can figure one which to use now
		uint32_t i;
		dim->dimension_count = dim_chosen->dimension_count;
		dim->source_address = dim_chosen->source_address;
		for(i = 0; i < dim->dimension_count; i++) {
			dim->dimension_addresses[i] = dim_chosen->dimension_addresses[i];
			dim->array_size_addresses[i] = dim_chosen->array_size_addresses[i];
		}
	} else if(SCALAR(dim1)) {
		// only need to choose between the second and third
		qb_choose_dimensions_from_two(cxt, dim2, dim3, dim);
		return;
	} else if(SCALAR(dim2)) {
		// only need to choose between the first and third
		qb_choose_dimensions_from_two(cxt, dim1, dim3, dim);
		return;
	} else if(SCALAR(dim3)) {
		// only need to choose between the first and second
		qb_choose_dimensions_from_two(cxt, dim1, dim2, dim);
		return;
	} else {
		// have to choose at runtime
		uint32_t i;
		if(dim1->dimension_count == dim2->dimension_count && dim1->dimension_count == dim3->dimension_count) {
			// keep all the dimensions
			dim->dimension_count = dim1->dimension_count;
		} else {
			// impossible to resolve--just leave it flat
			dim->dimension_count = 1;
		}
		for(i = 0; i < dim->dimension_count; i++) {
			dim->dimension_addresses[i] = qb_obtain_largest_of_three(cxt, dim1->array_size_address, dim1->dimension_addresses[i], dim2->array_size_address, dim2->dimension_addresses[i], dim3->array_size_address, dim3->dimension_addresses[i]);
			dim->array_size_addresses[i] = qb_obtain_largest_of_three(cxt, dim1->array_size_address, dim1->array_size_addresses[i], dim2->array_size_address, dim2->array_size_addresses[i], dim3->array_size_address, dim3->array_size_addresses[i]);
		}
	}
	dim->array_size_address = dim->array_size_addresses[0];
}

static void qb_choose_dimensions_from_three_addresses(qb_compiler_context *cxt, qb_address *address1, int32_t offset1, qb_address *address2, int32_t offset2, qb_address *address3, int32_t offset3, qb_variable_dimensions *dim) {
	if(SCALAR(address1) && SCALAR(address2)) {
		qb_copy_address_dimensions(cxt, address3, offset3, dim);
	} else if(SCALAR(address1) && SCALAR(address3)) {
		qb_copy_address_dimensions(cxt, address2, offset2, dim);
	} else if(SCALAR(address2) && SCALAR(address3)) {
		qb_copy_address_dimensions(cxt, address1, offset1, dim);
	} else {
		qb_variable_dimensions dim1, dim2, dim3;
		qb_copy_address_dimensions(cxt, address1, offset1, &dim1);
		qb_copy_address_dimensions(cxt, address2, offset2, &dim2);
		qb_copy_address_dimensions(cxt, address3, offset3, &dim3);
		qb_choose_dimensions_from_three(cxt, &dim1, &dim2, &dim3, dim);
	}
}

static void qb_append_address_dimensions(qb_compiler_context *cxt, qb_address *first_dimension_address, qb_address *address, int32_t offset, qb_variable_dimensions *dim) {
	uint32_t i;
	dim->dimension_count = 1 + address->dimension_count - offset;
	dim->dimension_addresses[0] = first_dimension_address;
	for(i = 1; i < dim->dimension_count; i++) {
		uint32_t dimension = VALUE(U32, address->dimension_addresses[i + (offset - 1)]);
		dim->dimension_addresses[i] = address->dimension_addresses[i + (offset - 1)];
	}
	for(i = dim->dimension_count - 1; (int32_t) i >= 0; i--) {
		if(i == dim->dimension_count - 1) {
			dim->array_size_addresses[i] = dim->dimension_addresses[i];
		} else {
			dim->array_size_addresses[i] = qb_obtain_on_demand_product(cxt, dim->dimension_addresses[i], dim->array_size_addresses[i + 1]);
		}
	}
	dim->array_size_address = dim->array_size_addresses[0];
}

static void qb_set_result_dimensions_first_operand(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_variable_dimensions *dim) {
	qb_operand *first = &operands[0];
	qb_copy_address_dimensions(cxt, first->address, 0, dim);
}

static void qb_set_result_dimensions_second_operand(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_variable_dimensions *dim) {
	qb_operand *second = &operands[1];
	qb_copy_address_dimensions(cxt, second->address, 0, dim);
}

static void qb_set_result_dimensions_object_property(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_variable_dimensions *dim) {
	qb_operand *container = &operands[0], *name = &operands[1];
	qb_address *address = qb_obtain_object_property(cxt, container, name, 0);
	qb_copy_address_dimensions(cxt, address, 0, dim);
}

static void qb_set_result_dimensions_larger_of_two(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_variable_dimensions *dim) {
	qb_operand *first = &operands[0], *second = &operands[1];
	qb_choose_dimensions_from_two_addresses(cxt, first->address, 0, second->address, 0, dim);
}

static void qb_set_result_dimensions_largest_of_three(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_variable_dimensions *dim) {
	qb_operand *first = &operands[0], *second = &operands[1], *third = &operands[2];
	qb_choose_dimensions_from_three_addresses(cxt, first->address, 0, second->address, 0, third->address, 0, dim);
}

static void qb_set_result_dimensions_round(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_variable_dimensions *dim) {
	switch(operand_count) {
		case 1: qb_set_result_dimensions_first_operand(cxt, f, operands, operand_count, dim); break;
		case 2: qb_set_result_dimensions_larger_of_two(cxt, f, operands, operand_count, dim); break;
		case 3: qb_set_result_dimensions_largest_of_three(cxt, f, operands, operand_count, dim); break;
	}
}

static void qb_set_result_dimensions_length(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_variable_dimensions *dim) {
	qb_address *vector_address = operands[0].address;
	qb_copy_address_dimensions(cxt, vector_address, -1, dim);
}

static void qb_set_result_dimensions_dot_product(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_variable_dimensions *dim) {
	qb_operand *first = &operands[0], *second = &operands[1];
	qb_choose_dimensions_from_two_addresses(cxt, first->address, -1, second->address, -1, dim);
}

static void qb_set_result_dimensions_cross_product(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_variable_dimensions *dim) {
	if(operand_count == 3) {
		qb_set_result_dimensions_largest_of_three(cxt, f, operands, operand_count, dim);	
	} else {
		qb_set_result_dimensions_larger_of_two(cxt, f, operands, operand_count, dim);	
	}
}

static void qb_copy_matrix_multiplication_result_dimensions(qb_compiler_context *cxt, qb_address *matrix_address, int32_t offset, qb_address *row_address, qb_address *col_address, qb_matrix_order order, qb_variable_dimensions *dim) {
	uint32_t i;
	dim->dimension_count = matrix_address->dimension_count + offset;
	for(i = 0; i < dim->dimension_count; i++) {
		dim->dimension_addresses[i] = matrix_address->dimension_addresses[i];
	}
	if(!row_address) {
		dim->dimension_addresses[dim->dimension_count] = col_address;
		dim->dimension_count += 1;
	} else if(!col_address) {
		dim->dimension_addresses[dim->dimension_count] = row_address;
		dim->dimension_count += 1;
	} else {
		int32_t row_offset = (order == QB_MATRIX_ORDER_ROW_MAJOR) ? 0 : 1;
		int32_t col_offset = (order == QB_MATRIX_ORDER_ROW_MAJOR) ? 1 : 0;
		dim->dimension_addresses[dim->dimension_count + row_offset] = row_address;
		dim->dimension_addresses[dim->dimension_count + col_offset] = col_address;
		dim->dimension_count += 2;
	}
	if(qb_compare_addresses(dim->dimension_addresses, dim->dimension_count, matrix_address->dimension_addresses, matrix_address->dimension_count)) {
		// the array sizes are the same
		for(i = 0; i < dim->dimension_count; i++) {
			dim->array_size_addresses[i] = matrix_address->array_size_addresses[i];
		}
	} else {
		// need to be recalculated
		for(i = dim->dimension_count - 1; (int32_t) i >= 0; i--) {
			if(i == dim->dimension_count - 1) {
				dim->array_size_addresses[i] = dim->dimension_addresses[i];
			} else {
				dim->array_size_addresses[i] = qb_obtain_on_demand_product(cxt, dim->dimension_addresses[i], dim->dimension_addresses[i + 1]);
			}
		}
	}
	dim->array_size_address = dim->array_size_addresses[0];
	dim->source_address = NULL;
}


static void qb_set_result_dimensions_mm_mult(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_variable_dimensions *dim) {
	qb_operand *matrix1 = &operands[0], *matrix2 = &operands[1];
	qb_matrix_order order = qb_get_matrix_order(cxt, f);
	qb_address *m1_row_address = qb_obtain_matrix_row_address(cxt, matrix1->address, order);
	qb_address *m2_col_address = qb_obtain_matrix_column_address(cxt, matrix2->address, order);

	// the result matrix is m1_row x m2_col
	if(matrix1->address->dimension_count == 2) {
		qb_copy_matrix_multiplication_result_dimensions(cxt, matrix2->address, -2, m1_row_address, m2_col_address, order, dim);
	} else if(matrix2->address->dimension_count == 2) {
		qb_copy_matrix_multiplication_result_dimensions(cxt, matrix1->address, -2, m1_row_address, m2_col_address, order, dim);
	} else {
		qb_variable_dimensions dim1, dim2;
		qb_copy_matrix_multiplication_result_dimensions(cxt, matrix1->address, -2, m1_row_address, m2_col_address, order, &dim1);
		qb_copy_matrix_multiplication_result_dimensions(cxt, matrix2->address, -2, m1_row_address, m2_col_address, order, &dim2);
		qb_choose_dimensions_from_two(cxt, &dim1, &dim2, dim);
	}
}

static void qb_set_result_dimensions_mv_mult(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_variable_dimensions *dim) {
	qb_operand *matrix1 = &operands[0], *matrix2 = &operands[1];
	qb_matrix_order order = qb_get_matrix_order(cxt, f);
	qb_address *m1_row_address = qb_obtain_matrix_row_address(cxt, matrix1->address, order);

	if(matrix1->address->dimension_count == 2) {
		qb_copy_matrix_multiplication_result_dimensions(cxt, matrix2->address, -1, m1_row_address, NULL, order, dim);
	} else if(matrix2->address->dimension_count == 1) {
		qb_copy_matrix_multiplication_result_dimensions(cxt, matrix1->address, -2, m1_row_address, NULL, order, dim);
	} else {
		qb_variable_dimensions dim1, dim2;
		qb_copy_matrix_multiplication_result_dimensions(cxt, matrix1->address, -2, m1_row_address, NULL, order, &dim1);
		qb_copy_matrix_multiplication_result_dimensions(cxt, matrix2->address, -1, m1_row_address, NULL, order, &dim2);
		qb_choose_dimensions_from_two(cxt, &dim1, &dim2, dim);
	}
}

static void qb_set_result_dimensions_vm_mult(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_variable_dimensions *dim) {
	qb_operand *matrix1 = &operands[0], *matrix2 = &operands[1];
	qb_matrix_order order = qb_get_matrix_order(cxt, f);
	qb_address *m2_col_address = qb_obtain_matrix_column_address(cxt, matrix2->address, order);

	if(matrix1->address->dimension_count == 1) {
		qb_copy_matrix_multiplication_result_dimensions(cxt, matrix2->address, -2, NULL, m2_col_address, order, dim);
	} else if(matrix2->address->dimension_count == 2) {
		qb_copy_matrix_multiplication_result_dimensions(cxt, matrix1->address, -1, NULL, m2_col_address, order, dim);
	} else {
		qb_variable_dimensions dim1, dim2;
		qb_copy_matrix_multiplication_result_dimensions(cxt, matrix1->address, -1, NULL, m2_col_address, order, &dim1);
		qb_copy_matrix_multiplication_result_dimensions(cxt, matrix2->address, -2, NULL, m2_col_address, order, &dim2);
		qb_choose_dimensions_from_two(cxt, &dim1, &dim2, dim);
	}
}

// make use of the fact that A' * B' = (B * A)' 
static void qb_set_result_dimensions_transpose_equivalent(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_variable_dimensions *dim) {
	qb_derived_op_factory *df = (qb_derived_op_factory *) f;
	qb_operand reversed_operands[2];
	reversed_operands[0] = operands[1];
	reversed_operands[1] = operands[0];
	f = df->parent;
	f->set_dimensions(cxt, f, reversed_operands, 2, dim);
}

static void qb_set_result_dimensions_matrix_current_mode(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_variable_dimensions *dim) {
	qb_matrix_op_factory_selector *s = (qb_matrix_op_factory_selector *) f;
	if(cxt->matrix_order == QB_MATRIX_ORDER_COLUMN_MAJOR) {
		f = s->cm_factory;
	} else {
		f = s->rm_factory;
	}
	f->set_dimensions(cxt, f, operands, operand_count, dim);
}

static void qb_set_result_dimensions_transpose(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_variable_dimensions *dim) {
	qb_address *matrix_address = operands[0].address, *temp;
	qb_copy_address_dimensions(cxt, matrix_address, 0, dim);

	// swap the lowest two dimensions
	temp = DIMENSION_ADDRESS(dim, -2);
	DIMENSION_ADDRESS(dim, -2) = DIMENSION_ADDRESS(dim, -1);
	DIMENSION_ADDRESS(dim, -1) = ARRAY_SIZE_ADDRESS(dim, -1) = temp;
	dim->source_address = matrix_address;
}

static void qb_set_result_dimensions_determinant(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_variable_dimensions *dim) {
	qb_address *matrix_address = operands[0].address;
	qb_copy_address_dimensions(cxt, matrix_address, -2, dim);
}

static void qb_set_result_dimensions_sampling(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_variable_dimensions *dim) {
	qb_address *image_address = operands[0].address;
	qb_address *channel_count_address = DIMENSION_ADDRESS(image_address, -1);
	qb_address *x_address = operands[1].address;
	qb_address *y_address = operands[2].address;

	if(SCALAR(x_address) && SCALAR(y_address)) {
		qb_copy_address_dimensions(cxt, image_address, 2, dim);
	} else if(SCALAR(x_address)) {
		// merge the dimensions of y with the last dimension of image
		qb_merge_address_dimensions(cxt, y_address, 0, image_address, 2, dim);
	} else if(SCALAR(y_address)) {
		// merge the dimensions of x with the last dimension of image
		qb_merge_address_dimensions(cxt, x_address, 0, image_address, 2, dim);
	} else {
		// need to choose between x and y
		qb_variable_dimensions dim1, dim2;
		qb_merge_address_dimensions(cxt, x_address, 0, image_address, 2, &dim1);
		qb_merge_address_dimensions(cxt, y_address, 0, image_address, 2, &dim2);
		qb_choose_dimensions_from_two(cxt, &dim1, &dim2, dim);
	}
}

static void qb_set_result_dimensions_array_merge(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_variable_dimensions *dim) {
	/*
	uint32_t i, final_length = 0;
	qb_address *dimension_source = NULL;
	for(i = 0; i < operand_count; i++) {
		qb_address *address = operands[i].address;
		if(VARIABLE_LENGTH_ARRAY(address)) {
			final_length = 0;
			dimension_source = NULL;
			break;
		} else {
			if(SCALAR(address)) {
				final_length += 1;
			} else {
				final_length += ARRAY_SIZE(address);
			}
		}
		if(!dimension_source || address->dimension_count > dimension_source->dimension_count) {
			dimension_source = address;
		}
	}
	dim->array_size = final_length;
	if(dimension_source && dimension_source->dimension_count > 1) {
		uint32_t element_count = final_length / VALUE(U32, dimension_source->array_size_addresses[1]);
		dim->dimension_addresses[0] = qb_obtain_constant_U32(cxt, element_count);
		for(i = 1; i < dimension_source->dimension_count; i++) {
			dim->dimension_addresses[i] = dimension_source->dimension_addresses[i];
		}
		dim->dimension_count = dimension_source->dimension_count;
	} else {
		dim->dimension_count = 1;
	}
	*/
}

static void qb_set_result_dimensions_array_fill(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_variable_dimensions *dim) {
	/*
	qb_address *index_address = operands[0].address;
	qb_address *number_address = operands[1].address;
	qb_address *value_address = operands[2].address;
	int32_t count_is_constant;
	uint32_t count;
	if(CONSTANT(index_address) && CONSTANT(number_address)) {
		uint32_t start_index = VALUE(U32, index_address);
		uint32_t number = VALUE(U32, number_address);
		count = start_index + number;
		count_is_constant = TRUE;
	} else {
		count_is_constant = FALSE;
	}

	if(count_is_constant && !VARIABLE_LENGTH_ARRAY(value_address)) {
		uint32_t value_size;
		if(SCALAR(value_address)) {
			value_size = 1;
		} else {
			value_size = ARRAY_SIZE(value_address);
		}
		dim->array_size = count * value_size;
	} else {
		dim->array_size = 0;
	}
	if(value_address->dimension_count > 0) {
		uint32_t i;
		dim->dimension_count = value_address->dimension_count + 1;
		for(i = 0; i < value_address->dimension_count; i++) {
			dim->dimension_addresses[i + 1] = value_address->dimension_addresses[i];
		}
		if(count_is_constant) {
			dim->dimension_addresses[0] = qb_obtain_constant_U32(cxt, count);
		} else {
			dim->dimension_addresses[0] = NULL;
		}
	} else {
		dim->dimension_count = 1;
	}
	*/
}

static void qb_set_result_dimensions_array_pad(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_variable_dimensions *dim) {
	qb_address *container_address = operands[0].address;
	/*
	dim->array_size = 0;
	if(container_address->dimension_count > 1) {
		uint32_t i;
		dim->dimension_count = container_address->dimension_count;
		for(i = 1; i < container_address->dimension_count; i++) {
			dim->dimension_addresses[i] = container_address->dimension_addresses[i];
		}
		dim->dimension_addresses[0] = NULL;
	} else {
		dim->dimension_count = 1;
	}
	*/
}

static void qb_set_result_dimensions_array_column(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_variable_dimensions *dim) {
	qb_address *address = operands[0].address;
	// copy the first dimension then the rest, skipping over the second
	qb_merge_address_dimensions(cxt, address, -((int32_t) address->dimension_count - 1), address, +2, dim);
}

static void qb_set_result_dimensions_range(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_variable_dimensions *dim) {
	/*
	dim->dimension_count = 1;
	if(current_array_size != UINT32_MAX) {
		dim->array_size = current_array_size;
	} else {
		// the array size isn't known (or the parameters are erroreous)
		dim->array_size = 0;
	}
	*/
}

static void qb_set_result_dimensions_array_rand(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_variable_dimensions *dim) {
	qb_address *count_address = operands[1].address;
	/*
	if(CONSTANT(count_address)) {
		uint32_t count = VALUE(U32, count_address);
		dim->array_size = count;
		dim->dimension_count = (count == 1) ? 0 : 1;
	} else {
		// don't know how many elements will be returned
		dim->dimension_count = 1;
		dim->array_size = 0;
	}
	*/
}

static void qb_set_result_dimensions_array_diff(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_variable_dimensions *dim) {
	qb_operand *first = &operands[0];
	qb_address *first_dimension_address = qb_obtain_on_demand_value(cxt, &factory_array_diff_count, operands, operand_count);
	qb_append_address_dimensions(cxt, first_dimension_address, first->address, 1, dim);
}

static void qb_set_result_dimensions_array_intersect(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_variable_dimensions *dim) {
	qb_operand *first = &operands[0];
	qb_address *first_dimension_address = qb_obtain_on_demand_value(cxt, &factory_array_intersect_count, operands, operand_count);
	qb_append_address_dimensions(cxt, first_dimension_address, first->address, 1, dim);
}

static void qb_set_result_dimensions_array_unique(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_variable_dimensions *dim) {
	qb_operand *first = &operands[0];
	qb_address *first_dimension_address = qb_obtain_on_demand_value(cxt, &factory_array_unique_count, operands, operand_count);
	qb_append_address_dimensions(cxt, first_dimension_address, first->address, 1, dim);
}

static void qb_set_result_dimensions_utf8_decode(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_variable_dimensions *dim) {
	qb_operand *string = &operands[0];
	qb_address *decoded_length_address = qb_obtain_on_demand_value(cxt, &factory_utf8_decode_count, operands, operand_count);
	dim->dimension_count = 1;
	dim->array_size_address = dim->array_size_addresses[0] = dim->dimension_addresses[0] = decoded_length_address;
}

static void qb_set_result_dimensions_utf8_encode(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_variable_dimensions *dim) {
	qb_operand *codepoints = &operands[0];
	qb_address *decoded_length_address = qb_obtain_on_demand_value(cxt, &factory_utf8_encode_count, operands, operand_count);
	dim->dimension_count = 1;
	dim->array_size_address = dim->array_size_addresses[0] = dim->dimension_addresses[0] = decoded_length_address;
}

static void qb_set_result_dimensions_pack(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_variable_dimensions *dim) {
	qb_operand *value = &operands[0];
	uint32_t byte_count = BYTE_COUNT(1, value->address->type);
	qb_address *byte_count_address = qb_obtain_constant_U32(cxt, byte_count);
	dim->dimension_count = 1;
	dim->array_size_address = dim->array_size_addresses[0] = dim->dimension_addresses[0] = byte_count_address;
}
