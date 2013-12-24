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
		if(ivar->value_pointer) {
			p_zvalue = ivar->value_pointer;
		} else {
			switch(scope->type) {
				case QB_IMPORT_SCOPE_GLOBAL: {
					// copy value from global symbol table
					if(zend_hash_quick_find(&EG(symbol_table), ivar->name, ivar->name_length + 1, ivar->hash_value, (void **) &p_zvalue) == SUCCESS) {
						SEPARATE_ZVAL_TO_MAKE_IS_REF(p_zvalue);
					}
				}	break;
				case QB_IMPORT_SCOPE_CLASS: {
					if(ivar->flags & QB_VARIABLE_CLASS_CONSTANT) {
						// static:: constants are treated like variables
						zend_class_entry *ce = scope->zend_class;
						zval **p_value;
						zend_hash_quick_find(&ce->constants_table, ivar->name, ivar->name_length + 1, ivar->hash_value, (void **) &p_value);
					} else {
						zend_class_entry *ce = scope->zend_class;
						p_zvalue = Z_CLASS_GET_PROP(ce, ivar->name, ivar->name_length);
						if(p_zvalue) {
							SEPARATE_ZVAL_TO_MAKE_IS_REF(p_zvalue);
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
					} else if(Z_OBJ_HT_P(container)->read_property) {
						zvalue = Z_OBJ_READ_PROP(container, name);
					}
				}	break;
				default: {
				}	break;
			}
		}
		if(p_zvalue) {
			zvalue = *p_zvalue;
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

static void qb_transfer_arguments_from_caller(qb_interpreter_context *cxt) {
	uint32_t received_argument_count = cxt->caller_context->argument_count;
	uint32_t i;
	for(i = 0; i < cxt->function->argument_count; i++) {
		qb_variable *qvar = cxt->function->variables[i];

		if(i < received_argument_count) {
			uint32_t argument_index = cxt->caller_context->argument_indices[i];
			qb_variable *caller_qvar = cxt->caller_context->function->variables[argument_index];
			qb_storage *caller_storage = cxt->caller_context->function->local_storage;
			uint32_t transfer_flags = 0;
			if((qvar->flags & QB_VARIABLE_BY_REF) || READ_ONLY(qvar->address)) {
				transfer_flags = QB_TRANSFER_CAN_BORROW_MEMORY;
			}
			qb_transfer_value_from_storage_location(cxt->function->local_storage, qvar->address, caller_storage, caller_qvar->address, transfer_flags);
		} else {
			USE_TSRM
			if(qvar->default_value) {
				zval *zarg = qvar->default_value;
				uint32_t transfer_flags = 0;
				if(READ_ONLY(qvar->address)) {
					transfer_flags = QB_TRANSFER_CAN_BORROW_MEMORY;
				}
				qb_transfer_value_from_zval(cxt->function->local_storage, qvar->address, zarg, transfer_flags);
			} else {
				const char *class_name = (EG(active_op_array)->scope) ? EG(active_op_array)->scope->name : NULL;
				qb_report_missing_argument_exception(cxt->thread, cxt->function->line_id, class_name, cxt->function->name, i, cxt->caller_line_id);
			}
		}
	}
}

static void qb_transfer_arguments_from_php(qb_interpreter_context *cxt) {
	USE_TSRM
	void **p = EG(current_execute_data)->prev_execute_data->function_state.arguments;
	uint32_t received_argument_count = (uint32_t) (zend_uintptr_t) *p;
	uint32_t i;

	for(i = 0; i < cxt->function->argument_count; i++) {
		qb_variable *qvar = cxt->function->variables[i];

		if(i < received_argument_count) {
			zval **p_zarg = (zval**) p - received_argument_count + i;
			zval *zarg = *p_zarg;
			uint32_t transfer_flags = 0;
			if((qvar->flags & QB_VARIABLE_BY_REF) || READ_ONLY(qvar->address)) {
				// avoid allocating new memory and copying contents if changes will be copied back anyway (or no changes will be made)
				transfer_flags = QB_TRANSFER_CAN_BORROW_MEMORY;
			}
			qb_transfer_value_from_zval(cxt->function->local_storage, qvar->address, zarg, transfer_flags);
		} else {
			if(qvar->default_value) {
				zval *zarg = qvar->default_value;
				uint32_t transfer_flags = 0;
				if(READ_ONLY(qvar->address)) {
					transfer_flags = QB_TRANSFER_CAN_BORROW_MEMORY;
				}
				qb_transfer_value_from_zval(cxt->function->local_storage, qvar->address, zarg, transfer_flags);
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
					zend_error(E_WARNING, "Missing argument %u for %s%s%s(), called in %s on line %d and defined", i + 1, class_name, space, cxt->function->name, ptr->op_array->filename, ptr->opline->lineno);
				} else {
					zend_error(E_WARNING, "Missing argument %u for %s%s%s()", i + 1, class_name, space, cxt->function->name);
				}
			}
		}
	}
}

static void qb_transfer_variables_from_php(qb_interpreter_context *cxt) {
	USE_TSRM
	uint32_t i;
	for(i = cxt->function->argument_count; i < cxt->function->variable_count; i++) {
		qb_variable *qvar = cxt->function->variables[i];

		if(qvar->flags & (QB_VARIABLE_CLASS_INSTANCE | QB_VARIABLE_CLASS | QB_VARIABLE_CLASS_CONSTANT | QB_VARIABLE_GLOBAL)) {
			zval *zobject = !(qvar->flags & QB_VARIABLE_GLOBAL) ? EG(This) : NULL;
			qb_import_scope *scope = qb_get_import_scope(cxt->function->local_storage, qvar, zobject TSRMLS_CC);
			qb_variable *ivar = qb_get_import_variable(cxt->function->local_storage, qvar, scope TSRMLS_CC);
			qb_memory_segment *local_segment, *scope_segment;

			// import the segment constaining the variable
			local_segment = &cxt->function->local_storage->segments[qvar->address->segment_selector];
			scope_segment = &scope->storage->segments[ivar->address->segment_selector];
			if(local_segment->imported_segment != scope_segment) {
				qb_import_segment(local_segment, scope_segment);

				// import the scalar segment holding the size and dimensions as well
				if(ivar->address->dimension_count > 0 && !CONSTANT(ivar->address->array_size_address)) {
					local_segment = &cxt->function->local_storage->segments[qvar->address->array_size_address->segment_selector];
					scope_segment = &scope->storage->segments[ivar->address->array_size_address->segment_selector];
					if(local_segment->imported_segment != scope_segment) {
						qb_import_segment(local_segment, scope_segment);
					}
				}
			}
			if(READ_ONLY(qvar->address) && !(ivar->flags & QB_VARIABLE_IMPORTED)) {
				// the mark it as read-only
				ivar->address->flags |= QB_ADDRESS_READ_ONLY;
			}
			// transfer the value from PHP
			qb_transfer_value_from_import_source(cxt, ivar, scope);
			if(!READ_ONLY(qvar->address)) {
				ivar->address->flags &= ~QB_ADDRESS_READ_ONLY;
			}
		}
	}
}

static void qb_transfer_variables_from_external_sources(qb_interpreter_context *cxt) {
	if(cxt->caller_context) {
		qb_transfer_arguments_from_caller(cxt);
	} else {
		qb_transfer_arguments_from_php(cxt);
	}
	qb_transfer_variables_from_php(cxt);
}

static void qb_transfer_arguments_to_php(qb_interpreter_context *cxt) {
	USE_TSRM
	void **p = EG(current_execute_data)->prev_execute_data->function_state.arguments;
	uint32_t received_argument_count = (uint32_t) (zend_uintptr_t) *p;
	uint32_t i;

	// copy changes to by-ref arguments
	for(i = 0; i < cxt->function->argument_count; i++) {
		qb_variable *qvar = cxt->function->variables[i];

		if(qvar->flags & QB_VARIABLE_BY_REF) {
			if(i < received_argument_count) {
				zval **p_zarg = (zval**) p - received_argument_count + i;
				zval *zarg = *p_zarg;
				qb_transfer_value_to_zval(cxt->function->local_storage, qvar->address, zarg);
			}
		}
	}

	if(EG(return_value_ptr_ptr)) {
		// copy value into return variable
		zval *ret;
		ALLOC_INIT_ZVAL(ret);
		*EG(return_value_ptr_ptr) = ret;
		if(cxt->function->return_variable->address) {
			qb_transfer_value_to_zval(cxt->function->local_storage, cxt->function->return_variable->address, ret);
		}
	}
}

#ifdef ZEND_ACC_GENERATOR
static void qb_transfer_arguments_from_generator(qb_interpreter_context *cxt) {
	USE_TSRM
	zend_generator *generator = (zend_generator *) EG(return_value_ptr_ptr);

	if(cxt->function->sent_variable->address) {
		if(generator->send_target) {
			zval *value = &generator->send_target->tmp_var;
			qb_transfer_value_from_zval(cxt->function->local_storage, cxt->function->sent_variable->address, value, QB_TRANSFER_CAN_BORROW_MEMORY);
		}
	}
}

static void qb_transfer_variables_to_generator(qb_interpreter_context *cxt) {
	USE_TSRM
	zend_generator *generator = (zend_generator *) EG(return_value_ptr_ptr);
	zval *ret, *ret_key;

	// destroy previous key and value
	if(generator->value) {
		zval_ptr_dtor(&generator->value);
	}
	if(generator->key) {
		zval_ptr_dtor(&generator->key);
	}

	ALLOC_INIT_ZVAL(ret);
	ALLOC_INIT_ZVAL(ret_key);
	generator->value = ret;
	generator->key = ret_key;

	if(cxt->function->return_variable->address) {
		qb_transfer_value_to_zval(cxt->function->local_storage, cxt->function->return_variable->address, ret);
	}
	if(cxt->function->return_key_variable->address) {
		qb_transfer_value_to_zval(cxt->function->local_storage, cxt->function->return_key_variable->address, ret_key);
	}
	if(cxt->function->sent_variable->address) {
		if(!generator->send_target) {
			cxt->send_target = emalloc(sizeof(temp_variable));
			memset(cxt->send_target, 0, sizeof(temp_variable));
			generator->send_target = cxt->send_target;
		}
	}
}
#endif

static void qb_refresh_imported_variables(qb_interpreter_context *cxt) {
	USE_TSRM
	uint32_t i, j;
	for(i = 0; i < QB_G(scope_count); i++) {
		qb_import_scope *scope = QB_G(scopes)[i];
		if(scope->type != QB_IMPORT_SCOPE_ABSTRACT_OBJECT) {
			for(j = 0; j < scope->variable_count; j++) {
				qb_variable *ivar = scope->variables[j];
				qb_transfer_value_from_import_source(cxt, ivar, scope);
			}
		}
	}
}

static void qb_sync_imported_variables(qb_interpreter_context *cxt) {
	USE_TSRM
	uint32_t i, j;
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

static void qb_transfer_variables_to_php(qb_interpreter_context *cxt) {
	qb_sync_imported_variables(cxt);
}

static void qb_transfer_arguments_to_caller(qb_interpreter_context *cxt) {
	uint32_t received_argument_count = cxt->caller_context->argument_count;
	uint32_t retval_index = cxt->caller_context->result_index;
	uint32_t i;
	for(i = 0; i < cxt->function->argument_count; i++) {
		qb_variable *qvar = cxt->function->variables[i];

		if(i < received_argument_count) {
			uint32_t argument_index = cxt->caller_context->argument_indices[i];
			qb_variable *caller_qvar = cxt->caller_context->function->variables[argument_index];
			qb_storage *caller_storage = cxt->caller_context->function->local_storage;
			if(qvar->flags & QB_VARIABLE_BY_REF) {
				qb_transfer_value_to_storage_location(cxt->function->local_storage, qvar->address, caller_storage, caller_qvar->address);
			}
		}
	}

	if(retval_index != INVALID_INDEX) {
		if(cxt->function->return_variable->address) {
			qb_variable *qvar = cxt->function->return_variable;
			qb_variable *caller_qvar = cxt->caller_context->function->variables[retval_index];
			qb_storage *caller_storage = cxt->caller_context->function->local_storage;
			qb_transfer_value_to_storage_location(cxt->function->local_storage, qvar->address, caller_storage, caller_qvar->address);
		}
	}
}

static void qb_transfer_variables_to_external_sources(qb_interpreter_context *cxt) {
	USE_TSRM

	if(cxt->caller_context) {
		qb_transfer_arguments_to_caller(cxt);
	} else {
		qb_transfer_arguments_to_php(cxt);
		qb_transfer_variables_to_php(cxt);
	}
}

static int32_t qb_lock_function(qb_function *f) {
#if defined(_WIN32)
	if(InterlockedIncrement(&f->in_use) == 1) {
		return TRUE;
	}
#elif defined(__GNUC__)
	if(__sync_add_and_fetch(&f->in_use, 1) == 1) {
		return TRUE;
	}
#endif
	f->in_use = 1;
	return FALSE;
}

static void qb_unlock_function(qb_function *f) {
	f->local_storage->current_owner = NULL;
	f->in_use = 0;
}

static void qb_create_function_copy_in_main_thread(void *param1, void *param2, int param3) {
	qb_function *last = param1, *f, **p_new = param2;
	int32_t reentrance = param3;
	if(reentrance) {
		while(last->next_reentrance_copy) {
			last = last->next_reentrance_copy;
		}
	} else {
		while(last->next_forked_copy) {
			last = last->next_forked_copy;
		}
	}
	f = qb_create_function_copy(last, reentrance);
	if(reentrance) {
		last->next_reentrance_copy = f;
	} else {
		last->next_forked_copy = f;
	}
	*p_new = f;
}

static qb_function * qb_acquire_function(qb_interpreter_context *cxt, qb_function *base, int32_t reentrance) {
	qb_function *f, *last;
	if(reentrance) {
		for(f = base; f; f = f->next_reentrance_copy) {
			if(!f->in_use && qb_lock_function(f)) {
				break;
			}
			last = f;
		}
	} else {
		for(f = base; f; f = f->next_forked_copy) {
			if(!f->in_use && qb_lock_function(f)) {
				break;
			}
			last = f;
		}
	}

	if(!f) {
		// need to create a copy
		if(cxt->thread->type == QB_THREAD_WORKER) {
			// do it in the main thread
			qb_run_in_main_thread(cxt->thread, qb_create_function_copy_in_main_thread, last, &f, reentrance, NULL);
		} else {
			f = qb_create_function_copy(last, reentrance);
			if(reentrance) {
				last->next_reentrance_copy = f;
			} else {
				last->next_forked_copy = f;
			}
		}
		qb_lock_function(f);
	}
	f->local_storage->current_owner = cxt->thread;
	return f;
}

void qb_initialize_interpreter_context(qb_interpreter_context *cxt, qb_function *qfunc, qb_interpreter_context *caller_cxt TSRMLS_DC) {
	if(caller_cxt) {
		cxt->call_depth = caller_cxt->call_depth + 1;
		cxt->thread = caller_cxt->thread;
		cxt->caller_context = NULL;
	} else {
		cxt->call_depth = 1;
		cxt->thread = (qb_thread *) qb_get_main_thread(TSRMLS_C);
		cxt->caller_context = caller_cxt;
	}
	cxt->function = qb_acquire_function(cxt, qfunc, TRUE);
	cxt->instruction_pointer = cxt->function->instruction_start;

	cxt->thread_count = qb_get_thread_count(TSRMLS_C);
	if(cxt->thread_count == 1) {
		cxt->thread_count = 0;
	}
	cxt->exit_type = QB_VM_RETURN;
	cxt->exit_status_code = 0;
	cxt->exception_encountered = FALSE;
	cxt->floating_point_precision = EG(precision);
	cxt->send_target = NULL;
	cxt->argument_indices = NULL;
	cxt->argument_count = 0;
	cxt->result_index = 0;
	cxt->caller_line_id = 0;
#ifdef ZEND_WIN32
	cxt->windows_timed_out_pointer = &EG(timed_out);
#endif
	SAVE_TSRMLS
}

void qb_free_interpreter_context(qb_interpreter_context *cxt) {
	if(cxt->function) {
		qb_unlock_function(cxt->function);
	}
	if(cxt->send_target) {
		// this should not happen inside a worker thread
		efree(cxt->send_target);
	}
}

static zend_always_inline void qb_enter_vm(qb_interpreter_context *cxt) {
#ifdef NATIVE_COMPILE_ENABLED
	if(cxt->function->native_proc) {
		qb_native_proc proc = cxt->function->native_proc;
		proc(cxt);
	} else {
		qb_main(cxt);
	}
#else
	qb_main(cxt);
#endif
}

static void qb_execute_in_worker_thread(void *param1, void *param2, int param3);

static void qb_fork_execution(qb_interpreter_context *cxt) {
	qb_interpreter_context *fork_contexts;
	qb_task_group _group, *group = &_group;
	qb_thread *original_thread = cxt->thread;
	uint32_t original_fork_id = cxt->fork_id;
	uint32_t original_thread_count = cxt->thread_count;
	uint32_t i, fork_count, function_count, new_context_count, remaining_thread_count;
	intptr_t instr_offset = cxt->instruction_pointer - cxt->function->instructions;
	int32_t reusing_original_cxt = 1;

	if(cxt->thread_count > 0) {
		// we'll create one copy of the function per thread
		function_count = cxt->thread_count;
	} else {
		function_count = 1;
	}

	if(cxt->fork_count == 0) {
		fork_count = cxt->fork_count = cxt->thread_count;
	} else {
		fork_count = cxt->fork_count;
		if(cxt->fork_count > (uint32_t) cxt->thread_count) {
			// if there're more forks than functions, we need to keep the original storage intact,
			// so we can restore a context to the state at the point of the fork
			reusing_original_cxt = 0;
		}
	}
	new_context_count = fork_count - reusing_original_cxt;

	// the number of threads that the each fork can use
	remaining_thread_count = (cxt->thread_count > cxt->fork_count) ? cxt->thread_count - cxt->fork_count : 0;

	// initialize the group, allocating extra memory for new interpreter contexts
	qb_initialize_task_group(group, original_thread, fork_count, sizeof(qb_interpreter_context) * new_context_count);
	fork_contexts = group->extra_memory;

	if(reusing_original_cxt) {
		// schedule the first worker
		cxt->fork_id = 0;
		cxt->thread = NULL;
		cxt->thread_count = remaining_thread_count;
		qb_add_task(group, qb_execute_in_worker_thread, cxt, NULL, 0, &cxt->thread);
	}

	// initialize new interpreter contexts
	if(new_context_count > 0) {
		USE_TSRM
		for(i = reusing_original_cxt; i < cxt->fork_count; i++) {
			qb_interpreter_context *fork_cxt = &fork_contexts[i - reusing_original_cxt];
			if(i < function_count && cxt->thread->type == QB_THREAD_MAIN) {
				// acquire the function in the main thread now to avoid context switching from the worker
				// (since emalloc() does not work inside worker threads)
				fork_cxt->function = qb_acquire_function(cxt, cxt->function, FALSE);
				fork_cxt->instruction_pointer = (int8_t *) (fork_cxt->function->instruction_base_address + instr_offset);
			} else {
				// let the worker acquire a copy relinquished by another that has finished
				fork_cxt->function = NULL;
				fork_cxt->instruction_pointer = (int8_t *) (instr_offset);
			}
			fork_cxt->caller_context = NULL;
			fork_cxt->thread = NULL;
			fork_cxt->thread_count = remaining_thread_count;
			fork_cxt->fork_id = i;
			fork_cxt->fork_count = fork_count;
			fork_cxt->argument_indices = NULL;
			fork_cxt->argument_count = 0;
			fork_cxt->result_index = 0;
			fork_cxt->caller_line_id = 0;
			fork_cxt->call_depth = cxt->call_depth;
			fork_cxt->exit_type = QB_VM_RETURN;
			fork_cxt->exit_status_code = 0;
			fork_cxt->windows_timed_out_pointer = cxt->windows_timed_out_pointer;
			fork_cxt->floating_point_precision = cxt->floating_point_precision;
			fork_cxt->send_target = NULL;
#ifdef ZTS
			fork_cxt->tsrm_ls = tsrm_ls;
#endif
			// pass the function as a parameter to qb_execute_in_worker_thread() if a copy has not been acquired yet
			qb_add_task(group, qb_execute_in_worker_thread, fork_cxt, (!fork_cxt->function) ? cxt->function : NULL, 0, &fork_cxt->thread);
		}
	}

	// run the tasks and wait for completion
	qb_run_task_group(group);

	if(reusing_original_cxt) {
		// restore variables in the original context
		cxt->fork_id = original_fork_id;
		cxt->thread = original_thread;
		cxt->thread_count = original_thread_count;
	} else {
		// transfer the execution state to the original context
		qb_interpreter_context *first_cxt = &fork_contexts[0];
		instr_offset = (intptr_t) first_cxt->instruction_pointer;
		cxt->exit_type = first_cxt->exit_type;
		cxt->instruction_pointer = cxt->function->instructions + instr_offset;

		if(cxt->exit_type == QB_VM_SPOON) {
			// need to transfer non-shared segments back into the original storage
			// since the execution will resume
		}
	}

	if(new_context_count > 0) {
		// free the new interpreter contexts
		for(i = reusing_original_cxt; i < fork_count; i++) {
			qb_interpreter_context *fork_cxt = &fork_contexts[i - reusing_original_cxt];
			if(fork_cxt->exit_type == QB_VM_EXCEPTION) {
				// an exception occurred in one of the fork
				cxt->exit_type = QB_VM_EXCEPTION;
			}
			qb_free_interpreter_context(fork_cxt);
		}
	}
	qb_free_task_group(group);
}

static void qb_handle_execution(qb_interpreter_context *cxt, int32_t forked) {
	int32_t continue_execution = FALSE;
	int32_t fork_execution = FALSE;

	do {
		if(fork_execution) {
			qb_fork_execution(cxt);
		} else {
			qb_enter_vm(cxt);
		}

		switch(cxt->exit_type) {
			case QB_VM_EXCEPTION: {
				continue_execution = FALSE;
			}	break;
			case QB_VM_YIELD: {
				continue_execution = FALSE;
			}	break;
			case QB_VM_RETURN: {
				continue_execution = FALSE;
			}	break;
			case QB_VM_SPOON: {
				// go back into the vm, unless we're running a forked copy
				continue_execution = !forked;
				fork_execution = FALSE;
			}	break;
			case QB_VM_FORK: {
				// fork the vm
				continue_execution = TRUE;
				if(cxt->fork_count > 0 || cxt->thread_count > 0) {
					fork_execution = TRUE;
				} else {
					// not threads are available and the call to fork() doesn't specify a number
					cxt->fork_id = 0;
					fork_execution = FALSE;
				}
			}	break;
			case QB_VM_ERROR: {
				USE_TSRM
				qb_dispatch_exceptions(TSRMLS_C);
				continue_execution = FALSE;
			}	break;
			case QB_VM_WARNING: {
				USE_TSRM
				qb_dispatch_exceptions(TSRMLS_C);
				continue_execution = TRUE;
			}	break;
			case QB_VM_TIMEOUT: {
				zend_timeout(0);
				continue_execution = FALSE;
			}	break;
			default: {
			}	break;
		}
	} while(continue_execution);
}

static void qb_execute_in_worker_thread(void *param1, void *param2, int param3) {
	qb_interpreter_context *cxt = param1;

	if(cxt->function) {
		if(UNEXPECTED(!(cxt->function->flags & QB_FUNCTION_RELOCATED))) {
			cxt->instruction_pointer += qb_relocate_function(cxt->function, FALSE);
		}
	} else {
		// if we're acquiring the function now, then it's because there were more forks than workers
		// a properly relocated function should be available
		qb_function *qfunc = param2;
		cxt->function = qb_acquire_function(cxt, qfunc, FALSE);
		cxt->instruction_pointer += cxt->function->instruction_base_address;

		// copy the original function state
		qb_copy_storage_contents(qfunc->local_storage, cxt->function->local_storage);
	}

	// resume execution
	qb_handle_execution(cxt, TRUE);

	// adjust instruction offset
	cxt->instruction_pointer = (int8_t *) (cxt->instruction_pointer - cxt->function->instructions);

	// unlock the function
	qb_unlock_function(cxt->function);
	cxt->function = NULL;
}

static int32_t qb_execute_in_main_thread(qb_interpreter_context *cxt) {
	qb_handle_execution(cxt, FALSE);

	if(cxt->exit_type == QB_VM_EXCEPTION) {
		return FALSE;
	} else if(cxt->exit_type == QB_VM_YIELD) {
		return FALSE;
	} else {
		return TRUE;
	}
}

static void qb_initialize_local_variables(qb_interpreter_context *cxt) {
	qb_memory_segment *shared_scalar_segment, *local_scalar_segment, *local_array_segment, *shared_array_segment;
	int8_t *memory_start, *memory_end;
	uint32_t i;

	// make sure the function is relocated first
	if(UNEXPECTED(!(cxt->function->flags & QB_FUNCTION_RELOCATED))) {
		cxt->instruction_pointer += qb_relocate_function(cxt->function, TRUE);
	}

	shared_scalar_segment = &cxt->function->local_storage->segments[QB_SELECTOR_SHARED_SCALAR];
	local_scalar_segment = &cxt->function->local_storage->segments[QB_SELECTOR_LOCAL_SCALAR];
	local_array_segment = &cxt->function->local_storage->segments[QB_SELECTOR_LOCAL_ARRAY];
	shared_array_segment = &cxt->function->local_storage->segments[QB_SELECTOR_SHARED_ARRAY];

	// clear the segments that require it
	// the following optimization depends very much on how the segments are laid out
	memory_start = shared_scalar_segment->memory;
	memory_end = local_scalar_segment->memory + local_scalar_segment->byte_count;
	memset(memory_start, 0, memory_end - memory_start);

	memory_start = local_array_segment->memory;
	memory_end = shared_array_segment->memory + shared_array_segment->byte_count;
	memset(memory_start, 0, memory_end - memory_start);

	// relocate large fixed-length local arrays
	for(i = QB_SELECTOR_ARRAY_START; i < cxt->function->local_storage->segment_count; i++) {
		qb_memory_segment *segment = &cxt->function->local_storage->segments[i];
		if(segment->flags & QB_SEGMENT_REALLOCATE_ON_CALL) {
			qb_allocate_segment_memory(segment, segment->byte_count);
			if(segment->flags & QB_SEGMENT_CLEAR_ON_CALL) {
				memset(segment->memory, 0, segment->current_allocation);
			}
		}
	}
}

static void qb_finalize_variables(qb_interpreter_context *cxt) {
	uint32_t i;
	for(i = QB_SELECTOR_ARRAY_START; i < cxt->function->local_storage->segment_count; i++) {
		qb_memory_segment *segment = &cxt->function->local_storage->segments[i];
		if(segment->flags & QB_SEGMENT_FREE_ON_RETURN) {
			qb_release_segment(segment);
			if(segment->flags & QB_SEGMENT_EMPTY_ON_RETURN) {
				segment->byte_count = 0;
			}
		}
	}
}

void qb_run_zend_extension_op(qb_interpreter_context *cxt, uint32_t zend_opcode, uint32_t line_id) {
}

int32_t qb_execute(qb_interpreter_context *cxt) {
	int32_t success = TRUE;

	// clear local memory segments
	qb_initialize_local_variables(cxt);

	// copy values from arguments, class variables, object variables, and global variables
	qb_transfer_variables_from_external_sources(cxt);

	// enter the vm
	if(qb_execute_in_main_thread(cxt)) {
		// move values back into caller space
		qb_transfer_variables_to_external_sources(cxt);
	} else {
		success = FALSE;
	}

	// release dynamically allocated segments
	qb_finalize_variables(cxt);

	return success;
}

#ifdef ZEND_ACC_GENERATOR
int32_t qb_execute_resume(qb_interpreter_context *cxt) {
	// copy variable passed by send()
	qb_transfer_arguments_from_generator(cxt);

	// enter the vm
	if(!qb_execute_in_main_thread(cxt)) {
		// there're more values still
		qb_transfer_variables_to_generator(cxt);
		return FALSE;
	} else {
		// done running
		qb_finalize_variables(cxt);
		return TRUE;
	}
}

int32_t qb_execute_rewind(qb_interpreter_context *cxt) {
	// clear local memory segments
	qb_initialize_local_variables(cxt);

	// copy values from arguments, class variables, object variables, and global variables
	qb_transfer_variables_from_external_sources(cxt);

	return qb_execute_resume(cxt);
}
#endif

int32_t qb_execute_internal(qb_interpreter_context *cxt) {
	int32_t success = TRUE;
	qb_initialize_local_variables(cxt);
	if(!qb_execute_in_main_thread(cxt)) {
		success = FALSE;
	}
	qb_finalize_variables(cxt);
	return success;
}

void qb_dispatch_instruction_to_threads(qb_interpreter_context *cxt, void *control_func, int8_t **instruction_pointers, uint32_t thread_count) {
	qb_task_group _group, *group = &_group;
	qb_task tasks[MAX_THREAD_COUNT];
	qb_thread *original_thread = cxt->thread;
	uint32_t original_thread_count = cxt->thread_count;
	uint32_t i;

	group->tasks = tasks;
	group->completion_count = 0;
	group->task_count = 0;
	group->task_index = 0;
	group->owner = cxt->thread;
	group->extra_memory = NULL;
	group->previous_group = NULL;
	group->next_group = NULL;
	for(i = 0; i < thread_count; i++) {
		int8_t *ip = instruction_pointers[i];
		qb_add_task(group, control_func, cxt, ip, 0, &cxt->thread);
	}

	// set the thread count to zero, so the controller function performs the task instead of redirecting it
	cxt->thread_count = 0;
	qb_run_task_group(group);

	// restore variables
	cxt->thread = original_thread;
	cxt->thread_count = original_thread_count;
}

void qb_dispatch_instruction_to_main_thread(qb_interpreter_context *cxt, void *control_func, int8_t *instruction_pointer) {
	qb_run_in_main_thread(cxt->thread, control_func, cxt, instruction_pointer, 0, &cxt->thread);
}

static zval * qb_invoke_zend_function(qb_interpreter_context *cxt, zend_function *zfunc, zval ***argument_pointers, uint32_t argument_count, uint32_t line_id) {
	USE_TSRM
	zval *retval = NULL;
	zend_op **p_user_op = EG(opline_ptr);
	zend_fcall_info fci;
	zend_fcall_info_cache fcc;
	int call_result;
	zend_execute_data *ex = EG(current_execute_data);

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

	// no good way of avoiding an extra item showing up in debug_print_backtrace()
	// put it the function being called so at least it matches the line number
	ex->function_state.function = zfunc;
	
	// set the qb user op's line number to the line number where the call occurs
	// so that debug_backtrace() will get the right number
	(*p_user_op)->lineno = LINE_NUMBER(line_id);
	call_result = zend_call_function(&fci, &fcc TSRMLS_CC);
	(*p_user_op)->lineno = 0;

	if(call_result != SUCCESS) {
		const char *function_name = zfunc->common.function_name;
		const char *class_name = (zfunc->common.scope) ? zfunc->common.scope->name : NULL;
		qb_report_function_call_exception(cxt->thread, line_id, class_name, zfunc->common.function_name);
	}
	return retval;
}

static int32_t qb_execute_zend_function_call(qb_interpreter_context *cxt, zend_function *zfunc, uint32_t *variable_indices, uint32_t argument_count, uint32_t result_index, uint32_t line_id) {
	USE_TSRM
	zval **arguments, ***argument_pointers, *retval;
	uint32_t i;
	ALLOCA_FLAG(use_heap1)
	ALLOCA_FLAG(use_heap2)

	arguments = do_alloca(sizeof(zval *) * argument_count, use_heap1);
	argument_pointers = do_alloca(sizeof(zval **) * argument_count, use_heap2);

	for(i = 0; i < argument_count; i++) {
		uint32_t var_index = variable_indices[i];
		qb_variable *var = cxt->function->variables[var_index];
		ALLOC_INIT_ZVAL(arguments[i]);
		qb_transfer_value_to_zval(cxt->function->local_storage, var->address, arguments[i]);
		argument_pointers[i] = &arguments[i];
	}

	qb_sync_imported_variables(cxt);

	retval = qb_invoke_zend_function(cxt, zfunc, argument_pointers, argument_count, line_id);
	if(!retval) {
		return FALSE;
	}

	qb_refresh_imported_variables(cxt);

	for(i = 0; i < argument_count; i++) {
		// if argument info is missing, then assume it's by ref
		int32_t by_ref = (zfunc->common.arg_info) ? zfunc->common.arg_info[i].pass_by_reference : TRUE;
		if(by_ref) {
			uint32_t var_index = variable_indices[i];
			qb_variable *var = cxt->function->variables[var_index];
			qb_transfer_value_from_zval(cxt->function->local_storage, var->address, arguments[i], QB_TRANSFER_CAN_SEIZE_MEMORY);
		}
	}

	if(result_index != INVALID_INDEX) {
		qb_variable *retvar = cxt->function->variables[result_index];
		qb_transfer_value_from_zval(cxt->function->local_storage, retvar->address, retval, QB_TRANSFER_CAN_SEIZE_MEMORY);
	}
	for(i = 0; i < argument_count; i++) {
		zval_ptr_dtor(&arguments[i]);
	}
	if(retval) {
		zval_ptr_dtor(&retval);
	}
	cxt->exception_encountered = (EG(exception) != NULL);

	free_alloca(arguments, use_heap1);
	free_alloca(argument_pointers, use_heap2);
	return TRUE;
}

static int32_t qb_execute_function_call(qb_interpreter_context *cxt, qb_function *qfunc, uint32_t *variable_indices, uint32_t argument_count, uint32_t result_index, uint32_t line_id) {
	if(cxt->call_depth < 1024) {
		USE_TSRM
		qb_interpreter_context _new_cxt, *new_cxt = &_new_cxt;
		int32_t successful;

		cxt->argument_indices = variable_indices;
		cxt->argument_count = argument_count;
		cxt->result_index = result_index;
		cxt->caller_line_id = line_id;
		cxt->exception_encountered = FALSE;

		qb_initialize_interpreter_context(new_cxt, qfunc, cxt TSRMLS_CC);
		successful = qb_execute(new_cxt);
		qb_free_interpreter_context(new_cxt);
		return successful;
	} else {
		qb_report_too_much_recursion_exception(cxt->thread, line_id, cxt->call_depth);
		return FALSE;
	}
}

static int32_t qb_execute_function_call_thru_zend(qb_interpreter_context *cxt, zend_function *zfunc, uint32_t *variable_indices, uint32_t argument_count, uint32_t result_index, uint32_t line_id) {
	USE_TSRM
	int32_t successful;

	cxt->argument_indices = variable_indices;
	cxt->argument_count = argument_count;
	cxt->result_index = result_index;
	cxt->caller_line_id = line_id;
	cxt->exception_encountered = FALSE;

	QB_G(caller_interpreter_context) = cxt;
	successful = qb_execute_zend_function_call(cxt, zfunc, variable_indices, argument_count, INVALID_INDEX, line_id);
	QB_G(caller_interpreter_context) = NULL;
	return successful;
}

int32_t qb_dispatch_function_call(qb_interpreter_context *cxt, uint32_t symbol_index, uint32_t *variable_indices, uint32_t argument_count, uint32_t result_index, uint32_t line_id) {
	USE_TSRM
	qb_external_symbol *symbol = &QB_G(external_symbols)[symbol_index];
	zend_function *zfunc = symbol->pointer;
	qb_function *qfunc;

	if(symbol->type == QB_EXT_SYM_STATIC_ZEND_FUNCTION) {
		zend_class_entry *called_scope = EG(called_scope);
		if(zfunc->common.scope != called_scope) {
			zend_hash_find(&called_scope->function_table, zfunc->common.function_name, (uint32_t) strlen(zfunc->common.function_name) + 1, (void **) &zfunc);
		}
	}

	qfunc = qb_get_compiled_function(zfunc);
	if(qfunc) {
		if(QB_G(allow_debug_backtrace)) {
			return qb_execute_function_call_thru_zend(cxt, zfunc, variable_indices, argument_count, result_index, line_id);
		} else {
			return qb_execute_function_call(cxt, qfunc, variable_indices, argument_count, result_index, line_id);
		}
	} else {
		return qb_execute_zend_function_call(cxt, zfunc, variable_indices, argument_count, result_index, line_id);
	}
}
