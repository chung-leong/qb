dnl $Id$
dnl config.m4 for extension qb

PHP_ARG_ENABLE(qb, whether to enable qb support,
[  --enable-qb           Enable qb support])

PHP_ARG_WITH(sse, whether to use SSE instructions on x86 processors,
[  --with-sse[=VER]          Enable SSE optimization.  VER is the SSE version], no, no)

PHP_ARG_WITH(avx, whether to use AVX instructions on x86 processors,
[  --with-avx[=VER]          Enable AVX optimization.  VER is the AVX version], no, no)

PHP_ARG_WITH(arch-native, whether to enable CPU-specific optimization,
[  --with-arch-native      Enable optimization specific to current CPU.], no, no)

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
  if test "$PHP_ARCH_NATIVE" != "no"; then
  	if test "$PHP_ARCH_NATIVE" == "yes"; then
    	qb_cflags="$qb_cflags -march=native"
    fi
  fi

  `echo "" | $CC -fsyntax-only -march=native -xc -`
  if [[ $? -eq 0 ]]; then
    AC_DEFINE(HAVE_GCC_MARCH_NATIVE,1,[ ])
  fi  

  PHP_CHECK_LIBRARY(c,sincos,
  [
    AC_DEFINE(HAVE_SINCOS,1,[ ])
  ],[
  ],[
  ])

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
