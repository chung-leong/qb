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

// ZEND_VERSION is a C-string and it isn't possible to do string comparison in the preprocessor
// detect the version of Zend Engines by checking for the absence of certain macros instead
#if !defined(ZEND_FETCH_RESOURCE_NO_RETURN)
	#define ZEND_ENGINE_2_1		1
#elif !defined(PHP_FE_END)
	#define ZEND_ENGINE_2_2		1
#elif !defined(Z_HASH_P)
	#define ZEND_ENGINE_2_3		1
#elif !defined(EX_CV_NUM)
	#define ZEND_ENGINE_2_4		1
#else
	#define ZEND_ENGINE_2_5		1
#endif

#ifdef ZTS
	#define USE_TSRM		void ***tsrm_ls = cxt->tsrm_ls;\

	#define SAVE_TSRMLS()	cxt->tsrm_ls = tsrm_ls\

#else
	#define USE_TSRM
	#define SAVE_TSRMLS()
#endif

#ifdef _MSC_VER
	#define NO_RETURN	__declspec(noreturn)
#endif

#ifdef __GNUC__
	#define NO_RETURN	__attribute__ ((noreturn))
#endif

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

#ifdef _MSC_VER
	#define NATIVE_COMPILE_ENABLED	1
#endif

#ifndef ZEND_FASTCALL
	#if defined(__GNUC__) && ZEND_GCC_VERSION >= 3004 && defined(__i386__)
		# define ZEND_FASTCALL __attribute__((fastcall))
	#elif defined(_MSC_VER) && defined(_M_IX86)
		# define ZEND_FASTCALL __fastcall
	#else
		# define ZEND_FASTCALL
	#endif
#endif

#define QB_EXTNAME	"qb"
#define QB_EXTVER	"1.0"

#include "qb_version.h"
#include "qb_compat.h"
#include "qb_types.h"
#include "qb_compiler.h"
#include "qb_interpreter.h"
#include "qb_native_compiler.h"
#include "qb_extractor.h"
#include "qb_opcodes.h"

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
ZEND_END_MODULE_GLOBALS(qb)

#ifdef ZTS
# define QB_G(v) TSRMG(qb_globals_id, zend_qb_globals *, v)
#else
# define QB_G(v) (qb_globals.v)
#endif

int ZEND_FASTCALL qb_compile(zval *arg1, zval *arg2 TSRMLS_DC);
int ZEND_FASTCALL qb_native_compile(TSRMLS_D);
int ZEND_FASTCALL qb_execute(zend_function *function, zval *this, zval ***arguments, int argument_count, zval *return_value TSRMLS_DC);
int ZEND_FASTCALL qb_extract(zval *input, int output_type, zval *return_value TSRMLS_DC);
int ZEND_FASTCALL qb_run_diagnostics(qb_diagnostics *info TSRMLS_DC);

int ZEND_FASTCALL qb_initialize_compiler(TSRMLS_D);
int ZEND_FASTCALL qb_initialize_interpreter(TSRMLS_D);

ZEND_ATTRIBUTE_FORMAT(printf, 1, 2) NO_RETURN 
void qb_abort(const char *format, ...);

void ZEND_FASTCALL qb_free_function(qb_function *qfunc);
void ZEND_FASTCALL qb_free_native_code(qb_native_code_bundle *bundle);

ZEND_EXTERN_MODULE_GLOBALS(qb)

#endif /* QB_H_ */
