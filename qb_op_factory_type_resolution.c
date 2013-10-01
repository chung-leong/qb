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

// the expression is always a boolean
static qb_primitive_type qb_resolve_expression_type_boolean(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count) {
	return QB_TYPE_I32;
}

// the expresion is always an index (uint32_t currently)
static qb_primitive_type qb_resolve_expression_type_index(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count) {
	return QB_TYPE_U32;
}

// the expression is always a U08
static qb_primitive_type qb_resolve_expression_type_string(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count) {
	return QB_TYPE_U08;
}

// the expresion has the same type as the highest-rank operand
static qb_primitive_type qb_resolve_expression_type_highest_rank(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count) {
	return qb_get_highest_rank_type(cxt, operands, operand_count, f->coercion_flags);
}

// the expression will have the same type as the l-value
static qb_primitive_type qb_resolve_expression_lvalue(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count) {
	return QB_TYPE_ANY;
}

// the expression has the same type as the first operand
static qb_primitive_type qb_resolve_expression_type_first_operand(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count) {
	return qb_get_operand_type(cxt, &operands[0], f->coercion_flags);
}

// the expression has the same type as the second operand
static qb_primitive_type qb_resolve_expression_type_second_operand(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count) {
	return qb_get_operand_type(cxt, &operands[1], f->coercion_flags);
}

// the expression is the a 
static qb_primitive_type qb_resolve_expression_type_cast(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count) {
	qb_cast_op_factory *cf = (qb_cast_op_factory *) f;
	return cf->type;
}

static qb_primitive_type qb_resolve_expression_type_object_property(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count) {
	qb_operand *container = &operands[0];
	qb_operand *name = &operands[1];
	qb_primitive_type expr_type = qb_get_property_type(cxt, container, name);

	// just return something--it'll fail during validation
	if(expr_type == QB_TYPE_ANY) {
		expr_type = QB_TYPE_I32;
	}
	return expr_type;
}

static qb_primitive_type qb_resolve_expression_type_fetch_local(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count) {
	qb_operand *name = &operands[0];
	qb_variable *qvar = qb_get_local_variable(cxt, name->constant);
	if(qvar && qvar->address) {
		return qvar->address->type;
	}
	return QB_TYPE_VOID;
}

static qb_primitive_type qb_resolve_expression_type_fetch_global(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count) {
	qb_operand *name = &operands[0];
	qb_variable *qvar = qb_get_global_variable(cxt, name->constant);
	if(qvar && qvar->address) {
		return qvar->address->type;
	}
	return QB_TYPE_VOID;
}

static qb_primitive_type qb_resolve_expression_type_fetch_static(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count) {
	qb_operand *name = &operands[0];
	qb_variable *qvar = qb_get_static_variable(cxt, name->constant);
	if(qvar && qvar->address) {
		return qvar->address->type;
	}
	return QB_TYPE_VOID;
}

static qb_primitive_type qb_resolve_expression_type_fetch_class(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count) {
	qb_operand *name = &operands[0], *scope = &operands[1];
	qb_variable *qvar = qb_get_class_variable(cxt, scope->zend_class, name->constant);
	if(qvar && qvar->address) {
		return qvar->address->type;
	}
	return QB_TYPE_VOID;
}

static qb_primitive_type qb_resolve_expression_type_intrinsic(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count) {
	qb_operand *func = &operands[0], *arguments = &operands[1], *argument_count = &operands[2];
	f = func->intrinsic_function->extra;
	if(f->resolve_type) {
		return f->resolve_type(cxt, f, arguments->arguments, argument_count->number);
	}
	return QB_TYPE_VOID;
}