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

#define DOC_COMMENT_FUNCTION_REGEXP	"\\*\\s*@(?:(engine)|(import)|(param)|(local)|(shared)|(static|staticvar)|(global)|(lexical)|(var)|(property)|(return)|(receive)|(inline))\\s+(.*?)\\s*(?:\\*+\\/)?$"

enum {
	FUNC_DECL_ENGINE = 1,
	FUNC_DECL_IMPORT,
	FUNC_DECL_PARAM,
	FUNC_DECL_LOCAL,
	FUNC_DECL_SHARED,
	FUNC_DECL_STATIC,
	FUNC_DECL_GLOBAL,
	FUNC_DECL_LEXICAL,
	FUNC_DECL_VAR,
	FUNC_DECL_PROPERTY,
	FUNC_DECL_RETURN,
	FUNC_DECL_RECEIVE,
	FUNC_DECL_INLINE,
	FUNC_DECL_DATA,

	FUNC_DECL_TOKEN_COUNT,
};

ulong func_decl_hashes[FUNC_DECL_TOKEN_COUNT];

#define TYPE_DECL_REGEXP			"^\\s*(?:(?:(u?int(\\d*))|(float(\\d*)))|(void)|(integer)|(double)|(char)|(bool|boolean)|(string)|(image[1234]?)|(array)|(object)|(resource)|(mixed)|(callback))\\s*((?:\\[.*?\\])*)\\s*"

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
	int next_offset = -1;

	matches = pcre_exec(type_dim_regexp, NULL, s, len, 0, 0, offsets, sizeof(offsets) / sizeof(int));
	if(matches > 0) {
		next_offset = offsets[1];
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
						qb_report_illegal_dimension_exception(cxt->line_id, const_value);
						free_alloca(name, use_heap);
						next_offset = -1;
					}
					dimension = const_value;
				} else if(Z_TYPE_P(constant) == IS_STRING) {
					char *expanded;
					uint32_t expanded_len = spprintf(&expanded, 0, "[%.*s]", Z_STRLEN_P(constant), Z_STRVAL_P(constant));
					if(qb_parse_type_dimension(cxt, expanded, expanded_len, decl, dimension_index)) {
						next_offset = offsets[1];
					}
					efree(expanded);
					free_alloca(name, use_heap);
					return next_offset;
				} else {
					decl = NULL;
				}
			} else {
				qb_report_undefined_constant_exception(cxt->line_id, NULL, name);
				next_offset = -1;
			}
			free_alloca(name, use_heap);
		} else if(FOUND_GROUP(TYPE_DIM_ASTERISK)) {
			if(dimension_index == 0) {
				decl->flags |= QB_TYPE_DECL_AUTOVIVIFICIOUS;
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

			next_offset = offsets[1];

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
		}
	}
	decl->dimensions[dimension_index] = dimension;
	return next_offset;
}

static qb_type_declaration * qb_parse_type_declaration(qb_parser_context *cxt, const char *s, uint32_t len, uint32_t var_type) {
	qb_type_declaration *decl = NULL;
	int offsets[64], matches;

	matches = pcre_exec(type_decl_regexp, NULL, s, len, 0, 0, offsets, sizeof(offsets) / sizeof(int));
	if(matches != -1) {
		uint32_t end_index = offsets[1];
		uint32_t base_dimension_count = 0;
		uint32_t base_dimensions[MAX_DIMENSION];

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
			decl->flags |= QB_TYPE_DECL_BOOLEAN;
		} else if(FOUND_GROUP(TYPE_DECL_STRING) && !FOUND_GROUP(TYPE_DECL_DIMENSIONS)) {
			decl->type = QB_TYPE_U08;
			decl->flags |= QB_TYPE_DECL_AUTOVIVIFICIOUS | QB_TYPE_DECL_STRING;
			base_dimension_count = 1;
			base_dimensions[0] = 0;
		} else if(FOUND_GROUP(TYPE_DECL_IMAGE)) {
			uint32_t identifier_len = GROUP_LENGTH(TYPE_DECL_IMAGE);
			decl->type = QB_TYPE_F32;
			decl->flags |= QB_TYPE_DECL_IMAGE;
			base_dimension_count = 3;
			base_dimensions[0] = 0;
			base_dimensions[1] = 0;
			if(identifier_len == 6) {
				const char *identifier = s + GROUP_OFFSET(TYPE_DECL_IMAGE);
				base_dimensions[2] = identifier[identifier_len - 1] - '0';
			} else {
				base_dimensions[2] = 4;
			}
		} else {
			return NULL;
		}

		if(FOUND_GROUP(TYPE_DECL_DIMENSIONS)) {
			uint32_t dimension_count = 0, i, j;
			uint32_t dimension_len = GROUP_LENGTH(TYPE_DECL_DIMENSIONS);
			const char *dimension_string = s + GROUP_OFFSET(TYPE_DECL_DIMENSIONS);

			// see how many brackets there are
			for(i = 0; i < dimension_len; i++) {
				if(dimension_string[i] == '[') {
					dimension_count++;
				}
			}

			if(dimension_count + base_dimension_count > MAX_DIMENSION) {
				return NULL;
			}

			decl->dimensions = qb_allocate_indices(cxt->pool, dimension_count + base_dimension_count);
			decl->dimension_count = dimension_count + base_dimension_count;

			for(i = 0; i < dimension_count; i++) {
				int32_t processed = qb_parse_type_dimension(cxt, dimension_string, dimension_len, decl, i);
				if(processed == -1) {
					return NULL;
				}
				dimension_string += processed;
				dimension_len -= processed;
			}
			for(j = 0; j < base_dimension_count; j++, i++) {
				decl->dimensions[i] = base_dimensions[j];
			}
		} else if(base_dimension_count > 0) {
			uint32_t i;
			decl->dimensions = qb_allocate_indices(cxt->pool, base_dimension_count);
			decl->dimension_count = base_dimension_count;
			for(i = 0; i < base_dimension_count; i++) {
				decl->dimensions[i] = base_dimensions[i];
			}
		}

		if(var_type && var_type != QB_VARIABLE_RETURN_VALUE && var_type != QB_VARIABLE_SENT_VALUE) {
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

static void qb_report_unexpected_doc_comment_tag(qb_parser_context *cxt, const char *comment, uint32_t comment_length, int matches, int *offsets) {
	int i;
	for(i = 1; i < matches; i++) {
		if(FOUND_GROUP(i)) {
			uint32_t tag_len = GROUP_LENGTH(i);
			uint32_t start_index = GROUP_OFFSET(i);
			const char *tag = comment + start_index;

			qb_find_doc_comment_line_number(cxt, comment, comment_length, start_index);
			qb_report_unexpected_tag_in_doc_comment_exception(cxt->line_id, tag, tag_len);
			break;
		}
	}
}

static void qb_report_doc_comment_syntax_issue(qb_parser_context *cxt, const char *comment, uint32_t comment_length, int offset) {
	qb_find_doc_comment_line_number(cxt, comment, comment_length, offset);
	qb_report_doc_comment_syntax_exception(cxt->line_id);
}

qb_function_declaration * qb_parse_function_doc_comment(qb_parser_context *cxt, zend_op_array *op_array) {
	USE_TSRM
	qb_function_declaration *function_decl = NULL;
	const char *doc_comment = op_array->doc_comment;
	uint32_t doc_comment_len = op_array->doc_comment_len;
	int offsets[48], matches;
	uint32_t start_index = 0;
	int32_t use_qb = FALSE;

	cxt->file_path = op_array->filename;
	cxt->file_id = qb_get_source_file_id(cxt->file_path TSRMLS_CC);
	cxt->line_number_max = op_array->line_start;

	function_decl = qb_allocate_function_declaration(cxt->pool);
	function_decl->zend_op_array = op_array;

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
			} else if(FOUND_GROUP(FUNC_DECL_INLINE)) {
				if(data_len == 5 && strncmp(data, "never", 5) == 0) {
					function_decl->flags |= QB_FUNCTION_NEVER_INLINE;
				} else if(data_len == 6 && strncmp(data, "always", 6) == 0) {
					function_decl->flags |= QB_FUNCTION_INLINE_ALWAYS;
				}
			} else {
				if(FOUND_GROUP(FUNC_DECL_GLOBAL)) {
					var_type = QB_VARIABLE_GLOBAL;
				} else if(FOUND_GROUP(FUNC_DECL_LOCAL)) {
					var_type = QB_VARIABLE_LOCAL;
				} else if(FOUND_GROUP(FUNC_DECL_SHARED)) {
					var_type = QB_VARIABLE_SHARED;
				} else if(FOUND_GROUP(FUNC_DECL_LEXICAL)) {
					var_type = QB_VARIABLE_LEXICAL;
				} else if(FOUND_GROUP(FUNC_DECL_PARAM)) {
					var_type = QB_VARIABLE_ARGUMENT;
				} else if(FOUND_GROUP(FUNC_DECL_STATIC)) {
					var_type = QB_VARIABLE_STATIC | QB_VARIABLE_CLASS;
				} else if(FOUND_GROUP(FUNC_DECL_PROPERTY)) {
					var_type = FUNC_DECL_PROPERTY;
				} else if(FOUND_GROUP(FUNC_DECL_RETURN)) {
					var_type = QB_VARIABLE_RETURN_VALUE;
				} else if(FOUND_GROUP(FUNC_DECL_RECEIVE)) {
					var_type = QB_VARIABLE_SENT_VALUE;
				} else {
					if(use_qb) {
						qb_report_unexpected_doc_comment_tag(cxt, doc_comment, doc_comment_len, matches, offsets);
					}
				}
				decl = qb_parse_type_declaration(cxt, data, data_len, var_type);
				if(decl) {
					qb_add_variable_declaration(function_decl, decl);
				} else {
					if(use_qb) {
						qb_report_doc_comment_syntax_issue(cxt, doc_comment, doc_comment_len, data_offset);
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
	USE_TSRM
	qb_class_declaration *class_decl;
	const char *doc_comment = Z_CLASS_INFO(ce, doc_comment);
	uint32_t doc_comment_len = Z_CLASS_INFO(ce, doc_comment_len);
	int offsets[48], matches;
	uint32_t start_index = 0;
	Bucket *p;

	cxt->file_path = Z_CLASS_INFO(ce, filename);
	cxt->file_id = qb_get_source_file_id(cxt->file_path TSRMLS_CC);
	cxt->line_number_max = Z_CLASS_INFO(ce, line_start);

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
				qb_report_unexpected_doc_comment_tag(cxt, doc_comment, doc_comment_len, matches, offsets);
			} 
			if(var_type) {
				decl = qb_parse_type_declaration(cxt, data, data_len, var_type);
				if(decl) {
					qb_add_class_variable_declaration(class_decl, decl);
				} else {
					qb_report_doc_comment_syntax_issue(cxt, doc_comment, doc_comment_len, data_offset);
				}
			}
		} else {
			break;
		}
	}

	// look for the doc comment of the properties start from the end of the class
	cxt->line_number_max = Z_CLASS_INFO(ce, line_end);

	for(p = ce->properties_info.pListHead; p; p = p->pListNext) {
		zend_property_info *prop = p->pData;
		const char *doc_comment = prop->doc_comment;
		uint32_t doc_comment_len = (uint32_t) prop->doc_comment_len;
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
							qb_report_doc_comment_syntax_issue(cxt, doc_comment, doc_comment_len, data_offset);
						}
					} else {
						qb_report_unexpected_doc_comment_tag(cxt, doc_comment, doc_comment_len, matches, offsets);
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
		func_decl_hashes[FUNC_DECL_LEXICAL] = zend_hash_func("lexical", 8);
		func_decl_hashes[FUNC_DECL_VAR] = zend_hash_func("var", 4);
		func_decl_hashes[FUNC_DECL_PROPERTY] = zend_hash_func("property", 9);
		func_decl_hashes[FUNC_DECL_RETURN] = zend_hash_func("return", 7);
		func_decl_hashes[FUNC_DECL_RECEIVE] = zend_hash_func("receive", 8);
		func_decl_hashes[FUNC_DECL_INLINE] = zend_hash_func("inline", 7);
		func_decl_hashes[FUNC_DECL_RETURN] = zend_hash_func("return", 7);
	}

	cxt->pool = pool;
	cxt->zend_class = ce;
	cxt->file_path = NULL;
	cxt->line_number_max = 0;
	cxt->file_id = 0;
	cxt->line_id = 0;

	SAVE_TSRMLS
}

void qb_free_parser_context(qb_parser_context *cxt) {
}
