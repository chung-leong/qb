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

#ifndef QB_PARSER_H_
#define QB_PARSER_H_

typedef struct qb_parser_context			qb_parser_context;
typedef struct qb_type_declaration			qb_type_declaration;
typedef struct qb_function_declaration		qb_function_declaration;
typedef struct qb_class_declaration			qb_class_declaration;

struct qb_type_declaration {
	pcre *regexp;
	const char *name;
	uint32_t name_length;
	ulong hash_value;
	uint32_t type;
	uint32_t flags;
	uint32_t *dimensions;
	uint32_t dimension_count;
	qb_index_alias_scheme **index_alias_schemes;
};

struct qb_class_declaration {
	qb_type_declaration **declarations;
	uint32_t declaration_count;
	zend_class_entry *zend_class;
};

struct qb_function_declaration {
	qb_type_declaration **declarations;
	uint32_t declaration_count;
	uint32_t flags;
	const char *import_path;
	uint32_t import_path_length;
	const char *function_name;
	qb_class_declaration *class_declaration;
};

struct qb_parser_context {
	qb_data_pool *pool;
	zend_class_entry *zend_class;
	const char *file_path;
	uint32_t line_number;
};

qb_function_declaration * qb_parse_function_doc_comment(qb_parser_context *cxt, const char *doc_comment, size_t doc_comment_len);
qb_function_declaration * qb_parse_function_declaration_table(qb_parser_context *cxt, HashTable *ht);
qb_class_declaration * qb_parse_class_doc_comment(qb_parser_context *cxt, zend_class_entry *ce);

void qb_initialize_parser_context(qb_parser_context *cxt, qb_data_pool *pool, zend_class_entry *ce, const char *filename, uint32_t line_number TSRMLS_DC);
void qb_free_parser_context(qb_parser_context *cxt);

#endif