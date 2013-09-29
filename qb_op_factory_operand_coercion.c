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
		qb_perform_type_coercion(cxt, &operands[i], expr_type, f->coercion_flags);
	}
}

// coerce all operands to boolean
static void ZEND_FASTCALL qb_coerce_operands_boolean(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count) {
	uint32_t i;
	for(i = 0; i < operand_count; i++) {
		qb_perform_boolean_coercion(cxt, &operands[i]);
	}
}

// coerce all operands to I32
static void ZEND_FASTCALL qb_coerce_operands_int(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count) {
	uint32_t i;
	for(i = 0; i < operand_count; i++) {
		qb_perform_type_coercion(cxt, &operands[i], QB_TYPE_I32, f->coercion_flags);
	}
}

// coerce all operands to the type of the highest-rank operand, ignoring expr_type
static void ZEND_FASTCALL qb_coerce_operands_highest_rank(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count) {
	qb_primitive_type operand_type = qb_get_highest_rank_type(cxt, operands, operand_count, f->coercion_flags);
	uint32_t i;
	for(i = 0; i < operand_count; i++) {
		qb_perform_type_coercion(cxt, &operands[i], operand_type, f->coercion_flags);
	}
}

// coerce operands that aren't addresses to the highest-rank type
static void ZEND_FASTCALL qb_coerce_operands_no_cast(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count) {
	qb_primitive_type operand_type = qb_get_highest_rank_type(cxt, operands, operand_count, f->coercion_flags);
	uint32_t i;
	for(i = 0; i < operand_count; i++) {
		if(operands[i].type != QB_OPERAND_ADDRESS) {
			qb_perform_type_coercion(cxt, &operands[i], operand_type, f->coercion_flags);
		}
	}
}

static void ZEND_FASTCALL qb_coerce_operands_assignment(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count) {
	qb_operand *value = &operands[1];
	if(value->type != QB_OPERAND_ADDRESS || f != (void *) &factory_assignment) {
		// since there're instructions for copying from one type to another, 
		// don't perform casting on when there's an address
		qb_perform_type_coercion(cxt, value, expr_type, f->coercion_flags);
	}
}

static void ZEND_FASTCALL qb_coerce_operands_array_element_assignment(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count) {
	qb_operand *index = &operands[1];
	qb_operand *value = &operands[2];
	qb_perform_type_coercion(cxt, index, QB_TYPE_U32, 0);
	if(value->type != QB_OPERAND_ADDRESS || f != (void *) &factory_array_element_assignment) {
		qb_perform_type_coercion(cxt, value, expr_type, f->coercion_flags);
	}
}

static void ZEND_FASTCALL qb_coerce_operands_object_property_assignment(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr, qb_operand *operands, uint32_t operand_count) {
	qb_operand *value = &operands[2];
	if(value->type != QB_OPERAND_ADDRESS || f != (void *) &factory_object_property_assignment) {
		qb_perform_type_coercion(cxt, value, expr, f->coercion_flags);
	}
}

static void ZEND_FASTCALL qb_coerce_operands_concat_variable(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count) {
	qb_operand *value = &operands[1];
	qb_perform_type_coercion(cxt, value, QB_TYPE_ANY, f->coercion_flags);
}

static void ZEND_FASTCALL qb_coerce_operands_concat_string(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count) {
	qb_operand *value = &operands[1];
	qb_perform_type_coercion(cxt, value, QB_TYPE_U08, f->coercion_flags);
}

static void ZEND_FASTCALL qb_coerce_operands_print(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count) {
	qb_operand *value = &operands[0];
	qb_perform_type_coercion(cxt, value, QB_TYPE_ANY, f->coercion_flags);
}

static void ZEND_FASTCALL qb_coerce_operands_return(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count) {
	qb_operand *value = &operands[0];
	if(cxt->stage == QB_STAGE_RESULT_TYPE_RESOLUTION || value->type != QB_OPERAND_ADDRESS) {
		qb_primitive_type retval_type;
		if(cxt->return_variable->address) {
			retval_type = cxt->return_variable->address->type;
			qb_perform_type_coercion(cxt, value, retval_type, f->coercion_flags);
		}
	}
}

static void ZEND_FASTCALL qb_coerce_operands_fetch_array_element(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count) {
	qb_operand *index = &operands[1];
	qb_perform_type_coercion(cxt, index, QB_TYPE_U32, f->coercion_flags);
}

static void ZEND_FASTCALL qb_coerce_operands_intrinsic(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count) {
	qb_operand *func = &operands[0], *arguments = &operands[1], *argument_count = &operands[2];
	f = func->intrinsic_function->extra;
	if(f->coerce_operands) {
		f->coerce_operands(cxt, f, expr_type, arguments->arguments, argument_count->number);
	}
}