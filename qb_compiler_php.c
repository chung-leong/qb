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

extern const char compressed_table_zend_op_names[];

static zend_always_inline const char * qb_get_zend_op_name(qb_compiler_context *cxt, uint32_t opcode) {
	if(!cxt->pool->zend_op_names) {
		qb_uncompress_table(compressed_table_zend_op_names, (void ***) &cxt->pool->zend_op_names, NULL, 0);
		if(!cxt->pool->zend_op_names) {
			qb_abort("Unable to decompress table");
		}
	}
	return cxt->pool->zend_op_names[opcode];
}

static void ZEND_FASTCALL qb_print_zend_ops(qb_compiler_context *cxt) {
	uint32_t i = 0;
	for(i = 0; i < cxt->zend_op_array->last; i++) {
		zend_op *zop = &cxt->zend_op_array->opcodes[i];
		const char *opname = qb_get_zend_op_name(cxt, zop->opcode);
		php_printf("%s (line number: %d)\n", opname, zop->lineno);
	}
}

static uint32_t ZEND_FASTCALL qb_get_zend_array_dimension_count(qb_compiler_context *cxt, zval *zvalue, int32_t element_type) {
	if((Z_TYPE_P(zvalue) == IS_ARRAY || Z_TYPE_P(zvalue) == IS_CONSTANT_ARRAY)) {
		HashTable *ht = Z_ARRVAL_P(zvalue);
		Bucket *p;
		uint32_t overall_sub_array_dimension_count = 0;
		for(p = ht->pListHead; p; p = p->pNext) {
			zval **p_element = p->pData;
			uint32_t sub_array_dimension_count = qb_get_zend_array_dimension_count(cxt, *p_element, element_type);
			if(overall_sub_array_dimension_count) {
				if(overall_sub_array_dimension_count != sub_array_dimension_count) {
					qb_abort("Array has irregular structure");
				}
			} else {
				overall_sub_array_dimension_count = sub_array_dimension_count;
			}
		}
		if(overall_sub_array_dimension_count + 1 > MAX_DIMENSION) {
			qb_abort("Array initializer has too many dimensions");
		}
		return overall_sub_array_dimension_count + 1;
	} else if(Z_TYPE_P(zvalue) == IS_STRING) {
		return 1;
	} else {
		return 0;
	}
}

static void ZEND_FASTCALL qb_get_zend_array_dimensions(qb_compiler_context *cxt, zval *zvalue, int32_t element_type, uint32_t *dimensions, uint32_t dimension_count) {
	if((Z_TYPE_P(zvalue) == IS_ARRAY || Z_TYPE_P(zvalue) == IS_CONSTANT_ARRAY)) {
		HashTable *ht = Z_ARRVAL_P(zvalue);
		Bucket *p;
		int32_t highest_index = -1;
		uint32_t dimension;
		for(p = ht->pListHead; p; p = p->pListNext) {
			if(p->nKeyLength > 0) {
				qb_abort("String key encountered (%s)", p->arKey);
			}
			if((long) p->h < 0) {
				qb_abort("Negative index encountered (%lu)", p->h);
			}
			if((long) p->h > highest_index) {
				highest_index = p->h;
			}
		}
		dimension = highest_index + 1;
		if(dimension > dimensions[0]) {
			dimensions[0] = dimension;
		}
		if(dimension_count > 1) {
			dimensions[1] = 0;
			for(p = ht->pListHead; p; p = p->pListNext) {
				zval **p_element = p->pData;
				qb_get_zend_array_dimensions(cxt, *p_element, element_type, dimensions + 1, dimension_count - 1);
			}
		}
	} else if(Z_TYPE_P(zvalue) == IS_STRING) {
		uint32_t byte_count = Z_STRLEN_P(zvalue);
		uint32_t dimension = byte_count >> type_size_shifts[element_type];
		if(byte_count != dimension * type_sizes[element_type]) {
			qb_abort("Number of bytes in string (%d) is not divisible by %d", byte_count, type_sizes[element_type]);
		}
		if(dimension > dimensions[0]) {
			dimensions[0] = dimension;
		}
	}
}

static void ZEND_FASTCALL qb_copy_element_from_zval(qb_compiler_context *cxt, zval *zvalue, qb_address *address) {
	if(Z_TYPE_P(zvalue) == IS_LONG) {
		switch(address->type) {
			case QB_TYPE_S08: VALUE(S08, address) = (CTYPE(S08)) Z_LVAL_P(zvalue); break;
			case QB_TYPE_U08: VALUE(U08, address) = (CTYPE(U08)) Z_LVAL_P(zvalue); break;
			case QB_TYPE_S16: VALUE(S16, address) = (CTYPE(S16)) Z_LVAL_P(zvalue); break;
			case QB_TYPE_U16: VALUE(U16, address) = (CTYPE(U16)) Z_LVAL_P(zvalue); break;
			case QB_TYPE_S32: VALUE(S32, address) = (CTYPE(S32)) Z_LVAL_P(zvalue); break;
			case QB_TYPE_U32: VALUE(U32, address) = (CTYPE(U32)) Z_LVAL_P(zvalue); break;
			case QB_TYPE_S64: VALUE(S64, address) = (CTYPE(S64)) Z_LVAL_P(zvalue); break;
			case QB_TYPE_U64: VALUE(U64, address) = (CTYPE(U64)) Z_LVAL_P(zvalue); break;
			case QB_TYPE_F32: VALUE(F32, address) = (CTYPE(F32)) Z_LVAL_P(zvalue); break;
			case QB_TYPE_F64: VALUE(F64, address) = (CTYPE(F64)) Z_LVAL_P(zvalue); break;
		}
	} else if(Z_TYPE_P(zvalue) == IS_DOUBLE) {
		switch(address->type) {
			case QB_TYPE_S08: VALUE(S08, address) = (CTYPE(S08)) (CTYPE(S64)) Z_DVAL_P(zvalue); break;
			case QB_TYPE_U08: VALUE(U08, address) = (CTYPE(U08)) (CTYPE(S64)) Z_DVAL_P(zvalue); break;
			case QB_TYPE_S16: VALUE(S16, address) = (CTYPE(S16)) (CTYPE(S64)) Z_DVAL_P(zvalue); break;
			case QB_TYPE_U16: VALUE(U16, address) = (CTYPE(U16)) (CTYPE(S64)) Z_DVAL_P(zvalue); break;
			case QB_TYPE_S32: VALUE(S32, address) = (CTYPE(S32)) (CTYPE(S64)) Z_DVAL_P(zvalue); break;
			case QB_TYPE_U32: VALUE(U32, address) = (CTYPE(U32)) (CTYPE(S64)) Z_DVAL_P(zvalue); break;
			case QB_TYPE_S64: VALUE(S64, address) = (CTYPE(S64)) Z_DVAL_P(zvalue); break;
			case QB_TYPE_U64: VALUE(U64, address) = (CTYPE(U64)) Z_DVAL_P(zvalue); break;
			case QB_TYPE_F32: VALUE(F32, address) = (CTYPE(F32)) Z_DVAL_P(zvalue); break;
			case QB_TYPE_F64: VALUE(F64, address) = (CTYPE(F64)) Z_DVAL_P(zvalue); break;
		}
	} else if(Z_TYPE_P(zvalue) == IS_STRING) {
		uint32_t type_size = type_sizes[address->type];
		uint32_t string_len = Z_STRLEN_P(zvalue);
		const char *string = Z_STRVAL_P(zvalue);
		if(type_size != string_len) {
			qb_abort("Cannot convert string to %s due to size mismatch", type_names[address->type]);
		}
		switch(address->type) {
			case QB_TYPE_S08: VALUE(S08, address) = *((CTYPE(S08) *) string); break;
			case QB_TYPE_U08: VALUE(U08, address) = *((CTYPE(U08) *) string); break;
			case QB_TYPE_S16: VALUE(S16, address) = *((CTYPE(S16) *) string); break;
			case QB_TYPE_U16: VALUE(U16, address) = *((CTYPE(U16) *) string); break;
			case QB_TYPE_S32: VALUE(S32, address) = *((CTYPE(S32) *) string); break;
			case QB_TYPE_U32: VALUE(U32, address) = *((CTYPE(U32) *) string); break;
			case QB_TYPE_S64: VALUE(S64, address) = *((CTYPE(S64) *) string); break;
			case QB_TYPE_U64: VALUE(U64, address) = *((CTYPE(U64) *) string); break;
			case QB_TYPE_F32: VALUE(F32, address) = *((CTYPE(F32) *) string); break;
			case QB_TYPE_F64: VALUE(F64, address) = *((CTYPE(F64) *) string); break;
		}
	} else if(Z_TYPE_P(zvalue) == IS_ARRAY || Z_TYPE_P(zvalue) == IS_CONSTANT_ARRAY) {
		switch(address->type) {
			case QB_TYPE_S64:
			case QB_TYPE_U64: {
				VALUE(I64, address) = qb_zval_array_to_int64(zvalue);
			}	break;
			default:
				qb_abort("Cannot convert an array to %s", type_names[address->type]);
		}
	}
}

static void ZEND_FASTCALL qb_copy_elements_from_zend_array(qb_compiler_context *cxt, zval *zvalue, qb_address *address) {
	qb_address *dimension_address = address->dimension_addresses[0];
	uint32_t dimension = VALUE(U32, dimension_address);
	int32_t element_type = address->type;
	uint32_t element_size = type_sizes[element_type], i;

	if((Z_TYPE_P(zvalue) == IS_ARRAY || Z_TYPE_P(zvalue) == IS_CONSTANT_ARRAY)) {
		HashTable *ht = Z_ARRVAL_P(zvalue);
		if(address->dimension_count > 1) {
			qb_address _sub_array_address, *sub_array_address = &_sub_array_address;
			uint32_t sub_array_array_size, sub_array_size;
			qb_initialize_subarray_address(cxt, address, sub_array_address);
			sub_array_array_size = VALUE(U32, sub_array_address->array_size_address);
			sub_array_size = element_size * sub_array_array_size;

			for(i = 0; i < dimension; i++) {
				zval **p_element;
				if(zend_hash_index_find(ht, i, (void **) &p_element) == SUCCESS) {
					qb_copy_elements_from_zend_array(cxt, *p_element, sub_array_address);
				} else {
					memset(ARRAY(I08, sub_array_address), 0, sub_array_size);
				}
				sub_array_address->segment_offset += sub_array_size;
			}
		} else {
			qb_address _element_address, *element_address = &_element_address;
			qb_initialize_element_address(cxt, address, element_address);

			for(i = 0; i < dimension; i++) {
				zval **p_element;
				if(zend_hash_index_find(ht, i, (void **) &p_element) == SUCCESS) {
					qb_copy_element_from_zval(cxt, *p_element, element_address);
				} else {
					memset(ARRAY(I08, element_address), 0, element_size);
				}
				element_address->segment_offset += element_size;
			}
		}
	} else if(Z_TYPE_P(zvalue) == IS_STRING) {
		uint32_t byte_count = Z_STRLEN_P(zvalue);
		uint32_t space_available = dimension * element_size;
		int8_t *memory = ARRAY(I08, address);
		memcpy(memory, Z_STRVAL_P(zvalue), byte_count);
		memset(memory + byte_count, 0, space_available - byte_count);
	}
}

static qb_address * ZEND_FASTCALL qb_obtain_zval_constant(qb_compiler_context *cxt, zval *zvalue, uint32_t desired_type) {
	if((Z_TYPE_P(zvalue) == IS_ARRAY || Z_TYPE_P(zvalue) == IS_CONSTANT_ARRAY) || Z_TYPE_P(zvalue) == IS_STRING) {
		qb_address *address;

		// figure out the dimensions of the array first
		uint32_t dimensions[MAX_DIMENSION];
		uint32_t dimension_count = qb_get_zend_array_dimension_count(cxt, zvalue, desired_type);
		dimensions[0] = 0;
		qb_get_zend_array_dimensions(cxt, zvalue, desired_type, dimensions, dimension_count);

		// create a local array for it
		address = qb_create_fixed_size_multidimensional_array(cxt, desired_type, dimensions, dimension_count, FALSE);
		address->flags |= QB_ADDRESS_CONSTANT | QB_ADDRESS_INITIALIZED;

		// copy the elements
		qb_copy_elements_from_zend_array(cxt, zvalue, address);

		if(Z_TYPE_P(zvalue) == IS_STRING) {
			if((desired_type & ~QB_TYPE_UNSIGNED) == QB_TYPE_I08) {
				// mark it as something that should print out as text
				address->flags |= QB_ADDRESS_STRING;
			}
		}
		return address;
	} else {
		switch(desired_type) {
			case QB_TYPE_S08: return qb_obtain_constant_S08(cxt, (CTYPE(S08)) qb_zval_to_long(zvalue));
			case QB_TYPE_U08: return qb_obtain_constant_U08(cxt, (CTYPE(U08)) qb_zval_to_long(zvalue));
			case QB_TYPE_S16: return qb_obtain_constant_S16(cxt, (CTYPE(S16)) qb_zval_to_long(zvalue));
			case QB_TYPE_U16: return qb_obtain_constant_U16(cxt, (CTYPE(U16)) qb_zval_to_long(zvalue));
			case QB_TYPE_S32: return qb_obtain_constant_S32(cxt, (CTYPE(S32)) qb_zval_to_long(zvalue));
			case QB_TYPE_U32: return qb_obtain_constant_U32(cxt, (CTYPE(U32)) qb_zval_to_long(zvalue));
			case QB_TYPE_S64: return qb_obtain_constant_S64(cxt, (CTYPE(S64)) qb_zval_to_long(zvalue));
			case QB_TYPE_U64: return qb_obtain_constant_U64(cxt, (CTYPE(U64)) qb_zval_to_long(zvalue));
			case QB_TYPE_F32: return qb_obtain_constant_F32(cxt, (CTYPE(F32)) qb_zval_to_double(zvalue));
			case QB_TYPE_F64: return qb_obtain_constant_F64(cxt, (CTYPE(F64)) qb_zval_to_double(zvalue));
		}
	}
	qb_abort("Unable to convert constant");
	return NULL;
}

static uint32_t qb_find_doc_comment_line_number(const char *comment, uint32_t comment_length, uint32_t offset, const char *filepath, uint32_t line_number_max) {
	// load the file
	uint32_t line_number = 0;
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
	QB_G(current_filename) = filepath;
	QB_G(current_line_number) = line_number;
	qb_abort("Syntax error encountered while parsing Doc Comments for type information");
}

static qb_function_declaration * ZEND_FASTCALL qb_parse_function_declaration_table(qb_compiler_data_pool *pool, zend_function *zfunc, HashTable *ht) {
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
							} else if(data_len == 8 && strncmp(data, "qb-native", 9) == 0) {
								function_decl->flags |= QB_ENGINE_COMPILE_IF_POSSIBLE;
							} else {
								error_zval = element;
							}
						}	break;
						case FUNC_DECL_IMPORT: {
						}	break;
						case FUNC_DECL_RETURN: {
							decl = qb_parse_type_declaration(pool, data, data_len, QB_VARIABLE_RETURN_VALUE);
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
								decl = qb_parse_type_declaration(pool, data, data_len, var_type);
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

static qb_function_declaration * ZEND_FASTCALL qb_parse_function_doc_comment(qb_compiler_data_pool *pool, zend_function *zfunc) {
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
			if(matches != -1) {
				uint32_t data_offset = GROUP_OFFSET(FUNC_DECL_DATA), data_len = GROUP_LENGTH(FUNC_DECL_DATA), var_type;
				const char *data = s + data_offset;
				qb_type_declaration *decl;

				if(FOUND_GROUP(FUNC_DECL_ENGINE)) {
					if(data_len == 2 && strncmp(data, "qb", 2) == 0) {
						use_qb = TRUE;
					} else if(data_len == 11 && strncmp(data, "qb-bytecode", 11) == 0) {
						use_qb = TRUE;
						function_decl->flags |= QB_ENGINE_NEVER_COMPILE;
					} else if(data_len == 8 && strncmp(data, "qb-native", 9) == 0) {
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
					decl = qb_parse_type_declaration(pool, data, data_len, var_type);
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

static qb_class_declaration * ZEND_FASTCALL qb_parse_class_doc_comment(qb_compiler_data_pool *pool, zend_class_entry *ce) {
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
			if(matches != -1) {
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
				decl = qb_parse_type_declaration(pool, data, data_len, var_type);
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

		for(start_index = 0; start_index < len; start_index = offsets[1]) {
			matches = pcre_exec(doc_comment_function_regexp, NULL, s, len, start_index, 0, offsets, sizeof(offsets) / sizeof(int));
			if(matches != -1) {
				uint32_t data_offset = GROUP_OFFSET(FUNC_DECL_DATA), data_len = GROUP_LENGTH(FUNC_DECL_DATA);
				const char *data = s + data_offset;
				qb_type_declaration *decl;

				if(FOUND_GROUP(FUNC_DECL_VAR)) {
					decl = qb_parse_type_declaration(pool, data, data_len, 0);
					if(decl) {
						if(prop->flags & (ZEND_ACC_PRIVATE | ZEND_ACC_PROTECTED)) {
							decl->name = prop->name + ce->name_length + 2;
							decl->name_length = prop->name_length - (ce->name_length + 2);
							decl->hash_value = zend_inline_hash_func(decl->name, decl->name_length + 1);
						} else {
							decl->name = prop->name;
							decl->name_length = prop->name_length;
							decl->hash_value = prop->h;
						}
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
			}
		}
	}

	return class_decl;
}

static uint32_t ZEND_FASTCALL qb_get_zval_type(qb_compiler_context *cxt, zval *zvalue, uint32_t flags);

static uint32_t ZEND_FASTCALL qb_get_zval_array_type(qb_compiler_context *cxt, zval *array, uint32_t flags) {
	HashTable *ht = Z_ARRVAL_P(array);
	Bucket *p;
	uint32_t highest_rank_type = 0;

	for(p = ht->pListHead; p; p = p->pNext) {
		zval **p_element = p->pData;
		uint32_t type = qb_get_zval_type(cxt, *p_element, flags);
		if(type > highest_rank_type) {
			highest_rank_type = type;
		}
	}
	return highest_rank_type;
}

static uint32_t ZEND_FASTCALL qb_get_zval_type(qb_compiler_context *cxt, zval *zvalue, uint32_t flags) {
	uint32_t type;
	switch(Z_TYPE_P(zvalue)) {
		case IS_BOOL: {
			type = QB_TYPE_I32;
		}	break;
		case IS_LONG: {
			int value = Z_LVAL_P(zvalue);
			if(flags & QB_COERCE_TO_FLOATING_POINT) {
				if(flags & QB_COERCE_TO_INTEGER_TO_DOUBLE) {
					type = QB_TYPE_F64;
				} else {
					// a single precision fp has 23 bits of precision
					type = (-0x007FFFFF <= value && value <= 0x007FFFFF) ?  QB_TYPE_F32 : QB_TYPE_F64;
				}
			} else {
				if(flags & QB_COERCE_TO_UNSIGNED) {
					if(value <= UINT8_MAX) {
						type = QB_TYPE_U08;
					} else if((unsigned int) value <= UINT16_MAX) {
						type = QB_TYPE_U16;
#if SIZEOF_LONG == 8
					} else if((unsigned int) value <= UINT32_MAX) {
						type = QB_TYPE_U32;
					} else {
						type = QB_TYPE_U64;
					}
#else
					} else {
						type = QB_TYPE_U32;
					}
#endif
				} else {
					if(INT8_MIN <= value && value <= INT8_MAX) {
						type = QB_TYPE_I08;
					} else if(INT16_MIN <= value && value <= INT16_MAX) {
						type = QB_TYPE_I16;
#if SIZEOF_LONG == 8
					} else if(INT32_MIN <= value && value <= INT32_MAX) {
						type = QB_TYPE_I32;
					} else {
						type = QB_TYPE_I64;
					}
#else
					} else {
						type = QB_TYPE_I32;
					}
#endif
				}
			}	break;
		}	break;
		case IS_DOUBLE: {
			double value = Z_DVAL_P(zvalue);
			if(flags & QB_COERCE_TO_INTEGER) {
				int64_t int_value = (int64_t) value;
				if(flags & QB_COERCE_TO_UNSIGNED) {
					if((uint64_t) int_value <= UINT8_MAX) {
						type = QB_TYPE_U08;
					} else if((uint64_t) int_value <= UINT16_MAX) {
						type = QB_TYPE_I16;
					} else if((uint64_t) int_value <= UINT32_MAX) {
						type = QB_TYPE_I32;
					} else {
						type = QB_TYPE_I64;
					}
				} else {
					if(INT8_MIN <= int_value && int_value <= INT8_MAX) {
						type = QB_TYPE_I08;
					} else if(INT16_MIN <= int_value && int_value <= INT16_MAX) {
						type = QB_TYPE_I16;
					} else if(INT32_MIN <= int_value && int_value <= INT32_MAX) {
						type = QB_TYPE_I32;
					} else {
						type = QB_TYPE_I64;
					}
				}
			} else {
				float32_t value32 = (float32_t) value;
				if((double) value32 == value) {
					// value can be stored as F32
					type = QB_TYPE_F32;
				} else {
					// value must be stored as F64
					type = QB_TYPE_F64;
				}
			}
		}	break;
		case IS_STRING: {
			type = QB_TYPE_U08;
		}	break;
		case IS_ARRAY: {
			type = qb_get_zval_array_type(cxt, zvalue, flags);
		}	break;
	}
	return type;
}

static qb_variable * ZEND_FASTCALL qb_find_variable(qb_compiler_context *cxt, zend_class_entry *class, zval *name, uint32_t type_mask) {
	uint32_t i;
	ulong hash_value = Z_HASH_P(name);
	for(i = 0; i < cxt->variable_count; i++) {
		qb_variable *qvar = cxt->variables[i];
		if(qvar->hash_value == hash_value && qvar->name_length == Z_STRLEN_P(name)) {
			if(strncmp(qvar->name, Z_STRVAL_P(name), Z_STRLEN_P(name)) == 0) {
				if(qvar->zend_class == class) {
					if(qvar->flags & type_mask || !type_mask) {
						return qvar;
					}
				}
			}
		}
	}
	return NULL;
}

static qb_variable * ZEND_FASTCALL qb_obtain_class_variable(qb_compiler_context *cxt, zend_class_entry *class, zval *name) {
	qb_variable *qvar = qb_find_variable(cxt, class, name, QB_VARIABLE_CLASS | QB_VARIABLE_CLASS_INSTANCE);
	if(!qvar) {
		qvar = qb_allocate_variable(cxt->pool);
		qvar->flags = (class) ? QB_VARIABLE_CLASS : QB_VARIABLE_CLASS_INSTANCE;
		qvar->name = Z_STRVAL_P(name);
		qvar->name_length = Z_STRLEN_P(name);
		qvar->hash_value = Z_HASH_P(name);
		qvar->zend_class = class;
		qb_set_variable_type(cxt, qvar);
		qb_add_variable(cxt, qvar);
	}
	return qvar;
}

static qb_variable * ZEND_FASTCALL qb_obtain_instance_variable(qb_compiler_context *cxt, zval *name) {
	return qb_obtain_class_variable(cxt, NULL, name);
}

static uint32_t ZEND_FASTCALL qb_get_lvalue_type(qb_compiler_context *cxt, uint32_t default_type);

static void ZEND_FASTCALL qb_retrieve_operand(qb_compiler_context *cxt, uint32_t zoperand_type, znode_op *zoperand, qb_operand *operand) {
	if(zoperand_type == Z_OPERAND_CV) {
		uint32_t var_index = Z_OPERAND_INFO(*zoperand, var);
		qb_variable *qvar = cxt->variables[var_index];
		if(!qvar->address) {
			// the variable type hasn't been set yet
			qvar->flags |= QB_VARIABLE_LOCAL;
			qb_set_variable_type(cxt, qvar);
		}
		operand->address = qvar->address;
		operand->type = QB_OPERAND_ADDRESS;
	} else if(zoperand_type == Z_OPERAND_CONST) {
		operand->constant = Z_OPERAND_ZV(*zoperand);
		operand->type = QB_OPERAND_ZVAL;
	} else if(zoperand_type == Z_OPERAND_TMP_VAR || zoperand_type == Z_OPERAND_VAR) {
		qb_operand *item = qb_pop_stack_item(cxt);
		*operand = *item;
	} else {
		operand->type = QB_OPERAND_NONE;
		operand->address = NULL;
	}
}

static void ZEND_FASTCALL qb_do_array_element_retrieval(qb_compiler_context *cxt, qb_operand *variable, qb_operand *value) {
	// the arguments hold the array and index before we change them
	qb_operand *container = variable;
	qb_operand *index = value;
	if(index->type == QB_OPERAND_NONE) {
		// an append operation
		if(!IS_EXPANDABLE_ARRAY(container->address)) {
			qb_abort("Adding element to an array that cannot expand");
		}
		index->address = container->address->dimension_addresses[0];
		index->type = QB_OPERAND_ADDRESS;
	} else {
		// make sure the index is a U32
		qb_do_type_coercion(cxt, index, QB_TYPE_U32);
	}
	variable->address = qb_get_array_element(cxt, container->address, index->address);

	// extra info is held in the next op
	if(cxt->zend_op[1].opcode == ZEND_OP_DATA) {
		// retrieve the actual value to assign the element to
		qb_retrieve_operand(cxt, Z_OPERAND_TYPE(cxt->zend_op[1].op1), &cxt->zend_op[1].op1, value);
	}
}

static void ZEND_FASTCALL qb_do_object_property_retrieval(qb_compiler_context *cxt, qb_operand *variable, qb_operand *value) {
	// the arguments hold the container (which should be null for $this) and the name of the property
	qb_operand *container = variable;
	qb_operand *name = value;
	if(name->type != QB_OPERAND_ZVAL) {
		qb_abort("No support for variable variable-names");
	}
	if(container->type == QB_OPERAND_NONE) {
		qb_variable *qvar = qb_obtain_instance_variable(cxt, name->constant);
		variable->address = qvar->address;
		variable->type = QB_OPERAND_ADDRESS;
	} else if(container->type == QB_OPERAND_ADDRESS) {
		variable->address = qb_get_named_element(cxt, container->address, name->constant);
		variable->type = QB_OPERAND_ADDRESS;
	} else {
		qb_abort("Cannot fetch property of objects other than $this");
	}
	if(cxt->zend_op[1].opcode == ZEND_OP_DATA) {
		// retrieve the actual value to assign the property to
		qb_retrieve_operand(cxt, Z_OPERAND_TYPE(cxt->zend_op[1].op1), &cxt->zend_op[1].op1, value);
	}
}

static void ZEND_FASTCALL qb_translate_assign(qb_compiler_context *cxt, void *op_factory, qb_operand *operands, uint32_t operand_count, qb_operand *result) {
	qb_operand *variable = &operands[0], *value = &operands[1];

	if(cxt->zend_op->opcode == ZEND_ASSIGN_DIM) {
		qb_do_array_element_retrieval(cxt, variable, value);
	} else if(cxt->zend_op->opcode == ZEND_ASSIGN_OBJ) {
		qb_do_object_property_retrieval(cxt, variable, value);
	}
	qb_do_type_coercion(cxt, value, variable->address->type);
	qb_do_assignment(cxt, value->address, variable->address);
	if(result->type != QB_OPERAND_NONE) {
		result->address = variable->address;
	}
}

static void ZEND_FASTCALL qb_translate_assign_ref(qb_compiler_context *cxt, void *op_factory, qb_operand *operands, uint32_t operand_count, qb_operand *result) {
	qb_operand *target = &operands[1];

	if(target->type == QB_OPERAND_GLOBAL_STATIC) {
		// initialization of static or global variable
	} else {
		qb_abort("No support for reference");
	}
}

static void ZEND_FASTCALL qb_translate_assign_temp(qb_compiler_context *cxt, void *op_factory, qb_operand *operands, uint32_t operand_count, qb_operand *result) {
	qb_operand *value = &operands[0];
	int32_t lvalue_type = qb_get_lvalue_type(cxt, QB_TYPE_I32);
	qb_do_type_coercion(cxt, value, lvalue_type);
	result->address = qb_obtain_qm_temporary_variable(cxt, lvalue_type, value->address->array_size_address);
	qb_do_assignment(cxt, value->address, result->address);
}

static void ZEND_FASTCALL qb_translate_assign_op(qb_compiler_context *cxt, void *op_factory, qb_operand *operands, uint32_t operand_count, qb_operand *result) {
	qb_operand *variable = &operands[0], *value = &operands[1];

	if(cxt->zend_op->extended_value == ZEND_ASSIGN_DIM) {
		qb_do_array_element_retrieval(cxt, variable, value);
	} else if(cxt->zend_op->extended_value == ZEND_ASSIGN_OBJ) {
		qb_do_object_property_retrieval(cxt, variable, value);
	}

	qb_do_type_coercion(cxt, value, variable->address->type);
	qb_create_op(cxt, op_factory, operands, 2, variable);

	if(result->type != QB_OPERAND_NONE) {
		result->address = variable->address;
	}
}

static void ZEND_FASTCALL qb_translate_cast(qb_compiler_context *cxt, void *op_factory, qb_operand *operands, uint32_t operand_count, qb_operand *result) {
	qb_operand *value = &operands[0];
	uint32_t zend_type = cxt->zend_op->extended_value;

	switch(zend_type) {
		case IS_NULL:
		case IS_STRING:
		case IS_OBJECT: {
			const char *type_name = zend_get_type_by_const(zend_type);
			qb_abort("Cannot convert value to %s", type_name);
		}	break;
		case IS_ARRAY: {
			result->address = value->address;
		}	break;
		case IS_BOOL: {
			//qb_address *temp_address = qb_push_temporary_variable(cxt, QB_TYPE_I32, NULL);
		}	break;
		case IS_LONG: {
			result->address = qb_obtain_temporary_variable(cxt, QB_TYPE_S64, NULL);
			qb_do_assignment(cxt, value->address, result->address);
		}	break;
		case IS_DOUBLE: {
			result->address = qb_obtain_temporary_variable(cxt, QB_TYPE_F64, NULL);
			qb_do_assignment(cxt, value->address, result->address);
		}	break;
	}
}

static void ZEND_FASTCALL qb_coerce_to_boolean(qb_compiler_context *cxt, qb_operand *operand) {
	if(!(operand->address->flags & QB_ADDRESS_BOOLEAN)) {
		if(operand->address->flags & QB_ADDRESS_CONSTANT) {
			int32_t is_true;
			if(IS_SCALAR(operand->address)) {
				switch(operand->address->type) {
					case QB_TYPE_S08:
					case QB_TYPE_U08: is_true = VALUE(I08, operand->address) != 0; break;
					case QB_TYPE_S16:
					case QB_TYPE_U16: is_true = VALUE(I16, operand->address) != 0; break;
					case QB_TYPE_S32:
					case QB_TYPE_U32: is_true = VALUE(I32, operand->address) != 0; break;
					case QB_TYPE_S64:
					case QB_TYPE_U64: is_true = VALUE(I64, operand->address) != 0; break;
					case QB_TYPE_F32: is_true = VALUE(F32, operand->address) != 0.0f; break;
					case QB_TYPE_F64: is_true = VALUE(F64, operand->address) != 0.0; break;
				}
			} else {
				is_true = TRUE;
			}
			operand->address = qb_obtain_constant_S32(cxt, is_true);
		} else {
			qb_address *new_address = qb_obtain_temporary_variable(cxt, QB_TYPE_I32, operand->address->array_size_address);
			qb_create_unary_op(cxt, &factory_boolean, operand->address, new_address);
			operand->address = new_address;
		}
	}
}

static uint32_t ZEND_FASTCALL qb_coerce_operands(qb_compiler_context *cxt, void *op_factory, qb_operand *operands, uint32_t operand_count) {
	qb_operand *operand1 = &operands[0], *operand2 = &operands[1];
	uint32_t operand_flags = qb_get_coercion_flags(cxt, op_factory);
	uint32_t expr_type = QB_TYPE_ANY;
	uint32_t i;

	if(operand_flags & QB_COERCE_TO_HIGHEST_RANK) {
		expr_type = qb_get_highest_rank_type(cxt, operands, operand_count, operand_flags);
	} else if(operand_flags & QB_COERCE_TO_FIRST_OPERAND_TYPE) {
		expr_type = qb_get_operand_type(cxt, operand1, operand_flags);
	} else if(operand_flags & QB_COERCE_TO_SECOND_OPERAND_TYPE) {
		expr_type = qb_get_operand_type(cxt, operand2, operand_flags);
	} else if(operand_count == 1) {
		expr_type = qb_get_operand_type(cxt, operand1, operand_flags);
	}
	if(operand_flags & QB_COERCE_TO_LVALUE_TYPE) {
		if(expr_type != QB_TYPE_F64) {
			// see what the result is--promote the expression to it if it's higher
			uint32_t result_type = qb_get_lvalue_type(cxt, expr_type);
			if(result_type > expr_type || expr_type == QB_TYPE_ANY) {
				if(!(result_type >= QB_TYPE_F32 && (operand_flags & QB_COERCE_TO_INTEGER))) { 
					expr_type = result_type;
				}
			}
		}
	}
	for(i = 0; i < operand_count; i++) {
		qb_do_type_coercion(cxt, &operands[i], expr_type);
	}
	if(operand_flags & QB_COERCE_TO_BOOLEAN) {
		for(i = 0; i < operand_count; i++) {
			qb_coerce_to_boolean(cxt, &operands[i]);
		}
		expr_type = QB_TYPE_I32;
	}
	return expr_type;
}

static qb_address * ZEND_FASTCALL qb_obtain_result_storage(qb_compiler_context *cxt, void *op_factory, qb_operand *operands, uint32_t operand_count, uint32_t expr_type) {
	qb_address *result_address, *result_size_address = NULL;
	uint32_t result_flags = qb_get_result_flags(cxt, op_factory);
	uint32_t result_type = QB_RESULT_TYPE(result_flags);
	int32_t is_constant = FALSE;

	if(result_flags & QB_RESULT_SIZE_OPERAND) {
		result_size_address = qb_get_largest_array_size(cxt, operands, operand_count);
	} else if(result_flags & QB_RESULT_SIZE_VECTOR_COUNT) {
		result_size_address = qb_get_largest_vector_count(cxt, operands, operand_count);
	} else if(result_flags & QB_RESULT_SIZE_MM_PRODUCT) {
		result_size_address = qb_get_matrix_matrix_product_size(cxt, &operands[0], &operands[1]);
	} else if(result_flags & QB_RESULT_SIZE_MV_PRODUCT) {
		result_size_address = qb_get_matrix_vector_product_size(cxt, &operands[0], &operands[1]);
	} else if(result_flags & QB_RESULT_SIZE_VM_PRODUCT) {
		result_size_address = qb_get_vector_matrix_product_size(cxt, &operands[0], &operands[1]);
	}
	if(result_type == QB_TYPE_OPERAND) {
		result_type = expr_type;
	}
	if(!cxt->resolving_result_type) {
		// see if the result comes from a pure function (i.e. its value is dependent only on the arguments)
		if(result_flags & QB_RESULT_FROM_PURE_FUNCTION) {
			// see if all the operands are constant
			uint32_t i;
			for(i = 0; i < operand_count; i++) {
				if(!(operands[i].address->flags & QB_ADDRESS_CONSTANT)) {
					break;
				}
			}
			is_constant = (i == operand_count);
		}
	}
	if(is_constant) {
		result_address = qb_allocate_constant(cxt, result_type, result_size_address);
	} else {
		result_address = qb_obtain_temporary_variable(cxt, result_type, result_size_address);
	}
	if(result_flags & QB_RESULT_IS_BOOLEAN) {
		qb_address *bool_address = qb_allocate_address(cxt->pool);
		*bool_address = *result_address;
		bool_address->source_address = result_address;
		bool_address->flags |= QB_ADDRESS_BOOLEAN;
		result_address = bool_address;
	} else if(result_flags & QB_RESULT_IS_STRING) {
		qb_address *string_address = qb_allocate_address(cxt->pool);
		*string_address = *result_address;
		string_address->source_address = result_address;
		string_address->flags |= QB_ADDRESS_STRING;
		result_address = string_address;
	}
	return result_address;
}

static void ZEND_FASTCALL qb_translate_basic_op(qb_compiler_context *cxt, void *op_factory, qb_operand *operands, uint32_t operand_count, qb_operand *result) {
	// coerce the operands to the proper type
	uint32_t expr_type = qb_coerce_operands(cxt, op_factory, operands, operand_count);

	if(result->type != QB_OPERAND_NONE) {
		// get the result address
		result->address = qb_obtain_result_storage(cxt, op_factory, operands, operand_count, expr_type);

		// create the op
		qb_create_op(cxt, op_factory, operands, operand_count, result);
	}
}

static void ZEND_FASTCALL qb_translate_incdec_pre(qb_compiler_context *cxt, void *op_factory, qb_operand *operands, uint32_t operand_count, qb_operand *result) {
	qb_operand *variable = &operands[0];
	if(cxt->zend_op->opcode == ZEND_PRE_INC_OBJ || cxt->zend_op->opcode == ZEND_PRE_DEC_OBJ) {
		qb_do_object_property_retrieval(cxt, &operands[0], &operands[1]);
	}
	qb_create_op(cxt, op_factory, NULL, 0, variable);
	if(result->type != QB_OPERAND_NONE) {
		result->address = variable->address;
	}
}

static void ZEND_FASTCALL qb_translate_incdec_post(qb_compiler_context *cxt, void *op_factory, qb_operand *operands, uint32_t operand_count, qb_operand *result) {
	qb_operand *variable = &operands[0];
	if(cxt->zend_op->opcode == ZEND_POST_INC_OBJ || cxt->zend_op->opcode == ZEND_POST_DEC_OBJ) {
		qb_do_object_property_retrieval(cxt, &operands[0], &operands[1]);
	}
	if(result->type != QB_OPERAND_NONE) {
		// copy the value to a temporary variable before performining the increment
		result->address = qb_obtain_temporary_variable(cxt, variable->address->type, variable->address->array_size_address);
		qb_create_unary_op(cxt, &factory_copy, variable->address, result->address);
	}
	qb_create_op(cxt, op_factory, NULL, 0, variable);
}

// zend_quick_get_constant() is not exported, so we need a copy of it here 
// when the module is compiled as dynamically linked library
#ifdef COMPILE_DL_QB

#if !ZEND_ENGINE_2_3 && !ZEND_ENGINE_2_2 && !ZEND_ENGINE_2_1
static int zend_get_special_constant(const char *name, uint name_len, zend_constant **c TSRMLS_DC)
{
	int ret;
	static char haltoff[] = "__COMPILER_HALT_OFFSET__";

	if (!EG(in_execution)) {
		return 0;
	} else if (name_len == sizeof("__CLASS__")-1 &&
	          !memcmp(name, "__CLASS__", sizeof("__CLASS__")-1)) {
		zend_constant tmp;

		/* Returned constants may be cached, so they have to be stored */
		if (EG(scope) && EG(scope)->name) {
			int const_name_len;
			char *const_name;
			ALLOCA_FLAG(use_heap)
			
			const_name_len = sizeof("\0__CLASS__") + EG(scope)->name_length;
			const_name = do_alloca(const_name_len, use_heap);
			memcpy(const_name, "\0__CLASS__", sizeof("\0__CLASS__")-1);
			zend_str_tolower_copy(const_name + sizeof("\0__CLASS__")-1, EG(scope)->name, EG(scope)->name_length);
			if (zend_hash_find(EG(zend_constants), const_name, const_name_len, (void**)c) == FAILURE) {
				zend_hash_add(EG(zend_constants), const_name, const_name_len, (void*)&tmp, sizeof(zend_constant), (void**)c);
				memset(*c, 0, sizeof(zend_constant));
				Z_STRVAL((**c).value) = estrndup(EG(scope)->name, EG(scope)->name_length);
				Z_STRLEN((**c).value) = EG(scope)->name_length;
				Z_TYPE((**c).value) = IS_STRING;
			}
			free_alloca(const_name, use_heap);
		} else {
			if (zend_hash_find(EG(zend_constants), "\0__CLASS__", sizeof("\0__CLASS__"), (void**)c) == FAILURE) {
				zend_hash_add(EG(zend_constants), "\0__CLASS__", sizeof("\0__CLASS__"), (void*)&tmp, sizeof(zend_constant), (void**)c);
				memset(*c, 0, sizeof(zend_constant));
				Z_STRVAL((**c).value) = estrndup("", 0);
				Z_STRLEN((**c).value) = 0;
				Z_TYPE((**c).value) = IS_STRING;
			}
		}
		return 1;
	} else if (name_len == sizeof("__COMPILER_HALT_OFFSET__")-1 &&
	          !memcmp(name, "__COMPILER_HALT_OFFSET__", sizeof("__COMPILER_HALT_OFFSET__")-1)) {
		const char *cfilename;
		char *haltname;
		int len, clen;

		cfilename = zend_get_executed_filename(TSRMLS_C);
		clen = strlen(cfilename);
		/* check for __COMPILER_HALT_OFFSET__ */
		zend_mangle_property_name(&haltname, &len, haltoff,
			sizeof("__COMPILER_HALT_OFFSET__") - 1, cfilename, clen, 0);
		ret = zend_hash_find(EG(zend_constants), haltname, len+1, (void **) c);
		efree(haltname);
		return ret == SUCCESS;
	} else {
		return 0;
	}
}

zend_constant *zend_quick_get_constant(const zend_literal *key, ulong flags TSRMLS_DC)
{
	zend_constant *c;

	if (zend_hash_quick_find(EG(zend_constants), Z_STRVAL(key->constant), Z_STRLEN(key->constant) + 1, key->hash_value, (void **) &c) == FAILURE) {
		key++;
		if (zend_hash_quick_find(EG(zend_constants), Z_STRVAL(key->constant), Z_STRLEN(key->constant) + 1, key->hash_value, (void **) &c) == FAILURE ||
		    (c->flags & CONST_CS) != 0) {
			if ((flags & (IS_CONSTANT_IN_NAMESPACE|IS_CONSTANT_UNQUALIFIED)) == (IS_CONSTANT_IN_NAMESPACE|IS_CONSTANT_UNQUALIFIED)) {
				key++;
				if (zend_hash_quick_find(EG(zend_constants), Z_STRVAL(key->constant), Z_STRLEN(key->constant) + 1, key->hash_value, (void **) &c) == FAILURE) {
				    key++;
					if (zend_hash_quick_find(EG(zend_constants), Z_STRVAL(key->constant), Z_STRLEN(key->constant) + 1, key->hash_value, (void **) &c) == FAILURE ||
					    (c->flags & CONST_CS) != 0) {

						key--;
						if (!zend_get_special_constant(Z_STRVAL(key->constant), Z_STRLEN(key->constant), &c TSRMLS_CC)) {
							return NULL;
						}
					}
				}
			} else {
				key--;
				if (!zend_get_special_constant(Z_STRVAL(key->constant), Z_STRLEN(key->constant), &c TSRMLS_CC)) {
					return NULL;
				}
			}
		}
	}
	return c;
}
#endif

#endif

static zval * ZEND_FASTCALL qb_get_type_constant(qb_compiler_context *cxt, const char *name, uint32_t length) {
	static zval constants[QB_TYPE_COUNT];
	zval *constant;
	uint32_t i;
	for(i = 0; i < QB_TYPE_COUNT; i++) {
		const char *type = type_names[i];
		if(strcmp(name, type) == 0) {
			constant = &constants[0];
			constant->type = IS_LONG;
			constant->value.lval = i;
			return constant;
		}
	}
	return NULL;
}

static void ZEND_FASTCALL qb_translate_fetch_constant(qb_compiler_context *cxt, void *op_factory, qb_operand *operands, uint32_t operand_count, qb_operand *result) {
	USE_TSRM
	qb_operand *container = &operands[0], *name = &operands[1];

#if ZEND_ENGINE_2_2 || ZEND_ENGINE_2_1
	if(container->type == QB_OPERAND_ZVAL) {
		*container = *qb_pop_stack_item(cxt);
	}
#endif

	if(container->type == QB_OPERAND_ZEND_CLASS) {
		zval *name_value = name->constant;
		ulong hash_value = Z_HASH_P(name_value);
		zend_class_entry *ce = container->zend_class;
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
		zend_constant *zconst = zend_quick_get_constant(key, cxt->zend_op->extended_value TSRMLS_CC);
		zval *name = &key->constant;
#else
		zval *name = Z_OPERAND_ZV(cxt->zend_op->op2);
		zend_constant *zconst;
		if(zend_hash_find(EG(zend_constants), Z_STRVAL_P(name), Z_STRLEN_P(name) + 1, (void **) &zconst) == FAILURE) {
			zconst = NULL;
		}
#endif
		if(zconst) {
			value = &zconst->value;
		} else {
			value = qb_get_type_constant(cxt, Z_STRVAL_P(name), Z_STRLEN_P(name));
		}
		if(value) {
			result->type = QB_OPERAND_ZVAL;
			result->constant = value;
		} else {
			qb_abort("Undefined constant '%s'", Z_STRVAL_P(name));
		}
	}

}

static void ZEND_FASTCALL qb_translate_fetch(qb_compiler_context *cxt, void *op_factory, qb_operand *operands, uint32_t operand_count, qb_operand *result) {
	USE_TSRM
	qb_operand *name = &operands[0], *scope = &operands[1];

	if(name->type == QB_OPERAND_ZVAL) {
#if ZEND_ENGINE_2_2 || ZEND_ENGINE_2_1
		if(scope->type == QB_OPERAND_ZVAL) {
			*scope = *qb_pop_stack_item(cxt);
		}
#endif

		if(scope->type == QB_OPERAND_NONE) {
#if !ZEND_ENGINE_2_3 && !ZEND_ENGINE_2_2 && !ZEND_ENGINE_2_1
			uint32_t fetch_type = cxt->zend_op->extended_value & ZEND_FETCH_TYPE_MASK;
#else
			uint32_t fetch_type = cxt->zend_op->op2.u.EA.type;
#endif
			if(fetch_type == ZEND_FETCH_LOCAL) {
				// look up the variable
				qb_variable *qvar = qb_find_variable(cxt, NULL, name->constant, 0);
				if(qvar) {
					if(!(qvar->flags & QB_VARIABLE_LOCAL)) {
						qvar->flags |= QB_VARIABLE_LOCAL;
						qb_set_variable_type(cxt, qvar);
					}
					result->address = qvar->address;
				} else {
					qb_abort("Undefined variable: %s", Z_STRVAL_P(name->constant));
				}
			} else if(fetch_type == ZEND_FETCH_GLOBAL || fetch_type == ZEND_FETCH_GLOBAL_LOCK) {
				// look up the variable and mark it as global
				qb_variable *qvar = qb_find_variable(cxt, NULL, name->constant, 0);
				if(qvar) {
					if(!(qvar->flags & QB_VARIABLE_GLOBAL)) {
						qvar->flags |= QB_VARIABLE_GLOBAL;
						qb_set_variable_type(cxt, qvar);
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
			qb_variable *qvar = qb_obtain_class_variable(cxt, scope->zend_class, name->constant);
			result->address = qvar->address;
		}
	} else {
		qb_abort("Internal error");
	}
}

static void ZEND_FASTCALL qb_translate_fetch_element(qb_compiler_context *cxt, void *op_factory, qb_operand *operands, uint32_t operand_count, qb_operand *result) {
	qb_operand *container = &operands[0], *index = &operands[1];
	qb_do_type_coercion(cxt, index, QB_TYPE_U32);
	result->address = qb_get_array_element(cxt, container->address, index->address);
}

static void ZEND_FASTCALL qb_translate_fetch_property(qb_compiler_context *cxt, void *op_factory, qb_operand *operands, uint32_t operand_count, qb_operand *result) {
	qb_operand *object = &operands[0], *name = &operands[1];

	if(name->type != QB_OPERAND_ZVAL) {
		qb_abort("No support for variable variable-names");
	}
	if(object->type == QB_OPERAND_NONE) {
		// this object
		qb_variable *qvar = qb_obtain_instance_variable(cxt, name->constant);
		result->address = qvar->address;
	} else if(object->type == QB_OPERAND_ADDRESS) {
		// named element
		result->address = qb_get_named_element(cxt, object->address, name->constant);
	} else {
		qb_abort("Cannot fetch property of objects other than $this");
	}
}

static void ZEND_FASTCALL qb_translate_fetch_class(qb_compiler_context *cxt, void *op_factory, qb_operand *operands, uint32_t operand_count, qb_operand *result) {
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
}

static void ZEND_FASTCALL qb_translate_receive_argument(qb_compiler_context *cxt, void *op_factory, qb_operand *operands, uint32_t operand_count, qb_operand *result) {
	qb_operand *init_value = &operands[1];

	if(init_value->type == QB_OPERAND_ZVAL) {
#if !ZEND_ENGINE_2_3 && !ZEND_ENGINE_2_2 && !ZEND_ENGINE_2_1
		uint32_t argument_number = Z_OPERAND_INFO(cxt->zend_op->op1, num);
#else
		uint32_t argument_number = Z_LVAL_P(Z_OPERAND_ZV(cxt->zend_op->op1));
#endif
		qb_variable *qvar = cxt->variables[argument_number - 1];
		qvar->default_value_address = qb_obtain_zval_constant(cxt, init_value->constant, qvar->address->type);
	}
}

static void ZEND_FASTCALL qb_translate_return(qb_compiler_context *cxt, void *op_factory, qb_operand *operands, uint32_t operand_count, qb_operand *result) {
	qb_operand *retval = &operands[0];

	if(cxt->return_variable->address) {
		qb_do_type_coercion(cxt, retval, cxt->return_variable->address->type);
	} else {
		if(retval->type != QB_OPERAND_NONE && !(retval->type == QB_OPERAND_ZVAL && retval->constant->type == IS_NULL)) {
			qb_abort("Returning a value in a function with return type declared to be void");
		}
	}
	if(cxt->return_variable->address) {
		qb_do_assignment(cxt, retval->address, cxt->return_variable->address);
	}
	qb_create_op(cxt, op_factory, NULL, 0, NULL);
	cxt->jump_target_index1 = QB_INSTRUCTION_END;
}

static void ZEND_FASTCALL qb_translate_exit(qb_compiler_context *cxt, void *op_factory, qb_operand *operands, uint32_t operand_count, qb_operand *result) {
	 qb_operand *status = &operands[0];

	if(status->type != QB_OPERAND_NONE) {
		if(status->type == QB_OPERAND_ZVAL && Z_TYPE_P(status->constant) == IS_STRING) {
			qb_do_type_coercion(cxt, status, QB_TYPE_U08);
		} else if(status->type == QB_OPERAND_ADDRESS && status->address->flags & QB_ADDRESS_STRING) {
			// it's a string already
		} else {
			qb_do_type_coercion(cxt, status, QB_TYPE_S32);
			if(!IS_SCALAR(status->address)) {
				qb_abort("Exit status code must be a scalar");
			}
		}
	}
	if(status->type == QB_OPERAND_NONE) {
		status->address = qb_obtain_constant_S32(cxt, 0);
	} else if(status->address->flags & QB_ADDRESS_STRING) {
		qb_create_op(cxt, &factory_print, operands, 1, NULL);
		status->address = qb_obtain_constant_S32(cxt, 0);
	}
	qb_create_op(cxt, op_factory, operands, operand_count, NULL);
	cxt->jump_target_index1 = QB_INSTRUCTION_END;
}

static void ZEND_FASTCALL qb_translate_jump(qb_compiler_context *cxt, void *op_factory, qb_operand *operands, uint32_t operand_count, qb_operand *result) {
	zend_op *target_op = Z_OPERAND_INFO(cxt->zend_op->op1, jmp_addr);
	uint32_t target_index = ZEND_OP_INDEX(target_op);
	qb_create_jump_op(cxt, op_factory, target_index);
	cxt->jump_target_index1 = target_index;
}

static void ZEND_FASTCALL qb_translate_jump_set(qb_compiler_context *cxt, void *op_factory, qb_operand *operands, uint32_t operand_count, qb_operand *result) {
	qb_operand *value = &operands[0];
	zend_op *target_op = Z_OPERAND_INFO(cxt->zend_op->op2, jmp_addr);
	uint32_t target_index1 = ZEND_OP_INDEX(target_op);
	uint32_t target_index2 = cxt->zend_op_index + 1;
	uint32_t type = qb_get_operand_type(cxt, value, 0);
	uint32_t lvalue_type = qb_get_lvalue_type(cxt, QB_TYPE_I32);
	qb_address *zero_address = qb_obtain_constant(cxt, 0, type);
	qb_do_type_coercion(cxt, value, type);

	// if value is not zero, then jump over the false clause 
	qb_create_comparison_branch_op(cxt, &factory_branch_on_not_equal, 1 | QB_INSTRUCTION_OFFSET, target_index2, value->address, zero_address);

	// copy value to qm temporary
	result->address = qb_obtain_qm_temporary_variable(cxt, lvalue_type, value->address->array_size_address);
	qb_do_assignment(cxt, value->address, result->address);

	// jump over the false clause first
	cxt->jump_target_index1 = target_index1;
	cxt->jump_target_index2 = target_index2;
}

static void ZEND_FASTCALL qb_translate_branch(qb_compiler_context *cxt, void *op_factory, qb_operand *operands, uint32_t operand_count, qb_operand *result) {
	qb_operand *condition = &operands[0];
	zend_op *target_op1 = Z_OPERAND_INFO(cxt->zend_op->op2, jmp_addr);
	uint32_t target_index1 = ZEND_OP_INDEX(target_op1);
	uint32_t target_index2 = cxt->zend_op_index + 1;

	if(condition->type == QB_OPERAND_ADDRESS) {
		if(!IS_SCALAR(condition->address)) {
			// use the size as the condition
			condition->address = condition->address->array_size_address;
		}
	} 
	qb_coerce_operands(cxt, op_factory, operands, operand_count);

	if(result->type != QB_OPERAND_NONE) {
		// push the condition back onto the stack for the purpose of short-circuiting logical statements
		if(!(condition->address->flags & QB_ADDRESS_REUSED)) {
			qb_address *new_address = qb_allocate_address(cxt->pool);
			*new_address = *condition->address;
			new_address->flags |= QB_ADDRESS_REUSED;
			new_address->source_address = condition->address;
			condition->address = new_address;
		}
		result->address = condition->address;
		cxt->pop_short_circuiting_bool = TRUE;
	}

	if(condition->address->flags & QB_ADDRESS_CONSTANT) {
		int32_t is_true = VALUE(I32, condition->address);
		if((is_true && op_factory == &factory_branch_on_true) || (!is_true && op_factory == &factory_branch_on_false)) {
			// the branch always occurs--jump to it
			qb_create_jump_op(cxt, &factory_jump, target_index1);
			cxt->jump_target_index1 = target_index1;
		} else {
			// the branch is not reachable, just go to the next op
			cxt->jump_target_index1 = target_index2;
		}
	} else {
		qb_create_branch_op(cxt, op_factory, target_index1, target_index2, condition->address);

		// start down the next instruction first before going down the branch
		cxt->jump_target_index1 = target_index2;
		cxt->jump_target_index2 = target_index1;
	}
}

static void ZEND_FASTCALL qb_translate_for_loop(qb_compiler_context *cxt, void *op_factory, qb_operand *operands, uint32_t operand_count, qb_operand *result) {
	qb_operand *condition = &operands[0];
	uint32_t target_index1 = cxt->zend_op->extended_value;
	uint32_t target_index2 = Z_OPERAND_INFO(cxt->zend_op->op2, opline_num);

	qb_do_type_coercion(cxt, condition, QB_TYPE_I32);
	qb_create_branch_op(cxt, op_factory, target_index1, target_index2, condition->address);

	cxt->jump_target_index1 = target_index1;
	cxt->jump_target_index2 = target_index2;
}

static zend_brk_cont_element * ZEND_FASTCALL qb_find_break_continue_element(qb_compiler_context *cxt, int32_t nest_levels, int32_t array_offset) {
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


static void ZEND_FASTCALL qb_translate_break(qb_compiler_context *cxt, void *op_factory, qb_operand *operands, uint32_t operand_count, qb_operand *result) {
	zval *nest_level = Z_OPERAND_ZV(cxt->zend_op->op2);
	zend_brk_cont_element *jmp_to = qb_find_break_continue_element(cxt, Z_LVAL_P(nest_level), Z_OPERAND_INFO(cxt->zend_op->op1, opline_num));
	qb_create_jump_op(cxt, op_factory, jmp_to->brk);
	cxt->jump_target_index1 = jmp_to->brk;
}

static void ZEND_FASTCALL qb_translate_continue(qb_compiler_context *cxt, void *op_factory, qb_operand *operands, uint32_t operand_count, qb_operand *result) {
	zval *nest_level = Z_OPERAND_ZV(cxt->zend_op->op2);
	zend_brk_cont_element *jmp_to = qb_find_break_continue_element(cxt, Z_LVAL_P(nest_level), Z_OPERAND_INFO(cxt->zend_op->op1, opline_num));
	qb_create_jump_op(cxt, op_factory, jmp_to->cont);
	cxt->jump_target_index1 = jmp_to->cont;
}

static void ZEND_FASTCALL qb_translate_add_element(qb_compiler_context *cxt, void *op_factory, qb_operand *operands, uint32_t operand_count, qb_operand *result) {
	qb_operand *value = &operands[0], *index = &operands[1];
	qb_operand *element;
	qb_array_initializer *initializer;
	uint32_t element_index;
	if(result->type == QB_OPERAND_ARRAY_INITIALIZER) {
		initializer = result->array_initializer;
	} else {
		// get it from the top of the stack
		qb_operand **stack_items = qb_get_stack_items(cxt, cxt->stack_item_count - 1);
		initializer = stack_items[0]->array_initializer;

		// set the result to none (instead of popping the initializer off the stack and pushing it back on)
		result->type = QB_OPERAND_NONE;
	}
	if(index->type == QB_OPERAND_NONE) {
		element_index = initializer->element_count;
	} else {
		if(Z_TYPE_P(index->constant) == IS_LONG) {
			element_index = Z_LVAL_P(index->constant);
		} else {
			qb_abort("String key encountered: %s", Z_STRVAL_P(index->constant));
		}
	}
	// don't add the item if when we're just looking for the result type
	if(!cxt->resolving_result_type) {
		element = qb_expand_array_initializer(cxt, initializer, element_index);
		*element = *value;
	}
}

static void ZEND_FASTCALL qb_translate_init_array(qb_compiler_context *cxt, void *op_factory, qb_operand *operands, uint32_t operand_count, qb_operand *result) {
	qb_operand *value = &operands[0];
	result->type = QB_OPERAND_ARRAY_INITIALIZER;
	result->array_initializer = qb_allocate_array_initializer(cxt->pool);
	if(value->type != QB_OPERAND_NONE) {
		qb_translate_add_element(cxt, op_factory, operands, operand_count, result);
	}
}

static void ZEND_FASTCALL qb_translate_foreach_reset(qb_compiler_context *cxt, void *op_factory, qb_operand *operands, uint32_t operand_count, qb_operand *result) {
	qb_operand *container = &operands[0];

	// set the index to -1 (since the increment has to happen before the assignment)
	qb_address *index_address = qb_create_scalar(cxt, QB_TYPE_U32);
	qb_address *neg1_address = qb_obtain_constant_S32(cxt, -1);
	qb_do_assignment(cxt, neg1_address, index_address);
	index_address->flags |= QB_ADDRESS_FOREACH_INDEX;

	// push the index onto the stack
	qb_push_address(cxt, index_address);

	// the result is the container; not checking if it is an array here 
	// since that'll be done in the handler for FOREACH_FETCH
	result->address = container->address;
}

static void ZEND_FASTCALL qb_translate_foreach_fetch(qb_compiler_context *cxt, void *op_factory, qb_operand *operands, uint32_t operand_count, qb_operand *result) {
	qb_operand *container = &operands[0];
	qb_address *index_address;
	uint32_t target_index = Z_OPERAND_INFO(cxt->zend_op->op2, opline_num);

	// pop the index off the stack
	index_address = qb_pop_stack_item(cxt)->address;

	// push it back on if the key is needed
	if(cxt->zend_op->extended_value & ZEND_FE_FETCH_WITH_KEY) {
		qb_push_address(cxt, index_address);
	}

	if(cxt->zend_op->extended_value & ZEND_FE_FETCH_BYREF) {
		qb_abort("Reference is not supported");
	}

	// increment the index
	qb_create_nullary_op(cxt, &factory_increment, index_address);

	// jump out of the loop when the index is no longer less than the dimension
	qb_create_comparison_branch_op(cxt, &factory_branch_on_less_than, QB_INSTRUCTION_NEXT, target_index, index_address, container->address->dimension_addresses[0]);

	// get the element from the container
	result->address = qb_get_array_element(cxt, container->address, index_address);

	cxt->jump_target_index1 = cxt->zend_op_index + 1;
	cxt->jump_target_index2 = target_index;
}

static void ZEND_FASTCALL qb_translate_case(qb_compiler_context *cxt, void *op_factory, qb_operand *operands, uint32_t operand_count, qb_operand *result) {
	qb_operand *value = &operands[0], *constant = &operands[1];

	if(value->type == QB_OPERAND_ZVAL) {
		// just in case someone decides to use a constant--coerce it to I64
		qb_do_type_coercion(cxt, value, QB_TYPE_I64);
	}

	// coerce the constant to the same type
	qb_do_type_coercion(cxt, constant, value->address->type);

	// push the value back onto the stack
	qb_push_address(cxt, value->address);

	result->address = qb_obtain_temporary_variable(cxt, QB_TYPE_I32, NULL);
	qb_create_op(cxt, &factory_equal, operands, 2, result);
}

static void ZEND_FASTCALL qb_translate_free(qb_compiler_context *cxt, void *op_factory, qb_operand *operands, uint32_t operand_count, qb_operand *result) {
	qb_operand *value = &operands[0];

	// if it's a temporary variable being freed, perhaps the op that created it isn't needed in the first place
	if(value->type == QB_OPERAND_ADDRESS && value->address->flags & QB_ADDRESS_TEMPORARY) {
		qb_op *redundant_qop = NULL;
		uint32_t i, j;
		for(i = cxt->op_count - 1; (int32_t) i >= 0; i--) {
			qb_op *prev_qop = cxt->ops[i];
			for(j = 0; j < prev_qop->operand_count; j++) {
				if(prev_qop->operands[j].type == QB_OPERAND_ADDRESS && prev_qop->operands[j].address == value->address) {
					uint32_t operand_flags = qb_get_operand_flags(cxt, prev_qop->opcode, j);
					if(operand_flags & QB_OPERAND_WRITABLE) {
						// this is the op that presumably produced the value
						// if it has no side-effect, then we can remove it
						// otherwise it has to be kept, eventhough the value it produces is ignored
						if(!(prev_qop->flags & QB_OP_CANNOT_REMOVE)) {
							redundant_qop = prev_qop;
						} else {
							// the value is used by another op
						}
					} 
					// end the loop
					i = 0;
					break;
				}
			}
		}
		if(redundant_qop) {
			// make it a nop
			redundant_qop->opcode = QB_NOP;
		}
	}

	// coerce to void so qb_get_lvalue_type() knows that the value type is irrelevant
	qb_do_type_coercion(cxt, value, QB_TYPE_VOID);
}

static void ZEND_FASTCALL qb_translate_echo(qb_compiler_context *cxt, void *op_factory, qb_operand *operands, uint32_t operand_count, qb_operand *result) {
	qb_operand *value = &operands[0];
	qb_do_type_coercion(cxt, value, QB_TYPE_ANY);
	qb_create_op(cxt, &factory_print, operands, 1, NULL);
	if(cxt->zend_op->opcode == ZEND_PRINT) {
		// print() returns a value unlike echo
		result->address = qb_obtain_constant_S32(cxt, 1);
	}
}

static void ZEND_FASTCALL qb_translate_init_string(qb_compiler_context *cxt, void *op_factory, qb_operand *operands, uint32_t operand_count, qb_operand *result) {
	// Zend Engine 2.2 only--higher versions skip this step
	result->address = qb_obtain_temporary_variable_length_array(cxt, QB_TYPE_U08);
	result->address->flags |= QB_ADDRESS_STRING;
}

static void ZEND_FASTCALL qb_translate_add_string(qb_compiler_context *cxt, void *op_factory, qb_operand *operands, uint32_t operand_count, qb_operand *result) {
	qb_operand *string = &operands[0], *addition = &operands[1];

	if(string->type == QB_OPERAND_NONE) {
		result->address = qb_obtain_temporary_variable_length_array(cxt, QB_TYPE_U08);
		result->address->flags |= QB_ADDRESS_STRING;
	} else {
		result->address = string->address;
	}
	if(addition->type == QB_OPERAND_ZVAL) {
		qb_do_type_coercion(cxt, addition, QB_TYPE_U08);
		if(IS_SCALAR(addition->address)) {
			// it's an ADD_CHAR op
			if(!(addition->address->flags & QB_ADDRESS_STRING)) {
				// create a new address and flag it as a string
				qb_address *new_address = qb_allocate_address(cxt->pool);
				*new_address = *addition->address;
				new_address->flags |= QB_ADDRESS_STRING;
				addition->address = new_address;
			}
		}
	}
	qb_create_op(cxt, op_factory, addition, 1, result);
}

static void ZEND_FASTCALL qb_translate_concat(qb_compiler_context *cxt, void *op_factory, qb_operand *operands, uint32_t operand_count, qb_operand *result) {
	qb_operand *string1 = &operands[0], *string2 = &operands[1];

	// handle zval constants
	if(string1->type == QB_OPERAND_ZVAL) {
		qb_do_type_coercion(cxt, string1, QB_TYPE_ANY);
	}
	if(string2->type == QB_OPERAND_ZVAL) {
		qb_do_type_coercion(cxt, string2, QB_TYPE_ANY);
	}

	if(string1->address->flags & QB_ADDRESS_TEMPORARY && IS_EXPANDABLE_ARRAY(string1->address) && string1->address->type == QB_TYPE_U08) {
		// reuse string1
		result->address = string1->address;
	} else {
		// copy string1 into a temporary variable
		result->address = qb_obtain_temporary_variable_length_array(cxt, QB_TYPE_U08);
		result->address->flags |= QB_ADDRESS_STRING;
		qb_create_op(cxt, op_factory, string1, 1, result);
	}
	// append string2
	qb_create_op(cxt, op_factory, string2, 1, result);
}

static void ZEND_FASTCALL qb_translate_assign_concat(qb_compiler_context *cxt, void *op_factory, qb_operand *operands, uint32_t operand_count, qb_operand *result) {
	qb_operand *variable = &operands[0], *addition = &operands[1];

	if(addition->type == QB_OPERAND_ZVAL) {
		qb_do_type_coercion(cxt, addition, QB_TYPE_ANY);
	}
	if((variable->address->type & ~QB_TYPE_UNSIGNED) != QB_TYPE_I08) {
		qb_abort("Cannot append to a variable of type %s", type_names[variable->address->type]);
	}
	qb_create_op(cxt, op_factory, addition, 1, variable);
	if(result->type != QB_OPERAND_NONE) {
		result->address = variable->address;
	}
}

static void ZEND_FASTCALL qb_translate_bool(qb_compiler_context *cxt, void *op_factor, qb_operand *operands, uint32_t operand_count, qb_operand *result) {
	qb_operand *value = &operands[0];

	if(value->type == QB_OPERAND_ZVAL) {
		int32_t is_true = zend_is_true(value->constant);
		qb_address *constant = qb_obtain_constant_S32(cxt, is_true ? 1 : 0);
		result->address = qb_allocate_address(cxt->pool);
		*result->address = *constant;
		result->address->flags |= QB_ADDRESS_BOOLEAN;
	} else if(value->type == QB_OPERAND_ADDRESS) {
		// if it's a array, use the size (i.e. non-empty means true)
		if(!IS_SCALAR(value->address)) {
			value->address = value->address->array_size_address;
		}
		qb_coerce_to_boolean(cxt, value);
		result->address = value->address;
	} else if(value->type == QB_OPERAND_PREVIOUS_RESULT) {
		qb_do_type_coercion(cxt, value, QB_TYPE_ANY);
	}
}

static void ZEND_FASTCALL qb_translate_isset(qb_compiler_context *cxt, void *op_factory, qb_operand *operands, uint32_t operand_count, qb_operand *result) {
	qb_translate_bool(cxt, op_factory, operands, operand_count, result);
}

static void ZEND_FASTCALL qb_translate_isset_element(qb_compiler_context *cxt, void *op_factory, qb_operand *operands, uint32_t operand_count, qb_operand *result) {
	qb_operand *container = &operands[0], *index = &operands[1], *element = &operands[0];
	element->address = qb_get_array_element(cxt, container->address, index->address);
	result->address = qb_obtain_temporary_variable(cxt, QB_TYPE_I32, NULL);
	qb_create_op(cxt, op_factory, operands, 1, result);
}

static void ZEND_FASTCALL qb_translate_unset(qb_compiler_context *cxt, void *op_factory, qb_operand *operands, uint32_t operand_count, qb_operand *result) {
	qb_operand *variable = &operands[0], *value = &operands[1];

	if(cxt->zend_op->opcode == ZEND_UNSET_DIM) {
		qb_do_array_element_retrieval(cxt, variable, value);
	} else if(cxt->zend_op->opcode == ZEND_UNSET_OBJ) {
		qb_do_object_property_retrieval(cxt, variable, value);
	}
	qb_create_nullary_op(cxt, op_factory, variable->address); 
}

static void ZEND_FASTCALL qb_translate_begin_silence(qb_compiler_context *cxt, void *op_factory, qb_operand *operands, uint32_t operand_count, qb_operand *result) {
	cxt->silence = 1;
}

static void ZEND_FASTCALL qb_translate_end_silence(qb_compiler_context *cxt, void *op_factory, qb_operand *operands, uint32_t operand_count, qb_operand *result) {
	cxt->silence = 0;
	// push the operand back onto the stack
	*result = operands[0];
}

#include "qb_compiler_php_intrinsic.c"

static zend_function * ZEND_FASTCALL qb_find_function(qb_compiler_context *cxt, zval *object, zend_class_entry *class, zval *name) {
	USE_TSRM

	// check to see if we've found the function already (this function is called whenever an argument is pushed)
	if(cxt->previous_object != object || cxt->previous_class != class || cxt->previous_function_name != name) {
		char *error = NULL;
		HashTable ht;
		int32_t use_array_callable = FALSE;
		zval *callable = name, *class_name;
		zval _array_callable, *array_callable = &_array_callable;
		zend_fcall_info_cache fcc;

		if(class && !object) {
			// no good way to get the class entry to zend_is_callable_ex() except by creating an array
			zend_hash_init(&ht, sizeof(zval *), NULL, NULL, 0);

			class_name = qb_string_to_zval(class->name, class->name_length);

			Z_ARRVAL_P(array_callable) = &ht;
			Z_TYPE_P(array_callable) = IS_ARRAY;

			zend_hash_next_index_insert(&ht, &class_name, sizeof(zval *), NULL);
			zend_hash_next_index_insert(&ht, &callable, sizeof(zval *), NULL);

			callable = array_callable;
			use_array_callable = TRUE;
		}
#if !ZEND_ENGINE_2_2 && !ZEND_ENGINE_2_1
		if(!zend_is_callable_ex(callable, object, IS_CALLABLE_CHECK_NO_ACCESS, NULL, NULL, &fcc, &error TSRMLS_CC)) {
			qb_abort("%s", error);
		}
		if(error) {
			efree(error);
		}
#else
		if(!class && object) {
			zend_hash_init(&ht, sizeof(zval *), NULL, NULL, 0);

			Z_ARRVAL_P(array_callable) = &ht;
			Z_TYPE_P(array_callable) = IS_ARRAY;

			zend_hash_next_index_insert(&ht, &object, sizeof(zval *), NULL);
			zend_hash_next_index_insert(&ht, &callable, sizeof(zval *), NULL);

			callable = array_callable;
			use_array_callable = TRUE;
		}
		{
			// suppress the non-static function being called as static warning message
			int error_reporting_before = EG(error_reporting);
			EG(error_reporting) = 0;
			if(!zend_is_callable_ex(callable, 0, NULL, NULL, NULL, &fcc.function_handler, &fcc.object_pp TSRMLS_CC)) {
				qb_abort("Cannot find function: %s", Z_STRVAL_P(name));
			}
			EG(error_reporting) = error_reporting_before;
		}
#endif
		if(use_array_callable) {
			zend_hash_destroy(&ht);
		}
		cxt->previous_object = object;
		cxt->previous_class = class;
		cxt->previous_function_name = name;
		cxt->previous_function = fcc.function_handler;
	}
	return cxt->previous_function;
}

static qb_function * ZEND_FASTCALL qb_get_compiled_function(qb_compiler_context *cxt, zend_function *zfunc) {
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

static uint32_t ZEND_FASTCALL qb_do_argument_coercion(qb_compiler_context *cxt, qb_operand *argument) {
	uint32_t type = QB_TYPE_UNKNOWN, send_count = 0, argument_index, i;
	zval *function_name = NULL, *object = NULL;
	zend_class_entry *class = NULL;

	// find the function call
	for(i = cxt->zend_op_index; i < cxt->zend_op_array->last && !function_name; i++) {
		zend_op *zop = &cxt->zend_op_array->opcodes[i];
		switch(zop->opcode) {
			case ZEND_SEND_VAR:
			case ZEND_SEND_REF:
			case ZEND_SEND_VAL:
			case ZEND_SEND_VAR_NO_REF: {
				send_count++;
			}	break;
			case ZEND_DO_FCALL: {
				uint32_t argument_count = zop->extended_value;
				if(argument_count >= send_count) {
					argument_index = argument_count - send_count;
					function_name = Z_OPERAND_ZV(zop->op1);
				} else {
					send_count -= argument_count;
				}
			}	break;
			case ZEND_DO_FCALL_BY_NAME: {
				uint32_t argument_count = zop->extended_value;
				if(argument_count >= send_count) {
					qb_operand **stack_items;
					argument_index = argument_count - send_count;
					stack_items = qb_get_stack_items(cxt, cxt->stack_item_count - argument_index - 2);
					if(stack_items[0]->type == QB_OPERAND_ZVAL) {
						function_name = stack_items[0]->constant;
					} else {
						qb_abort("Internal error");
					}
					if(stack_items[1]->type == QB_OPERAND_ZVAL) {
						object = stack_items[1]->constant;
					} else if(stack_items[1]->type == QB_OPERAND_ZEND_CLASS) {
						class = stack_items[1]->zend_class;
					}
				} else {
					send_count -= argument_count;
				}
			}	break;
		}
	}

	if(function_name) {
		qb_intrinsic_function *ifunc = qb_find_intrinsic_function(cxt, function_name);
		if(ifunc) {
			// try to coerce the argument to the correct type
			// the purpose of doing type coercion here is so qb_get_lvalue_type() can return more quickly
			// otherwise all subsequent arguments will have to be evaluated before we know what type this one should have
			// sometimes, this is in fact the case: when the arguments all have to be of the same type
			if(argument_index < ifunc->argument_count_max) {
				if(ifunc->do_type_coercion) {
					ifunc->do_type_coercion(cxt, ifunc, argument, argument_index);
				}
			} else {
				// coerce it to whatever so qb_get_lvalue_type() can exit
				qb_do_type_coercion(cxt, argument, QB_TYPE_ANY);
			}
		} else {
			zend_function *zfunc = qb_find_function(cxt, object, class, function_name);
			if(zfunc) {
				qb_function *qfunc = qb_get_compiled_function(cxt, zfunc);
				if(qfunc) {
					if(argument_index < qfunc->argument_count) {
						qb_variable *qvar = qfunc->variables[argument_index];

						if(qvar->flags & QB_VARIABLE_PASSED_BY_REF) {
							if(!STORAGE_TYPE_MATCH(qvar->address->type, argument->address->type)) {
								qb_abort("%s expects argument %d to be of the type %s, %s was passed", qfunc->name, argument_index + 1, type_names[qvar->address->type], type_names[argument->address->type]);
							}
							if(argument->address->flags & (QB_ADDRESS_TEMPORARY | QB_ADDRESS_QM_TEMPORARY)) {
								qb_abort("Only variable should be passed by reference");
							}

						} else {
							qb_do_type_coercion(cxt, argument, qvar->address->type);
						}
						if(IS_EXPANDABLE_ARRAY(qvar->address) && !IS_EXPANDABLE_ARRAY(argument->address)) {
							qb_abort("%s expects argument %d to be of a variable length array", qfunc->name, argument_index + 1);
						}
					}
				} else {
					qb_do_type_coercion(cxt, argument, QB_TYPE_ANY);
				}
			} else {
				qb_abort("Missing function");
			}
		}
	}
	return type;
}

static void ZEND_FASTCALL qb_translate_send_argument(qb_compiler_context *cxt, void *op_factory, qb_operand *operands, uint32_t operand_count, qb_operand *result) {
	qb_operand *argument = &operands[0], *stack_item;
	qb_do_argument_coercion(cxt, argument);
	stack_item = qb_push_stack_item(cxt);
	*stack_item = *argument;
}

static void ZEND_FASTCALL qb_translate_init_method_call(qb_compiler_context *cxt, void *op_factory, qb_operand *operands, uint32_t operand_count, qb_operand *result) {
	qb_operand *object = &operands[0], *name = &operands[1], *stack_item;

#if ZEND_ENGINE_2_2 || ZEND_ENGINE_2_1
	if(object->type == QB_OPERAND_ZVAL) {
		*object = *qb_pop_stack_item(cxt);
	}
#endif

	stack_item = qb_push_stack_item(cxt);	// function name
	*stack_item = *name;
	stack_item = qb_push_stack_item(cxt);	// object
	if(object->type == QB_OPERAND_NONE) {
		if(cxt->zend_this_object) {
			stack_item->type = QB_OPERAND_ZVAL;
			stack_item->constant = cxt->zend_this_object;
		} else if(cxt->zend_class) {
			stack_item->type = QB_OPERAND_ZEND_CLASS;
			stack_item->zend_class = cxt->zend_class;
		}
	} else if(object->type == QB_OPERAND_ZEND_CLASS) {
		stack_item->type = QB_OPERAND_ZEND_CLASS;
		stack_item->zend_class = object->zend_class;
	} else {
		qb_abort("No support for object method invocation");
	}
}

static void ZEND_FASTCALL qb_translate_function_call_init(qb_compiler_context *cxt, void *op_factory, qb_operand *operands, uint32_t operand_count, qb_operand *result) {
	qb_operand *object = &operands[0], *name = &operands[1], *stack_item;
	stack_item = qb_push_stack_item(cxt);	// function name
	*stack_item = *name;

	stack_item = qb_push_stack_item(cxt);	// object
#if !ZEND_ENGINE_2_3
	*stack_item = *object;
#else
	// for some reason in Zend Engine 2.3, the name of the function would show up here
	if(object->type == QB_OPERAND_ZVAL && object->constant->type == IS_OBJECT) {
		*stack_item = *object;
	} else {
		stack_item->type = QB_OPERAND_NONE;
		stack_item->address = NULL;
	}
#endif
}

static void ZEND_FASTCALL qb_translate_function_call(qb_compiler_context *cxt, void *op_factory, qb_operand *operands, uint32_t operand_count, qb_operand *result) {
	qb_operand *name = &operands[0], *object = &operands[1];
	qb_intrinsic_function *ifunc;
	qb_operand arguments[256], **stack_pointer;
	uint32_t argument_count, i;

	if(name->type != QB_OPERAND_ZVAL || Z_TYPE_P(name->constant) != IS_STRING) {
		qb_abort("Function name is not a literal string");
	}

	argument_count = cxt->zend_op->extended_value;
	if(argument_count > 256) {
		qb_abort("Too many arguments");
	}
	stack_pointer = qb_get_stack_items(cxt, cxt->stack_item_count - argument_count);
	for(i = 0; i < argument_count; i++) {
		arguments[i] = *stack_pointer[i];
	}

	// remove the items from the stack
	cxt->stack_item_count -= argument_count;

	ifunc = (object->type == QB_OPERAND_NONE) ? qb_find_intrinsic_function(cxt, name->constant) : NULL;
	if(ifunc) {
		if(argument_count < ifunc->argument_count_min || argument_count > ifunc->argument_count_max) {
			if(ifunc->argument_count_min == ifunc->argument_count_max) {
				qb_abort("%s() expects %d arguments but %d was passed", ifunc->name, ifunc->argument_count_min, argument_count);
			} else {
				qb_abort("%s() expects %d to %d arguments but %d was passed", ifunc->name, ifunc->argument_count_min, ifunc->argument_count_max, argument_count);
			}
		}
		if(ifunc->translate) {
			ifunc->translate(cxt, ifunc, arguments, argument_count, result);
		} else {
			// there's no special handler--translate it as a regular op
			qb_translate_basic_op(cxt, ifunc->extra, arguments, argument_count, result); 
		}
	} else {
		zend_function *zfunc;
		qb_function *qfunc;
		int32_t result_size_known, result_is_array;
		uint32_t result_type, result_size;

		// no intrinsic function found--look for the function
		if(object->type == QB_OPERAND_ZVAL) {
			zfunc = qb_find_function(cxt, object->constant, NULL, name->constant);
		} else if(object->type == QB_OPERAND_ZEND_CLASS) {
			zfunc = qb_find_function(cxt, NULL, object->zend_class, name->constant);
		} else {
			zfunc = qb_find_function(cxt, NULL, NULL, name->constant);
		}
		cxt->zend_function_being_called = zfunc;

		// see if it's a qb function
		qfunc = qb_get_compiled_function(cxt, zfunc);

		// the result type is determined by the caller; it's up to the interpreter to coerce 
		// the result into the right type; query subsequent opcodes to see what is expected
		// (if a return value is expected
		if(result->type != QB_OPERAND_NONE) {
			result_type = qb_get_lvalue_type(cxt, QB_TYPE_ANY);
		} else {
			result_type = QB_TYPE_VOID;
		}

		// the length of the result is determined by the callee, however
		// if it's another qb function, then we might know exactly how
		// long it's going to be; if it's Zend function, then we don't
		if(result_type != QB_TYPE_VOID) {
			if(qfunc) {
				if(qfunc->return_variable->address) {
					qb_address *retval_address = qfunc->return_variable->address;
					if(IS_SCALAR(retval_address)) {
						result_size = 1;
						result_size_known = TRUE;
						result_is_array = FALSE;
					} else {
						if(IS_FIXED_LENGTH_ARRAY(retval_address)) {
							// the length is fixed
							result_size = VALUE_IN(qfunc->local_storage, U32, retval_address->array_size_address);
							result_size_known = TRUE;
							result_is_array = TRUE;
						} else {
							// it's variable
							result_size = 0;
							result_size_known = FALSE;
							result_is_array = TRUE;
						}
					}
					if(result_type == QB_TYPE_ANY) {
						// the op that eventually gets the result will accept anything, so use what the callee returns
						result_type = retval_address->type;
					}
				} else {
					if(result_type == QB_TYPE_ANY) {
						result_type = QB_TYPE_VOID;
					} else {
						qb_abort("Cannot obtain the return value from %s() as it is declared void", qfunc->name);
					}
				}
			} else {
				if(result_type == QB_TYPE_ANY) {
					qb_abort("Cannot determine the return value type from context");
				} else {
					// we know the type expected, but not the size
					result_size = 0;
					result_size_known = FALSE;
				}
			}
		} else {
			// if the return value is ignored, use an empty array
			// the interpreter will respect this
			result_type = QB_TYPE_I32;
			result_size = 0;
			result_size_known = TRUE;
		}

		if(result_size_known) {
			if(result_is_array) {
				result->address = qb_obtain_temporary_fixed_length_array(cxt, result_type, result_size);
			} else {
				result->address = qb_obtain_temporary_scalar(cxt, result_type);
			}
		} else {
			result->address = qb_obtain_temporary_variable_length_array(cxt, result_type);
		}

		for(i = 0; i < argument_count; i++) {
			qb_address *address = arguments[i].address;

			// mark variables passed by-ref as writable
			if(i < zfunc->common.num_args && zfunc->common.arg_info) {
				zend_arg_info *zarg = &zfunc->common.arg_info[i];
				if(zarg->pass_by_reference) {
					qb_mark_as_writable(cxt, address);
				}
			} else {
				// not argument info--assume it's passed by ref just in case
				qb_mark_as_writable(cxt, address);
			}

			// mark all variables as exportable (since they have to be copied out and in)
			qb_mark_as_non_local(cxt, address);
		}
		qb_mark_as_writable(cxt, result->address);
		qb_mark_as_non_local(cxt, result->address);

		qb_create_op(cxt, op_factory, arguments, argument_count, result);
		cxt->zend_function_being_called = NULL;
	}
}

static void ZEND_FASTCALL qb_translate_function_call_by_name(qb_compiler_context *cxt, void *op_factory, qb_operand *operands, uint32_t operand_count, qb_operand *result) {
	qb_operand **stack_items, fcall_operands[2];
	uint32_t argument_count, i, name_index;

	argument_count = cxt->zend_op->extended_value;
	name_index = cxt->stack_item_count - argument_count - 2;
	stack_items = qb_get_stack_items(cxt, name_index);
	fcall_operands[0] = *stack_items[0];
	fcall_operands[1] = *stack_items[1];
	qb_translate_function_call(cxt, op_factory, fcall_operands, 2, result);

	// get rid of the name and the object
	stack_items = qb_get_stack_items(cxt, name_index);
	for(i = 2; i < cxt->stack_item_count; i++) {
		*stack_items[i - 2] = *stack_items[i];
	}
	cxt->stack_item_count -= 2; 
}

static void ZEND_FASTCALL qb_translate_user_opcode(qb_compiler_context *cxt, void *op_factory, qb_operand *operands, uint32_t operand_count, qb_operand *result) {
}

static void ZEND_FASTCALL qb_translate_extension_op(qb_compiler_context *cxt, void *op_factory, qb_operand *operands, uint32_t operand_count, qb_operand *result) {
	if(!cxt->resolving_result_type) {
		USE_TSRM
		if(QB_G(allow_debugger_inspection)) {
			qb_operand opcode; 
			opcode.type = QB_OPERAND_ADDRESS;
			opcode.address = qb_obtain_constant_U32(cxt, cxt->zend_op->opcode);
			qb_create_op(cxt, op_factory, &opcode, 1, NULL);
			cxt->function_flags |= QB_ENGINE_GO_THRU_ZEND;
		}
	}
}

static void ZEND_FASTCALL qb_translate_nop(qb_compiler_context *cxt, void *op_factory, qb_operand *operands, uint32_t operand_count, qb_operand *result) {
}

static qb_translator op_translators[] = {
	{	qb_translate_nop,					NULL						},	// ZEND_NOP
	{	qb_translate_basic_op,				&factory_add				},	// ZEND_ADD
	{	qb_translate_basic_op,				&factory_subtract			},	// ZEND_SUB
	{	qb_translate_basic_op,				&factory_multiply			},	// ZEND_MUL
	{	qb_translate_basic_op,				&factory_divide				},	// ZEND_DIV
	{	qb_translate_basic_op,				&factory_modulo				},	// ZEND_MOD
	{	qb_translate_basic_op,				&factory_shift_left			},	// ZEND_SL
	{	qb_translate_basic_op,				&factory_shift_right		},	// ZEND_SR
	{	qb_translate_concat,				&factory_concat				},	// ZEND_CONCAT
	{	qb_translate_basic_op,				&factory_bitwise_or			},	// ZEND_BW_OR
	{	qb_translate_basic_op,				&factory_bitwise_and		},	// ZEND_BW_AND
	{	qb_translate_basic_op,				&factory_bitwise_xor		},	// ZEND_BW_XOR
	{	qb_translate_basic_op,				&factory_bitwise_not		},	// ZEND_BW_NOT
	{	qb_translate_basic_op,				&factory_logical_not		},	// ZEND_BOOL_NOT
	{	qb_translate_basic_op,				&factory_logical_xor		},	// ZEND_BOOL_XOR
	{	qb_translate_basic_op,				&factory_equal				},	// ZEND_IS_IDENTICAL
	{	qb_translate_basic_op,				&factory_not_equal			},	// ZEND_IS_NOT_IDENTICAL
	{	qb_translate_basic_op,				&factory_equal				},	// ZEND_IS_EQUAL
	{	qb_translate_basic_op,				&factory_not_equal			},	// ZEND_IS_NOT_EQUAL
	{	qb_translate_basic_op,				&factory_less_than			},	// ZEND_IS_SMALLER
	{	qb_translate_basic_op,				&factory_less_equal			},	// ZEND_IS_SMALLER_OR_EQUAL
	{	qb_translate_cast,					NULL						},	// ZEND_CAST
	{	qb_translate_assign_temp,			&factory_copy				},	// ZEND_QM_ASSIGN
	{	qb_translate_assign_op,				&factory_add				},	// ZEND_ASSIGN_ADD
	{	qb_translate_assign_op,				&factory_subtract			},	// ZEND_ASSIGN_SUB
	{	qb_translate_assign_op,				&factory_multiply			},	// ZEND_ASSIGN_MUL
	{	qb_translate_assign_op,				&factory_divide				},	// ZEND_ASSIGN_DIV
	{	qb_translate_assign_op,				&factory_modulo				},	// ZEND_ASSIGN_MOD
	{	qb_translate_assign_op,				&factory_shift_left			},	// ZEND_ASSIGN_SL
	{	qb_translate_assign_op,				&factory_shift_right		},	// ZEND_ASSIGN_SR
	{	qb_translate_assign_concat,			&factory_concat				},	// ZEND_ASSIGN_CONCAT
	{	qb_translate_assign_op,				&factory_bitwise_or			},	// ZEND_ASSIGN_BW_OR
	{	qb_translate_assign_op,				&factory_bitwise_and		},	// ZEND_ASSIGN_BW_AND
	{	qb_translate_assign_op,				&factory_bitwise_xor		},	// ZEND_ASSIGN_BW_XOR
	{	qb_translate_incdec_pre,			&factory_increment			},	// ZEND_PRE_INC
	{	qb_translate_incdec_pre,			&factory_decrement			},	// ZEND_PRE_DEC
	{	qb_translate_incdec_post,			&factory_increment			},	// ZEND_POST_INC
	{	qb_translate_incdec_post,			&factory_decrement			},	// ZEND_POST_DEC
	{	qb_translate_assign,				NULL						},	// ZEND_ASSIGN
	{	qb_translate_assign_ref,			NULL						},	// ZEND_ASSIGN_REF
	{	qb_translate_echo,					&factory_print				},	// ZEND_ECHO
	{	qb_translate_echo,					&factory_print				},	// ZEND_PRINT
	{	qb_translate_jump,					&factory_jump				},	// ZEND_JMP
	{	qb_translate_branch,				&factory_branch_on_false	},	// ZEND_JMPZ
	{	qb_translate_branch,				&factory_branch_on_true		},	// ZEND_JMPNZ
	{	qb_translate_for_loop,				&factory_branch_on_true		},	// ZEND_JMPZNZ
	{	qb_translate_branch,				&factory_branch_on_false	},	// ZEND_JMPZ_EX
	{	qb_translate_branch,				&factory_branch_on_true		},	// ZEND_JMPNZ_EX
	{	qb_translate_case,					&factory_branch_on_true		},	// ZEND_CASE
	{	qb_translate_free,					NULL						},	// ZEND_SWITCH_FREE
	{	qb_translate_break,					&factory_jump				},	// ZEND_BRK
	{	qb_translate_continue,				&factory_jump				},	// ZEND_CONT
	{	qb_translate_bool,					&factory_boolean			},	// ZEND_BOOL
	{	qb_translate_init_string,			NULL						},	// ZEND_INIT_STRING
	{	qb_translate_add_string,			&factory_concat				},	// ZEND_ADD_CHAR
	{	qb_translate_add_string,			&factory_concat				},	// ZEND_ADD_STRING
	{	qb_translate_add_string,			&factory_concat				},	// ZEND_ADD_VAR
	{	qb_translate_begin_silence,			NULL						},	// ZEND_BEGIN_SILENCE
	{	qb_translate_end_silence,			NULL						},	// ZEND_END_SILENCE
	{	qb_translate_function_call_init,	NULL						},	// ZEND_INIT_FCALL_BY_NAME
	{	qb_translate_function_call,			&factory_fcall				},	// ZEND_DO_FCALL
	{	qb_translate_function_call_by_name,	&factory_fcall				},	// ZEND_DO_FCALL_BY_NAME
	{	qb_translate_return,				&factory_return				},	// ZEND_RETURN
	{	qb_translate_receive_argument,		NULL						},	// ZEND_RECV
	{	qb_translate_receive_argument,		NULL						},	// ZEND_RECV_INIT
	{	qb_translate_send_argument,			NULL						},	// ZEND_SEND_VAL
	{	qb_translate_send_argument,			NULL						},	// ZEND_SEND_VAR
	{	qb_translate_send_argument,			NULL						},	// ZEND_SEND_REF
	{	NULL,								NULL						},	// ZEND_NEW
	{	NULL,								NULL						},	// ZEND_INIT_NS_FCALL_BY_NAME
	{	qb_translate_free,					NULL						},	// ZEND_FREE
	{	qb_translate_init_array,			NULL						},	// ZEND_INIT_ARRAY
	{	qb_translate_add_element,			NULL						},	// ZEND_ADD_ARRAY_ELEMENT
	{	NULL,								NULL						},	// ZEND_INCLUDE_OR_EVAL
	{	qb_translate_unset,					&factory_unset				},	// ZEND_UNSET_VAR
	{	qb_translate_unset,					&factory_unset				},	// ZEND_UNSET_DIM
	{	qb_translate_unset,					&factory_unset				},	// ZEND_UNSET_OBJ
	{	qb_translate_foreach_reset,			NULL						},	// ZEND_FE_RESET
	{	qb_translate_foreach_fetch,			NULL						},	// ZEND_FE_FETCH
	{	qb_translate_exit,					&factory_exit				},	// ZEND_EXIT
	{	qb_translate_fetch,					NULL						},	// ZEND_FETCH_R
	{	qb_translate_fetch_element,			NULL						},	// ZEND_FETCH_DIM_R
	{	qb_translate_fetch_property,		NULL						},	// ZEND_FETCH_OBJ_R
	{	qb_translate_fetch,					NULL						},	// ZEND_FETCH_W
	{	qb_translate_fetch_element,			NULL						},	// ZEND_FETCH_DIM_W
	{	qb_translate_fetch_property,		NULL						},	// ZEND_FETCH_OBJ_W
	{	qb_translate_fetch,					NULL						},	// ZEND_FETCH_RW
	{	qb_translate_fetch_element,			NULL						},	// ZEND_FETCH_DIM_RW
	{	qb_translate_fetch_property,		NULL						},	// ZEND_FETCH_OBJ_RW
	{	qb_translate_fetch,					NULL						},	// ZEND_FETCH_IS
	{	qb_translate_fetch_element,			NULL						},	// ZEND_FETCH_DIM_IS
	{	qb_translate_fetch_property,		NULL						},	// ZEND_FETCH_OBJ_IS
	{	qb_translate_fetch,					NULL						},	// ZEND_FETCH_FUNC_ARG
	{	qb_translate_fetch_element,			NULL						},	// ZEND_FETCH_DIM_FUNC_ARG
	{	qb_translate_fetch_property,		NULL						},	// ZEND_FETCH_OBJ_FUNC_ARG
	{	qb_translate_fetch,					NULL						},	// ZEND_FETCH_UNSET
	{	qb_translate_fetch_element,			NULL						},	// ZEND_FETCH_DIM_UNSET
	{	qb_translate_fetch_property,		NULL						},	// ZEND_FETCH_OBJ_UNSET
	{	qb_translate_fetch_element,			NULL						},	// ZEND_FETCH_DIM_TMP_VAR
	{	qb_translate_fetch_constant,		NULL						},	// ZEND_FETCH_CONSTANT
	{	NULL,								NULL						},	// ZEND_GOTO
	{	qb_translate_extension_op,			&factory_ext				},	// ZEND_EXT_STMT
	{	qb_translate_extension_op,			&factory_ext				},	// ZEND_EXT_FCALL_BEGIN
	{	qb_translate_extension_op,			&factory_ext				},	// ZEND_EXT_FCALL_END
	{	qb_translate_extension_op,			&factory_ext				},	// ZEND_EXT_NOP
	{	NULL,								NULL						},	// ZEND_TICKS
	{	qb_translate_send_argument,			NULL						},	// ZEND_SEND_VAR_NO_REF
	{	NULL,								NULL						},	// ZEND_CATCH
	{	NULL,								NULL						},	// ZEND_THROW
	{	qb_translate_fetch_class,			NULL						},	// ZEND_FETCH_CLASS
	{	NULL,								NULL						},	// ZEND_CLONE
	{	NULL,								NULL						},	// ZEND_RETURN_BY_REF
	{	qb_translate_init_method_call,		NULL						},	// ZEND_INIT_METHOD_CALL
	{	qb_translate_init_method_call,		NULL						},	// ZEND_INIT_STATIC_METHOD_CALL
	{	qb_translate_isset,					NULL						},	// ZEND_ISSET_ISEMPTY_VAR
	{	qb_translate_isset_element,			&factory_isset				},	// ZEND_ISSET_ISEMPTY_DIM_OBJ
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
	{	qb_translate_incdec_pre,			&factory_increment			},	// ZEND_PRE_INC_OBJ
	{	qb_translate_incdec_pre,			&factory_decrement			},	// ZEND_PRE_DEC_OBJ
	{	qb_translate_incdec_post,			&factory_increment			},	// ZEND_POST_INC_OBJ
	{	qb_translate_incdec_post,			&factory_decrement			},	// ZEND_POST_DEC_OBJ
	{	qb_translate_assign,				NULL						},	// ZEND_ASSIGN_OBJ
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
	{	qb_translate_assign,				NULL						},	// ZEND_ASSIGN_DIM
	{	qb_translate_fetch_property,		NULL						},	// ZEND_ISSET_ISEMPTY_PROP_OBJ
	{	NULL,								NULL						},	// ZEND_HANDLE_EXCEPTION
	{	qb_translate_user_opcode,			NULL						},	// ZEND_USER_OPCODE
	{	NULL,								NULL						},	// 151
	{	qb_translate_jump_set,				NULL						},	// ZEND_JMP_SET
	{	NULL,								NULL						},	// ZEND_DECLARE_LAMBDA_FUNCTION
	{	NULL,								NULL						},	// ZEND_ADD_TRAIT
	{	NULL,								NULL						},	// ZEND_BIND_TRAITS
	{	NULL,								NULL						},	// ZEND_SEPARATE
	{	qb_translate_assign_temp,			NULL						},	// ZEND_QM_ASSIGN_VAR
	{	qb_translate_jump_set,				NULL						},	// ZEND_JMP_SET_VAR
};

static void ZEND_FASTCALL qb_translate_current_instruction(qb_compiler_context *cxt) {
#ifdef ZEND_DEBUG
	const char *op_name = qb_get_zend_op_name(cxt, cxt->zend_op->opcode);
#endif
	if(cxt->zend_op->opcode != ZEND_OP_DATA) {
		USE_TSRM
		qb_operand operands[2], result;
		qb_translator *t;
		uint32_t operand_count;

		QB_G(current_line_number) = cxt->zend_op->lineno;

		// retrieve operands (second one first)
		qb_retrieve_operand(cxt, Z_OPERAND_TYPE(cxt->zend_op->op2), &cxt->zend_op->op2, &operands[1]);
		qb_retrieve_operand(cxt, Z_OPERAND_TYPE(cxt->zend_op->op1), &cxt->zend_op->op1, &operands[0]);
		operand_count = (operands[0].type != QB_OPERAND_NONE) + (operands[1].type != QB_OPERAND_NONE);

		// see whether the op returns a value
		if(RETURN_VALUE_USED(cxt->zend_op)) {
			uint32_t result_type = Z_OPERAND_TYPE(cxt->zend_op->result);
			if(result_type == Z_OPERAND_VAR || result_type == Z_OPERAND_TMP_VAR) {
				result.type = QB_OPERAND_ADDRESS;
			} else {
				result.type = QB_OPERAND_NONE;
			}
		} else {
			result.type = QB_OPERAND_NONE;
		}
		result.address = NULL;

		// look up the translator for this opcode
		t = &op_translators[cxt->zend_op->opcode];
		if(t->translate) {
			cxt->line_number = cxt->zend_op->lineno;
			t->translate(cxt, t->extra, operands, operand_count, &result);

			if(result.type != QB_OPERAND_NONE) {
				// other operand types can simply be copied onto the stack
				qb_operand *stack_item = qb_push_stack_item(cxt);
				*stack_item = result;

				if(result.type == QB_OPERAND_ADDRESS) {
					qb_lock_address(cxt, result.address);
				}
			}
		} else {
			qb_abort("Unsupported language feature");
		}
	}
}

static uint32_t ZEND_FASTCALL qb_get_lvalue_type(qb_compiler_context *cxt, uint32_t default_type) {
	uint32_t zend_op_index, stack_item_offset, stack_item_count, resolving_result_type;
	uint32_t jump_target_index1, jump_target_index2;
	uint32_t result_type = QB_TYPE_UNKNOWN;
	qb_operand *result;

#if ZEND_DEBUG
	if(default_type == QB_TYPE_UNKNOWN) {
		qb_abort("Default type cannot be unknown: use void instead");
	}
#endif

	// save state variables
	zend_op_index = cxt->zend_op_index;
	jump_target_index1 = cxt->jump_target_index1;
	jump_target_index2 = cxt->jump_target_index2;
	resolving_result_type = cxt->resolving_result_type;
	qb_preserve_stack(cxt, &stack_item_offset, &stack_item_count);

	// push a result operand onto stack
	result = qb_push_stack_item(cxt);
	result->type = QB_OPERAND_PREVIOUS_RESULT;
	result->result_type = &result_type;

	// process zend instructions until the type become known
	cxt->zend_op++;
	cxt->zend_op_index++;
	cxt->default_result_type = default_type;
	cxt->resolving_result_type = 1;
	for(;;) {
		qb_translate_current_instruction(cxt);

		if(result_type != QB_TYPE_UNKNOWN) {
			break;
		} 
		if(cxt->jump_target_index1) {
			if(cxt->jump_target_index1 != 0xFFFFFFFF && cxt->jump_target_index1 > cxt->zend_op_index) {
				cxt->zend_op = ZEND_OP(cxt->jump_target_index1);
				cxt->zend_op_index = cxt->jump_target_index1;
				cxt->jump_target_index1 = 0;
				cxt->jump_target_index2 = 0;
			} else {
				// the result is ignore if we encounter a return or backward jump
				result_type = QB_TYPE_VOID;
				break;
			}
		} else {
			cxt->zend_op++;
			cxt->zend_op_index++;
		}
	}

	// what to use when there is no place for the value to go
	if(result_type == QB_TYPE_VOID) {
		result_type = default_type;
	}

	// restore state
	cxt->zend_op = ZEND_OP(zend_op_index);
	cxt->zend_op_index = zend_op_index;
	cxt->jump_target_index1 = jump_target_index1;
	cxt->jump_target_index2 = jump_target_index2;
	cxt->resolving_result_type = resolving_result_type;
	qb_restore_stack(cxt, stack_item_offset, stack_item_count);
	return result_type;
}

static void ZEND_FASTCALL qb_translate_instruction_range(qb_compiler_context *cxt, uint32_t start_index, uint32_t end_index) {
	uint32_t zend_op_index, stack_item_offset, stack_item_count;

	// save states
	zend_op_index = cxt->zend_op_index;
	qb_preserve_stack(cxt, &stack_item_offset, &stack_item_count);

	if(cxt->pop_short_circuiting_bool) {
		qb_operand *stack_item = qb_pop_stack_item(cxt);
		qb_unlock_address(cxt, stack_item->address);
		cxt->pop_short_circuiting_bool = FALSE;
	}

	// process zend instructions until we reach the end 
	// or if an instruction is already translated
	cxt->zend_op_index = start_index;
	cxt->zend_op = ZEND_OP(start_index);
	for(;;) {
		uint32_t current_op_count;

		if(cxt->op_translations[cxt->zend_op_index] != QB_OP_INDEX_NONE && cxt->op_translations[cxt->zend_op_index] != QB_OP_INDEX_JUMP_TARGET) {
			// instruction has already been translated--do a jump there and exit
			qb_operand jump_operands[1];
			jump_operands[0].type = QB_OPERAND_JUMP_TARGET;
			jump_operands[0].jump_target_index = cxt->zend_op_index;
			qb_create_op(cxt, &factory_jump, jump_operands, 1, NULL);

			break;
		}

		// translate the current instruction, saving the op-count
		// so we know where the first resulting new op is
		current_op_count = cxt->op_count;
		qb_translate_current_instruction(cxt);

		// add a nop if new one wasn't generated
		if(current_op_count == cxt->op_count) {
			qb_create_nop(cxt);
		}

		// flag new op as a jump target if there's a placeholder in the position
		if(cxt->op_translations[cxt->zend_op_index] == QB_OP_INDEX_JUMP_TARGET) {
			qb_op *first_op = cxt->ops[current_op_count];
			first_op->flags |= QB_OP_JUMP_TARGET;
		}
		cxt->op_translations[cxt->zend_op_index] = current_op_count;
		cxt->zend_op_processed_count++;

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

	// restore the stack
	qb_restore_stack(cxt, stack_item_offset, stack_item_count);
	cxt->zend_op = ZEND_OP(zend_op_index);
	cxt->zend_op_index = zend_op_index;
}

static void ZEND_FASTCALL qb_translate_instructions(qb_compiler_context *cxt) {
	cxt->op_translations = qb_allocate_indices(cxt->pool, cxt->zend_op_array->last);
	memset(cxt->op_translations, 0xFF, cxt->zend_op_array->last * sizeof(uint32_t));
	if(cxt->op_count > 0) {
		// there are instructions for initializing static variables
		// the first translated instruction is going to be a jump target
		cxt->op_translations[0] = QB_OP_INDEX_JUMP_TARGET;
	}
	qb_translate_instruction_range(cxt, 0, cxt->zend_op_array->last);

	// make sure there's always a RET at the end
	if(cxt->op_count == 0 || cxt->ops[cxt->op_count - 1]->opcode != QB_RET) {
		qb_create_op(cxt, &factory_return, NULL, 0, NULL);
	}
}
