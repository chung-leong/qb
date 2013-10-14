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

#define FOUND_GROUP(group)		(offsets[group * 2] != offsets[group * 2 + 1])
#define GROUP_OFFSET(group)		(offsets[group * 2])
#define GROUP_LENGTH(group)		(offsets[group * 2 + 1] - offsets[group * 2])

#define DOC_COMMENT_FUNCTION_REGEXP	"\\*\\s*@(?:(engine)|(import)|(param)|(local)|(shared)|(static|staticvar)|(global)|(var)|(property)|(return))\\s+(.*?)\\s*(?:\\*+\\/)?$"

enum {
	FUNC_DECL_ENGINE = 1,
	FUNC_DECL_IMPORT,
	FUNC_DECL_PARAM,
	FUNC_DECL_LOCAL,
	FUNC_DECL_SHARED,
	FUNC_DECL_STATIC,
	FUNC_DECL_GLOBAL,
	FUNC_DECL_VAR,
	FUNC_DECL_PROPERTY,
	FUNC_DECL_RETURN,
	FUNC_DECL_DATA,

	FUNC_DECL_TOKEN_COUNT,
};

ulong func_decl_hashes[FUNC_DECL_TOKEN_COUNT];

#define TYPE_DECL_REGEXP			"^\\s*(?:(?:(u?int(\\d*))|(float(\\d*)))|(void)|(integer)|(double)|(char)|(bool|boolean)|(string)|(image[134]?)|(array)|(object)|(resource)|(mixed)|(callback))\\s*((?:\\[.*?\\])*)\\s*"

enum {
	TYPE_DECL_INT = 1,
	TYPE_DECL_INT_WIDTH,
	TYPE_DECL_FLOAT,
	TYPE_DECL_FLOAT_WIDTH,
	TYPE_DECL_VOID,
	TYPE_DECL_INTEGER,
	TYPE_DECL_DOUBLE,
	TYPE_DECL_CHAR,
	TYPE_DECL_BOOLEAN,
	TYPE_DECL_STRING,
	TYPE_DECL_IMAGE,
	TYPE_DECL_ARRAY,
	TYPE_DECL_OBJECT,
	TYPE_DECL_RESOURCE,
	TYPE_DECL_MIXED,
	TYPE_DECL_CALLBACK,
	TYPE_DECL_DIMENSIONS,
};

#define TYPE_DIM_REGEXP				"\\[\\s*(?:(0x[0-9a-f]+|\\d*)|([A-Z_][A-Z0-9_]*)|(\\*?)|(\\\?\?))\\s*\\]\\s*"

enum {
	TYPE_DIM_INT = 1,
	TYPE_DIM_CONSTANT,
	TYPE_DIM_ASTERISK,
	TYPE_DIM_QUESTION_MARK,
};

#define TYPE_DIM_ALIAS_REGEXP		"\\[\\s*(?:(\\w+)\\s*:)?\\s*((?:[A-Z_][A-Z0-9_]*\\s*,\\s*)+[A-Z_][A-Z0-9_]*)\\s*\\]"

enum {
	TYPE_DIM_CLASS = 1,
	TYPE_DIM_NAMES,
};

#define IDENTIFIER_REGEXP			"^\\$(?:(\\w+)|(\\S+))"

enum {
	ID_NORMAL = 1,
	ID_PATTERN = 2,
};

pcre *doc_comment_function_regexp;
pcre *type_decl_regexp;
pcre *type_dim_regexp;
pcre *type_dim_alias_regexp;
pcre *identifier_regexp;

static void qb_notice_doc_comment_issue(qb_parser_context *cxt, const char *format, ... ) {
	va_list args;
	va_start(args, format);
	zend_error_cb(E_NOTICE, cxt->file_path, cxt->line_number, format, args);
	va_end(args);
}

static zend_always_inline void qb_add_variable_declaration(qb_function_declaration *function_decl, qb_type_declaration *var_decl) {
	qb_type_declaration **p = qb_enlarge_array((void **) &function_decl->declarations, 1);
	*p = var_decl;
}

static zend_always_inline void qb_add_class_variable_declaration(qb_class_declaration *class_decl, qb_type_declaration *var_decl) {
	qb_type_declaration **p = qb_enlarge_array((void **) &class_decl->declarations, 1);
	*p = var_decl;
}

static int32_t qb_parse_type_dimension(qb_parser_context *cxt, const char *s, uint32_t len, qb_type_declaration *decl, uint32_t dimension_index) {
	int offsets[64], matches;
	uint32_t dimension = 0;

	matches = pcre_exec(type_dim_regexp, NULL, s, len, 0, 0, offsets, sizeof(offsets) / sizeof(int));
	if(matches > 0) {
		if(FOUND_GROUP(TYPE_DIM_INT)) {
			const char *number = s + GROUP_OFFSET(TYPE_DIM_INT);
			dimension = strtol(number, NULL, 0);
		} else if(FOUND_GROUP(TYPE_DIM_CONSTANT)) {
			zval **p_value, *constant = NULL;
			uint32_t name_len = GROUP_LENGTH(TYPE_DIM_CONSTANT);
			ALLOCA_FLAG(use_heap)
			char *name = do_alloca(name_len + 1, use_heap);
			TSRMLS_FETCH();
			memcpy(name, s + GROUP_OFFSET(TYPE_DIM_CONSTANT), name_len);
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
				if(Z_TYPE_P(constant) == IS_LONG) {
					long const_value = Z_LVAL_P(constant);
					if(const_value <= 0) {
						qb_notice_doc_comment_issue(cxt, "Constant '%s' is not a positive integer", name);
						return -1;
					}
					dimension = const_value;
				} else if(Z_TYPE_P(constant) == IS_STRING) {
					char *expanded;
					uint32_t expanded_len = spprintf(&expanded, 0, "[%.*s]", Z_STRLEN_P(constant), Z_STRVAL_P(constant));
					int32_t processed = qb_parse_type_dimension(cxt, expanded, expanded_len, decl, dimension_index);
					efree(expanded);
					free_alloca(name, use_heap);
					return (processed == -1) ? -1 : offsets[1];
				} else {
					decl = NULL;
				}
			} else {
				qb_notice_doc_comment_issue(cxt, "Undefined constant '%s'", name);
				return -1;
			}
			free_alloca(name, use_heap);
		} else if(FOUND_GROUP(TYPE_DIM_ASTERISK)) {
			if(dimension_index == 0) {
				decl->flags |= QB_TYPE_DECL_EXPANDABLE;
			}
			dimension = 0;
		} else {
			dimension = 0;
		}
	} else {
		matches = pcre_exec(type_dim_alias_regexp, NULL, s, len, 0, 0, offsets, sizeof(offsets) / sizeof(int));
		if(matches > 0 && FOUND_GROUP(TYPE_DIM_NAMES)) {
			const char *names = s + GROUP_OFFSET(TYPE_DIM_NAMES), *class_name = s + GROUP_OFFSET(TYPE_DIM_CLASS);
			uint32_t names_len = GROUP_LENGTH(TYPE_DIM_NAMES), class_name_len = GROUP_LENGTH(TYPE_DIM_CLASS);
			uint32_t alias_count, alias_start, alias_len, i;
			qb_index_alias_scheme *scheme;						

			// count the number of commas
			for(i = 0, dimension = 1; i < names_len; i++) {
				if(names[i] == ',') {
					dimension++;
				}
			}
			scheme = qb_allocate_index_alias_scheme(cxt->pool);
			scheme->aliases = qb_allocate_pointers(cxt->pool, dimension);
			scheme->alias_lengths = qb_allocate_indices(cxt->pool, dimension);
			scheme->dimension = dimension;
			// divide the string into substrings
			for(i = 0, alias_count = 0, alias_start = 0, alias_len = 0; i <= names_len; i++) {
				if(names[i] == ' ' || names[i] == '\t' || names[i] == '\n' || names[i] == '\r' || names[i] == ',' || i == names_len) {
					if(alias_len > 0) {
						scheme->aliases[alias_count] = qb_allocate_string(cxt->pool, names + alias_start, alias_len);
						scheme->alias_lengths[alias_count] = alias_len;
						alias_count++;
						alias_len = 0;
					}
					alias_start = i + 1;
				} else {
					alias_len++;
				}
			}
			if(class_name_len) {
				scheme->class_name = qb_allocate_string(cxt->pool, class_name, class_name_len);
				scheme->class_name_length = class_name_len;
			}
			if(!decl->index_alias_schemes) {
				decl->index_alias_schemes = qb_allocate_pointers(cxt->pool, decl->dimension_count);
				memset(decl->index_alias_schemes, 0, sizeof(qb_index_alias_scheme *) * decl->dimension_count);
			}
			decl->dimensions[dimension_index] = dimension;
			decl->index_alias_schemes[dimension_index] = scheme;
			decl->flags |= QB_TYPE_DECL_HAS_ALIAS_SCHEMES;
		} else {
			return -1;
		}
	}
	decl->dimensions[dimension_index] = dimension;
	return offsets[1];
}

static qb_type_declaration * qb_parse_type_declaration(qb_parser_context *cxt, const char *s, uint32_t len, uint32_t var_type) {
	qb_type_declaration *decl = NULL;
	int offsets[64], matches;

	matches = pcre_exec(type_decl_regexp, NULL, s, len, 0, 0, offsets, sizeof(offsets) / sizeof(int));
	if(matches != -1) {
		uint32_t end_index = offsets[1];

		decl = qb_allocate_type_declaration(cxt->pool);
		decl->flags = var_type;

		if(FOUND_GROUP(TYPE_DECL_INT)) {
			const char *inttype = s + GROUP_OFFSET(TYPE_DECL_INT);
			const char *width = s + GROUP_OFFSET(TYPE_DECL_INT_WIDTH);
			uint32_t width_len = GROUP_LENGTH(TYPE_DECL_INT_WIDTH);
			uint32_t sign_flag = (inttype[0] == 'u') ? QB_TYPE_UNSIGNED : 0;
			if(width_len == 2 && width[0] == '6' && width[1] == '4') {
				decl->type = QB_TYPE_I64 | sign_flag;
			} else if(width_len == 2 && width[0] == '3' && width[1] == '2') {
				decl->type = QB_TYPE_I32 | sign_flag;
			} else if(width_len == 2 && width[0] == '1' && width[1] == '6') {
				decl->type = QB_TYPE_I16 | sign_flag;
			} else if(width_len == 1 && width[0] == '8') {
				decl->type = QB_TYPE_I08 | sign_flag;
			} else if(width_len == 0) {
				decl->type = QB_TYPE_I32;
			} else {
				return NULL;
			}
		} else if(FOUND_GROUP(TYPE_DECL_FLOAT)) {
			const char *width = s + GROUP_OFFSET(TYPE_DECL_FLOAT_WIDTH);
			uint32_t width_len = GROUP_LENGTH(TYPE_DECL_FLOAT_WIDTH);
			if(width_len == 2 && width[0] == '6' && width[1] == '4') {
				decl->type = QB_TYPE_F64;
			} else if(width_len == 2 && width[0] == '3' && width[1] == '2') {
				decl->type = QB_TYPE_F32;
			} else if(width_len == 0) {
				decl->type = QB_TYPE_F32;
			} else {
				return NULL;
			}
		} else if(FOUND_GROUP(TYPE_DECL_VOID)) {
			decl->type = QB_TYPE_VOID;
		} else if(FOUND_GROUP(TYPE_DECL_INTEGER)) {
			decl->type = QB_TYPE_I32;
		} else if(FOUND_GROUP(TYPE_DECL_DOUBLE)) {
			decl->type = QB_TYPE_F64;
		} else if(FOUND_GROUP(TYPE_DECL_CHAR)) {
			decl->type = QB_TYPE_U08;
			decl->flags |= QB_TYPE_DECL_STRING;
		} else if(FOUND_GROUP(TYPE_DECL_BOOLEAN)) {
			decl->type = QB_TYPE_I32;
		} else if(FOUND_GROUP(TYPE_DECL_STRING) && !FOUND_GROUP(TYPE_DECL_DIMENSIONS)) {
			decl->type = QB_TYPE_U08;
			decl->dimension_count = 1;
			decl->dimensions = qb_allocate_indices(cxt->pool, 1);
			decl->dimensions[0] = 0;
			decl->flags |= QB_TYPE_DECL_EXPANDABLE | QB_TYPE_DECL_STRING;
		} else if(FOUND_GROUP(TYPE_DECL_IMAGE) && !FOUND_GROUP(TYPE_DECL_DIMENSIONS)) {
			uint32_t identifier_len = GROUP_LENGTH(TYPE_DECL_IMAGE);
			decl->type = QB_TYPE_F32;
			decl->dimension_count = 3;
			decl->dimensions = qb_allocate_indices(cxt->pool, 3);
			decl->dimensions[0] = 0;
			decl->dimensions[1] = 0;
			if(identifier_len == 6) {
				const char *identifier = s + GROUP_OFFSET(TYPE_DECL_IMAGE);
				decl->dimensions[2] = identifier[identifier_len - 1] - '0';
			} else {
				decl->dimensions[2] = 4;
			}
		} else {
			return NULL;
		}

		if(FOUND_GROUP(TYPE_DECL_DIMENSIONS)) {
			uint32_t dimension_count = 0, i;
			uint32_t dimension_len = GROUP_LENGTH(TYPE_DECL_DIMENSIONS);
			const char *dimension_string = s + GROUP_OFFSET(TYPE_DECL_DIMENSIONS);

			// see how many brackets there are
			for(i = 0; i < dimension_len; i++) {
				if(dimension_string[i] == '[') {
					dimension_count++;
				}
			}

			decl->dimensions = qb_allocate_indices(cxt->pool, dimension_count);
			decl->dimension_count = dimension_count;

			for(i = 0; i < dimension_count; i++) {
				int32_t processed = qb_parse_type_dimension(cxt, dimension_string, dimension_len, decl, i);
				if(processed == -1) {
					return NULL;
				}
				dimension_string += processed;
				dimension_len -= processed;
			}
		}

		if(var_type && var_type != QB_VARIABLE_RETURN_VALUE) {
			const char *identifier = s + end_index;
			uint32_t identifier_len = len - end_index;
			matches = pcre_exec(identifier_regexp, NULL, identifier, identifier_len, 0, 0, offsets, sizeof(offsets) / sizeof(int));
			if(FOUND_GROUP(ID_NORMAL)) {
				const char *name = identifier + GROUP_OFFSET(ID_NORMAL);
				uint32_t name_len = GROUP_LENGTH(ID_NORMAL);
				decl->name = qb_allocate_string(cxt->pool, name, name_len);
				decl->name_length = name_len;
				decl->hash_value = zend_hash_func(decl->name, name_len + 1);
			} else if(FOUND_GROUP(ID_PATTERN)) {
				// string contains special characters--try to compile it as a regexp
				const char *pattern = identifier + GROUP_OFFSET(ID_PATTERN);
				uint32_t pattern_len = GROUP_LENGTH(ID_PATTERN);
				const char *pcre_error = NULL;
				int pcre_error_offset = 0;
				pcre *re;
				char *constricted_pattern;
				ALLOCA_FLAG(use_heap);

				// put ^ at the beginning and $ at the end to force complete match
				constricted_pattern = do_alloca(pattern_len + 3, use_heap);
				constricted_pattern[0] = '^';
				memcpy(constricted_pattern + 1, pattern, pattern_len);
				constricted_pattern[1 + pattern_len] = '$';
				constricted_pattern[1 + pattern_len + 1] = '\0';

				re = pcre_compile(constricted_pattern, 0, &pcre_error, &pcre_error_offset, NULL);
				free_alloca(constricted_pattern, use_heap);
				if(!re) {
					return NULL;
				}
				decl->regexp = re;
			} else {
				return NULL;
			}
		}
	}
	return decl;
}

static void qb_find_doc_comment_line_number(qb_parser_context *cxt, const char *comment, uint32_t comment_length, uint32_t offset) {
	// load the file
	uint32_t line_number_max = cxt->line_number;
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
						if(current_line_number >= line_number_max) {
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
		line_number = line_number_max;
		while(p >= start) {
			if(*p == '\n' || (*p == '\r' && (*(p+1) != '\n'))) {
				line_number--;
			}
			p--;
		}
	}
	cxt->line_number = line_number;
}

static void qb_notice_doc_comment_unexpected_tag(qb_parser_context *cxt, const char *comment, uint32_t comment_length, int matches, int *offsets) {
	int i;
	TSRMLS_FETCH();
	for(i = 1; i < matches; i++) {
		if(FOUND_GROUP(i)) {
			uint32_t tag_len = GROUP_LENGTH(i);
			uint32_t start_index = GROUP_OFFSET(i);
			const char *tag = comment + start_index;

			qb_find_doc_comment_line_number(cxt, comment, comment_length, start_index);
			qb_notice_doc_comment_issue(cxt, "Unexpected use of @%.*s", tag_len, tag);
			break;
		}
	}
}

static void qb_notice_doc_comment_syntax_issue(qb_parser_context *cxt, const char *comment, uint32_t comment_length, uint32_t offset) {
	qb_find_doc_comment_line_number(cxt, comment, comment_length, offset);
	qb_notice_doc_comment_issue(cxt, "Syntax error encountered while parsing Doc Comments for type information");
}

qb_function_declaration * qb_parse_function_declaration_table(qb_parser_context *cxt, HashTable *ht) {
	qb_function_declaration *function_decl = qb_allocate_function_declaration(cxt->pool);
	Bucket *p;

	for(p = ht->pListHead; p; p = p->pListNext) {
		if(p->nKeyLength) {
			zval **p_element = p->pData, *element = *p_element;
			ulong h = zend_hash_func(p->arKey, p->nKeyLength);
			uint32_t i, match = 0;
			for(i = 1; i < sizeof(func_decl_hashes) / sizeof(ulong); i++) {
				if(func_decl_hashes[i] == h) {
					match = i;
					break;
				}
			}
			if(match) {
				qb_type_declaration *decl;
				zval *error_zval = NULL;

				if(Z_TYPE_P(element) == IS_STRING) {
					const char *data = Z_STRVAL_P(element);
					uint32_t data_len = Z_STRLEN_P(element);
					switch(match) {
						case FUNC_DECL_ENGINE: {
							if(data_len == 2 && strncmp(data, "qb", 2) == 0) {
							} else if(data_len == 11 && strncmp(data, "qb-bytecode", 11) == 0) {
								function_decl->flags |= QB_FUNCTION_NEVER_NATIVE;
							} else if(data_len == 9 && strncmp(data, "qb-native", 9) == 0) {
								function_decl->flags |= QB_FUNCTION_NATIVE_IF_POSSIBLE;
							} else {
								error_zval = element;
							}
						}	break;
						case FUNC_DECL_IMPORT: {
						}	break;
						case FUNC_DECL_RETURN: {
							decl = qb_parse_type_declaration(cxt, data, data_len, QB_VARIABLE_RETURN_VALUE);
							if(decl) {
								qb_add_variable_declaration(function_decl, decl);
							} else {
								error_zval = element;
							}
						}	break;
						default:
							error_zval = element;
					}
				} else if(Z_TYPE_P(element) == IS_ARRAY) {
					HashTable *value_ht = Z_ARRVAL_P(element);
					Bucket *q;
					uint32_t var_type = 0;
					switch(match) {
						case FUNC_DECL_PARAM: var_type = QB_VARIABLE_ARGUMENT; break;
						case FUNC_DECL_LOCAL: var_type = QB_VARIABLE_LOCAL; break;
						case FUNC_DECL_STATIC: var_type = QB_VARIABLE_STATIC | QB_VARIABLE_CLASS; break;
						case FUNC_DECL_GLOBAL: var_type = QB_VARIABLE_GLOBAL; break;
						case FUNC_DECL_PROPERTY: var_type = QB_VARIABLE_CLASS_INSTANCE; break;
						default:
							error_zval = element;
					}
					if(var_type) {
						for(q = value_ht->pListHead; q; q = q->pListNext) {
							zval **p_inner_element = q->pData, *inner_element = *p_inner_element;
							if(Z_TYPE_P(inner_element) == IS_STRING) {
								const char *data = Z_STRVAL_P(inner_element);
								uint32_t data_len = Z_STRLEN_P(inner_element);
								decl = qb_parse_type_declaration(cxt, data, data_len, var_type);
								if(decl) {
									qb_add_variable_declaration(function_decl, decl);
								} else {
									error_zval = inner_element;
									break;
								}
							} else {
								error_zval = inner_element;
							}
						}
					}
				}
				if(error_zval) {
					if(Z_TYPE_P(error_zval) == IS_STRING) {
						qb_notice_doc_comment_issue(cxt, "Error encountered while processing type information: '%s'", Z_STRVAL_P(error_zval));
					} else {
						qb_notice_doc_comment_issue(cxt, "Unexpected %s encountered while processing type information", zend_get_type_by_const(Z_TYPE_P(error_zval)));
					}
				}
			} else {
				qb_notice_doc_comment_issue(cxt, "Unknown keyword '%s' encountered while processing type information", p->arKey);
			}
		} else {
			qb_notice_doc_comment_issue(cxt, "Numeric key encountered where a string is expected");
		}
	}
	return function_decl;
}

qb_function_declaration * qb_parse_function_doc_comment(qb_parser_context *cxt, const char *doc_comment, size_t doc_comment_len) {
	qb_function_declaration *function_decl = NULL;
	int offsets[48], matches;
	uint32_t start_index = 0;
	int32_t use_qb = FALSE;

	function_decl = qb_allocate_function_declaration(cxt->pool);

	for(start_index = 0; start_index < doc_comment_len; start_index = offsets[1]) {
		matches = pcre_exec(doc_comment_function_regexp, NULL, doc_comment, doc_comment_len, start_index, 0, offsets, sizeof(offsets) / sizeof(int));
		if(matches > 0) {
			uint32_t data_offset = GROUP_OFFSET(FUNC_DECL_DATA), data_len = GROUP_LENGTH(FUNC_DECL_DATA), var_type;
			const char *data = doc_comment + data_offset;
			qb_type_declaration *decl;

			if(FOUND_GROUP(FUNC_DECL_ENGINE)) {
				if(data_len == 2 && strncmp(data, "qb", 2) == 0) {
					use_qb = TRUE;
				} else if(data_len == 11 && strncmp(data, "qb-bytecode", 11) == 0) {
					use_qb = TRUE;
					function_decl->flags |= QB_FUNCTION_NEVER_NATIVE;
				} else if(data_len == 9 && strncmp(data, "qb-native", 9) == 0) {
					use_qb = TRUE;
					function_decl->flags |= QB_FUNCTION_NATIVE_IF_POSSIBLE;
				}
			} else if(FOUND_GROUP(FUNC_DECL_IMPORT)) {
				function_decl->import_path = qb_allocate_string(cxt->pool, data, data_len);
				function_decl->import_path_length = data_len;
			} else {
				if(FOUND_GROUP(FUNC_DECL_GLOBAL)) {
					var_type = QB_VARIABLE_GLOBAL;
				} else if(FOUND_GROUP(FUNC_DECL_LOCAL)) {
					var_type = QB_VARIABLE_LOCAL;
				} else if(FOUND_GROUP(FUNC_DECL_PARAM)) {
					var_type = QB_VARIABLE_ARGUMENT;
				} else if(FOUND_GROUP(FUNC_DECL_STATIC)) {
					var_type = QB_VARIABLE_STATIC | QB_VARIABLE_CLASS;
				} else if(FOUND_GROUP(FUNC_DECL_PROPERTY)) {
					var_type = FUNC_DECL_PROPERTY;
				} else if(FOUND_GROUP(FUNC_DECL_RETURN)) {
					var_type = QB_VARIABLE_RETURN_VALUE;
				} else {
					if(use_qb) {
						qb_notice_doc_comment_unexpected_tag(cxt, doc_comment, doc_comment_len, matches, offsets);
					}
				}
				decl = qb_parse_type_declaration(cxt, data, data_len, var_type);
				if(decl) {
					qb_add_variable_declaration(function_decl, decl);
				} else {
					if(use_qb) {
						qb_notice_doc_comment_syntax_issue(cxt, doc_comment, doc_comment_len, data_offset);
					}
				}
			}
		} else {
			break;
		}
	}
	if(!use_qb) {
		function_decl = NULL;
	}
	return function_decl;
}

qb_class_declaration * qb_parse_class_doc_comment(qb_parser_context *cxt, zend_class_entry *ce) {
	qb_class_declaration *class_decl;
	const char *doc_comment = Z_CLASS_INFO(ce, doc_comment);
	size_t doc_comment_len = Z_CLASS_INFO(ce, doc_comment_len);
	int offsets[48], matches;
	uint32_t start_index = 0;
	Bucket *p;

	cxt->file_path = Z_CLASS_INFO(ce, filename);
	cxt->line_number = Z_CLASS_INFO(ce, line_start);
	class_decl = qb_allocate_class_declaration(cxt->pool);
	class_decl->zend_class = ce;

	for(start_index = 0; start_index < doc_comment_len; start_index = offsets[1]) {
		matches = pcre_exec(doc_comment_function_regexp, NULL, doc_comment, doc_comment_len, start_index, 0, offsets, sizeof(offsets) / sizeof(int));
		if(matches > 0) {
			uint32_t data_offset = GROUP_OFFSET(FUNC_DECL_DATA), data_len = GROUP_LENGTH(FUNC_DECL_DATA), var_type = 0;
			const char *data = doc_comment + data_offset;
			qb_type_declaration *decl;

			if(FOUND_GROUP(FUNC_DECL_PROPERTY)) {
				var_type = QB_VARIABLE_CLASS_INSTANCE;
			} else if(FOUND_GROUP(FUNC_DECL_STATIC)) {
				var_type = QB_VARIABLE_CLASS;
			} else {
				qb_notice_doc_comment_unexpected_tag(cxt, doc_comment, doc_comment_len, matches, offsets);
			} 
			if(var_type) {
				decl = qb_parse_type_declaration(cxt, data, data_len, var_type);
				if(decl) {
					qb_add_class_variable_declaration(class_decl, decl);
				} else {
					qb_notice_doc_comment_syntax_issue(cxt, doc_comment, doc_comment_len, data_offset);
				}
			}
		} else {
			break;
		}
	}

	for(p = ce->properties_info.pListHead; p; p = p->pListNext) {
		zend_property_info *prop = p->pData;
		const char *doc_comment = prop->doc_comment;
		size_t doc_comment_len = prop->doc_comment_len;
		int offsets[48], matches;
		uint32_t start_index = 0;

		if(doc_comment) {
			for(start_index = 0; start_index < doc_comment_len; start_index = offsets[1]) {
				matches = pcre_exec(doc_comment_function_regexp, NULL, doc_comment, doc_comment_len, start_index, 0, offsets, sizeof(offsets) / sizeof(int));
				if(matches > 0) {
					uint32_t data_offset = GROUP_OFFSET(FUNC_DECL_DATA), data_len = GROUP_LENGTH(FUNC_DECL_DATA);
					const char *data = doc_comment + data_offset;
					qb_type_declaration *decl;

					if(FOUND_GROUP(FUNC_DECL_VAR)) {
						decl = qb_parse_type_declaration(cxt, data, data_len, 0);
						if(decl) {
							decl->name = p->arKey;
							decl->name_length = p->nKeyLength - 1;
							decl->hash_value = p->h;
							if(prop->flags & ZEND_ACC_STATIC) {
								decl->flags |= QB_VARIABLE_CLASS;
							} else {
								decl->flags |= QB_VARIABLE_CLASS_INSTANCE;
							}
							qb_add_class_variable_declaration(class_decl, decl);
						} else {
							qb_notice_doc_comment_syntax_issue(cxt, doc_comment, doc_comment_len, data_offset);
						}
					} else {
						qb_notice_doc_comment_unexpected_tag(cxt, doc_comment, doc_comment_len, matches, offsets);
					}
				} else {
					break;
				}
			}
		}
	}

	return class_decl;
}

void qb_initialize_parser_context(qb_parser_context *cxt, qb_data_pool *pool, zend_class_entry *ce, const char *filename, uint32_t line_number TSRMLS_DC) {
	static int regexp_created = FALSE;
	if(!regexp_created) {
		const char *pcre_error = NULL;
		int pcre_error_offset = 0;

		// compile regular expressions
		doc_comment_function_regexp = pcre_compile(DOC_COMMENT_FUNCTION_REGEXP, PCRE_MULTILINE, &pcre_error, &pcre_error_offset, NULL);
		type_decl_regexp = pcre_compile(TYPE_DECL_REGEXP, 0, &pcre_error, &pcre_error_offset, NULL);
		type_dim_regexp = pcre_compile(TYPE_DIM_REGEXP, PCRE_CASELESS, &pcre_error, &pcre_error_offset, NULL);
		type_dim_alias_regexp = pcre_compile(TYPE_DIM_ALIAS_REGEXP, PCRE_CASELESS, &pcre_error, &pcre_error_offset, NULL);
		identifier_regexp = pcre_compile(IDENTIFIER_REGEXP, 0, &pcre_error, &pcre_error_offset, NULL);
		regexp_created = TRUE;

		// calculate hashes of tokens
		func_decl_hashes[FUNC_DECL_ENGINE] = zend_hash_func("engine", 7);
		func_decl_hashes[FUNC_DECL_IMPORT] = zend_hash_func("import", 7);
		func_decl_hashes[FUNC_DECL_PARAM] = zend_hash_func("param", 6);
		func_decl_hashes[FUNC_DECL_LOCAL] = zend_hash_func("local", 6);
		func_decl_hashes[FUNC_DECL_SHARED] = zend_hash_func("shared", 7);
		func_decl_hashes[FUNC_DECL_STATIC] = zend_hash_func("static", 7);
		func_decl_hashes[FUNC_DECL_GLOBAL] = zend_hash_func("global", 7);
		func_decl_hashes[FUNC_DECL_VAR] = zend_hash_func("var", 4);
		func_decl_hashes[FUNC_DECL_PROPERTY] = zend_hash_func("property", 9);
		func_decl_hashes[FUNC_DECL_RETURN] = zend_hash_func("return", 7);
	}

	cxt->pool = pool;
	cxt->zend_class = NULL;
	cxt->file_path = filename;
	cxt->line_number = line_number;
}

void qb_free_parser_context(qb_parser_context *cxt) {
}