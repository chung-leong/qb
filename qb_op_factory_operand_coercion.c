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

// coerce all operands expr_type
static void ZEND_FASTCALL qb_coerce_operands_all(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count) {
	uint32_t i;
	for(i = 0; i < operand_count; i++) {
		qb_do_type_coercion(cxt, &operands[i], expr_type);
	}
}

// coerce all operands to boolean
static void ZEND_FASTCALL qb_coerce_operands_boolean(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count) {
	uint32_t i;
	for(i = 0; i < operand_count; i++) {
		qb_do_boolean_coercion(cxt, &operands[i]);
	}
}

// coerce all operands to I32
static void ZEND_FASTCALL qb_coerce_operands_int(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count) {
	uint32_t i;
	for(i = 0; i < operand_count; i++) {
		qb_do_type_coercion(cxt, &operands[i], QB_TYPE_I32);
	}
}

// coerce all operands to the type of the highest-rank operand, ignoring expr_type
static void ZEND_FASTCALL qb_coerce_operands_highest_rank(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count) {
	qb_primitive_type operand_type = qb_get_highest_rank_type(cxt, operands, operand_count, f->coercion_flags);
	uint32_t i;
	for(i = 0; i < operand_count; i++) {
		qb_do_type_coercion(cxt, &operands[i], operand_type);
	}
}

static void ZEND_FASTCALL qb_coerce_operands_assignment(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count) {
	qb_operand *value = &operands[1];
	if(cxt->stage == QB_STAGE_RESULT_TYPE_RESOLUTION) {
		// since there're instruction for copying from type to another, 
		// type coercion only needs to happen for result prototypes during type resolution
		qb_do_type_coercion(cxt, value, expr_type);
	}
}

static void ZEND_FASTCALL qb_coerce_operands_array_element_assignment(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count) {
	qb_operand *index = &operands[1];
	qb_operand *value = &operands[2];
	qb_do_type_coercion(cxt, index, QB_TYPE_INDEX);
	if(cxt->stage == QB_STAGE_RESULT_TYPE_RESOLUTION) {
		qb_do_type_coercion(cxt, value, expr_type);
	}
}

static void ZEND_FASTCALL qb_coerce_operands_object_property_assignment(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr, qb_operand *operands, uint32_t operand_count) {
	qb_operand *value = &operands[2];
	if(cxt->stage == QB_STAGE_RESULT_TYPE_RESOLUTION) {
		qb_do_type_coercion(cxt, value, expr);
	}
}