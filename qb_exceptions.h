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

#ifndef QB_EXCEPTIONS_H_
#define QB_EXCEPTIONS_H_

void qb_record_out_of_bound_exception(uint32_t index, uint32_t limit, int32_t inclusive, uint32_t line_id TSRMLS_DC);
void qb_record_missing_column_exception(uint32_t column_offset, uint32_t column_count, uint32_t line_id TSRMLS_DC);
void qb_record_divide_by_zero_exception(uint32_t numerator, uint32_t line_id TSRMLS_DC);

#endif
