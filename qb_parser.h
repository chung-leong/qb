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
typedef struct qb_lexer_context				qb_lexer_context;
typedef struct qb_type_declaration			qb_type_declaration;
typedef struct qb_function_declaration		qb_function_declaration;
typedef struct qb_class_declaration			qb_class_declaration;

typedef struct qb_matrix_dimension		qb_matrix_dimension;
typedef struct qb_token_position		qb_token_position;

enum {
	QB_TYPE_DECL_STRING				= 0x00010000,
	QB_TYPE_DECL_BOOLEAN			= 0x00020000,
	QB_TYPE_DECL_AUTOVIVIFICIOUS	= 0x00040000,
	QB_TYPE_DECL_HAS_ALIAS_SCHEMES	= 0x00080000,
	QB_TYPE_DECL_IMAGE				= 0x00100000,
};

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
	zend_op_array *zend_op_array;
	qb_class_declaration *class_declaration;
};

struct qb_matrix_dimension {
	uint32_t row;
	uint32_t column;
};


struct qb_token_position {
	uint32_t index;
	uint32_t length;
};

struct qb_lexer_context {
	const char *cursor;
	const char *token;
	const char *base;
	const char *marker;
	int condition;
};

struct qb_parser_context {
	qb_lexer_context *lexer_context;
	qb_lexer_context default_lexer_context;
	qb_lexer_context definition_lexer_context;
	int parser_selector;

	qb_function_declaration *function_declaration;
	qb_class_declaration *class_declaration;
	qb_type_declaration *type_declaration;

	qb_data_pool *pool;
	zend_class_entry *zend_class;	
	zend_property_info *zend_property;
	const char *file_path;
	uint32_t file_id;
	uint32_t line_id;
	uint32_t line_number_max;

#ifdef ZTS
	void ***tsrm_ls;
#endif
};

qb_function_declaration * qb_parse_function_doc_comment(qb_parser_context *cxt, zend_op_array *op_array);
qb_class_declaration * qb_parse_class_doc_comment(qb_parser_context *cxt, zend_class_entry *ce);

void qb_initialize_parser_context(qb_parser_context *cxt, qb_data_pool *pool, zend_class_entry *ce, const char *filename, uint32_t line_number TSRMLS_DC);
void qb_free_parser_context(qb_parser_context *cxt);

int32_t qb_find_engine_tag(const char *doc_comment);

void qb_set_engine_flags(qb_parser_context *cxt, uint32_t flags);
void qb_add_import(qb_parser_context *cxt, qb_token_position p);
void qb_add_variable_declaration(qb_parser_context *cxt, uint32_t type);
void qb_add_property_declaration(qb_parser_context *cxt, uint32_t type);
void qb_end_statement(qb_parser_context *cxt);
void qb_end_variable_declaration(qb_parser_context *cxt);
void qb_set_variable_type(qb_parser_context *cxt, qb_primitive_type type);
void qb_add_dimension(qb_parser_context *cxt, uint32_t count, uint32_t flags);
void qb_add_index_alias_scheme(qb_parser_context *cxt, qb_index_alias_scheme *scheme);
void qb_attach_variable_name(qb_parser_context *cxt, qb_token_position p);
void qb_attach_variable_name_regexp(qb_parser_context *cxt, qb_token_position p);
void qb_attach_index_alias_scheme_class(qb_parser_context *cxt, qb_index_alias_scheme *scheme, qb_token_position p);
qb_index_alias_scheme *qb_create_index_alias_scheme(qb_parser_context *cxt);
void qb_add_index_alias(qb_parser_context *cxt, qb_index_alias_scheme *scheme, qb_token_position p);
uint32_t qb_parse_integer(qb_parser_context *cxt, qb_token_position p, uint32_t radix);

void qb_doc_comment_yyinit(qb_parser_context *cxt, const char *doc_comment, int parser_selector);
int qb_doc_comment_yyparse(qb_parser_context *cxt);
int qb_doc_comment_yyerror(qb_parser_context *cxt, const char *msg);

#endif
