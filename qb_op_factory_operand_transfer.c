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

static void ZEND_FASTCALL qb_transfer_operands_all(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_operand *dest) {
	uint32_t i;
	for(i = 0; i < operand_count; i++) {
		dest[i] = operands[i];
	}
	if(result) {
		dest[i] = *result;
	}
}

static void ZEND_FASTCALL qb_transfer_operands_assignment(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_operand *dest) {
	dest[0] = operands[operand_count - 1];
	dest[1] = *result;
}

static void ZEND_FASTCALL qb_transfer_operands_result_only(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_operand *dest) {
	dest[0] = *result;
}

static void ZEND_FASTCALL qb_transfer_operands_reverse_binary(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_operand *dest) {
	dest[0] = operands[1];
	dest[1] = operands[0];
	dest[2] = *result;
}

static void ZEND_FASTCALL qb_transfer_operands_print(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_operand *dest) {
	qb_operand *value = &operands[0];
	dest[0] = *value;
	if(value->address->dimension_count > 1) {
		dest[1].type = QB_OPERAND_ADDRESS;
		dest[1].address = qb_retrieve_array_dimensions(cxt, value->address);
	}
}

static void ZEND_FASTCALL qb_transfer_operands_return(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_operand *dest) {
	qb_operand *value = &operands[0];
	if(cxt->return_variable->address != NULL && value->type == QB_OPERAND_ADDRESS && cxt->return_variable->address != value->address) {
		qb_operand assigment_operands[2];
		qb_operand assignment_result;
		assigment_operands[0].type = QB_OPERAND_ADDRESS;
		assigment_operands[0].address = cxt->return_variable->address;
		assigment_operands[1].type = QB_OPERAND_ADDRESS;
		assigment_operands[1].address = value->address;
		assignment_result.type = QB_OPERAND_ADDRESS;
		assignment_result.address = cxt->return_variable->address;
		qb_create_op(cxt, &factory_assignment, assigment_operands, 2, &assignment_result, NULL, 0, TRUE);
	}
}

static void ZEND_FASTCALL qb_transfer_operands_minmax(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_operand *dest) {
	if(operand_count == 1) {
		dest[0] = operands[0];
		dest[1] = *result;
	} else {
		if(operand_count == 2) {
			dest[0] = operands[0];
			dest[1] = operands[1];
			dest[2] = *result;
		} else {
			qb_create_op(cxt, f, operands, operand_count - 1, result, NULL, 0, TRUE);
			dest[0] = *result;
			dest[1] = operands[operand_count - 1];
			dest[2] = *result;
		}
	}
}

static void ZEND_FASTCALL qb_transfer_operands_increment(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_operand *dest) {
	qb_operand *variable = &operands[0];

	// perform the assignment if there's an address
	if(result->type == QB_OPERAND_ADDRESS) {
		qb_operand assigment_operands[2];
		assigment_operands[0].type = QB_OPERAND_ADDRESS;
		assigment_operands[0].address = result->address;
		assigment_operands[1].type = QB_OPERAND_ADDRESS;
		assigment_operands[1].address = variable->address;
		qb_create_op(cxt, &factory_assignment, assigment_operands, 2, result, NULL, 0, TRUE);
	}
	dest[0] = *variable;
}

static void ZEND_FASTCALL qb_transfer_operands_modify_assign(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_operand *dest) {
	dest[0] = *result;
	dest[1] = operands[operand_count - 1];
	dest[2] = *result;
}

static void ZEND_FASTCALL qb_transfer_operands_intrinsic(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_operand *dest) {
	qb_operand *func = &operands[0], *arguments = &operands[1], *argument_count = &operands[2];
	f = func->intrinsic_function->extra;
	if(f->transfer_operands) {
		f->transfer_operands(cxt, f, arguments->arguments, argument_count->number, result, dest);
	}
}

static uint32_t ZEND_FASTCALL qb_get_operand_count_intrinsic(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count) {
	qb_operand *func = &operands[0], *arguments = &operands[1], *argument_count = &operands[2];
	f = func->intrinsic_function->extra;
	if(f->get_operand_count) {
		return f->get_operand_count(cxt, f, operands, operand_count);
	}
	return 0;
}