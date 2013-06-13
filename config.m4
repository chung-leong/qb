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
  
  dnl See if compile support -march=native 
  `echo "" | $CC -fsyntax-only -march=native -xc - 2> /dev/null`
  if [[ $? -eq 0 ]]; then
    AC_DEFINE(HAVE_GCC_MARCH_NATIVE,1,[ ])
  fi  

  AC_MSG_CHECKING([for sincos])  
  `echo "void sincos(double x, double *ps, double *pc); int main(int argc, const char *argv[]) { sincos(0, 0, 0); return 0; }" | $CC -o sincos -xc - 2> /dev/null`
  if [[ $? -eq 0 ]]; then
    AC_DEFINE(HAVE_SINCOS,1,[ ])
    `rm -f sincos`
    AC_MSG_RESULT([yes])
  else
    AC_MSG_RESULT([no])
  fi

  AC_MSG_CHECKING([for __builtin_bswap16])  
  `echo "short __builtin_bswap16(short a) { return a; }" | $CC -o bswap16.o -xc -c - 2> /dev/null`
  if [[ $? -eq 0 ]]; then
    `rm -f bswap16.o`
    AC_MSG_RESULT([no])
  else
    AC_DEFINE(HAVE_BUILTIN_BSWAP16,1,[ ])
    AC_MSG_RESULT([yes])
  fi

  AC_CHECK_FUNCS([roundf exp2f log2f])
    
  PHP_SUBST(QB_SHARED_LIBADD)
  case $host_alias in
  *darwin*)
    ;;
  *)
    PHP_ADD_LIBRARY(rt,, QB_SHARED_LIBADD)
    ;;
  esac

  extra_sources="qb_types.c qb_compiler.c qb_native_compiler.c qb_interpreter.c qb_interpreter_gcc.c qb_extractor.c qb_data_tables_gcc.c qb_compat.c"

  PHP_NEW_EXTENSION(qb, qb.c $extra_sources, $ext_shared, , $qb_cflags)
fi
