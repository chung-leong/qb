dnl $Id$
dnl config.m4 for extension qb

PHP_ARG_ENABLE(qb, whether to enable qb support,
[  --enable-qb           Enable qb support])

PHP_ARG_WITH(sse, whether to use SSE instructions on x86 processors,
[  --with-sse[=VER]          Enable SSE optimization.  VER is the SSE version], no, no)

PHP_ARG_WITH(avx, whether to use AVX instructions on x86 processors,
[  --with-avx[=VER]          Enable AVX optimization.  VER is the AVX version], no, no)

PHP_ARG_WITH(cpu, whether to enable CPU-specific optimization,
[  --with-cpu[=arch]         Enable optimization specific to CPU archecture], no, no)

if test "$PHP_QB" != "no"; then
  qb_cflags=""

  if test "$PHP_SSE" != "no"; then
  	if test "$PHP_SSE" == "1" || test "$PHP_SSE" == "yes"; then
    	qb_cflags="$qb_cflags -msse"
    else  
    	qb_cflags="$qb_cflags -msse$PHP_SSE"
    fi
  fi
  if test "$PHP_AVX" != "no"; then
  	if test "$PHP_AVX" == "1" || test "$PHP_AVX" == "yes"; then
    	qb_cflags="$qb_cflags -mavx"
    else  
    	qb_cflags="$qb_cflags -mavx$PHP_AVX"
    fi
  fi
  if test "$PHP_CPU" != "no"; then
  	if test "$PHP_CPU" == "yes"; then
    	qb_cflags="$qb_cflags -march=native"
    else  
    	qb_cflags="$qb_cflags -march=$PHP_CPU"
    fi
  fi

  AC_SEARCH_LIBS(exp,m,[ ])
 
  AC_MSG_CHECKING([whether CC supports -march=native])
  ac_saved_cflags="$CFLAGS"
  CFLAGS=-march=native
  AC_COMPILE_IFELSE(
    [ AC_LANG_PROGRAM([]) ],
    [ 
      AC_DEFINE(HAVE_GCC_MARCH_NATIVE,1,[ ])
      AC_MSG_RESULT([yes]) 
    ],
    [ AC_MSG_RESULT([no])] 
  )
  CFLAGS="$ac_saved_cflags"
  
  AC_CHECK_FUNCS([exp2f log2f roundf qsort_r sincos cexp cexpf])

  ac_saved_cflags="$CFLAGS"
  CFLAGS="$CFLAGS -Werror"
  AC_MSG_CHECKING([for __builtin_bswap16])  
  AC_TRY_COMPILE(
    [ void __builtin_bswap16(void); ], 
    [ ], 
    [ AC_MSG_RESULT([no]) ],
    [ 
      AC_DEFINE(HAVE_BUILTIN_BSWAP16,1,[ ])
      AC_MSG_RESULT([yes])
    ]
  )

  AC_MSG_CHECKING([for __builtin_bswap32])  
  AC_TRY_COMPILE(
    [ void __builtin_bswap32(void); ], 
    [ ], 
    [ AC_MSG_RESULT([no]) ],
    [ 
      AC_DEFINE(HAVE_BUILTIN_BSWAP32,1,[ ])
      AC_MSG_RESULT([yes])
    ]
  )

  AC_MSG_CHECKING([for __builtin_bswap64])  
  AC_TRY_COMPILE(
    [ void __builtin_bswap64(void); ], 
    [ ], 
    [ AC_MSG_RESULT([no]) ],
    [ 
      AC_DEFINE(HAVE_BUILTIN_BSWAP64,1,[ ])
      AC_MSG_RESULT([yes])
    ]
  )
  CFLAGS="$ac_saved_cflags"

  PHP_SUBST(QB_SHARED_LIBADD)
  case $host_alias in
  *darwin*)
    ;;
  *)
    PHP_ADD_LIBRARY(rt,, QB_SHARED_LIBADD)
    ;;
  esac
  
  PHP_RUN_ONCE(EXTRA_LIBS, -pthread, [EXTRA_LIBS="$EXTRA_LIBS -pthread"])

  extra_sources="\
	qb_build.c\
	qb_compat.c\
	qb_crc64.c\
	qb_compiler.c\
	qb_data_tables_gcc.c\
	qb_debug_interface.c\
	qb_encoder.c\
	qb_exceptions.c\
	qb_extractor.c\
	qb_function_inliner.c\
	qb_interpreter.c\
	qb_interpreter_debug.c\
	qb_interpreter_functions_gcc.c\
	qb_interpreter_loop_gcc.c\
	qb_native_compiler.c\
	qb_op.c\
	qb_op_factories.c\
	qb_parser.c\
	qb_printer.c\
	qb_storage.c\
	qb_thread.c\
	qb_translator_pbj.c\
	qb_translator_php.c\
	qb_types.c\
	"
 
  PHP_NEW_EXTENSION(qb, qb.c $extra_sources, $ext_shared, , $qb_cflags)
fi
