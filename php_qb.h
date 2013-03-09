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

#ifndef PHP_QB_H
#define PHP_QB_H

extern zend_module_entry qb_module_entry;
#define phpext_qb_ptr &qb_module_entry

#ifdef PHP_WIN32
#	define PHP_QB_API __declspec(dllexport)
#elif defined(__GNUC__) && __GNUC__ >= 4
#	define PHP_QB_API __attribute__ ((visibility("default")))
#else
#	define PHP_QB_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

PHP_MINIT_FUNCTION(qb);
PHP_MSHUTDOWN_FUNCTION(qb);
PHP_RINIT_FUNCTION(qb);
PHP_RSHUTDOWN_FUNCTION(qb);
PHP_MINFO_FUNCTION(qb);

PHP_FUNCTION(qb_compile);
PHP_FUNCTION(qb_extract);

/* 
  	Declare any global variables you may need between the BEGIN
	and END macros here:     

ZEND_BEGIN_MODULE_GLOBALS(qb)
	long  global_value;
	char *global_string;
ZEND_END_MODULE_GLOBALS(qb)
*/

/* In every utility function you add that needs to use variables 
   in php_qb_globals, call TSRMLS_FETCH(); after declaring other 
   variables used by that function, or better yet, pass in TSRMLS_CC
   after the last function argument and declare your utility function
   with TSRMLS_DC after the last declared argument.  Always refer to
   the globals in your function as QB_G(variable).  You are 
   encouraged to rename these macros something shorter, see
   examples in any other php module directory.
*/

#ifdef ZTS
#define QB_G(v) TSRMG(qb_globals_id, zend_qb_globals *, v)
#else
#define QB_G(v) (qb_globals.v)
#endif

#endif	/* PHP_QB_H */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
