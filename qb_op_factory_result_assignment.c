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

static void ZEND_FASTCALL qb_set_result_temporary_value(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_result_prototype *result_prototype) {
	qb_variable_dimensions dim = { 0, 0 };

	// figure out the result size (it's a scalar if set_dimensions is null)
	if(f->set_dimensions) {
		f->set_dimensions(cxt, f, operands, operand_count, &dim);
	}

	// do this even if the result is not used
	result->type = QB_OPERAND_ADDRESS;
	result->address = qb_obtain_write_target(cxt, expr_type, &dim, result_prototype, f->result_flags);
}

static void ZEND_FASTCALL qb_set_result_variable(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_result_prototype *result_prototype) {
	qb_operand *variable = &operands[0];

	// if the expression type was set to void, then an earlier op has used the r-value as write target
	// so there's no need to perform the assignment
	if(expr_type != QB_TYPE_VOID) {
		*result = *variable;
	}
}

static void ZEND_FASTCALL qb_set_result_array_element(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_result_prototype *result_prototype) {
	qb_operand *container = &operands[0];
	qb_operand *index = &operands[1];

	if(expr_type != QB_TYPE_VOID) {
		result->type = QB_OPERAND_ADDRESS;
		result->address = qb_obtain_array_element(cxt, container->address, index->address);
	}
}

static void ZEND_FASTCALL qb_set_result_object_property(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_result_prototype *result_prototype) {
	qb_operand *container = &operands[0];
	qb_operand *name = &operands[1];

	if(expr_type != QB_TYPE_VOID) {
		if(container->type == QB_OPERAND_NONE) {
			result->address = qb_obtain_instance_variable(cxt, name->constant);
			result->type = QB_OPERAND_ADDRESS;
		} else if(container->type == QB_OPERAND_ADDRESS) {
			result->address = qb_obtain_named_element(cxt, container->address, name->constant);
			result->type = QB_OPERAND_ADDRESS;
		}
	}
}

static void ZEND_FASTCALL qb_set_result_true(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_result_prototype *result_prototype) {
	result->address = qb_obtain_constant_boolean(cxt, TRUE);
	result->type = QB_OPERAND_ADDRESS;
}

static void ZEND_FASTCALL qb_set_result_increment(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_result_prototype *result_prototype) {
	// set result to a temporary variable unless it'll end up just getting freed
	if(!result_prototype->destination || result_prototype->destination->type != QB_RESULT_DESTINATION_FREE) {
		// use the destination's type
		qb_primitive_type destination_type = qb_get_result_destination_type(cxt, result_prototype->destination);
		if(destination_type != QB_TYPE_UNKNOWN && destination_type != QB_TYPE_ANY) {
			expr_type = destination_type;
		}
		qb_set_result_temporary_value(cxt, f, expr_type, operands, operand_count, result, result_prototype);
	}
}
