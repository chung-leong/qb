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

typedef struct qb_build_context				qb_build_context;
typedef struct qb_function_tag				qb_function_tag;
typedef struct qb_function_dependencies		qb_function_dependencies;

struct qb_build_context {
	qb_function_tag *function_tags;
	uint32_t function_tag_count;

	qb_compiler_context **compiler_contexts;
	uint32_t compiler_context_count;

	qb_function_declaration **function_declarations;
	uint32_t function_declaration_count;

	qb_class_declaration **class_declarations;
	uint32_t class_declaration_count;

	qb_data_pool *pool;
	qb_data_pool _pool;

	void ***tsrm_ls;
};

struct qb_function_tag {
	const char *function_name;
	const char *file_path;
	zend_class_entry *scope;
	uint32_t line_number;
};

struct qb_function_dependencies {
	qb_function_declaration *declaration;
	uint32_t index;
	int8_t *flags;
	const char *name;
	uint32_t name_length;
	ulong hash;
};

void qb_build(qb_build_context *cxt);

qb_compiler_context * qb_find_compiler_context(qb_build_context *cxt, qb_function *function_prototype);

void qb_initialize_build_context(qb_build_context *cxt TSRMLS_DC);
void qb_free_build_context(qb_build_context *cxt);

#endif