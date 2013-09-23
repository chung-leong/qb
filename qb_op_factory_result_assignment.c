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

static void ZEND_FASTCALL qb_set_result_temporary_value(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_result_prototype *result_prototype) {
	qb_variable_dimensions dim;
	qb_primitive_type expr_type = result_prototype->final_type;

	// figure out the result size 
	f->set_dimensions(cxt, f, operands, operand_count, &dim);

	// do this even if the result is not used
	result->type = QB_OPERAND_ADDRESS;
	result->address = qb_obtain_write_target(cxt, expr_type, &dim, result_prototype, f->result_flags);
}

static void ZEND_FASTCALL qb_set_result_assignment(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_result_prototype *result_prototype) {
	qb_operand *variable = &operands[0];
	*result = *variable;
}

static void ZEND_FASTCALL qb_set_result_array_element_assignment(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_result_prototype *result_prototype) {
	qb_operand *container = &operands[0];
	qb_operand *index = &operands[1];

	result->type = QB_OPERAND_ADDRESS;
	result->address = qb_retrieve_array_element(cxt, container->address, index->address);
}

static void ZEND_FASTCALL qb_set_result_object_property_assignment(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_result_prototype *result_prototype) {
	qb_operand *container = &operands[0];
	qb_operand *name = &operands[1];

	if(container->type == QB_OPERAND_NONE) {
		qb_variable *qvar = qb_obtain_instance_variable(cxt, name->constant);
		result->address = qvar->address;
		result->type = QB_OPERAND_ADDRESS;
	} else if(container->type == QB_OPERAND_ADDRESS) {
		result->address = qb_retrieve_named_element(cxt, container->address, name->constant);
		result->type = QB_OPERAND_ADDRESS;
	}
}

static void ZEND_FASTCALL qb_set_result_true(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_result_prototype *result_prototype) {
	result->address = qb_obtain_constant_boolean(cxt, TRUE);
	result->type = QB_OPERAND_ADDRESS;
}