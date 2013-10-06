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

#include "qb_op_factory_operand_validation.c"
#include "qb_op_factory_type_resolution.c"
#include "qb_op_factory_result_linkage.c"
#include "qb_op_factory_operand_coercion.c"
#include "qb_op_factory_dimension_calculation.c"
#include "qb_op_factory_result_assignment.c"
#include "qb_op_factory_opcode_selection.c"
#include "qb_op_factory_operand_transfer.c"

qb_op_factory factory_nop = {
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	0,
	0,
	0,
};

qb_copy_op_factory factory_assign = {
	qb_resolve_expression_type_first_operand,
	qb_link_results_variable,
	qb_coerce_operands_assign,
	qb_set_result_prototype,
	NULL,
	qb_set_result_assign,
	NULL,
	qb_select_opcode_assign,
	NULL,
	qb_transfer_operands_assign,
	0,
	QB_RESULT_HAS_SIDE_EFFECT,
	0,
	{
		{	QB_MOV_F64_F64,	QB_MOV_F64_F32,	QB_MOV_F64_U64,	QB_MOV_F64_S64,	QB_MOV_F64_U32,	QB_MOV_F64_S32,	QB_MOV_F64_U16,	QB_MOV_F64_S16,	QB_MOV_F64_U08,	QB_MOV_F64_S08,	},
		{	QB_MOV_F32_F64,	QB_MOV_F32_F32,	QB_MOV_F32_U64,	QB_MOV_F32_S64,	QB_MOV_F32_U32,	QB_MOV_F32_S32,	QB_MOV_F32_U16,	QB_MOV_F32_S16,	QB_MOV_F32_U08,	QB_MOV_F32_S08,	},
		{	QB_MOV_U64_F64,	QB_MOV_U64_F32,	QB_MOV_I64_I64,	QB_MOV_I64_I64,	QB_MOV_I64_I32,	QB_MOV_I64_I32,	QB_MOV_I64_I16,	QB_MOV_I64_I16,	QB_MOV_I64_I08,	QB_MOV_I64_I08,	},
		{	QB_MOV_S64_F64,	QB_MOV_S64_F32,	QB_MOV_I64_I64,	QB_MOV_I64_I64,	QB_MOV_I64_I32,	QB_MOV_I64_I32,	QB_MOV_I64_I16,	QB_MOV_I64_I16,	QB_MOV_I64_I08,	QB_MOV_I64_I08,	},
		{	QB_MOV_U32_F64,	QB_MOV_U32_F32,	QB_MOV_U32_I64,	QB_MOV_U32_I64,	QB_MOV_I32_I32,	QB_MOV_I32_I32,	QB_MOV_I32_I16,	QB_MOV_I32_I16,	QB_MOV_I32_I08,	QB_MOV_I32_I08,	},
		{	QB_MOV_S32_F64,	QB_MOV_S32_F32,	QB_MOV_S32_I64,	QB_MOV_S32_I64,	QB_MOV_I32_I32,	QB_MOV_I32_I32,	QB_MOV_I32_I16,	QB_MOV_I32_I16,	QB_MOV_I32_I08,	QB_MOV_I32_I08,	},
		{	QB_MOV_U16_F64,	QB_MOV_U16_F32,	QB_MOV_U16_I64,	QB_MOV_U16_I64,	QB_MOV_U16_I32,	QB_MOV_U16_I32,	QB_MOV_I16_I16,	QB_MOV_I16_I16,	QB_MOV_I16_I08,	QB_MOV_I16_I08,	},
		{	QB_MOV_S16_F64,	QB_MOV_S16_F32,	QB_MOV_S16_I64,	QB_MOV_S16_I64,	QB_MOV_S16_I32,	QB_MOV_S16_I32,	QB_MOV_I16_I16,	QB_MOV_I16_I16,	QB_MOV_I16_I08,	QB_MOV_I16_I08,	},
		{	QB_MOV_S08_F64,	QB_MOV_S08_F32,	QB_MOV_S08_I64,	QB_MOV_S08_I64,	QB_MOV_S08_I32,	QB_MOV_S08_I32,	QB_MOV_S08_I16,	QB_MOV_S08_I16,	QB_MOV_I08_I08,	QB_MOV_I08_I08,	},
		{	QB_MOV_U08_F64,	QB_MOV_U08_F32,	QB_MOV_U08_I64,	QB_MOV_U08_I64,	QB_MOV_U08_I32,	QB_MOV_U08_I32,	QB_MOV_U08_I16,	QB_MOV_S08_I16,	QB_MOV_I08_I08,	QB_MOV_I08_I08,	},
	},
	{
		{	QB_MOV_2X_F64_F64,	QB_MOV_2X_F32_F32,	},
		{	QB_MOV_3X_F64_F64,	QB_MOV_3X_F32_F32,	},
		{	QB_MOV_4X_F64_F64,	QB_MOV_4X_F32_F32,	},
	},
};

qb_derived_op_factory factory_assign_array_element = {
	qb_resolve_expression_type_first_operand,
	qb_link_results_array_element,
	qb_coerce_operands_assign_array_element,
	qb_set_result_prototype,
	qb_validate_operands_array_element,
	qb_set_result_assign_array_element,
	NULL,
	qb_select_opcode_derived,
	NULL,
	qb_transfer_operands_assign,
	0,
	QB_RESULT_HAS_SIDE_EFFECT,
	0,
	&factory_assign,
};

qb_derived_op_factory factory_assign_object_property = {
	qb_resolve_expression_type_object_property,
	qb_link_results_object_property,
	qb_coerce_operands_assign_object_property,
	qb_set_result_prototype,
	qb_validate_operands_object_property,
	qb_set_result_assign_object_property,
	NULL,
	qb_select_opcode_derived,
	NULL,
	qb_transfer_operands_assign,
	0,
	QB_RESULT_HAS_SIDE_EFFECT,
	0,
	&factory_assign,
};

qb_op_factory factory_assign_ref = {
	NULL,
	NULL,
	NULL,
	NULL,
	qb_validate_operands_assign_ref,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	0,
	0,
	0,
};

qb_op_factory factory_fetch_local = {
	qb_resolve_expression_type_fetch_local,
	NULL,
	NULL,
	qb_set_result_fetch_local,
	NULL,
	qb_set_result_fetch_local,
	NULL,
	NULL,
	NULL,
	NULL,
	0,
	0,
	0,
};

qb_op_factory factory_fetch_global = {
	qb_resolve_expression_type_fetch_global,
	NULL,
	NULL,
	qb_set_result_fetch_global,
	NULL,
	qb_set_result_fetch_global,
	NULL,
	NULL,
	NULL,
	NULL,
	0,
	0,
	0,
};

qb_op_factory factory_fetch_static = {
	qb_resolve_expression_type_fetch_static,
	NULL,
	NULL,
	qb_set_result_fetch_static,
	NULL,
	qb_set_result_fetch_static,
	NULL,
	NULL,
	NULL,
	NULL,
	0,
	0,
	0,
};

qb_op_factory factory_fetch_class = {
	qb_resolve_expression_type_fetch_class,
	NULL,
	NULL,
	qb_set_result_fetch_class,
	NULL,
	qb_set_result_fetch_class,
	NULL,
	NULL,
	NULL,
	NULL,
	0,
	0,
	0,
};

void *factories_fetch_variable[] = { &factory_fetch_local, &factory_fetch_global, &factory_fetch_static, &factory_fetch_class };

qb_op_factory factory_fetch_array_size = {
	qb_resolve_expression_type_index,
	NULL,
	qb_coerce_operands_fetch_array_size,
	qb_set_result_prototype,
	qb_validate_operands_one_array,
	qb_set_result_fetch_array_size,
	NULL,
	NULL,
	NULL,
	NULL,
	0,
	0,
	0,
};

qb_fetch_op_factory factory_fetch_array_element_read = {
	qb_resolve_expression_type_first_operand,
	NULL,
	qb_coerce_operands_fetch_array_element,
	qb_set_result_prototype,
	qb_validate_operands_array_element,
	qb_set_result_fetch_array_element,
	NULL,
	NULL,
	NULL,
	NULL,
	0,
	0,
	0,
	QB_ARRAY_BOUND_CHECK_READ,
};

qb_fetch_op_factory factory_fetch_array_element_write = {
	qb_resolve_expression_type_first_operand,
	NULL,
	qb_coerce_operands_fetch_array_element,
	qb_set_result_prototype,
	qb_validate_operands_array_element,
	qb_set_result_fetch_array_element,
	NULL,
	NULL,
	NULL,
	NULL,
	0,
	0,
	0,
	QB_ARRAY_BOUND_CHECK_WRITE,
};

qb_fetch_op_factory factory_fetch_array_element_isset = {
	qb_resolve_expression_type_first_operand,
	NULL,
	qb_coerce_operands_fetch_array_element,
	qb_set_result_prototype,
	qb_validate_operands_array_element,
	qb_set_result_fetch_array_element,
	NULL,
	NULL,
	NULL,
	NULL,
	0,
	0,
	0,
	QB_ARRAY_BOUND_CHECK_ISSET,
};

qb_fetch_op_factory factory_fetch_object_property_read = {
	qb_resolve_expression_type_object_property,
	NULL,
	NULL,
	qb_set_result_prototype,
	qb_validate_operands_object_property,
	qb_set_result_fetch_object_property,
	NULL,
	NULL,
	NULL,
	NULL,
	0,
	0,
	0,
	QB_ARRAY_BOUND_CHECK_READ,
};

qb_fetch_op_factory factory_fetch_object_property_write = {
	qb_resolve_expression_type_object_property,
	NULL,
	NULL,
	qb_set_result_prototype,
	qb_validate_operands_object_property,
	qb_set_result_fetch_object_property,
	NULL,
	NULL,
	NULL,
	NULL,
	0,
	0,
	0,
	QB_ARRAY_BOUND_CHECK_WRITE,
};

qb_fetch_op_factory factory_fetch_object_property_isset = {
	qb_resolve_expression_type_object_property,
	NULL,
	NULL,
	qb_set_result_prototype,
	qb_validate_operands_object_property,
	qb_set_result_fetch_object_property,
	NULL,
	NULL,
	NULL,
	NULL,
	0,
	0,
	0,
	QB_ARRAY_BOUND_CHECK_ISSET,
};

qb_op_factory factory_fetch_class_self = {
	NULL,
	NULL,
	NULL,
	qb_set_result_fetch_class_self,
	qb_validate_operands_fetch_class_self,
	qb_set_result_fetch_class_self,
	NULL,
	NULL,
	NULL,
	NULL,
	0,
	0,
	0,
};

qb_op_factory factory_fetch_class_parent = {
	NULL,
	NULL,
	NULL,
	qb_set_result_fetch_class_parent,
	qb_validate_operands_fetch_class_parent,
	qb_set_result_fetch_class_parent,
	NULL,
	NULL,
	NULL,
	NULL,
	0,
	0,
	0,
};

qb_op_factory factory_fetch_class_static = {
	NULL,
	NULL,
	NULL,
	qb_set_result_fetch_class_static,
	qb_validate_operands_fetch_class_static,
	qb_set_result_fetch_class_static,
	NULL,
	NULL,
	NULL,
	NULL,
	0,
	0,
	0,
};

void *factories_fetch_class[] = { &factory_fetch_class_self, &factory_fetch_class_parent, &factory_fetch_class_static };

qb_op_factory factory_fetch_constant = {
	qb_resolve_expression_lvalue,
	NULL,
	NULL,
	qb_set_result_prototype,
	qb_validate_operands_fetch_constant,
	qb_set_result_fetch_constant,
	NULL,
	NULL,
	NULL,
	NULL,
	0,
	0,
	0,
};

qb_op_factory factory_array_init = {
	qb_resolve_expression_lvalue,
	qb_link_results_array_init,
	NULL,
	qb_set_result_prototype,
	qb_validate_array_init,
	qb_set_result_array_init,
	NULL,
	NULL,
	NULL,
	NULL,
	QB_COERCE_TO_LVALUE_TYPE,
	QB_RESULT_HAS_SIDE_EFFECT,
	0,
};

qb_op_factory factory_array_append = {
	NULL,
	qb_link_results_array_append,
	NULL,
	qb_set_result_prototype,
	qb_validate_array_append,
	qb_set_result_array_append,
	NULL,
	NULL,
	NULL,
	NULL,
	QB_COERCE_TO_LVALUE_TYPE,
	QB_RESULT_HAS_SIDE_EFFECT,
	0,
};

qb_op_factory factory_free = {
	NULL,
	qb_link_results_free,
	NULL,
	qb_set_result_prototype,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	0,
	0,
	0,
};

qb_simple_op_factory factory_guard_array_index = {
	qb_resolve_expression_type_index,
	NULL,
	NULL,
	NULL,
	NULL,
	qb_set_result_second_operand,
	NULL,
	qb_select_opcode_simple,
	NULL,
	qb_transfer_operands_guard_array_index,

	0,
	0,
	0,
	QB_IDX_GUARD_U32_U32,
};

qb_simple_op_factory factory_guard_array_index_add = {
	qb_resolve_expression_type_index,
	NULL,
	NULL,
	qb_set_result_prototype,
	NULL,
	qb_set_result_non_reusable_temporary_value,
	NULL,
	qb_select_opcode_simple,
	NULL,
	qb_transfer_operands_guard_array_index_add,

	0,
	0,
	QB_ADDRESS_TEMPORARY,
	QB_IDX_GUARD_ADD_U32_U32_U32_U32,
};

qb_simple_op_factory factory_guard_array_index_multiply = {
	qb_resolve_expression_type_index,
	NULL,
	NULL,
	qb_set_result_prototype,
	NULL,
	qb_set_result_non_reusable_temporary_value,
	NULL,
	qb_select_opcode_simple,
	NULL,
	qb_transfer_operands_guard_array_index_multiply,

	0,
	0,
	QB_ADDRESS_TEMPORARY,
	QB_IDX_GUARD_MUL_U32_U32_U32_U32,
};

qb_simple_op_factory factory_guard_array_index_multiply_add = {
	qb_resolve_expression_type_index,
	NULL,
	NULL,
	qb_set_result_prototype,
	NULL,
	qb_set_result_non_reusable_temporary_value,
	NULL,
	qb_select_opcode_simple,
	NULL,
	qb_transfer_operands_guard_array_index_multiply_add,

	0,
	0,
	QB_ADDRESS_TEMPORARY,
	QB_IDX_GUARD_MAC_U32_U32_U32_U32_U32,
};

qb_simple_op_factory factory_guard_array_size = {
	qb_resolve_expression_type_first_operand,
	NULL,
	NULL,
	NULL,
	NULL,
	qb_set_result_first_operand,
	NULL,
	qb_select_opcode_simple,
	NULL,
	qb_transfer_operands_guard_array_size,

	0,
	0,
	0,
	QB_SZ_GUARD_U32_U32,
};

qb_chained_op_factory factory_check_array_index_add = {
	qb_resolve_expression_type_index,
	NULL,
	NULL,
	qb_set_result_prototype,
	NULL,
	qb_set_result_check_array_index,
	NULL,
	qb_select_opcode_check_array_index,
	NULL,
	qb_transfer_operands_check_array_index_add,

	0,
	0,
	QB_ADDRESS_TEMPORARY,
	QB_IDX_CHECK_ADD_INIT_U32_U32_U32_I32_U32,
	QB_IDX_CHECK_ADD_U32_U32_U32_I32_U32,
};

qb_chained_op_factory factory_check_array_index_multiply = {
	qb_resolve_expression_type_index,
	NULL,
	NULL,
	qb_set_result_prototype,
	NULL,
	qb_set_result_check_array_index,
	NULL,
	qb_select_opcode_check_array_index,
	NULL,
	qb_transfer_operands_check_array_index_multiply,

	0,
	0,
	QB_ADDRESS_TEMPORARY,
	QB_IDX_CHECK_MUL_INIT_U32_U32_U32_I32_U32,
	QB_IDX_CHECK_MUL_U32_U32_U32_I32_U32,
};

qb_chained_op_factory factory_check_array_index_multiply_add = {
	qb_resolve_expression_type_index,
	NULL,
	NULL,
	qb_set_result_prototype,
	NULL,
	qb_set_result_check_array_index,
	NULL,
	qb_select_opcode_check_array_index,
	NULL,
	qb_transfer_operands_check_array_index_multiply_add,

	0,
	0,
	QB_ADDRESS_TEMPORARY,
	QB_IDX_CHECK_MAC_INIT_U32_U32_U32_U32_I32_U32,
	QB_IDX_CHECK_MAC_U32_U32_U32_U32_I32_U32,
};

qb_simple_op_factory factory_accommodate_array_index = {
	qb_resolve_expression_type_index,
	NULL,
	NULL,
	qb_set_result_prototype,
	NULL,
	qb_set_result_second_operand,
	NULL,
	qb_select_opcode_simple,
	NULL,
	qb_transfer_operands_accommodate_array_index,

	0,
	0,
	QB_ADDRESS_TEMPORARY,
	QB_IDX_ACCOM_U32_U32_U32_U32,
};

qb_simple_op_factory factory_accommodate_array_index_multiply = {
	qb_resolve_expression_type_index,
	NULL,
	NULL,
	qb_set_result_prototype,
	NULL,
	qb_set_result_non_reusable_temporary_value,
	NULL,
	qb_select_opcode_simple,
	NULL,
	qb_transfer_operands_accommodate_array_index_multiply,

	0,
	0,
	QB_ADDRESS_TEMPORARY,
	QB_IDX_ACCOM_MUL_U32_U32_U32_U32_U32_U32_U32,
};

qb_simple_op_factory factory_accommodate_array_push = {
	qb_resolve_expression_type_index,
	NULL,
	NULL,
	qb_set_result_prototype,
	NULL,
	qb_set_result_non_reusable_temporary_value,
	NULL,
	qb_select_opcode_simple,
	NULL,
	qb_transfer_operands_accommodate_array_push,

	0,
	0,
	QB_ADDRESS_TEMPORARY,
	QB_IDX_ACCOM_PUSH_U32_U32_U32_U32,
};

qb_simple_op_factory factory_accommodate_array_size = {
	qb_resolve_expression_type_first_operand,
	NULL,
	NULL,
	NULL,
	NULL,
	qb_set_result_first_operand,
	NULL,
	qb_select_opcode_simple,
	NULL,
	qb_transfer_operands_accommodate_array_size,

	0,
	0,
	0,
	QB_SZ_ACCOM_U32_U32_U32_U32,
};

qb_simple_op_factory factory_accommodate_array_size_update_dimension = {
	qb_resolve_expression_type_first_operand,
	NULL,
	NULL,
	NULL,
	NULL,
	qb_set_result_first_operand,
	NULL,
	qb_select_opcode_simple,
	NULL,
	qb_transfer_operands_accommodate_array_size_update_dimension,

	0,
	0,
	0,
	QB_SZ_ACCOM_DIM_U32_U32_U32_U32_U32_U32,
};

qb_simple_op_factory factory_choose_size_of_larger_array = {
	qb_resolve_expression_type_index,
	NULL,
	NULL,
	NULL,
	NULL,
	qb_set_result_non_reusable_temporary_value,
	NULL,
	qb_select_opcode_simple,
	NULL,
	qb_transfer_operands_choose_size_of_larger_array,

	0,
	0,
	QB_ADDRESS_TEMPORARY,
	QB_SZ_CHOOSE_2_U32_U32_U32_U32_U32,
};

qb_simple_op_factory factory_choose_size_of_larger_array_top_level = {
	qb_resolve_expression_type_index,
	NULL,
	NULL,
	NULL,
	NULL,
	qb_set_result_non_reusable_temporary_value,
	NULL,
	qb_select_opcode_simple,
	NULL,
	qb_transfer_operands_choose_size_of_larger_array_top_level,

	0,
	0,
	QB_ADDRESS_TEMPORARY,
	QB_SZ_CHOOSE_2_TOP_U32_U32_U32,
};

qb_simple_op_factory factory_choose_size_of_largest_of_three_arrays = {
	qb_resolve_expression_type_index,
	NULL,
	NULL,
	NULL,
	NULL,
	qb_set_result_non_reusable_temporary_value,
	NULL,
	qb_select_opcode_simple,
	NULL,
	qb_transfer_operands_choose_size_of_larger_array,

	0,
	0,
	QB_ADDRESS_TEMPORARY,
	QB_SZ_CHOOSE_3_U32_U32_U32_U32_U32_U32_U32,
};

qb_simple_op_factory factory_choose_size_of_largest_of_three_arrays_top_level = {
	qb_resolve_expression_type_index,
	NULL,
	NULL,
	NULL,
	NULL,
	qb_set_result_non_reusable_temporary_value,
	NULL,
	qb_select_opcode_simple,
	NULL,
	qb_transfer_operands_choose_size_of_larger_array_top_level,

	0,
	0,
	QB_ADDRESS_TEMPORARY,
	QB_SZ_CHOOSE_3_TOP_U32_U32_U32_U32,
};

extern qb_simple_op_factory factory_choose_dimension_of_larger_array;

qb_op_factory factory_receive_argument = {
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	qb_set_result_receive_argument,
	NULL,
	NULL,
	NULL,
	NULL,
	0,
	0,
	0,
};

qb_op_factory factory_send_argument = {
	NULL,
	NULL,
	NULL,
	qb_set_result_send_argument,
	NULL,
	qb_set_result_send_argument,
	NULL,
	NULL,
	NULL,
	NULL,
	0,
	0,
	0,
};

qb_op_factory factory_intrinsic = {
	qb_resolve_expression_type_intrinsic,
	qb_link_results_intrinsic,
	qb_coerce_operands_intrinsic,
	qb_set_preliminary_result_intrinsic,
	qb_validate_operands_intrinsic,
	qb_set_final_result_intrinsic,
	NULL,
	qb_select_opcode_intrinsic,
	NULL,
	qb_transfer_operands_intrinsic,
	0,
	0,
	0,
};

void *factories_fcall[] = { &factory_intrinsic, NULL, NULL };

qb_cast_op_factory factory_cast_S08 = {
	qb_resolve_expression_type_cast,
	NULL,
	NULL,
	qb_set_result_prototype,
	NULL,
	qb_set_result_temporary_value,
	qb_set_result_dimensions_first_operand,
	qb_select_opcode_derived,
	NULL,
	qb_transfer_operands_all,
	0,
	0,
	QB_ADDRESS_TEMPORARY,
	&factory_assign,
	QB_TYPE_S08,
};

qb_cast_op_factory factory_cast_U08 = {
	qb_resolve_expression_type_cast,
	NULL,
	NULL,
	qb_set_result_prototype,
	NULL,
	qb_set_result_temporary_value,
	qb_set_result_dimensions_first_operand,
	qb_select_opcode_derived,
	NULL,
	qb_transfer_operands_all,
	0,
	0,
	QB_ADDRESS_TEMPORARY,
	&factory_assign,
	QB_TYPE_U08,
};

qb_cast_op_factory factory_cast_S16 = {
	qb_resolve_expression_type_cast,
	NULL,
	NULL,
	qb_set_result_prototype,
	NULL,
	qb_set_result_temporary_value,
	qb_set_result_dimensions_first_operand,
	qb_select_opcode_derived,
	NULL,
	qb_transfer_operands_all,
	0,
	0,
	QB_ADDRESS_TEMPORARY,
	&factory_assign,
	QB_TYPE_S16,
};

qb_cast_op_factory factory_cast_U16 = {
	qb_resolve_expression_type_cast,
	NULL,
	NULL,
	qb_set_result_prototype,
	NULL,
	qb_set_result_temporary_value,
	qb_set_result_dimensions_first_operand,
	qb_select_opcode_derived,
	NULL,
	qb_transfer_operands_all,
	0,
	0,
	QB_ADDRESS_TEMPORARY,
	&factory_assign,
	QB_TYPE_U16,
};

qb_cast_op_factory factory_cast_S32 = {
	qb_resolve_expression_type_cast,
	NULL,
	NULL,
	qb_set_result_prototype,
	NULL,
	qb_set_result_temporary_value,
	qb_set_result_dimensions_first_operand,
	qb_select_opcode_derived,
	NULL,
	qb_transfer_operands_all,
	0,
	0,
	QB_ADDRESS_TEMPORARY,
	&factory_assign,
	QB_TYPE_S32,
};

qb_cast_op_factory factory_cast_U32 = {
	qb_resolve_expression_type_cast,
	NULL,
	NULL,
	qb_set_result_prototype,
	NULL,
	qb_set_result_temporary_value,
	qb_set_result_dimensions_first_operand,
	qb_select_opcode_derived,
	NULL,
	qb_transfer_operands_all,
	0,
	0,
	QB_ADDRESS_TEMPORARY,
	&factory_assign,
	QB_TYPE_U32,
};

qb_cast_op_factory factory_cast_S64 = {
	qb_resolve_expression_type_cast,
	NULL,
	NULL,
	qb_set_result_prototype,
	NULL,
	qb_set_result_temporary_value,
	qb_set_result_dimensions_first_operand,
	qb_select_opcode_derived,
	NULL,
	qb_transfer_operands_all,
	0,
	0,
	QB_ADDRESS_TEMPORARY,
	&factory_assign,
	QB_TYPE_S64,
};

qb_cast_op_factory factory_cast_U64 = {
	qb_resolve_expression_type_cast,
	NULL,
	NULL,
	qb_set_result_prototype,
	NULL,
	qb_set_result_temporary_value,
	qb_set_result_dimensions_first_operand,
	qb_select_opcode_derived,
	NULL,
	qb_transfer_operands_all,
	0,
	0,
	QB_ADDRESS_TEMPORARY,
	&factory_assign,
	QB_TYPE_U64,
};

qb_cast_op_factory factory_cast_F32 = {
	qb_resolve_expression_type_cast,
	NULL,
	NULL,
	qb_set_result_prototype,
	NULL,
	qb_set_result_temporary_value,
	qb_set_result_dimensions_first_operand,
	qb_select_opcode_derived,
	NULL,
	qb_transfer_operands_all,
	0,
	0,
	QB_ADDRESS_TEMPORARY,
	&factory_assign,
	QB_TYPE_F32,
};

qb_cast_op_factory factory_cast_F64 = {
	qb_resolve_expression_type_cast,
	NULL,
	NULL,
	qb_set_result_prototype,
	NULL,
	qb_set_result_temporary_value,
	qb_set_result_dimensions_first_operand,
	qb_select_opcode_derived,
	NULL,
	qb_transfer_operands_all,
	0,
	0,
	QB_ADDRESS_TEMPORARY,
	&factory_assign,
	QB_TYPE_F64,
};

qb_arithmetic_op_factory factory_increment_pre = {
	qb_resolve_expression_type_first_operand,
	NULL,
	NULL,
	qb_set_result_prototype,
	NULL,
	qb_set_result_first_operand,
	qb_set_result_dimensions_first_operand,
	qb_select_opcode_increment,
	NULL,
	qb_transfer_operands_result_only,

	0,
	QB_RESULT_HAS_SIDE_EFFECT,
	0,
	{	QB_INC_F64,	QB_INC_F32,	QB_INC_I64,	QB_INC_I64,	QB_INC_I32,	QB_INC_I32,	QB_INC_I16,	QB_INC_I16,	QB_INC_I08,	QB_INC_I08,	},
	{
		{	QB_INC_2X_F64,	QB_INC_2X_F32,	},
		{	QB_INC_3X_F64,	QB_INC_3X_F32,	},
		{	QB_INC_4X_F64,	QB_INC_4X_F32,	},
	},
};

qb_arithmetic_op_factory factory_decrement_pre = {
	qb_resolve_expression_type_first_operand,
	NULL,
	NULL,
	qb_set_result_prototype,
	NULL,
	qb_set_result_first_operand,
	qb_set_result_dimensions_first_operand,
	qb_select_opcode_increment,
	NULL,
	qb_transfer_operands_result_only,

	0,
	QB_RESULT_HAS_SIDE_EFFECT,
	0,
	{	QB_DEC_F64,	QB_DEC_F32,	QB_DEC_I64,	QB_DEC_I64,	QB_DEC_I32,	QB_DEC_I32,	QB_DEC_I16,	QB_DEC_I16,	QB_DEC_I08,	QB_DEC_I08,	},
	{
		{	QB_DEC_2X_F64,	QB_DEC_2X_F32,	},
		{	QB_DEC_3X_F64,	QB_DEC_3X_F32,	},
		{	QB_DEC_4X_F64,	QB_DEC_4X_F32,	},
	},
};

qb_derived_op_factory factory_increment_post = {
	qb_resolve_expression_type_first_operand,
	NULL,
	NULL,
	qb_set_result_prototype,
	NULL,
	qb_set_result_increment_post,
	qb_set_result_dimensions_first_operand,
	qb_select_opcode_derived,
	NULL,
	qb_transfer_operands_increment,

	0,
	QB_RESULT_HAS_SIDE_EFFECT,
	0,
	&factory_increment_pre,
};

qb_derived_op_factory factory_decrement_post = {
	qb_resolve_expression_type_first_operand,
	NULL,
	NULL,
	qb_set_result_prototype,
	NULL,
	qb_set_result_increment_post,
	qb_set_result_dimensions_first_operand,
	qb_select_opcode_derived,
	NULL,
	qb_transfer_operands_increment,

	0,
	QB_RESULT_HAS_SIDE_EFFECT,
	0,
	&factory_decrement_pre,
};

qb_arithmetic_op_factory factory_increment_object_property_pre = {
	qb_resolve_expression_type_object_property,
	NULL,
	NULL,
	qb_set_result_prototype,
	qb_validate_operands_object_property,
	qb_set_result_fetch_object_property,
	qb_set_result_dimensions_object_property,
	qb_select_opcode_increment_object_property,
	NULL,
	qb_transfer_operands_increment_object_property,

	0,
	QB_RESULT_HAS_SIDE_EFFECT,
	0,
	{	QB_INC_F64,	QB_INC_F32,	QB_INC_I64,	QB_INC_I64,	QB_INC_I32,	QB_INC_I32,	QB_INC_I16,	QB_INC_I16,	QB_INC_I08,	QB_INC_I08,	},
	{
		{	QB_INC_2X_F64,	QB_INC_2X_F32,	},
		{	QB_INC_3X_F64,	QB_INC_3X_F32,	},
		{	QB_INC_4X_F64,	QB_INC_4X_F32,	},
	},
};

qb_arithmetic_op_factory factory_decrement_object_property_pre = {
	qb_resolve_expression_type_object_property,
	NULL,
	NULL,
	qb_set_result_prototype,
	qb_validate_operands_object_property,
	qb_set_result_fetch_object_property,
	qb_set_result_dimensions_object_property,
	qb_select_opcode_increment_object_property,
	NULL,
	qb_transfer_operands_increment_object_property,

	0,
	QB_RESULT_HAS_SIDE_EFFECT,
	0,
	{	QB_DEC_F64,	QB_DEC_F32,	QB_DEC_I64,	QB_DEC_I64,	QB_DEC_I32,	QB_DEC_I32,	QB_DEC_I16,	QB_DEC_I16,	QB_DEC_I08,	QB_DEC_I08,	},
	{
		{	QB_DEC_2X_F64,	QB_DEC_2X_F32,	},
		{	QB_DEC_3X_F64,	QB_DEC_3X_F32,	},
		{	QB_DEC_4X_F64,	QB_DEC_4X_F32,	},
	},
};

qb_derived_op_factory factory_increment_object_property_post = {
	qb_resolve_expression_type_object_property,
	NULL,
	NULL,
	qb_set_result_prototype,
	qb_validate_operands_object_property,
	qb_set_result_increment_post,
	qb_set_result_dimensions_object_property,
	qb_select_opcode_derived,
	NULL,
	qb_transfer_operands_increment_object_property,

	0,
	QB_RESULT_HAS_SIDE_EFFECT,
	0,
	&factory_increment_object_property_pre,
};

qb_derived_op_factory factory_decrement_object_property_post = {
	qb_resolve_expression_type_object_property,
	NULL,
	NULL,
	qb_set_result_prototype,
	qb_validate_operands_object_property,
	qb_set_result_increment_post,
	qb_set_result_dimensions_object_property,
	qb_select_opcode_derived,
	NULL,
	qb_transfer_operands_increment_object_property,

	0,
	QB_RESULT_HAS_SIDE_EFFECT,
	0,
	&factory_decrement_object_property_pre,
};

qb_basic_op_factory factory_shift_left = {
	qb_resolve_expression_type_first_operand,
	qb_link_results_all_operands,
	qb_coerce_operands_all,
	qb_set_result_prototype,
	NULL,
	qb_set_result_temporary_value,
	qb_set_result_dimensions_larger_of_two,
	qb_select_opcode_basic,
	NULL,
	qb_transfer_operands_all,

	QB_COERCE_TO_LVALUE_TYPE | QB_COERCE_TO_INTEGER,
	QB_RESULT_FROM_PURE_FUNCTION,
	QB_ADDRESS_TEMPORARY,
	{	0,	0,	QB_SHL_U64_U64_U64,	QB_SHL_S64_S64_S64,	QB_SHL_U32_U32_U32,	QB_SHL_S32_S32_S32,	QB_SHL_U16_U16_U16,	QB_SHL_S16_S16_S16,	QB_SHL_U08_U08_U08,	QB_SHL_S08_S08_S08,	},
};

qb_basic_op_factory factory_shift_right = {
	qb_resolve_expression_type_first_operand,
	qb_link_results_all_operands,
	qb_coerce_operands_all,
	qb_set_result_prototype,
	NULL,
	qb_set_result_temporary_value,
	qb_set_result_dimensions_larger_of_two,
	qb_select_opcode_basic,
	NULL,
	qb_transfer_operands_all,

	QB_COERCE_TO_LVALUE_TYPE | QB_COERCE_TO_INTEGER,
	QB_RESULT_FROM_PURE_FUNCTION,
	QB_ADDRESS_TEMPORARY,
	{	0,	0,	QB_SHR_U64_U64_U64,	QB_SHR_S64_S64_S64,	QB_SHR_U32_U32_U32,	QB_SHR_S32_S32_S32,	QB_SHR_U16_U16_U16,	QB_SHR_S16_S16_S16,	QB_SHR_U08_U08_U08,	QB_SHR_S08_S08_S08,	},
};

qb_derived_op_factory factory_shift_left_assign = {
	qb_resolve_expression_type_first_operand,
	qb_link_results_all_operands,
	qb_coerce_operands_assign,
	qb_set_result_prototype,
	NULL,
	qb_set_result_first_operand,
	NULL,
	qb_select_opcode_derived,
	NULL,
	qb_transfer_operands_modify_assign,

	0,
	QB_RESULT_HAS_SIDE_EFFECT,
	0,
	&factory_shift_left,
};

qb_derived_op_factory factory_shift_left_assign_element = {
	qb_resolve_expression_type_first_operand,
	qb_link_results_all_operands,
	qb_coerce_operands_assign_array_element,
	qb_set_result_prototype,
	NULL,
	qb_set_result_assign_array_element,
	NULL,
	qb_select_opcode_derived,
	NULL,
	qb_transfer_operands_modify_assign,

	0,
	QB_RESULT_HAS_SIDE_EFFECT,
	0,
	&factory_shift_left,
};

qb_derived_op_factory factory_shift_left_assign_object_property = {
	qb_resolve_expression_type_object_property,
	qb_link_results_all_operands,
	qb_coerce_operands_assign_object_property,
	qb_set_result_prototype,
	NULL,
	qb_set_result_assign_object_property,
	NULL,
	qb_select_opcode_derived,
	NULL,
	qb_transfer_operands_modify_assign,

	0,
	QB_RESULT_HAS_SIDE_EFFECT,
	0,
	&factory_shift_left,
};

void *factories_shift_left_assign[3] = { &factory_shift_left_assign, &factory_shift_left_assign_element, &factory_shift_left_assign_object_property };

qb_derived_op_factory factory_shift_right_assign = {
	qb_resolve_expression_type_first_operand,
	qb_link_results_all_operands,
	qb_coerce_operands_assign,
	qb_set_result_prototype,
	NULL,
	qb_set_result_first_operand,
	NULL,
	qb_select_opcode_derived,
	NULL,
	qb_transfer_operands_modify_assign,

	0,
	QB_RESULT_HAS_SIDE_EFFECT,
	0,
	&factory_shift_right,
};

qb_derived_op_factory factory_shift_right_assign_element = {
	qb_resolve_expression_type_first_operand,
	qb_link_results_all_operands,
	qb_coerce_operands_assign_array_element,
	qb_set_result_prototype,
	NULL,
	qb_set_result_assign_array_element,
	NULL,
	qb_select_opcode_derived,
	NULL,
	qb_transfer_operands_modify_assign,

	0,
	QB_RESULT_HAS_SIDE_EFFECT,
	0,
	&factory_shift_right,
};

qb_derived_op_factory factory_shift_right_assign_object_property = {
	qb_resolve_expression_type_object_property,
	qb_link_results_all_operands,
	qb_coerce_operands_assign_object_property,
	qb_set_result_prototype,
	NULL,
	qb_set_result_assign_object_property,
	NULL,
	qb_select_opcode_derived,
	NULL,
	qb_transfer_operands_modify_assign,

	0,
	QB_RESULT_HAS_SIDE_EFFECT,
	0,
	&factory_shift_right,
};

void *factories_shift_right_assign[3] = { &factory_shift_right_assign, &factory_shift_right_assign_element, &factory_shift_right_assign_object_property };

qb_basic_op_factory factory_bitwise_not = {
	qb_resolve_expression_type_highest_rank,
	qb_link_results_all_operands,
	qb_coerce_operands_all,
	qb_set_result_prototype,
	NULL,
	qb_set_result_temporary_value,
	qb_set_result_dimensions_first_operand,
	qb_select_opcode_basic,
	NULL,
	qb_transfer_operands_all,

	QB_COERCE_TO_INTEGER,
	QB_RESULT_FROM_PURE_FUNCTION,
	QB_ADDRESS_TEMPORARY,
	{	0,	0,	QB_BW_NOT_I64_I64,	QB_BW_NOT_I64_I64,	QB_BW_NOT_I32_I32,	QB_BW_NOT_I32_I32,	QB_BW_NOT_I16_I16,	QB_BW_NOT_I16_I16,	QB_BW_NOT_I08_I08,	QB_BW_NOT_I08_I08,	}
};

qb_basic_op_factory factory_bitwise_and = {
	qb_resolve_expression_type_highest_rank,
	qb_link_results_all_operands,
	qb_coerce_operands_all,
	qb_set_result_prototype,
	NULL,
	qb_set_result_temporary_value,
	qb_set_result_dimensions_larger_of_two,
	qb_select_opcode_basic,
	NULL,
	qb_transfer_operands_all,

	QB_COERCE_TO_LVALUE_TYPE | QB_COERCE_TO_INTEGER,
	QB_RESULT_FROM_PURE_FUNCTION,
	QB_ADDRESS_TEMPORARY,
	{	0,	0,	QB_BW_AND_I64_I64_I64,	QB_BW_AND_I64_I64_I64,	QB_BW_AND_I32_I32_I32,	QB_BW_AND_I32_I32_I32,	QB_BW_AND_I16_I16_I16,	QB_BW_AND_I16_I16_I16,	QB_BW_AND_I08_I08_I08,	QB_BW_AND_I08_I08_I08,	},
};

qb_basic_op_factory factory_bitwise_or = {
	qb_resolve_expression_type_highest_rank,
	qb_link_results_all_operands,
	qb_coerce_operands_all,
	qb_set_result_prototype,
	NULL,
	qb_set_result_temporary_value,
	qb_set_result_dimensions_larger_of_two,
	qb_select_opcode_basic,
	NULL,
	qb_transfer_operands_all,

	QB_COERCE_TO_LVALUE_TYPE | QB_COERCE_TO_INTEGER,
	QB_RESULT_FROM_PURE_FUNCTION,
	QB_ADDRESS_TEMPORARY,
	{	0,	0,	QB_BW_OR_I64_I64_I64,	QB_BW_OR_I64_I64_I64,	QB_BW_OR_I32_I32_I32,	QB_BW_OR_I32_I32_I32,	QB_BW_OR_I16_I16_I16,	QB_BW_OR_I16_I16_I16,	QB_BW_OR_I08_I08_I08,	QB_BW_OR_I08_I08_I08,	},
};

qb_basic_op_factory factory_bitwise_xor = {
	qb_resolve_expression_type_highest_rank,
	qb_link_results_all_operands,
	qb_coerce_operands_all,
	qb_set_result_prototype,
	NULL,
	qb_set_result_temporary_value,
	qb_set_result_dimensions_larger_of_two,
	qb_select_opcode_basic,
	NULL,
	qb_transfer_operands_all,

	QB_COERCE_TO_LVALUE_TYPE | QB_COERCE_TO_INTEGER,
	QB_RESULT_FROM_PURE_FUNCTION,
	QB_ADDRESS_TEMPORARY,
	{	0,	0,	QB_BW_XOR_I64_I64_I64,	QB_BW_XOR_I64_I64_I64,	QB_BW_XOR_I32_I32_I32,	QB_BW_XOR_I32_I32_I32,	QB_BW_XOR_I16_I16_I16,	QB_BW_XOR_I16_I16_I16,	QB_BW_XOR_I08_I08_I08,	QB_BW_XOR_I08_I08_I08,	},
};

qb_derived_op_factory factory_bitwise_and_assign = {
	qb_resolve_expression_type_first_operand,
	qb_link_results_all_operands,
	qb_coerce_operands_assign,
	qb_set_result_prototype,
	NULL,
	qb_set_result_first_operand,
	NULL,
	qb_select_opcode_derived,
	NULL,
	qb_transfer_operands_modify_assign,

	0,
	QB_RESULT_HAS_SIDE_EFFECT,
	0,
	&factory_bitwise_and,
};

qb_derived_op_factory factory_bitwise_and_assign_element = {
	qb_resolve_expression_type_first_operand,
	qb_link_results_all_operands,
	qb_coerce_operands_assign_array_element,
	qb_set_result_prototype,
	NULL,
	qb_set_result_assign_array_element,
	NULL,
	qb_select_opcode_derived,
	NULL,
	qb_transfer_operands_modify_assign,

	0,
	QB_RESULT_HAS_SIDE_EFFECT,
	0,
	&factory_bitwise_and,
};

qb_derived_op_factory factory_bitwise_and_assign_object_property = {
	qb_resolve_expression_type_object_property,
	qb_link_results_all_operands,
	qb_coerce_operands_assign_object_property,
	qb_set_result_prototype,
	NULL,
	qb_set_result_assign_object_property,
	NULL,
	qb_select_opcode_derived,
	NULL,
	qb_transfer_operands_modify_assign,

	0,
	QB_RESULT_HAS_SIDE_EFFECT,
	0,
	&factory_bitwise_and,
};

void *factories_bitwise_and_assign[3] = { &factory_bitwise_and_assign, &factory_bitwise_and_assign_element, &factory_bitwise_and_assign_object_property };

qb_derived_op_factory factory_bitwise_or_assign = {
	qb_resolve_expression_type_first_operand,
	qb_link_results_all_operands,
	qb_coerce_operands_assign,
	qb_set_result_prototype,
	NULL,
	qb_set_result_first_operand,
	NULL,
	qb_select_opcode_derived,
	NULL,
	qb_transfer_operands_modify_assign,

	0,
	QB_RESULT_HAS_SIDE_EFFECT,
	0,
	&factory_bitwise_or,
};

qb_derived_op_factory factory_bitwise_or_assign_element = {
	qb_resolve_expression_type_first_operand,
	qb_link_results_all_operands,
	qb_coerce_operands_assign_array_element,
	qb_set_result_prototype,
	NULL,
	qb_set_result_assign_array_element,
	NULL,
	qb_select_opcode_derived,
	NULL,
	qb_transfer_operands_modify_assign,

	0,
	QB_RESULT_HAS_SIDE_EFFECT,
	0,
	&factory_bitwise_or,
};

qb_derived_op_factory factory_bitwise_or_assign_object_property = {
	qb_resolve_expression_type_object_property,
	qb_link_results_all_operands,
	qb_coerce_operands_assign_object_property,
	qb_set_result_prototype,
	NULL,
	qb_set_result_assign_object_property,
	NULL,
	qb_select_opcode_derived,
	NULL,
	qb_transfer_operands_modify_assign,

	0,
	QB_RESULT_HAS_SIDE_EFFECT,
	0,
	&factory_bitwise_or,
};

void *factories_bitwise_or_assign[3] = { &factory_bitwise_or_assign, &factory_bitwise_or_assign_element, &factory_bitwise_or_assign_object_property };

qb_derived_op_factory factory_bitwise_xor_assign = {
	qb_resolve_expression_type_first_operand,
	qb_link_results_all_operands,
	qb_coerce_operands_assign,
	qb_set_result_prototype,
	NULL,
	qb_set_result_first_operand,
	NULL,
	qb_select_opcode_derived,
	NULL,
	qb_transfer_operands_modify_assign,

	0,
	QB_RESULT_HAS_SIDE_EFFECT,
	0,
	&factory_bitwise_xor,
};

qb_derived_op_factory factory_bitwise_xor_assign_element = {
	qb_resolve_expression_type_first_operand,
	qb_link_results_all_operands,
	qb_coerce_operands_assign_array_element,
	qb_set_result_prototype,
	NULL,
	qb_set_result_assign_array_element,
	NULL,
	qb_select_opcode_derived,
	NULL,
	qb_transfer_operands_modify_assign,

	0,
	QB_RESULT_HAS_SIDE_EFFECT,
	0,
	&factory_bitwise_xor,
};

qb_derived_op_factory factory_bitwise_xor_assign_object_property = {
	qb_resolve_expression_type_object_property,
	qb_link_results_all_operands,
	qb_coerce_operands_assign_object_property,
	qb_set_result_prototype,
	NULL,
	qb_set_result_assign_object_property,
	NULL,
	qb_select_opcode_derived,
	NULL,
	qb_transfer_operands_modify_assign,

	0,
	QB_RESULT_HAS_SIDE_EFFECT,
	0,
	&factory_bitwise_xor,
};

void *factories_bitwise_xor_assign[3] = { &factory_bitwise_xor_assign, &factory_bitwise_xor_assign_element, &factory_bitwise_xor_assign_object_property };

qb_basic_op_factory factory_boolean_cast = {
	qb_resolve_expression_type_boolean,
	NULL,
	NULL,
	qb_set_result_prototype,
	NULL,
	qb_set_result_temporary_value,
	NULL,
	qb_select_opcode_boolean_cast,
	NULL,
	qb_transfer_operands_boolean_cast,

	0,
	QB_RESULT_FROM_PURE_FUNCTION,
	QB_ADDRESS_TEMPORARY | QB_ADDRESS_BOOLEAN,
	{	QB_BOOL_F64_I32,	QB_BOOL_F32_I32,	QB_BOOL_I64_I32,	QB_BOOL_I64_I32,	QB_BOOL_I32_I32,	QB_BOOL_I32_I32,	QB_BOOL_I16_I32,	QB_BOOL_I16_I32,	QB_BOOL_I08_I32,	QB_BOOL_I08_I32,	},
};

qb_basic_op_factory factory_logical_not = {
	qb_resolve_expression_type_boolean,
	NULL,
	qb_coerce_operands_all,
	qb_set_result_prototype,
	NULL,
	qb_set_result_temporary_value,
	NULL,
	qb_select_opcode_basic,
	NULL,
	qb_transfer_operands_all,

	QB_COERCE_TO_BOOLEAN,
	QB_RESULT_FROM_PURE_FUNCTION,
	QB_ADDRESS_TEMPORARY | QB_ADDRESS_BOOLEAN,
	{	0,	0,	0,	0,	QB_NOT_I32_I32,	QB_NOT_I32_I32,	0,	0,	0,	0,	}
};

qb_basic_op_factory factory_logical_and = {
	qb_resolve_expression_type_boolean,
	NULL,
	qb_coerce_operands_all,
	qb_set_result_prototype,
	NULL,
	qb_set_result_temporary_value,
	NULL,
	qb_select_opcode_basic,
	NULL,
	qb_transfer_operands_all,

	QB_COERCE_TO_BOOLEAN,
	QB_RESULT_FROM_PURE_FUNCTION,
	QB_ADDRESS_TEMPORARY | QB_ADDRESS_BOOLEAN,
	{	0,	0,	0,	0,	QB_AND_I32_I32_I32,	QB_AND_I32_I32_I32,	0,	0,	0,	0,	},
};

qb_basic_op_factory factory_logical_or = {
	qb_resolve_expression_type_boolean,
	NULL,
	qb_coerce_operands_all,
	qb_set_result_prototype,
	NULL,
	qb_set_result_temporary_value,
	NULL,
	qb_select_opcode_basic,
	NULL,
	qb_transfer_operands_all,

	QB_COERCE_TO_BOOLEAN,
	QB_RESULT_FROM_PURE_FUNCTION,
	QB_ADDRESS_TEMPORARY | QB_ADDRESS_BOOLEAN,
	{	0,	0,	0,	0,	QB_OR_I32_I32_I32,	QB_OR_I32_I32_I32,	0,	0,	0,	0,	},
};

qb_basic_op_factory factory_logical_xor = {
	qb_resolve_expression_type_boolean,
	NULL,
	qb_coerce_operands_all,
	qb_set_result_prototype,
	NULL,
	qb_set_result_temporary_value,
	NULL,
	qb_select_opcode_basic,
	NULL,
	qb_transfer_operands_all,

	QB_COERCE_TO_BOOLEAN,
	QB_RESULT_FROM_PURE_FUNCTION,
	QB_ADDRESS_TEMPORARY | QB_ADDRESS_BOOLEAN,
	{	0,	0,	0,	0,	QB_XOR_I32_I32_I32,	QB_XOR_I32_I32_I32,	0,	0,	0,	0,	},
};

qb_arithmetic_op_factory factory_add = {
	qb_resolve_expression_type_highest_rank,
	qb_link_results_all_operands,
	qb_coerce_operands_all,
	qb_set_result_prototype,
	NULL,
	qb_set_result_temporary_value,
	qb_set_result_dimensions_larger_of_two,
	qb_select_opcode_arithmetic,
	NULL,
	qb_transfer_operands_all,

	QB_COERCE_TO_LVALUE_TYPE,
	QB_RESULT_FROM_PURE_FUNCTION,
	QB_ADDRESS_TEMPORARY,
	{	QB_ADD_F64_F64_F64,	QB_ADD_F32_F32_F32,	QB_ADD_I64_I64_I64,	QB_ADD_I64_I64_I64,	QB_ADD_I32_I32_I32,	QB_ADD_I32_I32_I32,	QB_ADD_I16_I16_I16,	QB_ADD_I16_I16_I16,	QB_ADD_I08_I08_I08,	QB_ADD_I08_I08_I08,	},
	{
		{	QB_ADD_2X_F64_F64_F64,	QB_ADD_2X_F32_F32_F32,	},
		{	QB_ADD_3X_F64_F64_F64,	QB_ADD_3X_F32_F32_F32,	},
		{	QB_ADD_4X_F64_F64_F64,	QB_ADD_4X_F32_F32_F32,	},
	},
};

qb_arithmetic_op_factory factory_subtract = {
	qb_resolve_expression_type_highest_rank,
	qb_link_results_all_operands,
	qb_coerce_operands_all,
	qb_set_result_prototype,
	NULL,
	qb_set_result_temporary_value,
	qb_set_result_dimensions_larger_of_two,
	qb_select_opcode_arithmetic,
	NULL,
	qb_transfer_operands_all,

	QB_COERCE_TO_LVALUE_TYPE,
	QB_RESULT_FROM_PURE_FUNCTION,
	QB_ADDRESS_TEMPORARY,
	{	QB_SUB_F64_F64_F64,	QB_SUB_F32_F32_F32,	QB_SUB_I64_I64_I64,	QB_SUB_I64_I64_I64,	QB_SUB_I32_I32_I32,	QB_SUB_I32_I32_I32,	QB_SUB_I16_I16_I16,	QB_SUB_I16_I16_I16,	QB_SUB_I08_I08_I08,	QB_SUB_I08_I08_I08,	},
	{
		{	QB_SUB_2X_F64_F64_F64,	QB_SUB_2X_F32_F32_F32,	},
		{	QB_SUB_3X_F64_F64_F64,	QB_SUB_3X_F32_F32_F32,	},
		{	QB_SUB_4X_F64_F64_F64,	QB_SUB_4X_F32_F32_F32,	},
	},
};

qb_arithmetic_op_factory factory_multiply = {
	qb_resolve_expression_type_highest_rank,
	qb_link_results_all_operands,
	qb_coerce_operands_all,
	qb_set_result_prototype,
	NULL,
	qb_set_result_temporary_value,
	qb_set_result_dimensions_larger_of_two,
	qb_select_opcode_arithmetic,
	NULL,
	qb_transfer_operands_all,

	QB_COERCE_TO_LVALUE_TYPE,
	QB_RESULT_FROM_PURE_FUNCTION,
	QB_ADDRESS_TEMPORARY,
	{	QB_MUL_F64_F64_F64,	QB_MUL_F32_F32_F32,	QB_MUL_U64_U64_U64,	QB_MUL_U64_U64_U64,	QB_MUL_U32_U32_U32,	QB_MUL_U32_U32_U32,	QB_MUL_S16_S16_S16,	QB_MUL_S16_S16_S16,	QB_MUL_S08_S08_S08,	QB_MUL_S08_S08_S08,	},
	{
		{	QB_MUL_2X_F64_F64_F64,	QB_MUL_2X_F32_F32_F32,	},
		{	QB_MUL_3X_F64_F64_F64,	QB_MUL_3X_F32_F32_F32,	},
		{	QB_MUL_4X_F64_F64_F64,	QB_MUL_4X_F32_F32_F32,	},
	},
};

qb_arithmetic_op_factory factory_divide = {
	qb_resolve_expression_type_highest_rank,
	qb_link_results_all_operands,
	qb_coerce_operands_all,
	qb_set_result_prototype,
	NULL,
	qb_set_result_temporary_value,
	qb_set_result_dimensions_larger_of_two,
	qb_select_opcode_arithmetic,
	NULL,
	qb_transfer_operands_all,

	QB_COERCE_TO_LVALUE_TYPE,
	QB_RESULT_FROM_PURE_FUNCTION,
	QB_ADDRESS_TEMPORARY,
	{	QB_DIV_F64_F64_F64,	QB_DIV_F32_F32_F32,	QB_DIV_U64_U64_U64,	QB_DIV_S64_S64_S64,	QB_DIV_U32_U32_U32,	QB_DIV_S32_S32_S32,	QB_DIV_U16_U16_U16,	QB_DIV_S16_S16_S16,	QB_DIV_U08_U08_U08,	QB_DIV_S08_S08_S08,	},
	{
		{	QB_DIV_2X_F64_F64_F64,	QB_DIV_2X_F32_F32_F32,	},
		{	QB_DIV_3X_F64_F64_F64,	QB_DIV_3X_F32_F32_F32,	},
		{	QB_DIV_4X_F64_F64_F64,	QB_DIV_4X_F32_F32_F32,	},
	},
};

qb_arithmetic_op_factory factory_modulo = {
	qb_resolve_expression_type_highest_rank,
	qb_link_results_all_operands,
	qb_coerce_operands_all,
	qb_set_result_prototype,
	NULL,
	qb_set_result_temporary_value,
	qb_set_result_dimensions_larger_of_two,
	qb_select_opcode_arithmetic,
	NULL,
	qb_transfer_operands_all,

	QB_COERCE_TO_LVALUE_TYPE,
	QB_RESULT_FROM_PURE_FUNCTION,
	QB_ADDRESS_TEMPORARY,
	{	QB_MOD_F64_F64_F64,	QB_MOD_F32_F32_F32,	QB_MOD_U64_U64_U64,	QB_MOD_S64_S64_S64,	QB_MOD_U32_U32_U32,	QB_MOD_S32_S32_S32,	QB_MOD_U16_U16_U16,	QB_MOD_S16_S16_S16,	QB_MOD_U08_U08_U08,	QB_MOD_S08_S08_S08,	},
	{
		{	QB_MOD_2X_F64_F64_F64,	QB_MOD_2X_F32_F32_F32,	},
		{	QB_MOD_3X_F64_F64_F64,	QB_MOD_3X_F32_F32_F32,	},
		{	QB_MOD_4X_F64_F64_F64,	QB_MOD_4X_F32_F32_F32,	},
	},
};

qb_float_op_factory factory_floor_modulo = {
	qb_resolve_expression_type_highest_rank,
	qb_link_results_all_operands,
	qb_coerce_operands_all,
	qb_set_result_prototype,
	NULL,
	qb_set_result_temporary_value,
	qb_set_result_dimensions_larger_of_two,
	qb_select_opcode_arithmetic,
	NULL,
	qb_transfer_operands_all,

	QB_COERCE_TO_LVALUE_TYPE,
	QB_RESULT_FROM_PURE_FUNCTION,
	QB_ADDRESS_TEMPORARY | QB_ADDRESS_BOOLEAN,
	{	QB_MOD_FLR_F64_F64_F64,	QB_MOD_FLR_F32_F32_F32,	},
};

qb_derived_op_factory factory_add_assign = {
	qb_resolve_expression_type_first_operand,
	qb_link_results_all_operands,
	qb_coerce_operands_assign,
	qb_set_result_prototype,
	NULL,
	qb_set_result_first_operand,
	NULL,
	qb_select_opcode_derived,
	NULL,
	qb_transfer_operands_modify_assign,

	0,
	QB_RESULT_HAS_SIDE_EFFECT,
	0,
	&factory_add,
};

qb_derived_op_factory factory_add_assign_element = {
	qb_resolve_expression_type_first_operand,
	qb_link_results_all_operands,
	qb_coerce_operands_assign_array_element,
	qb_set_result_prototype,
	NULL,
	qb_set_result_assign_array_element,
	NULL,
	qb_select_opcode_derived,
	NULL,
	qb_transfer_operands_modify_assign,

	0,
	QB_RESULT_HAS_SIDE_EFFECT,
	0,
	&factory_add,
};

qb_derived_op_factory factory_add_assign_object_property = {
	qb_resolve_expression_type_object_property,
	qb_link_results_all_operands,
	qb_coerce_operands_assign_object_property,
	qb_set_result_prototype,
	NULL,
	qb_set_result_assign_object_property,
	NULL,
	qb_select_opcode_derived,
	NULL,
	qb_transfer_operands_modify_assign,

	0,
	QB_RESULT_HAS_SIDE_EFFECT,
	0,
	&factory_add,
};

void *factories_add_assign[3] = { &factory_add_assign, &factory_add_assign_element, &factory_add_assign_object_property };

qb_derived_op_factory factory_subtract_assign = {
	qb_resolve_expression_type_first_operand,
	qb_link_results_all_operands,
	qb_coerce_operands_assign,
	qb_set_result_prototype,
	NULL,
	qb_set_result_first_operand,
	NULL,
	qb_select_opcode_derived,
	NULL,
	qb_transfer_operands_modify_assign,

	0,
	QB_RESULT_HAS_SIDE_EFFECT,
	0,
	&factory_subtract,
};

qb_derived_op_factory factory_subtract_assign_element = {
	qb_resolve_expression_type_first_operand,
	qb_link_results_all_operands,
	qb_coerce_operands_assign_array_element,
	qb_set_result_prototype,
	NULL,
	qb_set_result_assign_array_element,
	NULL,
	qb_select_opcode_derived,
	NULL,
	qb_transfer_operands_modify_assign,

	0,
	QB_RESULT_HAS_SIDE_EFFECT,
	0,
	&factory_subtract,
};

qb_derived_op_factory factory_subtract_assign_object_property = {
	qb_resolve_expression_type_object_property,
	qb_link_results_all_operands,
	qb_coerce_operands_assign_object_property,
	qb_set_result_prototype,
	NULL,
	qb_set_result_assign_object_property,
	NULL,
	qb_select_opcode_derived,
	NULL,
	qb_transfer_operands_modify_assign,

	0,
	QB_RESULT_HAS_SIDE_EFFECT,
	0,
	&factory_subtract,
};

void *factories_subtract_assign[3] = { &factory_subtract_assign, &factory_subtract_assign_element, &factory_subtract_assign_object_property };

qb_derived_op_factory factory_multiply_assign = {
	qb_resolve_expression_type_first_operand,
	qb_link_results_all_operands,
	qb_coerce_operands_assign,
	qb_set_result_prototype,
	NULL,
	qb_set_result_first_operand,
	NULL,
	qb_select_opcode_derived,
	NULL,
	qb_transfer_operands_modify_assign,

	0,
	QB_RESULT_HAS_SIDE_EFFECT,
	0,
	&factory_multiply,
};

qb_derived_op_factory factory_multiply_assign_element = {
	qb_resolve_expression_type_first_operand,
	qb_link_results_all_operands,
	qb_coerce_operands_assign_array_element,
	qb_set_result_prototype,
	NULL,
	qb_set_result_assign_array_element,
	NULL,
	qb_select_opcode_derived,
	NULL,
	qb_transfer_operands_modify_assign,

	0,
	QB_RESULT_HAS_SIDE_EFFECT,
	0,
	&factory_multiply,
};

qb_derived_op_factory factory_multiply_assign_object_property = {
	qb_resolve_expression_type_object_property,
	qb_link_results_all_operands,
	qb_coerce_operands_assign_object_property,
	qb_set_result_prototype,
	NULL,
	qb_set_result_assign_object_property,
	NULL,
	qb_select_opcode_derived,
	NULL,
	qb_transfer_operands_modify_assign,

	0,
	QB_RESULT_HAS_SIDE_EFFECT,
	0,
	&factory_multiply,
};

void *factories_multiply_assign[3] = { &factory_multiply_assign, &factory_multiply_assign_element, &factory_multiply_assign_object_property };

qb_derived_op_factory factory_divide_assign = {
	qb_resolve_expression_type_first_operand,
	qb_link_results_all_operands,
	qb_coerce_operands_assign,
	qb_set_result_prototype,
	NULL,
	qb_set_result_first_operand,
	NULL,
	qb_select_opcode_derived,
	NULL,
	qb_transfer_operands_modify_assign,

	0,
	QB_RESULT_HAS_SIDE_EFFECT,
	0,
	&factory_divide,
};

qb_derived_op_factory factory_divide_assign_element = {
	qb_resolve_expression_type_first_operand,
	qb_link_results_all_operands,
	qb_coerce_operands_assign_array_element,
	qb_set_result_prototype,
	NULL,
	qb_set_result_assign_array_element,
	NULL,
	qb_select_opcode_derived,
	NULL,
	qb_transfer_operands_modify_assign,

	0,
	QB_RESULT_HAS_SIDE_EFFECT,
	0,
	&factory_divide,
};

qb_derived_op_factory factory_divide_assign_object_property = {
	qb_resolve_expression_type_object_property,
	qb_link_results_all_operands,
	qb_coerce_operands_assign_object_property,
	qb_set_result_prototype,
	NULL,
	qb_set_result_assign_object_property,
	NULL,
	qb_select_opcode_derived,
	NULL,
	qb_transfer_operands_modify_assign,

	0,
	QB_RESULT_HAS_SIDE_EFFECT,
	0,
	&factory_divide,
};

void *factories_divide_assign[3] = { &factory_divide_assign, &factory_divide_assign_element, &factory_divide_assign_object_property };

qb_derived_op_factory factory_modulo_assign = {
	qb_resolve_expression_type_first_operand,
	qb_link_results_all_operands,
	qb_coerce_operands_assign,
	qb_set_result_prototype,
	NULL,
	qb_set_result_first_operand,
	NULL,
	qb_select_opcode_derived,
	NULL,
	qb_transfer_operands_modify_assign,

	0,
	QB_RESULT_HAS_SIDE_EFFECT,
	0,
	&factory_modulo,
};

qb_derived_op_factory factory_modulo_assign_element = {
	qb_resolve_expression_type_first_operand,
	qb_link_results_all_operands,
	qb_coerce_operands_assign_array_element,
	qb_set_result_prototype,
	NULL,
	qb_set_result_assign_array_element,
	NULL,
	qb_select_opcode_derived,
	NULL,
	qb_transfer_operands_modify_assign,

	0,
	QB_RESULT_HAS_SIDE_EFFECT,
	0,
	&factory_modulo,
};

qb_derived_op_factory factory_modulo_assign_object_property = {
	qb_resolve_expression_type_object_property,
	qb_link_results_all_operands,
	qb_coerce_operands_assign_object_property,
	qb_set_result_prototype,
	NULL,
	qb_set_result_assign_object_property,
	NULL,
	qb_select_opcode_derived,
	NULL,
	qb_transfer_operands_modify_assign,

	0,
	QB_RESULT_HAS_SIDE_EFFECT,
	0,
	&factory_modulo,
};

void *factories_modulo_assign[3] = { &factory_modulo_assign, &factory_modulo_assign_element, &factory_modulo_assign_object_property };

qb_basic_op_factory factory_equal = {
	qb_resolve_expression_type_boolean,
	NULL,
	qb_coerce_operands_highest_rank,
	qb_set_result_prototype,
	NULL,
	qb_set_result_temporary_value,
	NULL,
	qb_select_opcode_basic,
	NULL,
	qb_transfer_operands_all,

	0,
	QB_RESULT_FROM_PURE_FUNCTION,
	QB_ADDRESS_TEMPORARY | QB_ADDRESS_BOOLEAN,
	{	QB_EQ_F64_F64_I32,	QB_EQ_F32_F32_I32,	QB_EQ_I64_I64_I32,	QB_EQ_I64_I64_I32,	QB_EQ_I32_I32_I32,	QB_EQ_I32_I32_I32,	QB_EQ_I16_I16_I32,	QB_EQ_I16_I16_I32,	QB_EQ_I08_I08_I32,	QB_EQ_I08_I08_I32,	},
};

qb_basic_op_factory factory_not_equal = {
	qb_resolve_expression_type_boolean,
	NULL,
	qb_coerce_operands_highest_rank,
	qb_set_result_prototype,
	NULL,
	qb_set_result_temporary_value,
	NULL,
	qb_select_opcode_basic,
	NULL,
	qb_transfer_operands_all,

	0,
	QB_RESULT_FROM_PURE_FUNCTION,
	QB_ADDRESS_TEMPORARY | QB_ADDRESS_BOOLEAN,
	{	QB_NE_F64_F64_I32,	QB_NE_F32_F32_I32,	QB_NE_I64_I64_I32,	QB_NE_I64_I64_I32,	QB_NE_I32_I32_I32,	QB_NE_I32_I32_I32,	QB_NE_I16_I16_I32,	QB_NE_I16_I16_I32,	QB_NE_I08_I08_I32,	QB_NE_I08_I08_I32,	},
};

qb_basic_op_factory factory_less_than = {
	qb_resolve_expression_type_boolean,
	NULL,
	qb_coerce_operands_highest_rank,
	qb_set_result_prototype,
	NULL,
	qb_set_result_temporary_value,
	NULL,
	qb_select_opcode_basic,
	NULL,
	qb_transfer_operands_all,

	0,
	QB_RESULT_FROM_PURE_FUNCTION,
	QB_ADDRESS_TEMPORARY | QB_ADDRESS_BOOLEAN,
	{	QB_LT_F64_F64_I32,	QB_LT_F32_F32_I32,	QB_LT_U64_U64_I32,	QB_LT_S64_S64_I32,	QB_LT_U32_U32_I32,	QB_LT_S32_S32_I32,	QB_LT_U16_U16_I32,	QB_LT_S16_S16_I32,	QB_LT_U08_U08_I32,	QB_LT_S08_S08_I32,	},
};

qb_basic_op_factory factory_less_equal = {
	qb_resolve_expression_type_boolean,
	NULL,
	qb_coerce_operands_highest_rank,
	qb_set_result_prototype,
	NULL,
	qb_set_result_temporary_value,
	NULL,
	qb_select_opcode_basic,
	NULL,
	qb_transfer_operands_all,

	0,
	QB_RESULT_FROM_PURE_FUNCTION,
	QB_ADDRESS_TEMPORARY | QB_ADDRESS_BOOLEAN,
	{	QB_LE_F64_F64_I32,	QB_LE_F32_F32_I32,	QB_LE_U64_U64_I32,	QB_LE_S64_S64_I32,	QB_LE_U32_U32_I32,	QB_LE_S32_S32_I32,	QB_LE_U16_U16_I32,	QB_LE_S16_S16_I32,	QB_LE_U08_U08_I32,	QB_LE_S08_S08_I32,	},
};

qb_derived_op_factory factory_identical = {
	qb_resolve_expression_type_boolean,
	NULL,
	qb_coerce_operands_no_cast,
	qb_set_result_prototype,
	qb_validate_operands_matching_type,
	qb_set_result_temporary_value,
	NULL,
	qb_select_opcode_derived,
	NULL,
	qb_transfer_operands_all,

	0,
	QB_RESULT_FROM_PURE_FUNCTION,
	QB_ADDRESS_TEMPORARY | QB_ADDRESS_BOOLEAN,
	&factory_equal,
};

qb_derived_op_factory factory_not_identical = {
	qb_resolve_expression_type_boolean,
	NULL,
	qb_coerce_operands_no_cast,
	qb_set_result_prototype,
	qb_validate_operands_matching_type,
	qb_set_result_temporary_value,
	NULL,
	qb_select_opcode_derived,
	NULL,
	qb_transfer_operands_all,

	0,
	QB_RESULT_FROM_PURE_FUNCTION,
	QB_ADDRESS_TEMPORARY | QB_ADDRESS_BOOLEAN,
	&factory_not_equal,
};

qb_basic_op_factory factory_array_element_isset = {
	qb_resolve_expression_type_boolean,
	NULL,
	qb_coerce_operands_fetch_array_element,
	qb_set_result_prototype,
	qb_validate_operands_array_element,
	qb_set_result_temporary_value,
	NULL,
	qb_select_opcode_array_element_isset,
	NULL,
	qb_transfer_operands_array_element_isset,
	0,
	0,
	0,
	{ QB_CBOOL_F64_I32_I32, QB_CBOOL_F32_I32_I32, QB_CBOOL_I64_I32_I32, QB_CBOOL_I64_I32_I32, QB_CBOOL_I32_I32_I32, QB_CBOOL_I32_I32_I32, QB_CBOOL_I16_I32_I32, QB_CBOOL_I16_I32_I32, QB_CBOOL_I08_I32_I32, QB_CBOOL_I08_I32_I32 }
};

qb_basic_op_factory factory_object_property_isset = {
	qb_resolve_expression_type_boolean,
	NULL,
	NULL,
	qb_set_result_prototype,
	qb_validate_operands_object_property,
	qb_set_result_temporary_value,
	NULL,
	qb_select_opcode_object_property_isset,
	NULL,
	qb_transfer_operands_object_property_isset,
	0,
	0,
	0,
	{ QB_BOOL_F64_I32, QB_BOOL_F32_I32, QB_BOOL_I64_I32, QB_BOOL_I64_I32, QB_BOOL_I32_I32, QB_BOOL_I32_I32, QB_BOOL_I16_I32, QB_BOOL_I16_I32, QB_BOOL_I08_I32, QB_BOOL_I08_I32 }
};

qb_unset_op_factory factory_unset = {
	NULL,
	NULL,
	NULL,
	NULL,
	qb_validate_operands_referenceable,
	NULL,
	NULL,
	qb_select_opcode_unset,
	NULL,
	qb_transfer_operands_unset,
	0,
	QB_RESULT_HAS_SIDE_EFFECT,
	0,
	{ QB_CCLR_ARR_I32_F64, QB_CCLR_ARR_I32_F32, QB_CCLR_ARR_I32_I64, QB_CCLR_ARR_I32_I64, QB_CCLR_ARR_I32_I32, QB_CCLR_ARR_I32_I32, QB_CCLR_ARR_I32_I16, QB_CCLR_ARR_I32_I16, QB_CCLR_ARR_I32_I08, QB_CCLR_ARR_I32_I08 },
	{ QB_CLR_ARR_RSZ_U32_F64, QB_CLR_ARR_RSZ_U32_F32, QB_CLR_ARR_RSZ_U32_I64, QB_CLR_ARR_RSZ_U32_I64, QB_CLR_ARR_RSZ_U32_I32, QB_CLR_ARR_RSZ_U32_I32, QB_CLR_ARR_RSZ_U32_I16, QB_CLR_ARR_RSZ_U32_I16, QB_CLR_ARR_RSZ_U32_I08, QB_CLR_ARR_RSZ_U32_I08 },
	{ QB_CLR_ARR_RSZ_DIM_U32_U32_F64, QB_CLR_ARR_RSZ_DIM_U32_U32_F32, QB_CLR_ARR_RSZ_DIM_U32_U32_I64, QB_CLR_ARR_RSZ_DIM_U32_U32_I64, QB_CLR_ARR_RSZ_DIM_U32_U32_I32, QB_CLR_ARR_RSZ_DIM_U32_U32_I32, QB_CLR_ARR_RSZ_DIM_U32_U32_I16, QB_CLR_ARR_RSZ_DIM_U32_U32_I16, QB_CLR_ARR_RSZ_DIM_U32_U32_I08, QB_CLR_ARR_RSZ_DIM_U32_U32_I08 },
	{ QB_CLR_SCA_F64, QB_CLR_SCA_F32, QB_CLR_SCA_I64, QB_CLR_SCA_I64, QB_CLR_SCA_I32, QB_CLR_SCA_I32, QB_CLR_SCA_I16, QB_CLR_SCA_I16, QB_CLR_SCA_I08, QB_CLR_SCA_I08 }
};

qb_unset_element_op_factory factory_unset_array_element = {
	NULL,
	NULL,
	qb_coerce_operands_fetch_array_element,
	NULL,
	qb_validate_operands_referenceable,
	NULL,
	NULL,
	qb_select_opcode_unset_array_element,
	NULL,
	qb_transfer_operands_unset_array_element,
	0,
	QB_RESULT_HAS_SIDE_EFFECT,
	0,
	{ QB_CCLR_ELE_U32_U32_I32_F64, QB_CCLR_ELE_U32_U32_I32_F32, QB_CCLR_ELE_U32_U32_I32_I64, QB_CCLR_ELE_U32_U32_I32_I64, QB_CCLR_ELE_U32_U32_I32_I32, QB_CCLR_ELE_U32_U32_I32_I32, QB_CCLR_ELE_U32_U32_I32_I16, QB_CCLR_ELE_U32_U32_I32_I16, QB_CCLR_ELE_U32_U32_I32_I08, QB_CCLR_ELE_U32_U32_I32_I08 },
	{ QB_CLR_ELE_RSZ_U32_U32_U32_F64, QB_CLR_ELE_RSZ_U32_U32_U32_F32, QB_CLR_ELE_RSZ_U32_U32_U32_I64, QB_CLR_ELE_RSZ_U32_U32_U32_I64, QB_CLR_ELE_RSZ_U32_U32_U32_I32, QB_CLR_ELE_RSZ_U32_U32_U32_I32, QB_CLR_ELE_RSZ_U32_U32_U32_I16, QB_CLR_ELE_RSZ_U32_U32_U32_I16, QB_CLR_ELE_RSZ_U32_U32_U32_I08, QB_CLR_ELE_RSZ_U32_U32_U32_I08 },
};

qb_unset_op_factory factory_unset_object_property = {
	NULL,
	NULL,
	NULL,
	NULL,
	qb_validate_operands_object_property,
	NULL,
	NULL,
	qb_select_opcode_unset_object_property,
	NULL,
	qb_transfer_operands_unset_object_property,
	0,
	QB_RESULT_HAS_SIDE_EFFECT,
	0,
	{ QB_CCLR_ARR_I32_F64, QB_CCLR_ARR_I32_F32, QB_CCLR_ARR_I32_I64, QB_CCLR_ARR_I32_I64, QB_CCLR_ARR_I32_I32, QB_CCLR_ARR_I32_I32, QB_CCLR_ARR_I32_I16, QB_CCLR_ARR_I32_I16, QB_CCLR_ARR_I32_I08, QB_CCLR_ARR_I32_I08 },
	{ QB_CLR_ARR_RSZ_U32_F64, QB_CLR_ARR_RSZ_U32_F32, QB_CLR_ARR_RSZ_U32_I64, QB_CLR_ARR_RSZ_U32_I64, QB_CLR_ARR_RSZ_U32_I32, QB_CLR_ARR_RSZ_U32_I32, QB_CLR_ARR_RSZ_U32_I16, QB_CLR_ARR_RSZ_U32_I16, QB_CLR_ARR_RSZ_U32_I08, QB_CLR_ARR_RSZ_U32_I08 },
	{ QB_CLR_ARR_RSZ_DIM_U32_U32_F64, QB_CLR_ARR_RSZ_DIM_U32_U32_F32, QB_CLR_ARR_RSZ_DIM_U32_U32_I64, QB_CLR_ARR_RSZ_DIM_U32_U32_I64, QB_CLR_ARR_RSZ_DIM_U32_U32_I32, QB_CLR_ARR_RSZ_DIM_U32_U32_I32, QB_CLR_ARR_RSZ_DIM_U32_U32_I16, QB_CLR_ARR_RSZ_DIM_U32_U32_I16, QB_CLR_ARR_RSZ_DIM_U32_U32_I08, QB_CLR_ARR_RSZ_DIM_U32_U32_I08 },
	{ QB_CLR_SCA_F64, QB_CLR_SCA_F32, QB_CLR_SCA_I64, QB_CLR_SCA_I64, QB_CLR_SCA_I32, QB_CLR_SCA_I32, QB_CLR_SCA_I16, QB_CLR_SCA_I16, QB_CLR_SCA_I08, QB_CLR_SCA_I08 }
};

qb_basic_op_factory factory_branch_on_equal = {
	NULL,
	NULL,
	qb_coerce_operands_highest_rank,
	qb_set_result_prototype,
	NULL,
	qb_set_result_temporary_value,
	NULL,
	qb_select_opcode_basic,
	NULL,
	qb_transfer_operands_all,

	0,
	0,
	0,
	{	QB_IF_EQ_F64_F64,	QB_IF_EQ_F32_F32,	QB_IF_EQ_I64_I64,	QB_IF_EQ_I64_I64,	QB_IF_EQ_I32_I32,	QB_IF_EQ_I32_I32,	QB_IF_EQ_I16_I16,	QB_IF_EQ_I16_I16,	QB_IF_EQ_I08_I08,	QB_IF_EQ_I08_I08,	},
};

qb_basic_op_factory factory_branch_on_not_equal = {
	NULL,
	NULL,
	qb_coerce_operands_highest_rank,
	qb_set_result_prototype,
	NULL,
	qb_set_result_temporary_value,
	NULL,
	qb_select_opcode_basic,
	NULL,
	qb_transfer_operands_all,

	0,
	0,
	0,
	{	QB_IF_NE_F64_F64,	QB_IF_NE_F32_F32,	QB_IF_NE_I64_I64,	QB_IF_NE_I64_I64,	QB_IF_NE_I32_I32,	QB_IF_NE_I32_I32,	QB_IF_NE_I16_I16,	QB_IF_NE_I16_I16,	QB_IF_NE_I08_I08,	QB_IF_NE_I08_I08,	},
};

qb_basic_op_factory factory_branch_on_less_than = {
	NULL,
	NULL,
	qb_coerce_operands_highest_rank,
	qb_set_result_prototype,
	NULL,
	qb_set_result_temporary_value,
	NULL,
	qb_select_opcode_basic,
	NULL,
	qb_transfer_operands_all,

	0,
	0,
	0,
	{	QB_IF_LT_F64_F64,	QB_IF_LT_F32_F32,	QB_IF_LT_U64_U64,	QB_IF_LT_S64_S64,	QB_IF_LT_U32_U32,	QB_IF_LT_S32_S32,	QB_IF_LT_U16_U16,	QB_IF_LT_S16_S16,	QB_IF_LT_U08_U08,	QB_IF_LT_S08_S08,	},
};

qb_basic_op_factory factory_branch_on_less_equal = {
	NULL,
	NULL,
	qb_coerce_operands_highest_rank,
	qb_set_result_prototype,
	NULL,
	qb_set_result_temporary_value,
	NULL,
	qb_select_opcode_basic,
	NULL,
	qb_transfer_operands_all,

	0,
	0,
	0,
	{	QB_IF_LE_F64_F64,	QB_IF_LE_F32_F32,	QB_IF_LE_U64_U64,	QB_IF_LE_S64_S64,	QB_IF_LE_U32_U32,	QB_IF_LE_S32_S32,	QB_IF_LE_U16_U16,	QB_IF_LE_S16_S16,	QB_IF_LE_U08_U08,	QB_IF_LE_S08_S08,	},
};

qb_basic_op_factory factory_set_equal = {
	qb_resolve_expression_type_boolean,
	NULL,
	qb_coerce_operands_highest_rank,
	qb_set_result_prototype,
	NULL,
	qb_set_result_temporary_value,
	qb_set_result_dimensions_larger_of_two,
	qb_select_opcode_basic,
	NULL,
	qb_transfer_operands_all,

	0,
	QB_RESULT_FROM_PURE_FUNCTION,
	QB_ADDRESS_TEMPORARY | QB_ADDRESS_BOOLEAN,
	{	QB_EQ_SET_F64_F64_I32,	QB_EQ_SET_F32_F32_I32,	QB_EQ_SET_I64_I64_I32,	QB_EQ_SET_I64_I64_I32,	QB_EQ_SET_I32_I32_I32,	QB_EQ_SET_I32_I32_I32,	QB_EQ_SET_I16_I16_I32,	QB_EQ_SET_I16_I16_I32,	QB_EQ_SET_I08_I08_I32,	QB_EQ_SET_I08_I08_I32,	},
};

qb_basic_op_factory factory_set_not_equal = {
	qb_resolve_expression_type_boolean,
	NULL,
	qb_coerce_operands_highest_rank,
	qb_set_result_prototype,
	NULL,
	qb_set_result_temporary_value,
	qb_set_result_dimensions_larger_of_two,
	qb_select_opcode_basic,
	NULL,
	qb_transfer_operands_all,

	0,
	QB_RESULT_FROM_PURE_FUNCTION,
	QB_ADDRESS_TEMPORARY | QB_ADDRESS_BOOLEAN,
	{	QB_NE_SET_F64_F64_I32,	QB_NE_SET_F32_F32_I32,	QB_NE_SET_I64_I64_I32,	QB_NE_SET_I64_I64_I32,	QB_NE_SET_I32_I32_I32,	QB_NE_SET_I32_I32_I32,	QB_NE_SET_I16_I16_I32,	QB_NE_SET_I16_I16_I32,	QB_NE_SET_I08_I08_I32,	QB_NE_SET_I08_I08_I32,	},
};

qb_basic_op_factory factory_set_less_than = {
	qb_resolve_expression_type_boolean,
	NULL,
	qb_coerce_operands_highest_rank,
	qb_set_result_prototype,
	NULL,
	qb_set_result_temporary_value,
	qb_set_result_dimensions_larger_of_two,
	qb_select_opcode_basic,
	NULL,
	qb_transfer_operands_all,

	0,
	QB_RESULT_FROM_PURE_FUNCTION,
	QB_ADDRESS_TEMPORARY | QB_ADDRESS_BOOLEAN,
	{	QB_LT_SET_F64_F64_I32,	QB_LT_SET_F32_F32_I32,	QB_LT_SET_U64_U64_I32,	QB_LT_SET_S64_S64_I32,	QB_LT_SET_U32_U32_I32,	QB_LT_SET_S32_S32_I32,	QB_LT_SET_U16_U16_I32,	QB_LT_SET_S16_S16_I32,	QB_LT_SET_U08_U08_I32,	QB_LT_SET_S08_S08_I32,	},
};

qb_basic_op_factory factory_set_less_equal = {
	qb_resolve_expression_type_boolean,
	NULL,
	qb_coerce_operands_highest_rank,
	qb_set_result_prototype,
	NULL,
	qb_set_result_temporary_value,
	qb_set_result_dimensions_larger_of_two,
	qb_select_opcode_basic,
	NULL,
	qb_transfer_operands_all,

	0,
	QB_RESULT_FROM_PURE_FUNCTION,
	QB_ADDRESS_TEMPORARY | QB_ADDRESS_BOOLEAN,
	{	QB_LE_SET_F64_F64_I32,	QB_LE_SET_F32_F32_I32,	QB_LE_SET_U64_U64_I32,	QB_LE_SET_S64_S64_I32,	QB_LE_SET_U32_U32_I32,	QB_LE_SET_S32_S32_I32,	QB_LE_SET_U16_U16_I32,	QB_LE_SET_S16_S16_I32,	QB_LE_SET_U08_U08_I32,	QB_LE_SET_S08_S08_I32,	},
};

qb_derived_op_factory factory_set_greater_equal = {
	qb_resolve_expression_type_boolean,
	NULL,
	qb_coerce_operands_highest_rank,
	qb_set_result_prototype,
	NULL,
	qb_set_result_temporary_value,
	qb_set_result_dimensions_larger_of_two,
	qb_select_opcode_derived,
	NULL,
	qb_transfer_operands_reverse_binary,

	0,
	QB_RESULT_FROM_PURE_FUNCTION,
	QB_ADDRESS_TEMPORARY | QB_ADDRESS_BOOLEAN,
	&factory_set_less_equal,
};

qb_derived_op_factory factory_set_greater_than = {
	qb_resolve_expression_type_boolean,
	NULL,
	qb_coerce_operands_highest_rank,
	qb_set_result_prototype,
	NULL,
	qb_set_result_temporary_value,
	qb_set_result_dimensions_larger_of_two,
	qb_select_opcode_derived,
	NULL,
	qb_transfer_operands_reverse_binary,

	0,
	QB_RESULT_FROM_PURE_FUNCTION,
	QB_ADDRESS_TEMPORARY | QB_ADDRESS_BOOLEAN,
	&factory_set_less_than,
};

qb_simple_op_factory factory_set_not = {
	qb_resolve_expression_type_boolean,
	NULL,
	qb_coerce_operands_all,
	qb_set_result_prototype,
	NULL,
	qb_set_result_temporary_value,
	qb_set_result_dimensions_first_operand,
	qb_select_opcode_simple,
	NULL,
	qb_transfer_operands_all,

	QB_COERCE_TO_BOOLEAN,
	QB_RESULT_FROM_PURE_FUNCTION,
	QB_ADDRESS_TEMPORARY | QB_ADDRESS_BOOLEAN,
	QB_NOT_SET_I32_I32,
};

qb_simple_op_factory factory_all = {
	qb_resolve_expression_type_boolean,
	NULL,
	qb_coerce_operands_all,
	qb_set_result_prototype,
	NULL,
	NULL,
	NULL,
	qb_select_opcode_simple,
	NULL,
	qb_transfer_operands_all,

	QB_COERCE_TO_BOOLEAN,
	QB_RESULT_FROM_PURE_FUNCTION,
	QB_ADDRESS_TEMPORARY | QB_ADDRESS_BOOLEAN,
	QB_ALL_I32_I32,
};

qb_simple_op_factory factory_any = {
	qb_resolve_expression_type_boolean,
	NULL,
	qb_coerce_operands_all,
	qb_set_result_prototype,
	NULL,
	NULL,
	NULL,
	qb_select_opcode_simple,
	NULL,
	qb_transfer_operands_all,

	QB_COERCE_TO_BOOLEAN,
	QB_RESULT_FROM_PURE_FUNCTION,
	QB_ADDRESS_TEMPORARY | QB_ADDRESS_BOOLEAN,
	QB_ANY_I32_I32,
};

qb_simple_op_factory factory_empty_string = {
	qb_resolve_expression_type_string,
	NULL,
	NULL,
	qb_set_result_prototype,
	NULL,
	qb_set_result_empty_string,
	NULL,
	qb_select_opcode_simple,
	NULL,
	qb_transfer_operands_empty_string,

	0,
	QB_RESULT_HAS_SIDE_EFFECT,
	QB_ADDRESS_TEMPORARY | QB_ADDRESS_STRING,
	QB_CLR_ARR_RSZ_U32_I08,
};

qb_string_op_factory factory_add_variable = {
	qb_resolve_expression_type_string,
	NULL,
	qb_coerce_operands_add_variable,
	qb_set_result_prototype,
	NULL,
	qb_set_result_add_string,
	NULL,
	qb_select_opcode_add_variable,
	NULL,
	qb_transfer_operands_add_string,

	0,
	QB_RESULT_HAS_SIDE_EFFECT,
	QB_ADDRESS_TEMPORARY | QB_ADDRESS_STRING,
	{	QB_APP_VAR_F64_U32_U08,	QB_APP_VAR_F32_U32_U08,	QB_APP_VAR_U64_U32_U08,	QB_APP_VAR_S64_U32_U08,	QB_APP_VAR_U32_U32_U08,	QB_APP_VAR_S32_U32_U08,	QB_APP_VAR_U16_U32_U08,	QB_APP_VAR_S16_U32_U08,	QB_APP_VAR_U08_U32_U08,	QB_APP_VAR_S08_U32_U08,	},
	{	QB_APP_VAR_DIM_F64_U32_U32_U08,	QB_APP_VAR_DIM_F32_U32_U32_U08,	QB_APP_VAR_DIM_U64_U32_U32_U08,	QB_APP_VAR_DIM_S64_U32_U32_U08,	QB_APP_VAR_DIM_U32_U32_U32_U08,	QB_APP_VAR_DIM_S32_U32_U32_U08,	QB_APP_VAR_DIM_U16_U32_U32_U08,	QB_APP_VAR_DIM_S16_U32_U32_U08,	QB_APP_VAR_DIM_U08_U32_U32_U08,	QB_APP_VAR_DIM_S08_U32_U32_U08,	},
	QB_APP_STR_U08_U32_U08,
};

qb_simple_op_factory factory_add_string = {
	qb_resolve_expression_type_string,
	NULL,
	qb_coerce_operands_add_string,
	qb_set_result_prototype,
	NULL,
	qb_set_result_add_string,
	NULL,
	qb_select_opcode_simple,
	NULL,
	qb_transfer_operands_add_string,

	0,
	QB_RESULT_HAS_SIDE_EFFECT,
	QB_ADDRESS_TEMPORARY | QB_ADDRESS_STRING,
	QB_APP_STR_U08_U32_U08,
};

qb_derived_op_factory factory_concat = {
	qb_resolve_expression_type_string,
	NULL,
	qb_coerce_operands_concat,
	qb_set_result_prototype,
	NULL,
	qb_set_result_concat,
	NULL,
	qb_select_opcode_derived,
	NULL,
	qb_transfer_operands_derived,

	0,
	QB_RESULT_HAS_SIDE_EFFECT,
	QB_ADDRESS_TEMPORARY | QB_ADDRESS_STRING,
	&factory_add_variable,
};

qb_string_op_factory factory_print = {
	qb_resolve_expression_type_boolean,
	qb_link_results_print,
	qb_coerce_operands_print,
	qb_set_result_prototype,
	NULL,
	qb_set_result_true,
	NULL,
	qb_select_opcode_print,
	NULL,
	qb_transfer_operands_print,

	0,
	QB_RESULT_HAS_SIDE_EFFECT,
	QB_ADDRESS_TEMPORARY | QB_ADDRESS_BOOLEAN,
	{	QB_PRN_F64,	QB_PRN_F32,	QB_PRN_U64,	QB_PRN_S64,	QB_PRN_U32,	QB_PRN_S32,	QB_PRN_U16,	QB_PRN_S16,	QB_PRN_U08,	QB_PRN_S08,	},
	{	QB_PRN_DIM_F64_U32,	QB_PRN_DIM_F32_U32,	QB_PRN_DIM_U64_U32,	QB_PRN_DIM_S64_U32,	QB_PRN_DIM_U32_U32,	QB_PRN_DIM_S32_U32,	QB_PRN_DIM_U16_U32,	QB_PRN_DIM_S16_U32,	QB_PRN_DIM_U08_U32,	QB_PRN_DIM_S08_U32,	},
	QB_PRN_STR_U08,
};

qb_derived_op_factory factory_echo = {
	NULL,
	qb_link_results_print,
	qb_coerce_operands_print,
	qb_set_result_prototype,
	NULL,
	NULL,
	NULL,
	qb_select_opcode_derived,
	NULL,
	qb_transfer_operands_print,

	0,
	QB_RESULT_HAS_SIDE_EFFECT,
	0,
	&factory_print,
};

qb_simple_op_factory factory_return = {
	NULL,
	qb_link_results_return,
	qb_coerce_operands_return,
	qb_set_result_prototype,
	qb_validate_operands_return,
	NULL,
	NULL,
	qb_select_opcode_simple,
	NULL,
	qb_transfer_operands_return,

	0,
	0,
	0,
	QB_RET,
};

qb_simple_op_factory factory_exit = {
	NULL,
	NULL,
	qb_coerce_operands_int,
	qb_set_result_prototype,
	NULL,
	NULL,
	NULL,
	qb_select_opcode_simple,
	NULL,
	qb_transfer_operands_all,

	0,
	0,
	QB_ADDRESS_TEMPORARY,
	QB_EXIT_I32,
};

qb_simple_op_factory factory_ext = {
	NULL,
	NULL,
	NULL,
	qb_set_result_prototype,
	NULL,
	NULL,
	NULL,
	qb_select_opcode_simple,
	NULL,
	qb_transfer_operands_all,

	0,
	0,
	0,
	QB_EXT_U32,
};

qb_simple_op_factory factory_jump = {
	NULL,
	NULL,
	NULL,
	qb_set_result_prototype,
	NULL,
	NULL,
	NULL,
	qb_select_opcode_simple,
	NULL,
	NULL,

	0,
	0,
	0,
	QB_JMP,
};

qb_simple_op_factory factory_branch_on_true = {
	NULL,
	NULL,
	qb_coerce_operands_boolean,
	qb_set_result_prototype,
	NULL,
	NULL,
	NULL,
	qb_select_opcode_simple,
	NULL,
	qb_transfer_operands_all,

	QB_COERCE_TO_BOOLEAN,
	0,
	0,
	QB_IF_T_I32,
};

qb_simple_op_factory factory_branch_on_false = {
	NULL,
	NULL,
	qb_coerce_operands_boolean,
	qb_set_result_prototype,
	NULL,
	NULL,
	NULL,
	qb_select_opcode_simple,
	NULL,
	qb_transfer_operands_all,

	QB_COERCE_TO_BOOLEAN,
	0,
	0,
	QB_IF_F_I32,
};

qb_simple_op_factory factory_foreach_reset = {
	NULL,
	NULL,
	NULL,
	qb_set_result_prototype,
	NULL,
	qb_set_result_foreach_reset,
	NULL,
	qb_select_opcode_simple,
	NULL,
	qb_transfer_operands_foreach_reset,

	0,
	0,
	0,
	QB_MOV_I32_I32,
};

qb_simple_op_factory factory_foreach_fetch = {
	qb_resolve_expression_type_first_operand,
	NULL,
	NULL,
	qb_set_result_prototype,
	NULL,
	qb_set_result_foreach_fetch,
	NULL,
	qb_select_opcode_simple,
	NULL,
	qb_transfer_operands_foreach_fetch,

	0,
	0,
	0,
	QB_INC_IF_GT_U32_U32,
};

qb_simple_op_factory factory_loop = {
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	qb_set_result_first_operand,
	NULL,
	qb_select_opcode_simple,
	NULL,
	qb_transfer_operands_assign,

	0,
	0,
	0,
	QB_INC_IF_GT_U32_U32,
};

/*
qb_fcall_op_factory factory_fcall = {
	qb_append_fcall,
	NULL,
	0,
	qb_set_result_prototype,
	0,
	QB_ADDRESS_TEMPORARY,
	QB_FCALL,
};
*/

qb_float_op_factory factory_sin = {
	qb_resolve_expression_type_first_operand,
	qb_link_results_all_operands,
	qb_coerce_operands_all,
	qb_set_result_prototype,
	NULL,
	qb_set_result_temporary_value,
	qb_set_result_dimensions_first_operand,
	qb_select_opcode_basic,
	NULL,
	qb_transfer_operands_all,

	QB_COERCE_TO_LVALUE_TYPE | QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION,
	QB_ADDRESS_TEMPORARY,
	{	QB_SIN_F64_F64,			QB_SIN_F32_F32,	},
};

qb_float_op_factory factory_asin = {
	qb_resolve_expression_type_first_operand,
	qb_link_results_all_operands,
	qb_coerce_operands_all,
	qb_set_result_prototype,
	NULL,
	qb_set_result_temporary_value,
	qb_set_result_dimensions_first_operand,
	qb_select_opcode_basic,
	NULL,
	qb_transfer_operands_all,

	QB_COERCE_TO_LVALUE_TYPE | QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION,
	QB_ADDRESS_TEMPORARY,
	{	QB_ASIN_F64_F64,		QB_ASIN_F32_F32,	},
};

qb_float_op_factory factory_cos = {
	qb_resolve_expression_type_first_operand,
	qb_link_results_all_operands,
	qb_coerce_operands_all,
	qb_set_result_prototype,
	NULL,
	qb_set_result_temporary_value,
	qb_set_result_dimensions_first_operand,
	qb_select_opcode_basic,
	NULL,
	qb_transfer_operands_all,

	QB_COERCE_TO_LVALUE_TYPE | QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION,
	QB_ADDRESS_TEMPORARY,
	{	QB_COS_F64_F64,			QB_COS_F32_F32,	},
};

qb_float_op_factory factory_acos = {
	qb_resolve_expression_type_first_operand,
	qb_link_results_all_operands,
	qb_coerce_operands_all,
	qb_set_result_prototype,
	NULL,
	qb_set_result_temporary_value,
	qb_set_result_dimensions_first_operand,
	qb_select_opcode_basic,
	NULL,
	qb_transfer_operands_all,

	QB_COERCE_TO_LVALUE_TYPE | QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION,
	QB_ADDRESS_TEMPORARY,
	{	QB_ACOS_F64_F64,		QB_ACOS_F32_F32,	},
};

qb_float_op_factory factory_tan = {
	qb_resolve_expression_type_first_operand,
	qb_link_results_all_operands,
	qb_coerce_operands_all,
	qb_set_result_prototype,
	NULL,
	qb_set_result_temporary_value,
	qb_set_result_dimensions_first_operand,
	qb_select_opcode_basic,
	NULL,
	qb_transfer_operands_all,

	QB_COERCE_TO_LVALUE_TYPE | QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION,
	QB_ADDRESS_TEMPORARY,
	{	QB_TAN_F64_F64,			QB_TAN_F32_F32,	},
};

qb_float_op_factory factory_atan = {
	qb_resolve_expression_type_first_operand,
	qb_link_results_all_operands,
	qb_coerce_operands_all,
	qb_set_result_prototype,
	NULL,
	qb_set_result_temporary_value,
	qb_set_result_dimensions_first_operand,
	qb_select_opcode_basic,
	NULL,
	qb_transfer_operands_all,

	QB_COERCE_TO_LVALUE_TYPE | QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION,
	QB_ADDRESS_TEMPORARY,
	{	QB_ATAN_F64_F64,		QB_ATAN_F32_F32,	},
};

qb_float_op_factory factory_atan2 = {
	qb_resolve_expression_type_highest_rank,
	qb_link_results_all_operands,
	qb_coerce_operands_all,
	qb_set_result_prototype,
	NULL,
	qb_set_result_temporary_value,
	qb_set_result_dimensions_larger_of_two,
	qb_select_opcode_basic,
	NULL,
	qb_transfer_operands_all,

	QB_COERCE_TO_LVALUE_TYPE | QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION,
	QB_ADDRESS_TEMPORARY,
	{	QB_ATAN2_F32_F32_F32,	QB_ATAN2_F32_F32_F32,	},
};

qb_float_op_factory factory_sinh = {
	qb_resolve_expression_type_first_operand,
	qb_link_results_all_operands,
	qb_coerce_operands_all,
	qb_set_result_prototype,
	NULL,
	qb_set_result_temporary_value,
	qb_set_result_dimensions_first_operand,
	qb_select_opcode_basic,
	NULL,
	qb_transfer_operands_all,

	QB_COERCE_TO_LVALUE_TYPE | QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION,
	QB_ADDRESS_TEMPORARY,
	{	QB_SINH_F64_F64,		QB_SINH_F32_F32,	},
};

qb_float_op_factory factory_asinh = {
	qb_resolve_expression_type_first_operand,
	qb_link_results_all_operands,
	qb_coerce_operands_all,
	qb_set_result_prototype,
	NULL,
	qb_set_result_temporary_value,
	qb_set_result_dimensions_first_operand,
	qb_select_opcode_basic,
	NULL,
	qb_transfer_operands_all,

	QB_COERCE_TO_LVALUE_TYPE | QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION,
	QB_ADDRESS_TEMPORARY,
	{	QB_ASINH_F64_F64,		QB_ASINH_F32_F32,	},
};

qb_float_op_factory factory_cosh = {
	qb_resolve_expression_type_first_operand,
	qb_link_results_all_operands,
	qb_coerce_operands_all,
	qb_set_result_prototype,
	NULL,
	qb_set_result_temporary_value,
	qb_set_result_dimensions_first_operand,
	qb_select_opcode_basic,
	NULL,
	qb_transfer_operands_all,

	QB_COERCE_TO_LVALUE_TYPE | QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION,
	QB_ADDRESS_TEMPORARY,
	{	QB_COSH_F64_F64,		QB_COSH_F32_F32,	},
};

qb_float_op_factory factory_acosh = {
	qb_resolve_expression_type_first_operand,
	qb_link_results_all_operands,
	qb_coerce_operands_all,
	qb_set_result_prototype,
	NULL,
	qb_set_result_temporary_value,
	qb_set_result_dimensions_first_operand,
	qb_select_opcode_basic,
	NULL,
	qb_transfer_operands_all,

	QB_COERCE_TO_LVALUE_TYPE | QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION,
	QB_ADDRESS_TEMPORARY,
	{	QB_ACOSH_F64_F64,		QB_ACOSH_F32_F32,	},
};

qb_float_op_factory factory_tanh = {
	qb_resolve_expression_type_first_operand,
	qb_link_results_all_operands,
	qb_coerce_operands_all,
	qb_set_result_prototype,
	NULL,
	qb_set_result_temporary_value,
	qb_set_result_dimensions_first_operand,
	qb_select_opcode_basic,
	NULL,
	qb_transfer_operands_all,

	QB_COERCE_TO_LVALUE_TYPE | QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION,
	QB_ADDRESS_TEMPORARY,
	{	QB_TANH_F64_F64,		QB_TANH_F32_F32,	},
};

qb_float_op_factory factory_atanh = {
	qb_resolve_expression_type_first_operand,
	qb_link_results_all_operands,
	qb_coerce_operands_all,
	qb_set_result_prototype,
	NULL,
	qb_set_result_temporary_value,
	qb_set_result_dimensions_first_operand,
	qb_select_opcode_basic,
	NULL,
	qb_transfer_operands_all,

	QB_COERCE_TO_LVALUE_TYPE | QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION,
	QB_ADDRESS_TEMPORARY,
	{	QB_ATANH_F64_F64,		QB_ATANH_F32_F32,	},
};

qb_float_op_factory factory_ceil = {
	qb_resolve_expression_type_first_operand,
	qb_link_results_all_operands,
	qb_coerce_operands_all,
	qb_set_result_prototype,
	NULL,
	qb_set_result_temporary_value,
	qb_set_result_dimensions_first_operand,
	qb_select_opcode_basic,
	NULL,
	qb_transfer_operands_all,

	QB_COERCE_TO_LVALUE_TYPE | QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION,
	QB_ADDRESS_TEMPORARY,
	{	QB_CEIL_F64_F64,		QB_CEIL_F32_F32,	},
};

qb_float_op_factory factory_floor = {
	qb_resolve_expression_type_first_operand,
	qb_link_results_all_operands,
	qb_coerce_operands_all,
	qb_set_result_prototype,
	NULL,
	qb_set_result_temporary_value,
	qb_set_result_dimensions_first_operand,
	qb_select_opcode_basic,
	NULL,
	qb_transfer_operands_all,

	QB_COERCE_TO_LVALUE_TYPE | QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION,
	QB_ADDRESS_TEMPORARY,
	{	QB_FLOOR_F64_F64,		QB_FLOOR_F32_F32,	},
};

qb_float_op_factory factory_fract = {
	qb_resolve_expression_type_first_operand,
	qb_link_results_all_operands,
	qb_coerce_operands_all,
	qb_set_result_prototype,
	NULL,
	qb_set_result_temporary_value,
	qb_set_result_dimensions_first_operand,
	qb_select_opcode_basic,
	NULL,
	qb_transfer_operands_all,

	QB_COERCE_TO_LVALUE_TYPE | QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION,
	QB_ADDRESS_TEMPORARY,
	{	QB_FRACT_F64_F64,		QB_FRACT_F32_F32,	},
};

qb_float_op_factory factory_round = {
	qb_resolve_expression_type_first_operand,
	qb_link_results_all_operands,
	qb_coerce_operands_all,
	qb_set_result_prototype,
	NULL,
	qb_set_result_temporary_value,
	qb_set_result_dimensions_largest_of_three,
	qb_select_opcode_basic,
	NULL,
	qb_transfer_operands_all,

	QB_COERCE_TO_LVALUE_TYPE | QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION,
	QB_ADDRESS_TEMPORARY,
	{	QB_ROUND_F64_I32_I32_F64,	QB_ROUND_F32_I32_I32_F32,	},
};

qb_float_op_factory factory_log = {
	qb_resolve_expression_type_first_operand,
	qb_link_results_all_operands,
	qb_coerce_operands_all,
	qb_set_result_prototype,
	NULL,
	qb_set_result_temporary_value,
	qb_set_result_dimensions_first_operand,
	qb_select_opcode_basic,
	NULL,
	qb_transfer_operands_all,

	QB_COERCE_TO_LVALUE_TYPE | QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION,
	QB_ADDRESS_TEMPORARY,
	{	QB_LOG_F64_F64,			QB_LOG_F32_F32,	},
};

qb_float_op_factory factory_log1p = {
	qb_resolve_expression_type_first_operand,
	qb_link_results_all_operands,
	qb_coerce_operands_all,
	qb_set_result_prototype,
	NULL,
	qb_set_result_temporary_value,
	qb_set_result_dimensions_first_operand,
	qb_select_opcode_basic,
	NULL,
	qb_transfer_operands_all,

	QB_COERCE_TO_LVALUE_TYPE | QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION,
	QB_ADDRESS_TEMPORARY,
	{	QB_LOG1P_F64_F64,		QB_LOG1P_F32_F32,	},
};

qb_float_op_factory factory_log10 = {
	qb_resolve_expression_type_first_operand,
	qb_link_results_all_operands,
	qb_coerce_operands_all,
	qb_set_result_prototype,
	NULL,
	qb_set_result_temporary_value,
	qb_set_result_dimensions_first_operand,
	qb_select_opcode_basic,
	NULL,
	qb_transfer_operands_all,

	QB_COERCE_TO_LVALUE_TYPE | QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION,
	QB_ADDRESS_TEMPORARY,
	{	QB_LOG10_F64_F64,		QB_LOG10_F32_F32,	},
};

qb_float_op_factory factory_log2 = {
	qb_resolve_expression_type_first_operand,
	qb_link_results_all_operands,
	qb_coerce_operands_all,
	qb_set_result_prototype,
	NULL,
	qb_set_result_temporary_value,
	qb_set_result_dimensions_first_operand,
	qb_select_opcode_basic,
	NULL,
	qb_transfer_operands_all,

	QB_COERCE_TO_LVALUE_TYPE | QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION,
	QB_ADDRESS_TEMPORARY,
	{	QB_LOG2_F64_F64,		QB_LOG2_F32_F32,	},
};

qb_float_op_factory factory_exp = {
	qb_resolve_expression_type_first_operand,
	qb_link_results_all_operands,
	qb_coerce_operands_all,
	qb_set_result_prototype,
	NULL,
	qb_set_result_temporary_value,
	qb_set_result_dimensions_first_operand,
	qb_select_opcode_basic,
	NULL,
	qb_transfer_operands_all,

	QB_COERCE_TO_LVALUE_TYPE | QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION,
	QB_ADDRESS_TEMPORARY,
	{	QB_EXP_F64_F64,			QB_EXP_F32_F32,	},
};

qb_float_op_factory factory_exp2 = {
	qb_resolve_expression_type_first_operand,
	qb_link_results_all_operands,
	qb_coerce_operands_all,
	qb_set_result_prototype,
	NULL,
	qb_set_result_temporary_value,
	qb_set_result_dimensions_first_operand,
	qb_select_opcode_basic,
	NULL,
	qb_transfer_operands_all,

	QB_COERCE_TO_LVALUE_TYPE | QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION,
	QB_ADDRESS_TEMPORARY,
	{	QB_EXP2_F64_F64,		QB_EXP2_F32_F32,	},
};

qb_float_op_factory factory_expm1 = {
	qb_resolve_expression_type_first_operand,
	qb_link_results_all_operands,
	qb_coerce_operands_all,
	qb_set_result_prototype,
	NULL,
	qb_set_result_temporary_value,
	qb_set_result_dimensions_first_operand,
	qb_select_opcode_basic,
	NULL,
	qb_transfer_operands_all,

	QB_COERCE_TO_LVALUE_TYPE | QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION,
	QB_ADDRESS_TEMPORARY,
	{	QB_EXPM1_F64_F64,		QB_EXPM1_F32_F32,	},
};

qb_float_op_factory factory_sqrt = {
	qb_resolve_expression_type_first_operand,
	qb_link_results_all_operands,
	qb_coerce_operands_all,
	qb_set_result_prototype,
	NULL,
	qb_set_result_temporary_value,
	qb_set_result_dimensions_first_operand,
	qb_select_opcode_basic,
	NULL,
	qb_transfer_operands_all,

	QB_COERCE_TO_LVALUE_TYPE | QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION,
	QB_ADDRESS_TEMPORARY,
	{	QB_SQRT_F64_F64,		QB_SQRT_F32_F32,	},
};

qb_float_op_factory factory_rsqrt = {
	qb_resolve_expression_type_first_operand,
	qb_link_results_all_operands,
	qb_coerce_operands_all,
	qb_set_result_prototype,
	NULL,
	qb_set_result_temporary_value,
	qb_set_result_dimensions_first_operand,
	qb_select_opcode_basic,
	NULL,
	qb_transfer_operands_all,

	QB_COERCE_TO_LVALUE_TYPE | QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION,
	QB_ADDRESS_TEMPORARY,
	{	QB_RSQRT_F64_F64,		QB_RSQRT_F32_F32,	},
};

qb_float_op_factory factory_pow = {
	qb_resolve_expression_type_highest_rank,
	qb_link_results_all_operands,
	qb_coerce_operands_all,
	qb_set_result_prototype,
	NULL,
	qb_set_result_temporary_value,
	qb_set_result_dimensions_larger_of_two,
	qb_select_opcode_basic,
	NULL,
	qb_transfer_operands_all,

	QB_COERCE_TO_LVALUE_TYPE | QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION,
	QB_ADDRESS_TEMPORARY,
	{	QB_POW_F64_F64_F64,		QB_POW_F32_F32_F32,	},
};

qb_float_op_factory factory_hypot = {
	qb_resolve_expression_type_highest_rank,
	qb_link_results_all_operands,
	qb_coerce_operands_all,
	qb_set_result_prototype,
	NULL,
	qb_set_result_temporary_value,
	qb_set_result_dimensions_larger_of_two,
	qb_select_opcode_basic,
	NULL,
	qb_transfer_operands_all,

	QB_COERCE_TO_LVALUE_TYPE | QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION,
	QB_ADDRESS_TEMPORARY,
	{	QB_HYPOT_F64_F64_F64,	QB_HYPOT_F32_F32_F32,	},
};

qb_float_op_factory factory_sign = {
	qb_resolve_expression_type_first_operand,
	qb_link_results_all_operands,
	qb_coerce_operands_all,
	qb_set_result_prototype,
	NULL,
	qb_set_result_temporary_value,
	qb_set_result_dimensions_first_operand,
	qb_select_opcode_basic,
	NULL,
	qb_transfer_operands_all,

	QB_COERCE_TO_LVALUE_TYPE | QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION,
	QB_ADDRESS_TEMPORARY,
	{	QB_SIGN_F64_F64,		QB_SIGN_F32_F32,	},
};

qb_float_op_factory factory_step = {
	qb_resolve_expression_type_highest_rank,
	qb_link_results_all_operands,
	qb_coerce_operands_all,
	qb_set_result_prototype,
	NULL,
	qb_set_result_temporary_value,
	qb_set_result_dimensions_larger_of_two,
	qb_select_opcode_basic,
	NULL,
	qb_transfer_operands_all,

	QB_COERCE_TO_LVALUE_TYPE | QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION,
	QB_ADDRESS_TEMPORARY,
	{	QB_STEP_F64_F64_F64,		QB_STEP_F32_F32_F32,	},
};

qb_float_op_factory factory_clamp = {
	qb_resolve_expression_type_highest_rank,
	qb_link_results_all_operands,
	qb_coerce_operands_all,
	qb_set_result_prototype,
	NULL,
	qb_set_result_temporary_value,
	qb_set_result_dimensions_largest_of_three,
	qb_select_opcode_basic,
	NULL,
	qb_transfer_operands_all,

	QB_COERCE_TO_LVALUE_TYPE | QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION,
	QB_ADDRESS_TEMPORARY,
	{	QB_CLAMP_F64_F64_F64_F64,	QB_CLAMP_F32_F32_F32_F32,	},
};

qb_float_op_factory factory_mix = {
	qb_resolve_expression_type_highest_rank,
	qb_link_results_all_operands,
	qb_coerce_operands_all,
	qb_set_result_prototype,
	NULL,
	qb_set_result_temporary_value,
	qb_set_result_dimensions_largest_of_three,
	qb_select_opcode_basic,
	NULL,
	qb_transfer_operands_all,

	QB_COERCE_TO_LVALUE_TYPE | QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION,
	QB_ADDRESS_TEMPORARY,
	{	QB_MIX_F64_F64_F64_F64,		QB_MIX_F32_F32_F32_F32,	},
};

qb_float_op_factory factory_smooth_step = {
	qb_resolve_expression_type_highest_rank,
	qb_link_results_all_operands,
	qb_coerce_operands_all,
	qb_set_result_prototype,
	NULL,
	qb_set_result_temporary_value,
	qb_set_result_dimensions_largest_of_three,
	qb_select_opcode_basic,
	NULL,
	qb_transfer_operands_all,

	QB_COERCE_TO_LVALUE_TYPE | QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION,
	QB_ADDRESS_TEMPORARY,
	{	QB_SSTEP_F64_F64_F64_F64,	QB_SSTEP_F32_F32_F32_F32,	},
};

qb_float_op_factory factory_deg2rad = {
	qb_resolve_expression_type_first_operand,
	qb_link_results_all_operands,
	qb_coerce_operands_all,
	qb_set_result_prototype,
	NULL,
	qb_set_result_temporary_value,
	qb_set_result_dimensions_first_operand,
	qb_select_opcode_basic,
	NULL,
	qb_transfer_operands_all,

	QB_COERCE_TO_LVALUE_TYPE | QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION,
	QB_ADDRESS_TEMPORARY,
	{	QB_DEG2RAD_F64_F64,		QB_DEG2RAD_F32_F32,	},
};

qb_float_op_factory factory_rad2deg = {
	qb_resolve_expression_type_first_operand,
	qb_link_results_all_operands,
	qb_coerce_operands_all,
	qb_set_result_prototype,
	NULL,
	qb_set_result_temporary_value,
	qb_set_result_dimensions_first_operand,
	qb_select_opcode_basic,
	NULL,
	qb_transfer_operands_all,

	QB_COERCE_TO_LVALUE_TYPE | QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION,
	QB_ADDRESS_TEMPORARY,
	{	QB_RAD2DEG_F64_F64,		QB_RAD2DEG_F32_F32,	},
};

qb_float_op_factory factory_is_finite = {
	qb_resolve_expression_type_boolean,
	NULL,
	qb_coerce_operands_first_operand,
	qb_set_result_prototype,
	NULL,
	qb_set_result_temporary_value,
	qb_set_result_dimensions_first_operand,
	qb_select_opcode_basic,
	NULL,
	qb_transfer_operands_all,

	QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION,
	QB_ADDRESS_TEMPORARY | QB_ADDRESS_BOOLEAN,
	{	QB_FIN_F64_I32,		QB_FIN_F32_I32,	},
};

qb_float_op_factory factory_is_infinite = {
	qb_resolve_expression_type_boolean,
	NULL,
	qb_coerce_operands_first_operand,
	qb_set_result_prototype,
	NULL,
	qb_set_result_temporary_value,
	qb_set_result_dimensions_first_operand,
	qb_select_opcode_basic,
	NULL,
	qb_transfer_operands_all,

	QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION,
	QB_ADDRESS_TEMPORARY | QB_ADDRESS_BOOLEAN,
	{	QB_INF_F64_I32,		QB_INF_F32_I32,	},
};

qb_float_op_factory factory_is_nan = {
	qb_resolve_expression_type_boolean,
	NULL,
	qb_coerce_operands_first_operand,
	qb_set_result_prototype,
	NULL,
	qb_set_result_temporary_value,
	qb_set_result_dimensions_first_operand,
	qb_select_opcode_basic,
	NULL,
	qb_transfer_operands_all,

	QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION,
	QB_ADDRESS_TEMPORARY | QB_ADDRESS_BOOLEAN,
	{	QB_NAN_F64_I32,		QB_NAN_F32_I32,	},
};	

qb_basic_op_factory factory_abs = {
	qb_resolve_expression_type_first_operand,
	NULL,
	qb_coerce_operands_all,
	qb_set_result_prototype,
	NULL,
	qb_set_result_temporary_value,
	qb_set_result_dimensions_first_operand,
	qb_select_opcode_basic,
	NULL,
	qb_transfer_operands_all,

	QB_COERCE_TO_SIGNED,
	QB_RESULT_FROM_PURE_FUNCTION,
	QB_ADDRESS_TEMPORARY,
	{	QB_ABS_F64_F64,	QB_ABS_F32_F32,	0,	QB_ABS_S64_S64,	0,	QB_ABS_S32_S32,	0,	QB_ABS_S16_S16,	0,	QB_ABS_S08_S08,	},
};

qb_minmax_op_factory factory_min = {
	qb_resolve_expression_type_highest_rank,
	NULL,
	qb_coerce_operands_all,
	qb_set_result_prototype,
	qb_validate_operands_minmax,
	NULL,
	NULL,
	qb_select_opcode_basic,
	NULL,
	qb_transfer_operands_minmax,

	0,
	QB_RESULT_FROM_PURE_FUNCTION,
	QB_ADDRESS_TEMPORARY,
	{	QB_MIN_F64_F64_F64,	QB_MIN_F32_F32_F32,	QB_MIN_U64_U64_U64,	QB_MIN_S64_S64_S64,	QB_MIN_U32_U32_U32,	QB_MIN_S32_S32_S32,	QB_MIN_U16_U16_U16,	QB_MIN_S16_S16_S16,	QB_MIN_U08_U08_U08,		QB_MIN_S08_S08_S08,	},
	{	QB_AMIN_F64_F64,	QB_AMIN_F32_F32,	QB_AMIN_U64_U64,	QB_AMIN_S64_S64,	QB_AMIN_U32_U32,	QB_AMIN_S32_S32,	QB_AMIN_U16_U16,	QB_AMIN_S16_S16,	QB_AMIN_U08_U08,		QB_AMIN_S08_S08,	},
};

qb_minmax_op_factory factory_max = {
	qb_resolve_expression_type_highest_rank,
	NULL,
	qb_coerce_operands_all,
	qb_set_result_prototype,
	qb_validate_operands_minmax,
	NULL,
	NULL,
	qb_select_opcode_basic,
	NULL,
	qb_transfer_operands_minmax,

	0,
	QB_RESULT_FROM_PURE_FUNCTION,
	QB_ADDRESS_TEMPORARY,
	{	QB_MAX_F64_F64_F64,	QB_MAX_F32_F32_F32,	QB_MAX_U64_U64_U64,	QB_MAX_S64_S64_S64,	QB_MAX_U32_U32_U32,	QB_MAX_S32_S32_S32,	QB_MAX_U16_U16_U16,	QB_MAX_S16_S16_S16,	QB_MAX_U08_U08_U08,		QB_MAX_S08_S08_S08,	},
	{	QB_AMAX_F64_F64,	QB_AMAX_F32_F32,	QB_AMAX_U64_U64,	QB_AMAX_S64_S64,	QB_AMAX_U32_U32,	QB_AMAX_S32_S32,	QB_AMAX_U16_U16,	QB_AMAX_S16_S16,	QB_AMAX_U08_U08,		QB_AMAX_S08_S08,	},
};

qb_basic_op_factory factory_rand = {
	qb_resolve_expression_type_highest_rank,
	qb_link_results_all_operands,
	qb_coerce_operands_all,
	qb_set_result_prototype,
	qb_validate_operands_rand,
	qb_set_result_temporary_value,
	qb_set_result_dimensions_larger_of_two,
	qb_select_opcode_basic,
	NULL,
	qb_transfer_operands_all,

	QB_COERCE_TO_LVALUE_TYPE,
	0,
	QB_ADDRESS_TEMPORARY,
	{	0 ,	0,	QB_RAND_U64_U64_U64,	QB_RAND_S64_S64_S64,	QB_RAND_U32_U32_U32,	QB_RAND_S32_S32_S32,	QB_RAND_U16_U16_U16,	QB_RAND_S16_S16_S16,	QB_RAND_U08_U08_U08,	QB_RAND_S08_S08_S08,	},
};

qb_basic_op_factory factory_mt_rand = {
	qb_resolve_expression_type_highest_rank,
	qb_link_results_all_operands,
	qb_coerce_operands_all,
	qb_set_result_prototype,
	qb_validate_operands_rand,
	qb_set_result_temporary_value,
	qb_set_result_dimensions_larger_of_two,
	qb_select_opcode_basic,
	NULL,
	qb_transfer_operands_all,

	QB_COERCE_TO_LVALUE_TYPE,
	0,
	QB_ADDRESS_TEMPORARY,
	{	0,	0,	QB_MT_RAND_U64_U64_U64,	QB_MT_RAND_S64_S64_S64,	QB_MT_RAND_U32_U32_U32,	QB_MT_RAND_S32_S32_S32,	QB_MT_RAND_U16_U16_U16,	QB_MT_RAND_S16_S16_S16,	QB_MT_RAND_U08_U08_U08,	QB_MT_RAND_S08_S08_S08,	},
};

qb_float_op_factory factory_lcg = {
	qb_resolve_expression_lvalue,
	NULL,
	NULL,
	qb_set_result_prototype,
	NULL,
	qb_set_result_temporary_value,
	NULL,
	qb_select_opcode_basic,
	NULL,
	qb_transfer_operands_result_only,

	QB_COERCE_TO_LVALUE_TYPE,
	0,
	QB_ADDRESS_TEMPORARY,
	{	QB_LCG_F64,				QB_LCG_F32,	},
};

qb_vector_op_factory factory_cross_product = {
	qb_resolve_expression_type_highest_rank,
	qb_link_results_all_operands,
	qb_coerce_operands_all,
	qb_set_result_prototype,
	qb_validate_operands_cross_product,
	qb_set_result_temporary_value,
	qb_set_result_dimensions_larger_of_two,
	qb_select_opcode_two_vectors,
	NULL,
	qb_transfer_operands_two_vectors,

	QB_COERCE_TO_LVALUE_TYPE | QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION,
	QB_ADDRESS_TEMPORARY,
	{	QB_NOP,		QB_NOP,	},
	{
		{	QB_CROSS_2X_F64_F64_F64,		QB_CROSS_2X_F32_F32_F32,		},
		{	QB_CROSS_3X_F64_F64_F64,		QB_CROSS_3X_F32_F32_F32,		},
		{	QB_CROSS_4X_F64_F64_F64_F64,	QB_CROSS_4X_F32_F32_F32_F32,	},
	},
};

qb_vector_op_factory factory_distance = {
	qb_resolve_expression_type_highest_rank,
	qb_link_results_all_operands,
	qb_coerce_operands_all,
	qb_set_result_prototype,
	qb_validate_operands_matching_vector_width,
	qb_set_result_temporary_value,
	qb_set_result_dimensions_dot_product,
	qb_select_opcode_one_vector,
	NULL,
	qb_transfer_operands_one_vector,

	QB_COERCE_TO_LVALUE_TYPE | QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION,
	QB_ADDRESS_TEMPORARY,
	{	QB_DIS_F64_F64_U32_F64,		QB_DIS_F32_F32_U32_F32,	},
	{
		{	QB_DIS_2X_F64_F64_F64,	QB_DIS_2X_F32_F32_F32,	},
		{	QB_DIS_3X_F64_F64_F64,	QB_DIS_3X_F32_F32_F32,	},
		{	QB_DIS_4X_F64_F64_F64,	QB_DIS_4X_F32_F32_F32,	},
	},
};

qb_vector_op_factory factory_dot_product = {
	qb_resolve_expression_type_highest_rank,
	qb_link_results_all_operands,
	qb_coerce_operands_all,
	qb_set_result_prototype,
	qb_validate_operands_matching_vector_width,
	qb_set_result_temporary_value,
	qb_set_result_dimensions_dot_product,
	qb_select_opcode_two_vectors,
	NULL,
	qb_transfer_operands_two_vectors,

	QB_COERCE_TO_LVALUE_TYPE | QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION,
	QB_ADDRESS_TEMPORARY,
	{	QB_DOT_F64_F64_U32_F64,		QB_DOT_F32_F32_U32_F32,	},
	{
		{	QB_DOT_2X_F64_F64_F64,	QB_DOT_2X_F32_F32_F32,	},
		{	QB_DOT_3X_F64_F64_F64,	QB_DOT_3X_F32_F32_F32,	},
		{	QB_DOT_4X_F64_F64_F64,	QB_DOT_4X_F32_F32_F32,	},
	},
};

qb_vector_op_factory factory_faceforward = {
	qb_resolve_expression_type_highest_rank,
	qb_link_results_all_operands,
	qb_coerce_operands_all,
	qb_set_result_prototype,
	qb_validate_operands_matching_vector_width,
	qb_set_result_temporary_value,
	qb_set_result_dimensions_larger_of_two,
	qb_select_opcode_two_vectors,
	NULL,
	qb_transfer_operands_two_vectors,

	QB_COERCE_TO_LVALUE_TYPE | QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION,
	QB_ADDRESS_TEMPORARY,
	{	QB_FORE_F64_F64_U32_F64,		QB_FORE_F32_F32_U32_F32,	},
	{
		{	QB_FORE_2X_F64_F64_F64,		QB_FORE_2X_F32_F32_F32,	},
		{	QB_FORE_3X_F64_F64_F64,		QB_FORE_3X_F32_F32_F32,	},
		{	QB_FORE_4X_F64_F64_F64,		QB_FORE_4X_F32_F32_F32,	},
	},
};

qb_vector_op_factory factory_length = {
	qb_resolve_expression_type_first_operand,
	qb_link_results_all_operands,
	qb_coerce_operands_all,
	qb_set_result_prototype,
	qb_validate_operands_one_array,
	qb_set_result_temporary_value,
	qb_set_result_dimensions_length,
	qb_select_opcode_one_vector,
	NULL,
	qb_transfer_operands_one_vector,

	QB_COERCE_TO_LVALUE_TYPE | QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION,
	QB_ADDRESS_TEMPORARY,
	{	QB_LEN_F64_U32_F64,			QB_LEN_F32_U32_F32,	},
	{
		{	QB_LEN_2X_F64_F64,		QB_LEN_2X_F32_F32,	},
		{	QB_LEN_3X_F64_F64,		QB_LEN_3X_F32_F32,	},
		{	QB_LEN_4X_F64_F64,		QB_LEN_4X_F32_F32,	},
	},
};

qb_vector_op_factory factory_normalize = {
	qb_resolve_expression_type_first_operand,
	qb_link_results_all_operands,
	qb_coerce_operands_all,
	qb_set_result_prototype,
	qb_validate_operands_one_array,
	qb_set_result_temporary_value,
	qb_set_result_dimensions_first_operand,
	qb_select_opcode_one_vector,
	NULL,
	qb_transfer_operands_one_vector,

	QB_COERCE_TO_LVALUE_TYPE | QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION,
	QB_ADDRESS_TEMPORARY,
	{	QB_NORM_F64_U32_F64,		QB_NORM_F32_U32_F32,	},
	{
		{	QB_NORM_2X_F64_F64,		QB_NORM_2X_F32_F32,	},
		{	QB_NORM_3X_F64_F64,		QB_NORM_3X_F32_F32,	},
		{	QB_NORM_4X_F64_F64,		QB_NORM_4X_F32_F32,	},
	},
};


qb_vector_op_factory factory_reflect = {
	qb_resolve_expression_type_highest_rank,
	qb_link_results_all_operands,
	qb_coerce_operands_all,
	qb_set_result_prototype,
	qb_validate_operands_matching_vector_width,
	qb_set_result_temporary_value,
	qb_set_result_dimensions_larger_of_two,
	qb_select_opcode_two_vectors,
	NULL,
	qb_transfer_operands_two_vectors,

	QB_COERCE_TO_LVALUE_TYPE | QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION,
	QB_ADDRESS_TEMPORARY,
	{	QB_REFL_F64_F64_U32_F64,		QB_REFL_F32_F32_U32_F32,	},
	{
		{	QB_REFL_2X_F64_F64_F64,		QB_REFL_2X_F32_F32_F32,	},
		{	QB_REFL_3X_F64_F64_F64,		QB_REFL_3X_F32_F32_F32,	},
		{	QB_REFL_4X_F64_F64_F64,		QB_REFL_4X_F32_F32_F32,	},
	},
};

qb_vector_op_factory factory_refract = {
	qb_resolve_expression_type_highest_rank,
	qb_link_results_all_operands,
	qb_coerce_operands_all,
	qb_set_result_prototype,
	qb_validate_operands_refract,
	qb_set_result_temporary_value,
	qb_set_result_dimensions_larger_of_two,
	qb_select_opcode_two_vectors,
	NULL,
	qb_transfer_operands_refract,

	QB_COERCE_TO_LVALUE_TYPE | QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION,
	QB_ADDRESS_TEMPORARY,
	{	QB_REFR_F64_F64_U32_U32_F64,		QB_REFR_F32_F32_U32_U32_F32,	},
	{
		{	QB_REFR_2X_F64_F64_F64_F64,		QB_REFR_2X_F32_F32_F32_F32,	},
		{	QB_REFR_3X_F64_F64_F64_F64,		QB_REFR_3X_F32_F32_F32_F32,	},
		{	QB_REFR_4X_F64_F64_F64_F64,		QB_REFR_4X_F32_F32_F32_F32,	},
	},
};

qb_matrix_op_factory factory_mm_mult_cm = {
	qb_resolve_expression_type_highest_rank,
	qb_link_results_all_operands,
	qb_coerce_operands_all,
	qb_set_result_prototype,
	qb_validate_operands_mm_mult,
	qb_set_result_temporary_value,
	qb_set_result_dimensions_mm_mult,
	qb_select_opcode_mm_mult_cm,
	NULL,
	qb_transfer_operands_mm_mult_cm,

	QB_COERCE_TO_LVALUE_TYPE | QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION | QB_RESULT_IS_COLUMN_MAJOR,
	QB_ADDRESS_TEMPORARY,
	{	QB_MUL_MM_CM_F64_F64_U32_U32_U32_F64,	QB_MUL_MM_CM_F32_F32_U32_U32_U32_F32,	},
	{
		{	QB_MUL_MM_CM_2X_F64_F64_F64,		QB_MUL_MM_CM_2X_F32_F32_F32,	},
		{	QB_MUL_MM_CM_3X_F64_F64_F64,		QB_MUL_MM_CM_3X_F32_F32_F32,	},
		{	QB_MUL_MM_CM_4X_F64_F64_F64,		QB_MUL_MM_CM_4X_F32_F32_F32,	},
	},
	QB_MUL_MM_CM_PAD_3X_F32_F32_F32,
};

qb_matrix_op_factory factory_mv_mult_cm = {
	qb_resolve_expression_type_highest_rank,
	qb_link_results_all_operands,
	qb_coerce_operands_all,
	qb_set_result_prototype,
	qb_validate_operands_mv_mult,
	qb_set_result_temporary_value,
	qb_set_result_dimensions_mv_mult,
	qb_select_opcode_mv_mult_cm,
	NULL,
	qb_transfer_operands_mv_mult_cm,

	QB_COERCE_TO_LVALUE_TYPE | QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION | QB_RESULT_IS_COLUMN_MAJOR,
	QB_ADDRESS_TEMPORARY,
	{	QB_MUL_MV_CM_F64_F64_U32_U32_F64,		QB_MUL_MV_CM_F32_F32_U32_U32_F32,	},
	{
		{	QB_MUL_MV_CM_2X_F64_F64_F64,		QB_MUL_MV_CM_2X_F32_F32_F32,	},
		{	QB_MUL_MV_CM_3X_F64_F64_F64,		QB_MUL_MV_CM_3X_F32_F32_F32,	},
		{	QB_MUL_MV_CM_4X_F64_F64_F64,		QB_MUL_MV_CM_4X_F32_F32_F32,	},
	},
	QB_MUL_MV_CM_PAD_3X_F32_F32_F32,
};

qb_matrix_op_factory factory_vm_mult_cm = {
	qb_resolve_expression_type_highest_rank,
	qb_link_results_all_operands,
	qb_coerce_operands_all,
	qb_set_result_prototype,
	qb_validate_operands_vm_mult,
	qb_set_result_temporary_value,
	qb_set_result_dimensions_vm_mult,
	qb_select_opcode_vm_mult_cm,
	NULL,
	qb_transfer_operands_vm_mult_cm,

	QB_COERCE_TO_LVALUE_TYPE | QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION | QB_RESULT_IS_COLUMN_MAJOR,
	QB_ADDRESS_TEMPORARY,
	{	QB_MUL_VM_CM_F64_F64_U32_U32_F64,		QB_MUL_VM_CM_F32_F32_U32_U32_F32,	},
	{
		{	QB_MUL_VM_CM_2X_F64_F64_F64,		QB_MUL_VM_CM_2X_F32_F32_F32,	},
		{	QB_MUL_VM_CM_3X_F64_F64_F64,		QB_MUL_VM_CM_3X_F32_F32_F32,	},
		{	QB_MUL_VM_CM_4X_F64_F64_F64,		QB_MUL_VM_CM_4X_F32_F32_F32,	},
	},
	QB_MUL_VM_CM_PAD_3X_F32_F32_F32,
};

qb_derived_op_factory factory_mm_mult_rm = {
	qb_resolve_expression_type_highest_rank,
	qb_link_results_all_operands,
	qb_coerce_operands_all,
	qb_set_result_prototype,
	qb_validate_operands_mm_mult,
	qb_set_result_temporary_value,
	qb_set_result_dimensions_transpose_equivalent,
	qb_select_opcode_transpose_equivalent,
	NULL,
	qb_transfer_operands_transpose_equivalent,

	QB_COERCE_TO_LVALUE_TYPE | QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION | QB_RESULT_IS_ROW_MAJOR,
	QB_ADDRESS_TEMPORARY,
	&factory_mm_mult_cm,
};

qb_derived_op_factory factory_mv_mult_rm = {
	qb_resolve_expression_type_highest_rank,
	qb_link_results_all_operands,
	qb_coerce_operands_all,
	qb_set_result_prototype,
	qb_validate_operands_mv_mult,
	qb_set_result_temporary_value,
	qb_set_result_dimensions_transpose_equivalent,
	qb_select_opcode_transpose_equivalent,
	NULL,
	qb_transfer_operands_transpose_equivalent,

	QB_COERCE_TO_LVALUE_TYPE | QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION | QB_RESULT_IS_ROW_MAJOR,
	QB_ADDRESS_TEMPORARY,
	&factory_vm_mult_cm,
};

qb_derived_op_factory factory_vm_mult_rm = {
	qb_resolve_expression_type_highest_rank,
	qb_link_results_all_operands,
	qb_coerce_operands_all,
	qb_set_result_prototype,
	qb_validate_operands_vm_mult,
	qb_set_result_temporary_value,
	qb_set_result_dimensions_transpose_equivalent,
	qb_select_opcode_transpose_equivalent,
	NULL,
	qb_transfer_operands_transpose_equivalent,

	QB_COERCE_TO_LVALUE_TYPE | QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION | QB_RESULT_IS_ROW_MAJOR,
	QB_ADDRESS_TEMPORARY,
	&factory_mv_mult_cm,
};

qb_matrix_op_factory_selector factory_mm_mult = { 
	qb_resolve_expression_type_highest_rank,
	qb_link_results_all_operands,
	qb_coerce_operands_all,
	qb_set_result_prototype,
	qb_validate_operands_mm_mult,
	qb_set_result_temporary_value,
	qb_set_result_dimensions_matrix_current_mode,
	qb_select_opcode_matrix_current_mode,
	NULL,
	qb_transfer_operands_matrix_current_mode,

	QB_COERCE_TO_LVALUE_TYPE | QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION,
	QB_ADDRESS_TEMPORARY,
	(qb_op_factory *) &factory_mm_mult_cm,
	(qb_op_factory *) &factory_mm_mult_rm,
};

qb_matrix_op_factory_selector factory_mv_mult = { 
	qb_resolve_expression_type_highest_rank,
	qb_link_results_all_operands,
	qb_coerce_operands_all,
	qb_set_result_prototype,
	qb_validate_operands_mv_mult,
	qb_set_result_temporary_value,
	qb_set_result_dimensions_matrix_current_mode,
	qb_select_opcode_matrix_current_mode,
	NULL,
	qb_transfer_operands_matrix_current_mode,

	QB_COERCE_TO_LVALUE_TYPE | QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION,
	QB_ADDRESS_TEMPORARY,
	(qb_op_factory *) &factory_mv_mult_cm,
	(qb_op_factory *) &factory_mv_mult_rm,
};

qb_matrix_op_factory_selector factory_vm_mult = { 
	qb_resolve_expression_type_highest_rank,
	qb_link_results_all_operands,
	qb_coerce_operands_all,
	qb_set_result_prototype,
	qb_validate_operands_matrix_current_mode,
	qb_set_result_temporary_value,
	qb_set_result_dimensions_matrix_current_mode,
	qb_select_opcode_matrix_current_mode,
	NULL,
	qb_transfer_operands_matrix_current_mode,

	QB_COERCE_TO_LVALUE_TYPE | QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION,
	QB_ADDRESS_TEMPORARY,
	(qb_op_factory *) &factory_vm_mult_cm,
	(qb_op_factory *) &factory_vm_mult_rm,
};

qb_matrix_op_factory factory_transpose = {
	qb_resolve_expression_type_first_operand,
	qb_link_results_all_operands,
	qb_coerce_operands_all,
	qb_set_result_prototype,
	qb_validate_operands_one_matrix,
	qb_set_result_temporary_value,
	qb_set_result_dimensions_transpose,
	qb_select_opcode_matrix_unary,
	NULL,
	qb_transfer_operands_tranpose,

	QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION,
	QB_ADDRESS_TEMPORARY,
	{	QB_MTRAN_F64_U32_U32_F64,		QB_MTRAN_F32_U32_U32_F32,	},
	{
		{	QB_MTRAN_2X_F64_F64,		QB_MTRAN_2X_F32_F32,	},
		{	QB_MTRAN_3X_F64_F64,		QB_MTRAN_3X_F32_F32,	},
		{	QB_MTRAN_4X_F64_F64,		QB_MTRAN_4X_F32_F32,	},
	},
	0,
};

qb_matrix_op_factory factory_inverse = {
	qb_resolve_expression_type_first_operand,
	qb_link_results_all_operands,
	qb_coerce_operands_all,
	qb_set_result_prototype,
	qb_validate_operands_square_matrix,
	qb_set_result_temporary_value,
	qb_set_result_dimensions_first_operand,
	qb_select_opcode_matrix_unary,
	NULL,
	qb_transfer_operands_square_matrix,

	QB_COERCE_TO_LVALUE_TYPE | QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION,
	QB_ADDRESS_TEMPORARY,
	{	QB_MINV_F64_U32_F64,		QB_MINV_F32_U32_F32,	},
	{
		{	QB_MINV_2X_F64_F64,		QB_MINV_2X_F32_F32,	},
		{	QB_MINV_3X_F64_F64,		QB_MINV_3X_F32_F32,	},
		{	QB_MINV_4X_F64_F64,		QB_MINV_4X_F32_F32,	},
	},
	0,
};

qb_matrix_op_factory factory_determinant = {
	qb_resolve_expression_type_first_operand,
	qb_link_results_all_operands,
	qb_coerce_operands_all,
	qb_set_result_prototype,
	qb_validate_operands_square_matrix,
	qb_set_result_temporary_value,
	qb_set_result_dimensions_determinant,
	qb_select_opcode_matrix_unary,
	NULL,
	qb_transfer_operands_square_matrix,

	QB_COERCE_TO_LVALUE_TYPE | QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION,
	QB_ADDRESS_TEMPORARY,
	{	QB_MDET_F64_U32_F64,			QB_MDET_F32_U32_F32,	},
	{
		{	QB_MDET_2X_F64_F64,		QB_MDET_2X_F32_F32,	},
		{	QB_MDET_3X_F64_F64,		QB_MDET_3X_F32_F32,	},
		{	QB_MDET_4X_F64_F64,		QB_MDET_4X_F32_F32,	},
	},
	0,
};

/*
qb_matrix_op_factory factory_transform_cm = {
	qb_resolve_expression_type,
	qb_link_results_all_operands,
	qb_coerce_operands,
	qb_set_result_prototype,
	qb_validate_operands,
	qb_set_result,
	qb_set_result_dimensions_mv_mult,
	qb_select_opcode_transform_cm,
	NULL,
	qb_transfer_operands,

	QB_COERCE_TO_LVALUE_TYPE | QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION | QB_RESULT_IS_COLUMN_MAJOR,
	QB_ADDRESS_TEMPORARY,
	{	QB_NOP,			QB_NOP,	},
	{
		{	QB_TRAN_CM_2X_F64_F64_F64,		QB_TRAN_CM_2X_F32_F32_F32,	},
		{	QB_TRAN_CM_3X_F64_F64_F64,		QB_TRAN_CM_3X_F32_F32_F32,	},
		{	QB_TRAN_CM_4X_F64_F64_F64,		QB_TRAN_CM_4X_F32_F32_F32,	},
	},
	0,
};
*/

/*
qb_matrix_op_factory factory_transform_rm = {
	qb_resolve_expression_type,
	qb_link_results_all_operands,
	qb_coerce_operands,
	qb_set_result_prototype,
	qb_validate_operands,
	qb_set_result,
	qb_set_result_dimensions_mv_mult,
	qb_select_opcode_transform_rm,
	NULL,
	qb_transfer_operands,

	QB_COERCE_TO_LVALUE_TYPE | QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION | QB_RESULT_IS_COLUMN_MAJOR,
	QB_ADDRESS_TEMPORARY,
	{	0,			0,	},
	{
		{	QB_TRAN_RM_2X_F64_F64_F64,		QB_TRAN_RM_2X_F32_F32_F32,	},
		{	QB_TRAN_RM_3X_F64_F64_F64,		QB_TRAN_RM_3X_F32_F32_F32,	},
		{	QB_TRAN_RM_4X_F64_F64_F64,		QB_TRAN_RM_4X_F32_F32_F32,	},
	},
	0,
};
*/

/*
qb_matrix_op_factory_selector factory_transform = {
	qb_resolve_expression_type,
	qb_link_results_all_operands,
	qb_coerce_operands_all,
	qb_validate_operands_matrix_current_mode,
	qb_set_result_temporary_value,
	qb_set_result_dimensions_matrix_current_mode,
	qb_select_opcode_matrix_current_mode,
	NULL,
	qb_transfer_operands_matrix_current_mode,

	QB_COERCE_TO_LVALUE_TYPE | QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION,
	QB_ADDRESS_TEMPORARY,
	(qb_op_factory *) &factory_transform_cm,
	(qb_op_factory *) &factory_transform_rm,
};
*/

/*
qb_op * qb_append_utf8_decode(qb_compiler_context *cxt, void *factory, qb_operand *operands, uint32_t operand_count, qb_operand *result) {
	qb_basic_op_factory *f = factory;
	qb_address *address = result->address;
	qb_opcode opcode = f->opcodes[QB_TYPE_F64 - address->type];
	qb_op *qop = qb_append_op(cxt, opcode, 2);
	qop->operands[0] = operands[0];
	qop->operands[0].type = QB_OPERAND_ADDRESS_ARR;
	qop->operands[1] = *result;
	qop->operands[1].type = QB_OPERAND_ADDRESS_ARR;
	return qop;
}
*/

/*
qb_basic_op_factory factory_utf8_decode = {
	qb_append_utf8_decode,
	NULL,
	0,
	qb_set_result_prototype,
	0,
	QB_ADDRESS_TEMPORARY,
	{	0, 0, 0, 0, QB_UTF8_DEC_U08_U32,	QB_UTF8_DEC_U08_U32,	QB_UTF8_DEC_U08_U16,	QB_UTF8_DEC_U08_U16,	},
};
*/

/*
qb_basic_op_factory factory_utf8_encode = {
	qb_append_utf8_encode,
	NULL,
	0,
	qb_set_result_prototype,
	0,
	QB_ADDRESS_TEMPORARY,
	{	0, 0, 0, 0, QB_UTF8_ENC_U32_U08,	QB_UTF8_ENC_U32_U08,	QB_UTF8_ENC_U16_U08,	QB_UTF8_ENC_U16_U08,	},
};
*/

/*
qb_basic_op_factory factory_pack_le = {
	qb_resolve_expression_type,
	qb_link_results_all_operands,
	qb_coerce_operands,
	qb_set_result_prototype,
	qb_validate_operands,
	qb_set_result,
	NULL,
	qb_select_opcode_basic,
	NULL,
	qb_transfer_operands,

	0,
	QB_RESULT_FROM_PURE_FUNCTION,
	{	QB_PACK_LE_F64_U08,	QB_PACK_LE_F32_U08,	QB_PACK_LE_I64_U08,	QB_PACK_LE_I64_U08,	QB_PACK_LE_I32_U08,	QB_PACK_LE_I32_U08,	QB_PACK_LE_I16_U08,	QB_PACK_LE_I16_U08,	},
};

qb_basic_op_factory factory_pack_be = {
	qb_resolve_expression_type,
	qb_link_results_all_operands,
	qb_coerce_operands,
	qb_set_result_prototype,
	qb_validate_operands,
	qb_set_result,
	NULL,
	qb_select_opcode_basic,
	NULL,
	qb_transfer_operands,

	0,
	QB_RESULT_FROM_PURE_FUNCTION,
	{	QB_PACK_BE_F64_U08 ,	QB_PACK_BE_F32_U08,	QB_PACK_BE_I64_U08,	QB_PACK_BE_I64_U08,	QB_PACK_BE_I32_U08,	QB_PACK_BE_I32_U08,	QB_PACK_BE_I16_U08,	QB_PACK_BE_I16_U08,	},
};
*/

/*
qb_basic_op_factory factory_unpack_le = {
	qb_resolve_expression_type,
	qb_link_results_all_operands,
	qb_coerce_operands,
	qb_set_result_prototype,
	qb_validate_operands,
	qb_set_result,
	NULL,
	qb_select_opcode_basic_result,
	NULL,
	qb_transfer_operands,

	0,
	QB_RESULT_FROM_PURE_FUNCTION,
	{	QB_UNPACK_LE_U08_F64,	QB_UNPACK_LE_U08_F32,	QB_UNPACK_LE_U08_I64,	QB_UNPACK_LE_U08_I64,	QB_UNPACK_LE_U08_I32,	QB_UNPACK_LE_U08_I32,	QB_UNPACK_LE_U08_I16,	QB_UNPACK_LE_U08_I16	},
};

qb_basic_op_factory factory_unpack_be = {
	qb_resolve_expression_type,
	qb_link_results_all_operands,
	qb_coerce_operands,
	qb_set_result_prototype,
	qb_validate_operands,
	qb_set_result,
	NULL,
	qb_select_opcode_basic_result,
	NULL,
	qb_transfer_operands,

	0,
	QB_RESULT_FROM_PURE_FUNCTION,
	{	QB_UNPACK_BE_U08_F64,	QB_UNPACK_BE_U08_F32,	QB_UNPACK_BE_U08_I64,	QB_UNPACK_BE_U08_I64,	QB_UNPACK_BE_U08_I32,	QB_UNPACK_BE_U08_I32,	QB_UNPACK_BE_U08_I16,	QB_UNPACK_BE_U08_I16	},
};
*/

qb_basic_op_factory factory_array_column = {
	qb_resolve_expression_type_first_operand,
	NULL,
	qb_coerce_operands_array_column,
	qb_set_result_prototype,
	qb_validate_operands_multidimensional_array,
	qb_set_result_temporary_value,
	qb_set_result_dimensions_array_column,
	qb_select_opcode_basic,
	NULL,
	qb_transfer_operands_array_column,

	0,
	0,
	QB_ADDRESS_TEMPORARY,
	{	QB_ACOL_F64_U32_U32_U32_F64,	QB_ACOL_F32_U32_U32_U32_F32,	QB_ACOL_I64_U32_U32_U32_I64,	QB_ACOL_I64_U32_U32_U32_I64,	QB_ACOL_I32_U32_U32_U32_I32,	QB_ACOL_I32_U32_U32_U32_I32,	QB_ACOL_I16_U32_U32_U32_I16,	QB_ACOL_I16_U32_U32_U32_I16,	QB_ACOL_I08_U32_U32_U32_I08,	QB_ACOL_I08_U32_U32_U32_I08	},
};

qb_basic_op_factory factory_array_sum = {
	qb_resolve_expression_type_first_operand,
	qb_link_results_all_operands,
	qb_coerce_operands_all,
	qb_set_result_prototype,
	qb_validate_operands_one_array,
	qb_set_result_temporary_value,
	NULL,
	qb_select_opcode_basic,
	NULL,
	qb_transfer_operands_all,

	0,
	QB_RESULT_FROM_PURE_FUNCTION,
	QB_ADDRESS_TEMPORARY,
	{	QB_ASUM_F64_F64,	QB_ASUM_F32_F32,	QB_ASUM_U64_U64,	QB_ASUM_S64_S64,	QB_ASUM_U32_U32,	QB_ASUM_S32_S32,	QB_ASUM_U16_U16,	QB_ASUM_S16_S16,	QB_ASUM_U08_U08,	QB_ASUM_S08_S08	},
};

qb_basic_op_factory factory_array_product = {
	qb_resolve_expression_type_first_operand,
	qb_link_results_all_operands,
	qb_coerce_operands_all,
	qb_set_result_prototype,
	qb_validate_operands_one_array,
	qb_set_result_temporary_value,
	NULL,
	qb_select_opcode_basic,
	NULL,
	qb_transfer_operands_all,

	0,
	QB_RESULT_FROM_PURE_FUNCTION,
	QB_ADDRESS_TEMPORARY,
	{	QB_APROD_F64_F64,	QB_APROD_F32_F32,	QB_APROD_U64_U64,	QB_APROD_S64_S64,	QB_APROD_U32_U32,	QB_APROD_S32_S32,	QB_APROD_U16_U16,	QB_APROD_S16_S16,	QB_APROD_U08_U08,	QB_APROD_S08_S08	},
};

qb_basic_op_factory factory_rsort = {
	qb_resolve_expression_type_boolean,
	NULL,
	NULL,
	qb_set_result_true,
	qb_validate_operands_one_array_variable,
	qb_set_result_true,
	NULL,
	qb_select_opcode_basic,
	NULL,
	qb_transfer_operands_sort,

	0,
	QB_RESULT_HAS_SIDE_EFFECT,
	QB_ADDRESS_BOOLEAN,
	{	QB_RSORT_U32_F64,	QB_RSORT_U32_F32,	QB_RSORT_U32_U64,	QB_RSORT_U32_S64,	QB_RSORT_U32_U32,	QB_RSORT_U32_S32,	QB_RSORT_U32_U16,	QB_RSORT_U32_S16,	QB_RSORT_U32_U08,	QB_RSORT_U32_S08	},
};

qb_basic_op_factory factory_shuffle = {
	qb_resolve_expression_type_boolean,
	NULL,
	NULL,
	qb_set_result_true,
	qb_validate_operands_one_array_variable,
	qb_set_result_true,
	NULL,
	qb_select_opcode_basic,
	NULL,
	qb_transfer_operands_sort,

	0,
	QB_RESULT_HAS_SIDE_EFFECT,
	QB_ADDRESS_BOOLEAN,
	{	QB_SHUFFLE_U32_F64,	QB_SHUFFLE_U32_F32,	QB_SHUFFLE_U32_I64,	QB_SHUFFLE_U32_I64,	QB_SHUFFLE_U32_I32,	QB_SHUFFLE_U32_I32,	QB_SHUFFLE_U32_I16,	QB_SHUFFLE_U32_I16,	QB_SHUFFLE_U32_I08,	QB_SHUFFLE_U32_I08	},
};

qb_basic_op_factory factory_sort = {
	qb_resolve_expression_type_first_operand,
	qb_link_results_all_operands,
	NULL,
	qb_set_result_true,
	qb_validate_operands_one_array_variable,
	qb_set_result_true,
	NULL,
	qb_select_opcode_basic,
	NULL,
	qb_transfer_operands_sort,

	0,
	QB_RESULT_HAS_SIDE_EFFECT,
	QB_ADDRESS_BOOLEAN,
	{	QB_SORT_U32_F64 ,	QB_SORT_U32_F32,	QB_SORT_U32_U64,	QB_SORT_U32_S64,	QB_SORT_U32_U32,	QB_SORT_U32_S32,	QB_SORT_U32_U16,	QB_SORT_U32_S16,	QB_SORT_U32_U08,	QB_SORT_U32_S08	},
};

/*
qb_basic_op_factory factory_array_insert = {
	qb_resolve_expression_type,
	qb_link_results_all_operands,
	qb_coerce_operands,
	qb_set_result_prototype,
	qb_validate_operands,
	qb_set_result,
	qb_set_result_dimensions_array_pad,
	qb_select_opcode_basic,
	NULL,
	qb_transfer_operands,
	0,
	0,
	QB_ADDRESS_TEMPORARY,
	{	QB_AINS_F64_U32_F64,	QB_AINS_F32_U32_F32,	QB_AINS_I64_U32_I64,	QB_AINS_I64_U32_I64,	QB_AINS_I32_U32_I32,	QB_AINS_I32_U32_I32,	QB_AINS_I16_U32_I16,	QB_AINS_I16_U32_I16,	QB_AINS_I08_U32_I08,	QB_AINS_I08_U32_I08,	},
};
*/

/*
qb_basic_op_factory factory_array_pad = {
	qb_resolve_expression_type,
	qb_link_results_all_operands,
	qb_coerce_operands,
	qb_set_result_prototype,
	qb_validate_operands,
	qb_set_result,
	qb_set_result_dimensions_array_pad,
	qb_select_opcode_basic,
	NULL,
	qb_transfer_operands,

	0,
	0,
	QB_ADDRESS_TEMPORARY,
	{	QB_APAD_F64_F64_S32_F64,	QB_APAD_F32_F32_S32_F32,	QB_APAD_I64_I64_S32_I64,	QB_APAD_I64_I64_S32_I64,	QB_APAD_I32_I32_S32_I32,	QB_APAD_I32_I32_S32_I32,	QB_APAD_I16_I16_S32_I16,	QB_APAD_I16_I16_S32_I16,	QB_APAD_I08_I08_S32_I08,	QB_APAD_I08_I08_S32_I08,	},
};
*/

/*
qb_basic_op_factory factory_array_search = {
	qb_resolve_expression_type,
	qb_link_results_all_operands,
	qb_coerce_operands,
	qb_set_result_prototype,
	qb_validate_operands,
	qb_set_result,
	NULL,
	qb_select_opcode_basic,
	NULL,
	qb_transfer_operands,

	QB_COERCE_TO_SECOND_OPERAND_TYPE,
	QB_TYPE_I32,
	{	QB_AFIND_IDX_F64_F64_I32,	QB_AFIND_IDX_F32_F32_I32,	QB_AFIND_IDX_I64_I64_I32,	QB_AFIND_IDX_I64_I64_I32,	QB_AFIND_IDX_I32_I32_I32,	QB_AFIND_IDX_I32_I32_I32,	QB_AFIND_IDX_I16_I16_I32,	QB_AFIND_IDX_I16_I16_I32,	QB_AFIND_IDX_I08_I08_I32,	QB_AFIND_IDX_I08_I08_I32	},
};
*/

/*
qb_copy_op_factory factory_array_merge = {
	qb_resolve_expression_type,
	qb_link_results_all_operands,
	qb_coerce_operands,
	qb_set_result_prototype,
	qb_validate_operands,
	qb_set_result,
	qb_set_result_dimensions_array_merge,
	qb_select_copy_opcode,
	NULL,
	qb_transfer_operands,

	QB_COERCE_TO_HIGHEST_RANK,
	QB_TYPE_OPERAND,
};
*/

/*
qb_copy_op_factory factory_array_fill = {
	qb_resolve_expression_type,
	qb_link_results_all_operands,
	qb_coerce_operands,
	qb_set_result_prototype,
	qb_validate_operands,
	qb_set_result,
	qb_set_result_dimensions_array_fill,
	qb_select_copy_opcode,
	NULL,
	qb_transfer_operands,

	0,
	QB_TYPE_OPERAND,
};

*/

/*
qb_basic_op_factory factory_subarray_pos = {
	qb_resolve_expression_type,
	qb_link_results_all_operands,
	qb_coerce_operands,
	qb_set_result_prototype,
	qb_validate_operands,
	qb_set_result,
	NULL,
	qb_select_opcode_basic,
	NULL,
	qb_transfer_operands,

	QB_COERCE_TO_FIRST_OPERAND_TYPE,
	QB_RESULT_FROM_PURE_FUNCTION | QB_TYPE_I32,
	QB_ADDRESS_TEMPORARY,
	{	QB_APOS_F64_F64_I32_I32,	QB_APOS_F32_F32_I32_I32,	QB_APOS_I64_I64_I32_I32,	QB_APOS_I64_I64_I32_I32,	QB_APOS_I32_I32_I32_I32,	QB_APOS_I32_I32_I32_I32,	QB_APOS_I16_I16_I32_I32,	QB_APOS_I16_I16_I32_I32,	QB_APOS_I08_I08_I32_I32,	QB_APOS_I08_I08_I32_I32	},
};
*/

/*
qb_basic_op_factory factory_subarray_rpos = {
	qb_resolve_expression_type,
	qb_link_results_all_operands,
	qb_coerce_operands,
	qb_set_result_prototype,
	qb_validate_operands,
	qb_set_result,
	NULL,
	qb_select_opcode_basic,
	NULL,
	qb_transfer_operands,

	QB_COERCE_TO_FIRST_OPERAND_TYPE,
	QB_RESULT_FROM_PURE_FUNCTION | QB_TYPE_I32,
	QB_ADDRESS_TEMPORARY,
	{	QB_ARPOS_F64_F64_I32_I32,	QB_ARPOS_F32_F32_I32_I32,	QB_ARPOS_I64_I64_I32_I32,	QB_ARPOS_I64_I64_I32_I32,	QB_ARPOS_I32_I32_I32_I32,	QB_ARPOS_I32_I32_I32_I32,	QB_ARPOS_I16_I16_I32_I32,	QB_ARPOS_I16_I16_I32_I32,	QB_ARPOS_I08_I08_I32_I32,	QB_ARPOS_I08_I08_I32_I32	},
};
*/

/*
qb_basic_op_factory factory_array_reverse = {
	qb_resolve_expression_type,
	qb_link_results_all_operands,
	qb_coerce_operands,
	qb_set_result_prototype,
	qb_validate_operands,
	qb_set_result_temporary_value,
	qb_set_result_dimensions_first_operand,
	qb_select_opcode_basic,
	NULL,
	qb_transfer_operands,

	QB_COERCE_TO_FIRST_OPERAND_TYPE,
	QB_RESULT_FROM_PURE_FUNCTION,
	QB_ADDRESS_TEMPORARY,
	{	QB_AREV_F64_U32_F64,	QB_AREV_F32_U32_F32,	QB_AREV_I64_U32_I64,	QB_AREV_I64_U32_I64,	QB_AREV_I32_U32_I32,	QB_AREV_I32_U32_I32,	QB_AREV_I16_U32_I16,	QB_AREV_I16_U32_I16,	QB_AREV_I08_U32_I08,	QB_AREV_I08_U32_I08		},
};
*/

/*
qb_basic_op_factory factory_array_diff = {
	qb_resolve_expression_type,
	qb_link_results_all_operands,
	qb_coerce_operands,
	qb_set_result_prototype,
	qb_validate_operands,
	qb_set_result,
	qb_set_result_dimensions_array_pad,
	qb_select_opcode_basic,
	NULL,
	qb_transfer_operands,

	QB_COERCE_TO_FIRST_OPERAND_TYPE,
	QB_TYPE_OPERAND,
	{	QB_ADIFF_F64_F64_U32_F64,	QB_ADIFF_F32_F32_U32_F32,	QB_ADIFF_I64_I64_U32_I64,	QB_ADIFF_I64_I64_U32_I64,	QB_ADIFF_I32_I32_U32_I32,	QB_ADIFF_I32_I32_U32_I32,	QB_ADIFF_I16_I16_U32_I16,	QB_ADIFF_I16_I16_U32_I16,	QB_ADIFF_I08_I08_U32_I08,	QB_ADIFF_I08_I08_U32_I08	},
};
*/

/*
qb_basic_op_factory factory_array_intersect = {
	qb_resolve_expression_type,
	qb_link_results_all_operands,
	qb_coerce_operands,
	qb_set_result_prototype,
	qb_validate_operands,
	qb_set_result,
	qb_set_result_dimensions_array_pad,
	qb_select_opcode_basic,
	NULL,
	qb_transfer_operands,

	QB_COERCE_TO_FIRST_OPERAND_TYPE,
	QB_TYPE_OPERAND,
	{	QB_AISECT_F64_F64_U32_F64,	QB_AISECT_F32_F32_U32_F32,	QB_AISECT_I64_I64_U32_I64,	QB_AISECT_I64_I64_U32_I64,	QB_AISECT_I32_I32_U32_I32,	QB_AISECT_I32_I32_U32_I32,	QB_AISECT_I16_I16_U32_I16,	QB_AISECT_I16_I16_U32_I16,	QB_AISECT_I08_I08_U32_I08,	QB_AISECT_I08_I08_U32_I08	},
};
*/

/*
qb_basic_op_factory factory_range = {
	qb_resolve_expression_type,
	qb_link_results_all_operands,
	qb_coerce_operands,
	qb_set_result_prototype,
	qb_validate_operands,
	qb_set_result,
	qb_set_result_dimensions_range,
	qb_select_opcode_basic,
	NULL,
	qb_transfer_operands,

	QB_COERCE_TO_LVALUE_TYPE,
	QB_RESULT_FROM_PURE_FUNCTION,
	QB_ADDRESS_TEMPORARY,
	{	QB_RANGE_F64_F64_F64_F64,	QB_RANGE_F32_F32_F32_F32,	QB_RANGE_U64_U64_S64_U64,	QB_RANGE_S64_S64_S64_S64,	QB_RANGE_U32_U32_S32_U32,	QB_RANGE_S32_S32_S32_S32,	QB_RANGE_U16_U16_S16_U16,	QB_RANGE_S16_S16_S16_S16,	QB_RANGE_U08_U08_S08_U08,	QB_RANGE_S08_S08_S08_S08	},
};
*/

/*
qb_basic_op_factory factory_array_unique = {
	qb_resolve_expression_type,
	qb_link_results_all_operands,
	qb_coerce_operands,
	qb_set_result_prototype,
	qb_validate_operands,
	qb_set_result,
	qb_set_result_dimensions_array_pad,
	qb_select_opcode_basic,
	NULL,
	qb_transfer_operands,

	QB_COERCE_TO_FIRST_OPERAND_TYPE,
	QB_RESULT_FROM_PURE_FUNCTION,
	QB_ADDRESS_TEMPORARY,
	{	QB_AUNIQ_F64_U32_F64,	QB_AUNIQ_F32_U32_F32,	QB_AUNIQ_I64_U32_I64,	QB_AUNIQ_I64_U32_I64,	QB_AUNIQ_I32_U32_I32,	QB_AUNIQ_I32_U32_I32,	QB_AUNIQ_I16_U32_I16,	QB_AUNIQ_I16_U32_I16,	QB_AUNIQ_I08_U32_I08,	QB_AUNIQ_I08_U32_I08	},
};
*/

/*
qb_simple_op_factory factory_array_rand = {
	qb_resolve_expression_type,
	qb_link_results_all_operands,
	qb_coerce_operands,
	qb_set_result_prototype,
	qb_validate_operands,
	qb_set_result,
	qb_set_result_dimensions_array_rand,
	qb_select_opcode_simple,
	NULL,
	qb_transfer_operands,

	0,
	QB_RESULT_FROM_PURE_FUNCTION,
	QB_ARAND_U32_U32_U32,
};

/*
qb_basic_op_factory factory_array_resize = {
	qb_append_array_resize,
	NULL,
	0,
	qb_set_result_prototype,
	0,
	QB_ADDRESS_TEMPORARY,
	{	QB_ARESIZE_F64,	QB_ARESIZE_F32,	QB_ARESIZE_I64,	QB_ARESIZE_I64,	QB_ARESIZE_I32,	QB_ARESIZE_I32,	QB_ARESIZE_I16,	QB_ARESIZE_I16,	QB_ARESIZE_I08,	QB_ARESIZE_I08	},
};
*/

qb_float_op_factory factory_complex_abs = {
	qb_resolve_expression_type_first_operand,
	qb_link_results_all_operands,
	qb_coerce_operands_all,
	qb_set_result_prototype,
	NULL,
	qb_set_result_temporary_value,
	qb_set_result_dimensions_length,
	qb_select_opcode_complex_number,
	NULL,
	qb_transfer_operands_all,

	QB_COERCE_TO_LVALUE_TYPE | QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION,
	QB_ADDRESS_TEMPORARY,
	{	QB_CABS_F64_F64,	QB_CABS_F32_F32	},
};

qb_float_op_factory factory_complex_arg = {
	qb_resolve_expression_type_first_operand,
	qb_link_results_all_operands,
	qb_coerce_operands_all,
	qb_set_result_prototype,
	NULL,
	qb_set_result_temporary_value,
	qb_set_result_dimensions_length,
	qb_select_opcode_complex_number,
	NULL,
	qb_transfer_operands_all,

	QB_COERCE_TO_LVALUE_TYPE | QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION,
	QB_ADDRESS_TEMPORARY,
	{	QB_CARG_F64_F64,	QB_CARG_F32_F32	},
};

qb_float_op_factory factory_complex_multiply = {
	qb_resolve_expression_type_highest_rank,
	qb_link_results_all_operands,
	qb_coerce_operands_all,
	qb_set_result_prototype,
	NULL,
	qb_set_result_temporary_value,
	qb_set_result_dimensions_larger_of_two,
	qb_select_opcode_complex_number,
	NULL,
	qb_transfer_operands_all,

	QB_COERCE_TO_LVALUE_TYPE | QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION,
	QB_ADDRESS_TEMPORARY,
	{	QB_CMUL_F64_F64_F64,	QB_CMUL_F32_F32_F32	},
};

qb_float_op_factory factory_complex_divide = {
	qb_resolve_expression_type_highest_rank,
	qb_link_results_all_operands,
	qb_coerce_operands_all,
	qb_set_result_prototype,
	NULL,
	qb_set_result_temporary_value,
	qb_set_result_dimensions_larger_of_two,
	qb_select_opcode_complex_number,
	NULL,
	qb_transfer_operands_all,

	QB_COERCE_TO_LVALUE_TYPE | QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION,
	QB_ADDRESS_TEMPORARY,
	{	QB_CDIV_F64_F64_F64,	QB_CDIV_F32_F32_F32	},
};

qb_float_op_factory factory_complex_exp = {
	qb_resolve_expression_type_first_operand,
	qb_link_results_all_operands,
	qb_coerce_operands_all,
	qb_set_result_prototype,
	NULL,
	qb_set_result_temporary_value,
	qb_set_result_dimensions_first_operand,
	qb_select_opcode_complex_number,
	NULL,
	qb_transfer_operands_all,

	QB_COERCE_TO_LVALUE_TYPE | QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION,
	QB_ADDRESS_TEMPORARY,
	{	QB_CEXP_F64_F64,	QB_CEXP_F32_F32	},
};

qb_float_op_factory factory_complex_log = {
	qb_resolve_expression_type_first_operand,
	qb_link_results_all_operands,
	qb_coerce_operands_all,
	qb_set_result_prototype,
	NULL,
	qb_set_result_temporary_value,
	qb_set_result_dimensions_first_operand,
	qb_select_opcode_complex_number,
	NULL,
	qb_transfer_operands_all,

	QB_COERCE_TO_LVALUE_TYPE | QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION,
	QB_ADDRESS_TEMPORARY,
	{	QB_CLOG_F64_F64,	QB_CLOG_F32_F32	},
};

qb_float_op_factory factory_complex_sqrt = {
	qb_resolve_expression_type_first_operand,
	qb_link_results_all_operands,
	qb_coerce_operands_all,
	qb_set_result_prototype,
	NULL,
	qb_set_result_temporary_value,
	qb_set_result_dimensions_first_operand,
	qb_select_opcode_complex_number,
	NULL,
	qb_transfer_operands_all,

	QB_COERCE_TO_LVALUE_TYPE | QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION,
	QB_ADDRESS_TEMPORARY,
	{	QB_CSQRT_F64_F64,	QB_CSQRT_F32_F32	},
};

qb_float_op_factory factory_complex_pow = {
	qb_resolve_expression_type_highest_rank,
	qb_link_results_all_operands,
	qb_coerce_operands_all,
	qb_set_result_prototype,
	NULL,
	qb_set_result_temporary_value,
	qb_set_result_dimensions_larger_of_two,
	qb_select_opcode_complex_number,
	NULL,
	qb_transfer_operands_all,

	QB_COERCE_TO_LVALUE_TYPE | QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION,
	QB_ADDRESS_TEMPORARY,
	{	QB_CPOW_F64_F64_F64,	QB_CPOW_F32_F32_F32	},
};

qb_float_op_factory factory_complex_sin = {
	qb_resolve_expression_type_first_operand,
	qb_link_results_all_operands,
	qb_coerce_operands_all,
	qb_set_result_prototype,
	NULL,
	qb_set_result_temporary_value,
	qb_set_result_dimensions_first_operand,
	qb_select_opcode_complex_number,
	NULL,
	qb_transfer_operands_all,

	QB_COERCE_TO_LVALUE_TYPE | QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION,
	QB_ADDRESS_TEMPORARY,
	{	QB_CSIN_F64_F64,	QB_CSIN_F32_F32	},
};

qb_float_op_factory factory_complex_cos = {
	qb_resolve_expression_type_first_operand,
	qb_link_results_all_operands,
	qb_coerce_operands_all,
	qb_set_result_prototype,
	NULL,
	qb_set_result_temporary_value,
	qb_set_result_dimensions_first_operand,
	qb_select_opcode_complex_number,
	NULL,
	qb_transfer_operands_all,

	QB_COERCE_TO_LVALUE_TYPE | QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION,
	QB_ADDRESS_TEMPORARY,
	{	QB_CCOS_F64_F64,	QB_CCOS_F32_F32	},
};

qb_float_op_factory factory_complex_tan = {
	qb_resolve_expression_type_first_operand,
	qb_link_results_all_operands,
	qb_coerce_operands_all,
	qb_set_result_prototype,
	NULL,
	qb_set_result_temporary_value,
	qb_set_result_dimensions_first_operand,
	qb_select_opcode_complex_number,
	NULL,
	qb_transfer_operands_all,

	QB_COERCE_TO_LVALUE_TYPE | QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION,
	QB_ADDRESS_TEMPORARY,
	{	QB_CTAN_F64_F64,	QB_CTAN_F32_F32	},
};

qb_float_op_factory factory_complex_sinh = {
	qb_resolve_expression_type_first_operand,
	qb_link_results_all_operands,
	qb_coerce_operands_all,
	qb_set_result_prototype,
	NULL,
	qb_set_result_temporary_value,
	qb_set_result_dimensions_first_operand,
	qb_select_opcode_complex_number,
	NULL,
	qb_transfer_operands_all,

	QB_COERCE_TO_LVALUE_TYPE | QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION,
	QB_ADDRESS_TEMPORARY,
	{	QB_CSINH_F64_F64,	QB_CSINH_F32_F32	},
};

qb_float_op_factory factory_complex_cosh = {
	qb_resolve_expression_type_first_operand,
	qb_link_results_all_operands,
	qb_coerce_operands_all,
	qb_set_result_prototype,
	NULL,
	qb_set_result_temporary_value,
	qb_set_result_dimensions_first_operand,
	qb_select_opcode_complex_number,
	NULL,
	qb_transfer_operands_all,

	QB_COERCE_TO_LVALUE_TYPE | QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION,
	QB_ADDRESS_TEMPORARY,
	{	QB_CCOSH_F64_F64,	QB_CCOSH_F32_F32	},
};

qb_float_op_factory factory_complex_tanh = {
	qb_resolve_expression_type_first_operand,
	qb_link_results_all_operands,
	qb_coerce_operands_all,
	qb_set_result_prototype,
	NULL,
	qb_set_result_temporary_value,
	qb_set_result_dimensions_first_operand,
	qb_select_opcode_complex_number,
	NULL,
	qb_transfer_operands_all,

	QB_COERCE_TO_LVALUE_TYPE | QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION,
	QB_ADDRESS_TEMPORARY,
	{	QB_CTANH_F64_F64,	QB_CTANH_F32_F32	},
};

qb_pixel_op_factory factory_sample_nearest = {
	qb_resolve_expression_type_first_operand,
	qb_link_results_all_operands,
	qb_coerce_operands_all,
	qb_set_result_prototype,
	qb_validate_operands_sampling,
	qb_set_result_temporary_value,
	qb_set_result_dimensions_sampling,
	qb_select_opcode_pixel,
	NULL,
	qb_transfer_operands_sampling,

	0,
	QB_RESULT_FROM_PURE_FUNCTION,
	QB_ADDRESS_TEMPORARY,
	{
		{	QB_SAMPLE_NN_3X_F64_U32_U32_F64_F64_F64,	QB_SAMPLE_NN_4X_F32_U32_U32_F32_F32_F32,	},
		{	QB_SAMPLE_NN_4X_F64_U32_U32_F64_F64_F64,	QB_SAMPLE_NN_4X_F32_U32_U32_F32_F32_F32,	},
	}
};

qb_pixel_op_factory factory_sample_bilinear = {
	qb_resolve_expression_type_first_operand,
	qb_link_results_all_operands,
	qb_coerce_operands_all,
	qb_set_result_prototype,
	qb_validate_operands_sampling,
	qb_set_result_temporary_value,
	qb_set_result_dimensions_sampling,
	qb_select_opcode_pixel,
	NULL,
	qb_transfer_operands_sampling,

	0,
	QB_RESULT_FROM_PURE_FUNCTION,
	QB_ADDRESS_TEMPORARY,
	{
		{	QB_SAMPLE_BL_3X_F64_U32_U32_F64_F64_F64,	QB_SAMPLE_BL_3X_F32_U32_U32_F32_F32_F32,	},
		{	QB_SAMPLE_BL_4X_F64_U32_U32_F64_F64_F64,	QB_SAMPLE_BL_4X_F32_U32_U32_F32_F32_F32,	},
	}
};

qb_float_op_factory factory_alpha_blend = {
	qb_resolve_expression_type_highest_rank,
	qb_link_results_all_operands,
	qb_coerce_operands_all,
	qb_set_result_prototype,
	qb_validate_operands_rgba,
	qb_set_result_temporary_value,
	qb_set_result_dimensions_larger_of_two,
	qb_select_opcode_basic,
	NULL,
	qb_transfer_operands_all,

	QB_COERCE_TO_FLOATING_POINT,
	QB_RESULT_FROM_PURE_FUNCTION,
	QB_ADDRESS_TEMPORARY,
	{	QB_BLEND_F64_F64_F64,		QB_BLEND_F32_F32_F32,	},
};

qb_pixel_op_factory factory_apply_premult = {
	qb_resolve_expression_type_first_operand,
	qb_link_results_all_operands,
	qb_coerce_operands_all,
	qb_set_result_prototype,
	qb_validate_operands_rgba,
	qb_set_result_temporary_value,
	qb_set_result_dimensions_first_operand,
	qb_select_opcode_pixel,
	NULL,
	qb_transfer_operands_all,

	QB_COERCE_TO_FLOATING_POINT,
	QB_RESULT_FROM_PURE_FUNCTION,
	QB_ADDRESS_TEMPORARY,
	{
		{	0,							0						},
		{	QB_PREMULT_F64_F64,			QB_PREMULT_F32_F32		},
	}
};

qb_pixel_op_factory factory_remove_premult = {
	qb_resolve_expression_type_first_operand,
	qb_link_results_all_operands,
	qb_coerce_operands_all,
	qb_set_result_prototype,
	qb_validate_operands_rgba,
	qb_set_result_temporary_value,
	qb_set_result_dimensions_first_operand,
	qb_select_opcode_pixel,
	NULL,
	qb_transfer_operands_all,

	QB_COERCE_TO_FLOATING_POINT,
	QB_RESULT_FROM_PURE_FUNCTION,
	QB_ADDRESS_TEMPORARY,
	{
		{	0,							0						},
		{	QB_UNPREMULT_F64_F64,		QB_UNPREMULT_F32_F32	},
	}
};

qb_pixel_op_factory factory_rgb2hsv = {
	qb_resolve_expression_type_first_operand,
	qb_link_results_all_operands,
	qb_coerce_operands_all,
	qb_set_result_prototype,
	qb_validate_operands_pixel,
	qb_set_result_temporary_value,
	qb_set_result_dimensions_first_operand,
	qb_select_opcode_pixel,
	NULL,
	qb_transfer_operands_all,

	QB_COERCE_TO_FLOATING_POINT,
	QB_RESULT_FROM_PURE_FUNCTION,
	QB_ADDRESS_TEMPORARY,
	{
		{	QB_RGB2HSV_3X_F64_F64,		QB_RGB2HSV_3X_F32_F32	},
		{	QB_RGB2HSV_4X_F64_F64,		QB_RGB2HSV_4X_F32_F32	},
	}
};

qb_pixel_op_factory factory_hsv2rgb = {
	qb_resolve_expression_type_first_operand,
	qb_link_results_all_operands,
	qb_coerce_operands_all,
	qb_set_result_prototype,
	qb_validate_operands_pixel,
	qb_set_result_temporary_value,
	qb_set_result_dimensions_first_operand,
	qb_select_opcode_pixel,
	NULL,
	qb_transfer_operands_all,

	QB_COERCE_TO_FLOATING_POINT,
	QB_RESULT_FROM_PURE_FUNCTION,
	QB_ADDRESS_TEMPORARY,
	{
		{	QB_HSV2RGB_3X_F64_F64,		QB_HSV2RGB_3X_F32_F32	},
		{	QB_HSV2RGB_4X_F64_F64,		QB_HSV2RGB_4X_F32_F32	},
	}
};

qb_pixel_op_factory factory_rgb2hsl = {
	qb_resolve_expression_type_first_operand,
	qb_link_results_all_operands,
	qb_coerce_operands_all,
	qb_set_result_prototype,
	qb_validate_operands_pixel,
	qb_set_result_temporary_value,
	qb_set_result_dimensions_first_operand,
	qb_select_opcode_pixel,
	NULL,
	qb_transfer_operands_all,

	QB_COERCE_TO_FLOATING_POINT,
	QB_RESULT_FROM_PURE_FUNCTION,
	QB_ADDRESS_TEMPORARY,
	{
		{	QB_RGB2HSL_3X_F64_F64,		QB_RGB2HSL_3X_F32_F32	},
		{	QB_RGB2HSL_4X_F64_F64,		QB_RGB2HSL_4X_F32_F32	},
	}
};

qb_pixel_op_factory factory_hsl2rgb = {
	qb_resolve_expression_type_first_operand,
	qb_link_results_all_operands,
	qb_coerce_operands_all,
	qb_set_result_prototype,
	qb_validate_operands_pixel,
	qb_set_result_temporary_value,
	qb_set_result_dimensions_first_operand,
	qb_select_opcode_pixel,
	NULL,
	qb_transfer_operands_all,

	QB_COERCE_TO_FLOATING_POINT,
	QB_RESULT_FROM_PURE_FUNCTION,
	QB_ADDRESS_TEMPORARY,
	{
		{	QB_HSL2RGB_3X_F64_F64,		QB_HSL2RGB_3X_F32_F32	},
		{	QB_HSL2RGB_4X_F64_F64,		QB_HSL2RGB_4X_F32_F32	},
	}
};
