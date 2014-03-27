/*
  +----------------------------------------------------------------------+
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

int debug_compatibility_mode = TRUE;
int permitted_thread_count = 0;

qb_import_scope * qb_find_import_scope(qb_import_scope_type type, void *associated_object TSRMLS_DC) {
	qb_import_scope *scope;
	uint32_t i;
	for(i = 0; i < QB_G(scope_count); i++) {
		scope = QB_G(scopes)[i];
		if(scope->type == type) {
			int match = FALSE;
			switch(scope->type) {
				case QB_IMPORT_SCOPE_CLASS:
				case QB_IMPORT_SCOPE_ABSTRACT_OBJECT: {
					zend_class_entry *class_entry = associated_object;
					match = (scope->class_entry == class_entry);
				}	break;
				case QB_IMPORT_SCOPE_OBJECT: {
					zval *object = associated_object;
					match = (Z_OBJ_HANDLE_P(scope->object) == Z_OBJ_HANDLE_P(object));
				}	break;
				case QB_IMPORT_SCOPE_LEXICAL:
				case QB_IMPORT_SCOPE_GLOBAL: {
					HashTable *symbol_table = associated_object;
					match = (scope->symbol_table == symbol_table);
				}	break;
				default: {
					break;
				}
			}
			if(match) {
				return scope;
			}
		}
	}
	if(type == QB_IMPORT_SCOPE_OBJECT) {
		// see if we can reuse a scope from an object released earlier
		zval *object = associated_object;
		zend_class_entry *class_entry = Z_OBJCE_P(object);
		for(i = 0; i < QB_G(scope_count); i++) {
			scope = QB_G(scopes)[i];
			if(scope->type == QB_IMPORT_SCOPE_FREED_OBJECT) {
				if(scope->parent->class_entry == class_entry) {
					Z_ADDREF_P(object);
					scope->object = object;
					scope->type = QB_IMPORT_SCOPE_OBJECT;
					return scope;
				}
			}
		}
	}
	return NULL;
}

qb_import_scope * qb_create_import_scope(qb_import_scope_type type, void *associated_object TSRMLS_DC) {
	qb_import_scope *scope = emalloc(sizeof(qb_import_scope)), **p_scope;
	memset(scope, 0, sizeof(qb_import_scope));
	scope->type = type;
	switch(type) {
		case QB_IMPORT_SCOPE_CLASS:
		case QB_IMPORT_SCOPE_ABSTRACT_OBJECT: {
			zend_class_entry *class_entry = associated_object;
			scope->class_entry = class_entry;
		}	break;
		case QB_IMPORT_SCOPE_OBJECT: {
			zval *object = associated_object;
			Z_ADDREF_P(object);
			SEPARATE_ZVAL(&object);
			scope->object = object;
		}	break;
		case QB_IMPORT_SCOPE_LEXICAL:
		case QB_IMPORT_SCOPE_GLOBAL: {
			HashTable *symbol_table = associated_object;
			scope->symbol_table = symbol_table;
		}	break;
		default: {
			break;
		}
	}

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

		scope->parent = abstract_scope;
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
			if(IS_IMMUTABLE(dim_address1) && IS_IMMUTABLE(dim_address2)) {
				uint32_t dim1 = VALUE_IN(storage1, U32, dim_address1);
				uint32_t dim2 = VALUE_IN(storage2, U32, dim_address2);
				if(dim1 != dim2) {
					return FALSE;
				}
			} else if(IS_IMMUTABLE(dim_address1)) {
				return FALSE;
			} else if(IS_IMMUTABLE(dim_address2)) {
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
	if(IS_IMMUTABLE(src_address)) {
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
		default: {
			break;
		}
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
	if(IS_SCALAR(ivar->address)) {
		byte_count = BYTE_COUNT(1, ivar->address->type);
		alignment = max(byte_count, 4);
		selector = scalar_selector;
	} else {
		alignment = 16;
		if(IS_FIXED_LENGTH(ivar->address)) {
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
					if(IS_READ_ONLY(ivar->address) && IS_READ_ONLY(var->address)) {
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


qb_function * qb_find_compiled_function(zend_function *zfunc TSRMLS_DC) {
	uint32_t i;
	qb_function *qfunc = qb_get_compiled_function(zfunc);
	if(!qfunc) {
		// see if the function in question in also in the middle of being compiled
		qb_build_context *build_cxt = QB_G(build_context);
		if(build_cxt) {
			for(i = 0; i < build_cxt->compiler_context_count; i++) {
				qb_compiler_context *compiler_cxt = build_cxt->compiler_contexts[i];
				if(compiler_cxt->function_prototype.zend_op_array == &zfunc->op_array) {
					return &compiler_cxt->function_prototype;
				}
			}
		}
	}
	return NULL;
}

void qb_attach_compiled_function(qb_function *qfunc, zend_op_array *op_array TSRMLS_DC) {
	qb_function **p_function;
	if(!QB_G(compiled_functions)) {
		qb_create_array((void **) &QB_G(compiled_functions), &QB_G(compiled_function_count), sizeof(qb_function *), 16);
	}
	p_function = qb_enlarge_array((void **) &QB_G(compiled_functions), 1);
	*p_function = qfunc;
	QB_SET_FUNCTION(op_array, qfunc);
}

qb_function * qb_get_compiled_function(zend_function *zfunc) {
	zend_op_array *op_array = &zfunc->op_array;
	if(zfunc->type == ZEND_USER_FUNCTION && QB_IS_COMPILED(op_array)) {
		return QB_GET_FUNCTION(op_array);
	}
	return NULL;
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

static void qb_scan_function(qb_build_context *cxt, zend_function *function, zend_class_entry *scope) {
	zend_op_array *op_array = &function->op_array;
	if(QB_IS_COMPILED(op_array)) {
		if(!QB_GET_FUNCTION(op_array)) {
			qb_function_tag *tag = qb_enlarge_array((void **) &cxt->function_tags, 1);
			tag->scope = scope;
			tag->op_array = op_array;
		}
	}
}

static void qb_scan_function_table(qb_build_context *cxt, HashTable *function_table, zend_class_entry *scope) {
	Bucket *p;
	for(p = function_table->pListTail; p; p = p->pListLast) {
		zend_function *zfunc = p->pData;
		if(zfunc->type == ZEND_USER_FUNCTION) {
			qb_scan_function(cxt, zfunc, scope);
		} else {
			break;
		}
	}
}

static void qb_scan_class_table(qb_build_context *cxt, HashTable *class_table) {
	Bucket *p;
	for(p = class_table->pListTail; p; p = p->pListLast) {
		zend_class_entry **p_ce = (zend_class_entry **) p->pData, *ce = *p_ce;
		if(ce->type == ZEND_USER_CLASS) {
			qb_scan_function_table(cxt, &ce->function_table, ce);
		} else {
			break;
		}
	}
}

static int32_t qb_compile_functions(zend_op_array *op_array TSRMLS_DC) {
	qb_build_context _build_cxt, *build_cxt = &_build_cxt;
	int32_t result = FALSE;

	// make sure the main thread is initialized
	if(QB_G(main_thread).type == QB_THREAD_UNINITIALIZED) {
		qb_initialize_main_thread(&QB_G(main_thread) TSRMLS_CC);
	}

	qb_initialize_build_context(build_cxt TSRMLS_CC);
	QB_G(build_context) = build_cxt;

	qb_scan_function_table(build_cxt, EG(function_table), NULL);
	qb_scan_class_table(build_cxt, EG(class_table));
#ifdef ZEND_ACC_CLOSURE
	if(op_array) {
		if(op_array->fn_flags & ZEND_ACC_CLOSURE) {
			qb_scan_function(build_cxt, (zend_function *) op_array, NULL);
		}
	}
#endif
	if(build_cxt->function_tag_count) {
		qb_build(build_cxt);
		result = TRUE;
	}
	qb_free_build_context(build_cxt);
	QB_G(build_context) = NULL;
	return result;
}

int qb_user_opcode_handler(ZEND_OPCODE_HANDLER_ARGS) {
	zend_op_array *op_array = EG(active_op_array);
	qb_function *qfunc = QB_GET_FUNCTION(op_array);
	if(!qfunc) {
		if(QB_IS_COMPILED(op_array)) {
			qb_compile_functions(op_array TSRMLS_CC);
			qfunc = QB_GET_FUNCTION(op_array);
		}
	}
	if(qfunc) {
		op_array->fn_flags &= ~ZEND_ACC_INTERACTIVE; 
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

int32_t qb_is_zend_optimizer_present(void) {
	static int32_t optimizer_checked = FALSE;
	static int32_t optimizer_present = FALSE;
	if(!optimizer_checked) {
		zend_extension *opcache = zend_get_extension("Zend OPcache");
		if(opcache) {
			optimizer_present = TRUE;
		}
		optimizer_checked = TRUE;
	}
	return optimizer_present;
}

void qb_disable_zend_optimizer(TSRMLS_D) {
	if(qb_is_zend_optimizer_present()) {
		char *entry_name = "opcache.optimization_level";
		char *entry_value = "0";
#if !ZEND_ENGINE_2_2 && !ZEND_ENGINE_2_1
		zend_alter_ini_entry_ex(entry_name, strlen(entry_name) + 1, entry_value, (uint) strlen(entry_value), PHP_INI_USER, PHP_INI_STAGE_RUNTIME, TRUE TSRMLS_CC);
#elif !ZEND_ENGINE_2_1
		zend_alter_ini_entry_ex(entry_name, strlen(entry_name) + 1, entry_value, (uint) strlen(entry_value), PHP_INI_USER, PHP_INI_STAGE_RUNTIME, TRUE);
#else
		zend_alter_ini_entry(entry_name, strlen(entry_name) + 1, entry_value, (uint) strlen(entry_value), PHP_INI_USER, PHP_INI_STAGE_ACTIVATE);
#endif
	}
}

void qb_reenable_zend_optimizer(TSRMLS_D) {
	if(qb_is_zend_optimizer_present()) {
		char *entry_name = "opcache.optimization_level";
		// since there isn't a "force" parameter for zend_restore_ini_entry()
		// we'll pass something other than PHP_INI_STAGE_RUNTIME so the function succeeds
		zend_restore_ini_entry(entry_name, strlen(entry_name) + 1, PHP_INI_STAGE_ACTIVATE);
	}
}

void qb_zend_ext_op_array_ctor(zend_op_array *op_array) {
	const char *doc_comment;
	TSRMLS_FETCH();
	doc_comment = CG(doc_comment);
	if(doc_comment && qb_find_engine_tag(doc_comment)) {
		zend_op *user_op;

		// add QB instruction
		user_op = &op_array->opcodes[op_array->last++];
		user_op->opcode = QB_USER_OPCODE;
		Z_OPERAND_TYPE(user_op->op1) = IS_UNUSED;
		Z_OPERAND_TYPE(user_op->op2) = IS_UNUSED;
		Z_OPERAND_TYPE(user_op->result) = IS_UNUSED;
		QB_SET_FUNCTION(op_array, NULL);

		// prevent Zend Optimizer from optimizing the opcodes
		qb_disable_zend_optimizer(TSRMLS_C);
	}
}

void qb_zend_ext_op_array_handler(zend_op_array *op_array) {
	if(QB_IS_COMPILED(op_array)) {
		TSRMLS_FETCH();
		// OpCache in Zend 2.6 and above doesn't the optimization elsewhere
		// turn on the flag to keep it from running
		op_array->fn_flags |= ZEND_ACC_INTERACTIVE; 
		qb_reenable_zend_optimizer(TSRMLS_C);
	}
}

extern zend_extension qb_zend_extension_entry;

int qb_install_user_opcode_handler() {
	// set the opcode handler
	if(zend_set_user_opcode_handler(QB_USER_OPCODE, qb_user_opcode_handler) == FAILURE) {
		return FAILURE;
	}

	// register extension
	zend_register_extension(&qb_zend_extension_entry, NULL);
	return SUCCESS;
}

void qb_uninstall_user_opcode_handler() {
	int compare(void *a, void *b) {
		return a == b;
	}

	zend_llist_del_element(&zend_extensions, &qb_zend_extension_entry, compare);
	zend_set_user_opcode_handler(QB_USER_OPCODE, NULL);
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
	PHP_QB_VERSION, /* Replace with version number for your extension */
#endif
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

extern qb_debug_interface debug_interface;

int qb_extension_startup(zend_extension *extension) {
	// ionCube loader calls this for some reason
	return 0;
}

zend_extension qb_zend_extension_entry = {
	"QB",
	PHP_QB_VERSION,
	"Chung Leong",
	"http://www.php-qb.net/",
	"Copyright (c) 2013-2014",
	qb_extension_startup,
	NULL,
	NULL,           /* activate_func_t */
	NULL,           /* deactivate_func_t */
	NULL,           /* message_handler_func_t */
	qb_zend_ext_op_array_handler,		/* op_array_handler_func_t */
	NULL,			/* statement_handler_func_t */
	NULL,           /* fcall_begin_handler_func_t */
	NULL,           /* fcall_end_handler_func_t */
	qb_zend_ext_op_array_ctor,			/* op_array_ctor_func_t */
	NULL,			/* op_array_dtor_func_t */
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
#ifdef QB_DISABLE_MULTITHREADING
		permitted_thread_count = 1;
#else
		permitted_thread_count = QB_G(thread_count);
#endif
	}
	if(QB_G(thread_count) > permitted_thread_count) {
		QB_G(thread_count) = permitted_thread_count;
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

	qb_uninstall_user_opcode_handler();

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
	QB_G(compiled_functions) = NULL;
	QB_G(compiled_function_count) = 0;
#ifdef ZEND_ACC_GENERATOR
	QB_G(generator_contexts) = NULL;
	QB_G(generator_context_count) = 0;
#endif
#ifdef NATIVE_COMPILE_ENABLED
	QB_G(native_code_bundles) = NULL;
	QB_G(native_code_bundle_count) = 0;
#endif

	if(qb_initialize_thread_pool(TSRMLS_C)) {
		qb_initialize_main_thread(&QB_G(main_thread) TSRMLS_CC);
		qb_add_workers(&QB_G(main_thread));
	}
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(qb)
{
	uint32_t i, j;

	if(QB_G(main_thread).type != QB_THREAD_UNINITIALIZED) {
		if(CG(unclean_shutdown)) {
			// bailing out of script--make sure threads are shutdown
			qb_terminate_associated_workers(&QB_G(main_thread));
		}
		qb_free_main_thread(&QB_G(main_thread));
	}

	for(i = 0; i < QB_G(scope_count); i++) {
		qb_import_scope *scope = QB_G(scopes)[i];
		// object scopes just point to the variables of the abstract scope
		if(scope->type != QB_IMPORT_SCOPE_OBJECT && scope->type != QB_IMPORT_SCOPE_FREED_OBJECT) {
			for(j = (scope->parent) ? scope->parent->variable_count : 0; j < scope->variable_count; j++) {
				qb_variable *var = scope->variables[j];
				efree(var);
			}
			efree(scope->variables);
		} else {
			if(scope->type == QB_IMPORT_SCOPE_OBJECT) {
				zval_ptr_dtor(&scope->object);
			}
		}
		
		// release the storage unless it's pointing to the parent's
		if(!scope->parent || scope->parent->storage != scope->storage) {
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

	if(QB_G(compiled_functions)) {
		// free the compiled functions
		for(i = 0; i < QB_G(compiled_function_count); i++) {
			qb_function *qfunc = QB_G(compiled_functions)[i];
			if(!(qfunc->flags & QB_FUNCTION_CLOSURE)) {
				zend_op_array *op_array = qfunc->zend_op_array;
				QB_SET_FUNCTION(op_array, NULL);
			}
			qb_free_function(qfunc);
		}
		qb_destroy_array((void **) &QB_G(compiled_functions));
	}

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

	php_info_print_table_row(2, "Version", PHP_QB_VERSION);
	php_info_print_table_row(2, "Release Name", QB_FULL_RELEASE_NAME);

	sprintf(buffer, "%u", cpu_count);
	php_info_print_table_row(2, "CPU count", buffer);

#if defined(__x86_64__)
	php_info_print_table_row(2, "CPU architecture", "x86-64");
#elif defined(__i386__)
	php_info_print_table_row(2, "CPU architecture", "x86");
#elif defined(__ARM_ARCH_7A__)
	php_info_print_table_row(2, "CPU architecture", "ARMv7-A");
#endif

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

/* Every user-visible function in PHP should document itself in the source */

/* {{{ proto bool qb_compile(void)
   Convert PHP instructions to qb instructions */
PHP_FUNCTION(qb_compile)
{
	if(qb_compile_functions(NULL TSRMLS_CC)) {
		RETURN_TRUE
	} else {
		RETURN_FALSE
	}
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

