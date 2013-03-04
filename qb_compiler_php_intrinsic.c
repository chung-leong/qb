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

static void ZEND_FASTCALL qb_translate_intrinsic_unary_vector_op(qb_compiler_context *cxt, qb_intrinsic_function *f, qb_operand *arguments, uint32_t argument_count, qb_operand *result) {
	uint32_t expr_type = qb_coerce_operands(cxt, f->extra, arguments, argument_count);
	qb_address *address = arguments[0].address;
	uint32_t vector_width = qb_get_vector_width(cxt, address);

	if(address->dimension_count == 1 && IS_EXPANDABLE_ARRAY(address)) {
		qb_abort("%s() expects an array with fixed dimension as parameter", f->name);
	}
	if(vector_width > 4096) {
		qb_abort("Dimension is too large");
	}
	if(result->type != QB_OPERAND_NONE) {
		result->address = qb_obtain_result_storage(cxt, f->extra, arguments, argument_count, expr_type);
		qb_create_op(cxt, f->extra, arguments, argument_count, result);
	}
}

static void ZEND_FASTCALL qb_translate_intrinsic_binary_vector_op(qb_compiler_context *cxt, qb_intrinsic_function *f, qb_operand *arguments, uint32_t argument_count, qb_operand *result) {
	uint32_t expr_type = qb_coerce_operands(cxt, f->extra, arguments, argument_count);
	qb_address *address1 = arguments[0].address;
	qb_address *address2 = arguments[1].address;
	uint32_t vector_width1 = qb_get_vector_width(cxt, address1);
	uint32_t vector_width2 = qb_get_vector_width(cxt, address2);

	if((address1->dimension_count == 1 && IS_EXPANDABLE_ARRAY(address1))
	|| (address2->dimension_count == 1 && IS_EXPANDABLE_ARRAY(address2))) {
		qb_abort("%s() expects arrays with one fixed dimension as parameters", f->name);
	}
	if(vector_width1 != vector_width2) {
		qb_abort("Dimension of first parameter (%d) does not match dimension of second parameter (%d)", vector_width1, vector_width2);
	}
	if(vector_width1 > 4096) {
		qb_abort("Dimension is too large");
	}
	if(result->type != QB_OPERAND_NONE) {
		result->address = qb_obtain_result_storage(cxt, f->extra, arguments, argument_count, expr_type);
		qb_create_op(cxt, f->extra, arguments, argument_count, result);
	}
}

static void ZEND_FASTCALL qb_translate_intrinsic_cross(qb_compiler_context *cxt, qb_intrinsic_function *f, qb_operand *arguments, uint32_t argument_count, qb_operand *result) {
	uint32_t expr_type = qb_coerce_operands(cxt, f->extra, arguments, argument_count);
	qb_address *address1 = arguments[0].address;
	qb_address *address2 = arguments[1].address;
	uint32_t vector_width1 = qb_get_vector_width(cxt, address1);
	uint32_t vector_width2 = qb_get_vector_width(cxt, address2);

	if((address1->dimension_count == 1 && IS_EXPANDABLE_ARRAY(address1))
	|| (address2->dimension_count == 1 && IS_EXPANDABLE_ARRAY(address2))) {
		qb_abort("%s() expects arrays with one fixed dimension as parameters", f->name);
	}
	if(vector_width1 != vector_width2) {
		qb_abort("Dimension of first parameter (%d) does not match dimension of second parameter (%d)", vector_width1, vector_width2);
	}
	if(vector_width1 != 3) {
		qb_abort("%s() only accepts three-dimensional vectors", f->name);
	}
	if(result->type != QB_OPERAND_NONE) {
		result->address = qb_obtain_result_storage(cxt, f->extra, arguments, argument_count, expr_type);
		qb_create_op(cxt, f->extra, arguments, argument_count, result);
	}
}

static void ZEND_FASTCALL qb_translate_intrinsic_mm_mult(qb_compiler_context *cxt, qb_intrinsic_function *f, qb_operand *arguments, uint32_t argument_count, qb_operand *result) {
	uint32_t expr_type = qb_coerce_operands(cxt, f->extra, arguments, argument_count);
	qb_address *m1_address = arguments[0].address;
	qb_address *m2_address = arguments[1].address;
	uint32_t m1_cols = qb_get_matrix_column_count(cxt, m1_address);
	uint32_t m1_rows = qb_get_matrix_row_count(cxt, m1_address);
	uint32_t m2_cols = qb_get_matrix_column_count(cxt, m2_address);
	uint32_t m2_rows = qb_get_matrix_row_count(cxt, m2_address);

	if(!(m1_address->dimension_count >= 2 && m2_address->dimension_count >= 2)) {
		qb_abort("%s() expects two-dimensional arrays as parameters", f->name);
	}
	if((m1_address->dimension_count == 2 && IS_EXPANDABLE_ARRAY(m1_address))
	|| (m2_address->dimension_count == 2 && IS_EXPANDABLE_ARRAY(m2_address))) {
		qb_abort("%s() expects arrays with two fixed dimensions as parameters", f->name);
	}
	if(m1_cols != m2_rows) {
		qb_abort("Number of columns in first matrix (%d) does not match number of rows in second matrix (%d)", m1_cols, m2_rows);
	}
	if(m1_rows > 4096 || m1_cols > 4096 || m2_cols > 4096) {
		qb_abort("Dimension is too large");
	}
	if(result->type != QB_OPERAND_NONE) {
		result->address = qb_obtain_result_storage(cxt, f->extra, arguments, argument_count, expr_type);
		qb_create_op(cxt, f->extra, arguments, argument_count, result);
	}
}

static void ZEND_FASTCALL qb_translate_intrinsic_mv_mult(qb_compiler_context *cxt, qb_intrinsic_function *f, qb_operand *arguments, uint32_t argument_count, qb_operand *result) {
	uint32_t expr_type = qb_coerce_operands(cxt, f->extra, arguments, argument_count);
	qb_address *m_address = arguments[0].address;
	qb_address *v_address = arguments[1].address;
	uint32_t m_cols = qb_get_matrix_column_count(cxt, m_address);
	uint32_t m_rows = qb_get_matrix_column_count(cxt, m_address);
	uint32_t v_width = qb_get_vector_width(cxt, v_address);

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
	if(m_cols != v_width) {
		qb_abort("Number of columns in matrix (%d) does not match number of elements in vector (%d)", m_cols, v_width);
	}
	if(v_width > 4096 || m_rows > 4096) {
		qb_abort("Dimension is too large");
	}
	if(result->type != QB_OPERAND_NONE) {
		result->address = qb_obtain_result_storage(cxt, f->extra, arguments, argument_count, expr_type);
		qb_create_op(cxt, f->extra, arguments, argument_count, result);
	}
}

static void ZEND_FASTCALL qb_translate_intrinsic_vm_mult(qb_compiler_context *cxt, qb_intrinsic_function *f, qb_operand *arguments, uint32_t argument_count, qb_operand *result) {
	uint32_t expr_type = qb_coerce_operands(cxt, f->extra, arguments, argument_count);
	qb_address *v_address = arguments[0].address;
	qb_address *m_address = arguments[1].address;
	uint32_t v_width = qb_get_vector_width(cxt, v_address);
	uint32_t m_rows = qb_get_matrix_row_count(cxt, m_address);
	uint32_t m_cols = qb_get_matrix_column_count(cxt, m_address);

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
	if(v_width != m_rows) {
		qb_abort("Number of elements in vector (%d) does not match number of rows in matrix (%d)", v_width, m_rows);
	}
	if(v_width > 4096 || m_cols > 4096) {
		qb_abort("Dimension is too large");
	}

	if(result->type != QB_OPERAND_NONE) {
		result->address = qb_obtain_result_storage(cxt, f->extra, arguments, argument_count, expr_type);
		qb_create_op(cxt, f->extra, arguments, argument_count, result);
	}
}

static void ZEND_FASTCALL qb_translate_intrinsic_count(qb_compiler_context *cxt, qb_intrinsic_function *f, qb_operand *arguments, uint32_t argument_count, qb_operand *result) {
	int32_t recursive = 0;
	qb_address *count_address, *address = arguments[0].address;
	qb_do_type_coercion(cxt, &arguments[0], QB_TYPE_ANY);
	if(argument_count == 2) {
		qb_do_type_coercion(cxt, &arguments[1], QB_TYPE_I32);
		if(arguments[1].address->flags & QB_ADDRESS_CONSTANT) {
			recursive = VALUE(I32, arguments[1].address);
		} else {
			qb_abort("%s() expects parameter 2 to be a constant expression", f->name);
		}
	}

	if(!IS_SCALAR(address)) {
		count_address = (recursive) ? address->array_size_addresses[0] : address->dimension_addresses[0];
	} else {
		count_address = qb_obtain_constant_U32(cxt, 1);
	}
	result->address = count_address;
}

static void ZEND_FASTCALL qb_translate_intrinsic_rand(qb_compiler_context *cxt, qb_intrinsic_function *f, qb_operand *arguments, uint32_t argument_count, qb_operand *result) {
	qb_address *result_size_address = NULL, *min_address, *max_address;
	uint32_t expr_type;

	if(argument_count != 0 && argument_count != 2) {
		qb_abort("%s() expects either 0 or 2 arguments", f->name);
	}

	// use the lvalue type to determine the type
	expr_type = qb_get_lvalue_type(cxt, QB_TYPE_I32);
	if(expr_type == QB_TYPE_F32 || expr_type == QB_TYPE_F64) {
		expr_type = QB_TYPE_I32;
	}

	if(argument_count == 0) {
		min_address = qb_obtain_constant(cxt, 0, expr_type);
		max_address = qb_obtain_constant(cxt, (int64_t) integer_upper_bounds[expr_type], expr_type);
	} else {
		qb_do_type_coercion(cxt, &arguments[0], expr_type);
		qb_do_type_coercion(cxt, &arguments[1], expr_type);
		min_address = arguments[0].address;
		max_address = arguments[1].address;
		result_size_address = qb_get_largest_array_size(cxt, arguments, 2);
	}

	if(result->type != QB_OPERAND_NONE) {
		result->address = qb_obtain_temporary_variable(cxt, expr_type, result_size_address);
		qb_create_binary_op(cxt, f->extra, min_address, max_address, result->address);
	}
}

static void ZEND_FASTCALL qb_translate_intrinsic_round(qb_compiler_context *cxt, qb_intrinsic_function *f, qb_operand *arguments, uint32_t argument_count, qb_operand *result) {
	qb_operand *value = &arguments[0], *precision = &arguments[1], *mode = &arguments[2];
	qb_address *precision_address, *mode_address, *address, *result_size_address;
	uint32_t expr_type = qb_get_operand_type(cxt, &arguments[0], QB_COERCE_TO_FLOATING_POINT);
	qb_do_type_coercion(cxt, &arguments[0], expr_type);
	address = value->address;

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
		mode_address = qb_obtain_constant_S32(cxt, PHP_ROUND_HALF_UP);
	}

	if(result->type != QB_OPERAND_NONE) {
		result_size_address = address->array_size_address;
		result->address = qb_obtain_temporary_variable(cxt, expr_type, result_size_address);
		qb_create_ternary_op(cxt, f->extra, address, precision_address, mode_address, result->address);
	}
}

static void ZEND_FASTCALL qb_translate_intrinsic_array_push(qb_compiler_context *cxt, qb_intrinsic_function *f, qb_operand *arguments, uint32_t argument_count, qb_operand *result) {
	qb_operand *container = &arguments[0], *value = &arguments[1];
	qb_address *size_address, *variable_address;
	if(container->type != QB_OPERAND_ADDRESS || IS_SCALAR(container->address)) {
		qb_abort("%s expects an array as parameter", f->name);
	}
	if(!IS_EXPANDABLE_ARRAY(container->address)) {
		qb_abort("Adding element to an array that cannot expand");
	}
	qb_do_type_coercion(cxt, value, container->address->type);
	size_address = container->address->dimension_addresses[0];
	variable_address = qb_get_array_element(cxt, container->address, size_address);
	qb_do_assignment(cxt, value->address, variable_address);
	if(result->type != QB_OPERAND_NONE) {
		result->address = size_address;
	}
}

static void ZEND_FASTCALL qb_translate_intrinsic_array_pop(qb_compiler_context *cxt, qb_intrinsic_function *f, qb_operand *arguments, uint32_t argument_count, qb_operand *result) {
	qb_operand *container = &arguments[0];
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
		result->address = qb_obtain_temporary_variable(cxt, variable_address->type, variable_address->array_size_address);
		qb_create_unary_op(cxt, &factory_copy, variable_address, result->address);
	}
	qb_create_nullary_op(cxt, &factory_unset, variable_address);
}

static void ZEND_FASTCALL qb_translate_intrinsic_array_shift(qb_compiler_context *cxt, qb_intrinsic_function *f, qb_operand *arguments, uint32_t argument_count, qb_operand *result) {
	qb_operand *container = &arguments[0];
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
		result->address = qb_obtain_temporary_variable(cxt, variable_address->type, variable_address->array_size_address);
		qb_do_assignment(cxt, variable_address, result->address);
	}
	qb_create_nullary_op(cxt, &factory_unset, variable_address);
}

static void ZEND_FASTCALL qb_translate_intrinsic_array_slice(qb_compiler_context *cxt, qb_intrinsic_function *f, qb_operand *arguments, uint32_t argument_count, qb_operand *result) {
	qb_operand *container = &arguments[0], *offset = &arguments[1], *length = &arguments[2];
	qb_do_type_coercion(cxt, container, QB_TYPE_ANY);
	if(IS_SCALAR(container->address)) {
		qb_abort("%s expects an array as parameter", f->name);
	}
	qb_do_type_coercion(cxt, offset, QB_TYPE_U32);
	if(argument_count >= 3) {
		qb_do_type_coercion(cxt, length, QB_TYPE_U32);
	}
	if(result->type != QB_OPERAND_NONE) {
		result->address = qb_get_array_slice(cxt, container->address, offset->address, (argument_count >= 3) ? length->address : NULL);
	}
}

static void ZEND_FASTCALL qb_translate_intrinsic_array_aggregate(qb_compiler_context *cxt, qb_intrinsic_function *f, qb_operand *arguments, uint32_t argument_count, qb_operand *result) {
	qb_operand *container = &arguments[0];
	uint32_t expr_type;

	qb_do_type_coercion(cxt, container, QB_TYPE_ANY);
	expr_type = container->address->type;
	if(IS_SCALAR(container->address)) {
		qb_abort("%s expects an array as parameter", f->name);
	}
	if(result->type != QB_OPERAND_NONE) {
		result->address = qb_obtain_temporary_variable(cxt, expr_type, NULL);
		qb_create_op(cxt, f->extra, arguments, 1, result);
	}
}

static void ZEND_FASTCALL qb_translate_intrinsic_array_search(qb_compiler_context *cxt, qb_intrinsic_function *f, qb_operand *arguments, uint32_t argument_count, qb_operand *result) {
	qb_operand *container = &arguments[0];
	qb_operand *needle = &arguments[1];
	uint32_t expr_type;

	qb_do_type_coercion(cxt, container, QB_TYPE_ANY);
	expr_type = container->address->type;
	qb_do_type_coercion(cxt, needle, expr_type);
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
		result->address = qb_obtain_temporary_variable(cxt, QB_TYPE_S32, NULL);
		qb_create_op(cxt, f->extra, arguments, 2, result);
	}
}

static void ZEND_FASTCALL qb_translate_intrinsic_subarray_search(qb_compiler_context *cxt, qb_intrinsic_function *f, qb_operand *arguments, uint32_t argument_count, qb_operand *result) {
	qb_operand *container = &arguments[0];
	qb_operand *needle = &arguments[1];
	qb_operand *offset = &arguments[2];
	qb_address *offset_address;
	uint32_t expr_type;

	qb_do_type_coercion(cxt, container, QB_TYPE_ANY);
	expr_type = container->address->type;
	qb_do_type_coercion(cxt, needle, expr_type);
	if(container->address->dimension_count != 1) {
		qb_abort("%s expects a one-dimensional array as the first parameter", f->name);
	} 
	if(needle->address->dimension_count > 1) {
		qb_abort("%s expects a one-dimensional array as the second parameter", f->name);
	}
	if(argument_count >= 3) {
		qb_do_type_coercion(cxt, offset, QB_TYPE_S32);
		if(!IS_SCALAR(offset->address)) {
			qb_abort("%s expects a scalar as the third parameter", f->name);
		}
		if(offset->address->segment_offset == QB_OFFSET_INVALID) {
			// need to copy the index value to a temporary variable first
			offset_address = qb_obtain_temporary_variable(cxt, QB_TYPE_S32, NULL);
			qb_do_assignment(cxt, offset_address, offset->address);
		} else {
			offset_address = offset->address;
		}
	} else {
		offset_address = qb_obtain_constant_S32(cxt, 0);
	}
	if(result->type != QB_OPERAND_NONE) {
		result->address = qb_obtain_temporary_variable(cxt, QB_TYPE_S32, NULL);
		qb_create_ternary_op(cxt, f->extra, container->address, needle->address, offset_address, result->address);
	}
}

static void ZEND_FASTCALL qb_translate_intrinsic_sort(qb_compiler_context *cxt, qb_intrinsic_function *f, qb_operand *arguments, uint32_t argument_count, qb_operand *result) {
	qb_operand *container = &arguments[0];
	if(container->type != QB_OPERAND_ADDRESS || IS_SCALAR(container->address) || container->address->dimension_count > 1) {
		qb_abort("%s expects a one-dimensional array as parameter", f->name);
	}
	qb_create_op(cxt, f->extra, NULL, 0, container);
	if(result->type != QB_OPERAND_NONE) {
		result->address = qb_obtain_constant_S32(cxt, 1);
	}
}

static void ZEND_FASTCALL qb_translate_intrinsic_array_reverse(qb_compiler_context *cxt, qb_intrinsic_function *f, qb_operand *arguments, uint32_t argument_count, qb_operand *result) {
	qb_operand *container = &arguments[0];

	if(IS_SCALAR(container->address)) {
		qb_abort("%s expects an array as the first parameter", f->name);
	}
	qb_do_type_coercion(cxt, container, QB_TYPE_ANY);
	if(result->type != QB_OPERAND_NONE) {
		qb_address *count_address = container->address->dimension_addresses[0];
		qb_address *width_address;
		if(container->address->dimension_count > 1) {
			width_address = container->address->array_size_addresses[1];
		} else {
			width_address = qb_obtain_constant_U32(cxt, 1);
		}
		result->address = qb_obtain_temporary_variable(cxt, container->address->type, container->address->array_size_address);
		qb_create_ternary_op(cxt, f->extra, container->address, count_address, width_address, result->address);
	}
}

static void ZEND_FASTCALL qb_translate_intrinsic_utf8_decode(qb_compiler_context *cxt, qb_intrinsic_function *f, qb_operand *arguments, uint32_t argument_count, qb_operand *result) {
	qb_operand *source = &arguments[0];
	uint32_t result_type = qb_get_lvalue_type(cxt, QB_TYPE_U16);
	qb_do_type_coercion(cxt, source, QB_TYPE_U08);

	if(result->type != QB_OPERAND_NONE) {
		if((result_type & ~QB_TYPE_UNSIGNED) != QB_TYPE_I16 && (result_type & ~QB_TYPE_UNSIGNED) != QB_TYPE_I32) {
			result_type = QB_TYPE_U16;
		}
		result->address = qb_obtain_temporary_variable_length_array(cxt, result_type);
		qb_create_op(cxt, f->extra, arguments, argument_count, result);
	}
}

static void ZEND_FASTCALL qb_translate_intrinsic_utf8_encode(qb_compiler_context *cxt, qb_intrinsic_function *f, qb_operand *arguments, uint32_t argument_count, qb_operand *result) {
	qb_operand *source = &arguments[0];
	uint32_t source_type = QB_TYPE_U16;
	if(source->type == QB_OPERAND_ADDRESS && (source->address->type & ~QB_TYPE_UNSIGNED) == QB_TYPE_I32) {
		source_type = QB_TYPE_U32;
	}
	qb_do_type_coercion(cxt, source, source_type);

	if(result->type != QB_OPERAND_NONE) {
		result->address = qb_obtain_temporary_variable_length_array(cxt, QB_TYPE_I08);
		result->address->flags |= QB_ADDRESS_STRING;
		qb_create_op(cxt, f->extra, arguments, argument_count, result);
	}
}

static void ZEND_FASTCALL qb_translate_intrinsic_pack(qb_compiler_context *cxt, qb_intrinsic_function *f, qb_operand *arguments, uint32_t argument_count, qb_operand *result) {
	qb_operand *value = &arguments[0], *type = &arguments[1];
	qb_address *result_size_address;
	uint32_t arg_type;
	if(argument_count >= 2) {
		if(type->type == QB_OPERAND_ZVAL && type->constant->type == IS_LONG) {
			arg_type = type->constant->value.lval;
		} else {
			qb_abort("%s expects the second parameter to be a constant indicating the type", f->name);
		}
	} else {
		if(value->type == QB_OPERAND_ADDRESS) {
			arg_type = value->address->type;
		} else {
			qb_abort("%s() requires the second parameter when the input type cannot be determined", f->name);
		}
	}
	qb_do_type_coercion(cxt, value, arg_type);
	if(!IS_SCALAR(value->address)) {
		qb_abort("%s expects a scalar as the first parameter", f->name);
	}

	if(result->type != QB_OPERAND_NONE) {
		result_size_address = qb_obtain_constant_U32(cxt, type_sizes[value->address->type]);
		result->address = qb_obtain_temporary_variable(cxt, QB_TYPE_U08, result_size_address);
		result->address->flags |= QB_ADDRESS_STRING;
		qb_create_op(cxt, f->extra, arguments, 1, result);
	}
}

static void ZEND_FASTCALL qb_translate_intrinsic_unpack(qb_compiler_context *cxt, qb_intrinsic_function *f, qb_operand *arguments, uint32_t argument_count, qb_operand *result) {
	qb_operand *array = &arguments[0], *offset = &arguments[1], *type = &arguments[2];
	qb_address *slice_address, *offset_address, *length_address;
	uint32_t result_type;

	qb_do_type_coercion(cxt, array, QB_TYPE_U08);
	if(IS_SCALAR(array->address) || array->address->dimension_count > 1) {
		qb_abort("%s expects a one-dimenstional array as the first parameter", f->name);
	}
	if(argument_count >= 2) {
		qb_do_type_coercion(cxt, offset, QB_TYPE_U32);
		if(!IS_SCALAR(offset->address)) {
			qb_abort("%s expects a scalar as the second parameter", f->name);
		}
		offset_address = offset->address;
	} else {
		offset_address = qb_obtain_constant_U32(cxt, 0);
	}
	if(argument_count >= 3) {
		if(type->type == QB_OPERAND_ZVAL && type->constant->type == IS_LONG) {
			result_type = type->constant->value.lval;
		} else {
			qb_abort("%s expects the third parameter to be a constant indicating the type", f->name);
		}
	} else {
		result_type = qb_get_lvalue_type(cxt, QB_TYPE_VOID);
		if(result_type == QB_TYPE_VOID) {
			qb_abort("%s() requires the third parameter when the desired type cannot be determined from context", f->name);
		}
	}
	if(result->type != QB_OPERAND_NONE) {
		length_address = qb_obtain_constant_U32(cxt, type_sizes[result_type]);
		slice_address = qb_get_array_slice(cxt, array->address, offset_address, length_address);
		result->address = qb_obtain_temporary_variable(cxt, result_type, NULL);
		qb_create_unary_op(cxt, f->extra, slice_address, result->address);
	}
}

static void ZEND_FASTCALL qb_translate_intrinsic_minmax(qb_compiler_context *cxt, qb_intrinsic_function *f, qb_operand *arguments, uint32_t argument_count, qb_operand *result) {
	if(argument_count == 1) {
		qb_operand *container = &arguments[0];
		uint32_t expr_type;

		qb_do_type_coercion(cxt, container, QB_TYPE_ANY);
		expr_type = container->address->type;
		if(IS_SCALAR(container->address)) {
			qb_abort("%s expects an array as parameter when only one parameter is given", f->name);
		}
		if(result->type != QB_OPERAND_NONE) {
			result->address = qb_obtain_temporary_variable(cxt, expr_type, NULL);
			qb_create_op(cxt, f->extra, arguments, 1, result);
		}
	} else {
		uint32_t expr_type = qb_get_highest_rank_type(cxt, arguments, argument_count, 0);
		uint32_t i = 0; 

		for(i = 0; i < argument_count; i++) {
			qb_do_type_coercion(cxt, &arguments[i], expr_type);
		}
		if(result->type != QB_OPERAND_NONE) {
			result->address = qb_obtain_temporary_variable(cxt, expr_type, NULL);
			qb_create_binary_op(cxt, f->extra, arguments[0].address, arguments[1].address, result->address);
			for(i = 2; i < argument_count; i++) {
				qb_create_binary_op(cxt, f->extra, result->address, arguments[i].address, result->address);
			}
		}
	}
}

static void ZEND_FASTCALL qb_translate_intrinsic_cast(qb_compiler_context *cxt, qb_intrinsic_function *f, qb_operand *arguments, uint32_t argument_count, qb_operand *result) {
	qb_operand *expr = &arguments[0];
	uint32_t type = (uintptr_t) f->extra;

	if(argument_count == 2) {
		qb_operand *hi_dword = &arguments[0], *lo_dword = &arguments[1];
		qb_do_type_coercion(cxt, hi_dword, QB_TYPE_U32);
		qb_do_type_coercion(cxt, lo_dword, QB_TYPE_U32);
		if(!cxt->resolving_result_type) {
			if(hi_dword->address->flags & QB_ADDRESS_CONSTANT && lo_dword->address->flags & QB_ADDRESS_CONSTANT) {
				uint64_t hi = VALUE(U32, hi_dword->address);
				uint64_t lo = VALUE(U32, lo_dword->address);
				uint64_t value = hi << 32 | lo;
				if(type == QB_TYPE_S64) {
					result->address = qb_obtain_constant_S64(cxt, (int64_t) value);
				} else {
					result->address = qb_obtain_constant_U64(cxt, value);
				}
			} else {
				qb_abort("%s() expects two constant scalar expressions when called with two parameters", f->name);
			}
		} else {
			// just put something there
			result->address = qb_obtain_temporary_variable(cxt, type, NULL);
		}
	} else {
		qb_do_type_coercion(cxt, expr, type);
		result->address = expr->address;
	}
}

static void ZEND_FASTCALL qb_translate_intrinsic_defined(qb_compiler_context *cxt, qb_intrinsic_function *f, qb_operand *arguments, uint32_t argument_count, qb_operand *result) {
	USE_TSRM
	qb_operand *name = &arguments[0];
	if(name->type == QB_OPERAND_ZVAL && Z_TYPE_P(name->constant) == IS_STRING) {
		zval c;
		char *name_str = Z_STRVAL_P(name->constant);
		uint32_t name_len = Z_STRLEN_P(name->constant);
#if !ZEND_ENGINE_2_2 && !ZEND_ENGINE_2_1
		if(zend_get_constant_ex(name_str, name_len, &c, NULL, ZEND_FETCH_CLASS_SILENT TSRMLS_CC)) {
#else
		if(zend_get_constant(name_str, name_len, &c TSRMLS_CC)) {
#endif
			zval_dtor(&c);
			result->address = qb_obtain_constant_S32(cxt, 1);
		} else {
			result->address = qb_obtain_constant_S32(cxt, 0);
		}
	} else {
		qb_abort("%s() expects a constant string", f->name);
	}
}

static void ZEND_FASTCALL qb_translate_intrinsic_define(qb_compiler_context *cxt, qb_intrinsic_function *f, qb_operand *arguments, uint32_t argument_count, qb_operand *result) {
	USE_TSRM
	qb_operand *name = &arguments[0];
	qb_operand *expr = &arguments[1];
	if(name->type == QB_OPERAND_ZVAL && Z_TYPE_P(name->constant) == IS_STRING) {
		zend_constant c;
		char *name_str = Z_STRVAL_P(name->constant);
		uint32_t name_len = Z_STRLEN_P(name->constant);
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
		if(zend_register_constant(&c TSRMLS_CC) == SUCCESS) {
			result->address = qb_obtain_constant_S32(cxt, TRUE);
		} else {
			result->address = qb_obtain_constant_S32(cxt, FALSE);
		}
	} else {
		qb_abort("%s() expects a constant string as the first parameter", f->name);
	}
}

#define MAX_INLINE_FUNCTION_NAME_LEN		32

static qb_intrinsic_function intrinsic_functions[] = {
	{	0,	"count",				NULL,		qb_translate_intrinsic_count,				1,		2,		NULL 						},
	{	0,	"sizeof",				NULL,		qb_translate_intrinsic_count,				1,		2,		NULL 						},
	{	0,	"strlen",				NULL,		qb_translate_intrinsic_count,				1,		2,		NULL 						},
	{	0,	"int8",					NULL,		qb_translate_intrinsic_cast,				1,		1,		(void *) QB_TYPE_S08		},
	{	0,	"uint8",				NULL,		qb_translate_intrinsic_cast,				1,		1,		(void *) QB_TYPE_U08		},
	{	0,	"int16",				NULL,		qb_translate_intrinsic_cast,				1,		1,		(void *) QB_TYPE_S16		},
	{	0,	"uint16",				NULL,		qb_translate_intrinsic_cast,				1,		1,		(void *) QB_TYPE_U16		},
	{	0,	"int32",				NULL,		qb_translate_intrinsic_cast,				1,		1,		(void *) QB_TYPE_S32		},
	{	0,	"uint32",				NULL,		qb_translate_intrinsic_cast,				1,		1,		(void *) QB_TYPE_U32		},
	{	0,	"int64",				NULL,		qb_translate_intrinsic_cast,				1,		2,		(void *) QB_TYPE_S64		},
	{	0,	"uint64",				NULL,		qb_translate_intrinsic_cast,				1,		2,		(void *) QB_TYPE_U64		},
	{	0,	"float32",				NULL,		qb_translate_intrinsic_cast,				1,		1,		(void *) QB_TYPE_F32		},
	{	0,	"float64",				NULL,		qb_translate_intrinsic_cast,				1,		1,		(void *) QB_TYPE_F64		},
	{	0,	"defined",				NULL,		qb_translate_intrinsic_defined,				1,		1,		NULL						},
	{	0,	"define",				NULL,		qb_translate_intrinsic_define,				2,		2,		NULL						},
	{	0,	"pack_le",				NULL,		qb_translate_intrinsic_pack,				1,		2,		&factory_pack_le			},
	{	0,	"unpack_le",			NULL,		qb_translate_intrinsic_unpack,				1,		3,		&factory_unpack_le			},
	{	0,	"pack_be",				NULL,		qb_translate_intrinsic_pack,				1,		2,		&factory_pack_be			},
	{	0,	"unpack_be",			NULL,		qb_translate_intrinsic_unpack,				1,		3,		&factory_unpack_be			},
	{	0,	"equal",				NULL,		NULL,										2,		2,		&factory_set_equal			},
	{	0,	"not_equal",			NULL,		NULL,										2,		2,		&factory_set_not_equal		},
	{	0,	"less_than",			NULL,		NULL,										2,		2,		&factory_set_less_than		},
	{	0,	"less_than_equal",		NULL,		NULL,										2,		2,		&factory_set_less_equal		},
	{	0,	"greater_than",			NULL,		NULL,										2,		2,		&factory_set_greater_than	},
	{	0,	"greater_than_equal",	NULL,		NULL,										2,		2,		&factory_set_greater_equal	},
	{	0,	"min",					NULL,		qb_translate_intrinsic_minmax,				1,		256,	&factory_min				},
	{	0,	"max",					NULL,		qb_translate_intrinsic_minmax,				1,		256,	&factory_max				},
	{	0,	"any",					NULL,		NULL,										1,		1,		&factory_any				},
	{	0,	"all",					NULL,		NULL,										1,		1,		&factory_all				},
	{	0,	"not",					NULL,		NULL,										1,		1,		&factory_set_not			},
	{	0,	"abs",					NULL,		NULL,										1,		1,		&factory_abs				},
	{	0,	"sin",					NULL,		NULL,										1,		1,		&factory_sin				},
	{	0,	"asin",					NULL,		NULL,										1,		1,		&factory_asin				},
	{	0,	"cos",					NULL,		NULL,										1,		1,		&factory_cos				},
	{	0,	"acos",					NULL,		NULL,										1,		1,		&factory_acos				},
	{	0,	"tan",					NULL,		NULL,										1,		1,		&factory_tan				},
	{	0,	"atan",					NULL,		NULL,										1,		1,		&factory_atan				},
	{	0,	"sinh",					NULL,		NULL,										1,		1,		&factory_sinh				},
	{	0,	"asinh",				NULL,		NULL,										1,		1,		&factory_asinh				},
	{	0,	"cosh",					NULL,		NULL,										1,		1,		&factory_cosh				},
	{	0,	"acosh",				NULL,		NULL,										1,		1,		&factory_acosh				},
	{	0,	"tanh",					NULL,		NULL,										1,		1,		&factory_tanh				},
	{	0,	"atanh",				NULL,		NULL,										1,		1,		&factory_atanh				},
	{	0,	"ceil",					NULL,		NULL,										1,		1,		&factory_ceil				},
	{	0,	"floor",				NULL,		NULL,										1,		1,		&factory_floor				},
	{	0,	"round",				NULL,		qb_translate_intrinsic_round,				1,		3,		&factory_round				},
	{	0,	"log",					NULL,		NULL,										1,		1,		&factory_log				},
	{	0,	"log1p",				NULL,		NULL,										1,		1,		&factory_log1p				},
	{	0,	"log10",				NULL,		NULL,										1,		1,		&factory_log10				},
	{	0,	"log2",					NULL,		NULL,										1,		1,		&factory_log2				},
	{	0,	"exp",					NULL,		NULL,										1,		1,		&factory_exp				},
	{	0,	"exp2",					NULL,		NULL,										1,		1,		&factory_exp2				},
	{	0,	"expm1",				NULL,		NULL,										1,		1,		&factory_expm1				},
	{	0,	"sqrt",					NULL,		NULL,										1,		1,		&factory_sqrt				},
	{	0,	"pow",					NULL,		NULL,										2,		2,		&factory_pow				},
	{	0,	"lcg_value",			NULL,		NULL,										0,		0,		&factory_lcg				},
	{	0,	"is_finite",			NULL,		NULL,										1,		1,		&factory_is_finite			},
	{	0,	"is_infinite",			NULL,		NULL,										1,		1,		&factory_is_infinite		},
	{	0,	"is_nan",				NULL,		NULL,										1,		1,		&factory_is_nan				},
	{	0,	"rand",					NULL,		qb_translate_intrinsic_rand,				0,		2,		&factory_rand				},
	{	0,	"mt_rand",				NULL,		qb_translate_intrinsic_rand,				0,		2,		&factory_mt_rand			},
	{	0,	"hypot",				NULL,		NULL,										2,		2,		&factory_hypot				},
	{	0,	"deg2rad",				NULL,		NULL,										1,		1,		&factory_deg2rad			},
	{	0,	"rad2deg",				NULL,		NULL,										1,		1,		&factory_rad2deg			},
	{	0,	"sign",					NULL,		NULL,										1,		1,		&factory_sign				},
	{	0,	"inv_sqrt",				NULL,		NULL,										1,		1,		&factory_inv_sqrt			},
	{	0,	"fract",				NULL,		NULL,										1,		1,		&factory_fract				},
	{	0,	"step",					NULL,		NULL,										2,		2,		&factory_step				},
	{	0,	"clamp",				NULL,		NULL,										3,		3,		&factory_clamp				},
	{	0,	"mix",					NULL,		NULL,										3,		3,		&factory_mix				},
	{	0,	"smooth_step",			NULL,		NULL,										3,		3,		&factory_smooth_step		},
	{	0,	"normalize",			NULL,		qb_translate_intrinsic_unary_vector_op,		1,		1,		&factory_normalize			},
	{	0,	"length",				NULL,		qb_translate_intrinsic_unary_vector_op,		1,		1,		&factory_length				},
	{	0,	"distance",				NULL,		qb_translate_intrinsic_binary_vector_op,	2,		2,		&factory_distance			},
	{	0,	"dot",					NULL,		qb_translate_intrinsic_binary_vector_op,	2,		2,		&factory_dot_product		},
	{	0,	"cross",				NULL,		qb_translate_intrinsic_cross,				2,		2,		&factory_cross_product		},
	{	0,	"faceforward",			NULL,		qb_translate_intrinsic_binary_vector_op,	2,		2,		&factory_faceforward		},
	{	0,	"reflect",				NULL,		qb_translate_intrinsic_binary_vector_op,	2,		2,		&factory_reflect			},
	{	0,	"refract",				NULL,		qb_translate_intrinsic_binary_vector_op,	3,		3,		&factory_refract			},
	{	0,	"mm_mult",				NULL,		qb_translate_intrinsic_mm_mult,				2,		2,		&factory_mm_multiply		},
	{	0,	"mv_mult",				NULL,		qb_translate_intrinsic_mv_mult,				2,		2,		&factory_mv_multiply		},
	{	0,	"vm_mult",				NULL,		qb_translate_intrinsic_vm_mult,				2,		2,		&factory_vm_multiply		},
	{	0,	"array_pop",			NULL,		qb_translate_intrinsic_array_pop,			1,		1,		NULL						},
	{	0,	"array_shift",			NULL,		qb_translate_intrinsic_array_shift,			1,		1,		NULL						},
	{	0,	"array_push",			NULL,		qb_translate_intrinsic_array_push,			2,		2,		NULL						},
	{	0,	"array_slice",			NULL,		qb_translate_intrinsic_array_slice,			2,		3,		NULL						},
	{	0,	"substr",				NULL,		qb_translate_intrinsic_array_slice,			2,		3,		NULL						},
	{	0,	"array_sum",			NULL,		qb_translate_intrinsic_array_aggregate,		1,		1,		&factory_array_sum			},
	{	0,	"array_product",		NULL,		qb_translate_intrinsic_array_aggregate,		1,		1,		&factory_array_product		},
	{	0,	"array_search",			NULL,		qb_translate_intrinsic_array_search,		2,		2,		&factory_array_search		},
	{	0,	"in_array",				NULL,		qb_translate_intrinsic_array_search,		2,		2,		&factory_in_array			},
	{	0,	"array_pos",			NULL,		qb_translate_intrinsic_subarray_search,		2,		3,		&factory_subarray_pos		},
	{	0,	"strpos",				NULL,		qb_translate_intrinsic_subarray_search,		2,		3,		&factory_subarray_pos		},
	{	0,	"array_rpos",			NULL,		qb_translate_intrinsic_subarray_search,		2,		3,		&factory_subarray_rpos		},
	{	0,	"strrpos",				NULL,		qb_translate_intrinsic_subarray_search,		2,		3,		&factory_subarray_rpos		},
	{	0,	"sort",					NULL,		qb_translate_intrinsic_sort,				1,		1,		&factory_sort				},
	{	0,	"rsort",				NULL,		qb_translate_intrinsic_sort,				1,		1,		&factory_rsort				},
	{	0,	"array_reverse",		NULL,		qb_translate_intrinsic_array_reverse,		1,		1,		&factory_array_reverse		},
	{	0,	"utf8_decode",			NULL,		qb_translate_intrinsic_utf8_decode,			1,		1,		&factory_utf8_decode		},
	{	0,	"utf8_encode",			NULL,		qb_translate_intrinsic_utf8_encode,			1,		1,		&factory_utf8_encode		},
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
