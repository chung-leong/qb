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

#ifndef QB_INTERPRETER_H_
#define QB_INTERPRETER_H_

typedef struct qb_variable_import		qb_variable_import;
typedef struct qb_call_stack_item		qb_call_stack_item;
typedef struct qb_interpreter_context	qb_interpreter_context;
typedef struct qb_native_symbol			qb_native_symbol;
typedef struct qb_native_proc_record	qb_native_proc_record;

struct qb_variable_import {
	qb_variable *variable;
	qb_storage *storage;
	uint32_t previous_copy_index;
	zval **value_pointer;
};

struct qb_call_stack_item {
	qb_storage *storage;
	qb_function *function;
	uint32_t function_call_line_number;
};

enum {
	QB_EMPLOY_SHADOW_VARIABLES = 0x00000001,
};

struct qb_interpreter_context {
	qb_storage *storage;
	qb_function *function;
	uint32_t flags;

	qb_call_stack_item *call_stack;
	uint32_t call_stack_height;

	qb_variable_import *variable_imports;
	uint32_t variable_import_count;

	int32_t exception_encountered;
	uint32_t function_call_line_number;
	uint32_t *line_number_pointer;
	volatile unsigned char *windows_timed_out_pointer;
	int floating_point_precision;
	void ***tsrm_ls;

	zend_function *zend_function;
	uint32_t argument_count;
	zval **zend_arguments;
	zval ***zend_argument_pointers;
	uint32_t zend_argument_buffer_size;
	zval *this_object;
	qb_address *argument_address;
	qb_address value_address;
	qb_address array_address;
	qb_address *dimension_addresse_pointers[MAX_DIMENSION];
	qb_address *array_size_address_pointers[MAX_DIMENSION];
	qb_address dimension_addresses[MAX_DIMENSION];
	qb_address array_size_addresses[MAX_DIMENSION];

	qb_thread_pool *thread_pool;
	uint32_t thread_count_for_next_op;
	qb_pointer_adjustment adjustments_for_next_op[MAX_THREAD_COUNT][8];

	zval **debug_fcall_arguments;
};

struct qb_native_symbol {
	long hash_value;
	const char *name;
	void *address;
};

struct qb_native_proc_record {
	uint64_t crc64;
	void *proc;
};

void ZEND_FASTCALL qb_execute_internal(qb_function *qfunc TSRMLS_DC);

#if ZEND_DEBUG
extern qb_native_proc_record *native_proc_table;
extern uint32_t native_proc_table_size;
#endif

#ifdef __GNUC__
extern void *op_handlers[];
#endif

#if !ZEND_ENGINE_2_4 && !ZEND_ENGINE_2_3 && !ZEND_ENGINE_2_2 && !ZEND_ENGINE_2_1
	#define Z_OBJ_GET_PROP_PTR_PTR(zv, n)		Z_OBJ_HT_P(zv)->get_property_ptr_ptr(zv, n, BP_VAR_W, NULL TSRMLS_CC)
	#define Z_OBJ_WRITE_PROP(zv, n, v)			Z_OBJ_HT_P(zvalue)->write_property(zv, n, v, NULL TSRMLS_CC)
	#define Z_OBJ_READ_PROP(zv, n)				Z_OBJ_HT_P(zvalue)->read_property(zv, n, BP_VAR_R, NULL TSRMLS_CC)

	#define Z_CLASS_GET_PROP(ce, n, len)		zend_std_get_static_property(ce, n, len, TRUE, NULL TSRMLS_CC)
#elif !ZEND_ENGINE_2_3 && !ZEND_ENGINE_2_2 && !ZEND_ENGINE_2_1
	#define Z_OBJ_GET_PROP_PTR_PTR(zv, n)		Z_OBJ_HT_P(zv)->get_property_ptr_ptr(zv, n, NULL TSRMLS_CC)
	#define Z_OBJ_WRITE_PROP(zv, n, v)			Z_OBJ_HT_P(zvalue)->write_property(zv, n, v, NULL TSRMLS_CC)
	#define Z_OBJ_READ_PROP(zv, n)				Z_OBJ_HT_P(zvalue)->read_property(zv, n, BP_VAR_R, NULL TSRMLS_CC)

	#define Z_CLASS_GET_PROP(ce, n, len)		zend_std_get_static_property(ce, n, len, TRUE, NULL TSRMLS_CC)
#else
	#define Z_OBJ_GET_PROP_PTR_PTR(zv, n)		Z_OBJ_HT_P(zv)->get_property_ptr_ptr(zv, n TSRMLS_CC)
	#define Z_OBJ_WRITE_PROP(zv, n, v)			Z_OBJ_HT_P(zvalue)->write_property(zv, n, v TSRMLS_CC)
	#define Z_OBJ_READ_PROP(zv, n)				Z_OBJ_HT_P(zvalue)->read_property(zv, n, BP_VAR_R TSRMLS_CC)

	#define Z_CLASS_GET_PROP(ce, n, len)		zend_std_get_static_property(ce, (char *) n, len, TRUE TSRMLS_CC)
#endif

void ZEND_FASTCALL qb_dispatch_instruction_to_threads(qb_interpreter_context *cxt, void *control_func, int8_t **instruction_pointers);
void ZEND_FASTCALL qb_initialize_function_call(qb_interpreter_context *cxt, zend_function *zfunc, uint32_t argument_count, uint32_t line_number);
void ZEND_FASTCALL qb_execute_function_call(qb_interpreter_context *cxt);
void ZEND_FASTCALL qb_copy_argument(qb_interpreter_context *cxt, uint32_t argument_index);
void ZEND_FASTCALL qb_resync_argument(qb_interpreter_context *cxt, uint32_t argument_index);
void ZEND_FASTCALL qb_finalize_function_call(qb_interpreter_context *cxt);
void ZEND_FASTCALL qb_enlarge_segment(qb_interpreter_context *cxt, qb_memory_segment *segment, uint32_t desired_size);
void ZEND_FASTCALL qb_shrink_segment(qb_interpreter_context *restrict cxt, qb_memory_segment *segment, uint32_t start_index, uint32_t count);
void ZEND_FASTCALL qb_run_zend_extension_op(qb_interpreter_context *cxt, uint32_t zend_opcode, uint32_t line_number);
ZEND_ATTRIBUTE_FORMAT(printf, 1, 3) NO_RETURN
void qb_abort_at(const char *format, uint32_t line_number, ...);
NO_RETURN void qb_abort_range_error(qb_interpreter_context *restrict cxt, qb_memory_segment *segment, uint32_t index, uint32_t count, uint32_t line_number);
NO_RETURN void qb_abort_divide_by_zero_error(qb_interpreter_context *restrict cxt, uint32_t line_number);

#endif
