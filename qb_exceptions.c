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

static void qb_record_exception_in_main_thread(void *param1, void *param2, int param3) {
	TSRMLS_FETCH();
	uint32_t line_id = param3;
	const char *format = param1;
	va_list args = param2;
	qb_exception *exception = emalloc(sizeof(qb_exception));
	qb_exception *last_exception = QB_G(first_exception);
	char *message = NULL;

	vspprintf(&message, 0, format, args);
	exception->message = message;
	if(last_exception) {
		do {
			last_exception = last_exception->next_exception;
		} while(last_exception->next_exception);
		last_exception->next_exception = exception;
	} else {
		QB_G(first_exception) = exception;
	}
}

ZEND_ATTRIBUTE_FORMAT(printf, 3, 4)
static void qb_record_exception(qb_thread *thread, uint32_t line_id, const char *format, ...) {
	va_list args;
	va_start(args, format);
	qb_run_in_main_thread(thread, qb_record_exception_in_main_thread, (void *) format, args, line_id, NULL);
	va_end(args);
}


void qb_record_out_of_bound_exception(qb_thread *thread, uint32_t line_id, uint32_t index, uint32_t limit, int32_t inclusive) {

}

void qb_record_missing_column_exception(qb_thread *thread, uint32_t line_id, uint32_t column_offset, uint32_t column_count) {

}

void qb_record_divide_by_zero_exception(qb_thread *thread, uint32_t line_id) {
	qb_record_exception(thread, line_id, "Division by zero");
}

void qb_record_vector_width_mismatch_exception(qb_thread *thread, uint32_t line_id, uint32_t vector_width1, uint32_t vector_width2) {
	qb_record_exception(thread, line_id, "The size of the first vector (%d) does not match the size of the second vector (%d)", vector_width1, vector_width2);
}

void qb_record_invalid_cross_product_exception(qb_thread *thread, uint32_t line_id, uint32_t vector_width1, uint32_t vector_width2) {
}

void qb_record_invalid_4d_cross_product_exception(qb_thread *thread, uint32_t line_id, uint32_t vector_width1, uint32_t vector_width2, uint32_t vector_width3) {
}

void qb_report_invalid_transform_exception(qb_thread *thread, uint32_t line_id, uint32_t matrix_column, uint32_t matrix_row, uint32_t vector_width) {
	if(!(vector_width >= 2 && vector_width <= 4)) {
		qb_record_exception(thread, line_id, "Only vectors with 2, 3, or 4 elements can be transformed currently");
	} else {
		qb_record_exception(thread, line_id, "A %dx%d matrix cannot transform a vector with %d elements", matrix_column, matrix_row, vector_width);
	}
}

void qb_record_dimension_mismatch_exception(qb_thread *thread, uint32_t line_id, uint32_t dimension1, uint32_t dimension2) {
}

void qb_record_dimension_count_mismatch_exception(qb_thread *thread, uint32_t line_id, uint32_t dimension1, uint32_t dimension2) {
}

void qb_record_illegal_dimension_count_exception(qb_thread *thread, uint32_t line_id, uint32_t dimension_count) {
	qb_record_exception(thread, line_id, "Array initializer has too many dimensions");
}

void qb_record_illegal_array_structure_exception(qb_thread *thread, uint32_t line_id) {
	qb_record_exception(thread, line_id, "Array has irregular structure");
}

void qb_record_binary_string_size_mismatch_exception(qb_thread *thread, uint32_t line_id, uint32_t byte_count, qb_primitive_type type) {
	qb_record_exception(thread, line_id, "Number of bytes in string (%d) is not divisible by %d", byte_count, type_sizes[type]);
}

void qb_record_illegal_array_conversion_exception(qb_thread *thread, uint32_t line_id, qb_primitive_type type) {
	qb_record_exception(thread, line_id, "Cannot convert an array to %s", type_names[type]);

}

void qb_record_missing_type_declaration_exception(qb_thread *thread, uint32_t line_id, qb_variable *qvar) {
	qb_record_exception(thread, line_id, "Missing type declaration: %s", qvar->name);
}

void qb_record_external_code_load_failure_exception(qb_thread *thread, uint32_t line_id, const char *import_path) {
	qb_record_exception(thread, line_id, "Unable to load file containing external code");
}

void qb_record_illegal_dimension_declaration_exception(qb_thread *thread, uint32_t line_id, long const_value) {
}

void qb_record_undefined_constant_in_dimension_declaration_exception(qb_thread *thread, uint32_t line_id, const char *name) {

}

void qb_record_missing_scope_exception(qb_thread *thread, uint32_t line_id, const char *scope_accessed) {
	qb_record_exception(thread, line_id, "Cannot access %s:: when no class scope is active", scope_accessed);
}


void qb_record_missing_parent_scope_exception(qb_thread *thread, uint32_t line_id, const char *scope_accessed) {
	qb_record_exception(thread, line_id, "Cannot access %s:: when current class scope has no parent", scope_accessed);
}

void qb_record_undefined_constant_exception(qb_thread *thread, uint32_t line_id, zend_class_entry *ce, const char *name) {
	qb_record_exception(thread, line_id, "Undefined constant '%s'", name);
}

void qb_record_reference_exception(qb_thread *thread, uint32_t line_id) {
	qb_record_exception(thread, line_id, "No support for reference");
}

void qb_record_incorrect_argument_count_exception(qb_thread *thread, uint32_t line_id, qb_intrinsic_function *ifunc, uint32_t argument_count) {
	if(ifunc->argument_count_min == ifunc->argument_count_max) {
		qb_record_exception(thread, line_id, "%s() expects %d arguments but %d was passed", ifunc->name, ifunc->argument_count_min, argument_count);
	} else {
		if(argument_count < ifunc->argument_count_min || argument_count > ifunc->argument_count_max) {
			qb_record_exception(thread, line_id, "%s() expects %d to %d arguments but %d was passed", ifunc->name, ifunc->argument_count_min, ifunc->argument_count_max, argument_count);
		} else {
			// function is being called because either argument count can be the min or the max
			// and not something in between
			qb_abort("%s() expects either %d or %d arguments", ifunc->name, ifunc->argument_count_min, ifunc->argument_count_max);
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
		case 'e:':
		case 'i':
		case 'o':
		case 'u': return "an";
		default: return "a";
	}
}

void qb_record_unexpected_intrinsic_argument_exception(qb_thread *thread, uint32_t line_id, qb_intrinsic_function *ifunc, uint32_t param_index, const char *expected_object) {
	const char *article = qb_get_indefinite_article(expected_object);
	const char *parameter = qb_get_parameter_name(param_index, ifunc->argument_count_max);
	qb_record_exception(thread, line_id, "%s() expects %s %s as %s", ifunc->name, article, expected_object, parameter);
}

void qb_record_unexpected_value_as_intrinsic_argument_exception(qb_thread *thread, uint32_t line_id, qb_intrinsic_function *ifunc, uint32_t param_index) {
	const char *parameter = qb_get_parameter_name(param_index, ifunc->argument_count_max);
	qb_record_exception(thread, line_id, "%s() expects a variable as %s", ifunc->name, parameter);
}

void qb_record_missing_intrinsic_argument_exception(qb_thread *thread, uint32_t line_id, qb_intrinsic_function *ifunc, uint32_t param_index, const char *condition) {
	const char *parameter = qb_get_parameter_name(param_index, ifunc->argument_count_max);
	qb_record_exception(thread, line_id, "%s() requires the %s when %s", ifunc->name, parameter, condition);
}

void qb_record_unmet_intrinsic_condition_exception(qb_thread *thread, uint32_t line_id, qb_intrinsic_function *ifunc, const char *condition) {
	qb_record_exception(thread, line_id, "%s() expects %s", ifunc->name, condition);
}

void qb_record_unmet_intrinsic_requirement_exception(qb_thread *thread, uint32_t line_id, qb_intrinsic_function *ifunc, const char *requirement) {
	qb_record_exception(thread, line_id, "%s() requires %s", ifunc->name, requirement);
}

void qb_record_variable_pixel_width_exception(qb_thread *thread, uint32_t line_id, qb_intrinsic_function *ifunc) {
	qb_record_exception(thread, line_id, "%s() can only handle fixed-length arrays", ifunc->name);
}

void qb_record_missing_alpha_channel_exception(qb_thread *thread, uint32_t line_id, qb_intrinsic_function *ifunc, uint32_t channel_count) {
	qb_record_exception(thread, line_id, "%s() expects an array whose last dimension is 4", ifunc->name);
}

void qb_record_invalid_pixel_format_exception(qb_thread *thread, uint32_t line_id, qb_intrinsic_function *ifunc, uint32_t channel_count) {
	qb_record_exception(thread, line_id, "%s() expects an array whose last dimension is either 3 or 4", ifunc->name);
}

void qb_record_unexpected_value_as_function_argument_exception(qb_thread *thread, uint32_t line_id, qb_function *qfunc, uint32_t param_index) {
	if(param_index <= 10) {
		const char *parameter = qb_get_parameter_name(param_index, 10);
		qb_record_exception(thread, line_id, "%s expects the %s to be a variable", qfunc->name, parameter);
	} else {
		qb_record_exception(thread, line_id, "%s expects parameter %d to be a variable", qfunc->name, param_index + 1);
	}
}
					
void qb_record_unexpected_function_argument_type_exception(qb_thread *thread, uint32_t line_id, qb_function *qfunc, uint32_t param_index, qb_primitive_type value_type, qb_primitive_type param_type) {
	const char *type1 = type_names[value_type];
	const char *type2 = type_names[param_type];

	if(param_index <= 10) {
		const char *parameter = qb_get_parameter_name(param_index, 10);
		qb_record_exception(thread, line_id, "%s expects the %s to be %s but a variable of the type %s is given", qfunc->name, parameter, type1, type2);
	} else {
		qb_record_exception(thread, line_id, "%s expects parameter %d to be %s but a variable of the type %s is given", qfunc->name, param_index + 1, type1, type2);
	}
}

