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

#ifdef _MSC_VER
double asinh(double z) {
	return(log(z + sqrt(1 + pow(z, 2))) / log(M_E));
}

double acosh(double x) {
	return(log(x + sqrt(x * x - 1)));
}

double atanh(double z) {
	return(0.5 * log((1 + z) / (1 - z)));
}

double log1p(double x) {
	if(fabs(x) > 1e-4) {

		return log(1.0 + x);

	} else {

		return (-0.5 * x + 1.0) * x;
	}
}

double expm1(double x) {
	if(fabs(x) < 1e-5) {

		return x + 0.5 * x * x;

	} else {

		return exp(x) - 1.0;
	}
}

double log2(double x) {
	return log(x) * 1.4426950408889634;
}

double exp2(double x) {
	return pow(2, x);
}

double round(double x) {
	return floor(x + 0.5);
}
#endif

#ifdef __GNUC__
#ifndef HAVE_SINCOS
// gcc intrinsic functions--just make the symbols available if they doesn't exist
void sincos() {
}

void sincosf() {
}
#endif
#endif

#if ZEND_ENGINE_2_2 || ZEND_ENGINE_2_1
/* {{{ php_intlog10abs
   Returns floor(log10(fabs(val))), uses fast binary search */
static inline int php_intlog10abs(double value) {
	int result;
	value = fabs(value);

	if (value < 1e-8 || value > 1e22) {
		result = (int)floor(log10(value));
	} else {
		static const double values[] = {
			1e-8, 1e-7, 1e-6, 1e-5, 1e-4, 1e-3, 1e-2, 1e-1,
			1e0,  1e1,  1e2,  1e3,  1e4,  1e5,  1e6,  1e7,
			1e8,  1e9,  1e10, 1e11, 1e12, 1e13, 1e14, 1e15,
			1e16, 1e17, 1e18, 1e19, 1e20, 1e21, 1e22};
		/* Do a binary search with 5 steps */
		result = 15;
		if (value < values[result]) {
			result -= 8;
		} else {
			result += 8;
		}
		if (value < values[result]) {
			result -= 4;
		} else {
			result += 4;
		}
		if (value < values[result]) {
			result -= 2;
		} else {
			result += 2;
		}
		if (value < values[result]) {
			result -= 1;
		} else {
			result += 1;
		}
		if (value < values[result]) {
			result -= 1;
		}
		result -= 8;
	}
	return result;
}
/* }}} */

/* {{{ php_intpow10
       Returns pow(10.0, (double)power), uses fast lookup table for exact powers */
static inline double php_intpow10(int power) {
	static const double powers[] = {
		1e0,  1e1,  1e2,  1e3,  1e4,  1e5,  1e6,  1e7,
		1e8,  1e9,  1e10, 1e11, 1e12, 1e13, 1e14, 1e15,
		1e16, 1e17, 1e18, 1e19, 1e20, 1e21, 1e22};

	/* Not in lookup table */
	if (power < 0 || power > 22) {
		return pow(10.0, (double)power);
	}
	return powers[power];
}
/* }}} */

/* {{{ php_math_is_finite */
static inline int php_math_is_finite(double value) {
#if defined(PHP_WIN32)
	return _finite(value);
#elif defined(isfinite)
	return isfinite(value);
#else
	return value == value && (value == 0. || value * 2. != value);
#endif
}
/* }}} */

/* {{{ php_round_helper
       Actually performs the rounding of a value to integer in a certain mode */
static inline double php_round_helper(double value, int mode) {
	double tmp_value;

	if (value >= 0.0) {
		tmp_value = floor(value + 0.5);
		if ((mode == PHP_ROUND_HALF_DOWN && value == (-0.5 + tmp_value)) ||
			(mode == PHP_ROUND_HALF_EVEN && value == (0.5 + 2 * floor(tmp_value/2.0))) ||
			(mode == PHP_ROUND_HALF_ODD  && value == (0.5 + 2 * floor(tmp_value/2.0) - 1.0)))
		{
			tmp_value = tmp_value - 1.0;
		}
	} else {
		tmp_value = ceil(value - 0.5);
		if ((mode == PHP_ROUND_HALF_DOWN && value == (0.5 + tmp_value)) ||
			(mode == PHP_ROUND_HALF_EVEN && value == (-0.5 + 2 * ceil(tmp_value/2.0))) ||
			(mode == PHP_ROUND_HALF_ODD  && value == (-0.5 + 2 * ceil(tmp_value/2.0) + 1.0)))
		{
			tmp_value = tmp_value + 1.0;
		}
	}

	return tmp_value;
}
/* }}} */

/* {{{ _php_math_round */
/*
 * Rounds a number to a certain number of decimal places in a certain rounding
 * mode. For the specifics of the algorithm, see http://wiki.php.net/rfc/rounding
 */
double _php_math_round(double value, int places, int mode) {
	double f1, f2;
	double tmp_value;
	int precision_places;

	if (!php_math_is_finite(value)) {
		return value;
	}

	precision_places = 14 - php_intlog10abs(value);

	f1 = php_intpow10(abs(places));

	/* If the decimal precision guaranteed by FP arithmetic is higher than
	   the requested places BUT is small enough to make sure a non-zero value
	   is returned, pre-round the result to the precision */
	if (precision_places > places && precision_places - places < 15) {
		f2 = php_intpow10(abs(precision_places));
		if (precision_places >= 0) {
			tmp_value = value * f2;
		} else {
			tmp_value = value / f2;
		}
		/* preround the result (tmp_value will always be something * 1e14,
		   thus never larger than 1e15 here) */
		tmp_value = php_round_helper(tmp_value, mode);
		/* now correctly move the decimal point */
		f2 = php_intpow10(abs(places - precision_places));
		/* because places < precision_places */
		tmp_value = tmp_value / f2;
	} else {
		/* adjust the value */
		if (places >= 0) {
			tmp_value = value * f1;
		} else {
			tmp_value = value / f1;
		}
		/* This value is beyond our precision, so rounding it is pointless */
		if (fabs(tmp_value) >= 1e15) {
			return value;
		}
	}

	/* round the temp value */
	tmp_value = php_round_helper(tmp_value, mode);

	/* see if it makes sense to use simple division to round the value */
	if (abs(places) < 23) {
		if (places > 0) {
			tmp_value = tmp_value / f1;
		} else {
			tmp_value = tmp_value * f1;
		}
	} else {
		/* Simple division can't be used since that will cause wrong results.
		   Instead, the number is converted to a string and back again using
		   strtod(). strtod() will return the nearest possible FP value for
		   that string. */

		/* 40 Bytes should be more than enough for this format string. The
		   float won't be larger than 1e15 anyway. But just in case, use
		   snprintf() and make sure the buffer is zero-terminated */
		char buf[40];
		snprintf(buf, 39, "%15fe%d", tmp_value, -places);
		buf[39] = '\0';
		tmp_value = zend_strtod(buf, NULL);
		/* couldn't convert to string and back */
		if (!zend_finite(tmp_value) || zend_isnan(tmp_value)) {
			tmp_value = value;
		}
	}

	return tmp_value;
}
/* }}} */


#if ZEND_ENGINE_2_1
char *zend_get_type_by_const(int type)

{

	switch(type) {

		case IS_BOOL:

			return "boolean";

		case IS_LONG:

			return "integer";

		case IS_DOUBLE:

			return "double";

		case IS_STRING:

			return "string";

		case IS_OBJECT:

			return "object";

		case IS_RESOURCE:

			return "resource";

		case IS_NULL:

			return "null";

		case IS_ARRAY:

			return "array";

		default:

			return "unknown";

	}



	

}
#include "qb_compat_snprintf.c"
#endif

#endif	// ZEND_ENGINE_2_2 || ZEND_ENGINE_2_1

#ifdef VC6_MSVCRT

FILE *	(*vc6_fdopen)(int _FileHandle, const char * _Mode);

int		(*vc6_open_osfhandle)(intptr_t _OSFileHandle, int _Flags);

FILE *	(*vc6_fopen)(const char * _Filename, const char * _Mode);

int		(*vc6_fclose)(FILE * _File);

size_t	(*vc6_fread)(void * _DstBuf, size_t _ElementSize, size_t _Count, FILE * _File);

size_t	(*vc6_fwrite)(const void * _Str, size_t _Size, size_t _Count, FILE * _File);

int		(*vc6_fprintf)(FILE * _File, const char * _Format, ...);

int		(*vc6_vfprintf)(FILE * _File, const char * _Format, va_list _ArgList);

void *	(*vc6_malloc)(size_t _Size);

void	(*vc6_free)(void * _Memory);

char *	(*vc6_strdup)(const char * _Src);



int ZEND_FASTCALL qb_get_vc6_msvcrt_functions(void) {

	HMODULE lib = GetModuleHandle("MSVCRT.DLL");

	if(!lib) {

		return FAILURE;

	}

	vc6_fdopen = (void *) GetProcAddress(lib, "_fdopen");

	vc6_open_osfhandle = (void *) GetProcAddress(lib, "_open_osfhandle");

	vc6_fopen = (void *) GetProcAddress(lib, "fopen");

	vc6_fclose = (void *) GetProcAddress(lib, "fclose");

	vc6_fread = (void *) GetProcAddress(lib, "fread");

	vc6_fwrite = (void *) GetProcAddress(lib, "fwrite");

	vc6_fprintf = (void *) GetProcAddress(lib, "fprintf");

	vc6_vfprintf = (void *) GetProcAddress(lib, "vfprintf");

	vc6_malloc = (void *) GetProcAddress(lib, "malloc");

	vc6_free = (void *) GetProcAddress(lib, "free");

	vc6_strdup = (void *) GetProcAddress(lib, "_strdup");

	return SUCCESS;

}



#endif
