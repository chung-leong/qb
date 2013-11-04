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

#ifndef QB_NATIVE_COMPILER_H_
#define QB_NATIVE_COMPILER_H_

#ifdef NATIVE_COMPILE_ENABLED

typedef struct qb_native_compiler_context	qb_native_compiler_context;

#define QB_NATIVE_FUNCTION_RET 			void
#define QB_NATIVE_FUNCTION_ATTR
#define QB_NATIVE_FUNCTION_ARGS 		qb_interpreter_context *__restrict cxt

typedef QB_NATIVE_FUNCTION_RET (QB_NATIVE_FUNCTION_ATTR *qb_native_proc)(QB_NATIVE_FUNCTION_ARGS);

struct qb_native_compiler_context {
	FILE *write_stream;
	FILE *read_stream;
	FILE *error_stream;

	qb_compiler_context **compiler_contexts;
	uint32_t compiler_context_count;
	qb_data_pool *pool;

	qb_op **ops;
	uint32_t op_count;
	qb_variable **variables;
	uint32_t variable_count;

	qb_address **constant_scalars;
	uint32_t constant_scalar_count;

	qb_address **writable_scalars;
	uint32_t writable_scalar_count;

	qb_address **constant_arrays;
	uint32_t constant_array_count;

	qb_address **writable_arrays;
	uint32_t writable_array_count;

	qb_address *zero_address;

	qb_external_symbol *external_symbols;
	uint32_t external_symbol_count;

	qb_function *compiled_function;
	qb_storage *storage;

	char * const *op_names;
	char * const *op_actions;
	int32_t * const *op_function_usages;
	char * const *function_prototypes;
	
	char *cache_folder_path;
	char *obj_file_path;
	char *c_file_path;
	uint64_t file_id;
	uint32_t qb_version;

#ifdef _MSC_VER
	HANDLE msc_thread;
	HANDLE msc_process;
#endif

	char *binary;
	uint32_t binary_size;

	int32_t print_errors;
	int32_t print_source;

#ifdef ZTS
	void ***tsrm_ls;
#endif

	uint32_t string_buffer_index;
	char string_buffers[16][128];
};

extern qb_native_symbol global_native_symbols[];
extern uint32_t global_native_symbol_count;

void qb_free_native_code(qb_native_code_bundle *bundle);
void qb_compile_to_native_code(qb_native_compiler_context *cxt);
void qb_initialize_native_compiler_context(qb_native_compiler_context *cxt, qb_build_context *build_cxt TSRMLS_DC);
void qb_free_native_compiler_context(qb_native_compiler_context *cxt);

#endif

#endif
