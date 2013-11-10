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

#ifndef QB_EXTRACTOR_PBJ_H_
#define QB_EXTRACTOR_PBJ_H_

enum {
	QB_PBJ_DETAILS	= 1,
	QB_PBJ_DECLARATION,
};

typedef struct qb_extractor_context qb_extractor_context;

struct qb_extractor_context {
	zval *input;
	zval *return_value;

	qb_data_pool *pool;
	qb_compiler_context *compiler_context;
	qb_pbj_translator_context *translator_context;

	void ***tsrm_ls;
};

void qb_extract_pbj_info(qb_extractor_context *cxt, int output_type);

void qb_initialize_extractor_context(qb_extractor_context *cxt, zval *input, zval *return_value TSRMLS_DC);
void qb_free_extractor_context(qb_extractor_context *cxt);

#endif
