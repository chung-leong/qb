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

static void qb_transfer_variables_from_php(qb_interpreter_context *cxt) {
	USE_TSRM
	qb_function *qfunc = cxt->active_function;
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
			qb_import_scope *scope = qb_get_import_scope(qfunc->local_storage, qvar, zobject TSRMLS_CC);
			qb_variable *ivar = qb_get_import_variable(qfunc->local_storage, qvar, scope TSRMLS_CC);
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

static void qb_transfer_variables_to_php(qb_interpreter_context *cxt) {
	USE_TSRM
	qb_function *qfunc = cxt->active_function;
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
	for(i = 0; i < QB_G(scope_count); i++) {
		qb_import_scope *scope = QB_G(scopes)[i];
		if(scope->type != QB_IMPORT_SCOPE_ABSTRACT_OBJECT) {
			for(j = 0; j < scope->variable_count; j++) {
				qb_variable *ivar = scope->variables[j];
				qb_transfer_value_to_import_source(cxt, ivar, scope);
			}
		}
	}
}

void qb_initialize_interpreter_context(qb_interpreter_context *cxt, qb_function *qfunc TSRMLS_DC) {
	memset(cxt, 0, sizeof(qb_interpreter_context));

	cxt->active_function = qfunc;
	cxt->next_handler = *((void **) qfunc->instruction_start);
	cxt->instruction_pointer = qfunc->instruction_start + sizeof(void *);

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
}

void qb_main(qb_interpreter_context *__restrict cxt);

static zend_always_inline void qb_enter_vm(qb_interpreter_context *cxt) {
#ifdef NATIVE_COMPILE_ENABLED
	if(qfunc->native_proc) {
		qb_native_proc proc = qfunc->native_proc;
		if(proc(cxt) == FAILURE) {
			USE_TSRM
			if(QB_G(allow_bytecode_interpretation)) {
				qb_main(cxt);
			} else {
				qb_abort("Unable to run compiled procedure");
			}
		}
	} else {
		qb_main(cxt);
	}
#else
	qb_main(cxt);
#endif
}

static void qb_initialize_static_segments(qb_interpreter_context *cxt) {
	qb_function *qfunc = cxt->active_function;
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

static void qb_initialize_dynamically_allocated_segments(qb_interpreter_context *cxt) {
	qb_function *qfunc = cxt->active_function;
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

static void qb_free_dynamically_allocated_segments(qb_interpreter_context *cxt) {
	qb_function *qfunc = cxt->active_function;
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

void qb_execute(qb_interpreter_context *cxt) {
	int result = SUCCESS;

	// clear static memory segments
	qb_initialize_static_segments(cxt);

	// copy values from arguments, class variables, object variables, and global variables
	qb_transfer_variables_from_php(cxt);

	// initialize local dynamically allocated local variables 
	qb_initialize_dynamically_allocated_segments(cxt);

	// enter the vm
	qb_enter_vm(cxt);

	// move values back into PHP space
	qb_transfer_variables_to_php(cxt);

	// release dynamically allocated segments
	qb_free_dynamically_allocated_segments(cxt);
}

void qb_execute_internal(qb_interpreter_context *cxt) {
	qb_enter_vm(cxt);
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

intptr_t qb_adjust_memory_segment(qb_interpreter_context *cxt, uint32_t segment_selector, uint32_t new_size) {
	qb_memory_segment *segment = &cxt->active_function->local_storage->segments[segment_selector];
#ifdef ZEND_DEBUG
	if(segment->flags & QB_SEGMENT_PREALLOCATED) {
		qb_abort("Invalid segment selector");
	}
#endif
	// TODO: this needs to happen in the main thread
	return qb_resize_segment(segment, new_size);
}

static zval * qb_invoke_zend_function(qb_interpreter_context *cxt, zend_function *zfunc, zval ***argument_pointers, uint32_t argument_count, uint32_t line_number) {
	USE_TSRM
	zval *retval;
	zend_op **p_user_op = EG(opline_ptr);
	zend_fcall_info fci;
	zend_fcall_info_cache fcc;
	uint32_t i;
	int call_result;

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
	fci.param_count = argument_count;
	fci.params = argument_pointers;
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
	return retval;
}

static void qb_execute_zend_function_call(qb_interpreter_context *cxt, zend_function *zfunc, uint32_t *variable_indices, uint32_t argument_count, uint32_t result_index, uint32_t line_number) {
	zval **arguments, ***argument_pointers, *retval;
	uint32_t i;
	ALLOCA_FLAG(use_heap1)
	ALLOCA_FLAG(use_heap2)

	arguments = do_alloca(sizeof(zval *) * argument_count, use_heap1);
	argument_pointers = do_alloca(sizeof(zval **) * argument_count, use_heap2);

	for(i = 0; i < argument_count; i++) {
		uint32_t var_index = variable_indices[i];
		qb_variable *var = cxt->active_function->variables[var_index];
		ALLOC_INIT_ZVAL(arguments[i]);
		qb_transfer_value_to_zval(cxt->active_function->local_storage, var->address, arguments[i]);
		argument_pointers[i] = &arguments[i];
	}

	// copy values of global and class variables back to PHP
	// TODO

	retval = qb_invoke_zend_function(cxt, zfunc, argument_pointers, argument_count, line_number);

	// copy values of global and class variables from PHP again
	// TODO

	if(result_index != -1) {
		qb_variable *retvar = cxt->active_function->variables[result_index];
		qb_transfer_value_from_zval(cxt->active_function->local_storage, retvar->address, retval, QB_TRANSFER_CAN_SEIZE_MEMORY);
	}
	for(i = 0; i < argument_count; i++) {
		zval_ptr_dtor(&arguments[i]);
	}
	zval_ptr_dtor(&retval);

	free_alloca(arguments, use_heap1);
	free_alloca(argument_pointers, use_heap2);
}

static void qb_execute_function_call(qb_interpreter_context *cxt, qb_function *qfunc, uint32_t *variable_indices, uint32_t argument_count, uint32_t result_index, uint32_t line_number) {

}

void qb_dispatch_function_call(qb_interpreter_context *cxt, uint32_t symbol_index, uint32_t *variable_indices, uint32_t argument_count, uint32_t result_index, uint32_t line_number) {
	USE_TSRM
	qb_external_symbol *symbol = &QB_G(external_symbols)[symbol_index];
	zend_function *zfunc = symbol->pointer;
	qb_function *qfunc;

	if(symbol->type == QB_EXT_SYM_STATIC_ZEND_FUNCTION) {
		zend_class_entry *called_scope = EG(called_scope);
		if(zfunc->common.scope != called_scope) {
			zend_hash_find(&called_scope->function_table, zfunc->common.function_name, strlen(zfunc->common.function_name) + 1, (void **) &zfunc);
		}
	}

	qfunc = qb_get_compiled_function(zfunc);
	if(qfunc) {
		qb_execute_function_call(cxt, qfunc, variable_indices, argument_count, result_index, line_number);
	} else {
		qb_execute_zend_function_call(cxt, zfunc, variable_indices, argument_count, result_index, line_number);
	}
}