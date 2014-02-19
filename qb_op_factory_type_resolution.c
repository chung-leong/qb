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
static int32_t qb_resolve_expression_type_boolean(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_primitive_type *p_type, uint32_t *p_flags) {
	*p_type = QB_TYPE_I32;
	*p_flags = f->address_flags;
	return TRUE;
}

// the expresion is always an index (uint32_t currently)
static int32_t qb_resolve_expression_type_index(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_primitive_type *p_type, uint32_t *p_flags) {
	*p_type = QB_TYPE_U32;
	*p_flags = f->address_flags;
	return TRUE;
}

// the expresion is always an signed index (int32_t currently)
static int32_t qb_resolve_expression_type_search_index(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_primitive_type *p_type, uint32_t *p_flags) {
	*p_type = QB_TYPE_S32;
	*p_flags = f->address_flags;
	return TRUE;
}

// the expression is always a U08
static int32_t qb_resolve_expression_type_string(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_primitive_type *p_type, uint32_t *p_flags) {
	*p_type = QB_TYPE_U08;
	*p_flags = f->address_flags;
	return TRUE;
}

// the expression is always a F64
static int32_t  qb_resolve_expression_type_double(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_primitive_type *p_type, uint32_t *p_flags) {
	*p_type = QB_TYPE_F64;
	*p_flags = f->address_flags;
	return TRUE;
}

// the expresion has the same type as the highest-rank operand
static int32_t qb_resolve_expression_type_highest_rank(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_primitive_type *p_type, uint32_t *p_flags) {
	*p_type = qb_get_highest_rank_type(cxt, operands, operand_count, f->coercion_flags);
	*p_flags = f->address_flags;
	return TRUE;
}

// the expression will have the same type as the l-value
static int32_t qb_resolve_expression_type_lvalue(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_primitive_type *p_type, uint32_t *p_flags) {
	*p_type = QB_TYPE_ANY;
	*p_flags = f->address_flags;
	return TRUE;
}

// the expression has the same type as the first operand
static int32_t qb_resolve_expression_type_first_operand(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_primitive_type *p_type, uint32_t *p_flags) {
	*p_type = qb_get_operand_type(cxt, &operands[0], f->coercion_flags);
	*p_flags = f->address_flags;
	return TRUE;
}

// the expression has the same type as the third operand
static int32_t qb_resolve_expression_type_third_operand(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_primitive_type *p_type, uint32_t *p_flags) {
	*p_type = qb_get_operand_type(cxt, &operands[2], f->coercion_flags);
	*p_flags = f->address_flags;
	return TRUE;
}

static int32_t qb_resolve_expression_type_assign(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_primitive_type *p_type, uint32_t *p_flags) {
	qb_operand *variable = &operands[0], *value = &operands[1];
	if(variable->type == QB_OPERAND_EMPTY) {
		// the variable doesn't necessarily have to exist
		// the type is determined by the value
		*p_type = qb_get_operand_type(cxt, value, f->coercion_flags);
	} else {
		*p_type = qb_get_operand_type(cxt, variable, f->coercion_flags);
	}
	*p_flags = f->address_flags;
	return TRUE;
}

static int32_t qb_resolve_expression_type_assign_branching(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_primitive_type *p_type, uint32_t *p_flags) {
	qb_operand *value = &operands[0];
	uint32_t flags = 0;
	if(cxt->stage == QB_STAGE_RESULT_TYPE_RESOLUTION) {
		// use the type of the value only when it's definite
		flags = QB_RETRIEVE_DEFINITE_TYPE_ONLY;
	}
	*p_type = qb_get_operand_type(cxt, value, flags);
	*p_flags = f->address_flags;
	return TRUE;
}

static int32_t qb_resolve_expression_type_assign_return_value(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_primitive_type *p_type, uint32_t *p_flags) {
	if(cxt->return_variable->address) {
		*p_type = cxt->return_variable->address->type;
	} else {
		*p_type = QB_TYPE_VOID;
	}
	*p_flags = f->address_flags;
	return TRUE;
}

static int32_t qb_resolve_expression_type_assign_generator_key(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_primitive_type *p_type, uint32_t *p_flags) {
	if(cxt->return_key_variable->address) {
		*p_type = cxt->return_key_variable->address->type;
	} else {
		*p_type = QB_TYPE_VOID;
	}
	*p_flags = f->address_flags;
	return TRUE;
}

static int32_t qb_resolve_expression_type_sent_value(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_primitive_type *p_type, uint32_t *p_flags) {
	if(cxt->sent_variable->address) {
		*p_type = cxt->sent_variable->address->type;
	} else {
		*p_type = QB_TYPE_VOID;
	}
	*p_flags = f->address_flags;
	return TRUE;
}

static int32_t qb_resolve_expression_type_cast(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_primitive_type *p_type, uint32_t *p_flags) {
	qb_cast_op_factory *cf = (qb_cast_op_factory *) f;
	*p_type = cf->type;
	*p_flags = f->address_flags;
	return TRUE;
}

static int32_t qb_resolve_expression_type_array_size(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_primitive_type *p_type, uint32_t *p_flags) {
	qb_operand *container = &operands[0], *recursive = &operands[1];
	*p_type = QB_TYPE_U32;
	*p_flags = 0;
	if(qb_is_constant_expression(cxt, recursive, 1)) {
		if(container->type == QB_OPERAND_ADDRESS && FIXED_LENGTH(container->address)) {
			*p_flags |= QB_ADDRESS_CONSTANT;
		} else if(container->type == QB_OPERAND_ARRAY_INITIALIZER) {
			*p_flags |= QB_ADDRESS_CONSTANT;
		}
	}
	return TRUE;
}

static int32_t qb_resolve_expression_type_object_property(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_primitive_type *p_type, uint32_t *p_flags) {
	qb_operand *container = &operands[0], *name = &operands[1];
	if(container->type == QB_OPERAND_RESULT_PROTOTYPE) {
		*p_type = container->result_prototype->preliminary_type;
	} else {
		qb_address *address = qb_obtain_object_property(cxt, container, name, 0);
		if(address) {
			*p_type = address->type;
		} else {
			// just set it to something--it'll fail during validation
			*p_type = QB_TYPE_I32;
		}
	}
	*p_flags = f->address_flags;
	return TRUE;
}

static int32_t qb_resolve_expression_type_fetch_local(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_primitive_type *p_type, uint32_t *p_flags) {
	qb_operand *name = &operands[0];
	qb_variable *qvar = qb_get_local_variable(cxt, name->constant);
	if(qvar && qvar->address) {
		*p_type = qvar->address->type;
	} else {
		*p_type = QB_TYPE_VOID;
	}
	*p_flags = f->address_flags;
	return TRUE;
}

static int32_t qb_resolve_expression_type_fetch_global(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_primitive_type *p_type, uint32_t *p_flags) {
	qb_operand *name = &operands[0];
	qb_variable *qvar = qb_get_global_variable(cxt, name->constant);
	if(qvar && qvar->address) {
		*p_type = qvar->address->type;
	} else {
		*p_type = QB_TYPE_VOID;
	}
	*p_flags = f->address_flags;
	return TRUE;
}

static int32_t qb_resolve_expression_type_fetch_static(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_primitive_type *p_type, uint32_t *p_flags) {
	qb_operand *name = &operands[0];
	qb_variable *qvar = qb_get_static_variable(cxt, name->constant);
	if(qvar && qvar->address) {
		*p_type = qvar->address->type;
	} else {
		*p_type = QB_TYPE_VOID;
	}
	*p_flags = f->address_flags;
	return TRUE;
}

static int32_t qb_resolve_expression_type_fetch_class(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_primitive_type *p_type, uint32_t *p_flags) {
	qb_operand *name = &operands[0], *scope = &operands[1];
	qb_variable *qvar = qb_get_class_variable(cxt, scope->zend_class, name->constant);
	if(qvar && qvar->address) {
		*p_type = qvar->address->type;
	} else {
		*p_type = QB_TYPE_VOID;
	}
	*p_flags = f->address_flags;
	return TRUE;
}

static int32_t qb_resolve_expression_type_rand(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_primitive_type *p_type, uint32_t *p_flags) {
	if(operand_count == 2) {
		qb_resolve_expression_type_highest_rank(cxt, f, operands, operand_count, p_type, p_flags);
	} else {
		qb_resolve_expression_type_lvalue(cxt, f, operands, operand_count, p_type, p_flags);
	}
	*p_flags = f->address_flags;
	return TRUE;
}

static int32_t qb_resolve_expression_type_utf8_decode(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_primitive_type *p_type, uint32_t *p_flags) {
	*p_type = QB_TYPE_U16;
	*p_flags = f->address_flags;
	return TRUE;
}

static int32_t qb_resolve_expression_type_unpack(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_primitive_type *p_type, uint32_t *p_flags) {
	qb_operand *type = &operands[2];
	*p_type = QB_TYPE_ANY;
	if(type->type == QB_OPERAND_ZVAL && type->constant->type == IS_LONG) {
		uint32_t value = Z_LVAL_P(type->constant);
		if(value < QB_TYPE_COUNT) {
			*p_type = value;
		}
	}
	*p_flags = f->address_flags;
	return TRUE;
}

static int32_t qb_resolve_expression_type_function_call(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_primitive_type *p_type, uint32_t *p_flags) {
	USE_TSRM
	qb_operand *func = &operands[0];
	qb_function *qfunc = qb_find_compiled_function(func->zend_function TSRMLS_CC);
	if(qfunc->return_variable->address) {
		*p_type = qfunc->return_variable->address->type;
		*p_flags = qfunc->return_variable->address->flags & (QB_ADDRESS_BOOLEAN | QB_ADDRESS_STRING);
	} else {
		*p_type = QB_TYPE_VOID;
		*p_flags = 0;
	}
	return TRUE;
}

static int32_t qb_resolve_expression_type_zend_function_call(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_primitive_type *p_type, uint32_t *p_flags) {
	*p_type = QB_TYPE_ANY;
	*p_flags = 0;
	return TRUE;
}

static int32_t qb_resolve_expression_version_compare(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_primitive_type *p_type, uint32_t *p_flags) {
	*p_type = QB_TYPE_I32;
	if(operand_count == 3) {
		*p_flags = QB_ADDRESS_BOOLEAN;
	} else {
		*p_flags = 0;
	}
	return TRUE;
}
