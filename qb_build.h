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

#ifndef QB_BUILD_H_
#define QB_BUILD_H_

struct qb_build_context {
	qb_compiler_context *compiler_contexts;
	uint32_t compiler_context_count;
	qb_function_declaration **function_declarations;
	uint32_t function_declaration_count;
	qb_class_declaration **class_declarations;
	uint32_t class_declaration_count;

	uint32_t deferral_count;

	qb_data_pool *pool;
	qb_data_pool _pool;

	void ***tsrm_ls;
};

static zend_always_inline void qb_add_function_declaration(qb_build_context *cxt, qb_function_declaration *function_decl) {
	qb_function_declaration **p = qb_enlarge_array((void **) &cxt->function_declarations, 1);
	*p = function_decl;
}

static zend_always_inline void qb_add_class_declaration(qb_build_context *cxt, qb_class_declaration *class_decl) {
	qb_class_declaration **p = qb_enlarge_array((void **) &cxt->class_declarations, 1);
	*p = class_decl;
}

qb_function_declaration * qb_get_function_declaration(qb_build_context *cxt, zend_op_array *zop_array);
qb_class_declaration * qb_get_class_declaration(qb_build_context *cxt, zend_class_entry *ce);

void qb_build(qb_build_context *cxt);

void qb_initialize_build_context(qb_build_context *cxt TSRMLS_DC);
void qb_free_build_context(qb_build_context *cxt);

#endif