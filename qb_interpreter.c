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
#include <math.h>
#include "zend_variables.h"

static void qb_transfer_value_from_import_source(qb_interpreter_context *cxt, qb_variable *ivar, qb_import_scope *scope) {
	if(!(ivar->flags & QB_VARIABLE_IMPORTED)) {
		USE_TSRM
		zval *zvalue = NULL, **p_zvalue = NULL;
		switch(scope->type) {
			case QB_IMPORT_SCOPE_GLOBAL: {
				// copy value from global symbol table
				if(zend_hash_quick_find(&EG(symbol_table), ivar->name, ivar->name_length + 1, ivar->hash_value, (void **) &p_zvalue) == SUCCESS) {
					SEPARATE_ZVAL_TO_MAKE_IS_REF(p_zvalue);
					zvalue = *p_zvalue;
				}
			}	break;
			case QB_IMPORT_SCOPE_CLASS: {
				if(ivar->flags & QB_VARIABLE_CLASS_CONSTANT) {
					// static:: constants are treated like variables
					zend_class_entry *ce = scope->zend_class;
					zval **p_value;
					zend_hash_quick_find(&ce->constants_table, ivar->name, ivar->name_length + 1, ivar->hash_value, (void **) &p_value);
					zvalue = *p_value;
				} else {
					zend_class_entry *ce = scope->zend_class;
					zval *name = qb_string_to_zval(ivar->name, ivar->name_length TSRMLS_CC);
					p_zvalue = Z_CLASS_GET_PROP(ce, ivar->name, ivar->name_length);
					if(p_zvalue) {
						SEPARATE_ZVAL_TO_MAKE_IS_REF(p_zvalue);
						zvalue = *p_zvalue;
					}
				}
			}	break;
			case QB_IMPORT_SCOPE_OBJECT: {
				// copy value from class instance
				zval *name = qb_string_to_zval(ivar->name, ivar->name_length TSRMLS_CC);
				zval *container = scope->zend_object;
				p_zvalue = Z_OBJ_GET_PROP_PTR_PTR(container, name);
				if(p_zvalue) {
					SEPARATE_ZVAL_TO_MAKE_IS_REF(p_zvalue);
					zvalue = *p_zvalue;
				} else if(Z_OBJ_HT_P(container)->read_property) {
					zvalue = Z_OBJ_READ_PROP(container, name);
				}
			}	break;
			default: {
			}	break;
		}
		if(zvalue) {
			qb_transfer_value_from_zval(scope->storage, ivar->address, zvalue, QB_TRANSFER_CAN_BORROW_MEMORY);
			if(!p_zvalue) {
				if(Z_REFCOUNT_P(zvalue) == 0) {
					Z_ADDREF_P(zvalue);
					zval_ptr_dtor(&zvalue);
				}
			}
		} else {
			qb_transfer_value_from_zval(scope->storage, ivar->address, &zval_used_for_init, QB_TRANSFER_CAN_BORROW_MEMORY);
		}
		ivar->value_pointer = p_zvalue;
		ivar->flags |= QB_VARIABLE_IMPORTED;
		ivar->address->flags |= QB_ADDRESS_READ_ONLY;
	}
}

static void qb_transfer_value_to_import_source(qb_interpreter_context *cxt, qb_variable *ivar, qb_import_scope *scope) {
	if(ivar->flags & QB_VARIABLE_IMPORTED) {
		USE_TSRM
		if(!READ_ONLY(ivar->address)) {
			zval *zvalue = NULL;
			if(ivar->value_pointer) {
				zvalue = *ivar->value_pointer;
			}
			if(!zvalue) {
				ALLOC_INIT_ZVAL(zvalue);
				if(ivar->value_pointer) {
					*ivar->value_pointer = zvalue;
				}
			}
			qb_transfer_value_to_zval(scope->storage, ivar->address, zvalue);

			if(!ivar->value_pointer) {
				if(ivar->flags & QB_VARIABLE_GLOBAL) {
					zend_hash_quick_update(&EG(symbol_table), ivar->name, ivar->name_length + 1, ivar->hash_value, (void **) &zvalue, sizeof(zval *), NULL);
				} else if(ivar->flags & QB_VARIABLE_CLASS_INSTANCE) {
					zval *container = scope->zend_object;
					zval *name = qb_string_to_zval(ivar->name, ivar->name_length TSRMLS_CC);
					Z_OBJ_WRITE_PROP(container, name, zvalue);
					zval_ptr_dtor(&zvalue);
				}
			}
		}
		ivar->flags &= ~QB_VARIABLE_IMPORTED;
	}
}

static void qb_transfer_variables_from_php(qb_interpreter_context *cxt, qb_function *qfunc) {
	USE_TSRM
	void **p = EG(current_execute_data)->prev_execute_data->function_state.arguments;
	uint32_t received_argument_count = (uint32_t) (zend_uintptr_t) *p;
	uint32_t i;

	// transfer arguments
	for(i = 0; i < qfunc->argument_count; i++) {
		qb_variable *qvar = qfunc->variables[i];
		zval *zarg;
		uint32_t transfer_flags = 0;

		if(i < received_argument_count) {
			zval **p_zarg = (zval**) p - received_argument_count + i;
			zarg = *p_zarg;
		} else {
			if(qvar->default_value) {
				zarg = qvar->default_value;
			} else {
				const char *space;
				const char *class_name;
				zend_execute_data *ptr = EG(current_execute_data)->prev_execute_data;

				if (EG(active_op_array)->scope) {
					class_name = EG(active_op_array)->scope->name;
					space = "::";
				} else {
					class_name = space = "";
				}

				if(ptr && ptr->op_array) {
					zend_error(E_WARNING, "Missing argument %u for %s%s%s(), called in %s on line %d and defined", i + 1, class_name, space, qfunc->name, ptr->op_array->filename, ptr->opline->lineno);
				} else {
					zend_error(E_WARNING, "Missing argument %u for %s%s%s()", i + 1, class_name, space, qfunc->name);
				}
				zarg = &zval_used_for_init;
			}
		}

		if((qvar->flags & QB_VARIABLE_BY_REF) || READ_ONLY(qvar->address)) {
			// avoid allocating new memory and copying contents if changes will be copied back anyway (or no changes will be made)
			transfer_flags = QB_TRANSFER_CAN_BORROW_MEMORY;
		}
		qb_transfer_value_from_zval(qfunc->local_storage, qvar->address, zarg, transfer_flags);
	}

	// transfer external variables
	for(i = qfunc->argument_count; i < qfunc->variable_count; i++) {
		qb_variable *qvar = qfunc->variables[i];

		if((qvar->flags & QB_VARIABLE_CLASS) || (qvar->flags & QB_VARIABLE_CLASS_INSTANCE) || (qvar->flags & QB_VARIABLE_CLASS_CONSTANT) || (qvar->flags & QB_VARIABLE_GLOBAL)) {
			zval *zobject = !(qvar->flags & QB_VARIABLE_GLOBAL) ? EG(This) : NULL;
			qb_import_scope *scope = qb_get_import_scope(cxt, qfunc->local_storage, qvar, zobject);
			qb_variable *ivar = qb_get_import_variable(cxt, qfunc->local_storage, qvar, scope);
			qb_memory_segment *local_segment, *scope_segment;

			// import the segment constaining the variable
			local_segment = &qfunc->local_storage->segments[qvar->address->segment_selector];
			scope_segment = &scope->storage->segments[ivar->address->segment_selector];
			if(local_segment->imported_segment != scope_segment) {
				qb_import_segment(local_segment, scope_segment);

				// import the scalar segment holding the size and dimensions as well
				if(ivar->address->dimension_count > 0 && !CONSTANT(ivar->address->array_size_address)) {
					local_segment = &qfunc->local_storage->segments[qvar->address->array_size_address->segment_selector];
					scope_segment = &scope->storage->segments[ivar->address->array_size_address->segment_selector];
					if(local_segment->imported_segment != scope_segment) {
						qb_import_segment(local_segment, scope_segment);
					}
				}
			}

			// transfer the value from PHP
			qb_transfer_value_from_import_source(cxt, ivar, scope);
			if(!READ_ONLY(qvar->address)) {
				ivar->address->flags &= ~QB_ADDRESS_READ_ONLY;
			}
		}
	}
}

static void qb_transfer_variables_to_php(qb_interpreter_context *cxt, qb_function *qfunc) {
	USE_TSRM
	void **p = EG(current_execute_data)->prev_execute_data->function_state.arguments;
	uint32_t received_argument_count = (uint32_t) (zend_uintptr_t) *p;
	uint32_t i, j;

	// copy value into return variable
	if(EG(return_value_ptr_ptr)) {
		zval *ret;
		ALLOC_INIT_ZVAL(ret);
		*EG(return_value_ptr_ptr) = ret;

		if(qfunc->return_variable->address) {
			qb_transfer_value_to_zval(qfunc->local_storage, qfunc->return_variable->address, ret);
		} 
	}

	// copy changes to by-ref arguments
	for(i = 0; i < qfunc->argument_count; i++) {
		qb_variable *qvar = qfunc->variables[i];

		if(qvar->flags & QB_VARIABLE_BY_REF) {
			if(i < received_argument_count) {
				zval **p_zarg = (zval**) p - received_argument_count + i;
				zval *zarg = *p_zarg;
				qb_transfer_value_to_zval(qfunc->local_storage, qvar->address, zarg);
			}
		}
	}

	// transfer imported values back to PHP
	for(i = 0; i < cxt->scope_count; i++) {
		qb_import_scope *scope = cxt->scopes[i];
		if(scope->type != QB_IMPORT_SCOPE_ABSTRACT_OBJECT) {
			for(j = 0; j < scope->variable_count; j++) {
				qb_variable *ivar = scope->variables[j];
				qb_transfer_value_to_import_source(cxt, ivar, scope);
			}
		}
	}
}

void qb_initialize_interpreter_context(qb_interpreter_context *cxt TSRMLS_DC) {
	memset(cxt, 0, sizeof(qb_interpreter_context));

	// use shadow variables for debugging purpose by default
	cxt->flags = QB_EMPLOY_SHADOW_VARIABLES;

	cxt->floating_point_precision = EG(precision);
	cxt->line_number_pointer = &QB_G(current_line_number);
#ifdef ZEND_WIN32
	cxt->windows_timed_out_pointer = &EG(timed_out);
#endif
	SAVE_TSRMLS
}

void qb_free_interpreter_context(qb_interpreter_context *cxt) {
	uint32_t i, j;
	for(i = 0; i < cxt->scope_count; i++) {
		qb_import_scope *scope = cxt->scopes[i];
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
				if(segment->memory) {
					efree(segment->memory);
				}
			}
			efree(scope->storage->segments);
			efree(scope->storage);
		}
		efree(scope);
	}
	if(cxt->scopes) {
		efree(cxt->scopes);
	}
}

void qb_main(qb_interpreter_context *__restrict cxt, qb_function *__restrict function);

static zend_always_inline void qb_enter_vm(qb_interpreter_context *cxt, qb_function *qfunc) {
#ifdef NATIVE_COMPILE_ENABLED
	if(qfunc->native_proc) {
		qb_native_proc proc = qfunc->native_proc;
		if(proc(cxt) == FAILURE) {
			USE_TSRM
			if(QB_G(allow_bytecode_interpretation)) {
				qb_main(cxt, qfunc);
			} else {
				qb_abort("Unable to run compiled procedure");
			}
		}
	} else {
		qb_main(cxt, qfunc);
	}
#else
	qb_main(cxt, qfunc);
#endif
}

static void qb_initialize_static_segments(qb_interpreter_context *cxt, qb_function *qfunc) {
	int8_t *memory;
	uint32_t combined_byte_count;

	// the following optimization depends very much on how the segments are laid out
	memory = qfunc->local_storage->segments[QB_SELECTOR_SHARED_SCALAR].memory;
	combined_byte_count = qfunc->local_storage->segments[QB_SELECTOR_SHARED_SCALAR].byte_count + qfunc->local_storage->segments[QB_SELECTOR_LOCAL_SCALAR].byte_count;
	memset(memory, 0, combined_byte_count);

	memory = qfunc->local_storage->segments[QB_SELECTOR_LOCAL_ARRAY].memory;
	combined_byte_count = qfunc->local_storage->segments[QB_SELECTOR_LOCAL_ARRAY].byte_count + qfunc->local_storage->segments[QB_SELECTOR_SHARED_ARRAY].byte_count;
	memset(memory, 0, combined_byte_count);
}

static void qb_initialize_dynamically_allocated_segments(qb_interpreter_context *cxt, qb_function *qfunc) {
	uint32_t i;
	for(i = QB_SELECTOR_ARRAY_START; i < qfunc->local_storage->segment_count; i++) {
		qb_memory_segment *segment = &qfunc->local_storage->segments[i];
		if(!segment->memory && segment->byte_count > 0) {
			qb_allocate_segment_memory(segment, segment->byte_count);
			if(segment->flags & QB_SEGMENT_CLEAR_ON_CALL) {
				memset(segment->memory, 0, segment->current_allocation);
			}
		}
	}
}

static void qb_free_dynamically_allocated_segments(qb_interpreter_context *cxt, qb_function *qfunc) {
	uint32_t i;
	for(i = QB_SELECTOR_ARRAY_START; i < qfunc->local_storage->segment_count; i++) {
		qb_memory_segment *segment = &qfunc->local_storage->segments[i];
		if(segment->flags & QB_SEGMENT_FREE_ON_RETURN) {
			qb_release_segment(segment);
			if(segment->flags & QB_SEGMENT_EMPTY_ON_RETURN) {
				segment->byte_count = 0;
			}
		}
	}
}

void qb_run_zend_extension_op(qb_interpreter_context *cxt, uint32_t zend_opcode, uint32_t line_number) {
}

void qb_execute(qb_interpreter_context *cxt, qb_function *qfunc) {
	int result = SUCCESS;

	// clear static memory segments
	qb_initialize_static_segments(cxt, qfunc);

	// copy values from arguments, class variables, object variables, and global variables
	qb_transfer_variables_from_php(cxt, qfunc);

	// initialize local dynamically allocated local variables 
	qb_initialize_dynamically_allocated_segments(cxt, qfunc);

	// enter the vm
	qb_enter_vm(cxt, qfunc);

	// move values back into PHP space
	qb_transfer_variables_to_php(cxt, qfunc);

	// release dynamically allocated segments
	qb_free_dynamically_allocated_segments(cxt, qfunc);
}

void qb_execute_internal(qb_function *qfunc TSRMLS_DC) {
	qb_interpreter_context _cxt, *cxt = &_cxt;
	unsigned char windows_time_out = 0;
	SAVE_TSRMLS
	cxt->windows_timed_out_pointer = &windows_time_out;
	qb_enter_vm(cxt, qfunc);
}

void qb_dispatch_instruction_to_threads(qb_interpreter_context *cxt, void *control_func, int8_t **instruction_pointers) {
	USE_TSRM
	uint32_t i;
	uint32_t count = cxt->thread_count_for_next_op;
	cxt->thread_count_for_next_op = 0;

	if(!QB_G(thread_pool)) {
		//qb_initialize_thread_pool(cxt->thread_pool);
	}
	for(i = 0; i < count; i++)  {
		int8_t *ip = instruction_pointers[i];
		//qb_schedule_task(cxt->thread_pool, control_func, cxt, ip);
	}
	//qb_run_tasks(cxt->thread_pool);
}

qb_import_scope * qb_find_import_scope(qb_interpreter_context *cxt, qb_import_scope_type type, void *associated_object) {
	uint32_t i;
	qb_import_scope *scope;
	for(i = 0; i < cxt->scope_count; i++) {
		scope = cxt->scopes[i];
		if(scope->type == type && scope->associated_object == associated_object) {
			return scope;
		}
	}
	return NULL;
}

qb_import_scope * qb_create_import_scope(qb_interpreter_context *cxt, qb_import_scope_type type, void *associated_object) {
	USE_TSRM
	qb_import_scope *scope = emalloc(sizeof(qb_import_scope));
	memset(scope, 0, sizeof(qb_import_scope));
	scope->type = type;
	scope->associated_object = associated_object;
	cxt->scopes = erealloc(cxt->scopes, sizeof(qb_import_scope *) * (cxt->scope_count + 1));
	cxt->scopes[cxt->scope_count++] = scope;

	if(type == QB_IMPORT_SCOPE_OBJECT) {
		// create the scope based on the scope of the abstract object
		zval *object = associated_object;
		zend_class_entry *ce = Z_OBJCE_P(object);
		qb_import_scope *abstract_scope = qb_find_import_scope(cxt, QB_IMPORT_SCOPE_ABSTRACT_OBJECT, ce);

		if(abstract_scope) {
			scope->variables = abstract_scope->variables;
			scope->variable_count = abstract_scope->variable_count;
			scope->storage = emalloc(sizeof(qb_storage));
			scope->storage->flags = abstract_scope->storage->flags;
			scope->storage->segment_count = abstract_scope->storage->segment_count;
			scope->storage->segments = emalloc(sizeof(qb_memory_segment) * abstract_scope->storage->segment_count);
			memcpy(scope->storage->segments, abstract_scope->storage->segments, sizeof(qb_memory_segment) * abstract_scope->storage->segment_count);
		}
	} else if(type == QB_IMPORT_SCOPE_CLASS || type == QB_IMPORT_SCOPE_ABSTRACT_OBJECT) {
		zend_class_entry *ce = associated_object;
		zend_class_entry *ancestor_ce;
		for(ancestor_ce = ce->parent; ancestor_ce; ancestor_ce = ancestor_ce->parent) {
			qb_import_scope *ancestor_scope = qb_find_import_scope(cxt, type, ancestor_ce);

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
		index_count = segment->byte_count / sizeof(uint32_t), i;
		for(i = 0; i < index_count; i++) {
			if(indices[i] == index) {
				dst_address->segment_offset = i * sizeof(uint32_t);
				return;
			}
		}
	} else {
		segment = &dst_storage->segments[variable_selector];
		dst_address->segment_selector = variable_selector;
		index = (uint32_t) -1;
	}
	dst_address->segment_offset = segment->byte_count;
	segment->byte_count += sizeof(uint32_t);
	if(segment->byte_count > segment->current_allocation) {
		segment->current_allocation = ALIGN_TO(segment->byte_count, 1024);
		segment->memory = erealloc(segment->memory, segment->current_allocation);
	}
	VALUE_IN(dst_storage, U32, dst_address) = index;
}

qb_variable * qb_import_variable(qb_interpreter_context *cxt, qb_storage *storage, qb_variable *var, qb_import_scope *scope) {
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

qb_import_scope * qb_get_import_scope(qb_interpreter_context *cxt, qb_storage *storage, qb_variable *var, zval *object) {
	USE_TSRM
	qb_import_scope *scope;
	qb_import_scope_type scope_type;
	void *associated_object;

	if(var->flags & QB_VARIABLE_GLOBAL) {
		scope_type = QB_IMPORT_SCOPE_GLOBAL;
		associated_object = NULL;
	} else if(var->flags & QB_VARIABLE_CLASS || var->flags & QB_VARIABLE_CLASS_CONSTANT) {
		scope_type = QB_IMPORT_SCOPE_CLASS;
		if(var->zend_class) {
			associated_object = var->zend_class;
		} else {
			USE_TSRM
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
	}
	scope = qb_find_import_scope(cxt, scope_type, associated_object);
	if(!scope) {
		scope = qb_create_import_scope(cxt, scope_type, associated_object);
	}
	return scope;
}

qb_variable * qb_get_import_variable(qb_interpreter_context *cxt, qb_storage *storage, qb_variable *var, qb_import_scope *scope) {
	uint32_t i;
	qb_variable *ivar;
	// look for the variable
	for(i = 0; i < scope->variable_count; i++) {
		ivar = scope->variables[i];
		if(ivar->hash_value == var->hash_value && ivar->name_length == var->name_length) {
			if(strcmp(ivar->name, var->name) == 0) {
				int32_t match = TRUE;
				
				if(qb_check_address_compatibility(scope->storage, ivar->address, storage, var->address)) {
					if(!(var->flags & QB_ADDRESS_READ_ONLY)) {
						ivar->flags &= ~QB_ADDRESS_READ_ONLY;
					}
					return ivar;
				} else {
					if(READ_ONLY(ivar->address) && READ_ONLY(var->address)) {
						// permit a variable to be imported differently if it's not modified 
					} else {
						qb_abort("Error message");
					}
				}
			}
		}
	}
	ivar = qb_import_variable(cxt, storage, var, scope);
	return ivar;
}

intptr_t qb_adjust_memory_segment(qb_interpreter_context *cxt, qb_storage *storage, uint32_t segment_selector, uint32_t new_size) {
	qb_memory_segment *segment = &storage->segments[segment_selector];
#ifdef ZEND_DEBUG
	if(segment->flags & QB_SEGMENT_PREALLOCATED) {
		qb_abort("Invalid segment selector");
	}
#endif
	// TODO: this needs to happen in the main thread
	return qb_resize_segment(segment, new_size);
}

void qb_push_zend_argument(qb_interpreter_context *cxt, qb_storage *storage, qb_variable *var, qb_zend_argument_stack *stack) {
	zval ***pp_zarg, **p_zarg, *zarg;
	if(!stack->arguments) {
		qb_create_array((void **) &stack->arguments, &stack->argument_count, sizeof(zval *), 4);
		qb_create_array((void **) &stack->argument_pointers, &stack->argument_pointer_count, sizeof(zval **), 4);
	}
	ALLOC_INIT_ZVAL(zarg);
	qb_transfer_value_to_zval(storage, var->address, zarg);
	p_zarg = qb_enlarge_array((void **) &stack->arguments, 1);
	pp_zarg = qb_enlarge_array((void **) &stack->argument_pointers, 1);
	*pp_zarg = p_zarg;
	*p_zarg = zarg;
}

void qb_free_zend_argument_stack(qb_interpreter_context *cxt, qb_zend_argument_stack *stack) {
	if(stack->arguments) {
		qb_destroy_array((void **) &stack->arguments);
		qb_destroy_array((void **) &stack->argument_pointers);
	}
}

void qb_execute_zend_function_call(qb_interpreter_context *cxt, qb_storage *storage, qb_variable *retvar, zend_function *zfunc, qb_zend_argument_stack *stack, uint32_t line_number) {
	USE_TSRM
	zval *retval;
	zend_op **p_user_op = EG(opline_ptr);
	zend_fcall_info fci;
	zend_fcall_info_cache fcc;
	uint32_t i;
	int call_result;

	// copy global and class variables back to PHP first
	// TODO

	fcc.calling_scope = EG(called_scope);
	fcc.function_handler = zfunc;
	fcc.initialized = 1;
	fci.size = sizeof(zend_fcall_info);
#if !ZEND_ENGINE_2_2 && !ZEND_ENGINE_2_1
	if(zfunc->common.scope) {
		fci.function_table = &zfunc->common.scope->function_table;
		if((zfunc->common.fn_flags & ZEND_ACC_STATIC)) {
			fci.object_ptr = fcc.object_ptr = NULL;
		} else {
			fci.object_ptr = fcc.object_ptr = EG(This);
		}
		fcc.called_scope = zfunc->common.scope;
	} else {
		fci.function_table = EG(function_table);
		fci.object_ptr = fcc.object_ptr = NULL;
	}
#else 
	if(zfunc->common.scope) {
		fci.function_table = &zfunc->common.scope->function_table;
		if((zfunc->common.fn_flags & ZEND_ACC_STATIC)) {
			fci.object_pp = fcc.object_pp = NULL;
		} else {
			fci.object_pp = fcc.object_pp = EG(This);
		}
	} else {
		fci.function_table = EG(function_table);
		fci.object_pp = fcc.object_pp = NULL;
	}
#endif
	fci.function_name = qb_cstring_to_zval(zfunc->common.function_name TSRMLS_CC);
	fci.retval_ptr_ptr = &retval;
	fci.param_count = stack->argument_count;
	fci.params = stack->argument_pointers;
	fci.no_separation = 1;
	fci.symbol_table = NULL;

	// set the qb user op's line number to the line number where the call occurs
	// so that debug_backtrace() will get the right number
	(*p_user_op)->lineno = line_number;
	call_result = zend_call_function(&fci, &fcc TSRMLS_CC);
	(*p_user_op)->lineno = 0;

	if(call_result == SUCCESS) {
		if(EG(exception)) {
			cxt->exception_encountered = TRUE;
		}
	} else {
		qb_abort("Internal error");
	}

	// copy values of global and class variables from PHP again
	// TODO

	if(retvar) {
		qb_transfer_value_from_zval(storage, retvar->address, retval, QB_TRANSFER_CAN_SEIZE_MEMORY);
	}
	for(i = 0; i < stack->argument_count; i++) {
		zval *zarg = stack->arguments[i];
		zval_ptr_dtor(&zarg);
	}
	stack->argument_count = 0;
	stack->argument_pointer_count = 0;
	zval_ptr_dtor(&retval);
}