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

static qb_address * ZEND_FASTCALL qb_obtain_constant_U32(qb_compiler_context *cxt, uint32_t value);

static void ZEND_FASTCALL qb_set_result_dimensions_matching(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_variable_dimensions *dim) {
	// size of the result matches the largest of the operands
	// the structure of the result also comes from the largest of the operands
	// if two operands are the same size, then the one with MORE dimensions wins
	uint32_t i, array_size, current_array_size = 0;
	qb_address *dimension_source = NULL;
	for(i = 0; i < operand_count; i++) {
		qb_address *address = operands[i].address;
		if(!SCALAR(address)) {
			if(VARIABLE_LENGTH_ARRAY(address)) {
				array_size = UINT32_MAX;
			} else {
				array_size = ARRAY_SIZE(address);
			}
			if(array_size > current_array_size) {
				current_array_size = array_size;
				dimension_source = address;
			} else if(array_size == current_array_size) {
				if(array_size == UINT32_MAX) {
					// if there're more than one variable-length array, the  
					// situation is ambiguous and we don't try to preserve array dimensions
					dimension_source = NULL;
				} else if(dimension_source && dimension_source->dimension_count < address->dimension_count) {
					dimension_source = address;
				}
			}
		}
	}
	if(current_array_size > 0) {
		dim->dimension_count = 1;
		if(current_array_size != UINT32_MAX) {
			dim->array_size = current_array_size;
		} else {
			dim->array_size = 0;
		}
		if(dimension_source && dimension_source->dimension_count > 1) {
			dim->dimension_count = dimension_source->dimension_count;
			for(i = 0; i < dimension_source->dimension_count; i++) {
				dim->dimension_addresses[i] = dimension_source->dimension_addresses[i];
			}
		}
	}
	dim->source_address = dimension_source;
}

static void ZEND_FASTCALL qb_set_result_dimensions_vector(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_variable_dimensions *dim) {
	qb_address *vector_address = operands[0].address;
	if(vector_address->dimension_count > 1) {
		dim->dimension_count = 1;
		if(VARIABLE_LENGTH_ARRAY(vector_address)) {
			dim->array_size = 0;
		} else {
			qb_address *vector_width_address = vector_address->array_size_addresses[vector_address->dimension_count - 1];
			uint32_t vector_width = VALUE(U32, vector_width_address);
			uint32_t element_count = ARRAY_SIZE(vector_address);
			dim->array_size = element_count / vector_width;
		}
	}
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

static void ZEND_FASTCALL qb_set_result_dimensions_mm_mult(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_variable_dimensions *dim) {
	qb_address *m1_address, *m1_size_address, *m1_row_address;
	qb_address *m2_address, *m2_size_address, *m2_col_address;
	uint32_t i, m1_count, m2_count, res_count;

	qb_matrix_order order = qb_get_matrix_order(cxt, f);
	int32_t col_offset = (order == QB_MATRIX_ORDER_COLUMN_MAJOR) ? -2 : -1;
	int32_t row_offset = (order == QB_MATRIX_ORDER_ROW_MAJOR) ? -2 : -1;

	m1_address = operands[0].address;
	m2_address = operands[1].address;
	m1_size_address = m1_address->array_size_addresses[m1_address->dimension_count - 2];
	m2_size_address = m2_address->array_size_addresses[m2_address->dimension_count - 2];
	m1_row_address = m1_address->dimension_addresses[m1_address->dimension_count + row_offset];
	m2_col_address = m2_address->dimension_addresses[m2_address->dimension_count + col_offset];

	if(m1_address->dimension_count > 2) {
		if(VARIABLE_LENGTH_ARRAY(m1_address)) {
			m1_count = UINT32_MAX;
		} else {
			m1_count = ARRAY_SIZE(m1_address) / VALUE(U32, m1_size_address);
		}
	} else {
		m1_count = 1;
	}
	if(m2_address->dimension_count > 2) {
		if(VARIABLE_LENGTH_ARRAY(m2_address)) {
			m2_count = UINT32_MAX;
		} else {
			m2_count = ARRAY_SIZE(m2_address) / VALUE(U32, m2_size_address);
		}
	} else {
		m2_count = 1;
	}
	res_count = max(m1_count, m2_count);
	if(res_count > 1) {
		if(res_count == UINT32_MAX) {
			dim->array_size = 0;
		} else {
			dim->array_size = res_count * VALUE(U32, m1_row_address) * VALUE(U32, m2_col_address);
		}
		if(m1_count == UINT32_MAX && m2_count == UINT32_MAX) {
			// can't feature out the result matrix count
			// just leave the entire list as a linear array
			dim->dimension_count = 1;
		} else {
			if(m1_count > m2_count || (m1_count == m2_count && (m1_address->dimension_count - 2) >= (m2_address->dimension_count - 2))) {
				// use the first matrix parameter's dimensions
				dim->dimension_count = (m1_address->dimension_count - 2) + 2;
				for(i = 0; i < m1_address->dimension_count - 2; i++) {
					dim->dimension_addresses[i] = m1_address->dimension_addresses[i];
				}
			} else {
				// use the second matrix parameter's dimensions
				dim->dimension_count = (m2_address->dimension_count - 2) + 2;
				for(i = 0; i < m2_address->dimension_count - 2; i++) {
					dim->dimension_addresses[i] = m2_address->dimension_addresses[i];
				}
			}
			dim->dimension_addresses[dim->dimension_count + row_offset] = m1_row_address;
			dim->dimension_addresses[dim->dimension_count + col_offset] = m2_col_address;
		}
	} else {
		dim->array_size = VALUE(U32, m1_row_address) * VALUE(U32, m2_col_address);
		dim->dimension_count = 2;
		dim->dimension_addresses[dim->dimension_count + row_offset] = m1_row_address;
		dim->dimension_addresses[dim->dimension_count + col_offset] = m2_col_address;
	}
}

static void ZEND_FASTCALL qb_set_result_dimensions_mv_mult(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_variable_dimensions *dim) {
	qb_address *m_address, *m_size_address;
	qb_address *v_address, *v_width_address;
	uint32_t i, m_count, v_count, res_count;

	m_address = operands[0].address;
	v_address = operands[1].address;
	v_width_address = v_address->array_size_addresses[v_address->dimension_count - 1];
	m_size_address = m_address->array_size_addresses[m_address->dimension_count - 2];

	if(v_address->dimension_count > 1) {
		if(VARIABLE_LENGTH_ARRAY(v_address)) {
			v_count = UINT32_MAX;
		} else {
			v_count = ARRAY_SIZE(v_address) / VALUE(U32, v_width_address);
		}
	} else {
		v_count = 1;
	}
	if(m_address->dimension_count > 2) {
		if(VARIABLE_LENGTH_ARRAY(v_address)) {
			m_count = UINT32_MAX;
		} else {
			m_count = ARRAY_SIZE(m_address) / VALUE(U32, m_size_address);
		}
	} else {
		m_count = 1;
	}
	res_count = max(v_count, m_count);
	if(res_count > 1) {
		if(res_count == UINT32_MAX) {
			dim->array_size = 0;
		} else {
			dim->array_size = res_count * VALUE(U32, v_width_address);
		}
		if(m_count == UINT32_MAX && v_count == UINT32_MAX) {
			// can't feature out the vector count
			// just leave it as a linear array
			dim->dimension_count = 1;
		} else {
			if(v_count < m_count || (v_count == m_count && (v_address->dimension_count - 1) <= (m_address->dimension_count - 2))) {
				// use the matrix parameter's dimensions
				dim->dimension_count = (m_address->dimension_count - 2) + 1;
				for(i = 0; i < m_address->dimension_count - 2; i++) {
					dim->dimension_addresses[i] = m_address->dimension_addresses[i];
				}
			} else {
				// use the vector parameter's dimensions
				dim->dimension_count = (v_address->dimension_count - 1) + 1;
				for(i = 0; i < v_address->dimension_count - 1; i++) {
					dim->dimension_addresses[i] = v_address->dimension_addresses[i];
				}
				dim->source_address = v_address;
			}
			dim->dimension_addresses[dim->dimension_count - 1] = v_width_address;
		}
	} else {
		dim->array_size = VALUE(U32, v_width_address);
		dim->dimension_count = 1;
	}
}

static void ZEND_FASTCALL qb_set_result_dimensions_vm_mult(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_variable_dimensions *dim) {
	// TODO: I think this is broken when the matrix isn't square
	qb_operand reversed_operands[2];
	reversed_operands[0] = operands[1];
	reversed_operands[1] = operands[0];
	qb_set_result_dimensions_mv_mult(cxt, f, reversed_operands, operand_count, dim);
}

// make use of the fact that A' * B' = (B * A)' 
static void ZEND_FASTCALL qb_set_result_dimensions_matrix_equivalent(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_variable_dimensions *dim) {
	qb_derived_op_factory *df = (qb_derived_op_factory *) f;
	qb_operand reversed_operands[2];
	reversed_operands[0] = operands[1];
	reversed_operands[1] = operands[0];
	f = df->parent;
	f->set_dimensions(cxt, f, reversed_operands, operand_count, dim);
}

static void ZEND_FASTCALL qb_set_result_dimensions_matrix_current_mode(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_variable_dimensions *dim) {
	qb_matrix_op_factory_selector *s = (qb_matrix_op_factory_selector *) f;
	if(cxt->matrix_order == QB_MATRIX_ORDER_COLUMN_MAJOR) {
		f = s->cm_factory;
	} else {
		f = s->rm_factory;
	}
	f->set_dimensions(cxt, f, operands, operand_count, dim);
}

static void ZEND_FASTCALL qb_set_result_dimensions_transpose(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_variable_dimensions *dim) {
	qb_address *matrix_address = operands[0].address;
	uint32_t i;

	dim->dimension_count = matrix_address->dimension_count;
	dim->array_size = ARRAY_SIZE(matrix_address);
	for(i = 0; i < dim->dimension_count; i++) {
		if(i == dim->dimension_count - 2) {
			dim->dimension_addresses[i] = matrix_address->dimension_addresses[i + 1];
		} else if(i == dim->dimension_count - 1) {
			dim->dimension_addresses[i] = matrix_address->dimension_addresses[i - 1];
		} else {
			dim->dimension_addresses[i] = matrix_address->dimension_addresses[i];
		}
	}
	dim->source_address = matrix_address;
}

static void ZEND_FASTCALL qb_set_result_dimensions_matrix_unary(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_variable_dimensions *dim) {
	qb_address *matrix_address = operands[0].address;
	if(matrix_address->dimension_count > 2) {
		dim->dimension_count = 1;
		if(VARIABLE_LENGTH_ARRAY(matrix_address)) {
			dim->array_size = 0;
		} else {
			qb_address *matrix_size_address = matrix_address->array_size_addresses[matrix_address->dimension_count - 2];
			uint32_t matrix_size = VALUE(U32, matrix_size_address);
			uint32_t element_count = ARRAY_SIZE(matrix_address);
			dim->array_size = element_count / matrix_size;
		}
	}
}

static void ZEND_FASTCALL qb_set_result_dimensions_sampling(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_variable_dimensions *dim) {
	qb_address *image_address = operands[0].address;
	qb_address *x_address = operands[1].address;
	qb_address *y_address = operands[2].address;
	qb_address *channel_count_address = image_address->dimension_addresses[image_address->dimension_count - 1];
	uint32_t channel_count = VALUE(U32, channel_count_address), i;
	if(SCALAR(x_address) && SCALAR(y_address)) {
		dim->array_size = channel_count;
		dim->dimension_count = 1;
	} else {
		if(VARIABLE_LENGTH_ARRAY(x_address) || VARIABLE_LENGTH_ARRAY(y_address)) {
			// don't know how many pixels will be returned
			dim->array_size = 0;
			dim->dimension_count = 1;
		} else {
			uint32_t x_count = SCALAR(x_address) ? 1: ARRAY_SIZE(x_address);
			uint32_t y_count = SCALAR(y_address) ? 1 : ARRAY_SIZE(y_address);
			dim->array_size = max(x_count, y_count) * channel_count;

			if(x_count > y_count || (x_count == y_count && x_address->dimension_count >= y_address->dimension_count)) {
				// use x's dimensions
				dim->dimension_count = x_address->dimension_count + 1;
				for(i = 0; i < x_address->dimension_count; i++) {
					dim->dimension_addresses[i] = x_address->dimension_addresses[i];
				}
			} else {
				// use y's dimensions
				dim->dimension_count = y_address->dimension_count + 1;
				for(i = 0; i < y_address->dimension_count; i++) {
					dim->dimension_addresses[i] = y_address->dimension_addresses[i];
				}
			}
			dim->dimension_addresses[dim->dimension_count - 1] = channel_count_address;
		}
	}
}

static void ZEND_FASTCALL qb_set_result_dimensions_array_merge(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_variable_dimensions *dim) {
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
}

static void ZEND_FASTCALL qb_set_result_dimensions_array_fill(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_variable_dimensions *dim) {
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
}

static void ZEND_FASTCALL qb_set_result_dimensions_array_pad(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_variable_dimensions *dim) {
	qb_address *container_address = operands[0].address;
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
}

static void ZEND_FASTCALL qb_set_result_dimensions_array_column(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_variable_dimensions *dim) {
	qb_address *address1 = operands[0].address;
	if(VARIABLE_LENGTH_ARRAY(address1)) {
		dim->array_size = 0;
	} else {
		dim->array_size = VALUE(U32, address1->dimension_addresses[0]);
		if(address1->dimension_count > 2) {
			dim->array_size *= VALUE(U32, address1->array_size_addresses[2]);
		}
	}
	if(address1->dimension_count > 2) {
		uint32_t i;
		dim->dimension_count = address1->dimension_count - 1;
		dim->dimension_addresses[0] = address1->dimension_addresses[0];
		for(i = 2; i < address1->dimension_count; i++) {
			dim->dimension_addresses[i - 1] = address1->dimension_addresses[i];
		}
	} else {
		dim->dimension_count = 1;
	}
}

uint32_t qb_get_range_length_F32(float32_t op1, float32_t op2, float32_t op3);
uint32_t qb_get_range_length_F64(float64_t op1, float64_t op2, float64_t op3);
uint32_t qb_get_range_length_S08(int8_t op1, int8_t op2, int8_t op3);
uint32_t qb_get_range_length_S16(int16_t op1, int16_t op2, int16_t op3);
uint32_t qb_get_range_length_S32(int32_t op1, int32_t op2, int32_t op3);
uint32_t qb_get_range_length_S64(int64_t op1, int64_t op2, int64_t op3);
uint32_t qb_get_range_length_U08(uint8_t op1, uint8_t op2, int8_t op3);
uint32_t qb_get_range_length_U16(uint16_t op1, uint16_t op2, int16_t op3);
uint32_t qb_get_range_length_U32(uint32_t op1, uint32_t op2, int32_t op3);
uint32_t qb_get_range_length_U64(uint64_t op1, uint64_t op2, int64_t op3);

static void ZEND_FASTCALL qb_set_result_dimensions_range(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_variable_dimensions *dim) {
	// the result size is known only if the all operands are constant
	qb_address *start_address = operands[0].address;
	qb_address *end_address = operands[1].address;
	qb_address *interval_address = (operand_count >= 3) ? operands[2].address : NULL;
	uint32_t current_array_size = UINT32_MAX;
	if(CONSTANT(start_address) && CONSTANT(end_address) && (!interval_address || CONSTANT(interval_address))) {
		switch(start_address->type) {
			case QB_TYPE_S08: current_array_size = qb_get_range_length_S08(VALUE(S08, start_address), VALUE(S08, end_address), (interval_address) ? VALUE(S08, interval_address) : 1); break;
			case QB_TYPE_U08: current_array_size = qb_get_range_length_U08(VALUE(U08, start_address), VALUE(U08, end_address), (interval_address) ? VALUE(U08, interval_address) : 1); break;
			case QB_TYPE_S16: current_array_size = qb_get_range_length_S16(VALUE(S16, start_address), VALUE(S16, end_address), (interval_address) ? VALUE(S16, interval_address) : 1); break;
			case QB_TYPE_U16: current_array_size = qb_get_range_length_U16(VALUE(U16, start_address), VALUE(U16, end_address), (interval_address) ? VALUE(U16, interval_address) : 1); break;
			case QB_TYPE_S32: current_array_size = qb_get_range_length_S32(VALUE(S32, start_address), VALUE(S32, end_address), (interval_address) ? VALUE(S32, interval_address) : 1); break;
			case QB_TYPE_U32: current_array_size = qb_get_range_length_U32(VALUE(U32, start_address), VALUE(U32, end_address), (interval_address) ? VALUE(U32, interval_address) : 1); break;
			case QB_TYPE_S64: current_array_size = qb_get_range_length_S64(VALUE(S64, start_address), VALUE(S64, end_address), (interval_address) ? VALUE(S64, interval_address) : 1); break;
			case QB_TYPE_U64: current_array_size = qb_get_range_length_U64(VALUE(U64, start_address), VALUE(U64, end_address), (interval_address) ? VALUE(U64, interval_address) : 1); break;
			case QB_TYPE_F32: current_array_size = qb_get_range_length_F32(VALUE(F32, start_address), VALUE(F32, end_address), (interval_address) ? VALUE(F32, interval_address) : 1); break;
			case QB_TYPE_F64: current_array_size = qb_get_range_length_F64(VALUE(F64, start_address), VALUE(F64, end_address), (interval_address) ? VALUE(F64, interval_address) : 1); break;
			default: break;
		}
	}
	dim->dimension_count = 1;
	if(current_array_size != UINT32_MAX) {
		dim->array_size = current_array_size;
	} else {
		// the array size isn't known (or the parameters are erroreous)
		dim->array_size = 0;
	}
}

static void ZEND_FASTCALL qb_set_result_dimensions_array_rand(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_variable_dimensions *dim) {
	qb_address *count_address = operands[1].address;
	if(CONSTANT(count_address)) {
		uint32_t count = VALUE(U32, count_address);
		dim->array_size = count;
		dim->dimension_count = (count == 1) ? 0 : 1;
	} else {
		// don't know how many elements will be returned
		dim->dimension_count = 1;
		dim->array_size = 0;
	}
}
