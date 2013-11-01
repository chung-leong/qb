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

// the expresion is always an signed index (int32_t currently)
static qb_primitive_type qb_resolve_expression_type_search_index(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count) {
	return QB_TYPE_S32;
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

// the expression has the same type as the third operand
static qb_primitive_type qb_resolve_expression_type_third_operand(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count) {
	return qb_get_operand_type(cxt, &operands[2], f->coercion_flags);
}

static qb_primitive_type qb_resolve_expression_type_assign(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count) {
	qb_operand *variable = &operands[0], *value = &operands[1];
	if(variable->type == QB_OPERAND_EMPTY) {
		// the variable doesn't necessarily have to exist
		// the type is determined by the value
		return qb_get_operand_type(cxt, value, f->coercion_flags);
	} else {
		return qb_get_operand_type(cxt, variable, f->coercion_flags);
	}
}

static qb_primitive_type qb_resolve_expression_type_cast(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count) {
	qb_cast_op_factory *cf = (qb_cast_op_factory *) f;
	return cf->type;
}

static qb_primitive_type qb_resolve_expression_type_object_property(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count) {
	qb_operand *container = &operands[0], *name = &operands[1];
	qb_primitive_type expr_type;
	if(container->type == QB_OPERAND_RESULT_PROTOTYPE) {
		expr_type = container->result_prototype->preliminary_type;
	} else {
		qb_address *address = qb_obtain_object_property(cxt, container, name, 0);

		if(address) {
			expr_type = address->type;
		} else {
			// just return something--it'll fail during validation
			expr_type = QB_TYPE_I32;
		}
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

static qb_primitive_type qb_resolve_expression_type_utf8_decode(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count) {
	return QB_TYPE_U16;
}

static qb_primitive_type qb_resolve_expression_type_unpack(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count) {
	qb_operand *type = &operands[2];
	qb_primitive_type expr_type = QB_TYPE_ANY;
	if(type->type == QB_OPERAND_ZVAL && type->constant->type == IS_LONG) {
		uint32_t value = Z_LVAL_P(type->constant);
		if(value < QB_TYPE_COUNT) {
			expr_type = value;
		}
	}
	return expr_type;
}

static qb_primitive_type qb_resolve_expression_type_function_call(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count) {
	qb_operand *func = &operands[0];
	qb_function *qfunc = qb_find_compiled_function(func->zend_function);
	f->address_flags &= ~(QB_ADDRESS_BOOLEAN | QB_ADDRESS_STRING);
	if(qfunc->return_variable->address) {
		// should handle the address flag in a cleaner way
		f->address_flags |= ((QB_ADDRESS_BOOLEAN | QB_ADDRESS_STRING) & qfunc->return_variable->address->flags);
		return qfunc->return_variable->address->type;
	} else {
		return QB_TYPE_VOID;
	}
}

static qb_primitive_type qb_resolve_expression_type_zend_function_call(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count) {
	return QB_TYPE_ANY;
}
