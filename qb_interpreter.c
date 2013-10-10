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

static void qb_transfer_variables_from_php(qb_interpreter_context *cxt, qb_function *qfunc) {
	USE_TSRM
	void **p = EG(current_execute_data)->prev_execute_data->function_state.arguments;
	uint32_t received_argument_count = (uint32_t) (zend_uintptr_t) *p;
	uint32_t i;

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

		if((qvar->flags & QB_VARIABLE_PASSED_BY_REF) || READ_ONLY(qvar->address)) {
			// avoid allocating new memory and copying contents if changes will be copied back anyway (or no changes will be made)
			transfer_flags = QB_TRANSFER_CAN_BORROW_MEMORY;
		}
		qb_transfer_value_from_zval(qfunc->local_storage, qvar->address, zarg, transfer_flags);
	}
}

static void qb_transfer_variables_to_php(qb_interpreter_context *cxt, qb_function *qfunc) {
	USE_TSRM
	void **p = EG(current_execute_data)->prev_execute_data->function_state.arguments;
	uint32_t received_argument_count = (uint32_t) (zend_uintptr_t) *p;
	uint32_t i;

	if(EG(return_value_ptr_ptr)) {
		zval *ret;
		ALLOC_INIT_ZVAL(ret);
		*EG(return_value_ptr_ptr) = ret;

		// copy value into return variable
		if(qfunc->return_variable->address) {
			qb_transfer_value_to_zval(qfunc->local_storage, qfunc->return_variable->address, ret);
		} 
	}

	for(i = 0; i < qfunc->argument_count; i++) {
		qb_variable *qvar = qfunc->variables[i];

		if(qvar->flags & QB_VARIABLE_PASSED_BY_REF) {
			if(i < received_argument_count) {
				zval **p_zarg = (zval**) p - received_argument_count + i;
				zval *zarg = *p_zarg;
				qb_transfer_value_to_zval(qfunc->local_storage, qvar->address, zarg);
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
			qb_allocate_segment_memory(qfunc->local_storage, segment, segment->byte_count);
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
			qb_release_segment(qfunc->local_storage, segment);
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

intptr_t qb_adjust_memory_segment(qb_interpreter_context *cxt, qb_storage *storage, uint32_t segment_selector, uint32_t new_size) {
	qb_memory_segment *segment = &storage->segments[segment_selector];
	// TODO: this needs to happen in the main thread
	return qb_resize_segment(storage, segment, new_size);
}