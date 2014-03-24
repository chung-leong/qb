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

#ifndef QB_H_
#define QB_H_

#if _FILE_OFFSET_BITS != 64
#	undef _FILE_OFFSET_BITS
#	define _FILE_OFFSET_BITS 64
#endif

#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif

#define _GNU_SOURCE		1

#include <limits.h>
#include <stdlib.h>
#include <stddef.h>
#include <math.h>

#ifdef _MSC_VER
#	include "win32\php_stdint.h"
#	include "win32\msc_inttypes.h"
#	include "win32\time.h"
#else
#	include <stdint.h>
#	include <inttypes.h>
#	include <sys/types.h>
#	include <sys/param.h>
#endif

#ifdef _MSC_VER
#	define QB_LITTLE_ENDIAN
#else
#	if __BYTE_ORDER == __LITTLE_ENDIAN
#		define QB_LITTLE_ENDIAN
#	else
#		define QB_BIG_ENDIAN
#	endif
#endif

#include "php.h"
#include "php_qb.h"
#include "ext/standard/php_rand.h"
#include "SAPI.h"

#ifdef ZEND_ACC_GENERATOR
#	include "zend_generators.h"
#endif

#if PHP_MAJOR_VERSION == 5 && PHP_MINOR_VERSION == 1
#	define ZEND_ENGINE_2_1		1
#elif PHP_MAJOR_VERSION == 5 && PHP_MINOR_VERSION == 2
#	define ZEND_ENGINE_2_2		1
#elif PHP_MAJOR_VERSION == 5 && PHP_MINOR_VERSION == 3
#	define ZEND_ENGINE_2_3		1
#elif PHP_MAJOR_VERSION == 5 && PHP_MINOR_VERSION == 4
#	define ZEND_ENGINE_2_4		1
#elif PHP_MAJOR_VERSION == 5 && PHP_MINOR_VERSION == 5
#	define ZEND_ENGINE_2_5		1
#elif PHP_MAJOR_VERSION == 5 && PHP_MINOR_VERSION == 6
#	define ZEND_ENGINE_2_6		1
#else
#	error Incompatible version of PHP
#endif

#ifdef ZTS
#	define USE_TSRM			void ***tsrm_ls = cxt->tsrm_ls;\

#	define SAVE_TSRMLS		cxt->tsrm_ls = tsrm_ls;\

#else
#	define USE_TSRM
#	define SAVE_TSRMLS
#endif

#ifdef _MSC_VER
#	define NO_RETURN	__declspec(noreturn)
#endif

#ifdef __GNUC__
#	define NO_RETURN	__attribute__ ((noreturn))
#endif

#ifdef __GNUC__
#	ifdef __linux__
#		if defined(__i386__) || defined(__x86_64__)
#			define NATIVE_COMPILE_ENABLED	1
#		elif defined(__ARM_ARCH_7A__)
#			define NATIVE_COMPILE_ENABLED	1
#		endif
#		elif defined(__MACH__)
#		if defined(__i386__) || defined(__x86_64__)
#			define NATIVE_COMPILE_ENABLED	1
#		endif
#	endif
#endif

#ifdef _MSC_VER
#	define NATIVE_COMPILE_ENABLED	1
#endif

#define QB_EXTNAME	"qb"

#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif

#include "qb_debug_interface.h"
#include "qb_version.h"
#include "qb_compat.h"
#include "qb_opcodes.h"
#include "qb_types.h"
#include "qb_op.h"
#include "qb_thread.h"
#include "qb_storage.h"
#include "qb_function.h"
#include "qb_exceptions.h"
#include "qb_parser.h"
#include "qb_compiler.h"
#include "qb_function_inliner.h"
#include "qb_op_factories.h"
#include "qb_translator_php.h"
#include "qb_translator_pbj.h"
#include "qb_encoder.h"
#include "qb_thread.h"
#include "qb_interpreter.h"
#include "qb_build.h"
#include "qb_native_compiler.h"
#include "qb_printer.h"
#include "qb_extractor.h"

enum {
	QB_SCAN_FILE				= 0,
	QB_SCAN_ALL					= 1,
	QB_START_DEFERRAL			= 2,
	QB_END_DEFERRAL				= 3,
};

#ifdef ZEND_ACC_GENERATOR
typedef struct qb_generator_context			qb_generator_context;

struct qb_generator_context {
	zend_generator *generator;
	qb_interpreter_context *interpreter_context;
};
#endif

ZEND_BEGIN_MODULE_GLOBALS(qb)
	qb_main_thread main_thread;
	long thread_count;
	long debug_fork_id;

	zend_bool allow_bytecode_interpretation;
	zend_bool allow_native_compilation;
	zend_bool allow_memory_map;
	zend_bool compile_to_native;
	zend_bool allow_debugger_inspection;
	zend_bool allow_debug_backtrace;
	zend_bool debug_with_exact_type;
	zend_bool column_major_matrix;

	zend_bool show_opcodes;
	zend_bool show_native_source;
	zend_bool show_compiler_errors;
	zend_bool show_source_opcodes;

	char *compiler_path;
	char *compiler_env_path;
	char *native_code_cache_path;
	char *execution_log_path;

	qb_build_context *build_context;

	qb_import_scope **scopes;
	uint32_t scope_count;

	qb_external_symbol *external_symbols;
	uint32_t external_symbol_count;

	qb_interpreter_context *caller_interpreter_context;

	qb_exception *exceptions;
	uint32_t exception_count;

	const char **source_files;
	uint32_t source_file_count;

	qb_function **compiled_functions;
	uint32_t compiled_function_count;

#if !ZEND_ENGINE_2_3 && !ZEND_ENGINE_2_2 && !ZEND_ENGINE_2_1
	zend_literal static_zvals[8];
#else
	zval static_zvals[8];
#endif
	uint32_t static_zval_index;

#ifdef NATIVE_COMPILE_ENABLED
	qb_native_code_bundle *native_code_bundles;
	uint32_t native_code_bundle_count;
#endif

#ifdef ZEND_ACC_GENERATOR
	qb_generator_context *generator_contexts;
	uint32_t generator_context_count;
#endif

	double execution_start_time;
ZEND_END_MODULE_GLOBALS(qb)

#ifdef ZTS
#	define QB_G(v) TSRMG(qb_globals_id, zend_qb_globals *, v)
#else
#	define QB_G(v) (qb_globals.v)
#endif

int qb_run_diagnostics(qb_diagnostics *info TSRMLS_DC);

void qb_attach_compiled_function(qb_function *qfunc, zend_op_array *zop_array TSRMLS_DC);
qb_function * qb_get_compiled_function(zend_function *zfunc);
qb_function * qb_find_compiled_function(zend_function *zfunc TSRMLS_DC);
int qb_is_compiled_function(zend_function *zfunc);

qb_import_scope * qb_find_import_scope(qb_import_scope_type type, void *associated_object TSRMLS_DC);
qb_import_scope * qb_get_import_scope(qb_storage *storage, qb_variable *var, zval *object TSRMLS_DC);
qb_variable * qb_get_import_variable(qb_storage *storage, qb_variable *var, qb_import_scope *scope  TSRMLS_DC);

uint32_t qb_import_external_symbol(qb_external_symbol_type type, const char *name, uint32_t name_len, void *pointer TSRMLS_DC);

extern int debug_compatibility_mode;
extern long multithreading_threshold;

ZEND_EXTERN_MODULE_GLOBALS(qb)

#endif /* QB_H_ */
