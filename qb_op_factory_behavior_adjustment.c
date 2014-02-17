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

static int32_t qb_indicate_non_inlineable(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_operand *result) {
	cxt->function_flags &= ~QB_FUNCTION_INLINEABLE;
	return TRUE;
}

static int32_t qb_indicate_fork(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_operand *result) {
	cxt->function_flags &= ~QB_FUNCTION_INLINEABLE;
	cxt->function_flags |= QB_FUNCTION_MULTITHREADED;
	return TRUE;
}

static int32_t qb_indicate_debugger_presence(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_operand *result) {
	USE_TSRM
	cxt->debugger_present = TRUE;

	// don't inline the function when debugger is active
	cxt->function_flags &= ~QB_FUNCTION_INLINEABLE;
	cxt->function_flags |= QB_FUNCTION_HAS_BREAKPOINTS;
	return TRUE;
}

static int32_t qb_indicate_shadow_variables(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_operand *result) {
	cxt->function_flags |= QB_FUNCTION_NEED_SHADOWS;
	return TRUE;
}

