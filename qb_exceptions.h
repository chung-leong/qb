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
const char *qb_get_source_file_path(uint32_t line_id TSRMLS_DC);

void qb_dispatch_exceptions(TSRMLS_D);

void qb_report_external_code_load_failure_exception(uint32_t line_id, const char *import_path);
void qb_report_corrupted_pbj_exception(uint32_t line_id);
void qb_report_unsupported_pbj_exception(uint32_t line_id);
void qb_report_native_compilation_exception(uint32_t line_id, const char *function_name);
void qb_report_missing_native_symbol_exception(uint32_t line_id, const char *symbol_name);
void qb_report_illegal_use_of_this(uint32_t line_id);

// these functions may be called at runtime
void qb_report_out_of_bound_exception(uint32_t line_id, uint32_t index, uint32_t limit, int32_t inclusive);
void qb_report_missing_column_exception(uint32_t line_id, uint32_t column_offset, uint32_t column_count);
void qb_report_divide_by_zero_exception(uint32_t line_id);
void qb_report_vector_width_mismatch_exception(uint32_t line_id, uint32_t vector_width1, uint32_t vector_width2);
void qb_report_invalid_cross_product_exception(uint32_t line_id, uint32_t vector_width1, uint32_t vector_width2);
void qb_report_invalid_4d_cross_product_exception(uint32_t line_id, uint32_t vector_width1, uint32_t vector_width2, uint32_t vector_width3);
void qb_report_invalid_transform_exception(uint32_t line_id, uint32_t matrix_column, uint32_t matrix_row, uint32_t vector_width);
void qb_report_invalid_matrix_multiplication_exception(uint32_t line_id, uint32_t matrix1_column, uint32_t matrix2_row, uint32_t matrix_flags);
void qb_report_not_square_matrix_exception(uint32_t line_id, uint32_t matrix_column, uint32_t matrix_row);

void qb_report_gd_image_exception(uint32_t line_id, uint32_t width, uint32_t height);

void qb_report_dimension_mismatch_exception(uint32_t line_id, uint32_t dimension1, uint32_t dimension2);
void qb_report_dimension_count_mismatch_exception(uint32_t line_id, uint32_t dimension_count1, uint32_t dimension_count2);
void qb_report_too_man_dimension_exception(uint32_t line_id);
void qb_report_illegal_dimension_count_exception(uint32_t line_id, uint32_t dimension_count);
void qb_report_illegal_dimension_exception(uint32_t line_id, long const_value);
void qb_report_incompatible_array_structure_exception(uint32_t line_id, qb_primitive_type type1, uint32_t *dimensions1, uint32_t dimension_count1, qb_primitive_type type2, uint32_t *dimensions2, uint32_t dimension_count2);

void qb_report_unexpected_tag_in_doc_comment_exception(uint32_t line_id, const char *tag, uint32_t tag_len);
void qb_report_doc_comment_syntax_exception(uint32_t line_id);
void qb_report_syntax_error_in_typedef_exception(uint32_t line_id, const char *text);
void qb_report_unknown_keyword_in_typedef_exception(uint32_t line_id, const char *keyword);
void qb_report_unexpected_numeric_key_in_typedef_exception(uint32_t line_id);

void qb_report_missing_type_declaration_exception(uint32_t line_id, qb_variable *qvar);

void qb_report_missing_scope_exception(uint32_t line_id, const char *scope_accessed);
void qb_report_missing_parent_scope_exception(uint32_t line_id, const char *scope_accessed);
void qb_report_undefined_constant_exception(uint32_t line_id, zend_class_entry *ce, const char *name);
void qb_report_reference_exception(uint32_t line_id);
void qb_report_not_an_array_exception(uint32_t line_id);
void qb_report_illegal_array_index_exception(uint32_t line_id);
void qb_report_associative_array_exception(uint32_t line_id);
void qb_report_fixed_length_array_exception(uint32_t line_id);
void qb_report_variable_variable_exception(uint32_t line_id);
void qb_report_missing_property_exception(uint32_t line_id, zend_class_entry *scope, const char *name);
void qb_report_missing_named_element_exception(uint32_t line_id, const char *name);
void qb_report_elements_not_named_exception(uint32_t line_id);
void qb_report_return_void_exception(uint32_t line_id);
void qb_report_type_mismatch_exception(uint32_t line_id, qb_primitive_type type1, qb_primitive_type type2);
void qb_report_missing_send_declaration_exception(uint32_t line_id);
void qb_report_ambiguous_return_value_exception(uint32_t line_id);
void qb_report_missing_function_exception(uint32_t line_id, const char *class_name, const char *name);
void qb_report_generator_function_exception(uint32_t line_id, const char *class_name, const char *name);
void qb_report_inline_function_exception(uint32_t line_id, const char *class_name, const char *name);

void qb_report_incorrect_argument_count_exception(uint32_t line_id, qb_intrinsic_function *ifunc, uint32_t argument_count);
void qb_report_unexpected_intrinsic_argument_exception(uint32_t line_id, qb_intrinsic_function *ifunc, uint32_t param_index, const char *expected_object);
void qb_report_unexpected_value_as_intrinsic_argument_exception(uint32_t line_id, qb_intrinsic_function *ifunc, uint32_t param_index);
void qb_report_unmet_intrinsic_condition_exception(uint32_t line_id, qb_intrinsic_function *ifunc, const char *condition);
void qb_report_unmet_intrinsic_requirement_exception(uint32_t line_id, qb_intrinsic_function *ifunc, const char *requirement);
void qb_report_missing_intrinsic_argument_exception(uint32_t line_id, qb_intrinsic_function *ifunc, uint32_t param_index, const char *condition);
void qb_report_variable_pixel_width_exception(uint32_t line_id, qb_intrinsic_function *ifunc);
void qb_report_missing_alpha_channel_exception(uint32_t line_id, qb_intrinsic_function *ifunc, uint32_t channel_count);
void qb_report_invalid_pixel_format_exception(uint32_t line_id, qb_intrinsic_function *ifunc, uint32_t channel_count);
void qb_report_unexpected_value_as_function_argument_exception(uint32_t line_id, qb_function *qfunc, uint32_t param_index);
void qb_report_unexpected_function_argument_type_exception(uint32_t line_id, qb_function *qfunc, uint32_t param_index, qb_primitive_type value_type, qb_primitive_type param_type);
void qb_report_missing_argument_exception(uint32_t line_id, const char *class_name, const char *function_name, uint32_t argument_index, uint32_t caller_line_id);
void qb_report_function_call_exception(uint32_t line_id, const char *class_name, const char *function_name);
void qb_report_too_much_recursion_exception(uint32_t line_id, uint32_t call_depth);

void qb_report_illegal_array_structure_exception(uint32_t line_id);
void qb_report_binary_string_size_mismatch_exception(uint32_t line_id, uint32_t byte_count, qb_primitive_type type);
void qb_report_memory_map_exception(uint32_t line_id, const char *file_path);
void qb_report_argument_size_mismatch_exception(uint32_t line_id, uint32_t dimension, uint32_t dimension_expected);
void qb_report_image_width_mismatch_exception(uint32_t line_id, uint32_t width, uint32_t width_expected);
void qb_report_image_height_mismatch_exception(uint32_t line_id, uint32_t height, uint32_t height_expected);
void qb_report_pixel_count_mismatch_exception(uint32_t line_id, uint32_t pixel_count, uint32_t pixel_count_expected);
void qb_report_invalid_variable_for_image_exception(uint32_t line_id, uint32_t dimension_count, int32_t true_color);
void qb_report_undefined_dimension_exception(uint32_t line_id);
void qb_report_illegal_conversion_to_array_exception(uint32_t line_id, const char *object);
void qb_report_illegal_conversion_from_array_exception(uint32_t line_id, const char *object);
void qb_report_missing_class_exception(uint32_t line_id, const char *class_name);
void qb_report_abstract_class_exception(uint32_t line_id, const char *class_name);
void qb_report_invalid_break_level_exception(uint32_t line_id, uint32_t nest_levels);
void qb_report_invalid_cast_exception(uint32_t line_id, const char *type_name);
void qb_report_unsupported_language_feature_exception(uint32_t line_id, uint32_t zend_opcode);

void qb_report_missing_pbj_output_image_parameter_exception(uint32_t line_id);
void qb_report_missing_pbj_input_image_parameter_exception(uint32_t line_id, const char *param_name);
void qb_report_missing_pbj_parameter_exception(uint32_t line_id, const char *param_name);
void qb_report_incorrect_pbj_parameter_type_exception(uint32_t line_id, const char *param_name);
void qb_report_extraneous_pbj_parameter_exception(uint32_t line_id, const char *param_name);

#define LINE_ID(file_id, line_no)		((file_id << 20) | line_no)
#define LINE_NUMBER(line_id)			(line_id & 0x000FFFFF)
#define FILE_ID(line_id)				(((uint32_t) line_id) >> 20)

#ifdef ZEND_DEBUG
ZEND_ATTRIBUTE_FORMAT(printf, 1, 2)
void qb_debug_abort(char *format, ...);
#endif

#endif
