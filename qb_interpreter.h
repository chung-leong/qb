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

typedef struct qb_import_scope			qb_import_scope;
typedef struct qb_interpreter_context	qb_interpreter_context;
typedef struct qb_native_symbol			qb_native_symbol;
typedef struct qb_native_proc_record	qb_native_proc_record;
typedef struct qb_import_scope			qb_import_scope;
typedef struct qb_zend_argument_stack	qb_zend_argument_stack;

typedef enum qb_import_scope_type		qb_import_scope_type;
typedef enum qb_vm_exit_type			qb_vm_exit_type;

enum qb_import_scope_type {
	QB_IMPORT_SCOPE_GLOBAL = 1,
	QB_IMPORT_SCOPE_CLASS,
	QB_IMPORT_SCOPE_OBJECT,
	QB_IMPORT_SCOPE_ABSTRACT_OBJECT,
};

struct qb_import_scope {
	qb_import_scope_type type;
	qb_import_scope *parent;
	qb_variable **variables;
	uint32_t variable_count;
	qb_storage *storage;

	union {
		zend_class_entry *zend_class;
		zval *zend_object;
		void *associated_object;
	};
};

struct qb_zend_argument_stack {
	zval **arguments;
	uint32_t argument_count;
	zval ***argument_pointers;
	uint32_t argument_pointer_count;
};

enum qb_vm_exit_type { 
	QB_VM_RETURN = 0,
	QB_VM_BAILOUT,
	QB_VM_FORK,
	QB_VM_SPOON,
	QB_VM_EXCEPTION,
	QB_VM_YIELD,
};

struct qb_interpreter_context {
	qb_function *function;
	int8_t *instruction_pointer;
	qb_interpreter_context *caller_context;

	qb_thread_worker *worker;

	uint32_t fork_id;
	uint32_t fork_count;

	uint32_t *argument_indices;
	uint32_t argument_count;
	uint32_t result_index;
	uint32_t line_number;
	uint32_t call_depth;

	qb_vm_exit_type exit_type;
	int32_t exit_status_code;

	volatile unsigned char *windows_timed_out_pointer;
	int floating_point_precision;
	void ***tsrm_ls;
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

#if ZEND_DEBUG
extern qb_native_proc_record *native_proc_table;
extern uint32_t native_proc_table_size;
#endif

#ifdef __GNUC__
extern void *op_handlers[];
#endif

void qb_dispatch_instruction_to_threads(qb_interpreter_context *cxt, void *control_func, int8_t **instruction_pointers);

void qb_execute(qb_interpreter_context *cxt);
void qb_execute_internal(qb_interpreter_context *cxt);

void qb_run_zend_extension_op(qb_interpreter_context *cxt, uint32_t zend_opcode, uint32_t line_number);

ZEND_ATTRIBUTE_FORMAT(printf, 1, 3) NO_RETURN
void qb_abort_at(const char *format, uint32_t line_number, ...);

intptr_t qb_adjust_memory_segment(qb_interpreter_context *cxt, uint32_t segment_selector, uint32_t new_size);

void qb_dispatch_function_call(qb_interpreter_context *cxt, uint32_t symbol_index, uint32_t *variable_indices, uint32_t argument_count, uint32_t result_index, uint32_t line_number);

void qb_initialize_interpreter_context(qb_interpreter_context *cxt, qb_function *qfunc, qb_interpreter_context *caller_cxt TSRMLS_DC);
void qb_free_interpreter_context(qb_interpreter_context *cxt);

void qb_main(qb_interpreter_context *__restrict cxt);

void qb_write_output(qb_interpreter_context *cxt, const char *s, uint32_t count);

#endif
