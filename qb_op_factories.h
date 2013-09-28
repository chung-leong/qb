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

typedef struct qb_op_factory					qb_op_factory; 
typedef struct qb_derived_op_factory			qb_derived_op_factory; 
typedef struct qb_basic_op_factory				qb_basic_op_factory; 
typedef struct qb_float_op_factory				qb_float_op_factory; 
typedef struct qb_simple_op_factory				qb_simple_op_factory;
typedef struct qb_copy_op_factory				qb_copy_op_factory;
typedef struct qb_cast_op_factory				qb_cast_op_factory;
typedef struct qb_comparison_op_factory			qb_comparison_op_factory;
typedef struct qb_comparison_branch_op_factory	qb_comparison_branch_op_factory;
typedef struct qb_arithmetic_op_factory			qb_arithmetic_op_factory;
typedef struct qb_string_op_factory				qb_string_op_factory;
typedef struct qb_fcall_op_factory				qb_fcall_op_factory;
typedef struct qb_minmax_op_factory				qb_minmax_op_factory;
typedef struct qb_vector_op_factory				qb_vector_op_factory;
typedef struct qb_matrix_op_factory				qb_matrix_op_factory;
typedef struct qb_equivalent_matrix_op_factory	qb_equivalent_matrix_op_factory;
typedef struct qb_matrix_op_factory_selector	qb_matrix_op_factory_selector;
typedef struct qb_pixel_op_factory				qb_pixel_op_factory;

typedef qb_primitive_type (ZEND_FASTCALL *qb_resolve_expression_type_proc)(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count);

typedef void (ZEND_FASTCALL *qb_link_results_proc)(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_result_prototype *result_prototype);

typedef void (ZEND_FASTCALL *qb_validate_operands_proc)(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count);

typedef void (ZEND_FASTCALL *qb_coerce_operands_proc)(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count);

typedef void (ZEND_FASTCALL *qb_set_result_proc)(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_result_prototype *result_prototype);

typedef void (ZEND_FASTCALL *qb_set_dimensions_proc)(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_variable_dimensions *dim);

typedef uint32_t (ZEND_FASTCALL *qb_get_operand_count_proc)(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count);

typedef qb_opcode (ZEND_FASTCALL *qb_select_opcode_proc)(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_operand *result);

typedef void (ZEND_FASTCALL *qb_transfer_operands_proc)(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_operand *dest);

#define OP_FACTORY_COMMON_ELEMENTS		\
	qb_resolve_expression_type_proc resolve_type;	\
	qb_link_results_proc link_results;	\
	qb_coerce_operands_proc coerce_operands;	\
	qb_set_result_proc set_preliminary_result;	\
	qb_validate_operands_proc validate_operands;	\
	qb_set_result_proc set_final_result;	\
	qb_set_dimensions_proc set_dimensions;	\
	qb_select_opcode_proc select_opcode;	\
	qb_get_operand_count_proc get_operand_count;	\
	qb_transfer_operands_proc transfer_operands;	\
	uint32_t coercion_flags;	\
	uint32_t result_flags;	\
	uint32_t address_flags;	\

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

struct qb_copy_op_factory {
	OP_FACTORY_COMMON_ELEMENTS
	qb_opcode opcodes[10][10];
	qb_opcode vector_opcodes[3][2];
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

struct qb_string_op_factory {
	OP_FACTORY_COMMON_ELEMENTS
	qb_opcode opcodes[10];
	qb_opcode multidim_opcodes[10];
	qb_opcode text_opcode;
}; 

struct qb_fcall_op_factory {
	OP_FACTORY_COMMON_ELEMENTS
	qb_opcode opcode;
};

struct qb_minmax_op_factory {
	OP_FACTORY_COMMON_ELEMENTS
	qb_opcode opcodes_pair[10];
	qb_opcode opcodes_list[10];
};

struct qb_vector_op_factory {
	OP_FACTORY_COMMON_ELEMENTS
	qb_opcode opcodes_any_size[2];
	qb_opcode opcodes_fixed_size[3][2];
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
	qb_opcode opcodes[2][2];
}; 

extern qb_simple_op_factory factory_nop;

extern qb_copy_op_factory factory_assignment;
extern qb_derived_op_factory factory_array_element_assignment;
extern qb_derived_op_factory factory_object_property_assignment;

extern qb_simple_op_factory factory_ref_assignment;

extern qb_simple_op_factory factory_fetch_array_element;
extern qb_simple_op_factory factory_fetch_object_property;
extern qb_simple_op_factory factory_fetch_constant;

extern void *factories_fetch_variable[4];
extern void *factories_fetch_class[3];

extern qb_simple_op_factory factory_array_init;
extern qb_simple_op_factory factory_array_append;

extern qb_simple_op_factory factory_free;

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

extern qb_arithmetic_op_factory factory_increment_pre;
extern qb_arithmetic_op_factory factory_decrement_pre;
extern qb_derived_op_factory factory_increment_post;
extern qb_derived_op_factory factory_decrement_post;

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

extern qb_basic_op_factory factory_boolean;
extern qb_basic_op_factory factory_logical_not;
extern qb_basic_op_factory factory_logical_and;
extern qb_basic_op_factory factory_logical_or;
extern qb_basic_op_factory factory_logical_xor;

extern qb_arithmetic_op_factory factory_add;
extern qb_arithmetic_op_factory factory_subtract;
extern qb_arithmetic_op_factory factory_multiply;
extern qb_arithmetic_op_factory factory_divide;
extern qb_arithmetic_op_factory factory_modulo;
extern qb_float_op_factory factory_floor_modulo;

extern void *factories_add_assign[3];
extern void *factories_subtract_assign[3];
extern void *factories_multiply_assign[3];
extern void *factories_divide_assign[3];
extern void *factories_modulo_assign[3];

extern qb_basic_op_factory factory_set_equal;
extern qb_basic_op_factory factory_set_not_equal;
extern qb_basic_op_factory factory_set_less_than;
extern qb_basic_op_factory factory_set_less_equal;
extern qb_derived_op_factory factory_set_greater_equal;
extern qb_derived_op_factory factory_set_greater_than;
extern qb_simple_op_factory factory_set_not;
extern qb_simple_op_factory factory_all;
extern qb_simple_op_factory factory_any;

extern qb_basic_op_factory factory_equal;
extern qb_basic_op_factory factory_not_equal;
extern qb_basic_op_factory factory_less_than;
extern qb_basic_op_factory factory_less_equal;
extern qb_derived_op_factory factory_identical;
extern qb_derived_op_factory factory_not_identical;

extern qb_basic_op_factory factory_branch_on_equal;
extern qb_basic_op_factory factory_branch_on_not_equal;
extern qb_basic_op_factory factory_branch_on_less_than;
extern qb_basic_op_factory factory_branch_on_less_equal;

extern qb_simple_op_factory factory_return;
extern qb_simple_op_factory factory_exit;
extern qb_simple_op_factory factory_jump;
extern qb_simple_op_factory factory_branch_on_true;
extern qb_simple_op_factory factory_branch_on_false;
extern qb_simple_op_factory factory_ext;

extern qb_string_op_factory factory_print;
extern qb_derived_op_factory factory_echo;

extern qb_float_op_factory factory_sin;
extern qb_float_op_factory factory_asin;
extern qb_float_op_factory factory_cos;
extern qb_float_op_factory factory_acos;
extern qb_float_op_factory factory_tan;
extern qb_float_op_factory factory_atan;
extern qb_float_op_factory factory_atan2;
extern qb_float_op_factory factory_sinh;
extern qb_float_op_factory factory_asinh;
extern qb_float_op_factory factory_cosh;
extern qb_float_op_factory factory_acosh;
extern qb_float_op_factory factory_tanh;
extern qb_float_op_factory factory_atanh;
extern qb_float_op_factory factory_ceil;
extern qb_float_op_factory factory_floor;
extern qb_float_op_factory factory_fract;
extern qb_float_op_factory factory_round;
extern qb_float_op_factory factory_log;
extern qb_float_op_factory factory_log1p;
extern qb_float_op_factory factory_log10;
extern qb_float_op_factory factory_log2;
extern qb_float_op_factory factory_exp;
extern qb_float_op_factory factory_exp2;
extern qb_float_op_factory factory_expm1;
extern qb_float_op_factory factory_sqrt;
extern qb_float_op_factory factory_rsqrt;
extern qb_float_op_factory factory_pow;
extern qb_float_op_factory factory_hypot;
extern qb_float_op_factory factory_sign;
extern qb_float_op_factory factory_step;
extern qb_float_op_factory factory_clamp;
extern qb_float_op_factory factory_mix;
extern qb_float_op_factory factory_smooth_step;
extern qb_float_op_factory factory_deg2rad;
extern qb_float_op_factory factory_rad2deg;
extern qb_float_op_factory factory_is_finite;
extern qb_float_op_factory factory_is_infinite;
extern qb_float_op_factory factory_is_nan;
extern qb_basic_op_factory factory_abs;
extern qb_minmax_op_factory factory_min;
extern qb_minmax_op_factory factory_max;
extern qb_basic_op_factory factory_rand;
extern qb_basic_op_factory factory_mt_rand;
extern qb_float_op_factory factory_lcg;

#endif