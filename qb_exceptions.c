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

uint32_t qb_get_source_file_id(const char *file_path TSRMLS_DC) {
	if(file_path) {
		const char **p_source_file;
		uint32_t i;
		for(i = 0; i < QB_G(source_file_count); i++) {
			const char *source_file = QB_G(source_files)[i];
			if(strcmp(source_file, file_path) == 0) {
				return i + 1;
			}
		}
		if(!QB_G(source_files)) {
			qb_create_array((void **) &QB_G(source_files), &QB_G(source_file_count), sizeof(const char *), 4);
		}
		p_source_file = qb_enlarge_array((void **) &QB_G(source_files), 1);
		*p_source_file = file_path;
		return QB_G(source_file_count);
	} else {
		return 0;
	}
}

const char *qb_get_source_file_path(uint32_t file_id TSRMLS_DC) {
	return (file_id) ? QB_G(source_files)[file_id - 1] : "Unknown";
}

uint32_t qb_get_zend_line_id(TSRMLS_D) {
	zend_execute_data *ed = EG(current_execute_data);
	do {
		ed = ed->prev_execute_data;
	} while(ed && !ed->op_array);
	if(ed) {
		const char *filename = ed->op_array->filename;
		uint32_t line_no = (ed->opline) ? ed->opline->lineno : 0;
		uint32_t file_id = qb_get_source_file_id(filename TSRMLS_CC);
		return LINE_ID(file_id, line_no);
	}
	return 0;
}

void qb_set_exception_line_id(uint32_t line_id TSRMLS_DC) {
	uint32_t i;
	for(i = 0; i < QB_G(exception_count); i++) {
		qb_exception *exception = &QB_G(exceptions)[i];
		if(exception->line_id == 0) {
			exception->line_id = line_id;
		}
	}
}

static void qb_show_error(int32_t type, const char *filename, uint32_t line_number, const char *format, ...) {
	va_list args;
	va_start(args, format);
	zend_error_cb(type, filename, line_number, "%s", args);
	va_end(args);
}

void qb_dispatch_exceptions_in_main_thread(void *param1, void *param2, int param3) {
	TSRMLS_FETCH();
	qb_dispatch_exceptions(TSRMLS_C);
}

void qb_dispatch_exceptions(TSRMLS_D) {
	if(QB_G(exception_count)) {
		if(!qb_in_main_thread()) {
			qb_run_in_main_thread(qb_dispatch_exceptions_in_main_thread, NULL, NULL, 0);
		} else {
			uint32_t i;
			for(i = 0; i < QB_G(exception_count); i++) {
				qb_exception *exception = &QB_G(exceptions)[i];
				uint32_t file_id = FILE_ID(exception->line_id);
				uint32_t line_number = LINE_NUMBER(exception->line_id);
				const char *source_file = qb_get_source_file_path(file_id TSRMLS_CC);
				qb_show_error(exception->type, source_file, line_number, "%s", exception->message);
			}
			QB_G(exception_count) = 0;
		}
	}
}

typedef struct qb_exception_params {
	uint32_t line_id;
	int32_t type;
	const char *format;
	va_list arguments;
} qb_exception_params;

static void qb_report_exception_in_main_thread(void *param1, void *param2, int param3) {
	qb_exception_params *params = param1;
	qb_exception *exception;
	char *message = NULL;
	TSRMLS_FETCH();

	if(!QB_G(exceptions)) {
		qb_create_array((void **) &QB_G(exceptions), &QB_G(exception_count), sizeof(qb_exception), 1);
	}
	exception = qb_enlarge_array((void **) &QB_G(exceptions), 1);

	vspprintf(&message, 0, params->format, params->arguments);
	exception->message = message;
	exception->line_id = params->line_id;
	exception->type = params->type;
}

ZEND_ATTRIBUTE_FORMAT(printf, 3, 4)
static void qb_report_exception(uint32_t line_id, int32_t type, const char *format, ...) {
	qb_exception_params params;
	params.line_id = line_id;
	params.type = type;
	params.format = format;
	va_start(params.arguments, format);
	qb_run_in_main_thread(qb_report_exception_in_main_thread, &params, NULL, 0);
	va_end(params.arguments);
}

void qb_report_external_code_load_failure_exception(uint32_t line_id, const char *import_path) {
	qb_report_exception(line_id, E_ERROR, "Unable to load file containing external code: %s", import_path);
}

void qb_report_corrupted_pbj_exception(uint32_t line_id) {
	qb_report_exception(line_id, E_ERROR, "Pixel Bender kernel has structural problems");
}

void qb_report_unsupported_pbj_exception(uint32_t line_id) {
	qb_report_exception(line_id, E_ERROR, "Pixel Bender kernel uses unsupported pixel format");
}

void qb_report_native_compilation_exception(uint32_t line_id, const char *function_name) {
	qb_report_exception(line_id, E_ERROR, "Unable to compile to native code: %s()", function_name);
}

void qb_report_missing_native_symbol_exception(uint32_t line_id, const char *symbol_name) {
	qb_report_exception(line_id, E_WARNING, "Unable to find external symbol: %s", symbol_name);
}

void qb_report_illegal_use_of_this(uint32_t line_id) {
	qb_report_exception(line_id, E_ERROR, "Illegal use of $this");
}

void qb_report_internal_error(uint32_t line_id, const char *problem) {
	qb_report_exception(line_id, E_ERROR, "Internal error: %s", problem);
}

void qb_report_fork_in_fork_exception(uint32_t line_id) {
	qb_report_exception(line_id, E_ERROR, "Cannot fork again inside a forked section");
}

void qb_report_resize_in_fork_exception(uint32_t line_id) {
	qb_report_exception(line_id, E_ERROR, "Cannot resize shared variables inside a forked section");
}

void qb_report_out_of_bound_exception(uint32_t line_id, uint32_t index, uint32_t limit, int32_t inclusive) {
	const char *comparison = (inclusive) ? ">" : ">=";
	qb_report_exception(line_id, E_ERROR, "Array out-of-bound condition: (%u %s %u)", index, comparison, limit);
}

void qb_report_element_size_mismatch_exception(uint32_t line_id, uint32_t size1, uint32_t size2) {
	qb_report_exception(line_id, E_ERROR, "Element size mismatch: (%u != %u)", size1, size2);
}

void qb_report_missing_column_exception(uint32_t line_id, uint32_t column_offset, uint32_t column_count) {
	qb_report_exception(line_id, E_ERROR, "Accessing non-existing column: (%u >= %u)", column_offset, column_count);
}

void qb_report_divide_by_zero_exception(uint32_t line_id) {
	qb_report_exception(line_id, E_ERROR, "Division by zero");
}

void qb_report_vector_width_mismatch_exception(uint32_t line_id, uint32_t vector_width1, uint32_t vector_width2) {
	qb_report_exception(line_id, E_ERROR, "The first vector's width (%u) does not match the sector vector's (%u)", vector_width1, vector_width2);
}

void qb_report_invalid_cross_product_exception(uint32_t line_id, uint32_t vector_width1, uint32_t vector_width2) {
	qb_report_exception(line_id, E_ERROR, "ERROR");
}

void qb_report_invalid_4d_cross_product_exception(uint32_t line_id, uint32_t vector_width1, uint32_t vector_width2, uint32_t vector_width3) {
	qb_report_exception(line_id, E_ERROR, "ERROR");
}

void qb_report_invalid_transform_exception(uint32_t line_id, uint32_t matrix_column, uint32_t matrix_row, uint32_t vector_width) {
	if(!(vector_width >= 2 && vector_width <= 4)) {
		qb_report_exception(line_id, E_ERROR, "Only vectors with 2, 3, or 4 elements can be transformed currently");
	} else {
		qb_report_exception(line_id, E_ERROR, "A %ux%u matrix cannot transform a vector with %u elements", matrix_column, matrix_row, vector_width);
	}
}

void qb_report_not_square_matrix_exception(uint32_t line_id, uint32_t matrix_column, uint32_t matrix_row) {
	qb_report_exception(line_id, E_ERROR, "Operation cannot be performed on a %ux%u matrix", matrix_column, matrix_row);
}

void qb_report_invalid_matrix_multiplication_exception(uint32_t line_id, uint32_t matrix1_column, uint32_t matrix2_row, uint32_t matrix_flags) {
	if((matrix_flags & 1) && (matrix_flags & 2)) {
		qb_report_exception(line_id, E_ERROR, "The number of columns in the first matrix (%u) does not match the number of rows in the second matrix (%u)", matrix1_column, matrix2_row);
	} else if(matrix_flags & 1) {
		qb_report_exception(line_id, E_ERROR, "The number of columns in the matrix (%u) does not match the vector's dimension (%u)", matrix1_column, matrix2_row);
	} else if(matrix_flags & 2) {
		qb_report_exception(line_id, E_ERROR, "The number of rows in the matrix (%u) does not match the vector's dimension (%u)", matrix1_column, matrix2_row);
	}
}

void qb_report_dimension_mismatch_exception(uint32_t line_id, uint32_t dimension1, uint32_t dimension2) {
	qb_report_exception(line_id, E_ERROR, "ERROR");
}

void qb_report_dimension_count_mismatch_exception(uint32_t line_id, uint32_t dimension_count1, uint32_t dimension_count2) {
	qb_report_exception(line_id, E_ERROR, "Incompatible array structure");
}

static void qb_construct_type_specifier(char *buffer, qb_primitive_type type, uint32_t *dimensions, uint32_t dimension_count) {
	uint32_t i;
	char *p = buffer;
	p += sprintf(p, "%s", type_names[type]);
	for(i = 0; i < dimension_count; i++) {
		if(dimensions[i] > 0) {
			p += sprintf(p, "[%u]", dimensions[i]);
		} else {
			p += sprintf(p, "[]");
		}
	}
}

void qb_report_incompatible_array_structure_exception(uint32_t line_id, qb_primitive_type type1, uint32_t *dimensions1, uint32_t dimension_count1, qb_primitive_type type2, uint32_t *dimensions2, uint32_t dimension_count2) {
	char type_specifier1[128], type_specifier2[128];
	if(type1 != QB_TYPE_UNKNOWN) {
		qb_construct_type_specifier(type_specifier1, type1, dimensions1, dimension_count1);
	} else {
		if(dimension_count1 > 0) {
			strcpy(type_specifier1, "array");
		} else {
			strcpy(type_specifier1, "scalar");
		}
	}
	qb_construct_type_specifier(type_specifier2, type2, dimensions2, dimension_count2);
	qb_report_exception(line_id, E_ERROR, "Unable to convert %s to %s", type_specifier1, type_specifier2);
}

void qb_report_file_write_error(uint32_t line_id, uint32_t byte_count, php_stream *stream) {
	qb_report_exception(line_id, E_ERROR, "Error encounter while writing %u bytes to %s", byte_count, stream->orig_path);
}
					
void qb_report_too_man_dimension_exception(uint32_t line_id) {
	qb_report_exception(line_id, E_ERROR, "Too many dimensions");
}

void qb_report_missing_type_declaration_exception(uint32_t line_id, qb_variable *qvar) {
	qb_report_exception(line_id, E_ERROR, "Missing type declaration: %s", qvar->name);
}

void qb_report_illegal_dimension_exception(uint32_t line_id, long const_value) {
	qb_report_exception(line_id, E_NOTICE, "Negative dimension: %ld", const_value);
}

void qb_report_unexpected_tag_in_doc_comment_exception(uint32_t line_id, const char *tag, uint32_t tag_len) {
	qb_report_exception(line_id, E_NOTICE, "Unexpected use of @%.*s", tag_len, tag);
}

void qb_report_doc_comment_syntax_exception(uint32_t line_id) {
	qb_report_exception(line_id, E_NOTICE, "Syntax error encountered while parsing Doc Comments for type information");
}

void qb_report_syntax_error_in_typedef_exception(uint32_t line_id, const char *text) {
	qb_report_exception(line_id, E_WARNING, "Syntax Error encountered while processing type information: %s", text);
}

void qb_report_unknown_keyword_in_typedef_exception(uint32_t line_id, const char *keyword) {
	qb_report_exception(line_id, E_WARNING, "Unknown keyword '%s' encountered while processing type information", keyword);
}

void qb_report_unexpected_numeric_key_in_typedef_exception(uint32_t line_id) {
	qb_report_exception(line_id, E_WARNING, "Numeric key encountered where a string is expected");
}

void qb_report_missing_scope_exception(uint32_t line_id, const char *scope_accessed) {
	qb_report_exception(line_id, E_ERROR, "Cannot access %s:: when no class scope is active", scope_accessed);
}

void qb_report_missing_parent_scope_exception(uint32_t line_id, const char *scope_accessed) {
	qb_report_exception(line_id, E_ERROR, "Cannot access %s:: when current class scope has no parent", scope_accessed);
}

void qb_report_undefined_constant_exception(uint32_t line_id, zend_class_entry *ce, const char *name) {
	if(ce) {
		qb_report_exception(line_id, E_ERROR, "Undefined class constant '%s'", name);
	} else {
		qb_report_exception(line_id, E_ERROR, "Undefined constant '%s'", name);
	}
}

void qb_report_reference_exception(uint32_t line_id) {
	qb_report_exception(line_id, E_ERROR, "No support for reference");
}

void qb_report_not_an_array_exception(uint32_t line_id) {
	qb_report_exception(line_id, E_ERROR, "Cannot retrieve element from non-array");
}

void qb_report_illegal_array_index_exception(uint32_t line_id) {
	qb_report_exception(line_id, E_ERROR, "An array cannot be used as an array index");
}

void qb_report_associative_array_exception(uint32_t line_id) {
	qb_report_exception(line_id, E_ERROR, "No support for associative array");
}

void qb_report_fixed_length_array_exception(uint32_t line_id) {
	qb_report_exception(line_id, E_ERROR, "Cannot change the size of a fixed-length array");
}

void qb_report_variable_variable_exception(uint32_t line_id) {
	qb_report_exception(line_id, E_ERROR, "No support for variable variable-names");
}

void qb_report_missing_property_exception(uint32_t line_id, zend_class_entry *scope, const char *name) {
	if(scope) {
		qb_report_exception(line_id, E_ERROR, "Undefined property: %s::$%s", scope->name, name);
	} else {
		qb_report_exception(line_id, E_ERROR, "Using $this when not in object context");
	}
}

void qb_report_missing_named_element_exception(uint32_t line_id, const char *name) {
	qb_report_exception(line_id, E_ERROR, "Undefined element: '%s'", name);
}

void qb_report_elements_not_named_exception(uint32_t line_id) {
	qb_report_exception(line_id, E_ERROR, "Array elements are not named");
}

void qb_report_return_void_exception(uint32_t line_id) {
	qb_report_exception(line_id, E_ERROR, "Returning a value in a function with return type declared to be void");
}

void qb_report_type_mismatch_exception(uint32_t line_id, qb_primitive_type type1, qb_primitive_type type2) {
	qb_report_exception(line_id, E_ERROR, "Type mismatch: %s != %s", type_names[type1], type_names[type2]);
}

void qb_report_missing_send_declaration_exception(uint32_t line_id) {
	qb_report_exception(line_id, E_ERROR, "Missing type declaration for value from send()");
}

void qb_report_ambiguous_return_value_exception(uint32_t line_id) {
	qb_report_exception(line_id, E_ERROR, "The return value from a regular PHP function must be either saved to a variable or ignored");
}

void qb_report_incorrect_argument_count_exception(uint32_t line_id, qb_intrinsic_function *ifunc, uint32_t argument_count) {
	if(ifunc->argument_count_min == ifunc->argument_count_max) {
		qb_report_exception(line_id, E_ERROR, "%s() expects %u arguments but %u was passed", ifunc->name, ifunc->argument_count_min, argument_count);
	} else {
		if(argument_count < ifunc->argument_count_min || argument_count > ifunc->argument_count_max) {
			if(ifunc->argument_count_max != INVALID_INDEX) {
				qb_report_exception(line_id, E_ERROR, "%s() expects %u to %u arguments but %u was passed", ifunc->name, ifunc->argument_count_min, ifunc->argument_count_max, argument_count);
			} else {
				qb_report_exception(line_id, E_ERROR, "%s() expects %u or more arguments but %u was passed", ifunc->name, ifunc->argument_count_min, argument_count);
			}
		} else {
			// function is being called because either argument count can be the min or the max
			// and not something in between
			qb_report_exception(line_id, E_ERROR, "%s() expects either %u or %u arguments", ifunc->name, ifunc->argument_count_min, ifunc->argument_count_max);
		}
	}
}

static const char * qb_get_parameter_name(uint32_t index, uint32_t max_param_count) {
	if(max_param_count == -1) {
		return "parameters";
	} else {
		if(max_param_count == 1) {
			return "parameter";
		} else {
			switch(index) {
				case 0: return "first parameter";
				case 1: return "second parameter";
				case 2: return "third parameter";
				case 3: return "fourth parameter";
				case 4: return "fifth parameter";
				case 5: return "sixth parameter";
				case 6: return "seventh parameter";
				case 7: return "eighth parameter";
				case 8: return "ninth parameter";
				case 9: return "tenth parameter";
				default: return "parameter";
			}
		}
	}
}

static const char * qb_get_indefinite_article(const char *object) {
	switch(object[0]) {
		case 'a':
		case 'e':
		case 'i':
		case 'o':
		case 'u': return "an";
		default: return "a";
	}
}

void qb_report_unexpected_intrinsic_argument_exception(uint32_t line_id, qb_intrinsic_function *ifunc, uint32_t param_index, const char *expected_object) {
	const char *article = qb_get_indefinite_article(expected_object);
	const char *parameter = qb_get_parameter_name(param_index, ifunc->argument_count_max);
	qb_report_exception(line_id, E_ERROR, "%s() expects %s %s as %s", ifunc->name, article, expected_object, parameter);
}

void qb_report_unexpected_value_as_intrinsic_argument_exception(uint32_t line_id, qb_intrinsic_function *ifunc, uint32_t param_index) {
	const char *parameter = qb_get_parameter_name(param_index, ifunc->argument_count_max);
	qb_report_exception(line_id, E_ERROR, "%s() expects a variable as %s", ifunc->name, parameter);
}

void qb_report_missing_intrinsic_argument_exception(uint32_t line_id, qb_intrinsic_function *ifunc, uint32_t param_index, const char *condition) {
	const char *parameter = qb_get_parameter_name(param_index, ifunc->argument_count_max);
	qb_report_exception(line_id, E_ERROR, "%s() requires the %s when %s", ifunc->name, parameter, condition);
}

void qb_report_unmet_intrinsic_condition_exception(uint32_t line_id, qb_intrinsic_function *ifunc, const char *condition) {
	qb_report_exception(line_id, E_ERROR, "%s() expects %s", ifunc->name, condition);
}

void qb_report_unmet_intrinsic_requirement_exception(uint32_t line_id, qb_intrinsic_function *ifunc, const char *requirement) {
	qb_report_exception(line_id, E_ERROR, "%s() requires %s", ifunc->name, requirement);
}

void qb_report_variable_pixel_width_exception(uint32_t line_id, qb_intrinsic_function *ifunc) {
	qb_report_exception(line_id, E_ERROR, "%s() can only handle fixed-length arrays", ifunc->name);
}

void qb_report_missing_alpha_channel_exception(uint32_t line_id, qb_intrinsic_function *ifunc, uint32_t channel_count) {
	qb_report_exception(line_id, E_ERROR, "%s() expects an array whose last dimension is 4", ifunc->name);
}

void qb_report_invalid_pixel_format_exception(uint32_t line_id, qb_intrinsic_function *ifunc, uint32_t channel_count) {
	qb_report_exception(line_id, E_ERROR, "%s() expects an array whose last dimension is either 3 or 4", ifunc->name);
}

void qb_report_unexpected_value_as_function_argument_exception(uint32_t line_id, const char *class_name, const char *function_name, uint32_t param_index) {
	const char *space;
	if(class_name) {
		space = "::";
	} else {
		class_name = "";
		space = "";
	}

	if(param_index <= 10) {
		const char *parameter = qb_get_parameter_name(param_index, 10);
		qb_report_exception(line_id, E_ERROR, "%s%s%s() expects the %s to be a variable", class_name, space, function_name, parameter);
	} else {
		qb_report_exception(line_id, E_ERROR, "%s%s%s() expects parameter %u to be a variable", class_name, space, function_name, param_index + 1);
	}
}
					
void qb_report_unexpected_function_argument_type_exception(uint32_t line_id, const char *class_name, const char *function_name, uint32_t param_index, qb_primitive_type value_type, qb_primitive_type param_type) {
	const char *type1 = type_names[value_type];
	const char *type2 = type_names[param_type];
	const char *space;
	if(class_name) {
		space = "::";
	} else {
		class_name = "";
		space = "";
	}

	if(param_index <= 10) {
		const char *parameter = qb_get_parameter_name(param_index, 10);
		qb_report_exception(line_id, E_ERROR, "%s%s%s() expects the %s to be %s but a variable of the type %s is given", class_name, space, function_name, parameter, type1, type2);
	} else {
		qb_report_exception(line_id, E_ERROR, "%s%s%s() expects parameter %u to be %s but a variable of the type %s is given", class_name, space, function_name, param_index + 1, type1, type2);
	}
}

void qb_report_missing_argument_exception(uint32_t line_id, const char *class_name, const char *function_name, uint32_t argument_index, uint32_t caller_line_id) {
#ifdef ZTS
	void ***tsrm_ls = qb_get_tsrm_ls();
#endif
	const char *space;
	if(class_name) {
		space = "::";
	} else {
		class_name = "";
		space = "";
	}
	if(caller_line_id) {
		uint32_t caller_file_id = FILE_ID(caller_line_id);
		uint32_t caller_line_number = LINE_NUMBER(caller_line_id);
		const char *caller_source_file = qb_get_source_file_path(caller_file_id TSRMLS_CC);
		qb_report_exception(line_id, E_WARNING, "Missing argument %u for %s%s%s(), called in %s on line %u and defined", argument_index + 1, class_name, space, function_name, caller_source_file, caller_line_number);
	} else {
		qb_report_exception(line_id, E_WARNING, "Missing argument %u for %s%s%s()", argument_index + 1, class_name, space, function_name);
	}
}

void qb_report_void_return_value_exception(uint32_t line_id, const char *class_name, const char *function_name) {
	const char *space;
	if(class_name) {
		space = "::";
	} else {
		class_name = "";
		space = "";
	}
	qb_report_exception(line_id, E_ERROR, "%s%s%s() returns void", class_name, space, function_name);
}


void qb_report_function_call_exception(uint32_t line_id, const char *class_name, const char *function_name) {
	const char *space;
	if(class_name) {
		space = "::";
	} else {
		class_name = "";
		space = "";
	}
	qb_report_exception(line_id, E_ERROR, "Error encountered calling %s%s%s()", class_name, space, function_name);
}

void qb_report_missing_function_exception(uint32_t line_id, const char *class_name, const char *function_name) {
	const char *space;
	if(class_name) {
		space = "::";
	} else {
		class_name = "";
		space = "";
	}
	qb_report_exception(line_id, E_ERROR, "Call to undefined function %s%s%s()", class_name, space, function_name);
}

void qb_report_generator_function_exception(uint32_t line_id, const char *class_name, const char *function_name) {
	const char *space;
	if(class_name) {
		space = "::";
	} else {
		class_name = "";
		space = "";
	}
	qb_report_exception(line_id, E_ERROR, "Unable to call generator function %s%s%s()", class_name, space, function_name);
}


void qb_report_inline_function_exception(uint32_t line_id, const char *class_name, const char *function_name) {
	const char *space;
	if(class_name) {
		space = "::";
	} else {
		class_name = "";
		space = "";
	}
	qb_report_exception(line_id, E_WARNING, "Cannot inline function %s%s%s()", class_name, space, function_name);
}

void qb_report_too_much_recursion_exception(uint32_t line_id, uint32_t call_depth) {
	qb_report_exception(line_id, E_ERROR, "Too much recursion");
}

void qb_report_illegal_dimension_count_exception(uint32_t line_id, uint32_t dimension_count) {
	qb_report_exception(line_id, E_ERROR, "Array initializer has too many dimensions");
}

void qb_report_illegal_array_structure_exception(uint32_t line_id) {
	qb_report_exception(line_id, E_ERROR, "Array has irregular structure");
}

void qb_report_binary_string_size_mismatch_exception(uint32_t line_id, uint32_t byte_count, qb_primitive_type type) {
	uint32_t element_byte_count = BYTE_COUNT(1, type);
	const char *type_name = type_names[type];
	if(byte_count < element_byte_count) {
		qb_report_exception(line_id, E_WARNING, "Number of bytes in string (%u) is less than the size of (%s)", byte_count, type_name);
	} else {
		qb_report_exception(line_id, E_WARNING, "Number of bytes in string (%u) is not divisible by the size of %s", byte_count, type_name);
	}
}

void qb_report_memory_map_exception(uint32_t line_id, const char *file_path) {
	qb_report_exception(line_id, E_ERROR, "Cannot map file into memory: %s", file_path);
}

void qb_report_argument_size_mismatch_exception(uint32_t line_id, uint32_t dimension, uint32_t dimension_expected) {
	qb_report_exception(line_id, E_ERROR, "Number of element (%u) exceeds the declared size of the array (%u)", dimension, dimension_expected);
}

void qb_report_image_width_mismatch_exception(uint32_t line_id, uint32_t width, uint32_t width_expected) {
	qb_report_exception(line_id, E_ERROR, "Declared array size (%u) does not match the width of image (%u)", width, width_expected);
}

void qb_report_image_height_mismatch_exception(uint32_t line_id, uint32_t height, uint32_t height_expected) {
	qb_report_exception(line_id, E_ERROR, "Declared array size (%u) does not match the height of image (%u)", height, height_expected);
}

void qb_report_pixel_count_mismatch_exception(uint32_t line_id, uint32_t pixel_count, uint32_t pixel_count_expected) {
	qb_report_exception(line_id, E_ERROR, "Declared array size (%u) does not match the number of pixel in the image (%u)", pixel_count, pixel_count_expected);
}

void qb_report_invalid_variable_for_image_exception(uint32_t line_id, uint32_t dimension_count, int32_t true_color) {
	if(dimension_count > 3) {
		qb_report_exception(line_id, E_ERROR, "Variable has too many dimensions for an image (%u)", dimension_count);
	} else {
		const char *type = (true_color) ? "true-color" : "paletted";
		qb_report_exception(line_id, E_ERROR, "Variable is not valid for a %s image", type);
	}
}

void qb_report_gd_image_exception(uint32_t line_id, uint32_t width, uint32_t height) {
	qb_report_exception(line_id, E_ERROR, "Unable to create a %ux%u GD image", width, height);
}

void qb_report_undefined_dimension_exception(uint32_t line_id) {
	qb_report_exception(line_id, E_ERROR, "Cannot derived element count due to undefined lower dimension");
}

void qb_report_illegal_conversion_to_array_exception(uint32_t line_id, const char *object) {
	const char *article = qb_get_indefinite_article(object);
	qb_report_exception(line_id, E_ERROR, "Cannot convert %s %s to an array", article, object);
}

void qb_report_illegal_conversion_from_array_exception(uint32_t line_id, const char *object) {
	const char *article = qb_get_indefinite_article(object);
	qb_report_exception(line_id, E_ERROR, "Cannot convert an array to %s %s", article, object);
}

void qb_report_missing_class_exception(uint32_t line_id, const char *class_name) {
	qb_report_exception(line_id, E_ERROR, "Class '%s' not found", class_name);
}

void qb_report_abstract_class_exception(uint32_t line_id, const char *class_name) {
	qb_report_exception(line_id, E_ERROR, "Cannot instantiate interface or abstract class '%s'", class_name);
}

void qb_report_invalid_break_level_exception(uint32_t line_id, uint32_t nest_levels) {
	qb_report_exception(line_id, E_ERROR, "Cannot break/continue %u level%s", nest_levels, (nest_levels == 1) ? "" : "s");
}

void qb_report_invalid_cast_exception(uint32_t line_id, const char *type_name) {
	qb_report_exception(line_id, E_ERROR, "Cannot typecast value to %s", type_name);
}

void qb_report_unsupported_language_feature_exception(uint32_t line_id, const char *op_name) {
	qb_report_exception(line_id, E_ERROR, "Unsupported language feature (%s)", op_name);
}

void qb_report_missing_pbj_output_image_parameter_exception(uint32_t line_id) {
	qb_report_exception(line_id, E_ERROR, "A parameter must be passed by reference to the function to receive the result");
}

void qb_report_missing_pbj_input_image_parameter_exception(uint32_t line_id, const char *param_name) {
	qb_report_exception(line_id, E_ERROR, "Input image '%s' must be passed to the function", param_name);
}

void qb_report_missing_pbj_parameter_exception(uint32_t line_id, const char *param_name) {
	qb_report_exception(line_id, E_ERROR, "Parameter '%s' does not have a default value and must be passed to the function", param_name);
}

void qb_report_incorrect_pbj_parameter_type_exception(uint32_t line_id, const char *param_name) {
	qb_report_exception(line_id, E_ERROR, "Parameter '%s' is not of the correct type", param_name);
}

void qb_report_extraneous_pbj_parameter_exception(uint32_t line_id, const char *param_name) {
	qb_report_exception(line_id, E_ERROR, "Parameter '%s' is obtained from the input image and cannot be passed separately", param_name);
}

#ifdef ZEND_DEBUG
ZEND_ATTRIBUTE_FORMAT(printf, 1, 2)
void qb_debug_abort(char *format, ...) {
	va_list args;
	va_start(args, format);
	vprintf(format, args);
	va_end(args);
	exit(-1);
}
#endif
