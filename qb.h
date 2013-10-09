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

#include <limits.h>
#include <stdlib.h>
#include <stddef.h>
#include <math.h>

#ifdef _MSC_VER
	#include "win32\php_stdint.h"
	#include "win32\msc_inttypes.h"
#else
	#include <stdint.h>
	#include <inttypes.h>
	#include <sys/types.h>
	#include <sys/param.h>
#endif

#ifdef _MSC_VER
	#define QB_LITTLE_ENDIAN
#else
	#if __BYTE_ORDER == __LITTLE_ENDIAN
		#define QB_LITTLE_ENDIAN
	#else
		#define QB_BIG_ENDIAN
	#endif
#endif

#include "php.h"
#include "ext/standard/php_rand.h"

#if PHP_MAJOR_VERSION == 5 && PHP_MINOR_VERSION == 1
	#define ZEND_ENGINE_2_1		1
#elif PHP_MAJOR_VERSION == 5 && PHP_MINOR_VERSION == 2
	#define ZEND_ENGINE_2_2		1
#elif PHP_MAJOR_VERSION == 5 && PHP_MINOR_VERSION == 3
	#define ZEND_ENGINE_2_3		1
#elif PHP_MAJOR_VERSION == 5 && PHP_MINOR_VERSION == 4
	#define ZEND_ENGINE_2_4		1
#elif PHP_MAJOR_VERSION == 5 && PHP_MINOR_VERSION == 5
	#define ZEND_ENGINE_2_5		1
#else
	#error Incompatible version of PHP
#endif

#ifdef ZTS
	#define USE_TSRM		void ***tsrm_ls = cxt->tsrm_ls;\

	#define SAVE_TSRMLS		cxt->tsrm_ls = tsrm_ls;\

#else
	#define USE_TSRM
	#define SAVE_TSRMLS
#endif

#ifdef _MSC_VER
	#define NO_RETURN	__declspec(noreturn)
#endif

#ifdef __GNUC__
	#define NO_RETURN	__attribute__ ((noreturn))
#endif

/*
#ifdef __GNUC__
	#ifdef __linux__
		#if defined(__i386__) || defined(__x86_64__)
			#define NATIVE_COMPILE_ENABLED	1
		#endif
	#elif defined(__MACH__)
		#if defined(__i386__) || defined(__x86_64__)
			#define NATIVE_COMPILE_ENABLED	1

		#endif
	#endif
#endif
*/

/*
#ifdef _MSC_VER
	#define NATIVE_COMPILE_ENABLED	1
#endif
*/

#define QB_EXTNAME	"qb"

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "qb_version.h"
#include "qb_compat.h"
#include "qb_opcodes.h"
#include "qb_types.h"
#include "qb_op.h"
#include "qb_storage.h"
#include "qb_function.h"
#include "qb_thread.h"
#include "qb_parser.h"
#include "qb_compiler.h"
#include "qb_op_factories.h"
#include "qb_translater_php.h"
#include "qb_encoder.h"
#include "qb_interpreter.h"
#include "qb_native_compiler.h"
#include "qb_printer.h"
#include "qb_extractor.h"
#include "qb_build.h"

enum {
	QB_SCAN_FILE				= 0,
	QB_SCAN_ALL					= 1,
	QB_START_DEFERRAL			= 2,
	QB_END_DEFERRAL				= 3,
};

ZEND_BEGIN_MODULE_GLOBALS(qb)
	const char *current_filename;
	uint32_t current_line_number;

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
	zend_bool show_zend_opcodes;
	zend_bool show_pbj_opcodes;

	char *compiler_path;
	char *compiler_env_path;
	char *native_code_cache_path;
	char *execution_log_path;

	qb_build_context *build_context;
	qb_interpreter_context *interpreter_context;

	zval static_zvals[8];
	uint32_t static_zval_index;

	qb_function **compiled_functions;
	uint32_t compiled_function_count;

	qb_native_code_bundle *native_code_bundles;
	uint32_t native_code_bundle_count;

	qb_thread_pool thread_pool;
ZEND_END_MODULE_GLOBALS(qb)

#ifdef ZTS
# define QB_G(v) TSRMG(qb_globals_id, zend_qb_globals *, v)
#else
# define QB_G(v) (qb_globals.v)
#endif

int qb_extract(zval *input, int output_type, zval *return_value TSRMLS_DC);
int qb_run_diagnostics(qb_diagnostics *info TSRMLS_DC);

int qb_initialize_compiler(TSRMLS_D);
int qb_initialize_interpreter(TSRMLS_D);

void qb_attach_compiled_function(qb_function *qfunc, zend_op_array *zop_array);
qb_function * qb_get_compiled_function(zend_function *zfunc);
int qb_is_compiled_function(zend_function *zfunc);

ZEND_ATTRIBUTE_FORMAT(printf, 1, 2) NO_RETURN 
void qb_abort(const char *format, ...);

extern int qb_user_opcode;
extern int qb_reserved_offset;

ZEND_EXTERN_MODULE_GLOBALS(qb)

#endif /* QB_H_ */
