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

#ifndef __QB_COMPAT_H
#define __QB_COMPAT_H

#if ZEND_ENGINE_2_3 || ZEND_ENGINE_2_2 || ZEND_ENGINE_2_1
	#define znode_op		znode
#endif

#if ZEND_ENGINE_2_2 || ZEND_ENGINE_2_1
	#define array_init_size(arg, size)		array_init(arg)

	#define PHP_ROUND_HALF_UP		0x01    /* Arithmetic rounding, up == away from zero */
	#define PHP_ROUND_HALF_DOWN		0x02    /* Down == towards zero */
	#define PHP_ROUND_HALF_EVEN		0x03    /* Banker's rounding */
	#define PHP_ROUND_HALF_ODD		0x04

	extern char *(*zend_resolve_path)(const char *filename, int filename_len TSRMLS_DC);

	#if ZEND_ENGINE_2_1
		#define ZEND_FETCH_RESOURCE_NO_RETURN(rsrc, rsrc_type, passed_id, default_id, resource_type_name, resource_type)	\
			(rsrc = (rsrc_type) zend_fetch_resource(passed_id TSRMLS_CC, default_id, resource_type_name, NULL, 1, resource_type))
		char *zend_get_type_by_const(int type);
	#endif
#endif

#ifndef zend_always_inline
	#if defined(__GNUC__)
		#if __GNUC__ >= 3
			#define zend_always_inline inline __attribute__((always_inline))
		#else
			#define zend_always_inline inline
		#endif
	#elif defined(_MSC_VER)
		#define zend_always_inline __forceinline
	#else
		#define zend_always_inline inline
	#endif
#endif

#ifndef EXPECTED
	#if (defined (__GNUC__) && __GNUC__ > 2 ) && !defined(DARWIN) && !defined(__hpux) && !defined(_AIX)
		#define EXPECTED(condition)   __builtin_expect(condition, 1)
		#define UNEXPECTED(condition) __builtin_expect(condition, 0)
	#else
		#define EXPECTED(condition)   (condition)
		#define UNEXPECTED(condition) (condition)
	#endif
#endif

#if defined(__GNUC__) && !defined(__builtin_bswap16)
static inline unsigned short __builtin_bswap16(unsigned short v) {
  return (v << 8) | (v >> 8);
}
#endif

#ifndef M_E
	#define M_E				2.7182818284590452354   /* e */
#endif

#ifdef _MSC_VER
#define hypot		_hypot
#define hypotf		_hypotf
#define llabs		_abs64
double asinh(double z);
double acosh(double x);
double atanh(double z);
double log1p(double x);
double expm1(double x);
double log2(double x);
double exp2(double x);
double round(double x);
#endif

#if !ZEND_ENGINE_2_2 && !ZEND_ENGINE_2_1
PHPAPI double _php_math_round(double value, int places, int mode);
#else
double _php_math_round(double value, int places, int mode);
#endif

#ifndef TRUE
	#define TRUE			1
#endif

#ifndef FALSE
	#define FALSE			0
#endif

#ifndef min
	#define min(a, b)		((a < b) ? a : b)
#endif
#ifndef max
	#define max(a, b)		((a > b) ? a : b)
#endif

#ifndef INFINITY
	#ifdef _MSC_VER
		#define INFINITY	(DBL_MAX + DBL_MAX)
	#else
		#define INFINITY	(1.0/0.0)
	#endif
#endif

#ifndef NAN
	#ifdef _MSC_VER
		#define NAN			(INFINITY - INFINITY) 
	#else
		#define NAN			(1.0/0.0)
	#endif
#endif

#ifndef restrict			
	#define	restrict		__restrict
#endif

#ifdef _MSC_VER
	#define strtoull	_strtoui64
#endif

#if ZEND_ENGINE_2_1
#define zend_strtod					__zend_strtod
#define ap_php_snprintf				__ap_php_snprintf
#define ap_php_vsnprintf			__ap_php_vsnprintf
#define ap_php_vasprintf			__ap_php_vasprintf
#define ap_php_asprintf				__ap_php_asprintf
#define vasprintf					__ap_php_vasprintf
#define asprintf					__ap_php_asprintf
#undef snprintf
#define snprintf					__ap_php_snprintf

int ap_php_snprintf(char *buf, size_t len, const char *format,...);
int ap_php_vsnprintf(char *buf, size_t len, const char *format, va_list ap);
double zend_strtod(const char *s00, char **se);
#endif

#ifdef VC6_MSVCRT

extern FILE *	(*vc6_fdopen)(int _FileHandle, const char * _Mode);
extern int		(*vc6_open_osfhandle)(intptr_t _OSFileHandle, int _Flags);
extern FILE *	(*vc6_fopen)(const char * _Filename, const char * _Mode);
extern int		(*vc6_fclose)(FILE * _File);
extern size_t	(*vc6_fread)(void * _DstBuf, size_t _ElementSize, size_t _Count, FILE * _File);
extern size_t	(*vc6_fwrite)(const void * _Str, size_t _Size, size_t _Count, FILE * _File);
extern int		(*vc6_fprintf)(FILE * _File, const char * _Format, ...);
extern int		(*vc6_vfprintf)(FILE * _File, const char * _Format, va_list _ArgList);
extern void *	(*vc6_malloc)(size_t _Size);
extern void		(*vc6_free)(void * _Memory);
extern char *	(*vc6_strdup)(const char * _Src);

#define _fdopen				vc6_fdopen
#define _open_osfhandle		vc6_open_osfhandle
#define fopen				vc6_fopen
#define fclose				vc6_fclose
#define fread				vc6_fread
#define fwrite				vc6_fwrite
#define fprintf				vc6_fprintf
#define vfprintf			vc6_vfprintf
#undef malloc
#define malloc				vc6_malloc
#undef free
#define free				vc6_free
#undef strdup
#define strdup				vc6_strdup

int ZEND_FASTCALL qb_get_vc6_msvcrt_functions(void);

#endif

#endif
