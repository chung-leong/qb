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

static void ZEND_FASTCALL qb_validate_operands_array_element_assignment(qb_compiler_context *cxt, void *factory, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_result_prototype *result_prototype) {
	qb_operand *container = &operands[0];
	qb_operand *index = &operands[1];
	qb_operand *value = &operands[2];

	if(container->type != QB_OPERAND_ADDRESS) {
		qb_abort("cannot retrieve element");
	}
	if(index->type == QB_OPERAND_NONE) {
		// an append operation
		if(!EXPANDABLE_ARRAY(container->address)) {
			qb_abort("cannot append element to a non-expanding array");
		}
	}
}

static void ZEND_FASTCALL qb_validate_operands_property_assignment(qb_compiler_context *cxt, void *factory, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_result_prototype *result_prototype) {
	qb_operand *object = &operands[0];
	qb_operand *name = &operands[1];
	qb_operand *value = &operands[2];

	if(name->type != QB_OPERAND_ZVAL) {
		qb_abort("no support for variable variable-names");
	}
	if(object->type == QB_OPERAND_NONE) {
		if(!qb_obtain_instance_variable(cxt, name->constant)) {
			qb_abort("no property by the name of '%s'", Z_STRVAL_P(name->constant));
		}
	} else if(object->type == QB_OPERAND_ADDRESS) {
		int32_t index;
		if(SCALAR(container->address)) {
			qb_abort("illegal operation: not an array");
		}
		if(!container->address->index_alias_schemes || !container_address->index_alias_schemes[0]) {
			qb_abort("array elements are not named");
		}
		index = qb_find_index_alias(cxt, container_address->index_alias_schemes[0], name->constant);
		if(index == -1) {
			qb_abort("no element by the name of '%s'", Z_STRVAL_P(name->constant));
		}
	} else {
		qb_abort("cannot fetch property of objects other than $this");
	}
}

