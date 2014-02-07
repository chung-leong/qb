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

#include "php_ini.h"
#include "ext/standard/info.h"
#include "zend_extensions.h"

int reserved_offset = -1;
int debug_compatibility_mode = TRUE;
int permitted_thread_count = 0;

zend_function * qb_find_zend_function(zval *class_name, zval *name TSRMLS_DC) {
	char *error = NULL;
#if ZEND_ENGINE_2_2 || ZEND_ENGINE_2_1
	int error_reporting_before;
#endif
	zend_fcall_info_cache fcc;
	zend_function *zfunc = NULL;

	if(class_name) {
		HashTable ht;
		zval _callable, *callable = &_callable;

		zend_hash_init(&ht, sizeof(zval *), NULL, NULL, 0);
		Z_ARRVAL_P(callable) = &ht;
		Z_TYPE_P(callable) = IS_ARRAY;

		Z_ADDREF_P(class_name);
		Z_ADDREF_P(name);
		zend_hash_next_index_insert(&ht, &class_name, sizeof(zval *), NULL);
		zend_hash_next_index_insert(&ht, &name, sizeof(zval *), NULL);

#if !ZEND_ENGINE_2_2 && !ZEND_ENGINE_2_1
		if(zend_is_callable_ex(callable, NULL, IS_CALLABLE_CHECK_NO_ACCESS, NULL, NULL, &fcc, &error TSRMLS_CC)) {
			zfunc = fcc.function_handler;
		}
#else
		// suppress the non-static function being called as static warning message
		error_reporting_before = EG(error_reporting);
		EG(error_reporting) = 0;
		if(zend_is_callable_ex(callable, IS_CALLABLE_CHECK_NO_ACCESS, NULL, NULL, NULL, &fcc.function_handler, &fcc.object_pp TSRMLS_CC)) {
			zfunc = fcc.function_handler;
		}
		EG(error_reporting) = error_reporting_before;
#endif
		zend_hash_destroy(&ht);
	} else {
#if !ZEND_ENGINE_2_2 && !ZEND_ENGINE_2_1
		if(zend_is_callable_ex(name, NULL, 0, NULL, NULL, &fcc, &error TSRMLS_CC)) {
			zfunc = fcc.function_handler;
		}
#else
		if(zend_is_callable_ex(name, IS_CALLABLE_CHECK_NO_ACCESS, NULL, NULL, NULL, &fcc.function_handler, &fcc.object_pp TSRMLS_CC)) {
			zfunc = fcc.function_handler;
		}
#endif
	}
	if(error) {
		efree(error);
	}
	return zfunc;
}

qb_import_scope * qb_find_import_scope(qb_import_scope_type type, void *associated_object TSRMLS_DC) {
	uint32_t i;
	qb_import_scope *scope;
	for(i = 0; i < QB_G(scope_count); i++) {
		scope = QB_G(scopes)[i];
		if(scope->type == type && scope->associated_object == associated_object) {
			return scope;
		}
	}
	return NULL;
}

qb_import_scope * qb_create_import_scope(qb_import_scope_type type, void *associated_object TSRMLS_DC) {
	qb_import_scope *scope = emalloc(sizeof(qb_import_scope)), **p_scope;
	memset(scope, 0, sizeof(qb_import_scope));
	scope->type = type;
	scope->associated_object = associated_object;

	if(!QB_G(scopes)) {
		qb_create_array((void **) &QB_G(scopes), &QB_G(scope_count), sizeof(qb_import_scope *), 4);
	}
	p_scope = qb_enlarge_array((void **) &QB_G(scopes), 1);
	*p_scope = scope;

	if(type == QB_IMPORT_SCOPE_OBJECT) {
		// create the scope based on the scope of the abstract object
		zval *object = associated_object;
		zend_class_entry *ce = Z_OBJCE_P(object);
		qb_import_scope *abstract_scope = qb_find_import_scope(QB_IMPORT_SCOPE_ABSTRACT_OBJECT, ce TSRMLS_CC);
		uint32_t i;

		if(!abstract_scope) {
			// create it--should inherit variables from parent class
			abstract_scope = qb_create_import_scope(QB_IMPORT_SCOPE_ABSTRACT_OBJECT, ce TSRMLS_CC);
		}

		scope->variables = abstract_scope->variables;
		scope->variable_count = abstract_scope->variable_count;
		scope->storage = emalloc(sizeof(qb_storage));
		scope->storage->flags = abstract_scope->storage->flags;
		scope->storage->segment_count = abstract_scope->storage->segment_count;
		scope->storage->segments = emalloc(sizeof(qb_memory_segment) * abstract_scope->storage->segment_count);
		memcpy(scope->storage->segments, abstract_scope->storage->segments, sizeof(qb_memory_segment) * abstract_scope->storage->segment_count);
		for(i = 0; i < scope->storage->segment_count; i++) {
			qb_memory_segment *src_segment = &abstract_scope->storage->segments[i];
			qb_memory_segment *dst_segment = &scope->storage->segments[i];
			if(src_segment->memory) {
				dst_segment->memory = emalloc(src_segment->byte_count);
				memcpy(dst_segment->memory, src_segment->memory, src_segment->byte_count);
				dst_segment->current_allocation = src_segment->byte_count;
			}
		}
	} else if(type == QB_IMPORT_SCOPE_CLASS || type == QB_IMPORT_SCOPE_ABSTRACT_OBJECT) {
		zend_class_entry *ce = associated_object;
		zend_class_entry *ancestor_ce;
		for(ancestor_ce = ce->parent; ancestor_ce; ancestor_ce = ancestor_ce->parent) {
			qb_import_scope *ancestor_scope = qb_find_import_scope(type, ancestor_ce TSRMLS_CC);

			if(ancestor_scope) {
				// inherit the properties
				scope->parent = ancestor_scope;
				scope->variable_count = ancestor_scope->variable_count;
				scope->variables = emalloc(sizeof(qb_variable *) * ancestor_scope->variable_count);
				memcpy(scope->variables, ancestor_scope->variables, sizeof(qb_variable *) * ancestor_scope->variable_count);

				// use the same storage
				scope->storage = ancestor_scope->storage;
			}
		}
	} else if(type == QB_IMPORT_SCOPE_LEXICAL) {
	}
	return scope;
}

static int32_t qb_check_address_compatibility(qb_storage *storage1, qb_address *address1, qb_storage *storage2, qb_address *address2) {
	if(!STORAGE_TYPE_MATCH(address1->type, address2->type)) {
		return FALSE;
	} else if(address1->dimension_count != address2->dimension_count) {
		return FALSE;
	} else {
		uint32_t j;
		for(j = 0; j < address1->dimension_count; j++) {
			qb_address *dim_address1 = address1->array_size_addresses[j];
			qb_address *dim_address2 = address2->array_size_addresses[j];
			if(CONSTANT(dim_address1) && CONSTANT(dim_address2)) {
				uint32_t dim1 = VALUE_IN(storage1, U32, dim_address1);
				uint32_t dim2 = VALUE_IN(storage2, U32, dim_address2);
				if(dim1 != dim2) {
					return FALSE;
				}
			} else if(CONSTANT(dim_address1)) {
				return FALSE;
			} else if(CONSTANT(dim_address2)) {
				return FALSE;
			}
		}
		if(address1->index_alias_schemes && address2->index_alias_schemes) {
			for(j = 0; j < address1->dimension_count; j++) {
				qb_index_alias_scheme *scheme1 = address1->index_alias_schemes[j];
				qb_index_alias_scheme *scheme2 = address2->index_alias_schemes[j];
				if(scheme1 && scheme2) {
					uint32_t dimension = VALUE_IN(storage1, U32, address1->dimension_addresses[j]);
					uint32_t k;
					for(k = 0; k < dimension; k++) {
						const char *alias1 = scheme1->aliases[k];
						const char *alias2 = scheme2->aliases[k];
						if(strcmp(alias1, alias2) != 0) {
							return FALSE;
						}
					}
				} else if(address1->index_alias_schemes[j]) {
					return FALSE;
				} else if(address2->index_alias_schemes[j]) {
					return FALSE;
				}
			}
		} else if(address1->index_alias_schemes) {
			return FALSE;
		} else if(address2->index_alias_schemes) {
			return FALSE;
		}
	}
	return TRUE;
}

static void qb_transfer_dimension(qb_storage *src_storage, qb_address *src_address, qb_storage *dst_storage, qb_address *dst_address, uint32_t variable_selector) {
	qb_memory_segment *segment;
	uint32_t index, *indices, index_count, i;
	if(CONSTANT(src_address)) {
		index = VALUE_IN(src_storage, U32, src_address);
		dst_address->segment_selector = QB_SELECTOR_CONSTANT_SCALAR;
		segment = &dst_storage->segments[QB_SELECTOR_CONSTANT_SCALAR];
		indices = (uint32_t *) segment->memory;
		index_count = segment->byte_count / sizeof(uint32_t);
		for(i = 0; i < index_count; i++) {
			if(indices[i] == index) {
				dst_address->segment_offset = i * sizeof(uint32_t);
				return;
			}
		}
	} else {
		segment = &dst_storage->segments[variable_selector];
		dst_address->segment_selector = variable_selector;
		index = INVALID_INDEX;
	}
	dst_address->segment_offset = segment->byte_count;
	segment->byte_count += sizeof(uint32_t);
	if(segment->byte_count > segment->current_allocation) {
		segment->current_allocation = ALIGN_TO(segment->byte_count, 1024);
		segment->memory = erealloc(segment->memory, segment->current_allocation);
	}
	VALUE_IN(dst_storage, U32, dst_address) = index;
}

qb_variable * qb_import_variable(qb_storage *storage, qb_variable *var, qb_import_scope *scope TSRMLS_DC) {
	qb_memory_segment *segment;
	uint32_t selector, start_offset, alignment, element_count, byte_count;
	uint32_t scalar_selector, array_selector;
	uint32_t i, variable_length;
	qb_variable *ivar;

	// create a copy of the variable
	variable_length = qb_get_variable_length(var);
	ivar = emalloc(variable_length);
	qb_copy_variable(var, (int8_t *) ivar);
	scope->variables = erealloc(scope->variables, sizeof(qb_variable *) * (scope->variable_count + 1));
	scope->variables[scope->variable_count++] = ivar;

	if(!scope->storage) {
		// create the storage 
		scope->storage = emalloc(sizeof(qb_storage));
		scope->storage->segment_count = QB_SELECTOR_ARRAY_START;
		scope->storage->segments = emalloc(sizeof(qb_memory_segment) * scope->storage->segment_count);
		memset(scope->storage->segments, 0, sizeof(qb_memory_segment) * scope->storage->segment_count);
	}

	// see where fixed-length variables should be placed
	switch(scope->type) {
		case QB_IMPORT_SCOPE_GLOBAL: {
			scalar_selector = QB_SELECTOR_GLOBAL_SCALAR;
			array_selector = QB_SELECTOR_GLOBAL_ARRAY;
		}	break;
		case QB_IMPORT_SCOPE_CLASS: {
			scalar_selector = QB_SELECTOR_CLASS_SCALAR;
			array_selector = QB_SELECTOR_CLASS_ARRAY;
		}	break;
		case QB_IMPORT_SCOPE_LEXICAL:
		case QB_IMPORT_SCOPE_ABSTRACT_OBJECT:
		case QB_IMPORT_SCOPE_OBJECT: {
			scalar_selector = QB_SELECTOR_OBJECT_SCALAR;
			array_selector = QB_SELECTOR_OBJECT_ARRAY;
		}	break;
	}

	if(var->address->dimension_count > 0) {
		// put dimensional values into the correct location in the scope storage
		if(var->address->dimension_count == 1) {
			qb_transfer_dimension(storage, var->address->array_size_address, scope->storage, ivar->address->array_size_address, scalar_selector);
		} else {
			for(i = 0; i < var->address->dimension_count; i++) {
				qb_transfer_dimension(storage, var->address->dimension_addresses[i], scope->storage, ivar->address->dimension_addresses[i], scalar_selector);
				qb_transfer_dimension(storage, var->address->array_size_addresses[i], scope->storage, ivar->address->array_size_addresses[i], scalar_selector);
			}
		}
	}

	// assign space to the variable
	if(SCALAR(ivar->address)) {
		byte_count = BYTE_COUNT(1, ivar->address->type);
		alignment = max(byte_count, 4);
		selector = scalar_selector;
	} else {
		alignment = 16;
		if(FIXED_LENGTH(ivar->address)) {
			element_count = ARRAY_SIZE_IN(scope->storage, ivar->address);
			byte_count = BYTE_COUNT(element_count, ivar->address->type);
			if(byte_count < 10240) {
				selector = array_selector;
			} else {
				selector = scope->storage->segment_count;
			}
		} else {
			element_count = 0;
			byte_count = 0;
			selector = scope->storage->segment_count;
		}
	}

	if(selector >= scope->storage->segment_count) {
		scope->storage->segment_count = selector + 1;
		scope->storage->segments = erealloc(scope->storage->segments, sizeof(qb_memory_segment) * scope->storage->segment_count);
		segment = &scope->storage->segments[selector];
		memset(segment, 0, sizeof(qb_memory_segment));
	} else {
		segment = &scope->storage->segments[selector];
	}

	start_offset = ALIGN_TO(segment->byte_count, alignment);
	segment->byte_count = start_offset + byte_count;

	if(selector < QB_SELECTOR_ARRAY_START) {
		if(segment->byte_count > segment->current_allocation) {
			// allocate actual memory
			segment->current_allocation = ALIGN_TO(segment->byte_count, 1024);
			segment->memory = erealloc(segment->memory, segment->current_allocation);
		}
	}

	ivar->address->segment_selector = selector;
	ivar->address->segment_offset = start_offset;
	return ivar;
}

qb_import_scope * qb_get_import_scope(qb_storage *storage, qb_variable *var, zval *object TSRMLS_DC) {
	qb_import_scope *scope;
	qb_import_scope_type scope_type;
	void *associated_object;

	if(var->flags & QB_VARIABLE_GLOBAL) {
		scope_type = QB_IMPORT_SCOPE_GLOBAL;
		associated_object = &EG(symbol_table);
	} else if((var->flags & QB_VARIABLE_CLASS) || (var->flags & QB_VARIABLE_CLASS_CONSTANT)) {
		scope_type = QB_IMPORT_SCOPE_CLASS;
		if(var->zend_class) {
			associated_object = var->zend_class;
		} else {
			// it's a variable qualifed with static::
			associated_object = Z_OBJCE_P(object);
		}
	} else if(var->flags & QB_VARIABLE_CLASS_INSTANCE) {
		if(object) {
			scope_type = QB_IMPORT_SCOPE_OBJECT;
			associated_object = object;
		} else {
			scope_type = QB_IMPORT_SCOPE_ABSTRACT_OBJECT;
			associated_object = var->zend_class;
		}
	} else if(var->flags & QB_VARIABLE_LEXICAL) {
		zend_op_array *op_array = EG(current_execute_data)->op_array;
		scope_type = QB_IMPORT_SCOPE_LEXICAL;
		associated_object = op_array->static_variables;
	}
	scope = qb_find_import_scope(scope_type, associated_object TSRMLS_CC);
	if(!scope) {
		scope = qb_create_import_scope(scope_type, associated_object TSRMLS_CC);
	}
	return scope;
}

qb_variable * qb_get_import_variable(qb_storage *storage, qb_variable *var, qb_import_scope *scope TSRMLS_DC) {
	uint32_t i;
	qb_variable *ivar;
	// look for the variable
	for(i = 0; i < scope->variable_count; i++) {
		ivar = scope->variables[i];
		if(ivar->hash_value == var->hash_value && ivar->name_length == var->name_length) {
			if(strcmp(ivar->name, var->name) == 0) {
				if(qb_check_address_compatibility(scope->storage, ivar->address, storage, var->address)) {
					if(!(var->flags & QB_ADDRESS_READ_ONLY)) {
						ivar->flags &= ~QB_ADDRESS_READ_ONLY;
					}
					return ivar;
				} else {
					if(READ_ONLY(ivar->address) && READ_ONLY(var->address)) {
						// permit a variable to be imported differently if it's not modified 
					} else {
						return NULL;
					}
				}
			}
		}
	}
	ivar = qb_import_variable(storage, var, scope TSRMLS_CC);
	return ivar;
}

uint32_t qb_import_external_symbol(qb_external_symbol_type type, const char *name, uint32_t name_len, void *pointer TSRMLS_DC) {
	uint32_t i;
	qb_external_symbol *symbol;
	for(i = 0; i < QB_G(external_symbol_count); i++) {
		symbol = &QB_G(external_symbols)[i];
		if(symbol->pointer == pointer && symbol->type == type) {
			return i;
		}
	}
	if(!QB_G(external_symbols)) {
		qb_create_array((void **) &QB_G(external_symbols), &QB_G(external_symbol_count), sizeof(qb_external_symbol), 4);
	}
	i = QB_G(external_symbol_count);
	symbol = qb_enlarge_array((void **) &QB_G(external_symbols), 1);
	symbol->type = type;
	symbol->name = name;
	symbol->name_length = name_len;
	symbol->pointer = pointer;
	return i;
}

qb_build_context * qb_get_current_build(TSRMLS_D) {
	qb_build_context *cxt = QB_G(build_context);
	if(!cxt) {
		cxt = emalloc(sizeof(qb_build_context));
		qb_initialize_build_context(cxt TSRMLS_CC);
		QB_G(build_context) = cxt;
	}
	return cxt;
}

static void qb_discard_current_build(TSRMLS_D) {
	qb_build_context *cxt = QB_G(build_context);
	if(cxt) {
		qb_free_build_context(cxt);
		efree(cxt);
		QB_G(build_context) = NULL;
	}
}

qb_function * qb_find_compiled_function(zend_function *zfunc) {
	uint32_t i;
	qb_function *qfunc = qb_get_compiled_function(zfunc);
	if(!qfunc) {
		// see if the function in question in also in the middle of being compiled
		qb_build_context *build_context;
		TSRMLS_FETCH();
		build_context = QB_G(build_context);
		for(i = 0; i < build_context->compiler_context_count; i++) {
			qb_compiler_context *compiler_cxt = build_context->compiler_contexts[i];
			if(compiler_cxt->function_prototype.zend_op_array == &zfunc->op_array) {
				return &compiler_cxt->function_prototype;
			}
		}
	}
	return NULL;
}

void qb_attach_compiled_function(qb_function *qfunc, zend_op_array *op_array) {
	QB_SET_FUNCTION(op_array, qfunc);

	// save the pointer in the reserved array so we can find it again in the destructor
	op_array->reserved[reserved_offset] = qfunc;
}

qb_function * qb_get_compiled_function(zend_function *zfunc) {
	zend_op_array *op_array = &zfunc->op_array;
	if(zfunc->type == ZEND_USER_FUNCTION && QB_IS_COMPILED(op_array)) {
		return QB_GET_FUNCTION(op_array);
	}
	return NULL;
}

qb_main_thread * qb_get_main_thread(TSRMLS_D) {
	return &QB_G(main_thread);
}

static void qb_start_execution_timer(qb_function *qfunc TSRMLS_DC) {
	if(QB_G(execution_log_path)[0]) {
		double start_time = qb_get_high_res_timestamp();
		QB_G(execution_start_time) = start_time;
	}
}

static void qb_stop_execution_timer(qb_function *qfunc TSRMLS_DC) {
	if(QB_G(execution_log_path)[0]) {
		double start_time = QB_G(execution_start_time);
		double end_time = qb_get_high_res_timestamp();
		double duration = end_time - start_time;
		if(duration > 0) {
			if(qfunc->name[0] != '_') {
				php_stream *stream = php_stream_open_wrapper_ex(QB_G(execution_log_path), "a", USE_PATH | ENFORCE_SAFE_MODE | REPORT_ERRORS, NULL, NULL);
				if(stream) {
					uint32_t file_id = FILE_ID(qfunc->line_id);
					const char *source_file = qb_get_source_file_path(file_id TSRMLS_CC);
					php_stream_printf(stream TSRMLS_CC, "%s\t%s\t%f\n", source_file, qfunc->name, duration);
					php_stream_close(stream);
				}
			}
		}
	}
}

#ifdef ZEND_ACC_GENERATOR
static qb_interpreter_context * qb_get_generator_context(zend_generator *generator TSRMLS_DC) {
	uint32_t i;
	for(i = 0; i < QB_G(generator_context_count); i++) {
		qb_generator_context *g = &QB_G(generator_contexts)[i];
		if(g->generator == generator) {
			return g->interpreter_context;
		}
	}
	return NULL;
}

static qb_interpreter_context * qb_add_generator_context(zend_generator *generator TSRMLS_DC) {
	qb_generator_context *g;
	uint32_t index = QB_G(generator_context_count)++;
	QB_G(generator_contexts) = erealloc(QB_G(generator_contexts), sizeof(qb_generator_context) * QB_G(generator_context_count));
	g = &QB_G(generator_contexts)[index];
	g->generator = generator;
	g->interpreter_context = emalloc(sizeof(qb_interpreter_context));
	return g->interpreter_context;
}

static void qb_remove_generator_context(zend_generator *generator TSRMLS_DC) {
	uint32_t i;
	for(i = 0; i < QB_G(generator_context_count); i++) {
		qb_generator_context *g = &QB_G(generator_contexts)[i];
		if(g->generator == generator) {
			efree(g->interpreter_context);
			QB_G(generator_context_count)--;
			if(i != QB_G(generator_context_count)) {
				memmove(g, g + 1, sizeof(qb_generator_context) * (QB_G(generator_context_count) - i));
			}
			if(QB_G(generator_context_count) == 0) {
				efree(QB_G(generator_contexts));
				QB_G(generator_contexts) = NULL;
			}
			break;
		}
	}
}
#endif

int qb_user_opcode_handler(ZEND_OPCODE_HANDLER_ARGS) {
	zend_op_array *op_array = EG(active_op_array);
	qb_function *qfunc = QB_GET_FUNCTION(op_array);
	if(!qfunc) {
		if(QB_G(main_thread).type == QB_THREAD_UNINITIALIZED) {
			qb_initialize_main_thread(&QB_G(main_thread) TSRMLS_CC);
		}
		if(QB_G(build_context)) {
			qb_build_context *build_cxt = qb_get_current_build(TSRMLS_C);
			qb_build(build_cxt);
			qfunc = QB_GET_FUNCTION(op_array);
			qb_discard_current_build(TSRMLS_C);
		}
	}
	if(qfunc) {
		if(qfunc->flags & QB_FUNCTION_GENERATOR) {
#ifdef ZEND_ACC_GENERATOR
			zend_generator *generator = (zend_generator *) EG(return_value_ptr_ptr);
			qb_interpreter_context *interpreter_cxt = qb_get_generator_context(generator TSRMLS_CC);
			if(!interpreter_cxt) {
				interpreter_cxt = qb_add_generator_context(generator TSRMLS_CC);
				qb_initialize_interpreter_context(interpreter_cxt, qfunc, NULL TSRMLS_CC);
				if(qb_execute_rewind(interpreter_cxt)) {
					// the function is returning nothing
					qb_free_interpreter_context(interpreter_cxt);
					qb_remove_generator_context(generator TSRMLS_CC);
				} else {
					// clear the flag so the generator doesn't get freed
					// a somewhat nasty workaround
					op_array->fn_flags &= ~ZEND_ACC_GENERATOR;
				}
			} else {
				if(qb_execute_resume(interpreter_cxt)) {
					qb_free_interpreter_context(interpreter_cxt);
					qb_remove_generator_context(generator TSRMLS_CC);

					// restore the flag
					op_array->fn_flags |= ZEND_ACC_GENERATOR;
				}
			}
#endif
		} else {
			qb_interpreter_context _interpreter_cxt, *interpreter_cxt = &_interpreter_cxt;
			qb_start_execution_timer(qfunc TSRMLS_CC);
			qb_initialize_interpreter_context(interpreter_cxt, qfunc, QB_G(caller_interpreter_context) TSRMLS_CC);
			qb_execute(interpreter_cxt);
			qb_free_interpreter_context(interpreter_cxt);
			qb_stop_execution_timer(qfunc TSRMLS_CC);
		}
#if ZEND_ENGINE_2_2 || ZEND_ENGINE_2_1
		EG(current_execute_data) = EG(current_execute_data)->prev_execute_data;
#endif
		return ZEND_USER_OPCODE_RETURN;
	} else {
		execute_data->opline++;
		return ZEND_USER_OPCODE_CONTINUE;
	}
}

void qb_zend_ext_op_array_ctor(zend_op_array *op_array) {
	TSRMLS_FETCH();
	const char *doc_comment = CG(doc_comment);
	if(doc_comment && qb_find_engine_tag(doc_comment)) {
		zend_op *user_op;
		qb_build_context *build_cxt;
		qb_function_tag *tag;

		// tag the function
		build_cxt = qb_get_current_build(TSRMLS_C);
		tag = qb_enlarge_array((void **) &build_cxt->function_tags, 1);
		tag->scope = CG(active_class_entry);
		tag->function_name = NULL;
		tag->file_path = CG(compiled_filename);
		tag->line_number = CG(zend_lineno);

		// add QB instruction
		user_op = &op_array->opcodes[op_array->last++];
		user_op->opcode = QB_USER_OPCODE;
		Z_OPERAND_TYPE(user_op->op1) = IS_UNUSED;
		Z_OPERAND_TYPE(user_op->op2) = IS_UNUSED;
		Z_OPERAND_TYPE(user_op->result) = IS_UNUSED;

		// stash the tag in the node until the compilation is done
		QB_SET_FUNCTION(op_array, tag);
	}
}

void qb_zend_ext_op_array_handler(zend_op_array *op_array) {
	if(QB_IS_COMPILED(op_array)) {
		qb_function_tag *tag = QB_GET_FUNCTION(op_array);

		// save the function name so we can find the function later
		// op_array might be temporary so we can't use this pointer
		tag->function_name = op_array->function_name;

		// save the doc comment pointer in case we can't locate
		// the function by name
		tag->doc_comment = op_array->doc_comment;

		QB_SET_FUNCTION(op_array, NULL);
	}
}

void qb_zend_ext_op_array_dtor(zend_op_array *op_array) {
	TSRMLS_FETCH();
	qb_function *qfunc = op_array->reserved[reserved_offset];
	if(qfunc) {
		qb_free_function(qfunc);
	}
}

extern zend_extension zend_extension_entry;

int qb_install_user_opcode_handler() {
	// get a reserved offset
	reserved_offset = zend_get_resource_handle(&zend_extension_entry);
	if(reserved_offset == -1) {
		return FAILURE;
	}

	// set the opcode handler
	if(zend_set_user_opcode_handler(QB_USER_OPCODE, qb_user_opcode_handler) == FAILURE) {
		return FAILURE;
	}

	// register extension
	zend_register_extension(&zend_extension_entry, NULL);
	return SUCCESS;
}

int qb_is_compiled_function(zend_function *zfunc) {
	return qb_get_compiled_function(zfunc) != NULL;
}

ZEND_DECLARE_MODULE_GLOBALS(qb)

/* {{{ qb_functions[]
 *
 * Every user visible function must have an entry in qb_functions[].
 */
const zend_function_entry qb_functions[] = {
	PHP_FE(qb_compile,		NULL)
	PHP_FE(qb_extract,		NULL)
#ifdef PHP_FE_END
	PHP_FE_END	/* Must be the last line in qb_functions[] */
#else
	{NULL, NULL, NULL}
#endif
};
/* }}} */

/* {{{ qb_module_entry
 */
zend_module_entry qb_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
	STANDARD_MODULE_HEADER,
#endif
	"qb",
	qb_functions,
	PHP_MINIT(qb),
	PHP_MSHUTDOWN(qb),
	PHP_RINIT(qb),		/* Replace with NULL if there's nothing to do at request start */
	PHP_RSHUTDOWN(qb),	/* Replace with NULL if there's nothing to do at request end */
	PHP_MINFO(qb),
#if ZEND_MODULE_API_NO >= 20010901
	STRING(QB_MAJOR_VERSION) "." STRING(QB_MINOR_VERSION), /* Replace with version number for your extension */
#endif
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

extern qb_debug_interface debug_interface;

zend_extension zend_extension_entry = {
	"QB",
	STRING(QB_MAJOR_VERSION) "." STRING(QB_MINOR_VERSION),
	"Chung Leong",
	"http://www.php-qb.net/",
	"Copyright (c) 2013-2014",
	NULL,
	NULL,
	NULL,           /* activate_func_t */
	NULL,           /* deactivate_func_t */
	NULL,           /* message_handler_func_t */
	qb_zend_ext_op_array_handler,           /* op_array_handler_func_t */
	NULL,			/* statement_handler_func_t */
	NULL,           /* fcall_begin_handler_func_t */
	NULL,           /* fcall_end_handler_func_t */
	qb_zend_ext_op_array_ctor,			/* op_array_ctor_func_t */
	qb_zend_ext_op_array_dtor,			/* op_array_dtor_func_t */
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	&debug_interface,			/* reserved8 */
	NULL,
	-1
};

#ifdef COMPILE_DL_QB
ZEND_GET_MODULE(qb)
#endif

static ZEND_INI_MH(OnUpdatePath) {
	if(new_value_length > 1) {
		uint32_t i = new_value_length - 1;
		int last_char = new_value[i];
		if(last_char == '\\' || last_char == '/') {
			new_value_length--;
		}
#ifdef ZEND_WIN32
		for(i = 0; i < new_value_length; i++) {
			if(new_value[i] == '/') {
				new_value[i] = '\\';
			}
		}
#endif
	}
	return OnUpdateString(entry, new_value, new_value_length, mh_arg1, mh_arg2, mh_arg3, stage TSRMLS_CC);
}

static ZEND_INI_MH(OnThreadCount) /* {{{ */
{
	OnUpdateLong(entry, new_value, new_value_length, mh_arg1, mh_arg2, mh_arg3, stage TSRMLS_CC);

	if(QB_G(thread_count) <= 0) {
		QB_G(thread_count) = qb_get_cpu_count();
	} else if(QB_G(thread_count) > MAX_THREAD_COUNT) {
		QB_G(thread_count) = MAX_THREAD_COUNT;
	}

	// allow the thread count to get smaller than the initial but not bigger
	if(permitted_thread_count == 0) {
		permitted_thread_count = QB_G(thread_count);
	} else {
		if(QB_G(thread_count) > permitted_thread_count) {
			QB_G(thread_count) = permitted_thread_count;
		}
	}
	return SUCCESS;
}

/* {{{ PHP_INI
 */
PHP_INI_BEGIN()
    STD_PHP_INI_BOOLEAN("qb.allow_native_compilation",		"0",	PHP_INI_SYSTEM,	OnUpdateBool,	allow_native_compilation,		zend_qb_globals,	qb_globals)
	STD_PHP_INI_BOOLEAN("qb.allow_memory_map",				"1",	PHP_INI_SYSTEM,	OnUpdateBool,	allow_memory_map,				zend_qb_globals,	qb_globals)

	STD_PHP_INI_ENTRY("qb.compiler_path",    				"",		PHP_INI_SYSTEM, OnUpdatePath,	compiler_path,    				zend_qb_globals,	qb_globals)
	STD_PHP_INI_ENTRY("qb.compiler_env_path",  				"",		PHP_INI_SYSTEM, OnUpdatePath,	compiler_env_path,  			zend_qb_globals,	qb_globals)
	STD_PHP_INI_ENTRY("qb.native_code_cache_path",  		"",		PHP_INI_SYSTEM, OnUpdatePath,	native_code_cache_path,			zend_qb_globals,	qb_globals)
	STD_PHP_INI_ENTRY("qb.execution_log_path",  			"",		PHP_INI_SYSTEM, OnUpdatePath,	execution_log_path,				zend_qb_globals,	qb_globals)

	STD_PHP_INI_ENTRY("qb.thread_count",					"0",	PHP_INI_ALL, 	OnThreadCount,	thread_count,					zend_qb_globals,	qb_globals)

	STD_PHP_INI_BOOLEAN("qb.allow_bytecode_interpretation",	"1",	PHP_INI_ALL,	OnUpdateBool,	allow_bytecode_interpretation,	zend_qb_globals,	qb_globals)
	STD_PHP_INI_BOOLEAN("qb.allow_debugger_inspection",		"1",	PHP_INI_ALL,	OnUpdateBool,	allow_debugger_inspection,		zend_qb_globals,	qb_globals)
	STD_PHP_INI_BOOLEAN("qb.allow_debug_backtrace",			"0",	PHP_INI_ALL,	OnUpdateBool,	allow_debug_backtrace,			zend_qb_globals,	qb_globals)
    STD_PHP_INI_BOOLEAN("qb.debug_with_exact_type",			"0",	PHP_INI_ALL,	OnUpdateBool,	debug_with_exact_type,			zend_qb_globals,	qb_globals)
    STD_PHP_INI_BOOLEAN("qb.column_major_matrix",			"0",	PHP_INI_ALL,	OnUpdateBool,	column_major_matrix,			zend_qb_globals,	qb_globals)

	STD_PHP_INI_BOOLEAN("qb.compile_to_native",				"0",	PHP_INI_ALL,	OnUpdateBool,	compile_to_native,				zend_qb_globals,	qb_globals)
    STD_PHP_INI_BOOLEAN("qb.show_opcodes",					"0",	PHP_INI_ALL,	OnUpdateBool,	show_opcodes,					zend_qb_globals,	qb_globals)
    STD_PHP_INI_BOOLEAN("qb.show_native_source",			"0",	PHP_INI_ALL,	OnUpdateBool,	show_native_source,				zend_qb_globals,	qb_globals)
    STD_PHP_INI_BOOLEAN("qb.show_compiler_errors",			"0",	PHP_INI_ALL,	OnUpdateBool,	show_compiler_errors,			zend_qb_globals,	qb_globals)
    STD_PHP_INI_BOOLEAN("qb.show_source_opcodes",			"0",	PHP_INI_ALL,	OnUpdateBool,	show_source_opcodes,			zend_qb_globals,	qb_globals)

PHP_INI_END()
/* }}} */

/* {{{ php_qb_init_globals
 */
static void php_qb_init_globals(zend_qb_globals *qb_globals)
{
}
/* }}} */

#if ZEND_ENGINE_2_1
int zend_startup_strtod(void);
int zend_shutdown_strtod(void);
#endif

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(qb)
{
	qb_main_thread main_thread;

	ZEND_INIT_MODULE_GLOBALS(qb, php_qb_init_globals, NULL);

	REGISTER_INI_ENTRIES();

	REGISTER_LONG_CONSTANT("QB_SCAN_FILE",			QB_SCAN_FILE,			CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("QB_SCAN_ALL",			QB_SCAN_ALL,			CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("QB_START_DEFERRAL",		QB_START_DEFERRAL,		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("QB_END_DEFERRAL",		QB_END_DEFERRAL,		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("QB_PBJ_DETAILS",		QB_PBJ_DETAILS,			CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("QB_PBJ_DECLARATION",	QB_PBJ_DECLARATION,		CONST_CS | CONST_PERSISTENT);

#ifdef VC6_MSVCRT
	if(qb_get_vc6_msvcrt_functions() != SUCCESS) {
		return FAILURE;
	}
#endif

	qb_install_user_opcode_handler();

	qb_initialize_thread_pool(TSRMLS_C);
	qb_initialize_main_thread(&main_thread TSRMLS_CC);
	qb_add_workers(&main_thread);
	qb_free_main_thread(&main_thread);

#if ZEND_ENGINE_2_1
	zend_startup_strtod();
#endif
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(qb)
{
	UNREGISTER_INI_ENTRIES();

	qb_free_thread_pool();

#if ZEND_ENGINE_2_1
	zend_shutdown_strtod();
#endif
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(qb)
{
	uint32_t i;
	for(i = 0; i < sizeof(QB_G(static_zvals)) / sizeof(QB_G(static_zvals)[0]); i++) {
		zval *value = (zval *) &QB_G(static_zvals)[i];
#if !ZEND_ENGINE_2_2 && !ZEND_ENGINE_2_1		
		value->refcount__gc = 1;
		value->is_ref__gc = 0;
#else
		value->refcount = 2;
		value->is_ref = 0;
#endif
		value->type = IS_STRING;
	}
	QB_G(main_thread).type = QB_THREAD_UNINITIALIZED;
	QB_G(static_zval_index) = 0;
	QB_G(build_context) = NULL;
	QB_G(scopes) = NULL;
	QB_G(scope_count) = 0;
	QB_G(external_symbols) = NULL;
	QB_G(external_symbol_count) = 0;
	QB_G(caller_interpreter_context) = NULL;
	QB_G(exceptions) = NULL;
	QB_G(exception_count) = 0;
	QB_G(source_files) = NULL;
	QB_G(source_file_count) = 0;
#ifdef ZEND_ACC_GENERATOR
	QB_G(generator_contexts) = NULL;
	QB_G(generator_context_count) = 0;
#endif
#ifdef NATIVE_COMPILE_ENABLED
	QB_G(native_code_bundles) = NULL;
	QB_G(native_code_bundle_count) = 0;
#endif
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(qb)
{
	uint32_t i, j;
	qb_discard_current_build(TSRMLS_C);

	if(QB_G(main_thread).type != QB_THREAD_UNINITIALIZED) {
		if(CG(unclean_shutdown)) {
			// bailing out of script--make sure threads are shutdown
			qb_terminate_associated_workers(&QB_G(main_thread));
		}
		qb_free_main_thread(&QB_G(main_thread));
	}

	for(i = 0; i < QB_G(scope_count); i++) {
		qb_import_scope *scope = QB_G(scopes)[i];
		if(scope->type != QB_IMPORT_SCOPE_OBJECT) {
			for(j = (scope->parent) ? scope->parent->variable_count : 0; j < scope->variable_count; j++) {
				qb_variable *var = scope->variables[j];
				efree(var);
			}
			efree(scope->variables);
		}
		
		if(!scope->parent) {
			for(j = 0; j < scope->storage->segment_count; j++) {
				qb_memory_segment *segment = &scope->storage->segments[j];
				if(!segment->flags & QB_SEGMENT_PREALLOCATED) {
					qb_release_segment(segment);
				}
			}
			efree(scope->storage->segments);
			efree(scope->storage);
		}
		efree(scope);
	}
	qb_destroy_array((void **) &QB_G(scopes));
	qb_destroy_array((void **) &QB_G(external_symbols));
	qb_destroy_array((void **) &QB_G(exceptions));
	qb_destroy_array((void **) &QB_G(source_files));

#ifdef ZEND_ACC_GENERATOR
	for(i = 0; i < QB_G(generator_context_count); i++) {
		qb_generator_context *g = &QB_G(generator_contexts)[i];
		qb_free_interpreter_context(g->interpreter_context);
		efree(g->interpreter_context);
	}
#endif
#ifdef NATIVE_COMPILE_ENABLED
	for(i = 0; i < QB_G(native_code_bundle_count); i++) {
		qb_native_code_bundle *bundle = &QB_G(native_code_bundles)[i];
		qb_free_native_code(bundle);
	}
	qb_destroy_array((void **) &QB_G(native_code_bundles));
#endif
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(qb)
{
	char buffer[256];
	uint32_t i;
	uint32_t cpu_count = qb_get_cpu_count();
	qb_diagnostics diag;
	php_info_print_table_start();
	php_info_print_table_header(2, "qb support", "enabled");

	php_info_print_table_row(2, "Version", STRING(QB_MAJOR_VERSION) "." STRING(QB_MINOR_VERSION));
	php_info_print_table_row(2, "Release Name", QB_FULL_RELEASE_NAME);

#if __SSE4_2__
	php_info_print_table_row(2, "SSE version", "4.2");
#elif __SSE4_1__
	php_info_print_table_row(2, "SSE version", "4.1");
#elif __SSE3__
	php_info_print_table_row(2, "SSE version", "3");
#elif __SSE2__
	php_info_print_table_row(2, "SSE version", "2");
#elif __SSE__
	php_info_print_table_row(2, "SSE version", "1");
#endif

#if __AVX__
	php_info_print_table_row(2, "AVX version", "1");
#endif
#if __AVX2__
	php_info_print_table_row(2, "AVX version", "2");
#endif

	sprintf(buffer, "%u", cpu_count);
	php_info_print_table_row(2, "CPU count", buffer);

	qb_run_diagnostics(&diag TSRMLS_CC);
	php_info_print_table_colspan_header(2, "Diagnostics");

	for(i = 1; i < QB_DIAGNOSTIC_SPEED_TEST_COUNTS; i++) {
		static const char *test_types[] = {
			NULL,
			"Integer addition throughput",
			"Integer multiplication throughput",
			"Integer division throughput",
			"Integer multiply-accumulate throughput",
			"Floating point addition throughput",
			"Floating point multiplication throughput",
			"Floating point division throughput",
			"Floating point multiply-accumulate throughput",
			"Vector addition throughput",
			"Vector multiplication throughput",
			"Vector division throughput",
			"Vector multiply-accumulate throughput",
		};
		snprintf(buffer, sizeof(buffer), "%g million operations per second", round(diag.instruction_speeds[i] / 1000000));
		php_info_print_table_row(2, test_types[i], buffer);
	}
	php_info_print_table_end();

	DISPLAY_INI_ENTRIES();
}
/* }}} */


/* Remove the following function when you have succesfully modified config.m4
   so that your module can be compiled into PHP, it exists only for testing
   purposes. */

/* Every user-visible function in PHP should document itself in the source */

/* {{{ proto bool qb_compile(callable name, string return_type, array variable_types [, integer options])
   Convert PHP instructions to qb instructions */
PHP_FUNCTION(qb_compile)
{
	zval *arg1 = NULL, *arg2 = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|zz", &arg1, &arg2) == FAILURE) {
		return;
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool qb_extract(callable name, integer output_type)
   Extract information from a given resource */
PHP_FUNCTION(qb_extract)
{
	qb_extractor_context _cxt, *cxt = &_cxt;
	zval *input = NULL;
	long output_type;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zl|", &input, &output_type) == FAILURE) {
		return;
	}

	qb_initialize_extractor_context(cxt, input, return_value TSRMLS_CC);
	switch(output_type) {
		case QB_PBJ_DETAILS:
		case QB_PBJ_DECLARATION:
			qb_extract_pbj_info(cxt, output_type);
			break;
	}
	qb_free_extractor_context(cxt);
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
