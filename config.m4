dnl $Id$
dnl config.m4 for extension qb

PHP_ARG_ENABLE(qb, whether to enable qb support,
[  --enable-qb           Enable qb support])

PHP_ARG_WITH(sse, whether to use SSE instructions on x86 processors,
[  --with-sse[=VER]          Enable SSE optimization.  VER is the SSE version number])

if test "$PHP_QB" != "no"; then
  if test "$PHP_SSE" != "no"; then
    sse_flags="-msse$PHP_SSE"  
  fi

  extra_sources="qb_compiler.c qb_interpreter.c qb_native_compiler.c qb_types.c qb_data_tables.c"

  PHP_NEW_EXTENSION(qb, qb.c $extra_sources, $ext_shared, , $sse_flags)
fi
