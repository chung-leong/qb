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

static void ZEND_FASTCALL qb_translate_intrinsic_math0(qb_compiler_context *cxt, qb_intrinsic_function *f, qb_operand *arguments, uint32_t argument_count, uint32_t result_flags) {
	qb_address *result_address;
	uint32_t *opcode_list = f->extra;
	uint32_t expr_type;

	expr_type = qb_get_result_type(cxt, QB_TYPE_F64);
	if(expr_type != QB_TYPE_F32) {
		expr_type = QB_TYPE_F64;
	}
	result_address = qb_push_temporary_variable(cxt, expr_type, NULL);
	if(!cxt->resolving_result_type) {
		uint32_t opcode = opcode_list[expr_type];
		qb_append_op_with_one_address(cxt, opcode, result_address);
	}
}

static uint32_t ZEND_FASTCALL qb_get_coercion_flags(qb_compiler_context *cxt, uint32_t *opcode_list) {
	// coerce to float if there're no integer version of the op
	if(!opcode_list[QB_TYPE_S32]) {
		if(!opcode_list[QB_TYPE_U32]) {
			// coerce to float if there're no integer version of the op, preferring double over single precision
			return QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE;
		} else {
			// coerce to unsigned
			return QB_COERCE_TO_UNSIGNED;
		}
	} else if(!opcode_list[QB_TYPE_U32]){
		// coerce to signed
		return QB_COERCE_TO_SIGNED;
	}
	return 0;
}

static void ZEND_FASTCALL qb_translate_intrinsic_math1(qb_compiler_context *cxt, qb_intrinsic_function *f, qb_operand *arguments, uint32_t argument_count, uint32_t result_flags) {
	qb_address *result_address, *result_size_address;
	uint32_t *opcode_list = f->extra;
	uint32_t operand_flags = qb_get_coercion_flags(cxt, opcode_list);
	uint32_t expr_type = qb_get_operand_type(cxt, &arguments[0], operand_flags);
	qb_do_type_coercion(cxt, &arguments[0], expr_type);
	result_size_address = arguments[0].address->array_size_address;
	if(!cxt->resolving_result_type) {
		uint32_t opcode = opcode_list[expr_type];
		if(arguments[0].address->flags & QB_ADDRESS_CONSTANT) {
			result_address = qb_push_new_constant(cxt, expr_type, result_size_address);
			qb_evaluate_op_with_two_addresses(cxt, opcode, arguments[0].address, result_address);
		} else {
			result_address = qb_push_temporary_variable(cxt, expr_type, result_size_address);
			qb_append_op_with_two_addresses(cxt, opcode, arguments[0].address, result_address);
		}
	} else {
		result_address = qb_push_temporary_variable(cxt, expr_type, result_size_address);
	}
}

static void ZEND_FASTCALL qb_translate_intrinsic_math1_bool(qb_compiler_context *cxt, qb_intrinsic_function *f, qb_operand *arguments, uint32_t argument_count, uint32_t result_flags) {
	qb_address *result_address, *result_size_address;
	qb_address *address;
	uint32_t arg_type = qb_get_operand_type(cxt, &arguments[0], QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE);
	uint32_t expr_type = QB_TYPE_I32;
	qb_do_type_coercion(cxt, &arguments[0], arg_type);
	address = arguments[0].address;
	result_size_address = address->array_size_address;
	if(!cxt->resolving_result_type) {
		uint32_t *opcode_list = f->extra, opcode = opcode_list[address->type];
		if(address->flags & QB_ADDRESS_CONSTANT) {
			result_address = qb_push_new_constant(cxt, expr_type, result_size_address);
			qb_evaluate_op_with_two_addresses(cxt, opcode, address, result_address);
		} else {
			result_address = qb_push_temporary_variable(cxt, expr_type, result_size_address);
			qb_append_op_with_two_addresses(cxt, opcode, address, result_address);
		}
	} else {
		result_address = qb_push_temporary_variable(cxt, expr_type, result_size_address);
	}
}

static void ZEND_FASTCALL qb_translate_intrinsic_math2(qb_compiler_context *cxt, qb_intrinsic_function *f, qb_operand *arguments, uint32_t argument_count, uint32_t result_flags) {
	qb_address *result_address, *result_size_address;
	qb_address *address1, *address2;
	uint32_t *opcode_list = f->extra;
	uint32_t operand_flags = qb_get_coercion_flags(cxt, opcode_list);
	uint32_t expr_type = qb_do_type_promotion(cxt, &arguments[0], &arguments[1], operand_flags);
	address1 = arguments[0].address;
	address2 = arguments[1].address;
	result_size_address = qb_get_larger_array_size(cxt, address1, address2);
	if(!cxt->resolving_result_type) {
		uint32_t opcode = opcode_list[expr_type];
		if((address1->flags & QB_ADDRESS_CONSTANT) && (address2->flags & QB_ADDRESS_CONSTANT)) {
			result_address = qb_push_new_constant(cxt, expr_type, result_size_address);
			qb_evaluate_op_with_three_addresses(cxt, opcode, address1, address2, result_address);
		} else {
			result_address = qb_push_temporary_variable(cxt, expr_type, result_size_address);
			qb_append_op_with_three_addresses(cxt, opcode, address1, address2, result_address);
		}
	} else {
		result_address = qb_push_temporary_variable(cxt, expr_type, result_size_address);
	}
}

static void ZEND_FASTCALL qb_translate_intrinsic_math3(qb_compiler_context *cxt, qb_intrinsic_function *f, qb_operand *arguments, uint32_t argument_count, uint32_t result_flags) {
	qb_address *result_address, *result_size_address;
	qb_address *address1, *address2, *address3;
	uint32_t *opcode_list = f->extra;
	uint32_t operand_flags = qb_get_coercion_flags(cxt, opcode_list);
	uint32_t expr_type = qb_do_type_promotion(cxt, &arguments[0], &arguments[1], operand_flags);
	uint32_t i;
	for(i = 0; i < 3; i++) {
		qb_do_type_coercion(cxt, &arguments[i], expr_type);
	}
	address1 = arguments[0].address;
	address2 = arguments[1].address;
	address3 = arguments[2].address;
	result_size_address = qb_get_largest_array_size(cxt, 3, address1, address2, address3);
	if(!cxt->resolving_result_type) {
		uint32_t opcode = opcode_list[expr_type];
		if((address1->flags & QB_ADDRESS_CONSTANT) && (address2->flags & QB_ADDRESS_CONSTANT) && (address3->flags & QB_ADDRESS_CONSTANT)) {
			result_address = qb_push_new_constant(cxt, expr_type, result_size_address);
			qb_evaluate_op_with_four_addresses(cxt, opcode, address1, address2, address3, result_address);
		} else {
			result_address = qb_push_temporary_variable(cxt, expr_type, result_size_address);
			qb_append_op_with_four_addresses(cxt, opcode, address1, address2, address3, result_address);
		}
	} else {
		result_address = qb_push_temporary_variable(cxt, expr_type, result_size_address);
	}
}

static void ZEND_FASTCALL qb_translate_intrinsic_mmult(qb_compiler_context *cxt, qb_intrinsic_function *f, qb_operand *arguments, uint32_t argument_count, uint32_t result_flags) {
	qb_address *result_address, *result_size_address, *result_row_address, *result_col_address;
	qb_address *address1, *address2;
	uint32_t operand_flags = QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE;
	uint32_t expr_type = qb_do_type_promotion(cxt, &arguments[0], &arguments[1], operand_flags);
	address1 = arguments[0].address;
	address2 = arguments[1].address;
	if(address1->dimension_count != 1 && address1->dimension_count != 2) {
		qb_abort("%s() expects parameter 1 to be a one or two-dimensional array", f->name);
	}
	if(address2->dimension_count != 1 && address2->dimension_count != 2) {
		qb_abort("%s() expects parameter 2 to be a one or two-dimensional array", f->name);
	}
	if(address1->dimension_count == 1 && address2->dimension_count == 1) {
		qb_abort("%s() expects at least one parameter to be a two-dimensional array", f->name);
	}
	result_row_address = address1->dimension_addresses[0];
	if(address2->dimension_count == 2) {
		result_col_address = address2->dimension_addresses[1];
	} else {
		// the result will be one-dimensional
		result_col_address = NULL;
	}
	if(!result_col_address) {
		result_size_address = result_row_address;
	} else {
		if(!(result_row_address->flags & QB_ADDRESS_CONSTANT) && (result_col_address->flags & QB_ADDRESS_CONSTANT)) {
			uint32_t row = VALUE(U32, result_row_address);
			uint32_t col = VALUE(U32, result_col_address);
			result_size_address = qb_obtain_constant_U32(cxt, row * col);
		} else {
			result_size_address = qb_obtain_temporary_scalar(cxt, QB_TYPE_U32);
			qb_add_reference(cxt, result_size_address);
		}
	}
	if(!cxt->resolving_result_type) {
		uint32_t opcode, address_count;
		qb_address *addresses[7];
		result_address = qb_push_temporary_variable(cxt, expr_type, result_size_address);
		if(address1->dimension_count == 2 && address2->dimension_count == 1) {
			opcode = mv_multiply_opcodes[expr_type];
			addresses[0] = address1;
			addresses[1] = address1->dimension_addresses[0];
			addresses[2] = address1->dimension_addresses[1];
			addresses[3] = address2;
			addresses[4] = result_address;
			address_count = 5;
		} else if(address1->dimension_count == 1 && address2->dimension_count == 2) {
			opcode = vm_multiply_opcodes[expr_type];
			addresses[0] = address1;
			addresses[1] = address2;
			addresses[2] = address2->dimension_addresses[0];
			addresses[3] = address2->dimension_addresses[1];
			addresses[4] = result_address;
			address_count = 5;
		} else {
			opcode = mm_multiply_opcodes[expr_type];
			addresses[0] = address1;
			addresses[1] = address1->dimension_addresses[0];
			addresses[2] = address1->dimension_addresses[1];
			addresses[3] = address2;
			addresses[4] = address2->dimension_addresses[0];
			addresses[5] = address2->dimension_addresses[1];
			addresses[6] = result_address;
			address_count = 7;
		}
		qb_append_op_with_multiple_addresses(cxt, opcode, addresses, address_count);
		if(result_col_address) {
			// add dimensional info
			result_address->dimension_count = 2;
			result_address->dimension_addresses = qb_allocate_pointers(cxt->pool, 2);
			result_address->array_size_addresses = qb_allocate_pointers(cxt->pool, 2);
			result_address->dimension_addresses[0] = result_row_address;
			result_address->dimension_addresses[1] = result_col_address;
			result_address->array_size_addresses[1] = result_col_address;
			result_address->array_size_addresses[0] = result_address->array_size_address;
		}
	} else {
		result_address = qb_push_temporary_variable(cxt, expr_type, result_size_address);
	}
}

static void ZEND_FASTCALL qb_translate_intrinsic_count(qb_compiler_context *cxt, qb_intrinsic_function *f, qb_operand *arguments, uint32_t argument_count, uint32_t result_flags) {
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
	qb_push_address(cxt, count_address);
}

static void ZEND_FASTCALL qb_translate_intrinsic_rand(qb_compiler_context *cxt, qb_intrinsic_function *f, qb_operand *arguments, uint32_t argument_count, uint32_t result_flags) {
	qb_address *result_address, *result_size_address;
	qb_address *min_address, *max_address;
	uint32_t expr_type;

	if(argument_count != 0 && argument_count != 2) {
		qb_abort("%s() expects either 0 or 2 arguments", f->name);
	}

	if(argument_count == 0) {
		expr_type = qb_get_result_type(cxt, QB_TYPE_I32);
		if(expr_type == QB_TYPE_F32 || expr_type == QB_TYPE_F64) {
			expr_type = QB_TYPE_I32;
		}
		min_address = qb_obtain_constant(cxt, 0, expr_type);
		max_address = qb_obtain_constant(cxt, (int64_t) integer_upper_bounds[expr_type], expr_type);
		result_size_address = NULL;
	} else {
		qb_do_type_promotion(cxt, &arguments[0], &arguments[1], QB_COERCE_TO_INTEGER);
		expr_type = arguments[0].address->type;
		min_address = arguments[0].address;
		max_address = arguments[1].address;
		result_size_address = qb_get_larger_array_size(cxt, arguments[0].address, arguments[1].address);
	}

	result_address = qb_push_temporary_variable(cxt, expr_type, result_size_address);

	if(!cxt->resolving_result_type) {
		uint32_t *opcode_list = f->extra, opcode = opcode_list[expr_type];
		qb_append_op_with_three_addresses(cxt, opcode, min_address, max_address, result_address);
	}
}

static void ZEND_FASTCALL qb_translate_intrinsic_round(qb_compiler_context *cxt, qb_intrinsic_function *f, qb_operand *arguments, uint32_t argument_count, uint32_t result_flags) {
	qb_operand *value = &arguments[0], *precision = &arguments[1], *mode = &arguments[2];
	qb_address *result_address, *result_size_address;
	qb_address *precision_address, *mode_address, *address;
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
	expr_type = address->type;
	result_size_address = address->array_size_address;
	result_address = qb_push_temporary_variable(cxt, expr_type, result_size_address);
	if(!cxt->resolving_result_type) {
		uint32_t *opcode_list = f->extra, opcode = opcode_list[expr_type];
		qb_append_op_with_four_addresses(cxt, opcode, address, precision_address, mode_address, result_address);
	}
}

static void ZEND_FASTCALL qb_translate_array_push(qb_compiler_context *cxt, qb_intrinsic_function *f, qb_operand *arguments, uint32_t argument_count, uint32_t result_flags) {
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
	if(result_flags & (QB_RESULT_EXPECT_TEMPORARY | QB_RESULT_EXPECT_VARIABLE)) {
		qb_push_address(cxt, size_address);
	}
}

static void ZEND_FASTCALL qb_translate_array_pop(qb_compiler_context *cxt, qb_intrinsic_function *f, qb_operand *arguments, uint32_t argument_count, uint32_t result_flags) {
	qb_operand *container = &arguments[0];
	qb_address *size_address, *index_address, *one_address, *variable_address, *result_address;
	if(container->type != QB_OPERAND_ADDRESS || IS_SCALAR(container->address)) {
		qb_abort("%s expects an array as parameter", f->name);
	}
	if(!IS_EXPANDABLE_ARRAY(container->address)) {
		qb_abort("Removing element to from an array that cannot shrink");
	}
	size_address = container->address->dimension_addresses[0];
	index_address = qb_obtain_temporary_variable(cxt, QB_TYPE_U32, NULL);
	one_address = qb_obtain_constant_U32(cxt, 1);
	qb_append_op_with_three_addresses(cxt, subtract_opcodes[QB_TYPE_U32], size_address, one_address, index_address);
	variable_address = qb_get_array_element(cxt, container->address, index_address);
	result_address = qb_push_temporary_variable(cxt, variable_address->type, variable_address->array_size_address);
	qb_do_assignment(cxt, variable_address, result_address);
	qb_do_unset(cxt, variable_address);
}

static void ZEND_FASTCALL qb_translate_array_shift(qb_compiler_context *cxt, qb_intrinsic_function *f, qb_operand *arguments, uint32_t argument_count, uint32_t result_flags) {
	qb_operand *container = &arguments[0];
	qb_address *zero_address, *variable_address, *result_address;
	if(container->type != QB_OPERAND_ADDRESS || IS_SCALAR(container->address)) {
		qb_abort("%s expects an array as parameter", f->name);
	}
	if(!IS_EXPANDABLE_ARRAY(container->address)) {
		qb_abort("Removing element to from an array that cannot shrink");
	}
	zero_address = qb_obtain_constant_U32(cxt, 0);
	variable_address = qb_get_array_element(cxt, container->address, zero_address);
	result_address = qb_push_temporary_variable(cxt, variable_address->type, variable_address->array_size_address);
	qb_do_assignment(cxt, variable_address, result_address);
	qb_do_unset(cxt, variable_address);
}

static void ZEND_FASTCALL qb_translate_array_slice(qb_compiler_context *cxt, qb_intrinsic_function *f, qb_operand *arguments, uint32_t argument_count, uint32_t result_flags) {
	qb_operand *container = &arguments[0], *offset = &arguments[1], *length = &arguments[2];
	qb_address *slice_address;
	qb_do_type_coercion(cxt, container, QB_TYPE_ANY);
	if(IS_SCALAR(container->address)) {
		qb_abort("%s expects an array as parameter", f->name);
	}
	qb_do_type_coercion(cxt, offset, QB_TYPE_U32);
	if(argument_count >= 3) {
		qb_do_type_coercion(cxt, length, QB_TYPE_U32);
	}
	slice_address = qb_get_array_slice(cxt, container->address, offset->address, (argument_count >= 3) ? length->address : NULL);
	qb_push_address(cxt, slice_address);
}

static void ZEND_FASTCALL qb_translate_array_aggregate(qb_compiler_context *cxt, qb_intrinsic_function *f, qb_operand *arguments, uint32_t argument_count, uint32_t result_flags) {
	qb_operand *container = &arguments[0];
	qb_address *result_address;
	uint32_t expr_type;

	qb_do_type_coercion(cxt, container, QB_TYPE_ANY);
	expr_type = container->address->type;
	if(IS_SCALAR(container->address)) {
		qb_abort("%s expects an array as parameter", f->name);
	}
	result_address = qb_push_temporary_variable(cxt, expr_type, NULL);
	if(!cxt->resolving_result_type) {
		uint32_t *opcode_list = f->extra, opcode = opcode_list[expr_type];
		qb_append_op_with_two_addresses(cxt, opcode, container->address, result_address);
	}
}

static void ZEND_FASTCALL qb_translate_array_search(qb_compiler_context *cxt, qb_intrinsic_function *f, qb_operand *arguments, uint32_t argument_count, uint32_t result_flags) {
	qb_operand *container = &arguments[0];
	qb_operand *needle = &arguments[1];
	qb_address *result_address;
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
	result_address = qb_push_temporary_variable(cxt, QB_TYPE_S32, NULL);
	if(!cxt->resolving_result_type) {
		uint32_t *opcode_list = f->extra, opcode = opcode_list[expr_type];
		qb_op *qop = qb_append_op_with_three_addresses(cxt, opcode, container->address, needle->address, result_address);
		qop->operands[0].type = QB_OPERAND_ADDRESS_ARR;
		qop->operands[2].type = QB_OPERAND_ADDRESS_VAR;
	}
}

static void ZEND_FASTCALL qb_translate_subarray_search(qb_compiler_context *cxt, qb_intrinsic_function *f, qb_operand *arguments, uint32_t argument_count, uint32_t result_flags) {
	qb_operand *container = &arguments[0];
	qb_operand *needle = &arguments[1];
	qb_operand *offset = &arguments[2];
	qb_address *result_address, *offset_address;
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
			qb_do_copy(cxt, offset_address, offset->address);
		} else {
			offset_address = offset->address;
		}
	} else {
		offset_address = qb_obtain_constant_S32(cxt, 0);
	}
	result_address = qb_push_temporary_variable(cxt, QB_TYPE_S32, NULL);
	if(!cxt->resolving_result_type) {
		uint32_t *opcode_list = f->extra, opcode = opcode_list[expr_type];
		qb_op *qop = qb_append_op_with_four_addresses(cxt, opcode, container->address, needle->address, offset_address, result_address);
		qop->operands[0].type = QB_OPERAND_ADDRESS_ARR;
		qop->operands[1].type = QB_OPERAND_ADDRESS_ARR;
		qop->operands[2].type = QB_OPERAND_ADDRESS_VAR;
		qop->operands[3].type = QB_OPERAND_ADDRESS_VAR;
	}
}

static void ZEND_FASTCALL qb_translate_sort(qb_compiler_context *cxt, qb_intrinsic_function *f, qb_operand *arguments, uint32_t argument_count, uint32_t result_flags) {
	qb_operand *container = &arguments[0];
	uint32_t *opcodes = f->extra;
	if(container->type != QB_OPERAND_ADDRESS || IS_SCALAR(container->address) || container->address->dimension_count > 1) {
		qb_abort("%s expects a one-dimensional array as parameter", f->name);
	}
	if(!cxt->resolving_result_type) {
		qb_append_op_with_one_address(cxt, opcodes[container->address->type], container->address);
	}
	if(result_flags) {
		qb_address *one_address = qb_obtain_constant_S32(cxt, 1);
		qb_push_address(cxt, one_address);
	}
}

static void ZEND_FASTCALL qb_translate_utf8_decode(qb_compiler_context *cxt, qb_intrinsic_function *f, qb_operand *arguments, uint32_t argument_count, uint32_t result_flags) {
	qb_operand *source = &arguments[0];
	qb_address *result_address;
	uint32_t result_type = qb_get_result_type(cxt, QB_TYPE_U16);
	qb_do_type_coercion(cxt, source, QB_TYPE_U08);
	if((result_type & ~QB_TYPE_UNSIGNED) != QB_TYPE_I16 && (result_type & ~QB_TYPE_UNSIGNED) != QB_TYPE_I32) {
		result_type = QB_TYPE_U16;
	}
	result_address = qb_obtain_temporary_variable_length_array(cxt, result_type);
	qb_push_address(cxt, result_address);
	qb_do_unset(cxt, result_address);
	if(!cxt->resolving_result_type) {
		uint32_t opcode = utf8_decode_opcodes[result_type];
		qb_append_op_with_two_addresses(cxt, opcode, source->address, result_address);
	}
}

static void ZEND_FASTCALL qb_translate_utf8_encode(qb_compiler_context *cxt, qb_intrinsic_function *f, qb_operand *arguments, uint32_t argument_count, uint32_t result_flags) {
	qb_operand *source = &arguments[0];
	qb_address *result_address;
	uint32_t source_type = QB_TYPE_U16;
	if(source->type == QB_OPERAND_ADDRESS && (source->address->type & ~QB_TYPE_UNSIGNED) == QB_TYPE_I32) {
		source_type = QB_TYPE_U32;
	}
	qb_do_type_coercion(cxt, source, source_type);
	result_address = qb_obtain_temporary_variable_length_array(cxt, QB_TYPE_I08);
	result_address->flags |= QB_ADDRESS_STRING;
	qb_push_address(cxt, result_address);
	qb_do_unset(cxt, result_address);
	if(!cxt->resolving_result_type) {
		uint32_t opcode = utf8_encode_opcodes[source->address->type];
		qb_append_op_with_two_addresses(cxt, opcode, source->address, result_address);
	}
}

static void ZEND_FASTCALL qb_translate_intrinsic_pack(qb_compiler_context *cxt, qb_intrinsic_function *f, qb_operand *arguments, uint32_t argument_count, uint32_t result_flags) {
	qb_operand *value = &arguments[0], *type = &arguments[1];
	qb_address *result_address, *result_size_address;
	uint32_t *opcode_list = f->extra;
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
	result_size_address = qb_obtain_constant_U32(cxt, type_sizes[value->address->type]);
	result_address = qb_push_temporary_variable(cxt, QB_TYPE_U08, result_size_address);
	result_address->flags |= QB_ADDRESS_STRING;
	if(!cxt->resolving_result_type) {
		uint32_t opcode = opcode_list[value->address->type];
		qb_op *qop = qb_append_op_with_two_addresses(cxt, opcode, value->address, result_address);
		qop->operands[1].type = QB_OPERAND_ADDRESS_ARR;
	}
}

static void ZEND_FASTCALL qb_translate_intrinsic_unpack(qb_compiler_context *cxt, qb_intrinsic_function *f, qb_operand *arguments, uint32_t argument_count, uint32_t result_flags) {
	qb_operand *array = &arguments[0], *offset = &arguments[1], *type = &arguments[2];
	qb_address *result_address,	*slice_address, *offset_address, *length_address;
	uint32_t *opcode_list = f->extra;
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
		result_type = qb_get_result_type(cxt, QB_TYPE_VOID);
		if(result_type == QB_TYPE_VOID) {
			qb_abort("%s() requires the third parameter when the desired type cannot be determined from context", f->name);
		}
	}
	length_address = qb_obtain_constant_U32(cxt, type_sizes[result_type]);
	slice_address = qb_get_array_slice(cxt, array->address, offset_address, length_address);
	result_address = qb_push_temporary_variable(cxt, result_type, NULL);
	if(!cxt->resolving_result_type) {
		uint32_t opcode = opcode_list[result_type];
		qb_op *qop = qb_append_op_with_two_addresses(cxt, opcode, slice_address, result_address);
		// the second operand will determine the address mode
		qop->operands[0].type = QB_OPERAND_ADDRESS_ARR;
	}
}

static void ZEND_FASTCALL qb_translate_intrinsic_comparison(qb_compiler_context *cxt, qb_intrinsic_function *f, qb_operand *arguments, uint32_t argument_count, uint32_t result_flags) {
	qb_operand *array1 = &arguments[0], *array2 = &arguments[1];
	uint32_t *opcode_list = f->extra;
	qb_address *result_address, *result_size_address;
	qb_do_type_promotion(cxt, array1, array2, 0);	
	result_size_address = qb_get_larger_array_size(cxt, array1->address, array2->address);
	result_address = qb_push_temporary_variable(cxt, QB_TYPE_I32, result_size_address);
	if(!cxt->resolving_result_type) {
		// the set comparison opcodes are placed after the regular ones
		uint32_t opcode = opcode_list[array1->address->type + QB_TYPE_COUNT];
		qb_append_op_with_three_addresses(cxt, opcode, array1->address, array2->address, result_address);
	}
}

static void ZEND_FASTCALL qb_translate_intrinsic_any_all(qb_compiler_context *cxt, qb_intrinsic_function *f, qb_operand *arguments, uint32_t argument_count, uint32_t result_flags) {
	qb_operand *set = &arguments[0];
	uint32_t opcode = *((uint32_t *) f->extra);
	qb_address *result_address;
	qb_do_type_coercion(cxt, set, QB_TYPE_I32);
	result_address = qb_push_temporary_variable(cxt, QB_TYPE_I32, NULL);
	if(!cxt->resolving_result_type) {
		qb_op *qop = qb_append_op_with_two_addresses(cxt, opcode, set->address, result_address);
		// the second operand will determine the address mode
		qop->operands[0].type = QB_OPERAND_ADDRESS_ARR;
	}
}

static void ZEND_FASTCALL qb_translate_intrinsic_not(qb_compiler_context *cxt, qb_intrinsic_function *f, qb_operand *arguments, uint32_t argument_count, uint32_t result_flags) {
	qb_operand *set = &arguments[0];
	uint32_t opcode = *((uint32_t *) f->extra);
	qb_address *result_address, *result_size_address;
	qb_do_type_coercion(cxt, set, QB_TYPE_I32);
	result_size_address = set->address->array_size_address;
	result_address = qb_push_temporary_variable(cxt, QB_TYPE_I32, result_size_address);
	if(!cxt->resolving_result_type) {
		qb_append_op_with_two_addresses(cxt, opcode, set->address, result_address);
	}
}

static void ZEND_FASTCALL qb_translate_intrinsic_comparison_swap(qb_compiler_context *cxt, qb_intrinsic_function *f, qb_operand *arguments, uint32_t argument_count, uint32_t result_flags) {
	qb_operand swapped_arguments[2];
	swapped_arguments[0] = arguments[1];
	swapped_arguments[1] = arguments[0];
	qb_translate_intrinsic_comparison(cxt, f, swapped_arguments, 2, result_flags);
}

static void ZEND_FASTCALL qb_translate_intrinsic_minmax(qb_compiler_context *cxt, qb_intrinsic_function *f, qb_operand *arguments, uint32_t argument_count, uint32_t result_flags) {
	if(argument_count == 1) {
		qb_operand *container = &arguments[0];
		qb_address *result_address;
		uint32_t expr_type;

		qb_do_type_coercion(cxt, container, QB_TYPE_ANY);
		expr_type = container->address->type;
		if(IS_SCALAR(container->address)) {
			qb_abort("%s expects an array as parameter when only one parameter is given", f->name);
		}
		result_address = qb_push_temporary_variable(cxt, expr_type, NULL);
		if(!cxt->resolving_result_type) {
			uint32_t *opcode_list = f->extra, opcode = opcode_list[expr_type + QB_TYPE_COUNT];
			qb_append_op_with_two_addresses(cxt, opcode, container->address, result_address);
		}
	} else {
		qb_address *result_address = NULL;
		uint32_t expr_type = qb_get_highest_rank_type(cxt, arguments, argument_count, 0);
		uint32_t *opcode_list = f->extra, opcode = opcode_list[expr_type];
		uint32_t i = 0; 

		for(i = 0; i < argument_count; i++) {
			qb_do_type_coercion(cxt, &arguments[i], expr_type);
			// put a reference on any temporary variable so it won't be reused 
			qb_add_reference(cxt, arguments[i].address);
		}
		result_address = qb_push_temporary_variable(cxt, expr_type, NULL);
		if(!cxt->resolving_result_type) {
			qb_append_op_with_three_addresses(cxt, opcode, arguments[0].address, arguments[1].address, result_address);
			for(i = 2; i < argument_count; i++) {
				qb_append_op_with_three_addresses(cxt, opcode, result_address, arguments[i].address, result_address);
			}
		}
		for(i = 0; i < argument_count; i++) {
			// now temporary variables from casting can be freed
			qb_remove_reference(cxt, arguments[i].address);
		}
	}
}

static void ZEND_FASTCALL qb_translate_intrinsic_cast(qb_compiler_context *cxt, qb_intrinsic_function *f, qb_operand *arguments, uint32_t argument_count, uint32_t result_flags) {
	qb_operand *expr = &arguments[0];
	qb_address *result_address;
	uint32_t type = (uintptr_t) f->extra;

	if(argument_count == 2) {
		qb_operand *hi_dword = &arguments[0], *lo_dword = &arguments[1];
		qb_do_type_coercion(cxt, hi_dword, QB_TYPE_U32);
		qb_do_type_coercion(cxt, lo_dword, QB_TYPE_U32);
		if(!cxt->resolving_result_type) {
			if(hi_dword->address->flags & QB_ADDRESS_CONSTANT && lo_dword->address->flags & QB_ADDRESS_CONSTANT) {
				uint64_t value = ((uint64_t) VALUE(U32, hi_dword->address)) << 32 | ((uint64_t) VALUE(U32, lo_dword->address));
				if(type == QB_TYPE_S64) {
					result_address = qb_obtain_constant_S64(cxt, (int64_t) value);
				} else {
					result_address = qb_obtain_constant_U64(cxt, value);
				}
			} else {
				qb_abort("%s() expects two constant scalar expressions when called with two parameters", f->name);
			}
		} else {
			// just put some there
			qb_do_type_coercion(cxt, expr, type);
			result_address = expr->address;
		}
	} else {
		qb_do_type_coercion(cxt, expr, type);
		result_address = expr->address;
	}
	qb_push_address(cxt, result_address);
}

static void ZEND_FASTCALL qb_translate_intrinsic_defined(qb_compiler_context *cxt, qb_intrinsic_function *f, qb_operand *arguments, uint32_t argument_count, uint32_t result_flags) {
	USE_TSRM
	qb_operand *name = &arguments[0];
	qb_address *result_address;
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
			result_address = qb_obtain_constant_S32(cxt, 1);
		} else {
			result_address = qb_obtain_constant_S32(cxt, 0);
		}
		qb_push_address(cxt, result_address);
	} else {
		qb_abort("%s() expects a constant string", f->name);
	}
}

static void ZEND_FASTCALL qb_translate_intrinsic_define(qb_compiler_context *cxt, qb_intrinsic_function *f, qb_operand *arguments, uint32_t argument_count, uint32_t result_flags) {
	USE_TSRM
	qb_operand *name = &arguments[0];
	qb_operand *expr = &arguments[1];
	qb_address *result_address;
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
			result_address = qb_obtain_constant_S32(cxt, 1);
		} else {
			result_address = qb_obtain_constant_S32(cxt, 0);
		}
		qb_push_address(cxt, result_address);
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
	{	0,	"pack_le",				NULL,		qb_translate_intrinsic_pack,				1,		2,		pack_le_opcodes				},
	{	0,	"unpack_le",			NULL,		qb_translate_intrinsic_unpack,				1,		3,		unpack_le_opcodes			},
	{	0,	"pack_be",				NULL,		qb_translate_intrinsic_pack,				1,		2,		pack_be_opcodes				},
	{	0,	"unpack_be",			NULL,		qb_translate_intrinsic_unpack,				1,		3,		unpack_be_opcodes			},
	{	0,	"equal",				NULL,		qb_translate_intrinsic_comparison,			2,		2,		equal_opcodes				},
	{	0,	"not_equal",			NULL,		qb_translate_intrinsic_comparison,			2,		2,		not_equal_opcodes			},
	{	0,	"less_than",			NULL,		qb_translate_intrinsic_comparison,			2,		2,		less_than_opcodes			},
	{	0,	"less_than_equal",		NULL,		qb_translate_intrinsic_comparison,			2,		2,		less_or_equal_opcodes		},
	{	0,	"greater_than",			NULL,		qb_translate_intrinsic_comparison_swap,	2,		2,		less_than_opcodes			},
	{	0,	"greater_than_equal",	NULL,		qb_translate_intrinsic_comparison_swap,	2,		2,		less_or_equal_opcodes		},
	{	0,	"min",					NULL,		qb_translate_intrinsic_minmax,				1,		256,	min_opcodes					},
	{	0,	"max",					NULL,		qb_translate_intrinsic_minmax,				1,		256,	max_opcodes					},
	{	0,	"any",					NULL,		qb_translate_intrinsic_any_all,			1,		1,		(void *) &any_opcode		},
	{	0,	"all",					NULL,		qb_translate_intrinsic_any_all,			1,		1,		(void *) &all_opcode		},
	{	0,	"not",					NULL,		qb_translate_intrinsic_not,				1,		1,		(void *) &not_opcode		},
	{	0,	"abs",					NULL,		qb_translate_intrinsic_math1,				1,		1,		abs_opcodes					},
	{	0,	"sin",					NULL,		qb_translate_intrinsic_math1,				1,		1,		sin_opcodes					},
	{	0,	"asin",					NULL,		qb_translate_intrinsic_math1,				1,		1,		asin_opcodes				},
	{	0,	"cos",					NULL,		qb_translate_intrinsic_math1,				1,		1,		cos_opcodes					},
	{	0,	"acos",					NULL,		qb_translate_intrinsic_math1,				1,		1,		acos_opcodes				},
	{	0,	"tan",					NULL,		qb_translate_intrinsic_math1,				1,		1,		tan_opcodes					},
	{	0,	"atan",					NULL,		qb_translate_intrinsic_math1,				1,		1,		atan_opcodes				},
	{	0,	"sinh",					NULL,		qb_translate_intrinsic_math1,				1,		1,		sinh_opcodes				},
	{	0,	"asinh",				NULL,		qb_translate_intrinsic_math1,				1,		1,		asinh_opcodes				},
	{	0,	"cosh",					NULL,		qb_translate_intrinsic_math1,				1,		1,		cosh_opcodes				},
	{	0,	"acosh",				NULL,		qb_translate_intrinsic_math1,				1,		1,		acosh_opcodes				},
	{	0,	"tanh",					NULL,		qb_translate_intrinsic_math1,				1,		1,		tanh_opcodes				},
	{	0,	"atanh",				NULL,		qb_translate_intrinsic_math1,				1,		1,		atanh_opcodes				},
	{	0,	"ceil",					NULL,		qb_translate_intrinsic_math1,				1,		1,		ceil_opcodes				},
	{	0,	"floor",				NULL,		qb_translate_intrinsic_math1,				1,		1,		floor_opcodes				},
	{	0,	"round",				NULL,		qb_translate_intrinsic_round,				1,		3,		round_opcodes				},
	{	0,	"log",					NULL,		qb_translate_intrinsic_math1,				1,		1,		log_opcodes					},
	{	0,	"log1p",				NULL,		qb_translate_intrinsic_math1,				1,		1,		log1p_opcodes				},
	{	0,	"log10",				NULL,		qb_translate_intrinsic_math1,				1,		1,		log10_opcodes				},
	{	0,	"log2",					NULL,		qb_translate_intrinsic_math1,				1,		1,		log2_opcodes				},
	{	0,	"exp",					NULL,		qb_translate_intrinsic_math1,				1,		1,		exp_opcodes					},
	{	0,	"exp2",					NULL,		qb_translate_intrinsic_math1,				1,		1,		exp2_opcodes				},
	{	0,	"expm1",				NULL,		qb_translate_intrinsic_math1,				1,		1,		expm1_opcodes				},
	{	0,	"sqrt",					NULL,		qb_translate_intrinsic_math1,				1,		1,		sqrt_opcodes				},
	{	0,	"pow",					NULL,		qb_translate_intrinsic_math2,				2,		2,		pow_opcodes					},
	{	0,	"lcg_value",			NULL,		qb_translate_intrinsic_math0,				0,		0,		lcg_opcodes					},
	{	0,	"is_finite",			NULL,		qb_translate_intrinsic_math1_bool,			1,		1,		is_finite_opcodes			},
	{	0,	"is_infinite",			NULL,		qb_translate_intrinsic_math1_bool,			1,		1,		is_infinite_opcodes			},
	{	0,	"is_nan",				NULL,		qb_translate_intrinsic_math1_bool,			1,		1,		is_nan_opcodes				},
	{	0,	"rand",					NULL,		qb_translate_intrinsic_rand,				0,		2,		rand_opcodes				},
	{	0,	"mt_rand",				NULL,		qb_translate_intrinsic_rand,				0,		2,		mt_rand_opcodes				},
	{	0,	"hypot",				NULL,		qb_translate_intrinsic_math2,				2,		2,		hypot_opcodes				},
	{	0,	"deg2rad",				NULL,		qb_translate_intrinsic_math1,				1,		1,		deg2rad_opcodes				},
	{	0,	"rad2deg",				NULL,		qb_translate_intrinsic_math1,				1,		1,		rad2deg_opcodes				},
	{	0,	"sign",					NULL,		qb_translate_intrinsic_math1,				1,		1,		sign_opcodes				},
	{	0,	"inv_sqrt",				NULL,		qb_translate_intrinsic_math1,				1,		1,		inv_sqrt_opcodes			},
	{	0,	"fract",				NULL,		qb_translate_intrinsic_math1,				1,		1,		fract_opcodes				},
	{	0,	"step",					NULL,		qb_translate_intrinsic_math2,				2,		2,		step_opcodes				},
	{	0,	"clamp",				NULL,		qb_translate_intrinsic_math3,				3,		3,		clamp_opcodes				},
	{	0,	"mix",					NULL,		qb_translate_intrinsic_math3,				3,		3,		mix_opcodes					},
	{	0,	"smooth_step",			NULL,		qb_translate_intrinsic_math3,				3,		3,		smooth_step_opcodes			},
	{	0,	"mmult",				NULL,		qb_translate_intrinsic_mmult,				2,		2,		NULL						},
	{	0,	"array_pop",			NULL,		qb_translate_array_pop,					1,		1,		NULL						},
	{	0,	"array_shift",			NULL,		qb_translate_array_shift,				1,		1,		NULL						},
	{	0,	"array_push",			NULL,		qb_translate_array_push,				2,		2,		NULL						},
	{	0,	"array_slice",			NULL,		qb_translate_array_slice,				2,		3,		NULL						},
	{	0,	"substr",				NULL,		qb_translate_array_slice,				2,		3,		NULL						},
	{	0,	"array_sum",			NULL,		qb_translate_array_aggregate,			1,		1,		array_sum_opcodes			},
	{	0,	"array_product",		NULL,		qb_translate_array_aggregate,			1,		1,		array_product_opcodes		},
	{	0,	"array_search",			NULL,		qb_translate_array_search,				2,		2,		array_search_opcodes		},
	{	0,	"in_array",				NULL,		qb_translate_array_search,				2,		2,		in_array_opcodes			},
	{	0,	"array_pos",			NULL,		qb_translate_subarray_search,			2,		3,		subarray_pos_opcodes		},
	{	0,	"strpos",				NULL,		qb_translate_subarray_search,			2,		3,		subarray_pos_opcodes		},
	{	0,	"array_rpos",			NULL,		qb_translate_subarray_search,			2,		3,		subarray_rpos_opcodes		},
	{	0,	"strrpos",				NULL,		qb_translate_subarray_search,			2,		3,		subarray_rpos_opcodes		},
	{	0,	"sort",					NULL,		qb_translate_sort,						1,		1,		sort_opcodes				},
	{	0,	"rsort",				NULL,		qb_translate_sort,						1,		1,		rsort_opcodes				},
	{	0,	"utf8_decode",			NULL,		qb_translate_utf8_decode,				1,		1,		NULL						},
	{	0,	"utf8_encode",			NULL,		qb_translate_utf8_encode,				1,		1,		NULL						},
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
