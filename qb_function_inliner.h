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

#ifndef QB_FUNCTION_INLINER_H_
#define QB_FUNCTION_INLINER_H_

typedef struct qb_function_inliner_context		qb_function_inliner_context;
typedef struct qb_address_substitution			qb_address_substitution;

struct qb_address_substitution {
	qb_address *original;
	qb_address *current;
};

struct qb_function_inliner_context {
	qb_compiler_context *caller_context;
	qb_compiler_context *callee_context;

	qb_address_substitution *substitutions;
	uint32_t substitution_count;

	qb_operand *arguments;
	uint32_t argument_count;
	qb_operand *result;
	qb_result_prototype *result_prototype;

	void ***tsrm_ls;
};

int32_t qb_transfer_inlined_function_ops(qb_function_inliner_context *cxt);

void qb_initialize_function_inliner_context(qb_function_inliner_context *cxt, qb_compiler_context *caller_cxt, qb_compiler_context *callee_cxt, qb_operand *arguments, uint32_t argument_count, qb_operand *result, qb_result_prototype *result_prototype TSRMLS_DC);
void qb_free_function_inliner_context(qb_function_inliner_context *cxt);

#endif