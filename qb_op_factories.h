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

#ifndef QB_OP_FACTORIES_H_
#define QB_OP_FACTORIES_H_

#define QB_OP_FACTORY_VAR								0
#define QB_OP_FACTORY_DIM								1
#define QB_OP_FACTORY_OBJ								2

#define QB_OP_FACTORY_INT32								0
#define QB_OP_FACTORY_INT64								1
#define QB_OP_FACTORY_DOUBLE							2
#define QB_OP_FACTORY_BOOLEAN							3
#define QB_OP_FACTORY_ARRAY								4
#define QB_OP_FACTORY_STRING							5

#define QB_OP_FACTORY_SELF								0
#define QB_OP_FACTORY_GLOBAL							1
#define QB_OP_FACTORY_STATIC							2
#define QB_OP_FACTORY_PARENT							3

#define QB_OP_FACTORY_LOCAL								0
#define QB_OP_FACTORY_GLOBAL							1
#define QB_OP_FACTORY_STATIC							2
#define QB_OP_FACTORY_STATIC_MEMBER						3

#define QB_OP_FACTORY_INTRINSIC							0
#define QB_OP_FACTORY_NORMAL							1
#define QB_OP_FACTORY_INLINE							2
#define QB_OP_FACTORY_ZEND								3

// special address flags 
enum {
	QB_ADDRESS_MM_RESULT								= 0x08000000,
	QB_ADDRESS_MV_RESULT								= 0x04000000,
	QB_ADDRESS_VM_RESULT								= 0x02000000,
	QB_ADDRESS_VV_RESULT								= 0x01000000,
	QB_ADDRESS_LITERAL_ARRAY							= 0x00800000,
	QB_ADDRESS_MULTIDIMENSIONAL_LITERAL_ARRAY			= 0x00400000,
};

typedef struct qb_op_factory							qb_op_factory; 
typedef struct qb_derived_op_factory					qb_derived_op_factory; 
typedef struct qb_basic_op_factory						qb_basic_op_factory; 
typedef struct qb_float_op_factory						qb_float_op_factory; 
typedef struct qb_simple_op_factory						qb_simple_op_factory;
typedef struct qb_fetch_op_factory						qb_fetch_op_factory;
typedef struct qb_copy_op_factory						qb_copy_op_factory;
typedef struct qb_unset_op_factory						qb_unset_op_factory;
typedef struct qb_unset_element_op_factory				qb_unset_element_op_factory;
typedef struct qb_cast_op_factory						qb_cast_op_factory;
typedef struct qb_comparison_op_factory					qb_comparison_op_factory;
typedef struct qb_comparison_branch_op_factory			qb_comparison_branch_op_factory;
typedef struct qb_arithmetic_op_factory					qb_arithmetic_op_factory;
typedef struct qb_append_op_factory						qb_append_op_factory;
typedef struct qb_print_op_factory						qb_print_op_factory;
typedef struct qb_fcall_op_factory						qb_fcall_op_factory;
typedef struct qb_matrix_op_factory						qb_matrix_op_factory;
typedef struct qb_equivalent_matrix_op_factory			qb_equivalent_matrix_op_factory;
typedef struct qb_matrix_op_factory_selector			qb_matrix_op_factory_selector;
typedef struct qb_pixel_op_factory						qb_pixel_op_factory;
typedef struct qb_utf8_op_factory						qb_utf8_op_factory;
typedef struct qb_gather_op_factory						qb_gather_op_factory;
typedef struct qb_array_resize_op_factory				qb_array_resize_op_factory;
typedef struct qb_php_function_result_factory			qb_php_function_result_factory;

typedef struct qb_intrinsic_op_factory					qb_intrinsic_op_factory;

typedef struct qb_op_decomposer							qb_op_decomposer;
typedef struct qb_minmax_decomposer						qb_minmax_decomposer;
typedef struct qb_fetch_do_op_decomposer				qb_fetch_do_op_decomposer;
typedef struct qb_set_op_chooser						qb_set_op_chooser;
typedef struct qb_fork_decomposer						qb_fork_decomposer;
typedef struct qb_round_decomposer						qb_round_decomposer;

typedef struct qb_multiply_factory_selector				qb_multiply_factory_selector;
typedef struct qb_complex_op_factory_selector			qb_complex_op_factory_selector;

typedef int32_t (*qb_produce_composite_proc)(qb_compiler_context *cxt, void *factory, qb_operand *operands, uint32_t operand_count, qb_operand *result, uint32_t *jump_target_indices, uint32_t jump_target_count, qb_result_prototype *result_prototype);

typedef int32_t (*qb_resolve_address_flags_proc)(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, uint32_t *p_flags);

typedef int32_t (*qb_resolve_expression_type_proc)(qb_compiler_context *cxt, qb_op_factory *f, uint32_t flags, qb_operand *operands, uint32_t operand_count, qb_primitive_type *p_type);

typedef int32_t (*qb_link_results_proc)(qb_compiler_context *cxt, qb_op_factory *f, uint32_t flags, qb_operand *operands, uint32_t operand_count, qb_result_prototype *result_prototype);

typedef int32_t (*qb_validate_operands_proc)(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, uint32_t flags, qb_operand *operands, uint32_t operand_count, qb_result_destination *result_destination);

typedef int32_t (*qb_coerce_operands_proc)(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, uint32_t flags, qb_operand *operands, uint32_t operand_count);

typedef int32_t (*qb_set_result_proc)(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, uint32_t flags, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_result_prototype *result_prototype);

typedef int32_t (*qb_set_dimensions_proc)(qb_compiler_context *cxt, qb_op_factory *f, uint32_t flags, qb_operand *operands, uint32_t operand_count, qb_variable_dimensions *dim);

typedef int32_t (*qb_select_opcode_proc)(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, uint32_t flags, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_opcode *opcode);

typedef int32_t (*qb_transfer_operands_proc)(qb_compiler_context *cxt, qb_op_factory *f, uint32_t flags, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_operand *dest, uint32_t dest_count);

typedef int32_t (*qb_set_function_flags_proc)(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_operand *result);

typedef enum qb_string_type						qb_string_type;

#define OP_FACTORY_COMMON_ELEMENTS		\
	qb_produce_composite_proc produce_composite;	\
	qb_resolve_address_flags_proc resolve_flags;	\
	qb_resolve_expression_type_proc resolve_type;	\
	qb_link_results_proc link_results;	\
	qb_coerce_operands_proc coerce_operands;	\
	qb_set_result_proc set_preliminary_result;	\
	qb_validate_operands_proc validate_operands;	\
	qb_set_result_proc set_final_result;	\
	qb_set_dimensions_proc set_dimensions;	\
	qb_select_opcode_proc select_opcode;	\
	qb_transfer_operands_proc transfer_operands;	\
	qb_set_function_flags_proc set_function_flags;	\
	uint32_t coercion_flags;	\
	uint32_t result_flags;	\

enum qb_string_type {
	QB_STRING_UTF8,
	QB_STRING_UTF16,
	QB_STRING_UTF32,
};

struct qb_op_factory {
	OP_FACTORY_COMMON_ELEMENTS
}; 

struct qb_derived_op_factory {
	OP_FACTORY_COMMON_ELEMENTS
	void *parent;
}; 

struct qb_basic_op_factory {
	OP_FACTORY_COMMON_ELEMENTS
	qb_opcode opcodes[10];
}; 

struct qb_float_op_factory {
	OP_FACTORY_COMMON_ELEMENTS
	qb_opcode opcodes[2];
}; 

struct qb_simple_op_factory {
	OP_FACTORY_COMMON_ELEMENTS
	qb_opcode opcode;
};

struct qb_fetch_op_factory {
	OP_FACTORY_COMMON_ELEMENTS
	uint32_t bound_check_flags;
}; 

struct qb_unset_op_factory {
	OP_FACTORY_COMMON_ELEMENTS
	qb_opcode no_resizing_opcodes[10];
	qb_opcode resizing_opcodes[10];
	qb_opcode resizing_dim_opcodes[10];
	qb_opcode scalar_opcodes[10];
};

struct qb_unset_element_op_factory {
	OP_FACTORY_COMMON_ELEMENTS
	qb_opcode no_resizing_opcodes[10];
	qb_opcode resizing_opcodes[10];
	qb_opcode resizing_dim_opcodes[10];
};

struct qb_copy_op_factory {
	OP_FACTORY_COMMON_ELEMENTS
	qb_opcode opcodes[10][10];
	qb_opcode vector_opcodes[3][2];
};

struct qb_gather_op_factory {
	OP_FACTORY_COMMON_ELEMENTS
	qb_opcode opcodes[7][10];
};

struct qb_cast_op_factory {
	OP_FACTORY_COMMON_ELEMENTS
	void *parent;
	qb_primitive_type type;
}; 

struct qb_arithmetic_op_factory {
	OP_FACTORY_COMMON_ELEMENTS
	qb_opcode regular_opcodes[10];
	qb_opcode vector_opcodes[3][2];
};

struct qb_append_op_factory {
	OP_FACTORY_COMMON_ELEMENTS
	qb_opcode opcodes[3][10];
	qb_opcode multidim_opcodes[3][10];
	qb_opcode text_opcodes[3];
}; 

struct qb_print_op_factory {
	OP_FACTORY_COMMON_ELEMENTS
	qb_opcode opcodes[10];
	qb_opcode multidim_opcodes[10];
	qb_opcode text_opcodes[3];
}; 

struct qb_fcall_op_factory {
	OP_FACTORY_COMMON_ELEMENTS
	qb_opcode opcode;
};

struct qb_matrix_op_factory {
	OP_FACTORY_COMMON_ELEMENTS
	qb_opcode opcodes_any_size[2];
	qb_opcode opcodes_fixed_size[3][2];
	qb_opcode opcode_3x3_padded;
};

struct qb_equivalent_matrix_op_factory {
	OP_FACTORY_COMMON_ELEMENTS
	qb_matrix_op_factory *factory;
};

struct qb_matrix_op_factory_selector {
	OP_FACTORY_COMMON_ELEMENTS
	qb_op_factory *cm_factory;
	qb_op_factory *rm_factory;
};

struct qb_pixel_op_factory {
	OP_FACTORY_COMMON_ELEMENTS
	qb_opcode opcodes[4][2];
}; 

struct qb_utf8_op_factory {
	OP_FACTORY_COMMON_ELEMENTS
	qb_opcode ucs16_opcode;
	qb_opcode ucs32_opcode;
};

struct qb_array_resize_op_factory {
	OP_FACTORY_COMMON_ELEMENTS
	qb_opcode opcodes[8][10];
};

struct qb_php_function_result_factory {
	OP_FACTORY_COMMON_ELEMENTS
	const char *function_name;
	int32_t argument_types[4];
};

struct qb_complex_op_factory_selector {
	OP_FACTORY_COMMON_ELEMENTS
	void *real_factory;
	void *complex_factory;
};

static zend_always_inline qb_op_factory * qb_select_complex_op_factory(qb_op_factory *f, uint32_t flags) {
	qb_complex_op_factory_selector *s = (qb_complex_op_factory_selector *) f;
	if(flags & QB_ADDRESS_COMPLEX) {
		f = s->complex_factory;
	} else {
		f = s->real_factory;
	}
	return f;
}

struct qb_multiply_factory_selector {
	OP_FACTORY_COMMON_ELEMENTS
	void *real_factory;
	void *complex_factory;
	void *vector_vector_factory;
	void *matrix_matrix_factory;
	void *matrix_vector_factory;
	void *vector_matrix_factory;
};

static zend_always_inline qb_op_factory * qb_select_multiply_factory(qb_op_factory *f, uint32_t flags) {
	qb_multiply_factory_selector *s = (qb_multiply_factory_selector *) f;
	if(flags & QB_ADDRESS_MM_RESULT) {
		f = s->matrix_matrix_factory;
	} else if(flags & QB_ADDRESS_MV_RESULT) {
		f = s->matrix_vector_factory;
	} else if(flags & QB_ADDRESS_VM_RESULT) {
		f = s->vector_matrix_factory;
	} else if(flags & QB_ADDRESS_VV_RESULT) {
		f = s->vector_vector_factory;
	} else if(flags & QB_ADDRESS_COMPLEX) {
		f = s->complex_factory;
	} else {
		f = s->real_factory;
	}
	return f;
}

struct qb_intrinsic_op_factory {
	qb_produce_composite_proc produce_composite;
};

struct qb_op_decomposer {
	qb_produce_composite_proc produce_composite;
	void *factory;
};

struct qb_minmax_decomposer {
	qb_produce_composite_proc produce_composite;
	void *pairwise_factory;
	void *array_factory;
};

struct qb_fetch_do_op_decomposer {
	qb_produce_composite_proc produce_composite;
	void *fetch_factory;
	void *do_factory;
};

struct qb_set_op_chooser {
	qb_produce_composite_proc produce_composite;
	void *scalar_factory;
	void *set_factory;
};

struct qb_fork_decomposer {
	qb_produce_composite_proc produce_composite;
	void *init_factory;
	void *resume_factory;
	void *result_factory;
};

struct qb_round_decomposer {
	qb_produce_composite_proc produce_composite;
	void *simple_factory;
	void *precision_factory;
};

extern qb_op_factory factory_nop;

extern qb_copy_op_factory factory_assign;
extern qb_derived_op_factory factory_assign_array_element;
extern qb_fetch_do_op_decomposer factory_assign_object_property;
extern qb_derived_op_factory factory_assign_branching;
extern qb_derived_op_factory factory_assign_select;
extern qb_derived_op_factory factory_assign_temporary;
extern qb_derived_op_factory factory_assign_return_value;
extern qb_derived_op_factory factory_assign_generator_key;
extern qb_op_factory factory_sent_value;
extern qb_op_factory factory_assign_ref;
extern qb_op_decomposer factory_select;

extern qb_op_factory factory_fetch_array_size;
extern qb_fetch_op_factory factory_fetch_array_element_read;
extern qb_fetch_op_factory factory_fetch_array_element_write;
extern qb_fetch_op_factory factory_fetch_array_element_isset;
extern qb_fetch_op_factory factory_fetch_object_property_read;
extern qb_fetch_op_factory factory_fetch_object_property_write;
extern qb_fetch_op_factory factory_fetch_object_property_isset;
extern qb_op_factory factory_fetch_constant;

extern void *factories_fetch_variable[4];
extern void *factories_fetch_class[4];

extern qb_gather_op_factory factory_gather;
extern qb_gather_op_factory factory_scatter;

extern qb_op_factory factory_array_init;
extern qb_op_factory factory_array_append;

extern void *factories_fcall[5];

extern qb_op_factory factory_free;

extern qb_simple_op_factory factory_guard_array_index;
extern qb_simple_op_factory factory_guard_array_index_add;
extern qb_simple_op_factory factory_guard_array_index_multiply;
extern qb_simple_op_factory factory_guard_array_index_multiply_add;
extern qb_simple_op_factory factory_guard_array_size;
extern qb_simple_op_factory factory_guard_array_size_exact;
extern qb_simple_op_factory factory_guard_array_extent;
extern qb_simple_op_factory factory_guard_array_extent_multiply;
extern qb_simple_op_factory factory_guard_array_extent_subtract;
extern qb_simple_op_factory factory_guard_array_extent_subtract_multiply;

extern qb_simple_op_factory factory_check_array_index_add;
extern qb_simple_op_factory factory_check_array_index_multiply;
extern qb_simple_op_factory factory_check_array_index_multiply_add;

extern qb_simple_op_factory factory_accommodate_array_index;
extern qb_simple_op_factory factory_accommodate_array_index_multiply;
extern qb_simple_op_factory factory_accommodate_array_push;
extern qb_simple_op_factory factory_accommodate_array_size;
extern qb_simple_op_factory factory_accommodate_array_size_update_dimension;
extern qb_basic_op_factory factory_accommodate_array_size_copy_dimension;

extern qb_simple_op_factory factory_choose_size_of_larger_array;
extern qb_simple_op_factory factory_choose_size_of_larger_array_top_level;
extern qb_simple_op_factory factory_choose_size_of_largest_of_three_arrays;
extern qb_simple_op_factory factory_choose_size_of_largest_of_three_arrays_top_level;

extern qb_simple_op_factory factory_index_multiply;

extern qb_cast_op_factory factory_cast_S08;
extern qb_cast_op_factory factory_cast_U08;
extern qb_cast_op_factory factory_cast_S16;
extern qb_cast_op_factory factory_cast_U16;
extern qb_cast_op_factory factory_cast_S32;
extern qb_cast_op_factory factory_cast_U32;
extern qb_cast_op_factory factory_cast_S64;
extern qb_cast_op_factory factory_cast_U64;
extern qb_cast_op_factory factory_cast_F32;
extern qb_cast_op_factory factory_cast_F64;

extern void *factories_cast[6];

extern qb_arithmetic_op_factory factory_increment_pre;
extern qb_arithmetic_op_factory factory_decrement_pre;
extern qb_derived_op_factory factory_increment_post;
extern qb_derived_op_factory factory_decrement_post;
extern qb_fetch_do_op_decomposer factory_increment_object_property_pre;
extern qb_fetch_do_op_decomposer factory_decrement_object_property_pre;
extern qb_fetch_do_op_decomposer factory_increment_object_property_post;
extern qb_fetch_do_op_decomposer factory_decrement_object_property_post;
extern qb_derived_op_factory factory_increment_generator_key;

extern qb_basic_op_factory factory_shift_left;
extern qb_basic_op_factory factory_shift_right;

extern void *factories_shift_left_assign[3];
extern void *factories_shift_right_assign[3];

extern qb_basic_op_factory factory_bitwise_not;
extern qb_basic_op_factory factory_bitwise_and;
extern qb_basic_op_factory factory_bitwise_or;
extern qb_basic_op_factory factory_bitwise_xor;

extern void *factories_bitwise_and_assign[3];
extern void *factories_bitwise_or_assign[3];
extern void *factories_bitwise_xor_assign[3];

extern qb_basic_op_factory factory_boolean_cast;
extern qb_basic_op_factory factory_logical_not;
extern qb_basic_op_factory factory_logical_and;
extern qb_basic_op_factory factory_logical_or;
extern qb_basic_op_factory factory_logical_xor;

extern qb_arithmetic_op_factory factory_add;
extern qb_arithmetic_op_factory factory_subtract;
extern qb_arithmetic_op_factory factory_real_multiply;
extern qb_arithmetic_op_factory factory_real_divide;
extern qb_arithmetic_op_factory factory_modulo;
extern qb_float_op_factory factory_floor_modulo;
extern qb_arithmetic_op_factory factory_multiply_add;
extern qb_derived_op_factory factory_reciprocal;

extern qb_multiply_factory_selector factory_multiply;
extern qb_complex_op_factory_selector factory_divide;

extern void *factories_add_assign[3];
extern void *factories_subtract_assign[3];
extern void *factories_multiply_assign[3];
extern void *factories_divide_assign[3];
extern void *factories_modulo_assign[3];
extern void *factories_pow_assign[3];

extern qb_basic_op_factory factory_set_equal;
extern qb_basic_op_factory factory_set_not_equal;
extern qb_basic_op_factory factory_set_less_than;
extern qb_basic_op_factory factory_set_less_equal;
extern qb_derived_op_factory factory_set_greater_equal;
extern qb_derived_op_factory factory_set_greater_than;
extern qb_simple_op_factory factory_set_not;
extern qb_simple_op_factory factory_all;
extern qb_simple_op_factory factory_any;
extern qb_set_op_chooser factory_set_or_scalar_less_than;
extern qb_set_op_chooser factory_set_or_scalar_less_equal;

extern qb_basic_op_factory factory_equal;
extern qb_basic_op_factory factory_not_equal;
extern qb_basic_op_factory factory_less_than;
extern qb_basic_op_factory factory_less_equal;
extern qb_derived_op_factory factory_identical;
extern qb_derived_op_factory factory_not_identical;
extern qb_derived_op_factory factory_case;

extern qb_basic_op_factory factory_array_element_isset;
extern qb_basic_op_factory factory_object_property_isset;
extern qb_unset_op_factory factory_unset;
extern qb_unset_element_op_factory factory_unset_array_element;
extern qb_unset_op_factory factory_unset_object_property;

extern qb_intrinsic_op_factory factory_intrinsic;
extern qb_simple_op_factory factory_zend_function_call;
extern qb_simple_op_factory factory_send_zend_argument;

extern qb_simple_op_factory factory_leave;
extern qb_simple_op_factory factory_interrupt;
extern qb_simple_op_factory factory_exit;
extern qb_simple_op_factory factory_jump;
extern qb_simple_op_factory factory_end_static;
extern qb_simple_op_factory factory_branch_on_true;
extern qb_simple_op_factory factory_branch_on_false;
extern qb_simple_op_factory factory_foreach_reset;
extern qb_simple_op_factory factory_foreach_fetch;
extern qb_simple_op_factory factory_loop;
extern qb_op_decomposer factory_return;
extern qb_op_decomposer factory_yield;
extern qb_op_decomposer factory_branch_on_true_set;

extern qb_fork_decomposer factory_fork;
extern qb_simple_op_factory factory_fork_count;
extern qb_simple_op_factory factory_spoon;

extern qb_append_op_factory factory_append_string;
extern qb_derived_op_factory factory_append_char;
extern qb_derived_op_factory factory_concat;
extern qb_derived_op_factory factory_concat_assign;
extern qb_print_op_factory factory_print;
extern qb_simple_op_factory factory_flush;
extern qb_derived_op_factory factory_echo;

extern qb_basic_op_factory factory_real_abs;
extern qb_float_op_factory factory_real_acos;
extern qb_float_op_factory factory_real_acosh;
extern qb_float_op_factory factory_real_asin;
extern qb_float_op_factory factory_real_asinh;
extern qb_float_op_factory factory_real_atan;
extern qb_float_op_factory factory_atan2;
extern qb_float_op_factory factory_real_atanh;
extern qb_float_op_factory factory_ceil;
extern qb_float_op_factory factory_clamp;
extern qb_float_op_factory factory_real_cos;
extern qb_float_op_factory factory_real_cosh;
extern qb_float_op_factory factory_deg2rad;
extern qb_float_op_factory factory_real_exp;
extern qb_float_op_factory factory_exp2;
extern qb_float_op_factory factory_expm1;
extern qb_float_op_factory factory_floor;
extern qb_float_op_factory factory_fract;
extern qb_float_op_factory factory_hypot;
extern qb_float_op_factory factory_is_finite;
extern qb_float_op_factory factory_is_infinite;
extern qb_float_op_factory factory_is_nan;
extern qb_float_op_factory factory_real_log;
extern qb_float_op_factory factory_log10;
extern qb_float_op_factory factory_log1p;
extern qb_float_op_factory factory_log2;
extern qb_float_op_factory factory_mix;
extern qb_float_op_factory factory_real_pow;
extern qb_float_op_factory factory_rad2deg;
extern qb_float_op_factory factory_rint;
extern qb_round_decomposer factory_round;
extern qb_float_op_factory factory_rsqrt;
extern qb_float_op_factory factory_sign;
extern qb_float_op_factory factory_real_sin;
extern qb_float_op_factory factory_real_sinh;
extern qb_float_op_factory factory_smooth_step;
extern qb_float_op_factory factory_real_sqrt;
extern qb_float_op_factory factory_step;
extern qb_float_op_factory factory_real_tan;
extern qb_float_op_factory factory_real_tanh;

extern qb_complex_op_factory_selector factory_abs;
extern qb_complex_op_factory_selector factory_acos;
extern qb_complex_op_factory_selector factory_acosh;
extern qb_complex_op_factory_selector factory_asin;
extern qb_complex_op_factory_selector factory_asinh;
extern qb_complex_op_factory_selector factory_atan;
extern qb_complex_op_factory_selector factory_atanh;
extern qb_complex_op_factory_selector factory_cos;
extern qb_complex_op_factory_selector factory_cosh;
extern qb_complex_op_factory_selector factory_exp;
extern qb_complex_op_factory_selector factory_log;
extern qb_complex_op_factory_selector factory_pow;
extern qb_complex_op_factory_selector factory_sin;
extern qb_complex_op_factory_selector factory_sinh;
extern qb_complex_op_factory_selector factory_sqrt;
extern qb_complex_op_factory_selector factory_tan;
extern qb_complex_op_factory_selector factory_tanh;

extern qb_minmax_decomposer factory_max;
extern qb_minmax_decomposer factory_min;
extern qb_basic_op_factory factory_min_two;
extern qb_basic_op_factory factory_max_two;

extern qb_basic_op_factory factory_mt_rand;
extern qb_basic_op_factory factory_rand;
extern qb_float_op_factory factory_lcg;

extern qb_float_op_factory factory_complex_add;
extern qb_float_op_factory factory_complex_subtract;
extern qb_float_op_factory factory_complex_divide;
extern qb_float_op_factory factory_complex_multiply;

extern qb_float_op_factory factory_complex_abs;
extern qb_float_op_factory factory_complex_acos;
extern qb_float_op_factory factory_complex_acosh;
extern qb_float_op_factory factory_complex_arg;
extern qb_float_op_factory factory_complex_asin;
extern qb_float_op_factory factory_complex_asinh;
extern qb_float_op_factory factory_complex_atan;
extern qb_float_op_factory factory_complex_atanh;
extern qb_float_op_factory factory_complex_cos;
extern qb_float_op_factory factory_complex_cosh;
extern qb_float_op_factory factory_complex_exp;
extern qb_float_op_factory factory_complex_log;
extern qb_float_op_factory factory_complex_pow;
extern qb_float_op_factory factory_complex_sin;
extern qb_float_op_factory factory_complex_sinh;
extern qb_float_op_factory factory_complex_sqrt;
extern qb_float_op_factory factory_complex_tan;
extern qb_float_op_factory factory_complex_tanh;

extern qb_matrix_op_factory factory_cross_product;
extern qb_matrix_op_factory factory_distance;
extern qb_matrix_op_factory factory_dot_product;
extern qb_matrix_op_factory factory_faceforward;
extern qb_matrix_op_factory factory_length;
extern qb_matrix_op_factory factory_normalize;
extern qb_matrix_op_factory factory_reflect;
extern qb_matrix_op_factory factory_refract;

extern qb_simple_op_factory factory_validate_cross_product_2d;
extern qb_simple_op_factory factory_validate_cross_product_3d;
extern qb_simple_op_factory factory_validate_cross_product_4d;
extern qb_simple_op_factory factory_validate_vector_width;

extern qb_matrix_op_factory factory_mm_mult_cm;
extern qb_matrix_op_factory factory_mv_mult_cm;
extern qb_matrix_op_factory factory_vm_mult_cm;
extern qb_derived_op_factory factory_mm_mult_rm;
extern qb_derived_op_factory factory_mv_mult_rm;
extern qb_derived_op_factory factory_vm_mult_rm;
extern qb_matrix_op_factory_selector factory_mm_mult; 
extern qb_matrix_op_factory_selector factory_mv_mult; 
extern qb_matrix_op_factory_selector factory_vm_mult; 
extern qb_matrix_op_factory factory_transform_cm;
extern qb_matrix_op_factory factory_transform_rm;
extern qb_matrix_op_factory_selector factory_transform;

extern qb_simple_op_factory factory_validate_mm_mult;
extern qb_simple_op_factory factory_validate_mv_mult;
extern qb_simple_op_factory factory_validate_vm_mult;

extern qb_matrix_op_factory factory_determinant;
extern qb_matrix_op_factory factory_inverse;
extern qb_matrix_op_factory factory_transpose;

extern qb_simple_op_factory factory_validate_square_matrix;

extern qb_pixel_op_factory factory_alpha_blend;
extern qb_pixel_op_factory factory_apply_premult;
extern qb_pixel_op_factory factory_remove_premult;
extern qb_pixel_op_factory factory_hsl2rgb;
extern qb_pixel_op_factory factory_hsv2rgb;
extern qb_pixel_op_factory factory_rgb2hsl;
extern qb_pixel_op_factory factory_rgb2hsv;
extern qb_pixel_op_factory factory_sample_nearest;
extern qb_derived_op_factory factory_sample_nearest_vector;
extern qb_pixel_op_factory factory_sample_bilinear;
extern qb_derived_op_factory factory_sample_bilinear_vector;

extern qb_basic_op_factory factory_array_column;
extern qb_op_decomposer factory_array_diff;
extern qb_basic_op_factory factory_array_diff_count;
extern qb_basic_op_factory factory_array_fill;
extern qb_op_decomposer factory_array_intersect;
extern qb_basic_op_factory factory_array_intersect_count;
extern qb_op_decomposer factory_array_merge;
extern qb_basic_op_factory factory_array_pad;
extern qb_simple_op_factory factory_array_pad_count;
extern qb_op_decomposer factory_array_pop;
extern qb_basic_op_factory factory_array_pos;
extern qb_basic_op_factory factory_array_product;
extern qb_op_decomposer factory_array_push;
extern qb_simple_op_factory factory_array_rand;
extern qb_basic_op_factory factory_array_replace;
extern qb_array_resize_op_factory factory_array_resize;
extern qb_basic_op_factory factory_array_reverse;
extern qb_basic_op_factory factory_array_rpos;
extern qb_basic_op_factory factory_array_search;
extern qb_op_decomposer factory_array_shift;
extern qb_basic_op_factory factory_array_slice;
extern qb_simple_op_factory factory_array_slice_count;
extern qb_op_decomposer factory_array_splice;
extern qb_basic_op_factory factory_array_sum;
extern qb_basic_op_factory factory_array_unique;
extern qb_basic_op_factory factory_array_unique_count;
extern qb_op_decomposer factory_array_unshift;
extern qb_op_decomposer factory_in_array;
extern qb_basic_op_factory factory_range;
extern qb_basic_op_factory factory_range_count;
extern qb_basic_op_factory factory_rsort;
extern qb_basic_op_factory factory_shuffle;
extern qb_basic_op_factory factory_sort;
extern qb_derived_op_factory factory_substr;

extern qb_utf8_op_factory factory_utf8_decode;
extern qb_utf8_op_factory factory_utf8_decode_count;
extern qb_utf8_op_factory factory_utf8_encode;
extern qb_utf8_op_factory factory_utf8_encode_count;

extern qb_basic_op_factory factory_pack_le;
extern qb_basic_op_factory factory_pack_be;
extern qb_basic_op_factory factory_unpack_le;
extern qb_basic_op_factory factory_unpack_be;

extern qb_simple_op_factory factory_get_time;

extern qb_basic_op_factory factory_intval;
extern qb_basic_op_factory factory_floatval;

extern qb_op_factory factory_define;
extern qb_op_factory factory_defined;

extern qb_php_function_result_factory factory_phpversion;
extern qb_php_function_result_factory factory_zend_version;
extern qb_php_function_result_factory factory_version_compare;
extern qb_php_function_result_factory factory_function_exists;
extern qb_php_function_result_factory factory_class_exists;
extern qb_php_function_result_factory factory_method_exists;
extern qb_php_function_result_factory factory_property_exists;
extern qb_php_function_result_factory factory_ini_get;
extern qb_php_function_result_factory factory_ini_set;
extern qb_php_function_result_factory factory_ini_restore;
extern qb_php_function_result_factory factory_get_cfg_var;
extern qb_php_function_result_factory factory_getenv;
extern qb_php_function_result_factory factory_putenv;

extern qb_simple_op_factory factory_ext;
extern qb_simple_op_factory factory_synchronize_debug_variable;

#endif
