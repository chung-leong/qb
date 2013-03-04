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
	unsigned char *windows_timed_out_pointer;
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

#if !ZEND_ENGINE_2_3 && !ZEND_ENGINE_2_2 && !ZEND_ENGINE_2_1
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

#ifndef Z_REFCOUNT_P
	#define Z_REFCOUNT_P(zv)		zv->refcount
#endif

#ifndef Z_ADDREF_P
	#define Z_ADDREF_P(zv)			zv->refcount++
#endif

#endif
