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

static void ZEND_FASTCALL qb_validate_operands_array_element(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count) {
	qb_operand *container = &operands[0];
	qb_operand *index = &operands[1];

	if(container->type != QB_OPERAND_ADDRESS) {
		qb_abort("cannot retrieve element from non-array");
	}

	if((index->type == QB_OPERAND_ADDRESS && !SCALAR(index->address)) || (index->type == QB_OPERAND_ARRAY_INITIALIZER)) {
		qb_abort("an array cannot be used as an array index");
	} else if(index->type == QB_OPERAND_ZVAL) {
		if(index->constant->type == IS_STRING) {
			qb_abort("no support for associative array");
		} else if(index->constant->type != IS_BOOL && index->constant->type != IS_LONG && index->constant->type != IS_DOUBLE) {
			qb_abort("invalid index type");
		}
	} else if(index->type == QB_OPERAND_NONE) {
		// an append operation
		if(!EXPANDABLE_ARRAY(container->address)) {
			qb_abort("cannot append element to a non-expanding array");
		}
	}
}

static void ZEND_FASTCALL qb_validate_operands_object_property(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count) {
	qb_operand *container = &operands[0];
	qb_operand *name = &operands[1];

	if(name->type != QB_OPERAND_ZVAL) {
		qb_abort("no support for variable variable-names");
	}
	if(container->type == QB_OPERAND_NONE) {
		if(!qb_find_instance_variable(cxt, name->constant)) {
			qb_abort("no property by the name of '%s'", Z_STRVAL_P(name->constant));
		}
	} else if(container->type == QB_OPERAND_ADDRESS) {
		int32_t index;
		if(SCALAR(container->address)) {
			qb_abort("illegal operation: not an array");
		}
		if(!container->address->index_alias_schemes || !container->address->index_alias_schemes[0]) {
			qb_abort("array elements are not named");
		}
		index = qb_find_index_alias(cxt, container->address->index_alias_schemes[0], name->constant);
		if(index == -1) {
			qb_abort("no element by the name of '%s'", Z_STRVAL_P(name->constant));
		}
	} else {
		qb_abort("cannot fetch property of objects other than $this");
	}
}

static void ZEND_FASTCALL qb_validate_operands_return(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count) {
	if(!cxt->return_variable->address) {
		qb_operand *value = &operands[0];
		if(value->type != QB_OPERAND_NONE && !(value->type == QB_OPERAND_ZVAL && value->constant->type == IS_NULL)) {
			qb_abort("returning a value in a function with return type declared to be void");
		}
	}
}
static void ZEND_FASTCALL qb_validate_operands_rand(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count) {
	if(operand_count != 0 && operand_count != 2) {
		qb_abort("%s() expects either 0 or 2 arguments", cxt->intrinsic_function_name);
	}
}

static void ZEND_FASTCALL qb_validate_operands_minmax(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count) {
	qb_operand *container = &operands[0];

	if(operand_count == 1) {
		if(SCALAR(container->address)) {
			qb_abort("%s() expects an array as parameter when only one parameter is given", cxt->intrinsic_function_name);
		}
	}
}

static void ZEND_FASTCALL qb_validate_array_append(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count) {
	qb_operand *value = &operands[0], *index = &operands[1];

	if(index->type == QB_OPERAND_ZVAL) {
		switch(Z_TYPE_P(index->constant)) {
			case IS_BOOL:
			case IS_LONG:
			case IS_DOUBLE: {
			}	break;
			case IS_STRING: {
			}	break;
		}
	}
}

static void ZEND_FASTCALL qb_validate_array_init(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count) {
	if(operand_count > 0) {
		qb_validate_array_append(cxt, f, operands, operand_count);
	}
}

static zval * ZEND_FASTCALL qb_get_special_constant(qb_compiler_context *cxt, const char *name, uint32_t length) {
	static zval type_constants[QB_TYPE_COUNT];
	static zval qb_indicator;

	if(strcmp(name, "__QB__") == 0) {
		Z_TYPE(qb_indicator) = IS_LONG;
		Z_LVAL(qb_indicator) = 1;
		return &qb_indicator;
	} else {
		uint32_t i;
		for(i = 0; i < QB_TYPE_COUNT; i++) {
			const char *type = type_names[i];
			if(strcmp(name, type) == 0) {
				Z_TYPE(type_constants[i]) = IS_LONG;
				Z_LVAL(type_constants[i]) = i;
				return &type_constants[i];
			}
		}
	}
	return NULL;
}

static void ZEND_FASTCALL qb_validate_operands_fetch_class_self(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count) {
	zend_class_entry *ce = cxt->zend_function->common.scope;
	if(!ce) {
		qb_abort("Cannot access self:: when no class scope is active");
	}
}

static void ZEND_FASTCALL qb_validate_operands_fetch_class_parent(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count) {
	zend_class_entry *ce = cxt->zend_function->common.scope;
	if(!ce) {
		qb_abort("Cannot access parent:: when no class scope is active");
	}
	if(!ce->parent) {
		qb_abort("Cannot access parent:: when current class scope has no parent");
	}
}

static void ZEND_FASTCALL qb_validate_operands_fetch_class_static(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count) {
	zend_class_entry *ce = cxt->zend_function->common.scope;
	if(!ce) {
		qb_abort("Cannot access static:: when no class scope is active");
	}
}

static void ZEND_FASTCALL qb_validate_operands_fetch_constant(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count) {
	USE_TSRM
	qb_operand *scope = &operands[0], *name = &operands[1];
	zend_class_entry *ce = NULL;
	if(scope->type == QB_OPERAND_ZEND_CLASS) {
		ce = scope->zend_class;
	} else if(scope->type == QB_OPERAND_ZVAL) {
		ce = zend_fetch_class_by_name(Z_STRVAL_P(scope->constant), Z_STRLEN_P(scope->constant), NULL, 0 TSRMLS_CC);
	}
	if(ce) {
		if(!zend_hash_exists(&ce->constants_table, Z_STRVAL_P(name->constant), Z_STRLEN_P(name->constant) + 1)) {
			qb_abort("Undefined class constant '%s'", Z_STRVAL_P(name->constant));
		}
	} else {
		if(!zend_hash_exists(EG(zend_constants), Z_STRVAL_P(name->constant), Z_STRLEN_P(name->constant) + 1)) {
			if(!qb_get_special_constant(cxt, Z_STRVAL_P(name->constant), Z_STRLEN_P(name->constant))) {
				qb_abort("Undefined constant '%s'", Z_STRVAL_P(name->constant));
			}
		}
	}
}