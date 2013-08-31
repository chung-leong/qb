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

static void ZEND_FASTCALL qb_translate_intrinsic_unary_vector_op(qb_compiler_context *cxt, qb_intrinsic_function *f, qb_operand *arguments, uint32_t argument_count, qb_operand *result, qb_result_prototype *result_prototype) {
	qb_primitive_type expr_type = qb_do_type_coercion_for_op(cxt, f->extra, arguments, argument_count, result_prototype);

	if(cxt->stage == QB_STAGE_RESULT_TYPE_RESOLUTION) {
		if(result->type != QB_OPERAND_NONE) {
			result->type = QB_OPERAND_RESULT_PROTOTYPE;
			result->result_prototype = result_prototype;
		}
	} else if(cxt->stage == QB_STAGE_OPCODE_TRANSLATION) {
		qb_address *address = arguments[0].address;
		uint32_t vector_width;

		if(address->dimension_count == 1 && IS_EXPANDABLE_ARRAY(address)) {
			qb_abort("%s() expects an array with fixed dimension as parameter", f->name);
		}

		vector_width = VALUE(U32, address->dimension_addresses[address->dimension_count - 1]);
		if(vector_width > 4096) {
			qb_abort("Dimension is too large");
		}
		if(result->type != QB_OPERAND_NONE) {
			result->type = QB_OPERAND_ADDRESS;
			result->address = qb_obtain_result_storage(cxt, f->extra, arguments, argument_count, expr_type, result_prototype);
			qb_create_op(cxt, f->extra, arguments, argument_count, result);
		}
	}
}

static void ZEND_FASTCALL qb_translate_intrinsic_binary_vector_op(qb_compiler_context *cxt, qb_intrinsic_function *f, qb_operand *arguments, uint32_t argument_count, qb_operand *result, qb_result_prototype *result_prototype) {
	qb_primitive_type expr_type = qb_do_type_coercion_for_op(cxt, f->extra, arguments, argument_count, result_prototype);

	if(cxt->stage == QB_STAGE_RESULT_TYPE_RESOLUTION) {
		if(result->type != QB_OPERAND_NONE) {
			result->type = QB_OPERAND_RESULT_PROTOTYPE;
			result->result_prototype = result_prototype;
		}
	} else if(cxt->stage == QB_STAGE_OPCODE_TRANSLATION) {
		qb_address *address1 = arguments[0].address;
		qb_address *address2 = arguments[1].address;
		uint32_t vector_width1, vector_width2;

		if((address1->dimension_count == 1 && IS_EXPANDABLE_ARRAY(address1))
		|| (address2->dimension_count == 1 && IS_EXPANDABLE_ARRAY(address2))) {
			qb_abort("%s() expects arrays with one fixed dimension as parameters", f->name);
		}

		vector_width1 = VALUE(U32, address1->dimension_addresses[address1->dimension_count - 1]);
		vector_width2 = VALUE(U32, address2->dimension_addresses[address2->dimension_count - 1]);
		if(vector_width1 != vector_width2) {
			qb_abort("Dimension of first parameter (%d) does not match dimension of second parameter (%d)", vector_width1, vector_width2);
		}
		if(vector_width1 > 4096) {
			qb_abort("Dimension is too large");
		}
		if(result->type != QB_OPERAND_NONE) {
			result->type = QB_OPERAND_ADDRESS;
			result->address = qb_obtain_result_storage(cxt, f->extra, arguments, argument_count, expr_type, result_prototype);
			qb_create_op(cxt, f->extra, arguments, argument_count, result);
		}
	}
}

static void ZEND_FASTCALL qb_translate_intrinsic_cross(qb_compiler_context *cxt, qb_intrinsic_function *f, qb_operand *arguments, uint32_t argument_count, qb_operand *result, qb_result_prototype *result_prototype) {
	qb_primitive_type expr_type = qb_do_type_coercion_for_op(cxt, f->extra, arguments, argument_count, result_prototype);

	if(cxt->stage == QB_STAGE_RESULT_TYPE_RESOLUTION) {
		if(result->type != QB_OPERAND_NONE) {
			result->type = QB_OPERAND_RESULT_PROTOTYPE;
			result->result_prototype = result_prototype;
		}
	} else if(cxt->stage == QB_STAGE_OPCODE_TRANSLATION) {
		qb_address *address1 = arguments[0].address;
		qb_address *address2 = arguments[1].address;
		qb_address *address3 = (argument_count >= 3) ? arguments[0].address : NULL;
		uint32_t vector_width1, vector_width2, vector_width3;

		if((address1->dimension_count == 1 && IS_EXPANDABLE_ARRAY(address1))
		|| (address2->dimension_count == 1 && IS_EXPANDABLE_ARRAY(address2))) {
			qb_abort("%s() expects arrays with one fixed dimension as parameters", f->name);
		}

		vector_width1 = VALUE(U32, address1->dimension_addresses[address1->dimension_count - 1]);
		vector_width2 = VALUE(U32, address2->dimension_addresses[address2->dimension_count - 1]);
		if(vector_width1 != vector_width2) {
			qb_abort("Dimension of first parameter (%d) does not match dimension of second parameter (%d)", vector_width1, vector_width2);
		}
		if(address3) {
			vector_width3 = VALUE(U32, address3->dimension_addresses[address3->dimension_count - 1]);
			if(vector_width1 != vector_width3) {
				qb_abort("Dimension of first parameter (%d) does not match dimension of third parameter (%d)", vector_width1, vector_width3);
			}
		}
		if(!(vector_width1 >= 2 && vector_width1 <= 4)) {
			qb_abort("%s() only accepts two, three, and four-dimensional vectors", f->name);
		}
		if(address3 && vector_width1 != 4) {
			qb_abort("%s() only accepts a third parameter when given four-dimensional vectors", f->name);
		} else if(!address3 && vector_width1 == 4) {
			qb_abort("%s() requires a third parameter when given four-dimensional vectors", f->name);
		}
		if(result->type != QB_OPERAND_NONE) {
			result->type = QB_OPERAND_ADDRESS;
			result->address = qb_obtain_result_storage(cxt, f->extra, arguments, argument_count, expr_type, result_prototype);
			qb_create_op(cxt, f->extra, arguments, argument_count, result);
		}
	}
}

static void ZEND_FASTCALL qb_translate_intrinsic_mm_mult(qb_compiler_context *cxt, qb_intrinsic_function *f, qb_operand *arguments, uint32_t argument_count, qb_operand *result, qb_result_prototype *result_prototype) {
	qb_primitive_type expr_type = qb_do_type_coercion_for_op(cxt, f->extra, arguments, argument_count, result_prototype);

	if(cxt->stage == QB_STAGE_RESULT_TYPE_RESOLUTION) {
		if(result->type != QB_OPERAND_NONE) {
			result->type = QB_OPERAND_RESULT_PROTOTYPE;
			result->result_prototype = result_prototype;
		}
	} else if(cxt->stage == QB_STAGE_OPCODE_TRANSLATION) {
		qb_address *m1_address, *m1_row_address, *m1_col_address;
		qb_address *m2_address, *m2_row_address, *m2_col_address;
		uint32_t m1_cols, m1_rows, m2_cols, m2_rows;

		qb_matrix_order order = qb_get_matrix_order(cxt, f);
		int32_t column_offset = (order == QB_MATRIX_ORDER_COLUMN_MAJOR) ? -2 : -1;
		int32_t row_offset = (order == QB_MATRIX_ORDER_ROW_MAJOR) ? -2 : -1;

		m1_address = arguments[0].address;
		m2_address = arguments[1].address;
		if(!(m1_address->dimension_count >= 2 && m2_address->dimension_count >= 2)) {
			qb_abort("%s() expects two-dimensional arrays as parameters", f->name);
		}
		if((m1_address->dimension_count == 2 && IS_EXPANDABLE_ARRAY(m1_address))
		|| (m2_address->dimension_count == 2 && IS_EXPANDABLE_ARRAY(m2_address))) {
			qb_abort("%s() expects arrays with two fixed dimensions as parameters", f->name);
		}

		m1_col_address = m1_address->dimension_addresses[m1_address->dimension_count + column_offset];
		m2_col_address = m2_address->dimension_addresses[m2_address->dimension_count + column_offset];
		m1_row_address = m1_address->dimension_addresses[m1_address->dimension_count + row_offset];
		m2_row_address = m2_address->dimension_addresses[m2_address->dimension_count + row_offset];
		m1_cols = VALUE(U32, m1_col_address);
		m1_rows = VALUE(U32, m1_row_address);
		m2_cols = VALUE(U32, m2_col_address);
		m2_rows = VALUE(U32, m2_row_address);
		if(m1_cols != m2_rows) {
			qb_abort("Number of columns in first matrix (%d) does not match number of rows in second matrix (%d)", m1_cols, m2_rows);
		}
		if(m1_rows > 4096 || m1_cols > 4096 || m2_cols > 4096) {
			qb_abort("Dimension is too large");
		}
		if(result->type != QB_OPERAND_NONE) {
			result->type = QB_OPERAND_ADDRESS;
			result->address = qb_obtain_result_storage(cxt, f->extra, arguments, argument_count, expr_type, result_prototype);
			qb_create_op(cxt, f->extra, arguments, argument_count, result);
		}
	}
}

static void ZEND_FASTCALL qb_translate_intrinsic_mv_mult(qb_compiler_context *cxt, qb_intrinsic_function *f, qb_operand *arguments, uint32_t argument_count, qb_operand *result, qb_result_prototype *result_prototype) {
	qb_primitive_type expr_type = qb_do_type_coercion_for_op(cxt, f->extra, arguments, argument_count, result_prototype);

	if(cxt->stage == QB_STAGE_RESULT_TYPE_RESOLUTION) {
		if(result->type != QB_OPERAND_NONE) {
			result->type = QB_OPERAND_RESULT_PROTOTYPE;
			result->result_prototype = result_prototype;
		}
	} else if(cxt->stage == QB_STAGE_OPCODE_TRANSLATION) {
		qb_address *m_address, *m_row_address, *m_col_address;
		qb_address *v_address, *v_width_address;
		uint32_t m_cols, m_rows, v_width;

		qb_matrix_order order = qb_get_matrix_order(cxt, f);
		int32_t column_offset = (order == QB_MATRIX_ORDER_COLUMN_MAJOR) ? -2 : -1;
		int32_t row_offset = (order == QB_MATRIX_ORDER_ROW_MAJOR) ? -2 : -1;

		m_address = arguments[0].address;
		v_address = arguments[1].address;
		if(!(m_address->dimension_count >= 2)) {
			qb_abort("%s() expects a two-dimensional array as first parameter", f->name);
		}
		if(m_address->dimension_count == 2 && IS_EXPANDABLE_ARRAY(m_address)) {
			qb_abort("%s() expects an array with two fixed dimensions as first parameter", f->name);
		}
		if(!(v_address->dimension_count >= 1)) {
			qb_abort("%s() expects an array as second parameter", f->name);
		}
		if(v_address->dimension_count == 1 && IS_EXPANDABLE_ARRAY(v_address)) {
			qb_abort("%s() expects an array with fixed dimension as second parameter", f->name);
		}

		m_col_address = m_address->dimension_addresses[m_address->dimension_count + column_offset];
		m_row_address = m_address->dimension_addresses[m_address->dimension_count + row_offset];
		v_width_address = v_address->dimension_addresses[v_address->dimension_count - 1];
		m_cols = VALUE(U32, m_col_address);
		m_rows = VALUE(U32, m_row_address);
		v_width = VALUE(U32, v_width_address);
		if(m_cols != v_width) {
			qb_abort("Number of columns in matrix (%d) does not match number of elements in vector (%d)", m_cols, v_width);
		}
		if(v_width > 4096 || m_rows > 4096) {
			qb_abort("Dimension is too large");
		}
		if(result->type != QB_OPERAND_NONE) {
			result->type = QB_OPERAND_ADDRESS;
			result->address = qb_obtain_result_storage(cxt, f->extra, arguments, argument_count, expr_type, result_prototype);
			qb_create_op(cxt, f->extra, arguments, argument_count, result);
		}
	}
}

static void ZEND_FASTCALL qb_translate_intrinsic_vm_mult(qb_compiler_context *cxt, qb_intrinsic_function *f, qb_operand *arguments, uint32_t argument_count, qb_operand *result, qb_result_prototype *result_prototype) {
	qb_primitive_type expr_type = qb_do_type_coercion_for_op(cxt, f->extra, arguments, argument_count, result_prototype);

	if(cxt->stage == QB_STAGE_RESULT_TYPE_RESOLUTION) {
		if(result->type != QB_OPERAND_NONE) {
			result->type = QB_OPERAND_RESULT_PROTOTYPE;
			result->result_prototype = result_prototype;
		}
	} else if(cxt->stage == QB_STAGE_OPCODE_TRANSLATION) {
		qb_address *m_address, *m_row_address, *m_col_address;
		qb_address *v_address, *v_width_address;
		uint32_t m_cols, m_rows, v_width;

		qb_matrix_order order = qb_get_matrix_order(cxt, f);
		int32_t column_offset = (order == QB_MATRIX_ORDER_COLUMN_MAJOR) ? -2 : -1;
		int32_t row_offset = (order == QB_MATRIX_ORDER_ROW_MAJOR) ? -2 : -1;

		v_address = arguments[0].address;
		m_address = arguments[1].address;
		if(!(v_address->dimension_count >= 1)) {
			qb_abort("%s() expects an array as first parameter", f->name);
		}
		if(v_address->dimension_count == 1 && IS_EXPANDABLE_ARRAY(v_address)) {
			qb_abort("%s() expects an array with fixed dimension as first parameter", f->name);
		}
		if(!(m_address->dimension_count >= 2)) {
			qb_abort("%s() expects a two-dimensional array as second parameter", f->name);
		}
		if(m_address->dimension_count == 2 && IS_EXPANDABLE_ARRAY(m_address)) {
			qb_abort("%s() expects an array with two fixed dimensions as second parameter", f->name);
		}

		m_col_address = m_address->dimension_addresses[m_address->dimension_count + column_offset];
		m_row_address = m_address->dimension_addresses[m_address->dimension_count + row_offset];
		v_width_address = v_address->dimension_addresses[v_address->dimension_count - 1];
		m_cols = VALUE(U32, m_col_address);
		m_rows = VALUE(U32, m_row_address);
		v_width = VALUE(U32, v_width_address);
		if(v_width != m_rows) {
			qb_abort("Number of elements in vector (%d) does not match number of rows in matrix (%d)", v_width, m_rows);
		}
		if(v_width > 4096 || m_cols > 4096) {
			qb_abort("Dimension is too large");
		}

		if(result->type != QB_OPERAND_NONE) {
			result->type = QB_OPERAND_ADDRESS;
			result->address = qb_obtain_result_storage(cxt, f->extra, arguments, argument_count, expr_type, result_prototype);
			qb_create_op(cxt, f->extra, arguments, argument_count, result);
		}
	}
}

static void ZEND_FASTCALL qb_translate_intrinsic_transform(qb_compiler_context *cxt, qb_intrinsic_function *f, qb_operand *arguments, uint32_t argument_count, qb_operand *result, qb_result_prototype *result_prototype) {
	qb_primitive_type expr_type = qb_do_type_coercion_for_op(cxt, f->extra, arguments, argument_count, result_prototype);

	if(cxt->stage == QB_STAGE_RESULT_TYPE_RESOLUTION) {
		if(result->type != QB_OPERAND_NONE) {
			result->type = QB_OPERAND_RESULT_PROTOTYPE;
			result->result_prototype = result_prototype;
		}
	} else if(cxt->stage == QB_STAGE_OPCODE_TRANSLATION) {
		qb_address *m_address, *m_row_address, *m_col_address;
		qb_address *v_address, *v_width_address;
		uint32_t m_cols, m_rows, v_width;

		qb_matrix_order order = qb_get_matrix_order(cxt, f);
		int32_t column_offset = (order == QB_MATRIX_ORDER_COLUMN_MAJOR) ? -2 : -1;
		int32_t row_offset = (order == QB_MATRIX_ORDER_ROW_MAJOR) ? -2 : -1;

		m_address = arguments[0].address;
		v_address = arguments[1].address;
		if(!(m_address->dimension_count >= 2)) {
			qb_abort("%s() expects a two-dimensional array as first parameter", f->name);
		}
		if(m_address->dimension_count == 2 && IS_EXPANDABLE_ARRAY(m_address)) {
			qb_abort("%s() expects an array with two fixed dimensions as first parameter", f->name);
		}
		if(!(v_address->dimension_count >= 1)) {
			qb_abort("%s() expects an array as second parameter", f->name);
		}
		if(v_address->dimension_count == 1 && IS_EXPANDABLE_ARRAY(v_address)) {
			qb_abort("%s() expects an array with fixed dimension as second parameter", f->name);
		}

		m_col_address = m_address->dimension_addresses[m_address->dimension_count + column_offset];
		m_row_address = m_address->dimension_addresses[m_address->dimension_count + row_offset];
		v_width_address = v_address->dimension_addresses[v_address->dimension_count - 1];
		m_cols = VALUE(U32, m_col_address);
		m_rows = VALUE(U32, m_row_address);
		v_width = VALUE(U32, v_width_address);
		if(!(v_width >= 2 && v_width <= 4)) {
			qb_abort("%s() can only handle vectors with 2, 3, or 4 elements", f->name);
		}
		if(m_cols != v_width + 1 || m_rows != v_width) {
			qb_abort("%s() expects a %dx%d matrix when given a vector with %d elements", f->name, v_width, v_width + 1, v_width);
		}
		if(result->type != QB_OPERAND_NONE) {
			result->type = QB_OPERAND_ADDRESS;
			result->address = qb_obtain_result_storage(cxt, f->extra, arguments, argument_count, expr_type, result_prototype);
			qb_create_op(cxt, f->extra, arguments, argument_count, result);
		}
	}
}

static void ZEND_FASTCALL qb_translate_intrinsic_sample_op(qb_compiler_context *cxt, qb_intrinsic_function *f, qb_operand *arguments, uint32_t argument_count, qb_operand *result, qb_result_prototype *result_prototype) {
	qb_operand *image = &arguments[0], *x = &arguments[1], *y = &arguments[2];

	qb_do_type_coercion(cxt, image, QB_TYPE_ANY);
	qb_do_type_coercion(cxt, x, image->address->type);
	qb_do_type_coercion(cxt, y, image->address->type);

	if(cxt->stage == QB_STAGE_RESULT_TYPE_RESOLUTION) {
		if(result->type != QB_OPERAND_NONE) {
			result->type = QB_OPERAND_RESULT_PROTOTYPE;
			result->result_prototype = result_prototype;
		}
	} else if(cxt->stage == QB_STAGE_OPCODE_TRANSLATION) {
		uint32_t channel_count = (image->address->dimension_count == 3) ? VALUE(U32, image->address->dimension_addresses[image->address->dimension_count - 1]) : 0;
		if(!(image->address->type >= QB_TYPE_F32 && (channel_count == 3 || channel_count == 4))) {
			qb_abort("%s() expects an image in floating-point representation", f->name);
		}

		if(result->type != QB_OPERAND_NONE) {
			uint32_t result_flags = qb_get_result_flags(cxt, f->extra);
			qb_variable_dimensions *result_dim;
			result_dim = qb_get_result_dimensions(cxt, f->extra, arguments, argument_count);
			result->type = QB_OPERAND_ADDRESS;
			result->address = qb_obtain_write_target_address(cxt, image->address->type, result_dim, result_prototype, result_flags);
			qb_create_op(cxt, f->extra, arguments, argument_count, result);
		}
	}
}

static void ZEND_FASTCALL qb_translate_intrinsic_blend(qb_compiler_context *cxt, qb_intrinsic_function *f, qb_operand *arguments, uint32_t argument_count, qb_operand *result, qb_result_prototype *result_prototype) {
	qb_primitive_type expr_type = qb_do_type_coercion_for_op(cxt, f->extra, arguments, argument_count, result_prototype);

	if(cxt->stage == QB_STAGE_RESULT_TYPE_RESOLUTION) {
		if(result->type != QB_OPERAND_NONE) {
			result->type = QB_OPERAND_RESULT_PROTOTYPE;
			result->result_prototype = result_prototype;
		}
	} else if(cxt->stage == QB_STAGE_OPCODE_TRANSLATION) {
		qb_address *address1 = arguments[0].address;
		qb_address *address2 = arguments[1].address;
		uint32_t vector_width1, vector_width2;

		if((address1->dimension_count == 1 && IS_EXPANDABLE_ARRAY(address1))
		|| (address2->dimension_count == 1 && IS_EXPANDABLE_ARRAY(address2))) {
			qb_abort("%s() expects arrays with one fixed dimension as parameters", f->name);
		}

		vector_width1 = VALUE(U32, address1->dimension_addresses[address1->dimension_count - 1]);
		vector_width2 = VALUE(U32, address2->dimension_addresses[address2->dimension_count - 1]);
		if(vector_width1 != 4 || vector_width2 != 4) {
			qb_abort("%s() only accepts four-channel pixels", f->name);
		}
		if(result->type != QB_OPERAND_NONE) {
			result->type = QB_OPERAND_ADDRESS;
			result->address = qb_obtain_result_storage(cxt, f->extra, arguments, argument_count, expr_type, result_prototype);
			qb_create_op(cxt, f->extra, arguments, argument_count, result);
		}
	}
}

static void ZEND_FASTCALL qb_translate_intrinsic_complex(qb_compiler_context *cxt, qb_intrinsic_function *f, qb_operand *arguments, uint32_t argument_count, qb_operand *result, qb_result_prototype *result_prototype) {
	qb_primitive_type expr_type = qb_do_type_coercion_for_op(cxt, f->extra, arguments, argument_count, result_prototype);

	if(cxt->stage == QB_STAGE_RESULT_TYPE_RESOLUTION) {
		if(result->type != QB_OPERAND_NONE) {
			result->type = QB_OPERAND_RESULT_PROTOTYPE;
			result->result_prototype = result_prototype;
		}
	} else if(cxt->stage == QB_STAGE_OPCODE_TRANSLATION) {
		uint32_t i;
		for(i = 0; i < argument_count; i++) {
			qb_address *address = arguments[i].address;
			uint32_t vector_width;

			if((address->dimension_count == 1 && IS_EXPANDABLE_ARRAY(address))) {
				qb_abort("%s() expects arrays with one fixed dimension as parameters", f->name);
			}

			vector_width = VALUE(U32, address->dimension_addresses[address->dimension_count - 1]);
			if(vector_width != 2) {
				qb_abort("%s() expects complex numbers expressed as two-dimensional arrays", f->name);
			}
		}
		if(result->type != QB_OPERAND_NONE) {
			result->type = QB_OPERAND_ADDRESS;
			result->address = qb_obtain_result_storage(cxt, f->extra, arguments, argument_count, expr_type, result_prototype);
			qb_create_op(cxt, f->extra, arguments, argument_count, result);
		}
	}
}

static void ZEND_FASTCALL qb_translate_intrinsic_count(qb_compiler_context *cxt, qb_intrinsic_function *f, qb_operand *arguments, uint32_t argument_count, qb_operand *result, qb_result_prototype *result_prototype) {
	if(cxt->stage == QB_STAGE_RESULT_TYPE_RESOLUTION) {
		if(result->type != QB_OPERAND_NONE) {
			result_prototype->preliminary_type = result_prototype->final_type = QB_TYPE_U32;
			result->type = QB_OPERAND_RESULT_PROTOTYPE;
			result->result_prototype = result_prototype;
		}
	} else if(cxt->stage == QB_STAGE_OPCODE_TRANSLATION) {
		int32_t recursive = 0;
		qb_address *address = arguments[0].address;
		qb_do_type_coercion(cxt, &arguments[0], QB_TYPE_ANY);
		if(argument_count == 2) {
			qb_do_type_coercion(cxt, &arguments[1], QB_TYPE_I32);
			if(arguments[1].address->flags & QB_ADDRESS_CONSTANT) {
				recursive = VALUE(I32, arguments[1].address);
			} else {
				qb_abort("%s() expects parameter 2 to be a constant expression", f->name);
			}
		}

		if(result->type != QB_OPERAND_NONE) {
			result->type = QB_OPERAND_ADDRESS;
			if(!IS_SCALAR(address)) {
				result->address = (recursive) ? address->array_size_addresses[0] : address->dimension_addresses[0];
			} else {
				result->address = qb_obtain_constant_U32(cxt, 1);
			}
		}
	}
}

static void ZEND_FASTCALL qb_translate_intrinsic_rand(qb_compiler_context *cxt, qb_intrinsic_function *f, qb_operand *arguments, uint32_t argument_count, qb_operand *result, qb_result_prototype *result_prototype) {
	qb_primitive_type expr_type = qb_do_type_coercion_for_op(cxt, f->extra, arguments, argument_count, result_prototype);

	if(cxt->stage == QB_STAGE_RESULT_TYPE_RESOLUTION) {
		if(result->type != QB_OPERAND_NONE) {
			result->type = QB_OPERAND_RESULT_PROTOTYPE;
			result->result_prototype = result_prototype;
		}
	} else if(cxt->stage == QB_STAGE_OPCODE_TRANSLATION) {
		qb_address *min_address, *max_address;
		qb_variable_dimensions _result_dim, *result_dim = &_result_dim;

		if(argument_count != 0 && argument_count != 2) {
			qb_abort("%s() expects either 0 or 2 arguments", f->name);
		}

		// use the lvalue type to determine the type
		if(expr_type == QB_TYPE_F32 || expr_type == QB_TYPE_F64) {
			expr_type = QB_TYPE_I32;
		}

		if(argument_count == 0) {
			min_address = qb_obtain_constant(cxt, 0, expr_type);
			max_address = qb_obtain_constant(cxt, (int64_t) integer_upper_bounds[expr_type], expr_type);			
		} else {
			min_address = arguments[0].address;
			max_address = arguments[1].address;
		}
		result_dim = qb_get_result_dimensions(cxt, f->extra, arguments, argument_count);

		if(result->type != QB_OPERAND_NONE) {
			uint32_t result_flags = qb_get_result_flags(cxt, f->extra);
			qb_address *lvalue_size_address = qb_obtain_write_target_size_address(cxt, result_prototype);

			// use the lvalue size if the parameters are scalar
			// this allow use to generate multiple random numbers in a single call
			if(lvalue_size_address && result_dim->dimension_count == 0) {
				result_dim->array_size = VALUE(U32, lvalue_size_address);
				result_dim->dimension_count = 1;
			}
			result->type = QB_OPERAND_ADDRESS;
			result->address = qb_obtain_write_target_address(cxt, expr_type, result_dim, result_prototype, result_flags);
			qb_create_binary_op(cxt, f->extra, min_address, max_address, result->address);
		}
	}
}

static void ZEND_FASTCALL qb_translate_intrinsic_round(qb_compiler_context *cxt, qb_intrinsic_function *f, qb_operand *arguments, uint32_t argument_count, qb_operand *result, qb_result_prototype *result_prototype) {
	qb_operand *value = &arguments[0], *precision = &arguments[1], *mode = &arguments[2];
	qb_primitive_type expr_type = qb_get_operand_type(cxt, &arguments[0], QB_COERCE_TO_FLOATING_POINT);
	qb_do_type_coercion(cxt, &arguments[0], expr_type);

	if(cxt->stage == QB_STAGE_RESULT_TYPE_RESOLUTION) {
		if(result->type != QB_OPERAND_NONE) {
			result_prototype->preliminary_type = expr_type;
			result->type = QB_OPERAND_RESULT_PROTOTYPE;
			result->result_prototype = result_prototype;
		}
	} else if(cxt->stage == QB_STAGE_OPCODE_TRANSLATION) {
		qb_address *precision_address, *mode_address, *address = value->address;
		qb_variable_dimensions *result_dim;

		if(argument_count >= 2) {
			qb_do_type_coercion(cxt, precision, QB_TYPE_S32);
			precision_address = precision->address;
		} else {
			precision_address = qb_obtain_constant_S32(cxt, 0);
		}
		if(argument_count >= 3) {
			qb_do_type_coercion(cxt, mode, QB_TYPE_S32);
			mode_address = mode->address;
		} else {
			mode_address = qb_obtain_constant_S32(cxt, 1);	//	PHP_ROUND_HALF_UP
		}

		if(result->type != QB_OPERAND_NONE) {
			uint32_t result_flags = qb_get_result_flags(cxt, f->extra);
			result_dim = qb_get_result_dimensions(cxt, f->extra, arguments, argument_count);
			result->type = QB_OPERAND_ADDRESS;
			result->address = qb_obtain_write_target_address(cxt, expr_type, result_dim, result_prototype, result_flags);
			qb_create_ternary_op(cxt, f->extra, address, precision_address, mode_address, result->address);
		}

	}
}

static void ZEND_FASTCALL qb_translate_intrinsic_array_push(qb_compiler_context *cxt, qb_intrinsic_function *f, qb_operand *arguments, uint32_t argument_count, qb_operand *result, qb_result_prototype *result_prototype) {
	qb_operand *container = &arguments[0], *value;

	if(cxt->stage == QB_STAGE_RESULT_TYPE_RESOLUTION) {
		result_prototype->preliminary_type = result_prototype->final_type = QB_TYPE_U32;
		result_prototype->address_flags = QB_ADDRESS_TEMPORARY;
		result->type = QB_OPERAND_RESULT_PROTOTYPE;
		result->result_prototype = result_prototype;
	} else if(cxt->stage == QB_STAGE_OPCODE_TRANSLATION) {
		qb_address *size_address, *variable_address;
		uint32_t i, container_element_size, value_size;
		if(container->type != QB_OPERAND_ADDRESS || IS_SCALAR(container->address)) {
			qb_abort("%s expects an array as parameter", f->name);
		}
		if(!IS_EXPANDABLE_ARRAY(container->address)) {
			qb_abort("adding element to an array that cannot expand");
		}
		if(container->address->dimension_count > 1) {
			container_element_size = VALUE(U32, container->address->array_size_addresses[container->address->dimension_count - 1]);
		} else {
			container_element_size = 1;
		}
		for(i = 1; i < argument_count; i++) {
			value = &arguments[i];
			qb_do_type_coercion(cxt, value, container->address->type);
			if(value->address->dimension_count > 0) {
				value_size = ARRAY_SIZE(value->address);
			} else {
				value_size = 1;
			}
			if(value_size != container_element_size) {
				qb_abort("elements in parameter %d are of different size", i + 1);
			}
		}
		size_address = container->address->dimension_addresses[0];
		variable_address = qb_get_array_element(cxt, container->address, size_address);

		for(i = 1; i < argument_count; i++) {
			value = &arguments[i];
			qb_do_assignment(cxt, value->address, variable_address);
		}
		if(result->type != QB_OPERAND_NONE) {
			result->type = QB_OPERAND_ADDRESS;
			result->address = size_address;
		}
	}
}

static void ZEND_FASTCALL qb_translate_intrinsic_array_unshift(qb_compiler_context *cxt, qb_intrinsic_function *f, qb_operand *arguments, uint32_t argument_count, qb_operand *result, qb_result_prototype *result_prototype) {
	qb_operand *container = &arguments[0], *value;

	if(cxt->stage == QB_STAGE_RESULT_TYPE_RESOLUTION) {
		result_prototype->preliminary_type = result_prototype->final_type = QB_TYPE_U32;
		result_prototype->address_flags = QB_ADDRESS_TEMPORARY;
		result->type = QB_OPERAND_RESULT_PROTOTYPE;
		result->result_prototype = result_prototype;
	} else if(cxt->stage == QB_STAGE_OPCODE_TRANSLATION) {
		qb_address *size_address, *zero_address;
		uint32_t i, container_element_size, value_size;
		if(container->type != QB_OPERAND_ADDRESS || IS_SCALAR(container->address)) {
			qb_abort("%s expects an array as parameter", f->name);
		}
		if(!IS_EXPANDABLE_ARRAY(container->address)) {
			qb_abort("adding element to an array that cannot expand");
		}
		if(container->address->dimension_count > 1) {
			container_element_size = VALUE(U32, container->address->array_size_addresses[container->address->dimension_count - 1]);
		} else {
			container_element_size = 1;
		}
		for(i = 1; i < argument_count; i++) {
			value = &arguments[i];
			qb_do_type_coercion(cxt, value, container->address->type);
			if(value->address->dimension_count > 0) {
				value_size = ARRAY_SIZE(value->address);
			} else {
				value_size = 1;
			}
			if(value_size != container_element_size) {
				qb_abort("elements in parameter %d are of different size", i + 1);
			}
		}
		size_address = container->address->dimension_addresses[0];
		zero_address = qb_obtain_constant_U32(cxt, 0);

		for(i = argument_count - 1; i >= 1; i--) {
			value = &arguments[i];
			qb_create_binary_op(cxt, &factory_array_insert, value->address, zero_address, container->address);
		}
		if(result->type != QB_OPERAND_NONE) {
			result->type = QB_OPERAND_ADDRESS;
			result->address = size_address;
		}
	}
}

static void ZEND_FASTCALL qb_translate_intrinsic_array_merge(qb_compiler_context *cxt, qb_intrinsic_function *f, qb_operand *arguments, uint32_t argument_count, qb_operand *result, qb_result_prototype *result_prototype) {
	qb_primitive_type expr_type = qb_do_type_coercion_for_op(cxt, f->extra, arguments, argument_count, result_prototype);

	if(cxt->stage == QB_STAGE_RESULT_TYPE_RESOLUTION) {
		result_prototype->preliminary_type = expr_type;
		result_prototype->address_flags = QB_ADDRESS_TEMPORARY;
		result->type = QB_OPERAND_RESULT_PROTOTYPE;
		result->result_prototype = result_prototype;
	} else if(cxt->stage == QB_STAGE_OPCODE_TRANSLATION) {
		uint32_t i, first_element_size;
		qb_address *first_address = arguments[0].address;
		// make sure the element size match up
		if(first_address->dimension_count > 1) {
			first_element_size = VALUE(U32, first_address->array_size_addresses[1]);
		} else {
			first_element_size = 1;
		}
		for(i = 1; i < argument_count; i++) {
			uint32_t element_size;
			qb_address *address = arguments[i].address;
			if(address->dimension_count > 1) {
				element_size = VALUE(U32, address->array_size_addresses[1]);
			} else {
				element_size = 1;
			}
			if(element_size != first_element_size) {
				qb_abort("elements in parameter %d are of different size", i + 1);
			}
		}
		if(result->type != QB_OPERAND_NONE) {
			uint32_t result_flags = qb_get_result_flags(cxt, f->extra);
			qb_variable_dimensions *result_dim = qb_get_result_dimensions(cxt, f->extra, arguments, argument_count);
			uint32_t current_offset = 0;
			result->type = QB_OPERAND_ADDRESS;
			result->address = qb_obtain_write_target_address(cxt, expr_type, result_dim, result_prototype, result_flags);

			for(i = 0; i < argument_count; i++) {
				qb_address *address = arguments[i].address;
				qb_address *target_address, *length_address, *offset_address;
				if(IS_SCALAR(address)) {
					length_address = qb_obtain_constant_U32(cxt, 1);
				} else {
					length_address = address->dimension_addresses[0];
				}
				if(IS_FIXED_LENGTH_ARRAY(result->address)) {
					// fixed-length result (all arguments are fixed-length)
					offset_address = qb_obtain_constant_U32(cxt, current_offset);
					current_offset += VALUE(U32, length_address);
				} else {
					// variable-length result (length starts out as zero)
					offset_address = result->address->array_size_address;
				}
				target_address = qb_get_array_slice(cxt, result->address, offset_address, length_address);
				qb_create_unary_op(cxt, f->extra, address, target_address);
			}
		}
	}
}

static void ZEND_FASTCALL qb_translate_intrinsic_array_fill(qb_compiler_context *cxt, qb_intrinsic_function *f, qb_operand *arguments, uint32_t argument_count, qb_operand *result, qb_result_prototype *result_prototype) {
	qb_operand *index = &arguments[0], *number = &arguments[1], *value = &arguments[2];
	uint32_t coercion_flags = qb_get_coercion_flags(cxt, f->extra);
	qb_primitive_type expr_type = qb_get_operand_type(cxt, value, coercion_flags);
	qb_do_type_coercion(cxt, index, QB_TYPE_U32);
	qb_do_type_coercion(cxt, number, QB_TYPE_U32);

	// use the lvalue type if it's higher
	if(result_prototype->final_type != QB_TYPE_UNKNOWN) {
		if(result_prototype->final_type > expr_type) {
			expr_type = result_prototype->final_type;
		}
	}
	qb_do_type_coercion(cxt, value, expr_type);

	if(cxt->stage == QB_STAGE_RESULT_TYPE_RESOLUTION) {
		result_prototype->preliminary_type = expr_type;
		result_prototype->address_flags = QB_ADDRESS_TEMPORARY;
		result->type = QB_OPERAND_RESULT_PROTOTYPE;
		result->result_prototype = result_prototype;
	} else if(cxt->stage == QB_STAGE_OPCODE_TRANSLATION) {
		if(!IS_SCALAR(index->address)) {
			qb_abort("%s expects the first parameter to be a scalar", f->name);
		}
		if(!IS_SCALAR(number->address)) {
			qb_abort("%s expects the second parameter to be a scalar", f->name);
		}
		if(result->type != QB_OPERAND_NONE) {
			uint32_t result_flags = qb_get_result_flags(cxt, f->extra);
			qb_variable_dimensions *result_dim = qb_get_result_dimensions(cxt, f->extra, arguments, argument_count);
			qb_address *target_address;
			result->type = QB_OPERAND_ADDRESS;
			result->address = qb_obtain_write_target_address(cxt, expr_type, result_dim, result_prototype, result_flags);
			if(!IS_EXPANDABLE_ARRAY(result->address)) {
				// expandable arrays are set to zero when an unset occur and we'd only get an expandable array here
				// if that has occurred
				if(!(index->address->flags & QB_ADDRESS_CONSTANT) || VALUE(U32, index->address) != 0) {
					// the start index is or might not be zero
					// zero out the elements before it
					qb_address *zero_address = qb_obtain_constant_U32(cxt, 0);
					qb_address *padding_address = qb_get_array_slice(cxt, result->address, zero_address, index->address);
					qb_create_unary_op(cxt, f->extra, zero_address, padding_address);
				}
			}
			target_address = qb_get_array_slice(cxt, result->address, index->address, number->address);
			qb_create_unary_op(cxt, f->extra, value->address, target_address);
		}
	}
}

static void ZEND_FASTCALL qb_translate_intrinsic_array_column(qb_compiler_context *cxt, qb_intrinsic_function *f, qb_operand *arguments, uint32_t argument_count, qb_operand *result, qb_result_prototype *result_prototype) {
	qb_operand *container = &arguments[0], *column_index = &arguments[1];
	uint32_t operand_flags = qb_get_coercion_flags(cxt, f->extra);
	qb_primitive_type expr_type = qb_get_operand_type(cxt, container, operand_flags);
	qb_do_type_coercion(cxt, container, expr_type);
	qb_do_type_coercion(cxt, column_index, QB_TYPE_U32);

	if(cxt->stage == QB_STAGE_RESULT_TYPE_RESOLUTION) {
		result_prototype->preliminary_type = expr_type;
		result_prototype->address_flags = QB_ADDRESS_TEMPORARY;
		result->type = QB_OPERAND_RESULT_PROTOTYPE;
		result->result_prototype = result_prototype;
	} else if(cxt->stage == QB_STAGE_OPCODE_TRANSLATION) {
		if(container->address->dimension_count < 2) {
			qb_abort("%s expects a multidimensional array as the first parameter", f->name);
		}
		if(!IS_SCALAR(column_index->address)) {
			qb_abort("%s expects a scalar as the second parameter", f->name);
		}
		if(result->type != QB_OPERAND_NONE) {
			uint32_t result_flags = qb_get_result_flags(cxt, f->extra);
			qb_variable_dimensions *result_dim = qb_get_result_dimensions(cxt, f->extra, arguments, argument_count);
			result->type = QB_OPERAND_ADDRESS;
			result->address = qb_obtain_write_target_address(cxt, expr_type, result_dim, result_prototype, result_flags);
			qb_create_op(cxt, f->extra, arguments, argument_count, result);
		}
	}
}

static void ZEND_FASTCALL qb_translate_intrinsic_array_diff(qb_compiler_context *cxt, qb_intrinsic_function *f, qb_operand *arguments, uint32_t argument_count, qb_operand *result, qb_result_prototype *result_prototype) {
	qb_primitive_type expr_type = qb_do_type_coercion_for_op(cxt, f->extra, arguments, argument_count, result_prototype);

	if(cxt->stage == QB_STAGE_RESULT_TYPE_RESOLUTION) {
		result_prototype->preliminary_type = expr_type;
		result_prototype->address_flags = QB_ADDRESS_TEMPORARY;
		result->type = QB_OPERAND_RESULT_PROTOTYPE;
		result->result_prototype = result_prototype;
	} else if(cxt->stage == QB_STAGE_OPCODE_TRANSLATION) {
		uint32_t i, first_element_size;
		qb_address *first_address = arguments[0].address;
		// make sure the element size match up
		if(first_address->dimension_count < 1) {
			qb_abort("%s expects an array as the first parameter", f->name);
		}
		if(first_address->dimension_count > 1) {
			first_element_size = VALUE(U32, first_address->array_size_addresses[1]);
		} else {
			first_element_size = 1;
		}
		for(i = 1; i < argument_count; i++) {
			uint32_t element_size;
			qb_address *address = arguments[i].address;
			if(address->dimension_count > 1) {
				element_size = VALUE(U32, address->array_size_addresses[1]);
			} else {
				element_size = 1;
			}
			if(element_size != first_element_size) {
				qb_abort("elements in parameter %d are of different size", i + 1);
			}
		}
		if(result->type != QB_OPERAND_NONE) {
			qb_variable_dimensions *result_dim = qb_get_result_dimensions(cxt, f->extra, arguments, argument_count);
			qb_address *result_address;
			uint32_t i;
			for(i = 1; i < argument_count; i++) {
				qb_address *address = arguments[i].address;
				result_address = qb_obtain_temporary_variable(cxt, expr_type, result_dim);
				qb_create_binary_op(cxt, f->extra, first_address, address, result_address);
				first_address = result_address;
			}
			result->type = QB_OPERAND_ADDRESS;
			result->address = result_address;
		}
	}
}

static void ZEND_FASTCALL qb_translate_intrinsic_array_pop(qb_compiler_context *cxt, qb_intrinsic_function *f, qb_operand *arguments, uint32_t argument_count, qb_operand *result, qb_result_prototype *result_prototype) {
	qb_operand *container = &arguments[0];

	qb_do_type_coercion(cxt, container, QB_TYPE_ANY);
	if(cxt->stage == QB_STAGE_RESULT_TYPE_RESOLUTION) {
		if(container->type == QB_OPERAND_ADDRESS) {
			result_prototype->preliminary_type = result_prototype->final_type = container->address->type;
		} else if(container->type == QB_OPERAND_RESULT_PROTOTYPE) {
			result_prototype->preliminary_type = container->result_prototype->preliminary_type;
			result_prototype->final_type = container->result_prototype->final_type;
		}
		result_prototype->address_flags = QB_ADDRESS_TEMPORARY;
		result->type = QB_OPERAND_RESULT_PROTOTYPE;
		result->result_prototype = result_prototype;
	} else if(cxt->stage == QB_STAGE_OPCODE_TRANSLATION) {
		qb_address *size_address, *index_address, *one_address, *variable_address;

		if(container->type != QB_OPERAND_ADDRESS || IS_SCALAR(container->address)) {
			qb_abort("%s expects an array as parameter", f->name);
		}
		if(!IS_EXPANDABLE_ARRAY(container->address)) {
			qb_abort("Removing element to from an array that cannot shrink");
		}
		size_address = container->address->dimension_addresses[0];
		index_address = qb_obtain_temporary_variable(cxt, QB_TYPE_U32, NULL);
		one_address = qb_obtain_constant_U32(cxt, 1);
		qb_create_binary_op(cxt, &factory_subtract, size_address, one_address, index_address);
		variable_address = qb_get_array_element(cxt, container->address, index_address);
		if(result->type != QB_OPERAND_NONE) {
			qb_variable_dimensions *result_dim = qb_get_address_dimensions(cxt, variable_address);
			result->type = QB_OPERAND_ADDRESS;
			result->address = qb_obtain_write_target_address(cxt, variable_address->type, result_dim, result_prototype, 0);
			qb_create_unary_op(cxt, &factory_copy, variable_address, result->address);
		}
		qb_create_nullary_op(cxt, &factory_unset, variable_address);
	}
}

static void ZEND_FASTCALL qb_translate_intrinsic_array_shift(qb_compiler_context *cxt, qb_intrinsic_function *f, qb_operand *arguments, uint32_t argument_count, qb_operand *result, qb_result_prototype *result_prototype) {
	qb_operand *container = &arguments[0];

	qb_do_type_coercion(cxt, container, QB_TYPE_ANY);
	if(cxt->stage == QB_STAGE_RESULT_TYPE_RESOLUTION) {
		if(container->type == QB_OPERAND_ADDRESS) {
			result_prototype->preliminary_type = result_prototype->final_type = container->address->type;
		} else if(container->type == QB_OPERAND_RESULT_PROTOTYPE) {
			result_prototype->preliminary_type = container->result_prototype->preliminary_type;
			result_prototype->final_type = container->result_prototype->final_type;
		}
		result_prototype->address_flags = QB_ADDRESS_TEMPORARY;
		result->type = QB_OPERAND_RESULT_PROTOTYPE;
		result->result_prototype = result_prototype;
	} else if(cxt->stage == QB_STAGE_OPCODE_TRANSLATION) {
		qb_address *zero_address, *variable_address;

		if(container->type != QB_OPERAND_ADDRESS || IS_SCALAR(container->address)) {
			qb_abort("%s expects an array as parameter", f->name);
		}
		if(!IS_EXPANDABLE_ARRAY(container->address)) {
			qb_abort("Removing element from an array that cannot shrink");
		}
		zero_address = qb_obtain_constant_U32(cxt, 0);
		variable_address = qb_get_array_element(cxt, container->address, zero_address);
		if(result->type != QB_OPERAND_NONE) {
			qb_variable_dimensions *result_dim = qb_get_address_dimensions(cxt, variable_address);
			result->type = QB_OPERAND_ADDRESS;
			result->address = qb_obtain_write_target_address(cxt, variable_address->type, result_dim, result_prototype, 0);
			qb_do_assignment(cxt, variable_address, result->address);
		}
		qb_create_nullary_op(cxt, &factory_unset, variable_address);
	}
}

static void ZEND_FASTCALL qb_translate_intrinsic_array_slice(qb_compiler_context *cxt, qb_intrinsic_function *f, qb_operand *arguments, uint32_t argument_count, qb_operand *result, qb_result_prototype *result_prototype) {
	qb_operand *container = &arguments[0], *offset = &arguments[1], *length = &arguments[2];

	qb_do_type_coercion(cxt, container, QB_TYPE_ANY);
	if(cxt->stage == QB_STAGE_RESULT_TYPE_RESOLUTION) {
		if(container->type == QB_OPERAND_ADDRESS) {
			result_prototype->preliminary_type = result_prototype->final_type = container->address->type;
		} else if(container->type == QB_OPERAND_RESULT_PROTOTYPE) {
			result_prototype->preliminary_type = container->result_prototype->preliminary_type;
			result_prototype->final_type = container->result_prototype->final_type;
		}
		result_prototype->address_flags = QB_ADDRESS_TEMPORARY;
		result->type = QB_OPERAND_RESULT_PROTOTYPE;
		result->result_prototype = result_prototype;
	} else if(cxt->stage == QB_STAGE_OPCODE_TRANSLATION) {
		qb_do_type_coercion(cxt, container, QB_TYPE_ANY);
		if(IS_SCALAR(container->address)) {
			qb_abort("%s expects an array as parameter", f->name);
		}
		qb_do_type_coercion(cxt, offset, QB_TYPE_U32);
		if(argument_count >= 3) {
			qb_do_type_coercion(cxt, length, QB_TYPE_U32);
		}
		if(result->type != QB_OPERAND_NONE) {
			result->type = QB_OPERAND_ADDRESS;
			result->address = qb_get_array_slice(cxt, container->address, offset->address, (argument_count >= 3) ? length->address : NULL);
		}
	}
}

static void ZEND_FASTCALL qb_translate_intrinsic_array_splice(qb_compiler_context *cxt, qb_intrinsic_function *f, qb_operand *arguments, uint32_t argument_count, qb_operand *result, qb_result_prototype *result_prototype) {
	qb_operand *container = &arguments[0], *offset = &arguments[1], *length = &arguments[2], *replacement = &arguments[3];

	qb_do_type_coercion(cxt, container, QB_TYPE_ANY);
	if(cxt->stage == QB_STAGE_RESULT_TYPE_RESOLUTION) {
		if(container->type == QB_OPERAND_ADDRESS) {
			result_prototype->preliminary_type = result_prototype->final_type = container->address->type;
		} else if(container->type == QB_OPERAND_RESULT_PROTOTYPE) {
			result_prototype->preliminary_type = container->result_prototype->preliminary_type;
			result_prototype->final_type = container->result_prototype->final_type;
		}
		result_prototype->address_flags = QB_ADDRESS_TEMPORARY;
		result->type = QB_OPERAND_RESULT_PROTOTYPE;
		result->result_prototype = result_prototype;
	} else if(cxt->stage == QB_STAGE_OPCODE_TRANSLATION) {
		qb_address *slice_address;
		if(IS_SCALAR(container->address)) {
			qb_abort("%s expects an array as parameter", f->name);
		}
		qb_do_type_coercion(cxt, offset, QB_TYPE_U32);
		if(argument_count >= 3) {
			qb_do_type_coercion(cxt, length, QB_TYPE_U32);
		}
		if(argument_count >= 4) {
			qb_do_type_coercion(cxt, replacement, container->address->type);
		}
		slice_address = qb_get_array_slice(cxt, container->address, offset->address, (argument_count >= 3) ? length->address : NULL);
		if(result->type != QB_OPERAND_NONE) {
			uint32_t result_flags = qb_get_result_flags(cxt, f->extra);
			qb_variable_dimensions *result_dim = qb_get_address_dimensions(cxt, slice_address);
			result->type = QB_OPERAND_ADDRESS;
			result->address = qb_obtain_write_target_address(cxt, slice_address->type, result_dim, result_prototype, result_flags);
			qb_create_unary_op(cxt, &factory_copy, slice_address, result->address);
		}
		if(argument_count >= 4) {
			if(IS_FIXED_LENGTH_ARRAY(replacement->address) && IS_FIXED_LENGTH_ARRAY(slice_address) && ARRAY_SIZE(replacement->address) == ARRAY_SIZE(slice_address)) {
				qb_create_unary_op(cxt, &factory_copy, replacement->address, slice_address);
			} else {
				qb_address *insert_offset_address;
				// clear the span (unless its length is zero)
				if(!(slice_address->array_size_address->flags & QB_ADDRESS_CONSTANT && ARRAY_SIZE(slice_address) == 0)) {
					qb_create_nullary_op(cxt, &factory_unset, slice_address);
				}

				// insert the replacement 
				if(slice_address->array_index_address) {
					insert_offset_address = slice_address->array_index_address;
				} else {
					uint32_t insert_offset = ELEMENT_COUNT(slice_address->segment_offset, slice_address->type);
					insert_offset_address = qb_obtain_constant_U32(cxt, insert_offset);
				}
				qb_create_binary_op(cxt, &factory_array_insert, replacement->address, insert_offset_address, container->address);
			}
		} else {
			qb_create_nullary_op(cxt, &factory_unset, slice_address);
		}
	}
}

static void ZEND_FASTCALL qb_translate_intrinsic_array_aggregate(qb_compiler_context *cxt, qb_intrinsic_function *f, qb_operand *arguments, uint32_t argument_count, qb_operand *result, qb_result_prototype *result_prototype) {
	qb_operand *container = &arguments[0];
	qb_primitive_type expr_type;

	qb_do_type_coercion(cxt, container, QB_TYPE_ANY);
	if(cxt->stage == QB_STAGE_RESULT_TYPE_RESOLUTION) {
		if(result->type != QB_OPERAND_NONE) {
			if(container->type == QB_OPERAND_ADDRESS) {
				result_prototype->preliminary_type = result_prototype->final_type = container->address->type;
			} else if(container->type == QB_OPERAND_RESULT_PROTOTYPE) {
				result_prototype->preliminary_type = container->result_prototype->preliminary_type;
				result_prototype->final_type = container->result_prototype->final_type;
			}
			result->type = QB_OPERAND_RESULT_PROTOTYPE;
			result->result_prototype = result_prototype;
		}
	} else if(cxt->stage == QB_STAGE_OPCODE_TRANSLATION) {
		expr_type = container->address->type;
		if(IS_SCALAR(container->address)) {
			qb_abort("%s expects an array as parameter", f->name);
		}
		if(result->type != QB_OPERAND_NONE) {
			uint32_t result_flags = qb_get_result_flags(cxt, f->extra);
			result->type = QB_OPERAND_ADDRESS;
			result->address = qb_obtain_write_target_address(cxt, expr_type, NULL, result_prototype, result_flags);
			qb_create_op(cxt, f->extra, arguments, 1, result);
		}
	}
}

static void ZEND_FASTCALL qb_translate_intrinsic_array_search(qb_compiler_context *cxt, qb_intrinsic_function *f, qb_operand *arguments, uint32_t argument_count, qb_operand *result, qb_result_prototype *result_prototype) {
	qb_operand *container = &arguments[0];
	qb_operand *needle = &arguments[1];
	qb_primitive_type expr_type;

	qb_do_type_coercion(cxt, container, QB_TYPE_ANY);
	expr_type = container->address->type;
	qb_do_type_coercion(cxt, needle, expr_type);

	if(cxt->stage == QB_STAGE_RESULT_TYPE_RESOLUTION) {
		if(result->type != QB_OPERAND_NONE) {
			result_prototype->preliminary_type = result_prototype->final_type = QB_TYPE_S32;
			result_prototype->operand_flags = QB_ADDRESS_TEMPORARY;
			result->type = QB_OPERAND_RESULT_PROTOTYPE;
			result->result_prototype = result_prototype;
		}
	} else if(cxt->stage == QB_STAGE_OPCODE_TRANSLATION) {
		if(IS_SCALAR(container->address)) {
			qb_abort("%s expects an array as the first parameter", f->name);
		}
		if(container->address->dimension_count == 1) {
			if(needle->address->dimension_count != 0) {
				qb_abort("%s expects a scalar as the second parameter", f->name);
			}
		} else {
			if(IS_SCALAR(container->address)) {
				qb_abort("%s expects an array as the second parameter when given a multidimensional array", f->name);
			} else {
				if(IS_VARIABLE_LENGTH_ARRAY(needle->address)) {
					qb_abort("%s expects a fixed-length array as the second parameter", f->name);
				} else {
					uint32_t needle_length = ARRAY_SIZE(needle->address);
					uint32_t subarray_length = VALUE(U32, container->address->array_size_addresses[1]);
					if(needle_length != subarray_length) {
						qb_abort("%s expects an array with %d elements as the second parameter", f->name, subarray_length);
					}
				}
			}
		}
		if(result->type != QB_OPERAND_NONE) {
			uint32_t result_flags = qb_get_result_flags(cxt, f->extra);
			result->type = QB_OPERAND_ADDRESS;
			result->address = qb_obtain_write_target_address(cxt, QB_TYPE_S32, NULL, result_prototype, result_flags);
			qb_create_op(cxt, f->extra, arguments, 2, result);
		}
	}
}

static void ZEND_FASTCALL qb_translate_intrinsic_subarray_search(qb_compiler_context *cxt, qb_intrinsic_function *f, qb_operand *arguments, uint32_t argument_count, qb_operand *result, qb_result_prototype *result_prototype) {
	qb_operand *container = &arguments[0], *needle = &arguments[1], *offset = &arguments[2];

	qb_do_type_coercion(cxt, container, QB_TYPE_ANY);
	if(argument_count >= 3) {
		qb_do_type_coercion(cxt, offset, QB_TYPE_S32);
	}
	if(cxt->stage == QB_STAGE_RESULT_TYPE_RESOLUTION) {
		if(result->type != QB_OPERAND_NONE) {
			result_prototype->preliminary_type = result_prototype->final_type = QB_TYPE_I32;
			result_prototype->address_flags = QB_ADDRESS_TEMPORARY;
			result->type = QB_OPERAND_RESULT_PROTOTYPE;
			result->result_prototype = result_prototype;
		}
	} else if(cxt->stage == QB_STAGE_OPCODE_TRANSLATION) {
		qb_primitive_type expr_type;
		qb_address *offset_address;

		expr_type = container->address->type;
		qb_do_type_coercion(cxt, needle, expr_type);
		if(container->address->dimension_count != 1) {
			qb_abort("%s expects a one-dimensional array as the first parameter", f->name);
		}
		if(needle->address->dimension_count > 1) {
			qb_abort("%s expects a one-dimensional array as the second parameter", f->name);
		}
		if(argument_count >= 3) {
			if(!IS_SCALAR(offset->address)) {
				qb_abort("%s expects a scalar as the third parameter", f->name);
			}
			if(offset->address->segment_offset == QB_OFFSET_INVALID) {
				// need to copy the index value to a temporary variable first
				offset_address = qb_obtain_temporary_scalar(cxt, QB_TYPE_S32);
				qb_do_assignment(cxt, offset_address, offset->address);
			} else {
				offset_address = offset->address;
			}
		} else {
			offset_address = qb_obtain_constant_S32(cxt, 0);
		}
		if(result->type != QB_OPERAND_NONE) {
			uint32_t result_flags = qb_get_result_flags(cxt, f->extra);
			result->type = QB_OPERAND_ADDRESS;
			result->address = qb_obtain_write_target_address(cxt, QB_TYPE_S32, NULL, result_prototype, result_flags);
			qb_create_ternary_op(cxt, f->extra, container->address, needle->address, offset_address, result->address);
		}
	}
}

static void ZEND_FASTCALL qb_translate_intrinsic_shuffle(qb_compiler_context *cxt, qb_intrinsic_function *f, qb_operand *arguments, uint32_t argument_count, qb_operand *result, qb_result_prototype *result_prototype) {
	qb_operand *container = &arguments[0];

	if(cxt->stage == QB_STAGE_RESULT_TYPE_RESOLUTION) {
		if(result->type != QB_OPERAND_NONE) {
			result_prototype->preliminary_type = result_prototype->final_type = QB_TYPE_S32;
			result_prototype->address_flags = QB_ADDRESS_TEMPORARY;
			result->type = QB_OPERAND_RESULT_PROTOTYPE;
			result->result_prototype = result_prototype;
		}
	} else if(cxt->stage == QB_STAGE_OPCODE_TRANSLATION) {
		qb_address *width_address;
		if(container->type != QB_OPERAND_ADDRESS || IS_SCALAR(container->address)) {
			qb_abort("%s expects an array as parameter", f->name);
		}
		if(container->address->dimension_count > 1) {
			width_address = container->address->array_size_addresses[1];
		} else {
			width_address = qb_obtain_constant_U32(cxt, 1);
		}
		qb_create_unary_op(cxt, f->extra, width_address, container->address);
		if(result->type != QB_OPERAND_NONE) {
			result->type = QB_OPERAND_ADDRESS;
			result->address = qb_obtain_constant_S32(cxt, 1);
		}
	}
}

static void ZEND_FASTCALL qb_translate_intrinsic_array_reverse(qb_compiler_context *cxt, qb_intrinsic_function *f, qb_operand *arguments, uint32_t argument_count, qb_operand *result, qb_result_prototype *result_prototype) {
	qb_operand *container = &arguments[0];

	qb_do_type_coercion(cxt, container, QB_TYPE_ANY);
	if(cxt->stage == QB_STAGE_RESULT_TYPE_RESOLUTION) {
		if(result->type != QB_OPERAND_NONE) {
			if(container->type == QB_OPERAND_ADDRESS) {
				result_prototype->preliminary_type = result_prototype->final_type = container->address->type;
			} else if(container->type == QB_OPERAND_RESULT_PROTOTYPE) {
				result_prototype->preliminary_type = container->result_prototype->preliminary_type;
				result_prototype->final_type = container->result_prototype->final_type;
			}
			result_prototype->address_flags = QB_ADDRESS_TEMPORARY;
			result->type = QB_OPERAND_RESULT_PROTOTYPE;
			result->result_prototype = result_prototype;
		}
	} else if(cxt->stage == QB_STAGE_OPCODE_TRANSLATION) {
		if(IS_SCALAR(container->address)) {
			qb_abort("%s expects an array as the first parameter", f->name);
		}
		if(result->type != QB_OPERAND_NONE) {
			qb_address *width_address;
			uint32_t result_flags = qb_get_result_flags(cxt, f->extra);
			qb_variable_dimensions *result_dim = qb_get_result_dimensions(cxt, f->extra, arguments, argument_count);
			if(container->address->dimension_count > 1) {
				width_address = container->address->array_size_addresses[1];
			} else {
				width_address = qb_obtain_constant_U32(cxt, 1);
			}
			result->type = QB_OPERAND_ADDRESS;
			result->address = qb_obtain_write_target_address(cxt, container->address->type, result_dim, result_prototype, result_flags);
			qb_create_binary_op(cxt, f->extra, container->address, width_address, result->address);
		}
	}
}

static void ZEND_FASTCALL qb_translate_intrinsic_range(qb_compiler_context *cxt, qb_intrinsic_function *f, qb_operand *arguments, uint32_t argument_count, qb_operand *result, qb_result_prototype *result_prototype) {
	qb_primitive_type expr_type = qb_do_type_coercion_for_op(cxt, f->extra, arguments, argument_count, result_prototype);
	qb_operand *start = &arguments[0];
	qb_operand *end = &arguments[1];
	qb_operand *interval = (argument_count >= 3) ? &arguments[2] : NULL;
	if(cxt->stage == QB_STAGE_RESULT_TYPE_RESOLUTION) {
		if(result->type != QB_OPERAND_NONE) {
			result_prototype->preliminary_type = result_prototype->final_type = expr_type;
			result->type = QB_OPERAND_RESULT_PROTOTYPE;
			result->result_prototype = result_prototype;
		}
	} else if(cxt->stage == QB_STAGE_OPCODE_TRANSLATION) {
		if(!IS_SCALAR(start->address)) {
			qb_abort("%s expects a scalar as the first parameter", f->name);
		}
		if(!IS_SCALAR(end->address)) {
			qb_abort("%s expects a scalar as the second parameter", f->name);
		}
		if(interval && !IS_SCALAR(interval->address)) {
			qb_abort("%s expects a scalar as the third parameter", f->name);
		}
		if(result->type != QB_OPERAND_NONE) {
			qb_address *interval_address;
			uint32_t result_flags = qb_get_result_flags(cxt, f->extra);
			qb_variable_dimensions *result_dim = qb_get_result_dimensions(cxt, f->extra, arguments, argument_count);
			if(interval) {
				interval_address = interval->address;
			} else {
				interval_address = qb_obtain_constant(cxt, 1, expr_type);
			}
			result->type = QB_OPERAND_ADDRESS;
			result->address = qb_obtain_write_target_address(cxt, expr_type, result_dim, result_prototype, result_flags);
			qb_create_ternary_op(cxt, f->extra, start->address, end->address, interval_address, result->address);
		}
	}
}

static void ZEND_FASTCALL qb_translate_intrinsic_array_rand(qb_compiler_context *cxt, qb_intrinsic_function *f, qb_operand *arguments, uint32_t argument_count, qb_operand *result, qb_result_prototype *result_prototype) {
	qb_operand *container = &arguments[0];
	qb_operand *count;
	if(argument_count >= 2) {
		count = &arguments[1];
		qb_do_type_coercion(cxt, count, QB_TYPE_U32);
	} else {
		count = NULL;
	}
	if(cxt->stage == QB_STAGE_RESULT_TYPE_RESOLUTION) {
		if(result->type != QB_OPERAND_NONE) {
			if(container->type == QB_OPERAND_ADDRESS) {
				result_prototype->preliminary_type = result_prototype->final_type = container->address->type;
			} else if(container->type == QB_OPERAND_RESULT_PROTOTYPE) {
				result_prototype->preliminary_type = container->result_prototype->preliminary_type;
				result_prototype->final_type = container->result_prototype->final_type;
			}
			result->type = QB_OPERAND_RESULT_PROTOTYPE;
			result->result_prototype = result_prototype;
		}
	} else if(cxt->stage == QB_STAGE_OPCODE_TRANSLATION) {
		if(IS_SCALAR(container->address)) {
			qb_abort("%s expects an array as the first parameter", f->name);
		}
		if(count && !IS_SCALAR(count->address)) {
			qb_abort("%s expects a scalar as the second parameter", f->name);
		}
		if(result->type != QB_OPERAND_NONE) {
			qb_address *size_address;
			qb_address *count_address;
			uint32_t result_flags = qb_get_result_flags(cxt, f->extra);
			qb_variable_dimensions *result_dim = qb_get_result_dimensions(cxt, f->extra, arguments, argument_count);
			size_address = container->address->dimension_addresses[0];
			if(count) {
				count_address = count->address;
			} else {
				count_address = qb_obtain_constant_U32(cxt, 1);
			}
			result->type = QB_OPERAND_ADDRESS;
			result->address = qb_obtain_write_target_address(cxt, QB_TYPE_U32, result_dim, result_prototype, result_flags);
			qb_create_binary_op(cxt, f->extra, size_address, count_address, result->address);
		}
	}
}

static void ZEND_FASTCALL qb_translate_intrinsic_array_resize(qb_compiler_context *cxt, qb_intrinsic_function *f, qb_operand *arguments, uint32_t argument_count, qb_operand *result, qb_result_prototype *result_prototype) {
	qb_operand *container = &arguments[0];
	uint32_t i;
	for(i = 1; i < argument_count; i++) {
		qb_do_type_coercion(cxt, &arguments[i], QB_TYPE_U32);
	}

	if(cxt->stage == QB_STAGE_RESULT_TYPE_RESOLUTION) {
		result->type = QB_OPERAND_NONE;
	} else if(cxt->stage == QB_STAGE_OPCODE_TRANSLATION) {
		if(IS_SCALAR(container->address)) {
			qb_abort("%s expects an array as the first parameter", f->name);
		}
		if((argument_count - 1) > container->address->dimension_count) {
			qb_abort("the array only has %d dimension%s", container->address->dimension_count, (container->address->dimension_count > 1) ? "s" : "");
		}
		for(i = 1; i < argument_count; i++) {
			qb_address *dimension_address = container->address->dimension_addresses[i - 1];
			qb_address *new_dimension_address = arguments[i].address;

			if(!IS_SCALAR_VARIABLE(new_dimension_address)) {
				// need to copy the dimension value to a temporary variable first
				// since the ARESIZE instruction expects VAR addresses only
				qb_address *new_address = qb_obtain_temporary_variable(cxt, QB_TYPE_U32, NULL);
				qb_create_unary_op(cxt, &factory_copy, new_dimension_address, new_dimension_address);
				new_dimension_address = arguments[i].address = new_address;
			}
			if(dimension_address->flags & QB_ADDRESS_CONSTANT) {
				if(!(new_dimension_address->flags & QB_ADDRESS_CONSTANT) || VALUE(U32, dimension_address) != VALUE(U32, new_dimension_address)) {
					qb_abort("cannot change array dimension that was declared to be fixed-size");
				}
			} else {
				dimension_address->flags &= ~QB_ADDRESS_READ_ONLY;
			}
		}
		container->address->flags |= QB_ADDRESS_INITIALIZED;
		container->address->flags &= ~QB_ADDRESS_READ_ONLY;
		qb_create_op(cxt, f->extra, arguments, argument_count, NULL);
	}
}

static void ZEND_FASTCALL qb_translate_intrinsic_utf8_decode(qb_compiler_context *cxt, qb_intrinsic_function *f, qb_operand *arguments, uint32_t argument_count, qb_operand *result, qb_result_prototype *result_prototype) {
	qb_operand *source = &arguments[0];
	qb_do_type_coercion(cxt, source, QB_TYPE_U08);

	if(result->type != QB_OPERAND_NONE) {
		if(cxt->stage == QB_STAGE_RESULT_TYPE_RESOLUTION) {
			if(result->type != QB_OPERAND_NONE) {
				result_prototype->address_flags = QB_ADDRESS_TEMPORARY;
				result->type = QB_OPERAND_RESULT_PROTOTYPE;
				result->result_prototype = result_prototype;
			}
		} else if(cxt->stage == QB_STAGE_OPCODE_TRANSLATION) {
			qb_primitive_type expr_type;

			qb_finalize_result_prototype(cxt, result_prototype);
			expr_type = result_prototype->final_type;

			if(!STORAGE_TYPE_MATCH(expr_type , QB_TYPE_U16)) {
				// not U16--use U32
				expr_type = QB_TYPE_U32;
			}
			if(result->type != QB_OPERAND_NONE) {
				uint32_t result_flags = qb_get_result_flags(cxt, f->extra);
				qb_variable_dimensions *result_dim = qb_get_variable_length_dimensions(cxt);
				result->type = QB_OPERAND_ADDRESS;
				result->address = qb_obtain_write_target_address(cxt, expr_type, result_dim, result_prototype, result_flags);
				qb_create_op(cxt, f->extra, arguments, argument_count, result);
			}
		}
	}
}

static void ZEND_FASTCALL qb_translate_intrinsic_utf8_encode(qb_compiler_context *cxt, qb_intrinsic_function *f, qb_operand *arguments, uint32_t argument_count, qb_operand *result, qb_result_prototype *result_prototype) {
	qb_operand *source = &arguments[0];
	qb_primitive_type source_type = QB_TYPE_ANY;

	if(source->type == QB_OPERAND_ADDRESS) {
		source_type = source->address->type;
	} else if(source->type == QB_OPERAND_RESULT_PROTOTYPE) {
		source_type = source->result_prototype->final_type;
	}
	if(!STORAGE_TYPE_MATCH(source->address->type, QB_TYPE_U16)) {
		// use U32 unless the input is U16
		source_type = QB_TYPE_U32;
	}
	qb_do_type_coercion(cxt, source, source_type);
	if(cxt->stage == QB_STAGE_RESULT_TYPE_RESOLUTION) {
		if(result->type != QB_OPERAND_NONE) {
			result_prototype->preliminary_type = result_prototype->final_type = QB_TYPE_U08;
			result_prototype->address_flags = QB_ADDRESS_STRING;
			result->type = QB_OPERAND_RESULT_PROTOTYPE;
			result->result_prototype = result_prototype;
		}
	} else {
		if(result->type != QB_OPERAND_NONE) {
			uint32_t result_flags = qb_get_result_flags(cxt, f->extra);
			qb_variable_dimensions *result_dim = qb_get_variable_length_dimensions(cxt);
			result->type = QB_OPERAND_ADDRESS;
			result->address = qb_obtain_write_target_address(cxt, QB_TYPE_I08, result_dim, result_prototype, result_flags);
			if(result->address->flags & QB_ADDRESS_TEMPORARY) {
				// it's a temporary address--make it as a string
				result->address->flags |= QB_ADDRESS_STRING;
			}
			qb_create_op(cxt, f->extra, arguments, argument_count, result);
		}
	}
}

static void ZEND_FASTCALL qb_translate_intrinsic_pack(qb_compiler_context *cxt, qb_intrinsic_function *f, qb_operand *arguments, uint32_t argument_count, qb_operand *result, qb_result_prototype *result_prototype) {
	qb_operand *value = &arguments[0], *type = &arguments[1];
	qb_primitive_type arg_type = QB_TYPE_ANY;
	if(argument_count >= 2) {
		if(type->type == QB_OPERAND_ZVAL && type->constant->type == IS_LONG) {
			arg_type = type->constant->value.lval;
		}
	} else {
		if(value->type == QB_OPERAND_ADDRESS) {
			arg_type = value->address->type;
		} else if(value->type == QB_OPERAND_RESULT_PROTOTYPE) {
			arg_type = value->result_prototype->final_type;
		}
	}
	qb_do_type_coercion(cxt, value, arg_type);
	if(cxt->stage == QB_STAGE_RESULT_TYPE_RESOLUTION) {
		if(result->type != QB_OPERAND_NONE) {
			result_prototype->preliminary_type = result_prototype->final_type = QB_TYPE_U08;
			result_prototype->address_flags = QB_ADDRESS_STRING;
			result->type = QB_OPERAND_RESULT_PROTOTYPE;
			result->result_prototype = result_prototype;
		}
	} else if(cxt->stage == QB_STAGE_OPCODE_TRANSLATION) {
		if(!(arg_type >= QB_TYPE_S08 && arg_type <= QB_TYPE_F64)) {
			qb_abort((argument_count >= 2) ? "%s expects the second parameter to be a constant indicating the type" : "%s() requires the second parameter when the input type cannot be determined", f->name);
		}
		if(!IS_SCALAR(value->address)) {
			qb_abort("%s expects a scalar as the first parameter", f->name);
		}

		if(result->type != QB_OPERAND_NONE) {
			uint32_t result_flags = qb_get_result_flags(cxt, f->extra);
			qb_variable_dimensions *result_dim = qb_get_variable_length_dimensions(cxt);
			result->type = QB_OPERAND_ADDRESS;
			result->address = qb_obtain_write_target_address(cxt, QB_TYPE_U08, result_dim, result_prototype, result_flags);
			if(result->address->flags & QB_ADDRESS_TEMPORARY) {
				// it's a temporary address--mark it as a string
				result->address->flags |= QB_ADDRESS_STRING;
			}
			qb_create_op(cxt, f->extra, arguments, 1, result);
		}
	}
}

static void ZEND_FASTCALL qb_translate_intrinsic_unpack(qb_compiler_context *cxt, qb_intrinsic_function *f, qb_operand *arguments, uint32_t argument_count, qb_operand *result, qb_result_prototype *result_prototype) {
	qb_operand *array = &arguments[0], *offset = &arguments[1], *type = &arguments[2];
	qb_primitive_type result_type = QB_TYPE_ANY;

	qb_do_type_coercion(cxt, array, QB_TYPE_U08);
	if(argument_count >= 2) {
		qb_do_type_coercion(cxt, offset, QB_TYPE_U32);
	}
	if(argument_count >= 3) {
		if(type->type == QB_OPERAND_ZVAL && type->constant->type == IS_LONG) {
			result_type = type->constant->value.lval;
		}
	}

	if(cxt->stage == QB_STAGE_RESULT_TYPE_RESOLUTION) {
		if(result->type != QB_OPERAND_NONE) {
			result_prototype->preliminary_type = result_type;
			if(result_type != QB_TYPE_ANY) {
				result_prototype->final_type = result_type;
			} else {
				// we need the lvalue type if it isn't specified
				result_prototype->operand_flags = QB_COERCE_TO_LVALUE_TYPE;
			}
			result_prototype->address_flags = QB_ADDRESS_TEMPORARY;
			result->type = QB_OPERAND_RESULT_PROTOTYPE;
			result->result_prototype = result_prototype;
		}
	} else if(cxt->stage == QB_STAGE_OPCODE_TRANSLATION) {
		qb_address *slice_address, *offset_address, *length_address;

		if(IS_SCALAR(array->address) || array->address->dimension_count > 1) {
			qb_abort("%s expects a one-dimenstional array as the first parameter", f->name);
		}
		if(argument_count >= 2) {
			if(!IS_SCALAR(offset->address)) {
				qb_abort("%s expects a scalar as the second parameter", f->name);
			}
			offset_address = offset->address;
		} else {
			offset_address = qb_obtain_constant_U32(cxt, 0);
		}
		qb_finalize_result_prototype(cxt, result_prototype);
		if(argument_count >= 3) {
			if(!(result_type >= QB_TYPE_S08 && result_type <= QB_TYPE_F64)) {
				qb_abort("%s expects the third parameter to be a constant indicating the type", f->name);
			}
		} else {
			result_type = result_prototype->final_type;
			if(result_type == QB_TYPE_ANY) {
				qb_abort("%s() requires the third parameter when the desired type cannot be determined from context", f->name);
			}
		}
		if(result->type != QB_OPERAND_NONE) {
			uint32_t result_flags = qb_get_result_flags(cxt, f->extra);
			length_address = qb_obtain_constant_U32(cxt, type_sizes[result_type]);
			slice_address = qb_get_array_slice(cxt, array->address, offset_address, length_address);
			result->type = QB_OPERAND_ADDRESS;
			result->address = qb_obtain_write_target_address(cxt, result_type, NULL, result_prototype, result_flags);
			qb_create_unary_op(cxt, f->extra, slice_address, result->address);
		}
	}
}

static void ZEND_FASTCALL qb_translate_intrinsic_minmax(qb_compiler_context *cxt, qb_intrinsic_function *f, qb_operand *arguments, uint32_t argument_count, qb_operand *result, qb_result_prototype *result_prototype) {
	if(argument_count == 1) {
		qb_operand *container = &arguments[0];
		qb_do_type_coercion(cxt, container, QB_TYPE_ANY);

		if(cxt->stage == QB_STAGE_RESULT_TYPE_RESOLUTION) {
			result->type = QB_OPERAND_RESULT_PROTOTYPE;
			result->result_prototype = result_prototype;
		} else if(cxt->stage == QB_STAGE_OPCODE_TRANSLATION) {
			qb_primitive_type expr_type = container->address->type;
			if(IS_SCALAR(container->address)) {
				qb_abort("%s expects an array as parameter when only one parameter is given", f->name);
			}
			if(result->type != QB_OPERAND_NONE) {
				uint32_t result_flags = qb_get_result_flags(cxt, f->extra);
				result->type = QB_OPERAND_ADDRESS;
				result->address = qb_obtain_write_target_address(cxt, expr_type, NULL, result_prototype, result_flags);
				qb_create_op(cxt, f->extra, arguments, 1, result);
			}
		}
	} else {
		qb_primitive_type expr_type = qb_get_highest_rank_type(cxt, arguments, argument_count, 0);
		uint32_t i = 0; 

		for(i = 0; i < argument_count; i++) {
			qb_do_type_coercion(cxt, &arguments[i], expr_type);
		}
		if(cxt->stage == QB_STAGE_RESULT_TYPE_RESOLUTION) {
			result_prototype->preliminary_type = expr_type;
			result->type = QB_OPERAND_RESULT_PROTOTYPE;
			result->result_prototype = result_prototype;
		} else if(cxt->stage == QB_STAGE_OPCODE_TRANSLATION) {
			if(result->type != QB_OPERAND_NONE) {
				uint32_t result_flags = qb_get_result_flags(cxt, f->extra);
				result->type = QB_OPERAND_ADDRESS;
				result->address = qb_obtain_write_target_address(cxt, expr_type, NULL, result_prototype, result_flags);
				qb_create_binary_op(cxt, f->extra, arguments[0].address, arguments[1].address, result->address);
				for(i = 2; i < argument_count; i++) {
					qb_create_binary_op(cxt, f->extra, result->address, arguments[i].address, result->address);
				}
			}
		}
	}
}

static void ZEND_FASTCALL qb_translate_intrinsic_cast(qb_compiler_context *cxt, qb_intrinsic_function *f, qb_operand *arguments, uint32_t argument_count, qb_operand *result, qb_result_prototype *result_prototype) {
	qb_operand *expr = &arguments[0];
	uint32_t type = (uintptr_t) f->extra;

	qb_do_type_coercion(cxt, expr, type);
	if(result->type != QB_OPERAND_NONE) {
		*result = *expr;
	}
}

static void ZEND_FASTCALL qb_translate_intrinsic_defined(qb_compiler_context *cxt, qb_intrinsic_function *f, qb_operand *arguments, uint32_t argument_count, qb_operand *result, qb_result_prototype *result_prototype) {
	if(cxt->stage == QB_STAGE_RESULT_TYPE_RESOLUTION) {
		result_prototype->preliminary_type = result_prototype->final_type = QB_TYPE_I32;
		result_prototype->address_flags = QB_ADDRESS_BOOLEAN;
		result->type = QB_OPERAND_RESULT_PROTOTYPE;
		result->result_prototype = result_prototype;
	} else if(cxt->stage == QB_STAGE_OPCODE_TRANSLATION) {
		USE_TSRM
		qb_operand *name = &arguments[0];
		if(name->type == QB_OPERAND_ZVAL && Z_TYPE_P(name->constant) == IS_STRING) {
			zval c;
			char *name_str = Z_STRVAL_P(name->constant);
			uint32_t name_len = Z_STRLEN_P(name->constant);
			int32_t constant_exists = FALSE;
			if(result->type != QB_OPERAND_NONE) {
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
				result->address = qb_obtain_constant_S32(cxt, constant_exists);
			}
		} else {
			qb_abort("%s() expects a constant string", f->name);
		}
	}
}

static void ZEND_FASTCALL qb_translate_intrinsic_define(qb_compiler_context *cxt, qb_intrinsic_function *f, qb_operand *arguments, uint32_t argument_count, qb_operand *result, qb_result_prototype *result_prototype) {
	if(cxt->stage == QB_STAGE_RESULT_TYPE_RESOLUTION) {
		result_prototype->preliminary_type = result_prototype->final_type = QB_TYPE_I32;
		result_prototype->address_flags = QB_ADDRESS_BOOLEAN;
		result->type = QB_OPERAND_RESULT_PROTOTYPE;
		result->result_prototype = result_prototype;
	} else if(cxt->stage == QB_STAGE_OPCODE_TRANSLATION) {
		USE_TSRM
		qb_operand *name = &arguments[0], *expr = &arguments[1];
		if(name->type == QB_OPERAND_ZVAL && Z_TYPE_P(name->constant) == IS_STRING) {
			zend_constant c;
			char *name_str = Z_STRVAL_P(name->constant);
			uint32_t name_len = Z_STRLEN_P(name->constant);
			int registration_result;
			if(zend_memnstr(name_str, "::", sizeof("::") - 1, name_str + name_len)) {
				qb_abort("Class constants cannot be defined or redefined");
			}

			if(expr->type == QB_OPERAND_ZVAL) {
				c.value = *expr->constant;
				zval_copy_ctor(&c.value);
			} else if(expr->type == QB_OPERAND_ADDRESS && (expr->address->flags & QB_ADDRESS_CONSTANT) && IS_SCALAR(expr->address)) {
				if(expr->address->type >= QB_TYPE_F32) {
					double value;
					switch(expr->address->type) {
						case QB_TYPE_F32: value = VALUE(F32, expr->address); break;
						case QB_TYPE_F64: value = VALUE(F64, expr->address); break;
						default: break;
					}
					Z_TYPE(c.value) = IS_DOUBLE;
					Z_DVAL(c.value) = value;
				} else {
					long value;
					switch(expr->address->type) {
						case QB_TYPE_S08: value = VALUE(S08, expr->address); break;
						case QB_TYPE_U08: value = VALUE(U08, expr->address); break;
						case QB_TYPE_S16: value = VALUE(S16, expr->address); break;
						case QB_TYPE_U16: value = VALUE(U16, expr->address); break;
						case QB_TYPE_S32: value = VALUE(S32, expr->address); break;
						case QB_TYPE_U32: value = VALUE(U32, expr->address); break;
						case QB_TYPE_S64: value = (long) VALUE(S64, expr->address); break;
						case QB_TYPE_U64: value = (long) VALUE(U64, expr->address); break;
						default: break;
					}
					Z_TYPE(c.value) = IS_LONG;
					Z_LVAL(c.value) = value;
				}
			} else {
				qb_abort("%s() expects a constant expression as the second parameter", f->name);
			}
		
			c.flags = CONST_CS;
			c.name = zend_strndup(name_str, name_len);
			c.name_len = name_len + 1;
			c.module_number = PHP_USER_CONSTANT;
			registration_result = zend_register_constant(&c TSRMLS_CC);
			if(result->type != QB_OPERAND_NONE) {
				result->type = QB_OPERAND_ADDRESS;
				if(registration_result == SUCCESS) {
					result->address = qb_obtain_constant_S32(cxt, TRUE);
				} else {
					result->address = qb_obtain_constant_S32(cxt, FALSE);
				}
			}
		} else {
			qb_abort("%s() expects a constant string as the first parameter", f->name);
		}
	}
}

static void ZEND_FASTCALL qb_translate_intrinsic_unary_matrix_op(qb_compiler_context *cxt, qb_intrinsic_function *f, qb_operand *arguments, uint32_t argument_count, qb_operand *result, qb_result_prototype *result_prototype) {
	qb_primitive_type expr_type = qb_do_type_coercion_for_op(cxt, f->extra, arguments, argument_count, result_prototype);

	if(cxt->stage == QB_STAGE_RESULT_TYPE_RESOLUTION) {
		if(result->type != QB_OPERAND_NONE) {
			result->type = QB_OPERAND_RESULT_PROTOTYPE;
			result->result_prototype = result_prototype;
		}
	} else if(cxt->stage == QB_STAGE_OPCODE_TRANSLATION) {
		qb_address *address = arguments[0].address;

		if(address->dimension_count == 2 && IS_EXPANDABLE_ARRAY(address)) {
			qb_abort("%s() expects an array with fixed dimensions as parameter", f->name);
		}
		if(result->type != QB_OPERAND_NONE) {
			result->type = QB_OPERAND_ADDRESS;
			result->address = qb_obtain_result_storage(cxt, f->extra, arguments, argument_count, expr_type, result_prototype);
			qb_create_op(cxt, f->extra, arguments, argument_count, result);
		}
	}
}

static void ZEND_FASTCALL qb_translate_intrinsic_square_matrix_op(qb_compiler_context *cxt, qb_intrinsic_function *f, qb_operand *arguments, uint32_t argument_count, qb_operand *result, qb_result_prototype *result_prototype) {
	qb_primitive_type expr_type = qb_do_type_coercion_for_op(cxt, f->extra, arguments, argument_count, result_prototype);

	if(cxt->stage == QB_STAGE_RESULT_TYPE_RESOLUTION) {
		if(result->type != QB_OPERAND_NONE) {
			result->type = QB_OPERAND_RESULT_PROTOTYPE;
			result->result_prototype = result_prototype;
		}
	} else if(cxt->stage == QB_STAGE_OPCODE_TRANSLATION) {
		qb_address *address = arguments[0].address;
		uint32_t matrix_cols, matrix_rows;

		if(address->dimension_count == 2 && IS_EXPANDABLE_ARRAY(address)) {
			qb_abort("%s() expects an array with fixed dimensions as parameter", f->name);
		}

		matrix_cols = VALUE(U32, address->dimension_addresses[address->dimension_count - 1]); 
		matrix_rows = VALUE(U32, address->dimension_addresses[address->dimension_count - 2]);
		if(matrix_cols != matrix_rows) {
			qb_abort("%s() expects a square matrix as parameter", f->name);
		}
		if(result->type != QB_OPERAND_NONE) {
			result->type = QB_OPERAND_ADDRESS;
			result->address = qb_obtain_result_storage(cxt, f->extra, arguments, argument_count, expr_type, result_prototype);
			qb_create_op(cxt, f->extra, arguments, argument_count, result);
		}
	}
}

#define MAX_INLINE_FUNCTION_NAME_LEN		32

static qb_intrinsic_function intrinsic_functions[] = {
	{	0,	"count",				qb_translate_intrinsic_count,				1,		2,		NULL 						},
	{	0,	"sizeof",				qb_translate_intrinsic_count,				1,		2,		NULL 						},
	{	0,	"strlen",				qb_translate_intrinsic_count,				1,		2,		NULL 						},
	{	0,	"int8",					qb_translate_intrinsic_cast,				1,		1,		(void *) QB_TYPE_S08		},
	{	0,	"uint8",				qb_translate_intrinsic_cast,				1,		1,		(void *) QB_TYPE_U08		},
	{	0,	"int16",				qb_translate_intrinsic_cast,				1,		1,		(void *) QB_TYPE_S16		},
	{	0,	"uint16",				qb_translate_intrinsic_cast,				1,		1,		(void *) QB_TYPE_U16		},
	{	0,	"int32",				qb_translate_intrinsic_cast,				1,		1,		(void *) QB_TYPE_S32		},
	{	0,	"uint32",				qb_translate_intrinsic_cast,				1,		1,		(void *) QB_TYPE_U32		},
	{	0,	"int64",				qb_translate_intrinsic_cast,				1,		2,		(void *) QB_TYPE_S64		},
	{	0,	"uint64",				qb_translate_intrinsic_cast,				1,		2,		(void *) QB_TYPE_U64		},
	{	0,	"float32",				qb_translate_intrinsic_cast,				1,		1,		(void *) QB_TYPE_F32		},
	{	0,	"float64",				qb_translate_intrinsic_cast,				1,		1,		(void *) QB_TYPE_F64		},
	{	0,	"defined",				qb_translate_intrinsic_defined,				1,		1,		NULL						},
	{	0,	"define",				qb_translate_intrinsic_define,				2,		2,		NULL						},
	{	0,	"pack_le",				qb_translate_intrinsic_pack,				1,		2,		&factory_pack_le			},
	{	0,	"unpack_le",			qb_translate_intrinsic_unpack,				1,		3,		&factory_unpack_le			},
	{	0,	"pack_be",				qb_translate_intrinsic_pack,				1,		2,		&factory_pack_be			},
	{	0,	"unpack_be",			qb_translate_intrinsic_unpack,				1,		3,		&factory_unpack_be			},
	{	0,	"equal",				NULL,										2,		2,		&factory_set_equal			},
	{	0,	"not_equal",			NULL,										2,		2,		&factory_set_not_equal		},
	{	0,	"less_than",			NULL,										2,		2,		&factory_set_less_than		},
	{	0,	"less_than_equal",		NULL,										2,		2,		&factory_set_less_equal		},
	{	0,	"greater_than",			NULL,										2,		2,		&factory_set_greater_than	},
	{	0,	"greater_than_equal",	NULL,										2,		2,		&factory_set_greater_equal	},
	{	0,	"min",					qb_translate_intrinsic_minmax,				1,		256,	&factory_min				},
	{	0,	"max",					qb_translate_intrinsic_minmax,				1,		256,	&factory_max				},
	{	0,	"any",					NULL,										1,		1,		&factory_any				},
	{	0,	"all",					NULL,										1,		1,		&factory_all				},
	{	0,	"not",					NULL,										1,		1,		&factory_set_not			},
	{	0,	"abs",					NULL,										1,		1,		&factory_abs				},
	{	0,	"sin",					NULL,										1,		1,		&factory_sin				},
	{	0,	"asin",					NULL,										1,		1,		&factory_asin				},
	{	0,	"cos",					NULL,										1,		1,		&factory_cos				},
	{	0,	"acos",					NULL,										1,		1,		&factory_acos				},
	{	0,	"tan",					NULL,										1,		1,		&factory_tan				},
	{	0,	"atan",					NULL,										1,		1,		&factory_atan				},
	{	0,	"sinh",					NULL,										1,		1,		&factory_sinh				},
	{	0,	"asinh",				NULL,										1,		1,		&factory_asinh				},
	{	0,	"cosh",					NULL,										1,		1,		&factory_cosh				},
	{	0,	"acosh",				NULL,										1,		1,		&factory_acosh				},
	{	0,	"tanh",					NULL,										1,		1,		&factory_tanh				},
	{	0,	"atanh",				NULL,										1,		1,		&factory_atanh				},
	{	0,	"ceil",					NULL,										1,		1,		&factory_ceil				},
	{	0,	"floor",				NULL,										1,		1,		&factory_floor				},
	{	0,	"round",				qb_translate_intrinsic_round,				1,		3,		&factory_round				},
	{	0,	"log",					NULL,										1,		1,		&factory_log				},
	{	0,	"log1p",				NULL,										1,		1,		&factory_log1p				},
	{	0,	"log10",				NULL,										1,		1,		&factory_log10				},
	{	0,	"log2",					NULL,										1,		1,		&factory_log2				},
	{	0,	"exp",					NULL,										1,		1,		&factory_exp				},
	{	0,	"exp2",					NULL,										1,		1,		&factory_exp2				},
	{	0,	"expm1",				NULL,										1,		1,		&factory_expm1				},
	{	0,	"sqrt",					NULL,										1,		1,		&factory_sqrt				},
	{	0,	"pow",					NULL,										2,		2,		&factory_pow				},
	{	0,	"fmod",					NULL,										2,		2,		&factory_modulo				},
	{	0,	"mod",					NULL,										2,		2,		&factory_floor_modulo		},
	{	0,	"lcg_value",			NULL,										0,		0,		&factory_lcg				},
	{	0,	"is_finite",			NULL,										1,		1,		&factory_is_finite			},
	{	0,	"is_infinite",			NULL,										1,		1,		&factory_is_infinite		},
	{	0,	"is_nan",				NULL,										1,		1,		&factory_is_nan				},
	{	0,	"rand",					qb_translate_intrinsic_rand,				0,		2,		&factory_rand				},
	{	0,	"mt_rand",				qb_translate_intrinsic_rand,				0,		2,		&factory_mt_rand			},
	{	0,	"hypot",				NULL,										2,		2,		&factory_hypot				},
	{	0,	"deg2rad",				NULL,										1,		1,		&factory_deg2rad			},
	{	0,	"rad2deg",				NULL,										1,		1,		&factory_rad2deg			},
	{	0,	"sign",					NULL,										1,		1,		&factory_sign				},
	{	0,	"rsqrt",				NULL,										1,		1,		&factory_rsqrt				},
	{	0,	"fract",				NULL,										1,		1,		&factory_fract				},
	{	0,	"step",					NULL,										2,		2,		&factory_step				},
	{	0,	"clamp",				NULL,										3,		3,		&factory_clamp				},
	{	0,	"mix",					NULL,										3,		3,		&factory_mix				},
	{	0,	"smooth_step",			NULL,										3,		3,		&factory_smooth_step		},
	{	0,	"normalize",			qb_translate_intrinsic_unary_vector_op,		1,		1,		&factory_normalize			},
	{	0,	"length",				qb_translate_intrinsic_unary_vector_op,		1,		1,		&factory_length				},
	{	0,	"distance",				qb_translate_intrinsic_binary_vector_op,	2,		2,		&factory_distance			},
	{	0,	"dot",					qb_translate_intrinsic_binary_vector_op,	2,		2,		&factory_dot_product		},
	{	0,	"cross",				qb_translate_intrinsic_cross,				2,		3,		&factory_cross_product		},
	{	0,	"faceforward",			qb_translate_intrinsic_binary_vector_op,	2,		2,		&factory_faceforward		},
	{	0,	"reflect",				qb_translate_intrinsic_binary_vector_op,	2,		2,		&factory_reflect			},
	{	0,	"refract",				qb_translate_intrinsic_binary_vector_op,	3,		3,		&factory_refract			},
	{	0,	"mm_mult",				qb_translate_intrinsic_mm_mult,				2,		2,		&factory_mm_multiply		},
	{	0,	"mv_mult",				qb_translate_intrinsic_mv_mult,				2,		2,		&factory_mv_multiply		},
	{	0,	"vm_mult",				qb_translate_intrinsic_vm_mult,				2,		2,		&factory_vm_multiply		},
	{	0,	"transpose",			qb_translate_intrinsic_unary_matrix_op,		1,		1,		&factory_transpose			},
	{	0,	"det",					qb_translate_intrinsic_square_matrix_op,	1,		1,		&factory_determinant		},
	{	0,	"inverse",				qb_translate_intrinsic_square_matrix_op,	1,		1,		&factory_inverse			},
	{	0,	"mm_mult_cm",			qb_translate_intrinsic_mm_mult,				2,		2,		&factory_mm_multiply_cm		},
	{	0,	"mv_mult_cm",			qb_translate_intrinsic_mv_mult,				2,		2,		&factory_mv_multiply_cm		},
	{	0,	"vm_mult_cm",			qb_translate_intrinsic_vm_mult,				2,		2,		&factory_vm_multiply_cm		},
	{	0,	"transpose_cm",			qb_translate_intrinsic_unary_matrix_op,		1,		1,		&factory_transpose			},
	{	0,	"det_cm",				qb_translate_intrinsic_square_matrix_op,	1,		1,		&factory_determinant		},
	{	0,	"inverse_cm",			qb_translate_intrinsic_square_matrix_op,	1,		1,		&factory_inverse			},
	{	0,	"mm_mult_rm",			qb_translate_intrinsic_mm_mult,				2,		2,		&factory_mm_multiply_rm		},
	{	0,	"mv_mult_rm",			qb_translate_intrinsic_mv_mult,				2,		2,		&factory_mv_multiply_rm		},
	{	0,	"vm_mult_rm",			qb_translate_intrinsic_vm_mult,				2,		2,		&factory_vm_multiply_rm		},
	{	0,	"transpose_rm",			qb_translate_intrinsic_unary_matrix_op,		1,		1,		&factory_transpose			},
	{	0,	"det_rm",				qb_translate_intrinsic_square_matrix_op,	1,		1,		&factory_determinant		},
	{	0,	"inverse_rm",			qb_translate_intrinsic_square_matrix_op,	1,		1,		&factory_inverse			},
	{	0,	"transform_cm",			qb_translate_intrinsic_transform,			2,		2,		&factory_transform_cm		},
	{	0,	"transform_rm",			qb_translate_intrinsic_transform,			2,		2,		&factory_transform_rm		},
	{	0,	"transform",			qb_translate_intrinsic_transform,			2,		2,		&factory_transform			},
	{	0,	"sample_nearest",		qb_translate_intrinsic_sample_op,			3,		3,		&factory_sample_nearest		},
	{	0,	"sample_bilinear",		qb_translate_intrinsic_sample_op,			3,		3,		&factory_sample_bilinear	},
	{	0,	"blend",				qb_translate_intrinsic_blend,				2,		2,		&factory_alpha_blend		},
	{	0,	"array_pop",			qb_translate_intrinsic_array_pop,			1,		1,		NULL						},
	{	0,	"array_shift",			qb_translate_intrinsic_array_shift,			1,		1,		NULL						},
	{	0,	"array_push",			qb_translate_intrinsic_array_push,			2,		-1,		NULL						},
	{	0,	"array_unshift",		qb_translate_intrinsic_array_unshift,		2,		-1,		NULL						},
	{	0,	"array_merge",			qb_translate_intrinsic_array_merge,			2,		-1,		&factory_array_merge		},
	{	0,	"array_fill",			qb_translate_intrinsic_array_fill,			3,		3,		&factory_array_fill			},
	{	0,	"array_slice",			qb_translate_intrinsic_array_slice,			2,		3,		NULL						},
	{	0,	"substr",				qb_translate_intrinsic_array_slice,			2,		3,		NULL						},
	{	0,	"array_sum",			qb_translate_intrinsic_array_aggregate,		1,		1,		&factory_array_sum			},
	{	0,	"array_product",		qb_translate_intrinsic_array_aggregate,		1,		1,		&factory_array_product		},
	{	0,	"array_search",			qb_translate_intrinsic_array_search,		2,		2,		&factory_array_search		},
	{	0,	"in_array",				qb_translate_intrinsic_array_search,		2,		2,		&factory_in_array			},
	{	0,	"array_pos",			qb_translate_intrinsic_subarray_search,		2,		3,		&factory_subarray_pos		},
	{	0,	"strpos",				qb_translate_intrinsic_subarray_search,		2,		3,		&factory_subarray_pos		},
	{	0,	"array_rpos",			qb_translate_intrinsic_subarray_search,		2,		3,		&factory_subarray_rpos		},
	{	0,	"strrpos",				qb_translate_intrinsic_subarray_search,		2,		3,		&factory_subarray_rpos		},
	{	0,	"sort",					qb_translate_intrinsic_shuffle,				1,		1,		&factory_sort				},
	{	0,	"rsort",				qb_translate_intrinsic_shuffle,				1,		1,		&factory_rsort				},
	{	0,	"shuffle",				qb_translate_intrinsic_shuffle,				1,		1,		&factory_shuffle			},
	{	0,	"array_reverse",		qb_translate_intrinsic_array_reverse,		1,		1,		&factory_array_reverse		},
	{	0,	"array_splice",			qb_translate_intrinsic_array_splice,		2,		4,		NULL						},
	{	0,	"range",				qb_translate_intrinsic_range,				2,		3,		&factory_range				},
	{	0,	"array_unique",			qb_translate_intrinsic_array_reverse,		1,		1,		&factory_array_unique		},
	{	0,	"array_column",			qb_translate_intrinsic_array_column,		2,		2,		&factory_array_column		},
	{	0,	"array_diff",			qb_translate_intrinsic_array_diff,			2,		-1,		&factory_array_diff			},
	{	0,	"array_intersect",		qb_translate_intrinsic_array_diff,			2,		-1,		&factory_array_intersect	},
	{	0,	"array_rand",			qb_translate_intrinsic_array_rand,			1,		2,		&factory_array_rand			},
	{	0,	"array_resize",			qb_translate_intrinsic_array_resize,		2,		-1,		&factory_array_resize		},
	{	0,	"utf8_decode",			qb_translate_intrinsic_utf8_decode,			1,		1,		&factory_utf8_decode		},
	{	0,	"utf8_encode",			qb_translate_intrinsic_utf8_encode,			1,		1,		&factory_utf8_encode		},
	{	0,	"cabs",					qb_translate_intrinsic_complex,				1,		1,		&factory_complex_abs		},
	{	0,	"carg",					qb_translate_intrinsic_complex,				1,		1,		&factory_complex_arg		},
	{	0,	"cmult",				qb_translate_intrinsic_complex,				2,		2,		&factory_complex_multiply	},
	{	0,	"cdiv",					qb_translate_intrinsic_complex,				2,		2,		&factory_complex_divide		},
	{	0,	"cexp",					qb_translate_intrinsic_complex,				1,		1,		&factory_complex_exp		},
	{	0,	"clog",					qb_translate_intrinsic_complex,				1,		1,		&factory_complex_log		},
	{	0,	"csqrt",				qb_translate_intrinsic_complex,				1,		1,		&factory_complex_sqrt		},
	{	0,	"cpow",					qb_translate_intrinsic_complex,				2,		2,		&factory_complex_pow		},
	{	0,	"csin",					qb_translate_intrinsic_complex,				1,		1,		&factory_complex_sin		},
	{	0,	"ccos",					qb_translate_intrinsic_complex,				1,		1,		&factory_complex_cos		},
	{	0,	"ctan",					qb_translate_intrinsic_complex,				1,		1,		&factory_complex_tan		},
	{	0,	"csinh",				qb_translate_intrinsic_complex,				1,		1,		&factory_complex_sinh		},
	{	0,	"ccosh",				qb_translate_intrinsic_complex,				1,		1,		&factory_complex_cosh		},
	{	0,	"ctanh",				qb_translate_intrinsic_complex,				1,		1,		&factory_complex_tanh		},
};

static qb_intrinsic_function * ZEND_FASTCALL qb_find_intrinsic_function(qb_compiler_context *cxt, zval *callable) {
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
		hash_value = zend_inline_hash_func(name_buffer, len + 1);
		for(i = 0; i < sizeof(intrinsic_functions) / sizeof(qb_intrinsic_function); i++) {
			qb_intrinsic_function *f = &intrinsic_functions[i];
			if(f->hash_value == hash_value && strcmp(name_buffer, f->name) == 0) {
				return f;
			}
		}
	}
	return NULL;
}
