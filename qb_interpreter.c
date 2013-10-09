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

static void qb_transfer_arguments_from_php(qb_interpreter_context *cxt, qb_function *qfunc, zval *this, zval *retval, zval ***p_args, uint32_t arg_count) {
	uint32_t i;

	cxt->this_object = this;
	if(arg_count > qfunc->argument_count) {
		arg_count = qfunc->argument_count;
	}

	for(i = 0; i < arg_count; i++) {
		qb_variable *qvar = qfunc->variables[i];
		zval *zarg = *p_args[i];
		uint32_t transfer_flags = 0;

		if((qvar->flags & QB_VARIABLE_PASSED_BY_REF) || READ_ONLY(qvar->address)) {
			// avoid allocating new memory and copying contents if changes will be copied back anyway (or no changes will be made)
			transfer_flags = QB_TRANSFER_CAN_BORROW_MEMORY;
		}
		qb_transfer_value_from_zval(qfunc->local_storage, qvar->address, zarg, transfer_flags);
	}
}

static void qb_transfer_arguments_to_php(qb_interpreter_context *cxt, qb_function *qfunc, zval *retval, zval ***p_args, int arg_count) {
	uint32_t i;

	// copy value into return variable
	if(qfunc->return_variable->address) {
		if(!SCALAR(qfunc->return_variable->address)) {
			qb_initialize_zval_array(cxt, qfunc->return_variable->address, NULL, retval);
		}
		qb_transfer_value_to_zval(cxt, qfunc->return_variable->address, retval);
	}

	// copy values into arguments passed by reference and return variable
	for(i = 0; i < qfunc->argument_count; i++) {
		qb_variable *qvar = qfunc->variables[i];

		if(qvar->flags & QB_VARIABLE_ARGUMENT && qvar->flags & QB_VARIABLE_PASSED_BY_REF) {
			zval *zarg = *p_args[i];
			qb_transfer_value_to_zval(qfunc->local_storage, qvar->address, zarg, zarg);
		}
	}
}

static void qb_initialize_interpreter_context(qb_interpreter_context *cxt TSRMLS_DC) {
	uint32_t i;
	memset(cxt, 0, sizeof(qb_interpreter_context));

	// use shadow variables for debugging purpose by default
	cxt->flags = QB_EMPLOY_SHADOW_VARIABLES;

	cxt->floating_point_precision = EG(precision);
	cxt->line_number_pointer = &QB_G(current_line_number);
	//cxt->thread_pool = &QB_G(thread_pool);
#ifdef ZEND_WIN32
	cxt->windows_timed_out_pointer = &EG(timed_out);
#endif
	SAVE_TSRMLS
}

static void qb_free_interpreter_context(qb_interpreter_context *cxt) {
	qb_destroy_array((void **) &cxt->variable_imports);
	qb_destroy_array((void **) &cxt->call_stack);
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

void qb_execute_function_call(qb_interpreter_context *cxt, qb_function *qfunc, uint32_t argument_count) {
	int8_t *memory;
	uint32_t combined_byte_count, i;
		
	// clear memory segments
	// the following optimization depends very much on how the segments are laid out
	memory = qfunc->local_storage->segments[QB_SELECTOR_SHARED_SCALAR].memory;
	combined_byte_count = qfunc->local_storage->segments[QB_SELECTOR_SHARED_SCALAR].byte_count + qfunc->local_storage->segments[QB_SELECTOR_LOCAL_SCALAR].byte_count;
	memset(memory, 0, combined_byte_count);
	memory = qfunc->local_storage->segments[QB_SELECTOR_LOCAL_ARRAY].memory;
	combined_byte_count = qfunc->local_storage->segments[QB_SELECTOR_LOCAL_ARRAY].byte_count + qfunc->local_storage->segments[QB_SELECTOR_SHARED_ARRAY].byte_count;
	memset(memory, 0, combined_byte_count);

	// process the other segments
	for(i = QB_SELECTOR_ARRAY_START; i < qfunc->local_storage->segment_count; i++) {
		qb_memory_segment *segment = &qfunc->local_storage->segments[i];
		if(segment->byte_count) {
			if(segment->byte_count < segment->current_allocation) {
				qb_resize_segment(qfunc->local_storage, segment, segment->byte_count);
				segment->current_allocation = segment->byte_count;
			}
			if(segment->flags & QB_SEGMENT_CLEAR_ON_CALL) {
				memset(segment->memory, 0, segment->byte_count);
			}
		}
	}

	// copy apply default arguments
	for(i = argument_count; i < qfunc->argument_count; i++) {
		qb_variable *qvar = qfunc->variables[i];
		if(qvar->default_value_address) {
			uint32_t transfer_flags;
			if((qvar->flags & QB_VARIABLE_PASSED_BY_REF) || !READ_ONLY(qvar->address)) {
				transfer_flags = 0;
			} else {
				transfer_flags = QB_TRANSFER_CAN_BORROW_MEMORY;
			}
			// copying from the function's own storage, actually
			qb_transfer_value_from_caller_storage(cxt, qfunc->local_storage, qvar->default_value_address, qvar->address, transfer_flags);
		}
	}

	// import external variables
	for(i = qfunc->argument_count; i < qfunc->variable_count; i++) {
		qb_variable *qvar = qfunc->variables[i];
		if(qvar->flags & (QB_VARIABLE_GLOBAL | QB_VARIABLE_CLASS_INSTANCE | QB_VARIABLE_CLASS | QB_VARIABLE_CLASS_CONSTANT)) {
			qb_import_variable(cxt, qvar);
		}
	}

	qb_enter_vm(cxt, qfunc);

	// copy values to imported variables and remove them
	for(i = qfunc->argument_count; i < qfunc->variable_count; i++) {
		qb_variable *qvar = qfunc->variables[i];
		if(qvar->flags & (QB_VARIABLE_GLOBAL | QB_VARIABLE_CLASS | QB_VARIABLE_CLASS_INSTANCE)) {
			qb_retire_imported_variable(cxt, qvar);
		}
	}

	// clean up segments
	for(i = QB_SELECTOR_ARRAY_START; i < qfunc->local_storage->segment_count; i++) {
		qb_memory_segment *segment = &qfunc->local_storage->segments[i];
		if(segment->flags & QB_SEGMENT_FREE_ON_RETURN) {
			qb_free_segment(cxt, segment);
			if(segment->flags & QB_SEGMENT_EMPTY_ON_RETURN) {
				segment->byte_count = 0;
			}
		}
	}
}

void qb_run_zend_extension_op(qb_interpreter_context *cxt, uint32_t zend_opcode, uint32_t line_number) {
}

void qb_execute(qb_function *qfunc, zval *this, zval ***arguments, int argument_count, zval *return_value TSRMLS_DC) {
	int result = SUCCESS;
	qb_interpreter_context _cxt, *cxt = &_cxt;
	qb_initialize_interpreter_context(cxt TSRMLS_CC);

	// copy values from arguments and global symbol table
	qb_transfer_arguments_from_php(cxt, qfunc, this, return_value, arguments, argument_count, qfunc);

	// execute the call
	qb_execute_function_call(cxt, qfunc, argument_count);

	// copy values back to pass-by-ref arguments and global variables
	qb_transfer_arguments_to_php(cxt, qfunc, return_value, arguments, argument_count);

	qb_free_interpreter_context(cxt);
	return result;
}

void qb_execute_internal(qb_function *qfunc TSRMLS_DC) {
	qb_interpreter_context _cxt, *cxt = &_cxt;
	unsigned char windows_time_out = 0;
	SAVE_TSRMLS
	cxt->windows_timed_out_pointer = &windows_time_out;
	qb_execute_function_call(cxt, qfunc, 0);
}

intptr_t qb_resize_array(qb_interpreter_context *__restrict cxt, qb_storage *__restrict storage, uint32_t segment_selector, uint32_t new_size) {
	qb_memory_segment *segment = &storage->segments[segment_selector];
	if(new_size > segment->current_allocation) {
		int8_t *current_data_end;
		int8_t *original_location = segment->memory, *new_location;
		uint32_t new_allocation = ALIGN_TO(new_size, 4);
		uint32_t addition = new_allocation - segment->current_allocation;

		segment->current_allocation = new_allocation;
		qb_resize_segment(cxt, segment, new_allocation);
		new_location = segment->memory;

		// clear the newly allcoated bytes
		current_data_end = new_location + segment->byte_count;
		memset(current_data_end, 0, addition);
		segment->byte_count = new_size;

		if(new_location != original_location) {
			// adjust references in code
			uint32_t i;
			uint32_t diff = new_location - original_location;
			for(i = 0; i < segment->reference_count; i++) {
				uintptr_t *p_ref = segment->references[i];
				*p_ref += diff;
			}
			return diff;
		}
	} else {
		segment->byte_count = new_size;
	}
	return 0;
}

void qb_dispatch_instruction_to_threads(qb_interpreter_context *cxt, void *control_func, int8_t **instruction_pointers) {
	uint32_t i;
	uint32_t count = cxt->thread_count_for_next_op;
	cxt->thread_count_for_next_op = 0;

	if(!cxt->thread_pool->thread_count) {
		//qb_initialize_thread_pool(cxt->thread_pool);
	}
	for(i = 0; i < count; i++)  {
		int8_t *ip = instruction_pointers[i];
		//qb_schedule_task(cxt->thread_pool, control_func, cxt, ip);
	}
	//qb_run_tasks(cxt->thread_pool);
}
