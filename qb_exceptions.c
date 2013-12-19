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

void qb_record_out_of_bound_exception(uint32_t index, uint32_t limit, int32_t inclusive, uint32_t line_id TSRMLS_DC) {

}

void qb_record_missing_column_exception(uint32_t column_offset, uint32_t column_count, uint32_t line_id TSRMLS_DC) {

}

void qb_record_divide_by_zero_exception(uint32_t numerator, uint32_t line_id TSRMLS_DC) {

}

void qb_record_illegal_dimension_count_exception(uint32_t dimension_count TSRMLS_DC) {
	qb_abort("Array initializer has too many dimensions");
}

void qb_record_illegal_array_structure_exception(TSRMLS_D) {
	qb_abort("Array has irregular structure");
}

void qb_record_binary_string_size_mismatch_exception(uint32_t byte_count, qb_primitive_type type TSRMLS_DC) {
	qb_abort("Number of bytes in string (%d) is not divisible by %d", byte_count, type_sizes[type]);
}

void qb_record_illegal_array_conversion_exception(qb_primitive_type type TSRMLS_DC) {
	qb_abort("Cannot convert an array to %s", type_names[type]);

}

void qb_record_missing_type_declaration_exception(qb_variable *qvar TSRMLS_DC) {
	qb_abort("missing type declaration: %s", qvar->name);
}

void qb_record_external_code_load_failure_exception(const char *import_path TSRMLS_DC) {
	qb_abort("unable to load file containing external code");
}

void qb_record_illegal_dimension_declaration_exception(long const_value TSRMLS_DC) {
}

void qb_record_undefined_constant_in_dimension_declaration_exception(const char *name TSRMLS_DC) {

}

void qb_record_no_class_scope_exception(const char *scope_accessed TSRMLS_DC) {
	qb_abort("Cannot access %s:: when no class scope is active", scope_accessed);
}


void qb_record_missing_parent_scope_exception(const char *scope_accessed TSRMLS_DC) {
	qb_abort("Cannot access %s:: when current class scope has no parent", scope_accessed);
}

void qb_record_undefined_constant_exception(zend_class_entry *ce, const char *name TSRMLS_DC) {
	qb_abort("Undefined constant '%s'", name);
}

void qb_record_reference_exception(TSRMLS_D) {
	qb_abort("No support for reference");
}

void qb_record_intrinsic_expectation_unmet_exception(qb_intrinsic_function *ifunc, uint32_t param_index, const char *expected_object TSRMLS_DC) {
	qb_abort("%s() expects an %s as parameter", ifunc->name, expected_object);
}
