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
	#define znode_op				znode
	#define ZEND_FETCH_CLASS_MASK	0x0f
#endif

#if ZEND_ENGINE_2_2 || ZEND_ENGINE_2_1
	#define array_init_size(arg, size)		array_init(arg)

	#define PHP_ROUND_HALF_UP		0x01    /* Arithmetic rounding, up == away from zero */
	#define PHP_ROUND_HALF_DOWN		0x02    /* Down == towards zero */
	#define PHP_ROUND_HALF_EVEN		0x03    /* Banker's rounding */
	#define PHP_ROUND_HALF_ODD		0x04

	extern char *(*zend_resolve_path)(const char *filename, int filename_len TSRMLS_DC);

	#define Z_REFCOUNT_P(zv)		zv->refcount
	#define Z_ADDREF_P(zv)			zv->refcount++
	#define Z_DELREF_P(zv)			zv->refcount--

	#define INIT_CLASS_ENTRY_EX(class_container, class_name, class_name_len, functions) \
		{															\
			int _len = class_name_len;								\
			class_container.name = zend_strndup(class_name, _len);	\
			class_container.name_length = _len;						\
			INIT_CLASS_ENTRY_INIT_METHODS(class_container, functions, NULL, NULL, NULL, NULL, NULL) \
		}

	#if ZEND_ENGINE_2_2
		#define INIT_CLASS_ENTRY_INIT_METHODS(class_container, functions, handle_fcall, handle_propget, handle_propset, handle_propunset, handle_propisset) \
			{															\
				class_container.builtin_functions = functions;			\
				class_container.constructor = NULL;						\
				class_container.destructor = NULL;						\
				class_container.clone = NULL;							\
				class_container.serialize = NULL;						\
				class_container.unserialize = NULL;						\
				class_container.create_object = NULL;					\
				class_container.interface_gets_implemented = NULL;		\
				class_container.__call = handle_fcall;					\
				class_container.__tostring = NULL;						\
				class_container.__get = handle_propget;					\
				class_container.__set = handle_propset;					\
				class_container.__unset = handle_propunset;				\
				class_container.__isset = handle_propisset;				\
				class_container.serialize_func = NULL;					\
				class_container.unserialize_func = NULL;				\
				class_container.serialize = NULL;						\
				class_container.unserialize = NULL;						\
				class_container.parent = NULL;							\
				class_container.num_interfaces = 0;						\
				class_container.interfaces = NULL;						\
				class_container.get_iterator = NULL;					\
				class_container.iterator_funcs.funcs = NULL;			\
				class_container.module = NULL;							\
			}
	#else
		#define INIT_CLASS_ENTRY_INIT_METHODS(class_container, functions, handle_fcall, handle_propget, handle_propset, handle_propunset, handle_propisset) \
			{															\
				class_container.builtin_functions = functions;			\
				class_container.constructor = NULL;						\
				class_container.destructor = NULL;						\
				class_container.clone = NULL;							\
				class_container.serialize = NULL;						\
				class_container.unserialize = NULL;						\
				class_container.create_object = NULL;					\
				class_container.interface_gets_implemented = NULL;		\
				class_container.__call = handle_fcall;					\
				class_container.__get = handle_propget;					\
				class_container.__set = handle_propset;					\
				class_container.__unset = handle_propunset;				\
				class_container.__isset = handle_propisset;				\
				class_container.serialize_func = NULL;					\
				class_container.unserialize_func = NULL;				\
				class_container.serialize = NULL;						\
				class_container.unserialize = NULL;						\
				class_container.parent = NULL;							\
				class_container.num_interfaces = 0;						\
				class_container.interfaces = NULL;						\
				class_container.get_iterator = NULL;					\
				class_container.iterator_funcs.funcs = NULL;			\
				class_container.module = NULL;							\
			}
	#endif

	#if ZEND_ENGINE_2_1
		char *zend_get_type_by_const(int type);
		typedef uintptr_t zend_uintptr_t;
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

#if defined(__GNUC__) && !defined(HAVE_BUILTIN_BSWAP16)
static inline unsigned short __builtin_bswap16(unsigned short v) {
  return (v << 8) | (v >> 8);
}
#endif

#ifndef M_E
	#define M_E				2.7182818284590452354   /* e */
#endif

#ifdef _MSC_VER
	#if _M_IX86_FP == 2 
		#define __SSE2__		1
		#define __SSE__			1
	#elif _M_IX86_FP == 1 
		#define __SSE__			1
	#endif

	#if defined(_M_IX86)
		#define __i386__
	#elif defined(_M_X64)
		#define __x86_64__
	#endif

	#define hypot		_hypot
	#define hypotf		_hypotf
	#define llabs		_abs64
	#define isnan		_isnan

double asinh(double z);
double acosh(double x);
double atanh(double z);
double log1p(double x);
double expm1(double x);
double log2(double x);
double exp2(double x);
double round(double x);
double rsqrt(double x);

float rsqrtf(float x);

__forceinline float exp2f(float x) { return (float) exp2(x); }
__forceinline float expm1f(float x) { return (float) expm1(x); }
__forceinline float log2f(float x) { return (float) log2(x); }
__forceinline float log1pf(float x) { return (float) log1p(x); }
__forceinline float asinhf(float x) { return (float) asinh(x); }
__forceinline float acoshf(float x) { return (float) acosh(x); }
__forceinline float atanhf(float x) { return (float) atanh(x); }
__forceinline float roundf(float x) { return (float) round(x); }
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

#if defined(_MSC_VER) || ZEND_ENGINE_2_2 || ZEND_ENGINE_2_1
	// not sure why Zend doesn't use alloca in win32
	#undef ALLOCA_FLAG
	#undef SET_ALLOCA_FLAG
	#undef do_alloca
	#undef free_alloca
	#define ZEND_ALLOCA_MAX_SIZE (32 * 1024)
	#define ALLOCA_FLAG(name) \
		zend_bool name;
	#define SET_ALLOCA_FLAG(name) \
		name = 1
	#define do_alloca_ex(size, limit, use_heap) \
		((use_heap = (UNEXPECTED((size) > (limit)))) ? emalloc(size) : alloca(size))
	#define do_alloca(size, use_heap) \
		do_alloca_ex(size, ZEND_ALLOCA_MAX_SIZE, use_heap)
	#define free_alloca(p, use_heap) \
		do { if (UNEXPECTED(use_heap)) efree(p); } while (0)
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

#ifdef _MSC_VER
	void _ftol2(void);
	void _allshr(void);
	void _allshl(void);
#endif

#ifdef __GNUC__
	__extension__ extern long long int llabs (long long int __x);
	extern double round (double __x);
	extern double expm1 (double __x);
	extern double log1p (double __x);
	extern double exp2 (double __x);
	extern double log2 (double __x);
#endif

// the following is copied from the PHP source so we can build without the
// full source code--i.e. using just phpize

#ifndef RAND_MAX
#define RAND_MAX (1<<15)
#endif

#if !defined(ZTS) && (defined(HAVE_LRAND48) || defined(HAVE_RANDOM))
#define PHP_RAND_MAX 2147483647
#else
#define PHP_RAND_MAX RAND_MAX
#endif

#define PHP_MT_RAND_MAX ((long) (0x7FFFFFFF)) /* (1<<31) - 1 */

PHPAPI double php_combined_lcg(TSRMLS_D);
PHPAPI long php_rand(TSRMLS_D);
PHPAPI uint32_t php_mt_rand(TSRMLS_D);

#define gdMaxColors 256

#define gdAlphaMax 127
#define gdAlphaOpaque 0
#define gdAlphaTransparent 127
#define gdRedMax 255
#define gdGreenMax 255
#define gdBlueMax 255
#define gdTrueColorGetAlpha(c) (((c) & 0x7F000000) >> 24)
#define gdTrueColorGetRed(c) (((c) & 0xFF0000) >> 16)
#define gdTrueColorGetGreen(c) (((c) & 0x00FF00) >> 8)
#define gdTrueColorGetBlue(c) ((c) & 0x0000FF)

#define gdImagePalettePixel(im, x, y) (im)->pixels[(y)][(x)]
#define gdImageTrueColorPixel(im, x, y) (im)->tpixels[(y)][(x)]

typedef struct gdImageStruct {
	/* Palette-based image pixels */
	unsigned char ** pixels;
	int sx;
	int sy;
	/* These are valid in palette images only. See also
		'alpha', which appears later in the structure to
		preserve binary backwards compatibility */
	int colorsTotal;
	int red[gdMaxColors];
	int green[gdMaxColors];
	int blue[gdMaxColors];
	int open[gdMaxColors];
	/* For backwards compatibility, this is set to the
		first palette entry with 100% transparency,
		and is also set and reset by the
		gdImageColorTransparent function. Newer
		applications can allocate palette entries
		with any desired level of transparency; however,
		bear in mind that many viewers, notably
		many web browsers, fail to implement
		full alpha channel for PNG and provide
		support for full opacity or transparency only. */
	int transparent;
	int *polyInts;
	int polyAllocated;
	struct gdImageStruct *brush;
	struct gdImageStruct *tile;
	int brushColorMap[gdMaxColors];
	int tileColorMap[gdMaxColors];
	int styleLength;
	int stylePos;
	int *style;
	int interlace;
	/* New in 2.0: thickness of line. Initialized to 1. */
	int thick;
	/* New in 2.0: alpha channel for palettes. Note that only
		Macintosh Internet Explorer and (possibly) Netscape 6
		really support multiple levels of transparency in
		palettes, to my knowledge, as of 2/15/01. Most
		common browsers will display 100% opaque and
		100% transparent correctly, and do something
		unpredictable and/or undesirable for levels
		in between. TBB */
	int alpha[gdMaxColors];
	/* Truecolor flag and pixels. New 2.0 fields appear here at the
		end to minimize breakage of existing object code. */
	int trueColor;
	int ** tpixels;
	/* Should alpha channel be copied, or applied, each time a
		pixel is drawn? This applies to truecolor images only.
		No attempt is made to alpha-blend in palette images,
		even if semitransparent palette entries exist.
		To do that, build your image as a truecolor image,
		then quantize down to 8 bits. */
	int alphaBlendingFlag;
	/* Should antialias functions be used */
	int antialias;
	/* Should the alpha channel of the image be saved? This affects
		PNG at the moment; other future formats may also
		have that capability. JPEG doesn't. */
	int saveAlphaFlag;


	/* 2.0.12: anti-aliased globals */
	int AA;
	int AA_color;
	int AA_dont_blend;
	unsigned char **AA_opacity;
	int AA_polygon;
	/* Stored and pre-computed variables for determining the perpendicular
	 * distance from a point to the anti-aliased line being drawn:
	 */
	int AAL_x1;
	int AAL_y1;
	int AAL_x2;
	int AAL_y2;
	int AAL_Bx_Ax;
	int AAL_By_Ay;
	int AAL_LAB_2;
	float AAL_LAB;

	/* 2.0.12: simple clipping rectangle. These values must be checked for safety when set; please use gdImageSetClip */
	int cx1;
	int cy1;
	int cx2;
	int cy2;
} gdImage;

typedef gdImage * gdImagePtr;

#define gdTrueColorAlpha(r, g, b, a) (((a) << 24) + \
	((r) << 16) + \
	((g) << 8) + \
	(b))

typedef struct real_pcre pcre;

#define PCRE_CASELESS           0x00000001
#define PCRE_MULTILINE          0x00000002
#define PCRE_ANCHORED           0x00000010
#define PCRE_NOTEMPTY           0x00000400

#if defined(HAVE_BUNDLED_PCRE) || !defined(PHP_VERSION)
#define pcre_compile			php_pcre_compile
#define pcre_exec				php_pcre_exec
#endif

#ifndef PCRE_SPTR
#define PCRE_SPTR const char *
#endif

#if defined(_WIN32) && !defined(PCRE_STATIC)
#  ifndef PCRE_EXP_DECL
#    define PCRE_EXP_DECL  extern __declspec(dllimport)
#  endif
#  ifdef __cplusplus
#    ifndef PCRECPP_EXP_DECL
#      define PCRECPP_EXP_DECL  extern __declspec(dllimport)
#    endif
#    ifndef PCRECPP_EXP_DEFN
#      define PCRECPP_EXP_DEFN  __declspec(dllimport)
#    endif
#  endif
#endif

#ifndef PCRE_EXP_DECL
#  ifdef __cplusplus
#    define PCRE_EXP_DECL  extern "C"
#  else
#    define PCRE_EXP_DECL  extern
#  endif
#endif

#ifdef __cplusplus
#  ifndef PCRECPP_EXP_DECL
#    define PCRECPP_EXP_DECL  extern
#  endif
#  ifndef PCRECPP_EXP_DEFN
#    define PCRECPP_EXP_DEFN
#  endif
#endif

typedef struct pcre_extra {
  unsigned long int flags;        /* Bits for which fields are set */
  void *study_data;               /* Opaque data from pcre_study() */
  unsigned long int match_limit;  /* Maximum number of calls to match() */
  void *callout_data;             /* Data passed back in callouts */
  const unsigned char *tables;    /* Pointer to character tables */
  unsigned long int match_limit_recursion; /* Max recursive calls to match() */
  unsigned char **mark;           /* For passing back a mark pointer */
} pcre_extra;

PCRE_EXP_DECL pcre *pcre_compile(const char *, int, const char **, int *, const unsigned char *);
PCRE_EXP_DECL int  pcre_exec(const pcre *, const pcre_extra *, PCRE_SPTR, int, int, int, int *, int);

#endif
