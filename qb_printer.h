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

#ifndef QB_PRINTER_H_
#define QB_PRINTER_H_

typedef struct qb_printer_context		qb_printer_context;

struct qb_printer_context {
	qb_data_pool *pool;
	qb_compiler_context *compiler_context;
	qb_storage *storage;

	void ***tsrm_ls;
};

void qb_print_ops(qb_printer_context *cxt);
void qb_print_source_ops(qb_printer_context *cxt);

void qb_initialize_printer_context(qb_printer_context *cxt, qb_compiler_context *compiler_cxt TSRMLS_DC);
void qb_free_printer_context(qb_printer_context *cxt);

#endif
