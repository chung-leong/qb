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

typedef struct qb_exception				qb_exception;

struct qb_exception {
	char *message;
	const char *source_file;
	uint32_t line_number;
	int32_t type;
};

uint32_t qb_get_source_file_id(const char *file_path TSRMLS_DC);

void qb_dispatch_exceptions(TSRMLS_D);

void qb_record_external_code_load_failure_exception(qb_thread *thread, uint32_t line_id, const char *import_path);

// these functions may be called at runtime
void qb_record_out_of_bound_exception(qb_thread *thread, uint32_t line_id, uint32_t index, uint32_t limit, int32_t inclusive);
void qb_record_missing_column_exception(qb_thread *thread, uint32_t line_id, uint32_t column_offset, uint32_t column_count);
void qb_record_divide_by_zero_exception(qb_thread *thread, uint32_t line_id);
void qb_record_vector_width_mismatch_exception(qb_thread *thread, uint32_t line_id, uint32_t vector_width1, uint32_t vector_width2);
void qb_record_invalid_cross_product_exception(qb_thread *thread, uint32_t line_id, uint32_t vector_width1, uint32_t vector_width2);
void qb_record_invalid_4d_cross_product_exception(qb_thread *thread, uint32_t line_id, uint32_t vector_width1, uint32_t vector_width2, uint32_t vector_width3);
void qb_report_invalid_transform_exception(qb_thread *thread, uint32_t line_id, uint32_t matrix_column, uint32_t matrix_row, uint32_t vector_width);
void qb_record_invalid_matrix_multiplication_exception(qb_thread *thread, uint32_t line_id, uint32_t matrix1_column, uint32_t matrix2_row, uint32_t matrix_flags);

void qb_record_dimension_mismatch_exception(qb_thread *thread, uint32_t line_id, uint32_t dimension1, uint32_t dimension2);
void qb_record_dimension_count_mismatch_exception(qb_thread *thread, uint32_t line_id, uint32_t dimension1, uint32_t dimension2);
void qb_record_illegal_dimension_count_exception(qb_thread *thread, uint32_t line_id, uint32_t dimension_count);
void qb_record_illegal_array_structure_exception(qb_thread *thread, uint32_t line_id);
void qb_record_binary_string_size_mismatch_exception(qb_thread *thread, uint32_t line_id, uint32_t byte_count, qb_primitive_type type);

void qb_record_illegal_array_conversion_exception(qb_thread *thread, uint32_t line_id, qb_primitive_type type);
void qb_record_missing_type_declaration_exception(qb_thread *thread, uint32_t line_id, qb_variable *qvar);

void qb_record_illegal_dimension_declaration_exception(qb_thread *thread, uint32_t line_id, long const_value);
void qb_record_undefined_constant_in_dimension_declaration_exception(qb_thread *thread, uint32_t line_id, const char *name);

void qb_record_missing_scope_exception(qb_thread *thread, uint32_t line_id, const char *scope_accessed);
void qb_record_missing_parent_scope_exception(qb_thread *thread, uint32_t line_id, const char *scope_accessed);
void qb_record_undefined_constant_exception(qb_thread *thread, uint32_t line_id, zend_class_entry *ce, const char *name);
void qb_record_reference_exception(qb_thread *thread, uint32_t line_id);
void qb_record_not_an_array_exception(qb_thread *thread, uint32_t line_id);
void qb_record_illegal_array_index_exception(qb_thread *thread, uint32_t line_id);
void qb_record_associative_array_exception(qb_thread *thread, uint32_t line_id);
void qb_record_fixed_length_array_exception(qb_thread *thread, uint32_t line_id);
void qb_record_variable_variable_exception(qb_thread *thread, uint32_t line_id);
void qb_record_missing_property_exception(qb_thread *thread, uint32_t line_id, zend_class_entry *scope, const char *name);
void qb_record_missing_named_element_exception(qb_thread *thread, uint32_t line_id, const char *name);
void qb_record_elements_not_named_exception(qb_thread *thread, uint32_t line_id);
void qb_record_return_void_exception(qb_thread *thread, uint32_t line_id);
void qb_record_type_mismatch_exception(qb_thread *thread, uint32_t line_id, qb_primitive_type type1, qb_primitive_type type2);
void qb_record_missing_send_declaration(qb_thread *thread, uint32_t line_id);
void qb_record_ambiguous_return_value_exception(qb_thread *thread, uint32_t line_id);
void qb_record_missing_function_exception(qb_thread *thread, uint32_t line_id, const char *class_name, const char *name);
void qb_record_inline_function_exception(qb_thread *thread, uint32_t line_id, const char *class_name, const char *name);

void qb_record_incorrect_argument_count_exception(qb_thread *thread, uint32_t line_id, qb_intrinsic_function *ifunc, uint32_t argument_count);
void qb_record_unexpected_intrinsic_argument_exception(qb_thread *thread, uint32_t line_id, qb_intrinsic_function *ifunc, uint32_t param_index, const char *expected_object);
void qb_record_unexpected_value_as_intrinsic_argument_exception(qb_thread *thread, uint32_t line_id, qb_intrinsic_function *ifunc, uint32_t param_index);
void qb_record_unmet_intrinsic_condition_exception(qb_thread *thread, uint32_t line_id, qb_intrinsic_function *ifunc, const char *condition);
void qb_record_unmet_intrinsic_requirement_exception(qb_thread *thread, uint32_t line_id, qb_intrinsic_function *ifunc, const char *requirement);
void qb_record_missing_intrinsic_argument_exception(qb_thread *thread, uint32_t line_id, qb_intrinsic_function *ifunc, uint32_t param_index, const char *condition);
void qb_record_variable_pixel_width_exception(qb_thread *thread, uint32_t line_id, qb_intrinsic_function *ifunc);
void qb_record_missing_alpha_channel_exception(qb_thread *thread, uint32_t line_id, qb_intrinsic_function *ifunc, uint32_t channel_count);
void qb_record_invalid_pixel_format_exception(qb_thread *thread, uint32_t line_id, qb_intrinsic_function *ifunc, uint32_t channel_count);
void qb_record_unexpected_value_as_function_argument_exception(qb_thread *thread, uint32_t line_id, qb_function *qfunc, uint32_t param_index);
void qb_record_unexpected_function_argument_type_exception(qb_thread *thread, uint32_t line_id, qb_function *qfunc, uint32_t param_index, qb_primitive_type value_type, qb_primitive_type param_type);

#define LINE_ID(file_id, line_no)		((file_id << 20) | line_no)
#define LINE_NUMBER(line_id)			(line_id & 0x000FFFFF)
#define FILE_ID(line_id)				(((uint32_t) line_id) >> 20)

#endif
