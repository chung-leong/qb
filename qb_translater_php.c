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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "qb.h"
#include "qb_translater_php.h"

extern const char compressed_table_zend_op_names[];

const char * ZEND_FASTCALL qb_get_zend_op_name(qb_php_translater_context *cxt, uint32_t opcode) {
	if(!cxt->pool->zend_op_names) {
		qb_uncompress_table(compressed_table_zend_op_names, (void ***) &cxt->pool->zend_op_names, NULL, 0);
		if(!cxt->pool->zend_op_names) {
			return "?";
		}
	}
	return cxt->pool->zend_op_names[opcode];
}

void ZEND_FASTCALL qb_print_zend_ops(qb_php_translater_context *cxt) {
	uint32_t i = 0;
	for(i = 0; i < cxt->zend_op_array->last; i++) {
		zend_op *zop = &cxt->zend_op_array->opcodes[i];
		const char *opname = qb_get_zend_op_name(cxt, zop->opcode);
		php_printf("[%04d] %s (line number: %d)\n", i, opname, zop->lineno);
	}
}

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
};

ulong func_decl_hashes[10];

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

static int32_t ZEND_FASTCALL qb_parse_type_dimension(qb_compiler_data_pool *pool, const char *s, uint32_t len, qb_type_declaration *decl, uint32_t dimension_index, zend_class_entry *ce) {
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

			if(ce && (zend_hash_find(&ce->constants_table, name, name_len + 1, (void **) &p_value) == SUCCESS)) {
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
						qb_abort("constant '%s' is not a positive integer", name);
					}
					dimension = const_value;
				} else if(Z_TYPE_P(constant) == IS_STRING) {
					char *expanded;
					uint32_t expanded_len = spprintf(&expanded, 0, "[%.*s]", Z_STRLEN_P(constant), Z_STRVAL_P(constant));
					int32_t processed = qb_parse_type_dimension(pool, expanded, expanded_len, decl, dimension_index, ce);
					efree(expanded);
					free_alloca(name, use_heap);
					return (processed == -1) ? -1 : offsets[1];
				} else {
					decl = NULL;
				}
			} else {
				qb_abort("undefined constant '%s'", name);
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
			scheme = qb_allocate_index_alias_scheme(pool);
			scheme->aliases = qb_allocate_pointers(pool, dimension);
			scheme->alias_lengths = qb_allocate_indices(pool, dimension);
			scheme->dimension = dimension;
			// divide the string into substrings
			for(i = 0, alias_count = 0, alias_start = 0, alias_len = 0; i <= names_len; i++) {
				if(names[i] == ' ' || names[i] == '\t' || names[i] == '\n' || names[i] == '\r' || names[i] == ',' || i == names_len) {
					if(alias_len > 0) {
						scheme->aliases[alias_count] = qb_allocate_string(pool, names + alias_start, alias_len);
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
				scheme->class_name = qb_allocate_string(pool, class_name, class_name_len);
				scheme->class_name_length = class_name_len;
			}
			if(!decl->index_alias_schemes) {
				decl->index_alias_schemes = qb_allocate_pointers(pool, decl->dimension_count);
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

static qb_type_declaration * ZEND_FASTCALL qb_parse_type_declaration(qb_compiler_data_pool *pool, const char *s, uint32_t len, uint32_t var_type, zend_class_entry *ce) {
	qb_type_declaration *decl = NULL;
	int offsets[64], matches;

	matches = pcre_exec(type_decl_regexp, NULL, s, len, 0, 0, offsets, sizeof(offsets) / sizeof(int));
	if(matches != -1) {
		uint32_t end_index = offsets[1];

		decl = qb_allocate_type_declaration(pool);
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
			decl->dimensions = qb_allocate_indices(pool, 1);
			decl->dimensions[0] = 0;
			decl->flags |= QB_TYPE_DECL_EXPANDABLE | QB_TYPE_DECL_STRING;
		} else if(FOUND_GROUP(TYPE_DECL_IMAGE) && !FOUND_GROUP(TYPE_DECL_DIMENSIONS)) {
			uint32_t identifier_len = GROUP_LENGTH(TYPE_DECL_IMAGE);
			decl->type = QB_TYPE_F32;
			decl->dimension_count = 3;
			decl->dimensions = qb_allocate_indices(pool, 3);
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

			decl->dimensions = qb_allocate_indices(pool, dimension_count);
			decl->dimension_count = dimension_count;

			for(i = 0; i < dimension_count; i++) {
				int32_t processed = qb_parse_type_dimension(pool, dimension_string, dimension_len, decl, i, ce);
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
				decl->name = qb_allocate_string(pool, name, name_len);
				decl->name_length = name_len;
				decl->hash_value = zend_inline_hash_func(decl->name, name_len + 1);
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

static uint32_t qb_find_doc_comment_line_number(const char *comment, uint32_t comment_length, uint32_t offset, const char *filepath, uint32_t line_number_max) {
	// load the file
	uint32_t line_number = 0;
	TSRMLS_FETCH();
	if(filepath) {
		php_stream *stream = php_stream_open_wrapper_ex((char *) filepath, "rb", 0, NULL, NULL);
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
	return line_number;
}

static void qb_abort_doc_comment_unexpected_error(const char *comment, uint32_t comment_length, int matches, int *offsets, const char *filepath, uint32_t line_number_max) {
	int i;
	TSRMLS_FETCH();
	for(i = 1; i < matches; i++) {
		if(FOUND_GROUP(i)) {
			uint32_t tag_len = GROUP_LENGTH(i);
			uint32_t start_index = GROUP_OFFSET(i);
			uint32_t line_number = qb_find_doc_comment_line_number(comment, comment_length, start_index, filepath, line_number_max);
			const char *tag = comment + start_index;

			QB_G(current_filename) = filepath;
			QB_G(current_line_number) = line_number;
			qb_abort("Unexpected use of @%.*s", tag_len, tag);
		}
	}
}

static void qb_abort_doc_comment_syntax_error(const char *comment, uint32_t comment_length, uint32_t offset, const char *filepath, uint32_t line_number_max) {
	uint32_t line_number = qb_find_doc_comment_line_number(comment, comment_length, offset, filepath, line_number_max);
	TSRMLS_FETCH();
	QB_G(current_filename) = filepath;
	QB_G(current_line_number) = line_number;
	qb_abort("Syntax error encountered while parsing Doc Comments for type information");
}

qb_function_declaration * ZEND_FASTCALL qb_parse_function_declaration_table(qb_compiler_data_pool *pool, zend_function *zfunc, HashTable *ht) {
	qb_function_declaration *function_decl = qb_allocate_function_declaration(pool);
	Bucket *p;

	function_decl->zend_function = zfunc;

	for(p = ht->pListHead; p; p = p->pListNext) {
		if(p->nKeyLength) {
			zval **p_element = p->pData, *element = *p_element;
			ulong h = zend_inline_hash_func(p->arKey, p->nKeyLength);
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
								function_decl->flags |= QB_ENGINE_NEVER_COMPILE;
							} else if(data_len == 9 && strncmp(data, "qb-native", 9) == 0) {
								function_decl->flags |= QB_ENGINE_COMPILE_IF_POSSIBLE;
							} else {
								error_zval = element;
							}
						}	break;
						case FUNC_DECL_IMPORT: {
						}	break;
						case FUNC_DECL_RETURN: {
							decl = qb_parse_type_declaration(pool, data, data_len, QB_VARIABLE_RETURN_VALUE, NULL);
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
								decl = qb_parse_type_declaration(pool, data, data_len, var_type, NULL);
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
						qb_abort("Error encountered while processing type information: '%s'", Z_STRVAL_P(error_zval));
					} else {
						qb_abort("Unexpected %s encountered while processing type information", zend_get_type_by_const(Z_TYPE_P(error_zval)));
					}
				}
			} else {
				qb_abort("Unknown keyword '%s' encountered while processing type information", p->arKey);
			}
		} else {
			qb_abort("Numeric key encountered where a string is expected");
		}
	}
	return function_decl;
}

qb_function_declaration * ZEND_FASTCALL qb_parse_function_doc_comment(qb_compiler_data_pool *pool, zend_function *zfunc, zend_class_entry *ce) {
	qb_function_declaration *function_decl = NULL;
	const char *s = zfunc->op_array.doc_comment;
	size_t len = zfunc->op_array.doc_comment_len;

	if(s && strstr(s, "@engine")) {
		int offsets[48], matches;
		uint32_t start_index = 0;
		int32_t use_qb = FALSE;

		function_decl = qb_allocate_function_declaration(pool);
		function_decl->zend_function = zfunc;

		for(start_index = 0; start_index < len; start_index = offsets[1]) {
			matches = pcre_exec(doc_comment_function_regexp, NULL, s, len, start_index, 0, offsets, sizeof(offsets) / sizeof(int));
			if(matches > 0) {
				uint32_t data_offset = GROUP_OFFSET(FUNC_DECL_DATA), data_len = GROUP_LENGTH(FUNC_DECL_DATA), var_type;
				const char *data = s + data_offset;
				qb_type_declaration *decl;

				if(FOUND_GROUP(FUNC_DECL_ENGINE)) {
					if(data_len == 2 && strncmp(data, "qb", 2) == 0) {
						use_qb = TRUE;
					} else if(data_len == 11 && strncmp(data, "qb-bytecode", 11) == 0) {
						use_qb = TRUE;
						function_decl->flags |= QB_ENGINE_NEVER_COMPILE;
					} else if(data_len == 9 && strncmp(data, "qb-native", 9) == 0) {
						use_qb = TRUE;
						function_decl->flags |= QB_ENGINE_COMPILE_IF_POSSIBLE;
					}
				} else if(FOUND_GROUP(FUNC_DECL_IMPORT)) {
					function_decl->import_path = qb_allocate_string(pool, data, data_len);
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
						qb_abort_doc_comment_unexpected_error(s, len, matches, offsets, zfunc->op_array.filename, zfunc->op_array.line_start);
					}
					decl = qb_parse_type_declaration(pool, data, data_len, var_type, ce);
					if(decl) {
						qb_add_variable_declaration(function_decl, decl);
					} else {
						qb_abort_doc_comment_syntax_error(s, len, data_offset, zfunc->op_array.filename, zfunc->op_array.line_start);
					}
				}
			} else {
				break;
			}
		}
		if(!use_qb) {
			function_decl = NULL;
		}
	}
	return function_decl;
}

qb_class_declaration * ZEND_FASTCALL qb_parse_class_doc_comment(qb_compiler_data_pool *pool, zend_class_entry *ce) {
	qb_class_declaration *class_decl;
	const char *s = Z_CLASS_INFO(ce, doc_comment);
	size_t len = Z_CLASS_INFO(ce, doc_comment_len);
	int offsets[48], matches;
	uint32_t start_index = 0;
	Bucket *p;

	class_decl = qb_allocate_class_declaration(pool);
	class_decl->zend_class = ce;

	if(s) {
		for(start_index = 0; start_index < len; start_index = offsets[1]) {
			matches = pcre_exec(doc_comment_function_regexp, NULL, s, len, start_index, 0, offsets, sizeof(offsets) / sizeof(int));
			if(matches > 0) {
				uint32_t data_offset = GROUP_OFFSET(FUNC_DECL_DATA), data_len = GROUP_LENGTH(FUNC_DECL_DATA), var_type;
				const char *data = s + data_offset;
				qb_type_declaration *decl;

				if(FOUND_GROUP(FUNC_DECL_PROPERTY)) {
					var_type = QB_VARIABLE_CLASS_INSTANCE;
				} else if(FOUND_GROUP(FUNC_DECL_STATIC)) {
					var_type = QB_VARIABLE_CLASS;
				} else {
					qb_abort_doc_comment_unexpected_error(s, len, matches, offsets, Z_CLASS_INFO(ce, filename), Z_CLASS_INFO(ce, line_start));
				} 
				decl = qb_parse_type_declaration(pool, data, data_len, var_type, ce);
				if(decl) {
					qb_add_class_variable_declaration(class_decl, decl);
				} else {
					qb_abort_doc_comment_syntax_error(s, len, data_offset, Z_CLASS_INFO(ce, filename), Z_CLASS_INFO(ce, line_start));
				}
			} else {
				break;
			}
		}
	}

	for(p = ce->properties_info.pListHead; p; p = p->pListNext) {
		zend_property_info *prop = p->pData;
		const char *s = prop->doc_comment;
		size_t len = prop->doc_comment_len;
		int offsets[48], matches;
		uint32_t start_index = 0;

		if(s) {
			for(start_index = 0; start_index < len; start_index = offsets[1]) {
				matches = pcre_exec(doc_comment_function_regexp, NULL, s, len, start_index, 0, offsets, sizeof(offsets) / sizeof(int));
				if(matches > 0) {
					uint32_t data_offset = GROUP_OFFSET(FUNC_DECL_DATA), data_len = GROUP_LENGTH(FUNC_DECL_DATA);
					const char *data = s + data_offset;
					qb_type_declaration *decl;

					if(FOUND_GROUP(FUNC_DECL_VAR)) {
						decl = qb_parse_type_declaration(pool, data, data_len, 0, ce);
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
							qb_abort_doc_comment_syntax_error(s, len, data_offset, Z_CLASS_INFO(ce, filename), Z_CLASS_INFO(ce, line_end));
						}
					} else {
						qb_abort_doc_comment_unexpected_error(s, len, matches, offsets, Z_CLASS_INFO(ce, filename), Z_CLASS_INFO(ce, line_end));
					}
				} else {
					break;
				}
			}
		}
	}

	return class_decl;
}

static void ZEND_FASTCALL qb_retrieve_operand(qb_php_translater_context *cxt, uint32_t zoperand_type, znode_op *zoperand, qb_operand *operand) {
	switch(zoperand_type) {
		case Z_OPERAND_CV: {
			uint32_t var_index = Z_OPERAND_INFO(*zoperand, var);
			qb_variable *qvar = cxt->compiler_context->variables[var_index];
			if(!qvar->address) {
				// the variable type hasn't been set yet
				qvar->flags |= QB_VARIABLE_LOCAL;
				qb_set_variable_type(cxt->compiler_context, qvar);
			}
			operand->address = qvar->address;
			operand->type = QB_OPERAND_ADDRESS;
		}	break;
		case Z_OPERAND_CONST: {
			operand->constant = Z_OPERAND_ZV(*zoperand);
			operand->type = QB_OPERAND_ZVAL;
		}	break;
		case Z_OPERAND_TMP_VAR:
		case Z_OPERAND_VAR: {
			uint32_t temp_var_index = Z_OPERAND_TMP_INDEX(zoperand);
			if(temp_var_index < cxt->temp_variable_count) {
				qb_temporary_variable *temp_variable = &cxt->temp_variables[temp_var_index];
				*operand = temp_variable->operand;
				if(cxt->compiler_context->stage == QB_STAGE_RESULT_TYPE_RESOLUTION) {
					temp_variable->last_access_op_index = cxt->zend_op_index;
				}
				break;
			}
		}	// fall through in case of invalid index
		default: {
			operand->type = QB_OPERAND_NONE;
			operand->generic_pointer = NULL;
		}
	}
}

static void ZEND_FASTCALL qb_retire_operand(qb_php_translater_context *cxt, uint32_t zoperand_type, znode_op *zoperand, qb_operand *operand) {
	switch(zoperand_type) {
		case Z_OPERAND_TMP_VAR:
		case Z_OPERAND_VAR: {
			uint32_t temp_var_index = Z_OPERAND_TMP_INDEX(zoperand);
			if(temp_var_index < cxt->temp_variable_count) {
				qb_temporary_variable *temp_variable = &cxt->temp_variables[temp_var_index];
				if(cxt->compiler_context->stage == QB_STAGE_RESULT_TYPE_RESOLUTION) {
					temp_variable->operand = *operand;
				} else if(cxt->compiler_context->stage == QB_STAGE_OPCODE_TRANSLATION) {
					if(temp_variable->last_access_op_index == cxt->zend_op_index) {
						// unlock the operand if the current op is the last to access it
						qb_unlock_operand(cxt->compiler_context, operand);
						temp_variable->operand.type = QB_OPERAND_EMPTY;
						temp_variable->operand.generic_pointer = NULL;
					} else {
						if(memcmp(&temp_variable->operand, operand, sizeof(qb_operand)) != 0) {
							// unlock what was there before if it's different
							qb_unlock_operand(cxt->compiler_context, &temp_variable->operand);
							temp_variable->operand = *operand;
							qb_lock_operand(cxt->compiler_context, &temp_variable->operand);
						}
					}
				}
			}
		}	break;
	}
}

static void ZEND_FASTCALL qb_lock_temporary_variables(qb_php_translater_context *cxt) {
	uint32_t i = 0;
	for(i = 0; i < cxt->temp_variable_count; i++) {
		qb_temporary_variable *temp_variable = &cxt->temp_variables[i];
		if(temp_variable->operand.type == QB_OPERAND_ADDRESS) {
			qb_address *address = temp_variable->operand.address;
			if(address->source_address) {
				qb_lock_address(cxt->compiler_context, address->source_address);
			}
			if(address->array_index_address) {
				qb_lock_address(cxt->compiler_context, address->array_index_address);
			}			
		} else {
			// lock temporary variables used to initialize an array as well
			if(temp_variable->operand.type == QB_OPERAND_ARRAY_INITIALIZER) {
				qb_lock_operand(cxt->compiler_context, &temp_variable->operand);
			}
		}
	}
}

static qb_address * ZEND_FASTCALL qb_obtain_write_target_size(qb_php_translater_context *cxt, qb_result_prototype *result_prototype) {
	if(result_prototype->destination) {
		qb_result_destination *destination = result_prototype->destination;

		switch(destination->type) {
			case QB_RESULT_DESTINATION_VARIABLE: {
				qb_address *address = destination->variable.address;
				return address->array_size_address;
			}	break;
			case QB_RESULT_DESTINATION_ELEMENT: {
				if(destination->element.container.type == QB_OPERAND_ADDRESS) {
					qb_address *address = destination->element.container.address;
					if(address->dimension_count > 1) {
						return address->array_size_addresses[1];
					}
				}
			}	break;
			case QB_RESULT_DESTINATION_PROPERTY: {
				if(destination->property.container.type == QB_OPERAND_ADDRESS) {
					qb_address *address = destination->property.container.address;
					if(address->dimension_count > 1) {
						return address->array_size_addresses[1];
					}
				}
			}	break;
			default: break;
		}
	}
	return NULL;
}

static void ZEND_FASTCALL qb_translate_cast(qb_php_translater_context *cxt, void *op_factory, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_result_prototype *result_prototype) {
	uint32_t zend_type = cxt->zend_op->extended_value;
	qb_primitive_type type;

	switch(zend_type) {
		case IS_ARRAY:	type = QB_TYPE_ANY; break;
		case IS_BOOL:	type = QB_TYPE_S32; break;
		case IS_LONG:	type = QB_TYPE_S64; break;
		case IS_DOUBLE:	type = QB_TYPE_F64; break;
		default:		type = QB_TYPE_UNKNOWN;
	}
}

static void ZEND_FASTCALL qb_translate_basic_op(qb_php_translater_context *cxt, void *op_factory, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_result_prototype *result_prototype) {
	qb_produce_op(cxt->compiler_context, op_factory, operands, operand_count, result, NULL, 0, result_prototype);
}

static zval * ZEND_FASTCALL qb_get_special_constant(qb_php_translater_context *cxt, const char *name, uint32_t length) {
	static zval type_constants[QB_TYPE_COUNT];
	static zval qb_indicator;

	if(strcmp(name, "__QB__") == 0) {
		Z_TYPE(qb_indicator) = IS_LONG;
		Z_LVAL(qb_indicator) = 1;
		return &qb_indicator;
	} else {
		uint32_t i;
		for(i = 0; i < QB_TYPE_COUNT; i++) {
			const char *type = type_names[i];
			if(strcmp(name, type) == 0) {
				Z_TYPE(type_constants[i]) = IS_LONG;
				Z_LVAL(type_constants[i]) = i;
				return &type_constants[i];
			}
		}
	}
	return NULL;
}

static void ZEND_FASTCALL qb_translate_fetch_constant(qb_php_translater_context *cxt, void *op_factory, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_result_prototype *result_prototype) {
	qb_operand *scope = &operands[0], *name = &operands[1];
#if ZEND_ENGINE_2_2 || ZEND_ENGINE_2_1
	if(scope->type == QB_OPERAND_ZVAL) {
		qb_retrieve_operand(cxt, Z_OPERAND_TMP_VAR, &cxt->zend_op->op1, scope);
	}
#endif

	if(cxt->compiler_context->stage == QB_STAGE_RESULT_TYPE_RESOLUTION) {
		result_prototype->preliminary_type = QB_TYPE_ANY;
	} else if(cxt->compiler_context->stage == QB_STAGE_OPCODE_TRANSLATION) {
		USE_TSRM
		if(scope->type == QB_OPERAND_ZEND_CLASS) {
			zval *name_value = name->constant;
			ulong hash_value = Z_HASH_P(name_value);
			zend_class_entry *ce = scope->zend_class;
			zval **p_value;
			if(zend_hash_quick_find(&ce->constants_table, Z_STRVAL_P(name_value), Z_STRLEN_P(name_value) + 1, hash_value, (void **) &p_value) == SUCCESS) {
				result->type = QB_OPERAND_ZVAL;
				result->constant = *p_value;
			} else {
				qb_abort("Undefined class constant '%s'", Z_STRVAL_P(name_value));
			}
		} else {
			zval *value = NULL;
#if !ZEND_ENGINE_2_3 && !ZEND_ENGINE_2_2 && !ZEND_ENGINE_2_1
			zend_literal *key = Z_OPERAND_INFO(cxt->zend_op->op2, literal) + 1;
			zval *name = &key->constant;
			ulong hash_value = key->hash_value;
#else
			zval *name = Z_OPERAND_ZV(cxt->zend_op->op2);
			ulong hash_value = zend_inline_hash_func(Z_STRVAL_P(name), Z_STRLEN_P(name) + 1);
#endif
			zend_constant *zconst;
			if(zend_hash_quick_find(EG(zend_constants), Z_STRVAL_P(name), Z_STRLEN_P(name) + 1, hash_value, (void **) &zconst) == SUCCESS) {
				value = &zconst->value;
			}
			if(!value) {
				value = qb_get_special_constant(cxt, Z_STRVAL_P(name), Z_STRLEN_P(name));
			}
			if(value) {
				result->type = QB_OPERAND_ZVAL;
				result->constant = value;
			} else {
				qb_abort("Undefined constant '%s'", Z_STRVAL_P(name));
			}
		}
	}
}

static void ZEND_FASTCALL qb_translate_fetch(qb_php_translater_context *cxt, void *op_factory, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_result_prototype *result_prototype) {
	USE_TSRM
	qb_operand *name = &operands[0], *scope = &operands[1];

#if ZEND_ENGINE_2_2 || ZEND_ENGINE_2_1
	if(scope->type == QB_OPERAND_ZVAL) {
		qb_retrieve_operand(cxt, Z_OPERAND_TMP_VAR, &cxt->zend_op->op2, scope);
	}
#endif
	if(name->type == QB_OPERAND_ZVAL) {
		if(scope->type == QB_OPERAND_NONE) {
			uint32_t fetch_type = FETCH_TYPE(cxt->zend_op);
			if(fetch_type == ZEND_FETCH_LOCAL) {
				// look up the variable
				qb_variable *qvar = qb_find_variable(cxt->compiler_context, NULL, name->constant, 0);
				if(qvar) {
					if(!(qvar->flags & QB_VARIABLE_LOCAL)) {
						qvar->flags |= QB_VARIABLE_LOCAL;
						qb_set_variable_type(cxt->compiler_context, qvar);
					}
					result->type = QB_OPERAND_ADDRESS;
					result->address = qvar->address;
				} else {
					qb_abort("Undefined variable: %s", Z_STRVAL_P(name->constant));
				}
			} else if(fetch_type == ZEND_FETCH_GLOBAL || fetch_type == ZEND_FETCH_GLOBAL_LOCK) {
				// look up the variable and mark it as global
				qb_variable *qvar = qb_find_variable(cxt->compiler_context, NULL, name->constant, 0);
				if(qvar) {
					if(!(qvar->flags & QB_VARIABLE_GLOBAL)) {
						qvar->flags |= QB_VARIABLE_GLOBAL;
						qb_set_variable_type(cxt->compiler_context, qvar);
					}
				}
				// set result to GLOBAL_STATIC so the ZEND_ASSIGN_REF handler knows that
				// the original op is mapping a variable from the global/static symbol table
				result->type = QB_OPERAND_GLOBAL_STATIC;
				result->address = NULL;
			} else if(fetch_type == ZEND_FETCH_STATIC) {
				// static variables are already handled in qb_add_variables()
				result->type = QB_OPERAND_GLOBAL_STATIC;
				result->address = NULL;
			}
		} else if(scope->type == QB_OPERAND_ZEND_CLASS) {
			// get class variable
			result->type = QB_OPERAND_ADDRESS;
			result->address = qb_obtain_class_variable(cxt->compiler_context, scope->zend_class, name->constant);
		}
	} else {
		qb_abort("internal error");
	}
}

static void ZEND_FASTCALL qb_translate_fetch_class(qb_php_translater_context *cxt, void *op_factory, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_result_prototype *result_prototype) {
	int32_t fetch_type = cxt->zend_op->extended_value & ZEND_FETCH_CLASS_MASK;

	if(fetch_type == ZEND_FETCH_CLASS_SELF) {
		result->zend_class = cxt->zend_class;
#if !ZEND_ENGINE_2_2 && !ZEND_ENGINE_2_1
	} else if(fetch_type == ZEND_FETCH_CLASS_STATIC) {
		result->zend_class = cxt->zend_class;
#endif
	} else if(fetch_type == ZEND_FETCH_CLASS_PARENT) {
		result->zend_class = (cxt->zend_class) ? cxt->zend_class->parent : NULL;
	}
	result->type = QB_OPERAND_ZEND_CLASS;

#if ZEND_ENGINE_2_3 || ZEND_ENGINE_2_2 || ZEND_ENGINE_2_1
	qb_retire_operand(cxt, Z_OPERAND_TMP_VAR, &cxt->zend_op->result, result);
#endif
}

static void ZEND_FASTCALL qb_translate_receive_argument(qb_php_translater_context *cxt, void *op_factory, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_result_prototype *result_prototype) {
	if(cxt->compiler_context->stage == QB_STAGE_RESULT_TYPE_RESOLUTION) {
		qb_operand *init_value = &operands[1];

		if(init_value->type == QB_OPERAND_ZVAL) {
#if !ZEND_ENGINE_2_3 && !ZEND_ENGINE_2_2 && !ZEND_ENGINE_2_1
			uint32_t argument_number = Z_OPERAND_INFO(cxt->zend_op->op1, num);
#else
			uint32_t argument_number = Z_LVAL_P(Z_OPERAND_ZV(cxt->zend_op->op1));
#endif
			qb_variable *qvar = cxt->compiler_context->variables[argument_number - 1];
			qvar->default_value_address = qb_obtain_constant_zval(cxt->compiler_context, init_value->constant, qvar->address->type);
		}
	}
}

static void ZEND_FASTCALL qb_translate_return(qb_php_translater_context *cxt, void *op_factory, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_result_prototype *result_prototype) {
	qb_produce_op(cxt->compiler_context, op_factory, operands, operand_count, result, NULL, 0, result_prototype);

	cxt->jump_target_index1 = QB_INSTRUCTION_END;
}

static void ZEND_FASTCALL qb_translate_exit(qb_php_translater_context *cxt, void *op_factory, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_result_prototype *result_prototype) {
	qb_produce_op(cxt->compiler_context, op_factory, operands, operand_count, result, NULL, 0, result_prototype);

	cxt->jump_target_index1 = QB_INSTRUCTION_END;
}

static void ZEND_FASTCALL qb_translate_jump(qb_php_translater_context *cxt, void *op_factory, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_result_prototype *result_prototype) {
	zend_op *target_op = Z_OPERAND_INFO(cxt->zend_op->op1, jmp_addr);
	uint32_t target_index = ZEND_OP_INDEX(target_op);

	qb_produce_op(cxt->compiler_context, op_factory, operands, operand_count, result, &target_index, 1, result_prototype);

	cxt->jump_target_index1 = target_index;
}

static void ZEND_FASTCALL qb_translate_jump_set(qb_php_translater_context *cxt, void *op_factory, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_result_prototype *result_prototype) {
	// TODO
	// jump over the false clause first
	//cxt->jump_target_index1 = ZEND_OP_INDEX(target_op);
	//cxt->jump_target_index2 = cxt->zend_op_index + 1;
}

static void ZEND_FASTCALL qb_translate_branch(qb_php_translater_context *cxt, void *op_factory, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_result_prototype *result_prototype) {
	qb_operand *condition = &operands[0];
	zend_op *target_op1 = Z_OPERAND_INFO(cxt->zend_op->op2, jmp_addr);
	uint32_t target_indices[2];

	target_indices[0] = ZEND_OP_INDEX(target_op1);
	target_indices[1] = cxt->zend_op_index + 1;

	qb_produce_op(cxt->compiler_context, op_factory, operands, operand_count, result, target_indices, 2, result_prototype);

	// start down the next instruction first before going down the branch
	cxt->jump_target_index1 = target_indices[1];
	cxt->jump_target_index2 = target_indices[0];
}

static void ZEND_FASTCALL qb_translate_for_loop(qb_php_translater_context *cxt, void *op_factory, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_result_prototype *result_prototype) {
	qb_operand *condition = &operands[0];
	uint32_t target_indices[2];
	
	target_indices[0] = cxt->zend_op->extended_value;
	target_indices[1] = Z_OPERAND_INFO(cxt->zend_op->op2, opline_num);

	qb_produce_op(cxt->compiler_context, op_factory, operands, operand_count, result, target_indices, 2, result_prototype);

	cxt->jump_target_index1 = target_indices[0];
	cxt->jump_target_index2 = target_indices[1];
}

static zend_brk_cont_element * ZEND_FASTCALL qb_find_break_continue_element(qb_php_translater_context *cxt, int32_t nest_levels, int32_t array_offset) {
	zend_brk_cont_element *brk_cont_array = cxt->zend_op_array->brk_cont_array, *jmp_to;
	int32_t i;
	for(i = 0; i < nest_levels; i++) {
		if(array_offset == -1) {
			qb_abort("Cannot break/continue %d level%s", nest_levels, (nest_levels == 1) ? "" : "s");
		}
		jmp_to = &brk_cont_array[array_offset];
		array_offset = jmp_to->parent;
	}
	return jmp_to;
}


static void ZEND_FASTCALL qb_translate_break(qb_php_translater_context *cxt, void *op_factory, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_result_prototype *result_prototype) {
	zval *nest_level = Z_OPERAND_ZV(cxt->zend_op->op2);
	zend_brk_cont_element *jmp_to = qb_find_break_continue_element(cxt, Z_LVAL_P(nest_level), Z_OPERAND_INFO(cxt->zend_op->op1, opline_num));
	uint32_t target_index = jmp_to->brk;

	qb_produce_op(cxt->compiler_context, op_factory, operands, operand_count, result, &target_index, 1, result_prototype);

	cxt->jump_target_index1 = target_index;
}

static void ZEND_FASTCALL qb_translate_continue(qb_php_translater_context *cxt, void *op_factory, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_result_prototype *result_prototype) {
	zval *nest_level = Z_OPERAND_ZV(cxt->zend_op->op2);
	zend_brk_cont_element *jmp_to = qb_find_break_continue_element(cxt, Z_LVAL_P(nest_level), Z_OPERAND_INFO(cxt->zend_op->op1, opline_num));
	uint32_t target_index = jmp_to->cont;

	qb_produce_op(cxt->compiler_context, op_factory, operands, operand_count, result, &target_index, 1, result_prototype);

	cxt->jump_target_index1 = jmp_to->cont;
}

static void ZEND_FASTCALL qb_translate_foreach_fetch(qb_php_translater_context *cxt, void *op_factory, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_result_prototype *result_prototype) {
	qb_operand *container = &operands[0];
	uint32_t target_index = Z_OPERAND_INFO(cxt->zend_op->op2, opline_num);

	if(cxt->zend_op->extended_value & ZEND_FE_FETCH_BYREF) {
		qb_abort("Reference is not supported");
	}

	cxt->jump_target_index1 = cxt->zend_op_index + 2;
	cxt->jump_target_index2 = target_index;
}

static zend_function * ZEND_FASTCALL qb_find_function(qb_php_translater_context *cxt, zend_class_entry *ce, zval *name) {
	USE_TSRM

	// check to see if we've found the function already (this function is called whenever an argument is pushed)
	if(cxt->previous_class != ce || cxt->previous_function_name != name) {
		char *error = NULL;
#if ZEND_ENGINE_2_2 || ZEND_ENGINE_2_1
		int error_reporting_before;
#endif
		zend_fcall_info_cache fcc;

		if(ce) {
			// no good way to get the class entry to zend_is_callable_ex() except by creating an array
			HashTable ht;
			zval _callable, *callable = &_callable;
			zval *class_name;

			zend_hash_init(&ht, sizeof(zval *), NULL, NULL, 0);
			Z_ARRVAL_P(callable) = &ht;
			Z_TYPE_P(callable) = IS_ARRAY;

			class_name = qb_string_to_zval(ce->name, ce->name_length TSRMLS_CC);
			zend_hash_next_index_insert(&ht, &class_name, sizeof(zval *), NULL);
			zend_hash_next_index_insert(&ht, &name, sizeof(zval *), NULL);

#if !ZEND_ENGINE_2_2 && !ZEND_ENGINE_2_1
			if(!zend_is_callable_ex(callable, NULL, IS_CALLABLE_CHECK_NO_ACCESS, NULL, NULL, &fcc, &error TSRMLS_CC)) {
				qb_abort("%s", error);
			}
#else
			// suppress the non-static function being called as static warning message
			error_reporting_before = EG(error_reporting);
			EG(error_reporting) = 0;
			if(!zend_is_callable_ex(callable, IS_CALLABLE_CHECK_NO_ACCESS, NULL, NULL, NULL, &fcc.function_handler, &fcc.object_pp TSRMLS_CC)) {
				qb_abort("Cannot find function: %s", Z_STRVAL_P(name));
			}
			EG(error_reporting) = error_reporting_before;
#endif
			zend_hash_destroy(&ht);
		} else {
#if !ZEND_ENGINE_2_2 && !ZEND_ENGINE_2_1
			if(!zend_is_callable_ex(name, NULL, 0, NULL, NULL, &fcc, &error TSRMLS_CC)) {
				qb_abort("%s", error);
			}
#else
			if(!zend_is_callable_ex(name, IS_CALLABLE_CHECK_NO_ACCESS, NULL, NULL, NULL, &fcc.function_handler, &fcc.object_pp TSRMLS_CC)) {
				qb_abort("Cannot find function: %s", Z_STRVAL_P(name));
			}
#endif
		}
		
		cxt->previous_class = ce;
		cxt->previous_function_name = name;
		cxt->previous_function = fcc.function_handler;
		if(error) {
			efree(error);
		}
	}
	return cxt->previous_function;
}

static qb_function * ZEND_FASTCALL qb_get_compiled_function(qb_php_translater_context *cxt, zend_function *zfunc) {
	uint32_t i;
	if(qb_is_compiled_function(zfunc)) {
		return zfunc->op_array.reserved[0];
	} else {
		// see if the function in question in also in the middle of being compiled
		USE_TSRM
		qb_build_context *build_context = QB_G(build_context);
		for(i = 0; i < build_context->compiler_context_count; i++) {
			qb_compiler_context *compiler_cxt = &build_context->compiler_contexts[i];
			if(compiler_cxt->function_prototype.zend_function == zfunc) {
				return &compiler_cxt->function_prototype;
			}
		}
	}
	return NULL;
}

static void ZEND_FASTCALL qb_translate_user_opcode(qb_php_translater_context *cxt, void *op_factory, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_result_prototype *result_prototype) {
}

static void ZEND_FASTCALL qb_translate_extension_op(qb_php_translater_context *cxt, void *op_factory, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_result_prototype *result_prototype) {
}

static void ZEND_FASTCALL qb_translate_nop(qb_php_translater_context *cxt, void *op_factory, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_result_prototype *result_prototype) {
}

static qb_php_op_translator op_translators[] = {
	{	qb_translate_basic_op,				&factory_nop						},	// ZEND_NOP
	{	qb_translate_basic_op,				&factory_add						},	// ZEND_ADD
	{	qb_translate_basic_op,				&factory_subtract					},	// ZEND_SUB
	{	qb_translate_basic_op,				&factory_multiply					},	// ZEND_MUL
	{	qb_translate_basic_op,				&factory_divide						},	// ZEND_DIV
	{	qb_translate_basic_op,				&factory_modulo						},	// ZEND_MOD
	{	qb_translate_basic_op,				&factory_shift_left					},	// ZEND_SL
	{	qb_translate_basic_op,				&factory_shift_right				},	// ZEND_SR
	{	qb_translate_basic_op,				NULL								},	// ZEND_CONCAT
	{	qb_translate_basic_op,				&factory_bitwise_or					},	// ZEND_BW_OR
	{	qb_translate_basic_op,				&factory_bitwise_and				},	// ZEND_BW_AND
	{	qb_translate_basic_op,				&factory_bitwise_xor				},	// ZEND_BW_XOR
	{	qb_translate_basic_op,				&factory_bitwise_not				},	// ZEND_BW_NOT
	{	qb_translate_basic_op,				&factory_logical_not				},	// ZEND_BOOL_NOT
	{	qb_translate_basic_op,				&factory_logical_xor				},	// ZEND_BOOL_XOR
	{	qb_translate_basic_op,				NULL						},	// ZEND_IS_IDENTICAL
	{	qb_translate_basic_op,				NULL						},	// ZEND_IS_NOT_IDENTICAL
	{	qb_translate_basic_op,				&factory_equal						},	// ZEND_IS_EQUAL
	{	qb_translate_basic_op,				&factory_not_equal					},	// ZEND_IS_NOT_EQUAL
	{	qb_translate_basic_op,				&factory_less_than					},	// ZEND_IS_SMALLER
	{	qb_translate_basic_op,				&factory_less_equal					},	// ZEND_IS_SMALLER_OR_EQUAL
	{	qb_translate_basic_op,				NULL						},	// ZEND_CAST
	{	qb_translate_basic_op,				NULL						},	// ZEND_QM_ASSIGN
	{	qb_translate_basic_op,				&factory_add				},	// ZEND_ASSIGN_ADD
	{	qb_translate_basic_op,				&factory_subtract			},	// ZEND_ASSIGN_SUB
	{	qb_translate_basic_op,				&factory_multiply			},	// ZEND_ASSIGN_MUL
	{	qb_translate_basic_op,				&factory_divide				},	// ZEND_ASSIGN_DIV
	{	qb_translate_basic_op,				&factory_modulo				},	// ZEND_ASSIGN_MOD
	{	qb_translate_basic_op,				&factory_shift_left			},	// ZEND_ASSIGN_SL
	{	qb_translate_basic_op,				&factory_shift_right		},	// ZEND_ASSIGN_SR
	{	qb_translate_basic_op,				NULL						},	// ZEND_ASSIGN_CONCAT
	{	qb_translate_basic_op,				&factory_bitwise_or			},	// ZEND_ASSIGN_BW_OR
	{	qb_translate_basic_op,				&factory_bitwise_and		},	// ZEND_ASSIGN_BW_AND
	{	qb_translate_basic_op,				&factory_bitwise_xor		},	// ZEND_ASSIGN_BW_XOR
	{	qb_translate_basic_op,				&factory_increment			},	// ZEND_PRE_INC
	{	qb_translate_basic_op,				&factory_decrement			},	// ZEND_PRE_DEC
	{	qb_translate_basic_op,				&factory_increment			},	// ZEND_POST_INC
	{	qb_translate_basic_op,				&factory_decrement			},	// ZEND_POST_DEC
	{	qb_translate_basic_op,				&factory_assignment					},	// ZEND_ASSIGN
	{	qb_translate_basic_op,				NULL						},	// ZEND_ASSIGN_REF
	{	qb_translate_basic_op,				&factory_echo						},	// ZEND_ECHO
	{	qb_translate_basic_op,				&factory_print						},	// ZEND_PRINT
	{	qb_translate_jump,					&factory_jump				},	// ZEND_JMP
	{	qb_translate_branch,				&factory_branch_on_false	},	// ZEND_JMPZ
	{	qb_translate_branch,				&factory_branch_on_true		},	// ZEND_JMPNZ
	{	qb_translate_for_loop,				&factory_branch_on_true		},	// ZEND_JMPZNZ
	{	qb_translate_branch,				&factory_branch_on_false	},	// ZEND_JMPZ_EX
	{	qb_translate_branch,				&factory_branch_on_true		},	// ZEND_JMPNZ_EX
	{	qb_translate_basic_op,				NULL						},	// ZEND_CASE
	{	qb_translate_basic_op,				NULL						},	// ZEND_SWITCH_FREE
	{	qb_translate_break,					&factory_jump				},	// ZEND_BRK
	{	qb_translate_continue,				&factory_jump				},	// ZEND_CONT
	{	qb_translate_basic_op,				&factory_boolean			},	// ZEND_BOOL
	{	qb_translate_basic_op,				NULL						},	// ZEND_INIT_STRING
	{	qb_translate_basic_op,				NULL						},	// ZEND_ADD_CHAR
	{	qb_translate_basic_op,				NULL						},	// ZEND_ADD_STRING
	{	qb_translate_basic_op,				NULL						},	// ZEND_ADD_VAR
	{	qb_translate_basic_op,				NULL						},	// ZEND_BEGIN_SILENCE
	{	qb_translate_basic_op,				NULL						},	// ZEND_END_SILENCE
	{	qb_translate_basic_op,				NULL						},	// ZEND_INIT_FCALL_BY_NAME
	{	qb_translate_basic_op,				NULL						},	// ZEND_DO_FCALL
	{	qb_translate_basic_op,				NULL						},	// ZEND_DO_FCALL_BY_NAME
	{	qb_translate_return,				&factory_return				},	// ZEND_RETURN
	{	qb_translate_receive_argument,		NULL						},	// ZEND_RECV
	{	qb_translate_receive_argument,		NULL						},	// ZEND_RECV_INIT
	{	qb_translate_basic_op,				NULL						},	// ZEND_SEND_VAL
	{	qb_translate_basic_op,				NULL						},	// ZEND_SEND_VAR
	{	qb_translate_basic_op,				NULL						},	// ZEND_SEND_REF
	{	NULL,								NULL						},	// ZEND_NEW
	{	NULL,								NULL						},	// ZEND_INIT_NS_FCALL_BY_NAME
	{	qb_translate_basic_op,				NULL						},	// ZEND_FREE
	{	qb_translate_basic_op,				NULL						},	// ZEND_INIT_ARRAY
	{	qb_translate_basic_op,				NULL						},	// ZEND_ADD_ARRAY_ELEMENT
	{	NULL,								NULL						},	// ZEND_INCLUDE_OR_EVAL
	{	qb_translate_basic_op,				NULL						},	// ZEND_UNSET_VAR
	{	qb_translate_basic_op,				NULL						},	// ZEND_UNSET_DIM
	{	qb_translate_basic_op,				NULL						},	// ZEND_UNSET_OBJ
	{	qb_translate_basic_op,				NULL						},	// ZEND_FE_RESET
	{	qb_translate_foreach_fetch,			NULL						},	// ZEND_FE_FETCH
	{	qb_translate_exit,					&factory_exit						},	// ZEND_EXIT
	{	qb_translate_basic_op,				NULL						},	// ZEND_FETCH_R
	{	qb_translate_basic_op,				&factory_fetch_array_element		},	// ZEND_FETCH_DIM_R
	{	qb_translate_basic_op,				&factory_fetch_object_property		},	// ZEND_FETCH_OBJ_R
	{	qb_translate_basic_op,				NULL						},	// ZEND_FETCH_W
	{	qb_translate_basic_op,				&factory_fetch_array_element		},	// ZEND_FETCH_DIM_W
	{	qb_translate_basic_op,				&factory_fetch_object_property		},	// ZEND_FETCH_OBJ_W
	{	qb_translate_basic_op,				NULL						},	// ZEND_FETCH_RW
	{	qb_translate_basic_op,				&factory_fetch_array_element		},	// ZEND_FETCH_DIM_RW
	{	qb_translate_basic_op,				&factory_fetch_object_property		},	// ZEND_FETCH_OBJ_RW
	{	qb_translate_basic_op,				NULL						},	// ZEND_FETCH_IS
	{	qb_translate_basic_op,				&factory_fetch_array_element		},	// ZEND_FETCH_DIM_IS
	{	qb_translate_basic_op,				&factory_fetch_object_property		},	// ZEND_FETCH_OBJ_IS
	{	qb_translate_basic_op,				NULL						},	// ZEND_FETCH_FUNC_ARG
	{	qb_translate_basic_op,				&factory_fetch_array_element		},	// ZEND_FETCH_DIM_FUNC_ARG
	{	qb_translate_basic_op,				&factory_fetch_object_property		},	// ZEND_FETCH_OBJ_FUNC_ARG
	{	qb_translate_basic_op,				NULL						},	// ZEND_FETCH_UNSET
	{	qb_translate_basic_op,				&factory_fetch_array_element		},	// ZEND_FETCH_DIM_UNSET
	{	qb_translate_basic_op,				&factory_fetch_object_property		},	// ZEND_FETCH_OBJ_UNSET
	{	qb_translate_basic_op,				&factory_fetch_array_element		},	// ZEND_FETCH_DIM_TMP_VAR
	{	qb_translate_basic_op,				NULL						},	// ZEND_FETCH_CONSTANT
	{	NULL,								NULL						},	// ZEND_GOTO
	{	qb_translate_extension_op,			&factory_ext				},	// ZEND_EXT_STMT
	{	qb_translate_extension_op,			&factory_ext				},	// ZEND_EXT_FCALL_BEGIN
	{	qb_translate_extension_op,			&factory_ext				},	// ZEND_EXT_FCALL_END
	{	qb_translate_extension_op,			&factory_ext				},	// ZEND_EXT_NOP
	{	NULL,								NULL						},	// ZEND_TICKS
	{	qb_translate_basic_op,				NULL						},	// ZEND_SEND_VAR_NO_REF
	{	NULL,								NULL						},	// ZEND_CATCH
	{	NULL,								NULL						},	// ZEND_THROW
	{	qb_translate_basic_op,				NULL						},	// ZEND_FETCH_CLASS
	{	NULL,								NULL						},	// ZEND_CLONE
	{	NULL,								NULL						},	// ZEND_RETURN_BY_REF
	{	qb_translate_basic_op,				NULL						},	// ZEND_INIT_METHOD_CALL
	{	qb_translate_basic_op,				NULL						},	// ZEND_INIT_STATIC_METHOD_CALL
	{	qb_translate_basic_op,				NULL						},	// ZEND_ISSET_ISEMPTY_VAR
	{	qb_translate_basic_op,				NULL						},	// ZEND_ISSET_ISEMPTY_DIM_OBJ
	{	NULL,								NULL						},	// 116
	{	NULL,								NULL						},	// 117
	{	NULL,								NULL						},	// 118
	{	NULL,								NULL						},	// 119
	{	NULL,								NULL						},	// 120
	{	NULL,								NULL						},	// 121
	{	NULL,								NULL						},	// 122
	{	NULL,								NULL						},	// 123
	{	NULL,								NULL						},	// 124
	{	NULL,								NULL						},	// 125
	{	NULL,								NULL						},	// 126
	{	NULL,								NULL						},	// 127
	{	NULL,								NULL						},	// 128
	{	NULL,								NULL						},	// 129
	{	NULL,								NULL						},	// 130
	{	NULL,								NULL						},	// 131
	{	qb_translate_basic_op,				&factory_increment			},	// ZEND_PRE_INC_OBJ
	{	qb_translate_basic_op,				&factory_decrement			},	// ZEND_PRE_DEC_OBJ
	{	qb_translate_basic_op,				&factory_increment			},	// ZEND_POST_INC_OBJ
	{	qb_translate_basic_op,				&factory_decrement			},	// ZEND_POST_DEC_OBJ
	{	qb_translate_basic_op,				&factory_object_property_assignment	},	// ZEND_ASSIGN_OBJ
	{	NULL,								NULL						},	// ZEND_OP_DATA
	{	NULL,								NULL						},	// ZEND_INSTANCEOF
	{	NULL,								NULL						},	// ZEND_DECLARE_CLASS
	{	NULL,								NULL						},	// ZEND_DECLARE_INHERITED_CLASS
	{	NULL,								NULL						},	// ZEND_DECLARE_FUNCTION
	{	NULL,								NULL						},	// ZEND_RAISE_ABSTRACT_ERROR
	{	NULL,								NULL						},	// ZEND_DECLARE_CONST
	{	NULL,								NULL						},	// ZEND_ADD_INTERFACE
	{	NULL,								NULL						},	// ZEND_DECLARE_INHERITED_CLASS_DELAYED
	{	NULL,								NULL						},	// ZEND_VERIFY_ABSTRACT_CLASS
	{	qb_translate_basic_op,				&factory_array_element_assignment	},	// ZEND_ASSIGN_DIM
	{	qb_translate_basic_op,				&factory_fetch_object_property	},	// ZEND_ISSET_ISEMPTY_PROP_OBJ
	{	NULL,								NULL						},	// ZEND_HANDLE_EXCEPTION
	{	qb_translate_user_opcode,			NULL						},	// ZEND_USER_OPCODE
	{	NULL,								NULL						},	// 151
	{	qb_translate_jump_set,				NULL						},	// ZEND_JMP_SET
	{	NULL,								NULL						},	// ZEND_DECLARE_LAMBDA_FUNCTION
	{	NULL,								NULL						},	// ZEND_ADD_TRAIT
	{	NULL,								NULL						},	// ZEND_BIND_TRAITS
	{	NULL,								NULL						},	// ZEND_SEPARATE
	{	qb_translate_basic_op,				NULL						},	// ZEND_QM_ASSIGN_VAR
	{	qb_translate_jump_set,				NULL						},	// ZEND_JMP_SET_VAR
	{	NULL,								NULL,						},	// ZEND_DISCARD_EXCEPTION
	{	NULL,								NULL,						},	// ZEND_YIELD
	{	NULL,								NULL,						},	// ZEND_GENERATOR_RETURN
	{	NULL,								NULL,						},	// ZEND_FAST_CALL
	{	NULL,								NULL,						},	// ZEND_FAST_RET
};

static void ZEND_FASTCALL qb_translate_current_instruction(qb_php_translater_context *cxt) {
	if(cxt->zend_op->opcode != ZEND_OP_DATA) {
		USE_TSRM
		qb_operand operands[3], result;
		qb_result_prototype *result_prototype = &cxt->result_prototypes[cxt->zend_op_index];
		qb_php_op_translator *t;
		uint32_t operand_count = 0;
		int32_t need_return_value = RETURN_VALUE_USED(cxt->zend_op);
		uint32_t zend_opcode = cxt->zend_op->opcode;

		QB_G(current_line_number) = cxt->zend_op->lineno;

		// retrieve operands 
		qb_retrieve_operand(cxt, Z_OPERAND_TYPE(cxt->zend_op->op1), &cxt->zend_op->op1, &operands[0]);
		qb_retrieve_operand(cxt, Z_OPERAND_TYPE(cxt->zend_op->op2), &cxt->zend_op->op2, &operands[1]);
		operand_count = (operands[0].type != QB_OPERAND_NONE) + (operands[1].type != QB_OPERAND_NONE);

		if(cxt->zend_op[1].opcode == ZEND_OP_DATA) {
			// retrieve the extra data
			qb_retrieve_operand(cxt, Z_OPERAND_TYPE(cxt->zend_op[1].op1), &cxt->zend_op[1].op1, &operands[2]);
			operand_count++;
		}

		// see whether the op returns a value
		if(need_return_value) {
			qb_retrieve_operand(cxt, Z_OPERAND_TYPE(cxt->zend_op->result), &cxt->zend_op->result, &result);
		} else {
			result.type = QB_OPERAND_NONE;
			result.address = NULL;
		}

		// look up the translator for this opcode
		if(EXPECTED(zend_opcode < sizeof(op_translators) / sizeof(op_translators[0]))) {
			t = &op_translators[zend_opcode];
		} else {
			t = NULL;
		}
		if(t && t->translate) {
			cxt->compiler_context->line_number = cxt->zend_op->lineno;
			t->translate(cxt, t->extra, operands, operand_count, &result, result_prototype);

			if(cxt->compiler_context->stage == QB_STAGE_RESULT_TYPE_RESOLUTION) {
				if(result.type == QB_OPERAND_ADDRESS) {
					result_prototype->preliminary_type = result_prototype->final_type = result.address->type;
					result_prototype->address_flags = result.address->flags;
				} else if(result.type == QB_OPERAND_RESULT_PROTOTYPE) {
					if(result.result_prototype != result_prototype) {
						// copy the result and link it to the previous one
						*result_prototype = *result.result_prototype;
						result.result_prototype->parent = result_prototype;
						result.result_prototype = result_prototype;
					}
					if(result_prototype->preliminary_type == QB_TYPE_UNKNOWN) {
						// if we're returning it, it's got to be something
						result_prototype->preliminary_type = QB_TYPE_ANY;
					}
				} 
				if(result_prototype->preliminary_type != QB_TYPE_UNKNOWN) {
					// link any operand that's a prototype operands to this prototype
					uint32_t i;
					for(i = 0; i < operand_count; i++) {
						if(operands[i].type == QB_OPERAND_RESULT_PROTOTYPE) {
							operands[i].result_prototype->parent = result_prototype;
						}
					}
				} else {
					result_prototype->preliminary_type = QB_TYPE_VOID;
				}
			}

			if(operand_count >= 1) {
				qb_retire_operand(cxt, Z_OPERAND_TYPE(cxt->zend_op->op1), &cxt->zend_op->op1, &operands[0]);
			}
			if(operand_count >= 2) {
				qb_retire_operand(cxt, Z_OPERAND_TYPE(cxt->zend_op->op2), &cxt->zend_op->op2, &operands[1]);
			}
			if(operand_count >= 3) {
				qb_retire_operand(cxt, Z_OPERAND_TYPE(cxt->zend_op->op2), &cxt->zend_op->op2, &operands[2]);
			}
			if(need_return_value) {
				qb_retire_operand(cxt, Z_OPERAND_TYPE(cxt->zend_op->result), &cxt->zend_op->result, &result);
			}

			// lock operands kept as temporary variables
			qb_lock_temporary_variables(cxt);
		} else {
			qb_abort("Unsupported language feature");
		}
	}
}

static qb_php_function_translator intrinsic_functions[] = {
	{	0,	"count",				1,		2,		NULL 						},
	{	0,	"sizeof",				1,		2,		NULL 						},
	{	0,	"strlen",				1,		2,		NULL 						},
	{	0,	"int8",					1,		1,		(void *) QB_TYPE_S08		},
	{	0,	"uint8",				1,		1,		(void *) QB_TYPE_U08		},
	{	0,	"int16",				1,		1,		(void *) QB_TYPE_S16		},
	{	0,	"uint16",				1,		1,		(void *) QB_TYPE_U16		},
	{	0,	"int32",				1,		1,		(void *) QB_TYPE_S32		},
	{	0,	"uint32",				1,		1,		(void *) QB_TYPE_U32		},
	{	0,	"int64",				1,		2,		(void *) QB_TYPE_S64		},
	{	0,	"uint64",				1,		2,		(void *) QB_TYPE_U64		},
	{	0,	"float32",				1,		1,		(void *) QB_TYPE_F32		},
	{	0,	"float64",				1,		1,		(void *) QB_TYPE_F64		},
	{	0,	"defined",				1,		1,		NULL						},
	{	0,	"define",				2,		2,		NULL						},
	{	0,	"equal",				2,		2,		&factory_set_equal			},
	{	0,	"not_equal",			2,		2,		&factory_set_not_equal		},
	{	0,	"less_than",			2,		2,		&factory_set_less_than		},
	{	0,	"less_than_equal",		2,		2,		&factory_set_less_equal		},
	{	0,	"greater_than",			2,		2,		&factory_set_greater_than	},
	{	0,	"greater_than_equal",	2,		2,		&factory_set_greater_equal	},
	{	0,	"min",					1,		-1,		&factory_min				},
	{	0,	"max",					1,		-1,		&factory_max				},
	{	0,	"any",					1,		1,		&factory_any				},
	{	0,	"all",					1,		1,		&factory_all				},
	{	0,	"not",					1,		1,		&factory_set_not			},
	{	0,	"abs",					1,		1,		&factory_abs				},
	{	0,	"sin",					1,		1,		&factory_sin				},
	{	0,	"asin",					1,		1,		&factory_asin				},
	{	0,	"cos",					1,		1,		&factory_cos				},
	{	0,	"acos",					1,		1,		&factory_acos				},
	{	0,	"tan",					1,		1,		&factory_tan				},
	{	0,	"atan",					1,		1,		&factory_atan				},
	{	0,	"sinh",					1,		1,		&factory_sinh				},
	{	0,	"asinh",				1,		1,		&factory_asinh				},
	{	0,	"cosh",					1,		1,		&factory_cosh				},
	{	0,	"acosh",				1,		1,		&factory_acosh				},
	{	0,	"tanh",					1,		1,		&factory_tanh				},
	{	0,	"atanh",				1,		1,		&factory_atanh				},
	{	0,	"ceil",					1,		1,		&factory_ceil				},
	{	0,	"floor",				1,		1,		&factory_floor				},
	{	0,	"round",				1,		3,		&factory_round				},
	{	0,	"log",					1,		1,		&factory_log				},
	{	0,	"log1p",				1,		1,		&factory_log1p				},
	{	0,	"log10",				1,		1,		&factory_log10				},
	{	0,	"log2",					1,		1,		&factory_log2				},
	{	0,	"exp",					1,		1,		&factory_exp				},
	{	0,	"exp2",					1,		1,		&factory_exp2				},
	{	0,	"expm1",				1,		1,		&factory_expm1				},
	{	0,	"sqrt",					1,		1,		&factory_sqrt				},
	{	0,	"pow",					2,		2,		&factory_pow				},
	{	0,	"fmod",					2,		2,		&factory_modulo				},
	{	0,	"mod",					2,		2,		&factory_floor_modulo		},
	{	0,	"lcg_value",			0,		0,		&factory_lcg				},
	{	0,	"is_finite",			1,		1,		&factory_is_finite			},
	{	0,	"is_infinite",			1,		1,		&factory_is_infinite		},
	{	0,	"is_nan",				1,		1,		&factory_is_nan				},
	{	0,	"rand",					0,		2,		&factory_rand				},
	{	0,	"mt_rand",				0,		2,		&factory_mt_rand			},
	{	0,	"hypot",				2,		2,		&factory_hypot				},
	{	0,	"deg2rad",				1,		1,		&factory_deg2rad			},
	{	0,	"rad2deg",				1,		1,		&factory_rad2deg			},
	{	0,	"sign",					1,		1,		&factory_sign				},
	{	0,	"rsqrt",				1,		1,		&factory_rsqrt				},
	{	0,	"fract",				1,		1,		&factory_fract				},
	{	0,	"step",					2,		2,		&factory_step				},
	{	0,	"clamp",				3,		3,		&factory_clamp				},
	{	0,	"mix",					3,		3,		&factory_mix				},
	{	0,	"smooth_step",			3,		3,		&factory_smooth_step		},
/*
	{	0,	"normalize",			1,		1,		&factory_normalize			},
	{	0,	"length",				1,		1,		&factory_length				},
	{	0,	"distance",				2,		2,		&factory_distance			},
	{	0,	"dot",					2,		2,		&factory_dot_product		},
	{	0,	"cross",				2,		3,		&factory_cross_product		},
	{	0,	"faceforward",			2,		2,		&factory_faceforward		},
	{	0,	"reflect",				2,		2,		&factory_reflect			},
	{	0,	"refract",				3,		3,		&factory_refract			},
	{	0,	"mm_mult",				2,		2,		&factory_mm_multiply		},
	{	0,	"mv_mult",				2,		2,		&factory_mv_multiply		},
	{	0,	"vm_mult",				2,		2,		&factory_vm_multiply		},
	{	0,	"transpose",			1,		1,		&factory_transpose			},
	{	0,	"det",					1,		1,		&factory_determinant		},
	{	0,	"inverse",				1,		1,		&factory_inverse			},
	{	0,	"mm_mult_cm",			2,		2,		&factory_mm_multiply_cm		},
	{	0,	"mv_mult_cm",			2,		2,		&factory_mv_multiply_cm		},
	{	0,	"vm_mult_cm",			2,		2,		&factory_vm_multiply_cm		},
	{	0,	"transpose_cm",			1,		1,		&factory_transpose			},
	{	0,	"det_cm",				1,		1,		&factory_determinant		},
	{	0,	"inverse_cm",			1,		1,		&factory_inverse			},
	{	0,	"mm_mult_rm",			2,		2,		&factory_mm_multiply_rm		},
	{	0,	"mv_mult_rm",			2,		2,		&factory_mv_multiply_rm		},
	{	0,	"vm_mult_rm",			2,		2,		&factory_vm_multiply_rm		},
	{	0,	"transpose_rm",			1,		1,		&factory_transpose			},
	{	0,	"det_rm",				1,		1,		&factory_determinant		},
	{	0,	"inverse_rm",			1,		1,		&factory_inverse			},
	{	0,	"transform_cm",			2,		2,		&factory_transform_cm		},
	{	0,	"transform_rm",			2,		2,		&factory_transform_rm		},
	{	0,	"transform",			2,		2,		&factory_transform			},
	{	0,	"sample_nearest",		3,		3,		&factory_sample_nearest		},
	{	0,	"sample_bilinear",		3,		3,		&factory_sample_bilinear	},
	{	0,	"blend",				2,		2,		&factory_alpha_blend		},
	{	0,	"array_pop",			1,		1,		NULL						},
	{	0,	"array_shift",			1,		1,		NULL						},
	{	0,	"array_push",			2,		-1,		NULL						},
	{	0,	"array_unshift",		2,		-1,		NULL						},
	{	0,	"array_merge",			2,		-1,		&factory_array_merge		},
	{	0,	"array_fill",			3,		3,		&factory_array_fill			},
	{	0,	"array_pad",			3,		3,		&factory_array_pad			},
	{	0,	"array_slice",			2,		3,		NULL						},
	{	0,	"substr",				2,		3,		NULL						},
	{	0,	"array_sum",			1,		1,		&factory_array_sum			},
	{	0,	"array_product",		1,		1,		&factory_array_product		},
	{	0,	"array_search",			2,		2,		&factory_array_search		},
	{	0,	"in_array",				2,		2,		&factory_in_array			},
	{	0,	"array_pos",			2,		3,		&factory_subarray_pos		},
	{	0,	"strpos",				2,		3,		&factory_subarray_pos		},
	{	0,	"array_rpos",			2,		3,		&factory_subarray_rpos		},
	{	0,	"strrpos",				2,		3,		&factory_subarray_rpos		},
	{	0,	"sort",					1,		1,		&factory_sort				},
	{	0,	"rsort",				1,		1,		&factory_rsort				},
	{	0,	"shuffle",				1,		1,		&factory_shuffle			},
	{	0,	"array_reverse",		1,		1,		&factory_array_reverse		},
	{	0,	"array_splice",			2,		4,		NULL						},
	{	0,	"range",				2,		3,		&factory_range				},
	{	0,	"array_unique",			1,		1,		&factory_array_unique		},
	{	0,	"array_column",			2,		2,		&factory_array_column		},
	{	0,	"array_diff",			2,		-1,		&factory_array_diff			},
	{	0,	"array_filter",			1,		1,		&factory_array_diff			},
	{	0,	"array_intersect",		2,		-1,		&factory_array_intersect	},
	{	0,	"array_rand",			1,		2,		&factory_array_rand			},
	{	0,	"array_resize",			2,		-1,		&factory_array_resize		},
	{	0,	"pack_le",				1,		2,		&factory_pack_le			},
	{	0,	"unpack_le",			1,		3,		&factory_unpack_le			},
	{	0,	"pack_be",				1,		2,		&factory_pack_be			},
	{	0,	"unpack_be",			1,		3,		&factory_unpack_be			},
	{	0,	"utf8_decode",			1,		1,		&factory_utf8_decode		},
	{	0,	"utf8_encode",			1,		1,		&factory_utf8_encode		},
	{	0,	"cabs",					1,		1,		&factory_complex_abs		},
	{	0,	"carg",					1,		1,		&factory_complex_arg		},
	{	0,	"cmult",				2,		2,		&factory_complex_multiply	},
	{	0,	"cdiv",					2,		2,		&factory_complex_divide		},
	{	0,	"cexp",					1,		1,		&factory_complex_exp		},
	{	0,	"clog",					1,		1,		&factory_complex_log		},
	{	0,	"csqrt",				1,		1,		&factory_complex_sqrt		},
	{	0,	"cpow",					2,		2,		&factory_complex_pow		},
	{	0,	"csin",					1,		1,		&factory_complex_sin		},
	{	0,	"ccos",					1,		1,		&factory_complex_cos		},
	{	0,	"ctan",					1,		1,		&factory_complex_tan		},
	{	0,	"csinh",				1,		1,		&factory_complex_sinh		},
	{	0,	"ccosh",				1,		1,		&factory_complex_cosh		},
	{	0,	"ctanh",				1,		1,		&factory_complex_tanh		},
	{	0,	"rgb2hsv",				1,		1,		&factory_rgb2hsv			},
	{	0,	"hsv2rgb",				1,		1,		&factory_hsv2rgb			},
	{	0,	"rgb2hsl",				1,		1,		&factory_rgb2hsl			},
	{	0,	"hsl2rgb",				1,		1,		&factory_hsl2rgb			},
	{	0,	"rgb_premult",			1,		1,		&factory_apply_premult		},
	{	0,	"rgb_demult",			1,		1,		&factory_remove_premult		},
*/
	// unsupported functions
	{	0,	"compact",				0,		-1,		NULL						},
	{	0,	"extract",				0,		-1,		NULL						},
	{	0,	"each",					0,		-1,		NULL						},
	{	0,	"pos"	,				0,		-1,		NULL						},
	{	0,	"key"	,				0,		-1,		NULL						},
	{	0,	"current",				0,		-1,		NULL						},
	{	0,	"prev",					0,		-1,		NULL						},
	{	0,	"next",					0,		-1,		NULL						},
	{	0,	"reset",				0,		-1,		NULL						},
	{	0,	"end",					0,		-1,		NULL						},
	{	0,	"array_map",			0,		-1,		NULL						},
	{	0,	"array_reduce",			0,		-1,		NULL						},
	{	0,	"array_walk",			0,		-1,		NULL						},
};

#define MAX_INLINE_FUNCTION_NAME_LEN		32

static qb_php_function_translator * ZEND_FASTCALL qb_find_intrinsic_function(qb_php_translater_context *cxt, zval *callable) {
	const char *name = Z_STRVAL_P(callable);
	uint32_t len = Z_STRLEN_P(callable);

	if(len < MAX_INLINE_FUNCTION_NAME_LEN) {
		// make it lower case
		char name_buffer[MAX_INLINE_FUNCTION_NAME_LEN];
		uint32_t i;
		ulong hash_value;

		for(i = 0; i < len; i++) {
			name_buffer[i] = tolower(name[i]);
		}
		name_buffer[i] = '\0';

		// calculate the hash value and look up the function
		hash_value = zend_inline_hash_func(name_buffer, len + 1);
		for(i = 0; i < sizeof(intrinsic_functions) / sizeof(qb_php_function_translator); i++) {
			qb_php_function_translator *f = &intrinsic_functions[i];
			if(f->hash_value == hash_value && strcmp(name_buffer, f->name) == 0) {
				return f;
			}
		}
	}
	return NULL;
}

static void ZEND_FASTCALL qb_translate_instruction_range(qb_php_translater_context *cxt, uint32_t start_index, uint32_t end_index) {
	uint32_t zend_op_index;

	// save states
	zend_op_index = cxt->zend_op_index;

	// process zend instructions until we reach the end 
	// or if an instruction is already translated
	cxt->zend_op_index = start_index;
	cxt->zend_op = ZEND_OP(start_index);
	for(;;) {
		if(cxt->compiler_context->stage == QB_STAGE_RESULT_TYPE_RESOLUTION) {
			if(cxt->result_prototypes[cxt->zend_op_index].preliminary_type != QB_TYPE_UNKNOWN) {
				// the result prototype has been built already
				break;
			}
			qb_translate_current_instruction(cxt);
		} else if(cxt->compiler_context->stage == QB_STAGE_OPCODE_TRANSLATION) {
			uint32_t current_op_count;

			if(cxt->compiler_context->op_translations[cxt->zend_op_index] != QB_OP_INDEX_NONE && cxt->compiler_context->op_translations[cxt->zend_op_index] != QB_OP_INDEX_JUMP_TARGET) {
				// instruction has already been translated--do a jump there and exit
				qb_create_op(cxt->compiler_context, &factory_jump, NULL, 0, NULL, &cxt->zend_op_index, 1, FALSE);
				break;
			}

			// translate the current instruction, saving the op-count
			// so we know where the first resulting new op is
			current_op_count = cxt->compiler_context->op_count;
			qb_translate_current_instruction(cxt);

			// add a nop if new one wasn't generated
			if(current_op_count == cxt->compiler_context->op_count) {
				qb_create_op(cxt->compiler_context, &factory_nop, NULL, 0, NULL, 0, 0, TRUE);
			}

			// flag new op as a jump target if there's a placeholder in the position
			if(cxt->compiler_context->op_translations[cxt->zend_op_index] == QB_OP_INDEX_JUMP_TARGET) {
				qb_op *first_op = cxt->compiler_context->ops[current_op_count];
				first_op->flags |= QB_OP_JUMP_TARGET;
			}
			cxt->compiler_context->op_translations[cxt->zend_op_index] = current_op_count;
		}

		// see if it was a branch or a jump
		if(cxt->jump_target_index1) {
			uint32_t target_index1 = cxt->jump_target_index1;
			uint32_t target_index2 = cxt->jump_target_index2;
			cxt->jump_target_index1 = 0;
			cxt->jump_target_index2 = 0;
			if(target_index1 == QB_INSTRUCTION_END) {
				break;
			} else if(target_index2) {
				qb_translate_instruction_range(cxt, target_index1, target_index2);
			} else {
				cxt->zend_op = ZEND_OP(target_index1);
				cxt->zend_op_index = target_index1;
			}
			if(target_index2) {
				cxt->zend_op = ZEND_OP(target_index2);
				cxt->zend_op_index = target_index2;
			}
		} else {
			cxt->zend_op++;
			cxt->zend_op_index++;
		}
		if(cxt->zend_op_index == end_index) {
			break;
		}
	}

	// restore the state
	cxt->zend_op = ZEND_OP(zend_op_index);
	cxt->zend_op_index = zend_op_index;
}

void ZEND_FASTCALL qb_translate_instructions(qb_php_translater_context *cxt) {
	uint32_t i;
	cxt->compiler_context->op_translations = qb_allocate_indices(cxt->pool, cxt->zend_op_array->last);
	memset(cxt->compiler_context->op_translations, 0xFF, cxt->zend_op_array->last * sizeof(uint32_t));

	if(cxt->zend_op_array->T > 0) {
		qb_attach_new_array(cxt->pool, (void **) &cxt->temp_variables, &cxt->temp_variable_count, sizeof(qb_temporary_variable), cxt->zend_op_array->T);
		qb_enlarge_array((void **) &cxt->temp_variables, cxt->zend_op_array->T);

		// set the operand type to EMPTY (which is somewhat different from NONE)
		for(i = 0; i < cxt->temp_variable_count; i++) {
			qb_temporary_variable *temp_variable = &cxt->temp_variables[i];
			temp_variable->operand.type = QB_OPERAND_EMPTY;
		}
	}

	qb_attach_new_array(cxt->pool, (void **) &cxt->result_prototypes, &cxt->result_prototype_count, sizeof(qb_result_prototype), cxt->zend_op_array->last);
	qb_enlarge_array((void **) &cxt->result_prototypes, cxt->zend_op_array->last);
	for(i = 0; i < cxt->result_prototype_count; i++) {
		qb_result_prototype *prototype = &cxt->result_prototypes[i];
		prototype->preliminary_type = QB_TYPE_UNKNOWN;
		prototype->final_type = QB_TYPE_UNKNOWN;
	}

	cxt->compiler_context->stage = QB_STAGE_RESULT_TYPE_RESOLUTION;
	qb_translate_instruction_range(cxt, 0, cxt->zend_op_array->last);

	cxt->compiler_context->stage = QB_STAGE_OPCODE_TRANSLATION;
	for(i = 0; i < cxt->temp_variable_count; i++) {
		qb_temporary_variable *temp_variable = &cxt->temp_variables[i];
		temp_variable->operand.type = QB_OPERAND_EMPTY;
		temp_variable->operand.generic_pointer = NULL;
	}
	if(cxt->compiler_context->op_count > 0) {
		// there are instructions for initializing static variables
		// the first translated instruction is going to be a jump target
		cxt->compiler_context->op_translations[0] = QB_OP_INDEX_JUMP_TARGET;
		cxt->compiler_context->initialization_op_count = cxt->compiler_context->op_count;
	}
	qb_translate_instruction_range(cxt, 0, cxt->zend_op_array->last);

	// make sure there's always a RET at the end
	if(cxt->compiler_context->op_count == 0 || cxt->compiler_context->ops[cxt->compiler_context->op_count - 1]->opcode != QB_RET) {
		//qb_create_op(cxt, &factory_return, NULL, 0, NULL);
	}
}

void ZEND_FASTCALL qb_initialize_php_translater_context(qb_php_translater_context *cxt, qb_compiler_context *compiler_cxt TSRMLS_DC) {
	memset(cxt, 0, sizeof(qb_php_translater_context));
	cxt->pool = compiler_cxt->pool;
	cxt->compiler_context = compiler_cxt;
	if(compiler_cxt->function_declaration) {
		cxt->zend_op_array = &compiler_cxt->zend_function->op_array;
		cxt->zend_class = compiler_cxt->zend_function->op_array.scope;
	}
	SAVE_TSRMLS
}

void ZEND_FASTCALL qb_free_php_translater_context(qb_php_translater_context *cxt) {
}

int ZEND_FASTCALL qb_initialize_php_translater(TSRMLS_D) {
	uint32_t i;
	const char *pcre_error = NULL;
	int pcre_error_offset = 0;

	// calculate the hash of inline function names for quicker look-ups
	for(i = 0; i < sizeof(intrinsic_functions) / sizeof(qb_php_function_translator); i++) {
		qb_php_function_translator *f = &intrinsic_functions[i];
		f->hash_value = zend_inline_hash_func(f->name, strlen(f->name) + 1);
	}

	func_decl_hashes[FUNC_DECL_ENGINE] = zend_inline_hash_func("engine", 7);
	func_decl_hashes[FUNC_DECL_IMPORT] = zend_inline_hash_func("import", 7);
	func_decl_hashes[FUNC_DECL_PARAM] = zend_inline_hash_func("param", 6);
	func_decl_hashes[FUNC_DECL_LOCAL] = zend_inline_hash_func("local", 6);
	func_decl_hashes[FUNC_DECL_STATIC] = zend_inline_hash_func("static", 7);
	func_decl_hashes[FUNC_DECL_GLOBAL] = zend_inline_hash_func("global", 7);
	func_decl_hashes[FUNC_DECL_VAR] = zend_inline_hash_func("var", 4);
	func_decl_hashes[FUNC_DECL_PROPERTY] = zend_inline_hash_func("property", 9);
	func_decl_hashes[FUNC_DECL_RETURN] = zend_inline_hash_func("return", 7);

	// compile regular expressions
	doc_comment_function_regexp = pcre_compile(DOC_COMMENT_FUNCTION_REGEXP, PCRE_MULTILINE, &pcre_error, &pcre_error_offset, NULL);
	type_decl_regexp = pcre_compile(TYPE_DECL_REGEXP, 0, &pcre_error, &pcre_error_offset, NULL);
	type_dim_regexp = pcre_compile(TYPE_DIM_REGEXP, PCRE_CASELESS, &pcre_error, &pcre_error_offset, NULL);
	type_dim_alias_regexp = pcre_compile(TYPE_DIM_ALIAS_REGEXP, PCRE_CASELESS, &pcre_error, &pcre_error_offset, NULL);
	identifier_regexp = pcre_compile(IDENTIFIER_REGEXP, 0, &pcre_error, &pcre_error_offset, NULL);

	return (doc_comment_function_regexp && type_decl_regexp && type_dim_regexp && type_dim_alias_regexp && identifier_regexp) ? SUCCESS : FAILURE;
}