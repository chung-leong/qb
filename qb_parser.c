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

#include "qb.h"
#include "qb_parser_re2c.h"
#include "qb_parser_bison.h"

int32_t qb_find_engine_tag(const char *doc_comment) {
	// use strstr() to quickly check if the tag is there
	const char *tag;
	for(tag = strstr(doc_comment, "@engine"); tag; tag = strstr(tag, "@engine")) {
		tag += sizeof("@engine") - 1;
		if(*tag == ' ' || *tag == '\t') {
			do {
				tag++;
			} while(*tag == ' ' || *tag == '\t');
			if(tag[0] == 'q' && tag[1] == 'b') {
				return TRUE;
			}
		}
	} 
	return FALSE;
}

void qb_set_engine_flags(qb_parser_context *cxt, uint32_t flags) {
	qb_function_declaration *f_decl = cxt->function_declaration;
	cxt->function_declaration->flags |= flags;
}

void qb_add_import(qb_parser_context *cxt, qb_token_position p) {
	qb_function_declaration *f_decl = cxt->function_declaration;
	f_decl->import_path = qb_allocate_string(cxt->pool, cxt->lexer_context->base + p.index, p.length);
	f_decl->import_path_length = p.length;
}

void qb_add_variable_declaration(qb_parser_context *cxt, uint32_t type) {
	qb_function_declaration *f_decl = cxt->function_declaration;
	qb_type_declaration *decl = qb_allocate_type_declaration(cxt->pool);
	qb_type_declaration **p = qb_enlarge_array((void **) &f_decl->declarations, 1);
	*p = cxt->type_declaration = decl;
	decl->flags |= type;
}

void qb_add_property_declaration(qb_parser_context *cxt, uint32_t type) {
	qb_class_declaration *c_decl = cxt->class_declaration;
	qb_type_declaration *decl = qb_allocate_type_declaration(cxt->pool);
	qb_type_declaration **p = qb_enlarge_array((void **) &c_decl->declarations, 1);
	*p = cxt->type_declaration = decl;
	if(cxt->property_name) {
		decl->name = cxt->property_name;
		decl->name_length = cxt->property_name_length;
		decl->hash_value = cxt->property_hash_value;
		if(cxt->property_flags & ZEND_ACC_STATIC) {
			decl->flags |= QB_VARIABLE_CLASS;
		} else {
			decl->flags |= QB_VARIABLE_CLASS_INSTANCE;
		}
	} else {
		decl->flags |= type;
	}
}

void qb_end_variable_declaration(qb_parser_context *cxt) {
	cxt->type_declaration = NULL;
}

void qb_set_variable_type(qb_parser_context *cxt, qb_primitive_type type, uint32_t flags) {
	qb_type_declaration *decl = cxt->type_declaration;
	decl->type = type;
	decl->flags |= flags;
}

void qb_add_dimension(qb_parser_context *cxt, uint32_t count, uint32_t flags) {
	qb_type_declaration *decl = cxt->type_declaration;
	if(decl->dimension_count < MAX_DIMENSION) {
		uint32_t index = decl->dimension_count++;
		decl->dimensions = qb_reallocate_indices(cxt->pool, decl->dimensions, index, decl->dimension_count);
		decl->dimensions[index] = count;
		if(decl->flags & QB_TYPE_DECL_HAS_ALIAS_SCHEMES) {
			decl->index_alias_schemes = qb_reallocate_pointers(cxt->pool, decl->index_alias_schemes, index, decl->dimension_count);
		}
		decl->flags |= flags;
	} else {
		qb_report_too_man_dimension_exception(cxt->line_id);
	}
}

void qb_add_index_alias_scheme(qb_parser_context *cxt, qb_index_alias_scheme *scheme) {
	qb_type_declaration *decl = cxt->type_declaration;
	uint32_t index = decl->dimension_count;
	decl->flags |= QB_TYPE_DECL_HAS_ALIAS_SCHEMES;
	qb_add_dimension(cxt, scheme->dimension, 0);
	decl->index_alias_schemes[index] = scheme;
}

void qb_attach_variable_name(qb_parser_context *cxt, qb_token_position p) {
	qb_type_declaration *decl = cxt->type_declaration;
	decl->name = qb_allocate_string(cxt->pool, cxt->lexer_context->base + p.index, p.length);
	decl->name_length = p.length;
	decl->hash_value = zend_hash_func(decl->name, decl->name_length + 1);
}

void qb_attach_variable_name_regexp(qb_parser_context *cxt, qb_token_position p) {
	qb_type_declaration *decl = cxt->type_declaration;
	char *constricted_pattern;
	const char *pcre_error = NULL;
	int pcre_error_offset = 0;
	ALLOCA_FLAG(use_heap);

	// put ^ at the beginning and $ at the end to force complete match
	constricted_pattern = do_alloca(p.length + 3, use_heap);
	constricted_pattern[0] = '^';
	memcpy(constricted_pattern + 1, cxt->lexer_context->base + p.index, p.length);
	constricted_pattern[1 + p.length] = '$';
	constricted_pattern[1 + p.length + 1] = '\0';

	decl->regexp = pcre_compile(constricted_pattern, 0, &pcre_error, &pcre_error_offset, NULL);
	free_alloca(constricted_pattern, use_heap);
	if(!decl->regexp) {
		// TODO report the error 
	}
}

uint32_t qb_parse_integer(qb_parser_context *cxt, qb_token_position p, uint32_t radix) {
	const char *s = cxt->lexer_context->base + p.index;
	char buffer[64]; 
	size_t len = p.length, i;	
	for(i = 0; i < p.length; i++) {
		if(*s >= '0' && *s <= '9') {
			break;
		}
		s++;
		len--;
	}
	if(len >= 0 && len < sizeof(buffer)) {
		memcpy(buffer, s, len);
		buffer[len] = '\0';
		return strtoul(buffer, NULL, radix);
	} else {
		return 0;
	}
}

void qb_attach_index_alias_scheme_class(qb_parser_context *cxt, qb_index_alias_scheme *scheme, qb_token_position p) {
	scheme->class_name = qb_allocate_string(cxt->pool, cxt->lexer_context->base + p.index, p.length);
	scheme->class_name_length = p.length;
}
	
qb_index_alias_scheme *qb_create_index_alias_scheme(qb_parser_context *cxt) {
	return qb_allocate_index_alias_scheme(cxt->pool);
}

void qb_add_index_alias(qb_parser_context *cxt, qb_index_alias_scheme *scheme, qb_token_position p) {
	uint32_t index = scheme->dimension++;
	scheme->aliases = qb_reallocate_pointers(cxt->pool, scheme->aliases, index, scheme->dimension);
	scheme->alias_lengths = qb_reallocate_indices(cxt->pool, scheme->alias_lengths, index, scheme->dimension);
	scheme->aliases[index] = qb_allocate_string(cxt->pool, cxt->lexer_context->base + p.index, p.length);
	scheme->alias_lengths[index] = p.length;
}

void qb_parse_constant(qb_parser_context *cxt, qb_token_position p) {
	if(cxt->lexer_context != &cxt->definition_lexer_context) {
		ALLOCA_FLAG(use_heap)
		char *name = do_alloca(p.length + 1, use_heap);
		uint32_t name_len = p.length;
		zval **p_value, *constant = NULL;
		TSRMLS_FETCH();

		memcpy(name, cxt->lexer_context->base + p.index, name_len);
		name[name_len] = '\0';

		if(cxt->zend_class && (zend_hash_find(&cxt->zend_class->constants_table, name, name_len + 1, (void **) &p_value) == SUCCESS)) {
			constant = *p_value;
		} else {
			zend_constant *zconst;
			if(zend_hash_find(EG(zend_constants), name, name_len + 1, (void **) &zconst) != FAILURE) {
				constant = &zconst->value;
			}
		}

		if(constant) {
			char *expanded;
			uint32_t expanded_len;
			int condition;

			switch (Z_TYPE_P(constant)) {
				case IS_NULL: {
					expanded_len = spprintf(&expanded, 0, "[]", Z_STRLEN_P(constant), Z_STRVAL_P(constant));
				}	break;
				case IS_STRING: {
					expanded_len = spprintf(&expanded, 0, "[%.*s]", Z_STRLEN_P(constant), Z_STRVAL_P(constant));
				}	break;
				case IS_BOOL: {
					if (Z_LVAL_P(constant)) {
						expanded_len = spprintf(&expanded, 0, "[1]");
					} else {
						expanded_len = spprintf(&expanded, 0, "[]");
					}
				}	break;
				case IS_RESOURCE:
				case IS_LONG: {
					expanded_len = spprintf(&expanded, 0, "[%ld]", Z_LVAL_P(constant));
				}	break;
				case IS_DOUBLE: {
					USE_TSRM
					expanded_len = spprintf(&expanded, 0, "[%.*G]", (int) EG(precision), Z_DVAL_P(constant));
				}	break;
			}

			// switch to a differrent lexer context
			condition = cxt->lexer_context->condition;
			cxt->lexer_context = &cxt->definition_lexer_context;
			cxt->lexer_context->base = cxt->lexer_context->cursor = expanded;
			cxt->lexer_context->token = cxt->lexer_context->marker = NULL;
			cxt->lexer_context->condition = condition;
		} else {
			qb_report_undefined_constant_exception(cxt->line_id, NULL, name);
		}
		free_alloca(name, use_heap);
	} else {
		// force the lexer to return a T_UNEXPECTED
		cxt->parser_selector = T_UNEXPECTED;
	}
}

int qb_doc_comment_yyerror(qb_parser_context *cxt, const char *msg) {
	// TODO: report error properly
	int32_t len = cxt->lexer_context->cursor - cxt->lexer_context->token;
	const char *token = cxt->lexer_context->token;
    php_printf("Error: %s near %.*s\n", msg, len, token);
	return 0;
}

int qb_scan_next_token(qb_lexer_context *l);

int qb_doc_comment_yylex(YYSTYPE *lvalp, qb_parser_context *cxt) {
	int ret;
	if(cxt->parser_selector) {
		ret = cxt->parser_selector;
		cxt->parser_selector = 0;
	} else {
		ret = qb_scan_next_token(cxt->lexer_context);
		lvalp->token.index = cxt->lexer_context->token - cxt->lexer_context->base;
		lvalp->token.length = cxt->lexer_context->cursor - cxt->lexer_context->token;
	}
	if(!ret) {
		if(cxt->lexer_context == &cxt->definition_lexer_context) {
			efree(cxt->lexer_context->base);
			cxt->lexer_context = &cxt->default_lexer_context;
			return qb_doc_comment_yylex(lvalp, cxt);
		}
	}
	return ret;
}

void qb_doc_comment_yyinit(qb_parser_context *cxt, const char *doc_comment, int parser_selector) {
	cxt->lexer_context = &cxt->default_lexer_context;
	cxt->lexer_context->base = cxt->lexer_context->cursor = doc_comment;
	cxt->lexer_context->token = cxt->lexer_context->marker = NULL;
	cxt->lexer_context->condition = yycINITIAL;
	cxt->parser_selector = parser_selector;
}

static void qb_find_doc_comment_line_number(qb_parser_context *cxt, const char *comment, uint32_t comment_length, uint32_t offset) {
	// load the file
	uint32_t line_number = 0;
	TSRMLS_FETCH();
	if(cxt->file_path) {
		php_stream *stream = php_stream_open_wrapper_ex((char *) cxt->file_path, "rb", 0, NULL, NULL);
		if(stream) {
			char *data = NULL;
			size_t data_len = php_stream_copy_to_mem(stream, &data, PHP_STREAM_COPY_ALL, FALSE);
			php_stream_close(stream);

			// find where the comment is
			if(data) {
				uint32_t current_line_number = 1;
				const char *p = data, *end = data + (data_len - comment_length);
				while(p < end) {
					if(*p == '\n' || (*p == '\r' && (*(p+1) != '\n'))) {
						current_line_number++;
						if(current_line_number >= cxt->line_number_max) {
							break;
						}
					} else {
						if(memcmp(p, comment, comment_length) == 0) {
							line_number = current_line_number;
						}
					}
					p++;
				}
				efree(data);
			}
		}
	}

	if(line_number) {
		const char *p = comment, *end = comment + offset;
		while(p < end) {
			if(*p == '\n' || (*p == '\r' && (*(p+1) != '\n'))) {
				line_number++;
			}
			p++;
		}
	} else {
		const char *p = comment + comment_length -1, *start = comment + offset;
		line_number = cxt->line_number_max;
		while(p >= start) {
			if(*p == '\n' || (*p == '\r' && (*(p+1) != '\n'))) {
				line_number--;
			}
			p--;
		}
	}
	cxt->line_id = LINE_ID(cxt->file_id, line_number);
}

static void qb_report_doc_comment_syntax_issue(qb_parser_context *cxt, const char *comment, uint32_t comment_length, int offset) {
	qb_find_doc_comment_line_number(cxt, comment, comment_length, offset);
	qb_report_doc_comment_syntax_exception(cxt->line_id);
}

qb_function_declaration * qb_parse_function_doc_comment(qb_parser_context *cxt, zend_op_array *op_array) {
	USE_TSRM
	qb_function_declaration *function_decl = NULL;

	function_decl = qb_allocate_function_declaration(cxt->pool);
	function_decl->zend_op_array = op_array;

	cxt->function_declaration = function_decl;
	cxt->file_path = op_array->filename;
	cxt->file_id = qb_get_source_file_id(cxt->file_path TSRMLS_CC);
	cxt->line_number_max = op_array->line_start;

	qb_doc_comment_yyinit(cxt, op_array->doc_comment, T_FUNCTION_SELECTOR);
	qb_doc_comment_yyparse(cxt);

	cxt->function_declaration = NULL;
	return function_decl;
}

qb_class_declaration * qb_parse_class_doc_comment(qb_parser_context *cxt, zend_class_entry *ce) {
	USE_TSRM
	qb_class_declaration *class_decl;
	uint32_t start_index = 0;
	const char *doc_comment = Z_CLASS_INFO(ce, doc_comment);
	Bucket *p;

	class_decl = qb_allocate_class_declaration(cxt->pool);
	class_decl->zend_class = ce;

	cxt->class_declaration = class_decl;
	cxt->file_path = Z_CLASS_INFO(ce, filename);
	cxt->file_id = qb_get_source_file_id(cxt->file_path TSRMLS_CC);

	if(doc_comment) {
		cxt->line_number_max = Z_CLASS_INFO(ce, line_start);
		cxt->property_name = NULL;
		cxt->property_name_length = 0;
		cxt->property_hash_value = 0;
		cxt->property_flags = 0;

		qb_doc_comment_yyinit(cxt, doc_comment, T_CLASS_SELECTOR);
		qb_doc_comment_yyparse(cxt);
	}

	// look for the doc comment of the properties start from the end of the class
	cxt->line_number_max = Z_CLASS_INFO(ce, line_end);

	for(p = ce->properties_info.pListHead; p; p = p->pListNext) {
		zend_property_info *prop = p->pData;
		if(prop->doc_comment) {
			// prop->name is null for private properties
			cxt->property_name = p->arKey;
			cxt->property_name_length = p->nKeyLength - 1;
			cxt->property_hash_value = p->h;
			cxt->property_flags = prop->flags;

			qb_doc_comment_yyinit(cxt, prop->doc_comment, T_PROPERTY_SELECTOR);
			qb_doc_comment_yyparse(cxt);
		}
	}

	cxt->class_declaration = NULL;
	return class_decl;
}

void qb_initialize_parser_context(qb_parser_context *cxt, qb_data_pool *pool, zend_class_entry *ce, const char *filename, uint32_t line_number TSRMLS_DC) {
	memset(cxt, 0, sizeof(qb_parser_context));
	cxt->pool = pool;
	cxt->zend_class = ce;

	SAVE_TSRMLS
}

void qb_free_parser_context(qb_parser_context *cxt) {
}
