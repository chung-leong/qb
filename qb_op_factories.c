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

typedef struct qb_op_factory					qb_op_factory; 
typedef struct qb_basic_op_factory				qb_basic_op_factory; 
typedef struct qb_float_op_factory				qb_float_op_factory; 
typedef struct qb_simple_op_factory				qb_simple_op_factory;
typedef struct qb_copy_op_factory				qb_copy_op_factory;
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

typedef qb_primitive_type (ZEND_FASTCALL *qb_resolve_expression_type_proc)(qb_compiler_context *cxt, void *op_factory, qb_operand *operands, uint32_t operand_count);

typedef void (ZEND_FASTCALL *qb_link_results_proc)(qb_compiler_context *cxt, void *op_factory, qb_operand *operands, uint32_t operand_count, qb_result_prototype *result_prototype);

typedef void (ZEND_FASTCALL *qb_validate_operands_proc)(qb_compiler_context *cxt, void *factory, qb_operand *operands, uint32_t operand_count);

typedef void (ZEND_FASTCALL *qb_coerce_operands_proc)(qb_compiler_context *cxt, void *factory, qb_operand *operands, uint32_t operand_count);

typedef void (ZEND_FASTCALL *qb_set_result_proc)(qb_compiler_context *cxt, void *factory, qb_operand *operands, uint32_t operand_count, qb_operand *result);

typedef void (ZEND_FASTCALL *qb_set_dimensions_proc)(qb_compiler_context *cxt, void *factory, qb_operand *operands, uint32_t operand_count, qb_variable_dimensions *dim);

typedef qb_opcode (ZEND_FASTCALL *qb_select_opcode_proc)(qb_compiler_context *cxt, void *factory, qb_operand *operands, uint32_t operand_count, qb_operand *result);

typedef void (ZEND_FASTCALL *qb_transfer_operands_proc)(qb_compiler_context *cxt, void *factory, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_op *qop);

#define OP_FACTORY_COMMON_ELEMENTS		\
	qb_resolve_expression_type_proc resolve_type;	\
	qb_link_results_proc link_results;	\
	qb_coerce_operands_proc coerce_operands;	\
	qb_validate_operands_proc validate_operands;	\
	qb_set_result_proc set_result;	\
	qb_set_dimensions_proc set_dimensions;	\
	qb_select_opcode_proc select_opcode;	\
	qb_transfer_operands_proc transfer_operands;	\
	uint32_t coercion_flags;	\
	uint32_t address_flags;	\
	uint32_t result_flags;	\

struct qb_op_factory {
	OP_FACTORY_COMMON_ELEMENTS
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

struct qb_comparison_op_factory {
	OP_FACTORY_COMMON_ELEMENTS
	qb_opcode opcodes[10];
	qb_basic_op_factory *set_factory;
};

struct qb_comparison_branch_op_factory {
	OP_FACTORY_COMMON_ELEMENTS
	qb_opcode opcodes[10];
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

static zend_always_inline uint32_t qb_get_coercion_flags(qb_compiler_context *cxt, void *factory) {
	qb_op_factory *f = factory;
	return f->coercion_flags;
}

static zend_always_inline uint32_t qb_get_result_flags(qb_compiler_context *cxt, void *factory) {
	qb_op_factory *f = factory;
	return f->result_flags;
}

static qb_variable_dimensions * ZEND_FASTCALL qb_get_result_dimensions(qb_compiler_context *cxt, void *factory, qb_operand *operands, uint32_t operand_count) {
	qb_op_factory *f = factory;
	qb_variable_dimensions *dim = &cxt->result_dimensions;

	dim->array_size = 0;
	dim->dimension_count = 0;
	dim->source_address = NULL;
	if(f->set_dimensions) {
		f->set_dimensions(cxt, f, operands, operand_count, dim);
	}
	return dim;
}

static void ZEND_FASTCALL qb_execute_op(qb_compiler_context *cxt, qb_op *op);
static qb_address * ZEND_FASTCALL qb_obtain_temporary_scalar(qb_compiler_context *cxt, qb_primitive_type desired_type);

static void ZEND_FASTCALL qb_produce_op(qb_compiler_context *cxt, void *factory, qb_operand *operands, uint32_t operand_count, qb_operand *result, uint32_t *jump_target_indices, uint32_t jump_target_count, qb_result_prototype *result_prototype) {
	qb_basic_op_factory *f = factory;
	qb_primitive_type expr_type = QB_TYPE_VOID;
	int32_t result_used = (result->type != QB_OPERAND_NONE);

	if(cxt->stage == QB_STAGE_RESULT_TYPE_RESOLUTION) {
		// determine the expression type
		if(f->resolve_type) {
			expr_type = f->resolve_type(cxt, f, operands, operand_count);
		}

		// indicate in the prototype that the expression has this type
		result_prototype->preliminary_type = expr_type;
		if(!(f->coercion_flags & QB_COERCE_TO_LVALUE_TYPE) || result->type == QB_OPERAND_NONE) {
			// as the result doesn't depend on the context (or there is no context at all)
			// we're certain about the type 
			result_prototype->final_type = expr_type;
		}
		result_prototype->operand_flags = f->coercion_flags;
		result_prototype->address_flags = f->address_flags;

		result->type = QB_OPERAND_RESULT_PROTOTYPE;
		result->result_prototype = result_prototype;
	} else if(cxt->stage == QB_STAGE_OPCODE_TRANSLATION) {
		// use the result from the previous stage, finalizing it first
		qb_finalize_result_prototype(cxt, result_prototype);
		expr_type = result_prototype->final_type;
	}

	// then, assign result to result object
	if(cxt->stage == QB_STAGE_RESULT_TYPE_RESOLUTION) {
		f->link_results(cxt, f, expr_type, operands, operand_count, result_prototype);
	} else if(cxt->stage == QB_STAGE_OPCODE_TRANSLATION) {
		f->set_result(cxt, f, expr_type, operands, operand_count, result, result_prototype);
	}

	// perform type coercion on operand
	if(f->coerce_operands) {
		// note that the handler might not necessarily convert all the operands to expr_type
		f->coerce_operands(cxt, f, expr_type, operands, operand_count)
	}

	if(cxt->stage == QB_STAGE_OPCODE_TRANSLATION) {
		// add the op
		uint32_t initial_op_count = cxt->op_count;
		uint32_t op_index, op_flags;
		uint32_t i;
		qb_opcode opcode;
		qb_op *qop;

		// add the ops for calculating on-demand values first
		for(i = 0; i < operand_count; i++) {
			qb_operand *operand = &operands[i];
			if(operand->type == QB_OPERAND_ADDRESS) {
				if(operand->address->flags & QB_ADDRESS_ON_DEMAND_VALUE) {
					qb_on_demand_address *od_address = (qb_on_demand_address *) operand->address;
					qb_operand *od_result = operand;
					qb_operand od_operands[4];
					uint32_t i;

					od_result->address = qb_obtain_temporary_scalar(cxt, od_address->type);
					for(i = 0; i < od_address->operand_count; i++) {
						od_operands[i].address = od_address->operand_addresses[i];
						od_operands[i].type = QB_OPERAND_ADDRESS;
					}
					qb_create_any_op(cxt, od_address->op_factory, od_operands, od_address->operand_count, od_result, NULL, 0);
				}
			}
		}

		// get the opcode for the operands 
		// at this point, the operands should have the correct type
		opcode = f->select_opcode(cxt, f, operands, operand_count, result);
		op_flags = qb_get_op_flags(cxt, opcode);

		if(result_used || (op_flags & (QB_OP_SIDE_EFFECT | QB_OP_JUMP))) {
			if(!(op_flags & QB_OP_VARIABLE_LENGTH)) {
				final_operand_count = qb_get_operand_count(cxt, opcode);
			} else {
				final_operand_count = f->get_operand_count(cxt, operands, operand_count);
			}

			for(i = 0; i < operand_count; i++) {
				qb_address *address = operands[operand_count].address;
				if(address->mode == QB_ADDRESS_MODE_ELE) {
					// see if the opcode has an ELE version
					// most do, but a few do not
					uint32_t op_flags = qb_get_op_flags(cxt, opcode);
					if(!(op_flags & QB_OP_VERSION_AVAILABLE_ELE)) {
						// copy the element to an temporary variable

					}
				}
			}

			// create the op
			qop = qb_append_op(cxt, opcode);
			qop->flags = op_flags;
			if(op_flags & QB_OP_BRANCH) {
				qop->jump_target_count = 2;
				qop->jump_target_indices = qb_allocate_indices(cxt->pool, 2);
			} else if(op_flags & QB_OP_JUMP) {
				qop->jump_target_count = 1;
				qop->jump_target_indices = qb_allocate_indices(cxt->pool, 1);
			}
			qop->opcode = opcode;
			qop->operands = qb_allocate_operands(cxt->pool, operand_count);
			qop->operand_count = operand_count;
			qop->line_number = cxt->line_number;

			// copy jump target indices
			if(jump_target_count > 0) {
				for(i = 0; i < jump_target_count; i++) {
					uint32_t jump_target_index = jump_target_indices[i];
					qop->jump_target_indices[i] = jump_target_index;
					qb_mark_jump_target(cxt, qop_index, jump_target_index);
				}
			}

			// unlock the operands after the op is created
			for(i = 0; i < operand_count; i++) {
				qb_operand *operand = &operands[i];
				if(operand->type == QB_OPERAND_ADDRESS) {
					qb_unlock_address(cxt, operand->address);
				}
			}

			if(result && result->type == QB_OPERAND_ADDRESS) {
				if(CONSTANT(result->address)) {
					// evalulate the expression at compile-time
					qb_execute_op(cxt, qop);

					// roll back the op counter
					cxt->op_count = initial_op_count;
				} else {
					// mark result address as writable
					qb_mark_as_writable(cxt, result->address);
				}
			}
		}
	}
}

static void ZEND_FASTCALL qb_create_op(qb_compiler_context *cxt, void *factory, qb_operand *operands, uint32_t operand_count, qb_operand *result) {
	qb_create_any_op(cxt, factory, operands, operand_count, result, NULL, 0);
}

static void ZEND_FASTCALL qb_create_branching_op(qb_compiler_context *cxt, void *factory, qb_operand *operands, uint32_t operand_count, uint32_t *jump_target_indices, uint32_t jump_target_count) {
	qb_create_any_op(cxt, factory, operands, operand_count, NULL, jump_target_indices, jump_target_count);
}

static void ZEND_FASTCALL qb_create_nop(qb_compiler_context *cxt) {
	qb_append_op(cxt, QB_NOP);
}

#include "qb_op_factory_operand_validation.c"
#include "qb_op_factory_type_resolution.c"
#include "qb_op_factory_result_linkage.c"
#include "qb_op_factory_operand_coercion.c"
#include "qb_op_factory_dimension_calculation.c"
#include "qb_op_factory_result_assignment.c"
#include "qb_op_factory_opcode_selection.c"

static qb_copy_op_factory factory_copy = {
	qb_resolve_expression_first_operand,
	qb_set_result_type,
	NULL,
	NULL,
	qb_select_copy_opcode,
	NULL,
	0,
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

/*
static qb_basic_op_factory factory_array_insert = {
	qb_append_array_insert,
	NULL,
	0,
	0,
	{	QB_AINS_F64_U32_F64,	QB_AINS_F32_U32_F32,	QB_AINS_I64_U32_I64,	QB_AINS_I64_U32_I64,	QB_AINS_I32_U32_I32,	QB_AINS_I32_U32_I32,	QB_AINS_I16_U32_I16,	QB_AINS_I16_U32_I16,	QB_AINS_I08_U32_I08,	QB_AINS_I08_U32_I08,	},
};
*/

static qb_basic_op_factory factory_array_pad = {
	qb_select_opcode_basic,
	qb_set_result_dimensions_array_pad,
	0,
	0,
	{	QB_APAD_F64_F64_S32_F64,	QB_APAD_F32_F32_S32_F32,	QB_APAD_I64_I64_S32_I64,	QB_APAD_I64_I64_S32_I64,	QB_APAD_I32_I32_S32_I32,	QB_APAD_I32_I32_S32_I32,	QB_APAD_I16_I16_S32_I16,	QB_APAD_I16_I16_S32_I16,	QB_APAD_I08_I08_S32_I08,	QB_APAD_I08_I08_S32_I08,	},
};

static qb_arithmetic_op_factory factory_increment = { 
	qb_select_opcode_arithmetic,
	NULL,
	0,
	0,
	{	QB_INC_F64,	QB_INC_F32,	QB_INC_I64,	QB_INC_I64,	QB_INC_I32,	QB_INC_I32,	QB_INC_I16,	QB_INC_I16,	QB_INC_I08,	QB_INC_I08,	},
	{
		{	QB_INC_2X_F64,	QB_INC_2X_F32,	},
		{	QB_INC_3X_F64,	QB_INC_3X_F32,	},
		{	QB_INC_4X_F64,	QB_INC_4X_F32,	},
	},
};

static qb_arithmetic_op_factory factory_decrement = { 
	qb_select_opcode_arithmetic,
	NULL,
	0,
	0,
	{	QB_DEC_F64,	QB_DEC_F32,	QB_DEC_I64,	QB_DEC_I64,	QB_DEC_I32,	QB_DEC_I32,	QB_DEC_I16,	QB_DEC_I16,	QB_DEC_I08,	QB_DEC_I08,	},
	{
		{	QB_DEC_2X_F64,	QB_DEC_2X_F32,	},
		{	QB_DEC_3X_F64,	QB_DEC_3X_F32,	},
		{	QB_DEC_4X_F64,	QB_DEC_4X_F32,	},
	},
};



static qb_basic_op_factory factory_bitwise_not = {
	qb_select_opcode_basic,
	qb_set_result_dimensions_matching,
	QB_COERCE_TO_INTEGER,
	QB_RESULT_FROM_PURE_FUNCTION | QB_TYPE_OPERAND,
	{	0,	0,	QB_BW_NOT_I64_I64,	QB_BW_NOT_I64_I64,	QB_BW_NOT_I32_I32,	QB_BW_NOT_I32_I32,	QB_BW_NOT_I16_I16,	QB_BW_NOT_I16_I16,	QB_BW_NOT_I08_I08,	QB_BW_NOT_I08_I08,	}
};

static qb_basic_op_factory factory_logical_not = {
	qb_select_opcode_basic,
	NULL,
	QB_COERCE_TO_BOOLEAN,
	QB_RESULT_FROM_PURE_FUNCTION | QB_RESULT_IS_BOOLEAN | QB_TYPE_I32,
	{	0,	0,	0,	0,	QB_NOT_I32_I32,	QB_NOT_I32_I32,	0,	0,	0,	0,	}
};

static qb_basic_op_factory factory_shift_left = { 
	qb_select_opcode_basic,
	qb_set_result_dimensions_matching,
	QB_COERCE_TO_HIGHEST_RANK | QB_COERCE_TO_LVALUE_TYPE | QB_COERCE_TO_INTEGER,
	QB_RESULT_FROM_PURE_FUNCTION | QB_TYPE_OPERAND,
	{	0,	0,	QB_SHL_U64_U64_U64,	QB_SHL_S64_S64_S64,	QB_SHL_U32_U32_U32,	QB_SHL_S32_S32_S32,	QB_SHL_U16_U16_U16,	QB_SHL_S16_S16_S16,	QB_SHL_U08_U08_U08,	QB_SHL_S08_S08_S08,	},
};

static qb_basic_op_factory factory_shift_right = { 
	qb_select_opcode_basic,
	qb_set_result_dimensions_matching,
	QB_COERCE_TO_HIGHEST_RANK | QB_COERCE_TO_LVALUE_TYPE | QB_COERCE_TO_INTEGER,
	QB_RESULT_FROM_PURE_FUNCTION | QB_TYPE_OPERAND,
	{	0,	0,	QB_SHR_U64_U64_U64,	QB_SHR_S64_S64_S64,	QB_SHR_U32_U32_U32,	QB_SHR_S32_S32_S32,	QB_SHR_U16_U16_U16,	QB_SHR_S16_S16_S16,	QB_SHR_U08_U08_U08,	QB_SHR_S08_S08_S08,	},
};

static qb_basic_op_factory factory_bitwise_or = { 
	qb_select_opcode_basic,
	qb_set_result_dimensions_matching,
	QB_COERCE_TO_HIGHEST_RANK | QB_COERCE_TO_LVALUE_TYPE | QB_COERCE_TO_INTEGER,
	QB_RESULT_FROM_PURE_FUNCTION | QB_TYPE_OPERAND,
	{	0,	0,	QB_BW_OR_I64_I64_I64,	QB_BW_OR_I64_I64_I64,	QB_BW_OR_I32_I32_I32,	QB_BW_OR_I32_I32_I32,	QB_BW_OR_I16_I16_I16,	QB_BW_OR_I16_I16_I16,	QB_BW_OR_I08_I08_I08,	QB_BW_OR_I08_I08_I08,	},
};

static qb_basic_op_factory factory_bitwise_and = { 
	qb_select_opcode_basic,
	qb_set_result_dimensions_matching,
	QB_COERCE_TO_HIGHEST_RANK | QB_COERCE_TO_LVALUE_TYPE | QB_COERCE_TO_INTEGER,
	QB_RESULT_FROM_PURE_FUNCTION | QB_TYPE_OPERAND,
	{	0,	0,	QB_BW_AND_I64_I64_I64,	QB_BW_AND_I64_I64_I64,	QB_BW_AND_I32_I32_I32,	QB_BW_AND_I32_I32_I32,	QB_BW_AND_I16_I16_I16,	QB_BW_AND_I16_I16_I16,	QB_BW_AND_I08_I08_I08,	QB_BW_AND_I08_I08_I08,	},
};

static qb_basic_op_factory factory_bitwise_xor = {
	qb_select_opcode_basic,
	qb_set_result_dimensions_matching,
	QB_COERCE_TO_HIGHEST_RANK | QB_COERCE_TO_LVALUE_TYPE | QB_COERCE_TO_INTEGER,
	QB_RESULT_FROM_PURE_FUNCTION | QB_TYPE_OPERAND,
	{	0,	0,	QB_BW_XOR_I64_I64_I64,	QB_BW_XOR_I64_I64_I64,	QB_BW_XOR_I32_I32_I32,	QB_BW_XOR_I32_I32_I32,	QB_BW_XOR_I16_I16_I16,	QB_BW_XOR_I16_I16_I16,	QB_BW_XOR_I08_I08_I08,	QB_BW_XOR_I08_I08_I08,	},
};

static qb_basic_op_factory factory_logical_and = {
	qb_select_opcode_basic,
	NULL,
	QB_COERCE_TO_BOOLEAN,
	QB_RESULT_FROM_PURE_FUNCTION | QB_RESULT_IS_BOOLEAN | QB_TYPE_I32,
	{	0,	0,	0,	0,	QB_AND_I32_I32_I32,	QB_AND_I32_I32_I32,	0,	0,	0,	0,	},
};

static qb_basic_op_factory factory_logical_or = {
	qb_select_opcode_basic,
	NULL,
	QB_COERCE_TO_BOOLEAN,
	QB_RESULT_FROM_PURE_FUNCTION | QB_RESULT_IS_BOOLEAN | QB_TYPE_I32,
	{	0,	0,	0,	0,	QB_OR_I32_I32_I32,	QB_OR_I32_I32_I32,	0,	0,	0,	0,	},
};

static qb_basic_op_factory factory_logical_xor = {
	qb_select_opcode_basic,
	NULL,
	QB_COERCE_TO_BOOLEAN,
	QB_RESULT_FROM_PURE_FUNCTION | QB_RESULT_IS_BOOLEAN | QB_TYPE_I32,
	{	0,	0,	0,	0,	QB_XOR_I32_I32_I32,	QB_XOR_I32_I32_I32,	0,	0,	0,	0,	},
};

static qb_arithmetic_op_factory factory_add = { 
	qb_select_opcode_arithmetic,
	qb_set_result_dimensions_matching,
	QB_COERCE_TO_HIGHEST_RANK | QB_COERCE_TO_LVALUE_TYPE,
	QB_RESULT_FROM_PURE_FUNCTION | QB_TYPE_OPERAND,
	{	QB_ADD_F64_F64_F64,	QB_ADD_F32_F32_F32,	QB_ADD_I64_I64_I64,	QB_ADD_I64_I64_I64,	QB_ADD_I32_I32_I32,	QB_ADD_I32_I32_I32,	QB_ADD_I16_I16_I16,	QB_ADD_I16_I16_I16,	QB_ADD_I08_I08_I08,	QB_ADD_I08_I08_I08,	},
	{
		{	QB_ADD_2X_F64_F64_F64,	QB_ADD_2X_F32_F32_F32,	},
		{	QB_ADD_3X_F64_F64_F64,	QB_ADD_3X_F32_F32_F32,	},
		{	QB_ADD_4X_F64_F64_F64,	QB_ADD_4X_F32_F32_F32,	},
	},
};

static qb_arithmetic_op_factory factory_subtract = { 
	qb_select_opcode_basic,
	qb_set_result_dimensions_matching,
	QB_COERCE_TO_HIGHEST_RANK | QB_COERCE_TO_LVALUE_TYPE,
	QB_RESULT_FROM_PURE_FUNCTION | QB_TYPE_OPERAND,
	{	QB_SUB_F64_F64_F64,	QB_SUB_F32_F32_F32,	QB_SUB_I64_I64_I64,	QB_SUB_I64_I64_I64,	QB_SUB_I32_I32_I32,	QB_SUB_I32_I32_I32,	QB_SUB_I16_I16_I16,	QB_SUB_I16_I16_I16,	QB_SUB_I08_I08_I08,	QB_SUB_I08_I08_I08,	},
	{
		{	QB_SUB_2X_F64_F64_F64,	QB_SUB_2X_F32_F32_F32,	},
		{	QB_SUB_3X_F64_F64_F64,	QB_SUB_3X_F32_F32_F32,	},
		{	QB_SUB_4X_F64_F64_F64,	QB_SUB_4X_F32_F32_F32,	},
	},
};

static qb_arithmetic_op_factory factory_multiply = { 
	qb_select_opcode_basic,
	qb_set_result_dimensions_matching,
	QB_COERCE_TO_HIGHEST_RANK | QB_COERCE_TO_LVALUE_TYPE,
	QB_RESULT_FROM_PURE_FUNCTION | QB_TYPE_OPERAND,
	{	QB_MUL_F64_F64_F64,	QB_MUL_F32_F32_F32,	QB_MUL_U64_U64_U64,	QB_MUL_U64_U64_U64,	QB_MUL_U32_U32_U32,	QB_MUL_U32_U32_U32,	QB_MUL_S16_S16_S16,	QB_MUL_S16_S16_S16,	QB_MUL_S08_S08_S08,	QB_MUL_S08_S08_S08,	},
	{
		{	QB_MUL_2X_F64_F64_F64,	QB_MUL_2X_F32_F32_F32,	},
		{	QB_MUL_3X_F64_F64_F64,	QB_MUL_3X_F32_F32_F32,	},
		{	QB_MUL_4X_F64_F64_F64,	QB_MUL_4X_F32_F32_F32,	},
	},
};

static qb_arithmetic_op_factory factory_divide = { 
	qb_select_opcode_basic,
	qb_set_result_dimensions_matching,
	QB_COERCE_TO_HIGHEST_RANK | QB_COERCE_TO_LVALUE_TYPE,
	QB_RESULT_FROM_PURE_FUNCTION | QB_TYPE_OPERAND,
	{	QB_DIV_F64_F64_F64,	QB_DIV_F32_F32_F32,	QB_DIV_U64_U64_U64,	QB_DIV_S64_S64_S64,	QB_DIV_U32_U32_U32,	QB_DIV_S32_S32_S32,	QB_DIV_U16_U16_U16,	QB_DIV_S16_S16_S16,	QB_DIV_U08_U08_U08,	QB_DIV_S08_S08_S08,	},
	{
		{	QB_DIV_2X_F64_F64_F64,	QB_DIV_2X_F32_F32_F32,	},
		{	QB_DIV_3X_F64_F64_F64,	QB_DIV_3X_F32_F32_F32,	},
		{	QB_DIV_4X_F64_F64_F64,	QB_DIV_4X_F32_F32_F32,	},
	},
};

static qb_arithmetic_op_factory factory_modulo = { 
	qb_select_opcode_basic,
	qb_set_result_dimensions_matching,
	QB_COERCE_TO_HIGHEST_RANK,
	QB_RESULT_FROM_PURE_FUNCTION | QB_TYPE_OPERAND,
	{	QB_MOD_F64_F64_F64,	QB_MOD_F32_F32_F32,	QB_MOD_U64_U64_U64,	QB_MOD_S64_S64_S64,	QB_MOD_U32_U32_U32,	QB_MOD_S32_S32_S32,	QB_MOD_U16_U16_U16,	QB_MOD_S16_S16_S16,	QB_MOD_U08_U08_U08,	QB_MOD_S08_S08_S08,	},
	{
		{	QB_MOD_2X_F64_F64_F64,	QB_MOD_2X_F32_F32_F32,	},
		{	QB_MOD_3X_F64_F64_F64,	QB_MOD_3X_F32_F32_F32,	},
		{	QB_MOD_4X_F64_F64_F64,	QB_MOD_4X_F32_F32_F32,	},
	},
};

static qb_float_op_factory factory_floor_modulo = { 
	qb_select_opcode_basic,
	qb_set_result_dimensions_matching,
	QB_COERCE_TO_HIGHEST_RANK | QB_COERCE_TO_LVALUE_TYPE,
	QB_TYPE_OPERAND,
	{	QB_MOD_FLR_F64_F64_F64,	QB_MOD_FLR_F32_F32_F32,	},
};

static qb_basic_op_factory factory_boolean = {
	qb_select_opcode_basic,
	NULL,
	0,
	QB_RESULT_IS_BOOLEAN | QB_TYPE_I32,
	{	QB_BOOL_F64_I32,	QB_BOOL_F32_I32,	QB_BOOL_I64_I32,	QB_BOOL_I64_I32,	QB_BOOL_I32_I32,	QB_BOOL_I32_I32,	QB_BOOL_I16_I32,	QB_BOOL_I16_I32,	QB_BOOL_I08_I32,	QB_BOOL_I08_I32,	},
};

static qb_basic_op_factory factory_set_equal = {
	qb_select_opcode_basic,
	qb_set_result_dimensions_matching,
	QB_COERCE_TO_HIGHEST_RANK,
	QB_RESULT_FROM_PURE_FUNCTION | QB_RESULT_IS_BOOLEAN | QB_TYPE_I32,
	{	QB_EQ_SET_F64_F64_I32,	QB_EQ_SET_F32_F32_I32,	QB_EQ_SET_I64_I64_I32,	QB_EQ_SET_I64_I64_I32,	QB_EQ_SET_I32_I32_I32,	QB_EQ_SET_I32_I32_I32,	QB_EQ_SET_I16_I16_I32,	QB_EQ_SET_I16_I16_I32,	QB_EQ_SET_I08_I08_I32,	QB_EQ_SET_I08_I08_I32,	},
};

static qb_basic_op_factory factory_set_not_equal = {
	qb_select_opcode_basic,
	qb_set_result_dimensions_matching,
	QB_COERCE_TO_HIGHEST_RANK,
	QB_RESULT_FROM_PURE_FUNCTION | QB_RESULT_IS_BOOLEAN | QB_TYPE_I32,
	{	QB_NE_SET_F64_F64_I32,	QB_NE_SET_F32_F32_I32,	QB_NE_SET_I64_I64_I32,	QB_NE_SET_I64_I64_I32,	QB_NE_SET_I32_I32_I32,	QB_NE_SET_I32_I32_I32,	QB_NE_SET_I16_I16_I32,	QB_NE_SET_I16_I16_I32,	QB_NE_SET_I08_I08_I32,	QB_NE_SET_I08_I08_I32,	},
};

static qb_basic_op_factory factory_set_less_than = {
	qb_select_opcode_basic,
	qb_set_result_dimensions_matching,
	QB_COERCE_TO_HIGHEST_RANK,
	QB_RESULT_FROM_PURE_FUNCTION | QB_RESULT_IS_BOOLEAN | QB_TYPE_I32,
	{	QB_LT_SET_F64_F64_I32,	QB_LT_SET_F32_F32_I32,	QB_LT_SET_U64_U64_I32,	QB_LT_SET_S64_S64_I32,	QB_LT_SET_U32_U32_I32,	QB_LT_SET_S32_S32_I32,	QB_LT_SET_U16_U16_I32,	QB_LT_SET_S16_S16_I32,	QB_LT_SET_U08_U08_I32,	QB_LT_SET_S08_S08_I32,	},
};

static qb_basic_op_factory factory_set_less_equal = {
	qb_select_opcode_basic,
	qb_set_result_dimensions_matching,
	QB_COERCE_TO_HIGHEST_RANK,
	QB_RESULT_FROM_PURE_FUNCTION | QB_RESULT_IS_BOOLEAN | QB_TYPE_I32,
	{	QB_LE_SET_F64_F64_I32,	QB_LE_SET_F32_F32_I32,	QB_LE_SET_U64_U64_I32,	QB_LE_SET_S64_S64_I32,	QB_LE_SET_U32_U32_I32,	QB_LE_SET_S32_S32_I32,	QB_LE_SET_U16_U16_I32,	QB_LE_SET_S16_S16_I32,	QB_LE_SET_U08_U08_I32,	QB_LE_SET_S08_S08_I32,	},
};

static qb_comparison_op_factory factory_equal = {
	qb_select_opcode_comparison,
	NULL,
	QB_COERCE_TO_HIGHEST_RANK,
	QB_RESULT_FROM_PURE_FUNCTION | QB_RESULT_IS_BOOLEAN | QB_TYPE_I32,
	{	QB_EQ_F64_F64_I32,	QB_EQ_F32_F32_I32,	QB_EQ_I64_I64_I32,	QB_EQ_I64_I64_I32,	QB_EQ_I32_I32_I32,	QB_EQ_I32_I32_I32,	QB_EQ_I16_I16_I32,	QB_EQ_I16_I16_I32,	QB_EQ_I08_I08_I32,	QB_EQ_I08_I08_I32,	},
	&factory_set_equal,
};

static qb_comparison_op_factory factory_not_equal = { 
	qb_select_opcode_comparison,
	NULL,
	QB_COERCE_TO_HIGHEST_RANK,
	QB_RESULT_FROM_PURE_FUNCTION | QB_RESULT_IS_BOOLEAN | QB_TYPE_I32,
	{	QB_NE_F64_F64_I32,	QB_NE_F32_F32_I32,	QB_NE_I64_I64_I32,	QB_NE_I64_I64_I32,	QB_NE_I32_I32_I32,	QB_NE_I32_I32_I32,	QB_NE_I16_I16_I32,	QB_NE_I16_I16_I32,	QB_NE_I08_I08_I32,	QB_NE_I08_I08_I32,	},
	&factory_set_not_equal,
};

static qb_comparison_op_factory factory_less_than = {
	qb_select_opcode_comparison,
	NULL,
	QB_COERCE_TO_HIGHEST_RANK,
	QB_RESULT_FROM_PURE_FUNCTION | QB_RESULT_IS_BOOLEAN | QB_TYPE_I32,
	{	QB_LT_F64_F64_I32,	QB_LT_F32_F32_I32,	QB_LT_U64_U64_I32,	QB_LT_S64_S64_I32,	QB_LT_U32_U32_I32,	QB_LT_S32_S32_I32,	QB_LT_U16_U16_I32,	QB_LT_S16_S16_I32,	QB_LT_U08_U08_I32,	QB_LT_S08_S08_I32,	},
	&factory_set_less_than,
};

static qb_comparison_op_factory factory_less_equal = {
	qb_select_opcode_comparison,
	NULL,
	QB_COERCE_TO_HIGHEST_RANK,
	QB_RESULT_FROM_PURE_FUNCTION | QB_RESULT_IS_BOOLEAN | QB_TYPE_I32,
	{	QB_LE_F64_F64_I32,	QB_LE_F32_F32_I32,	QB_LE_U64_U64_I32,	QB_LE_S64_S64_I32,	QB_LE_U32_U32_I32,	QB_LE_S32_S32_I32,	QB_LE_U16_U16_I32,	QB_LE_S16_S16_I32,	QB_LE_U08_U08_I32,	QB_LE_S08_S08_I32,	},
	&factory_set_less_equal,
};

static qb_comparison_branch_op_factory factory_branch_on_equal = {
	qb_select_opcode_basic,
	NULL,
	QB_COERCE_TO_HIGHEST_RANK,
	0,
	{	QB_IF_EQ_F64_F64,	QB_IF_EQ_F32_F32,	QB_IF_EQ_I64_I64,	QB_IF_EQ_I64_I64,	QB_IF_EQ_I32_I32,	QB_IF_EQ_I32_I32,	QB_IF_EQ_I16_I16,	QB_IF_EQ_I16_I16,	QB_IF_EQ_I08_I08,	QB_IF_EQ_I08_I08,	},
};

static qb_comparison_branch_op_factory factory_branch_on_not_equal = { 
	qb_select_opcode_basic,
	NULL,
	QB_COERCE_TO_HIGHEST_RANK,
	0,
	{	QB_IF_NE_F64_F64,	QB_IF_NE_F32_F32,	QB_IF_NE_I64_I64,	QB_IF_NE_I64_I64,	QB_IF_NE_I32_I32,	QB_IF_NE_I32_I32,	QB_IF_NE_I16_I16,	QB_IF_NE_I16_I16,	QB_IF_NE_I08_I08,	QB_IF_NE_I08_I08,	},
};

static qb_comparison_branch_op_factory factory_branch_on_less_than = {
	qb_select_opcode_basic,
	NULL,
	QB_COERCE_TO_HIGHEST_RANK,
	0,
	{	QB_IF_LT_F64_F64,	QB_IF_LT_F32_F32,	QB_IF_LT_U64_U64,	QB_IF_LT_S64_S64,	QB_IF_LT_U32_U32,	QB_IF_LT_S32_S32,	QB_IF_LT_U16_U16,	QB_IF_LT_S16_S16,	QB_IF_LT_U08_U08,	QB_IF_LT_S08_S08,	},
};

static qb_comparison_branch_op_factory factory_branch_on_less_equal = {
	qb_select_opcode_basic,
	NULL,
	QB_COERCE_TO_HIGHEST_RANK,
	0,
	{	QB_IF_LE_F64_F64,	QB_IF_LE_F32_F32,	QB_IF_LE_U64_U64,	QB_IF_LE_S64_S64,	QB_IF_LE_U32_U32,	QB_IF_LE_S32_S32,	QB_IF_LE_U16_U16,	QB_IF_LE_S16_S16,	QB_IF_LE_U08_U08,	QB_IF_LE_S08_S08,	},
};

static qb_basic_op_factory factory_set_greater_equal = {
	qb_select_opcode_basic,
	qb_set_result_dimensions_matching,
	QB_COERCE_TO_HIGHEST_RANK,
	QB_RESULT_FROM_PURE_FUNCTION | QB_RESULT_IS_BOOLEAN | QB_TYPE_I32,
	{	QB_LE_SET_F64_F64_I32,	QB_LE_SET_F32_F32_I32,	QB_LE_SET_U64_U64_I32,	QB_LE_SET_S64_S64_I32,	QB_LE_SET_U32_U32_I32,	QB_LE_SET_S32_S32_I32,	QB_LE_SET_U16_U16_I32,	QB_LE_SET_S16_S16_I32,	QB_LE_SET_U08_U08_I32,	QB_LE_SET_S08_S08_I32,	},
};

static qb_basic_op_factory factory_set_greater_than = {
	qb_select_opcode_basic,
	qb_set_result_dimensions_matching,
	QB_COERCE_TO_HIGHEST_RANK,
	QB_RESULT_FROM_PURE_FUNCTION | QB_RESULT_IS_BOOLEAN | QB_TYPE_I32,
	{	QB_LT_SET_F64_F64_I32,	QB_LT_SET_F32_F32_I32,	QB_LT_SET_U64_U64_I32,	QB_LT_SET_S64_S64_I32,	QB_LT_SET_U32_U32_I32,	QB_LT_SET_S32_S32_I32,	QB_LT_SET_U16_U16_I32,	QB_LT_SET_S16_S16_I32,	QB_LT_SET_U08_U08_I32,	QB_LT_SET_S08_S08_I32,	},
};

static qb_basic_op_factory factory_set_not = {
	qb_select_opcode_basic,
	qb_set_result_dimensions_matching,
	QB_COERCE_TO_BOOLEAN,
	QB_RESULT_FROM_PURE_FUNCTION | QB_RESULT_IS_BOOLEAN | QB_TYPE_I32,
	{	0,	0,	0,	0,	QB_NOT_SET_I32_I32,	QB_NOT_SET_I32_I32,	0,	0,	0,	0,	},
};

static qb_basic_op_factory factory_all = {
	qb_select_opcode_basic,
	NULL,
	QB_COERCE_TO_BOOLEAN,
	QB_RESULT_FROM_PURE_FUNCTION | QB_TYPE_I32,
	{	0,	0,	0,	0,	QB_ALL_I32_I32,	QB_ALL_I32_I32,	0,	0,	0,	0,	},
};

static qb_basic_op_factory factory_any = {
	qb_select_opcode_basic,
	NULL,
	QB_COERCE_TO_BOOLEAN,
	QB_RESULT_FROM_PURE_FUNCTION | QB_RESULT_IS_BOOLEAN | QB_TYPE_I32,
	{	0,	0,	0,	0,	QB_ANY_I32_I32,	QB_ANY_I32_I32,	0,	0,	0,	0,	},
};

static qb_address *ZEND_FASTCALL qb_get_array_dimensions(qb_compiler_context *cxt, qb_address *address);

/*
static qb_op *ZEND_FASTCALL qb_append_concat_op(qb_compiler_context *cxt, void *factory, qb_operand *operands, uint32_t operand_count, qb_operand *result) {
	qb_string_op_factory *f = factory;
	qb_address *addition = operands[0].address;
	qb_opcode opcode;
	qb_op *qop;

	if(addition->dimension_count > 1) {
		qb_address *subarray_sizes_address = qb_get_array_dimensions(cxt, addition);
		opcode = f->multidim_opcodes[QB_TYPE_F64 - addition->type];
		qop = qb_append_op(cxt, opcode, 3);
		qop->operands[0].type = QB_OPERAND_ADDRESS_ARR;
		qop->operands[0].address = addition;
		qop->operands[1].type = QB_OPERAND_ADDRESS_ARR;
		qop->operands[1].address = subarray_sizes_address;
		qop->operands[2] = *result;
		qb_unlock_address(cxt, subarray_sizes_address);
	} else {
		if(addition->flags & QB_ADDRESS_STRING) {
			opcode = f->text_opcode;
		} else {
			opcode = f->opcodes[QB_TYPE_F64 - addition->type];
		}
		qop = qb_append_op(cxt, opcode, 2);
		qop->operands[0] = operands[0];
		qop->operands[1] = *result;
	}
	return qop;
}
*/

/*
static qb_string_op_factory factory_concat = {
	qb_append_concat_op,
	NULL,
	0,
	0,
	{	QB_CAT_F64_U08,	QB_CAT_F32_U08,	QB_CAT_U64_U08,	QB_CAT_S64_U08,	QB_CAT_U32_U08,	QB_CAT_S32_U08,	QB_CAT_U16_U08,	QB_CAT_S16_U08,	QB_CAT_U08_U08,	QB_CAT_S08_U08,	},
	{	QB_CAT_DIM_F64_U32_U08,	QB_CAT_DIM_F32_U32_U08,	QB_CAT_DIM_U64_U32_U08,	QB_CAT_DIM_S64_U32_U08,	QB_CAT_DIM_U32_U32_U08,	QB_CAT_DIM_S32_U32_U08,	QB_CAT_DIM_U16_U32_U08,	QB_CAT_DIM_S16_U32_U08,	QB_CAT_DIM_U08_U32_U08,	QB_CAT_DIM_S08_U32_U08,	},
	QB_CAT_STR_U08_U08,
};
*/

static qb_string_op_factory factory_print = {
	qb_select_opcode_print,
	NULL,
	0,
	0,
	{	QB_PRN_F64,	QB_PRN_F32,	QB_PRN_U64,	QB_PRN_S64,	QB_PRN_U32,	QB_PRN_S32,	QB_PRN_U16,	QB_PRN_S16,	QB_PRN_U08,	QB_PRN_S08,	},
	{	QB_PRN_DIM_F64_U32,	QB_PRN_DIM_F32_U32,	QB_PRN_DIM_U64_U32,	QB_PRN_DIM_S64_U32,	QB_PRN_DIM_U32_U32,	QB_PRN_DIM_S32_U32,	QB_PRN_DIM_U16_U32,	QB_PRN_DIM_S16_U32,	QB_PRN_DIM_U08_U32,	QB_PRN_DIM_S08_U32,	},
	QB_PRN_STR_U08,
};

static qb_simple_op_factory factory_return = {
	qb_select_opcode_simple,
	NULL,
	0,
	0,
	QB_RET,
};

static qb_simple_op_factory factory_exit = {
	qb_select_opcode_simple,
	NULL,
	0,
	0,
	QB_EXIT_I32,
};

static qb_simple_op_factory factory_ext = {
	qb_select_opcode_simple,
	NULL,
	0,
	0,
	QB_EXT_U32,
};

static qb_simple_op_factory factory_jump = {
	qb_select_opcode_simple,
	NULL,
	0,
	0,
	QB_JMP,
};

static qb_simple_op_factory factory_branch_on_true = {
	qb_select_opcode_simple,
	NULL,
	QB_COERCE_TO_BOOLEAN,
	0,
	QB_IF_T_I32,
};

static qb_simple_op_factory factory_branch_on_false = {
	qb_select_opcode_simple,
	NULL,
	QB_COERCE_TO_BOOLEAN,
	0,
	QB_IF_F_I32,
};

/*
static qb_fcall_op_factory factory_fcall = {
	qb_append_fcall,
	NULL,
	0,
	0,
	QB_FCALL,
};
*/

static qb_float_op_factory factory_sin = { 
	qb_select_opcode_basic,
	qb_set_result_dimensions_matching,
	QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION | QB_TYPE_OPERAND,
	{	QB_SIN_F64_F64,			QB_SIN_F32_F32,	},
};

static qb_float_op_factory factory_asin = {
	qb_select_opcode_basic,
	qb_set_result_dimensions_matching,
	QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION | QB_TYPE_OPERAND,
	{	QB_ASIN_F64_F64,		QB_ASIN_F32_F32,	},
};

static qb_float_op_factory factory_cos = {
	qb_select_opcode_basic,
	qb_set_result_dimensions_matching,
	QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION | QB_TYPE_OPERAND,
	{	QB_COS_F64_F64,			QB_COS_F32_F32,	},
};

static qb_float_op_factory factory_acos = {
	qb_select_opcode_basic,
	qb_set_result_dimensions_matching,
	QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION | QB_TYPE_OPERAND,
	{	QB_ACOS_F64_F64,		QB_ACOS_F32_F32,	},
};

static qb_float_op_factory factory_tan = {
	qb_select_opcode_basic,
	qb_set_result_dimensions_matching,
	QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION | QB_TYPE_OPERAND,
	{	QB_TAN_F64_F64,			QB_TAN_F32_F32,	},
};

static qb_float_op_factory factory_atan = {
	qb_select_opcode_basic,
	qb_set_result_dimensions_matching,
	QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION | QB_TYPE_OPERAND,
	{	QB_ATAN_F64_F64,		QB_ATAN_F32_F32,	},
};

static qb_float_op_factory factory_atan2 = {
	qb_select_opcode_basic,
	qb_set_result_dimensions_matching,
	QB_COERCE_TO_HIGHEST_RANK | QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION | QB_TYPE_OPERAND,
	{	QB_ATAN2_F32_F32_F32,	QB_ATAN2_F32_F32_F32,	},
};

static qb_float_op_factory factory_sinh = {
	qb_select_opcode_basic,
	qb_set_result_dimensions_matching,
	QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION | QB_TYPE_OPERAND,
	{	QB_SINH_F64_F64,		QB_SINH_F32_F32,	},
};

static qb_float_op_factory factory_asinh = {
	qb_select_opcode_basic,
	qb_set_result_dimensions_matching,
	QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION | QB_TYPE_OPERAND,
	{	QB_ASINH_F64_F64,		QB_ASINH_F32_F32,	},
};

static qb_float_op_factory factory_cosh = {
	qb_select_opcode_basic,
	qb_set_result_dimensions_matching,
	QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION | QB_TYPE_OPERAND,
	{	QB_COSH_F64_F64,		QB_COSH_F32_F32,	},
};

static qb_float_op_factory factory_acosh = {
	qb_select_opcode_basic,
	qb_set_result_dimensions_matching,
	QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION | QB_TYPE_OPERAND,
	{	QB_ACOSH_F64_F64,		QB_ACOSH_F32_F32,	},
};

static qb_float_op_factory factory_tanh = {
	qb_select_opcode_basic,
	qb_set_result_dimensions_matching,
	QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION | QB_TYPE_OPERAND,
	{	QB_TANH_F64_F64,		QB_TANH_F32_F32,	},
};

static qb_float_op_factory factory_atanh = {
	qb_select_opcode_basic,
	qb_set_result_dimensions_matching,
	QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION | QB_TYPE_OPERAND,
	{	QB_ATANH_F64_F64,		QB_ATANH_F32_F32,	},
};

static qb_float_op_factory factory_ceil = {
	qb_select_opcode_basic,
	qb_set_result_dimensions_matching,
	QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION | QB_TYPE_OPERAND,
	{	QB_CEIL_F64_F64,		QB_CEIL_F32_F32,	},
};

static qb_float_op_factory factory_floor = {
	qb_select_opcode_basic,
	qb_set_result_dimensions_matching,
	QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION | QB_TYPE_OPERAND,
	{	QB_FLOOR_F64_F64,		QB_FLOOR_F32_F32,	},
};

static qb_float_op_factory factory_fract = {
	qb_select_opcode_basic,
	qb_set_result_dimensions_matching,
	QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION | QB_TYPE_OPERAND,
	{	QB_FRACT_F64_F64,		QB_FRACT_F32_F32,	},
};

static qb_float_op_factory factory_round = {
	qb_select_opcode_basic,
	qb_set_result_dimensions_matching,
	QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION | QB_TYPE_OPERAND,
	{	QB_ROUND_F64_I32_I32_F64,	QB_ROUND_F32_I32_I32_F32,	},
};

static qb_float_op_factory factory_log = {
	qb_select_opcode_basic,
	qb_set_result_dimensions_matching,
	QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION | QB_TYPE_OPERAND,
	{	QB_LOG_F64_F64,			QB_LOG_F32_F32,	},
};

static qb_float_op_factory factory_log1p = {
	qb_select_opcode_basic,
	qb_set_result_dimensions_matching,
	QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION | QB_TYPE_OPERAND,
	{	QB_LOG1P_F64_F64,		QB_LOG1P_F32_F32,	},
};

static qb_float_op_factory factory_log10 = {
	qb_select_opcode_basic,
	qb_set_result_dimensions_matching,
	QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION | QB_TYPE_OPERAND,
	{	QB_LOG10_F64_F64,		QB_LOG10_F32_F32,	},
};

static qb_float_op_factory factory_log2 = {
	qb_select_opcode_basic,
	qb_set_result_dimensions_matching,
	QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION | QB_TYPE_OPERAND,
	{	QB_LOG2_F64_F64,		QB_LOG2_F32_F32,	},
};

static qb_float_op_factory factory_exp = {
	qb_select_opcode_basic,
	qb_set_result_dimensions_matching,
	QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION | QB_TYPE_OPERAND,
	{	QB_EXP_F64_F64,			QB_EXP_F32_F32,	},
};

static qb_float_op_factory factory_exp2 = {
	qb_select_opcode_basic,
	qb_set_result_dimensions_matching,
	QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION | QB_TYPE_OPERAND,
	{	QB_EXP2_F64_F64,		QB_EXP2_F32_F32,	},
};

static qb_float_op_factory factory_expm1 = {
	qb_select_opcode_basic,
	qb_set_result_dimensions_matching,
	QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION | QB_TYPE_OPERAND,
	{	QB_EXPM1_F64_F64,		QB_EXPM1_F32_F32,	},
};

static qb_float_op_factory factory_sqrt = {
	qb_select_opcode_basic,
	qb_set_result_dimensions_matching,
	QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION | QB_TYPE_OPERAND,
	{	QB_SQRT_F64_F64,		QB_SQRT_F32_F32,	},
};

static qb_float_op_factory factory_rsqrt = {
	qb_select_opcode_basic,
	qb_set_result_dimensions_matching,
	QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION | QB_TYPE_OPERAND,
	{	QB_RSQRT_F64_F64,		QB_RSQRT_F32_F32,	},
};

static qb_float_op_factory factory_pow = {
	qb_select_opcode_basic,
	qb_set_result_dimensions_matching,
	QB_COERCE_TO_HIGHEST_RANK | QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION | QB_TYPE_OPERAND,
	{	QB_POW_F64_F64_F64,		QB_POW_F32_F32_F32,	},
};

static qb_float_op_factory factory_hypot = {
	qb_select_opcode_basic,
	qb_set_result_dimensions_matching,
	QB_COERCE_TO_HIGHEST_RANK | QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION | QB_TYPE_OPERAND,
	{	QB_HYPOT_F64_F64_F64,	QB_HYPOT_F32_F32_F32,	},
};

static qb_float_op_factory factory_sign = { 
	qb_select_opcode_basic,
	qb_set_result_dimensions_matching,
	QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION | QB_TYPE_OPERAND,
	{	QB_SIGN_F64_F64,		QB_SIGN_F32_F32,	},
};

static qb_float_op_factory factory_step = {
	qb_select_opcode_basic,
	qb_set_result_dimensions_matching,
	QB_COERCE_TO_HIGHEST_RANK | QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION | QB_TYPE_OPERAND,
	{	QB_STEP_F64_F64_F64,		QB_STEP_F32_F32_F32,	},
};

static qb_float_op_factory factory_clamp = {
	qb_select_opcode_basic,
	qb_set_result_dimensions_matching,
	QB_COERCE_TO_HIGHEST_RANK | QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION | QB_TYPE_OPERAND,
	{	QB_CLAMP_F64_F64_F64_F64,	QB_CLAMP_F32_F32_F32_F32,	},
};

static qb_float_op_factory factory_mix = {
	qb_select_opcode_basic,
	qb_set_result_dimensions_matching,
	QB_COERCE_TO_HIGHEST_RANK | QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION | QB_TYPE_OPERAND,
	{	QB_MIX_F64_F64_F64_F64,		QB_MIX_F32_F32_F32_F32,	},
};

static qb_float_op_factory factory_smooth_step = {
	qb_select_opcode_basic,
	qb_set_result_dimensions_matching,
	QB_COERCE_TO_HIGHEST_RANK | QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION | QB_TYPE_OPERAND,
	{	QB_SSTEP_F64_F64_F64_F64,	QB_SSTEP_F32_F32_F32_F32,	},
};

static qb_float_op_factory factory_deg2rad = {
	qb_select_opcode_basic,
	qb_set_result_dimensions_matching,
	QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION | QB_TYPE_OPERAND,
	{	QB_DEG2RAD_F64_F64,		QB_DEG2RAD_F32_F32,	},
};

static qb_float_op_factory factory_rad2deg = {
	qb_select_opcode_basic,
	qb_set_result_dimensions_matching,
	QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION | QB_TYPE_OPERAND,
	{	QB_RAD2DEG_F64_F64,		QB_RAD2DEG_F32_F32,	},
};

static qb_float_op_factory factory_is_finite = {
	qb_select_opcode_basic,
	qb_set_result_dimensions_matching,
	QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION | QB_RESULT_IS_BOOLEAN | QB_TYPE_I32,
	{	QB_FIN_F64_I32,		QB_FIN_F32_I32,	},
};

static qb_float_op_factory factory_is_infinite = {
	qb_select_opcode_basic,
	qb_set_result_dimensions_matching,
	QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION | QB_RESULT_IS_BOOLEAN | QB_TYPE_I32,
	{	QB_INF_F64_I32,		QB_INF_F32_I32,	},
};

static qb_float_op_factory factory_is_nan = {
	qb_select_opcode_basic,
	qb_set_result_dimensions_matching,
	QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION | QB_RESULT_IS_BOOLEAN | QB_TYPE_I32,
	{	QB_NAN_F64_I32,		QB_NAN_F32_I32,	},
};	

static qb_basic_op_factory factory_abs = {
	qb_select_opcode_basic,
	qb_set_result_dimensions_matching,
	QB_COERCE_TO_SIGNED,
	QB_RESULT_FROM_PURE_FUNCTION | QB_TYPE_OPERAND,
	{	QB_ABS_F64_F64,	QB_ABS_F32_F32,	0,	QB_ABS_S64_S64,	0,	QB_ABS_S32_S32,	0,	QB_ABS_S16_S16,	0,	QB_ABS_S08_S08,	},
};

static qb_minmax_op_factory factory_min = {
	qb_select_opcode_basic,
	NULL,
	QB_COERCE_TO_HIGHEST_RANK,
	QB_RESULT_FROM_PURE_FUNCTION | QB_TYPE_OPERAND,
	{	QB_MIN_F64_F64_F64,	QB_MIN_F32_F32_F32,	QB_MIN_U64_U64_U64,	QB_MIN_S64_S64_S64,	QB_MIN_U32_U32_U32,	QB_MIN_S32_S32_S32,	QB_MIN_U16_U16_U16,	QB_MIN_S16_S16_S16,	QB_MIN_U08_U08_U08,		QB_MIN_S08_S08_S08,	},
	{	QB_AMIN_F64_F64,	QB_AMIN_F32_F32,	QB_AMIN_U64_U64,	QB_AMIN_S64_S64,	QB_AMIN_U32_U32,	QB_AMIN_S32_S32,	QB_AMIN_U16_U16,	QB_AMIN_S16_S16,	QB_AMIN_U08_U08,		QB_AMIN_S08_S08,	},
};

static qb_minmax_op_factory factory_max = { 
	qb_select_opcode_basic,
	NULL,
	QB_COERCE_TO_HIGHEST_RANK,
	QB_RESULT_FROM_PURE_FUNCTION | QB_TYPE_OPERAND,
	{	QB_MAX_F64_F64_F64,	QB_MAX_F32_F32_F32,	QB_MAX_U64_U64_U64,	QB_MAX_S64_S64_S64,	QB_MAX_U32_U32_U32,	QB_MAX_S32_S32_S32,	QB_MAX_U16_U16_U16,	QB_MAX_S16_S16_S16,	QB_MAX_U08_U08_U08,		QB_MAX_S08_S08_S08,	},
	{	QB_AMAX_F64_F64,	QB_AMAX_F32_F32,	QB_AMAX_U64_U64,	QB_AMAX_S64_S64,	QB_AMAX_U32_U32,	QB_AMAX_S32_S32,	QB_AMAX_U16_U16,	QB_AMAX_S16_S16,	QB_AMAX_U08_U08,		QB_AMAX_S08_S08,	},
};

static qb_basic_op_factory factory_rand = {
	qb_select_opcode_basic,
	qb_set_result_dimensions_matching,
	QB_COERCE_TO_LVALUE_TYPE,
	QB_TYPE_OPERAND,
	{	0 ,	0,	QB_RAND_U64_U64_U64,	QB_RAND_S64_S64_S64,	QB_RAND_U32_U32_U32,	QB_RAND_S32_S32_S32,	QB_RAND_U16_U16_U16,	QB_RAND_S16_S16_S16,	QB_RAND_U08_U08_U08,	QB_RAND_S08_S08_S08,	},
};

static qb_basic_op_factory factory_mt_rand = {
	qb_select_opcode_basic,
	qb_set_result_dimensions_matching,
	QB_COERCE_TO_LVALUE_TYPE,
	QB_TYPE_OPERAND,
	{	0,	0,	QB_MT_RAND_U64_U64_U64,	QB_MT_RAND_S64_S64_S64,	QB_MT_RAND_U32_U32_U32,	QB_MT_RAND_S32_S32_S32,	QB_MT_RAND_U16_U16_U16,	QB_MT_RAND_S16_S16_S16,	QB_MT_RAND_U08_U08_U08,	QB_MT_RAND_S08_S08_S08,	},
};

static qb_float_op_factory factory_lcg = {
	qb_select_opcode_basic,
	qb_set_result_dimensions_matching,
	QB_COERCE_TO_LVALUE_TYPE,
	QB_TYPE_OPERAND,
	{	QB_LCG_F64,				QB_LCG_F32,	},
};

static qb_pixel_op_factory factory_sample_nearest	= { 
	qb_select_opcode_pixel,
	qb_set_result_dimensions_sampling,
	QB_COERCE_TO_HIGHEST_RANK | QB_COERCE_TO_FLOATING_POINT,
	0,
	{
		{	QB_SAMPLE_NN_3X_F64_U32_U32_F64_F64_F64,	QB_SAMPLE_NN_4X_F32_U32_U32_F32_F32_F32,	},
		{	QB_SAMPLE_NN_4X_F64_U32_U32_F64_F64_F64,	QB_SAMPLE_NN_4X_F32_U32_U32_F32_F32_F32,	},
	}
};

static qb_pixel_op_factory factory_sample_bilinear = {
	qb_select_opcode_pixel,
	qb_set_result_dimensions_sampling,
	QB_COERCE_TO_HIGHEST_RANK | QB_COERCE_TO_FLOATING_POINT,
	0,
	{
		{	QB_SAMPLE_BL_3X_F64_U32_U32_F64_F64_F64,	QB_SAMPLE_BL_3X_F32_U32_U32_F32_F32_F32,	},
		{	QB_SAMPLE_BL_4X_F64_U32_U32_F64_F64_F64,	QB_SAMPLE_BL_4X_F32_U32_U32_F32_F32_F32,	},
	}
};

static qb_vector_op_factory factory_dot_product = {
	qb_select_opcode_vector,
	qb_set_result_dimensions_vector,
	QB_COERCE_TO_HIGHEST_RANK | QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION | QB_TYPE_OPERAND,
	{	QB_DOT_F64_F64_U32_F64,		QB_DOT_F32_F32_U32_F32,	},
	{
		{	QB_DOT_2X_F64_F64_F64,	QB_DOT_2X_F32_F32_F32,	},
		{	QB_DOT_3X_F64_F64_F64,	QB_DOT_3X_F32_F32_F32,	},
		{	QB_DOT_4X_F64_F64_F64,	QB_DOT_4X_F32_F32_F32,	},
	},
};

static qb_vector_op_factory factory_length = {
	qb_select_opcode_vector,
	qb_set_result_dimensions_vector,
	QB_COERCE_TO_HIGHEST_RANK | QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION | QB_TYPE_OPERAND,
	{	QB_LEN_F64_U32_F64,			QB_LEN_F32_U32_F32,	},
	{
		{	QB_LEN_2X_F64_F64,		QB_LEN_2X_F32_F32,	},
		{	QB_LEN_3X_F64_F64,		QB_LEN_3X_F32_F32,	},
		{	QB_LEN_4X_F64_F64,		QB_LEN_4X_F32_F32,	},
	},
};

static qb_vector_op_factory factory_distance = {
	qb_select_opcode_vector,
	qb_set_result_dimensions_vector,
	QB_COERCE_TO_HIGHEST_RANK | QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION | QB_TYPE_OPERAND,
	{	QB_DIS_F64_F64_U32_F64,		QB_DIS_F32_F32_U32_F32,	},
	{
		{	QB_DIS_2X_F64_F64_F64,	QB_DIS_2X_F32_F32_F32,	},
		{	QB_DIS_3X_F64_F64_F64,	QB_DIS_3X_F32_F32_F32,	},
		{	QB_DIS_4X_F64_F64_F64,	QB_DIS_4X_F32_F32_F32,	},
	},
};

static qb_vector_op_factory factory_normalize = {
	qb_select_opcode_vector,
	qb_set_result_dimensions_matching,
	QB_COERCE_TO_HIGHEST_RANK | QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION | QB_TYPE_OPERAND,
	{	QB_NORM_F64_U32_F64,		QB_NORM_F32_U32_F32,	},
	{
		{	QB_NORM_2X_F64_F64,		QB_NORM_2X_F32_F32,	},
		{	QB_NORM_3X_F64_F64,		QB_NORM_3X_F32_F32,	},
		{	QB_NORM_4X_F64_F64,		QB_NORM_4X_F32_F32,	},
	},
};

static qb_vector_op_factory factory_faceforward = { 
	qb_select_opcode_vector,
	qb_set_result_dimensions_matching,
	QB_COERCE_TO_HIGHEST_RANK | QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION | QB_TYPE_OPERAND,
	{	QB_FORE_F64_F64_U32_F64,		QB_FORE_F32_F32_U32_F32,	},
	{
		{	QB_FORE_2X_F64_F64_F64,		QB_FORE_2X_F32_F32_F32,	},
		{	QB_FORE_3X_F64_F64_F64,		QB_FORE_3X_F32_F32_F32,	},
		{	QB_FORE_4X_F64_F64_F64,		QB_FORE_4X_F32_F32_F32,	},
	},
};

static qb_vector_op_factory factory_reflect = { 
	qb_select_opcode_vector,
	qb_set_result_dimensions_matching,
	QB_COERCE_TO_HIGHEST_RANK | QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION | QB_TYPE_OPERAND,
	{	QB_REFL_F64_F64_U32_F64,		QB_REFL_F32_F32_U32_F32,	},
	{
		{	QB_REFL_2X_F64_F64_F64,		QB_REFL_2X_F32_F32_F32,	},
		{	QB_REFL_3X_F64_F64_F64,		QB_REFL_3X_F32_F32_F32,	},
		{	QB_REFL_4X_F64_F64_F64,		QB_REFL_4X_F32_F32_F32,	},
	},
};

static qb_vector_op_factory factory_refract = { 
	qb_select_opcode_vector,
	qb_set_result_dimensions_matching,
	QB_COERCE_TO_HIGHEST_RANK | QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION | QB_TYPE_OPERAND,
	{	QB_REFR_F64_F64_U32_U32_F64,		QB_REFR_F32_F32_U32_U32_F32,	},
	{
		{	QB_REFR_2X_F64_F64_F64_F64,		QB_REFR_2X_F32_F32_F32_F32,	},
		{	QB_REFR_3X_F64_F64_F64_F64,		QB_REFR_3X_F32_F32_F32_F32,	},
		{	QB_REFR_4X_F64_F64_F64_F64,		QB_REFR_4X_F32_F32_F32_F32,	},
	},
};

static qb_vector_op_factory factory_cross_product = {
	qb_select_opcode_vector,
	qb_set_result_dimensions_matching,
	QB_COERCE_TO_HIGHEST_RANK | QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION | QB_TYPE_OPERAND,
	{	QB_NOP,		QB_NOP,	},
	{
		{	QB_CROSS_2X_F64_F64_F64,		QB_CROSS_2X_F32_F32_F32,		},
		{	QB_CROSS_3X_F64_F64_F64,		QB_CROSS_3X_F32_F32_F32,		},
		{	QB_CROSS_4X_F64_F64_F64_F64,	QB_CROSS_4X_F32_F32_F32_F32,	},
	},
};

static qb_matrix_op_factory factory_mm_multiply_cm = { 
	qb_select_opcode_mm_mult_cm,
	qb_set_result_dimensions_mm_mult,
	QB_COERCE_TO_HIGHEST_RANK | QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION | QB_RESULT_IS_COLUMN_MAJOR | QB_TYPE_OPERAND,
	{	QB_MUL_MM_CM_F64_F64_U32_U32_U32_F64,	QB_MUL_MM_CM_F32_F32_U32_U32_U32_F32,	},
	{
		{	QB_MUL_MM_CM_2X_F64_F64_F64,		QB_MUL_MM_CM_2X_F32_F32_F32,	},
		{	QB_MUL_MM_CM_3X_F64_F64_F64,		QB_MUL_MM_CM_3X_F32_F32_F32,	},
		{	QB_MUL_MM_CM_4X_F64_F64_F64,		QB_MUL_MM_CM_4X_F32_F32_F32,	},
	},
	QB_MUL_MM_CM_PAD_3X_F32_F32_F32,
};

static qb_matrix_op_factory factory_mv_multiply_cm = { 
	qb_select_opcode_mv_mult_cm,
	qb_set_result_dimensions_mv_mult,
	QB_COERCE_TO_HIGHEST_RANK | QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION | QB_RESULT_IS_COLUMN_MAJOR | QB_TYPE_OPERAND,
	{	QB_MUL_MV_CM_F64_F64_U32_U32_F64,		QB_MUL_MV_CM_F32_F32_U32_U32_F32,	},
	{
		{	QB_MUL_MV_CM_2X_F64_F64_F64,		QB_MUL_MV_CM_2X_F32_F32_F32,	},
		{	QB_MUL_MV_CM_3X_F64_F64_F64,		QB_MUL_MV_CM_3X_F32_F32_F32,	},
		{	QB_MUL_MV_CM_4X_F64_F64_F64,		QB_MUL_MV_CM_4X_F32_F32_F32,	},
	},
	QB_MUL_MV_CM_PAD_3X_F32_F32_F32,
};

static qb_matrix_op_factory factory_vm_multiply_cm = { 
	qb_select_opcode_vm_mult_cm,
	qb_set_result_dimensions_vm_mult,
	QB_COERCE_TO_HIGHEST_RANK | QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION | QB_RESULT_IS_COLUMN_MAJOR | QB_TYPE_OPERAND,
	{	QB_MUL_VM_CM_F64_F64_U32_U32_F64,		QB_MUL_VM_CM_F32_F32_U32_U32_F32,	},
	{
		{	QB_MUL_VM_CM_2X_F64_F64_F64,		QB_MUL_VM_CM_2X_F32_F32_F32,	},
		{	QB_MUL_VM_CM_3X_F64_F64_F64,		QB_MUL_VM_CM_3X_F32_F32_F32,	},
		{	QB_MUL_VM_CM_4X_F64_F64_F64,		QB_MUL_VM_CM_4X_F32_F32_F32,	},
	},
	QB_MUL_VM_CM_PAD_3X_F32_F32_F32,
};

static qb_equivalent_matrix_op_factory factory_mm_multiply_rm = { 
	qb_select_opcode_matrix_unary_equivalent,
	qb_set_result_dimensions_matrix_equivalent,
	QB_COERCE_TO_HIGHEST_RANK | QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION | QB_RESULT_IS_ROW_MAJOR | QB_TYPE_OPERAND,
	&factory_mm_multiply_cm,
};

static qb_equivalent_matrix_op_factory factory_mv_multiply_rm = { 
	qb_select_opcode_matrix_unary_equivalent,
	qb_set_result_dimensions_matrix_equivalent,
	QB_COERCE_TO_HIGHEST_RANK | QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION | QB_RESULT_IS_ROW_MAJOR | QB_TYPE_OPERAND,
	&factory_vm_multiply_cm,
};

static qb_equivalent_matrix_op_factory factory_vm_multiply_rm = { 
	qb_select_opcode_matrix_unary_equivalent,
	qb_set_result_dimensions_matrix_equivalent,
	QB_COERCE_TO_HIGHEST_RANK | QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION | QB_RESULT_IS_ROW_MAJOR | QB_TYPE_OPERAND,
	&factory_mv_multiply_cm,
};

static qb_matrix_op_factory_selector factory_mm_multiply = { 
	qb_select_opcode_matrix_unary_current_mode,
	qb_set_result_dimensions_matrix_current_mode,
	QB_COERCE_TO_HIGHEST_RANK | QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION | QB_TYPE_OPERAND,
	(qb_op_factory *) &factory_mm_multiply_cm,
	(qb_op_factory *) &factory_mm_multiply_rm,
};

static qb_matrix_op_factory_selector factory_mv_multiply = { 
	qb_select_opcode_matrix_unary_current_mode,
	qb_set_result_dimensions_matrix_current_mode,
	QB_COERCE_TO_HIGHEST_RANK | QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION | QB_TYPE_OPERAND,
	(qb_op_factory *) &factory_mv_multiply_cm,
	(qb_op_factory *) &factory_mv_multiply_rm,
};

static qb_matrix_op_factory_selector factory_vm_multiply = { 
	qb_select_opcode_matrix_unary_current_mode,
	qb_set_result_dimensions_matrix_current_mode,
	QB_COERCE_TO_HIGHEST_RANK | QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION | QB_TYPE_OPERAND,
	(qb_op_factory *) &factory_vm_multiply_cm,
	(qb_op_factory *) &factory_vm_multiply_rm,
};

static qb_matrix_op_factory factory_transpose = { 
	qb_select_opcode_matrix_unary,
	qb_set_result_dimensions_transpose,
	QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION | QB_TYPE_OPERAND,
	{	QB_MTRAN_F64_U32_U32_F64,		QB_MTRAN_F32_U32_U32_F32,	},
	{
		{	QB_MTRAN_2X_F64_F64,		QB_MTRAN_2X_F32_F32,	},
		{	QB_MTRAN_3X_F64_F64,		QB_MTRAN_3X_F32_F32,	},
		{	QB_MTRAN_4X_F64_F64,		QB_MTRAN_4X_F32_F32,	},
	},
	0,
};

static qb_matrix_op_factory factory_inverse = { 
	qb_select_opcode_matrix_unary,
	qb_set_result_dimensions_matching,
	QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION | QB_TYPE_OPERAND,
	{	QB_MINV_F64_U32_F64,		QB_MINV_F32_U32_F32,	},
	{
		{	QB_MINV_2X_F64_F64,		QB_MINV_2X_F32_F32,	},
		{	QB_MINV_3X_F64_F64,		QB_MINV_3X_F32_F32,	},
		{	QB_MINV_4X_F64_F64,		QB_MINV_4X_F32_F32,	},
	},
	0,
};

static qb_matrix_op_factory factory_determinant = { 
	qb_select_opcode_matrix_unary,
	qb_set_result_dimensions_matrix_unary,
	QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION | QB_TYPE_OPERAND,
	{	QB_MDET_F64_U32_F64,			QB_MDET_F32_U32_F32,	},
	{
		{	QB_MDET_2X_F64_F64,		QB_MDET_2X_F32_F32,	},
		{	QB_MDET_3X_F64_F64,		QB_MDET_3X_F32_F32,	},
		{	QB_MDET_4X_F64_F64,		QB_MDET_4X_F32_F32,	},
	},
	0,
};

static qb_matrix_op_factory factory_transform_cm = {
	qb_select_opcode_transform_cm,
	qb_set_result_dimensions_mv_mult,
	QB_COERCE_TO_HIGHEST_RANK | QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION | QB_RESULT_IS_COLUMN_MAJOR | QB_TYPE_OPERAND,
	{	QB_NOP,			QB_NOP,	},
	{
		{	QB_TRAN_CM_2X_F64_F64_F64,		QB_TRAN_CM_2X_F32_F32_F32,	},
		{	QB_TRAN_CM_3X_F64_F64_F64,		QB_TRAN_CM_3X_F32_F32_F32,	},
		{	QB_TRAN_CM_4X_F64_F64_F64,		QB_TRAN_CM_4X_F32_F32_F32,	},
	},
	0,
};

static qb_matrix_op_factory factory_transform_rm = {
	qb_select_opcode_transform_rm,
	qb_set_result_dimensions_mv_mult,
	QB_COERCE_TO_HIGHEST_RANK | QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION | QB_RESULT_IS_COLUMN_MAJOR | QB_TYPE_OPERAND,
	{	0,			0,	},
	{
		{	QB_TRAN_RM_2X_F64_F64_F64,		QB_TRAN_RM_2X_F32_F32_F32,	},
		{	QB_TRAN_RM_3X_F64_F64_F64,		QB_TRAN_RM_3X_F32_F32_F32,	},
		{	QB_TRAN_RM_4X_F64_F64_F64,		QB_TRAN_RM_4X_F32_F32_F32,	},
	},
	0,
};

static qb_matrix_op_factory_selector factory_transform = {
	qb_select_opcode_matrix_unary_current_mode,
	qb_set_result_dimensions_matrix_current_mode,
	QB_COERCE_TO_HIGHEST_RANK | QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION | QB_TYPE_OPERAND,
	(qb_op_factory *) &factory_transform_cm,
	(qb_op_factory *) &factory_transform_rm,
};

static qb_float_op_factory factory_alpha_blend = {
	qb_select_opcode_basic,
	qb_set_result_dimensions_matching,
	QB_COERCE_TO_HIGHEST_RANK | QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION | QB_TYPE_OPERAND,
	{	QB_BLEND_F64_F64_F64,		QB_BLEND_F32_F32_F32,	},
};

/*
static qb_op * ZEND_FASTCALL qb_append_utf8_decode(qb_compiler_context *cxt, void *factory, qb_operand *operands, uint32_t operand_count, qb_operand *result) {
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
static qb_basic_op_factory factory_utf8_decode = {
	qb_append_utf8_decode,
	NULL,
	0,
	0,
	{	0, 0, 0, 0, QB_UTF8_DEC_U08_U32,	QB_UTF8_DEC_U08_U32,	QB_UTF8_DEC_U08_U16,	QB_UTF8_DEC_U08_U16,	},
};
*/

/*
static qb_basic_op_factory factory_utf8_encode = {
	qb_append_utf8_encode,
	NULL,
	0,
	0,
	{	0, 0, 0, 0, QB_UTF8_ENC_U32_U08,	QB_UTF8_ENC_U32_U08,	QB_UTF8_ENC_U16_U08,	QB_UTF8_ENC_U16_U08,	},
};
*/

/*
static qb_basic_op_factory factory_pack_le = { 
	qb_select_opcode_basic,
	NULL,
	0,
	QB_RESULT_FROM_PURE_FUNCTION,
	{	QB_PACK_LE_F64_U08,	QB_PACK_LE_F32_U08,	QB_PACK_LE_I64_U08,	QB_PACK_LE_I64_U08,	QB_PACK_LE_I32_U08,	QB_PACK_LE_I32_U08,	QB_PACK_LE_I16_U08,	QB_PACK_LE_I16_U08,	},
};

static qb_basic_op_factory factory_pack_be = {
	qb_select_opcode_basic,
	NULL,
	0,
	QB_RESULT_FROM_PURE_FUNCTION,
	{	QB_PACK_BE_F64_U08 ,	QB_PACK_BE_F32_U08,	QB_PACK_BE_I64_U08,	QB_PACK_BE_I64_U08,	QB_PACK_BE_I32_U08,	QB_PACK_BE_I32_U08,	QB_PACK_BE_I16_U08,	QB_PACK_BE_I16_U08,	},
};
*/

/*
static qb_basic_op_factory factory_unpack_le = {
	qb_select_opcode_basic_result,
	NULL,
	0,
	QB_RESULT_FROM_PURE_FUNCTION,
	{	QB_UNPACK_LE_U08_F64,	QB_UNPACK_LE_U08_F32,	QB_UNPACK_LE_U08_I64,	QB_UNPACK_LE_U08_I64,	QB_UNPACK_LE_U08_I32,	QB_UNPACK_LE_U08_I32,	QB_UNPACK_LE_U08_I16,	QB_UNPACK_LE_U08_I16	},
};

static qb_basic_op_factory factory_unpack_be = {
	qb_select_opcode_basic_result,
	NULL,
	0,
	QB_RESULT_FROM_PURE_FUNCTION,
	{	QB_UNPACK_BE_U08_F64,	QB_UNPACK_BE_U08_F32,	QB_UNPACK_BE_U08_I64,	QB_UNPACK_BE_U08_I64,	QB_UNPACK_BE_U08_I32,	QB_UNPACK_BE_U08_I32,	QB_UNPACK_BE_U08_I16,	QB_UNPACK_BE_U08_I16	},
};
*/

static qb_basic_op_factory factory_sort = {
	qb_select_opcode_basic,
	NULL,
	0,
	0,
	{	QB_SORT_U32_F64 ,	QB_SORT_U32_F32,	QB_SORT_U32_U64,	QB_SORT_U32_S64,	QB_SORT_U32_U32,	QB_SORT_U32_S32,	QB_SORT_U32_U16,	QB_SORT_U32_S16,	QB_SORT_U32_U08,	QB_SORT_U32_S08	},
};

static qb_basic_op_factory factory_rsort = {
	qb_select_opcode_basic,
	NULL,
	0,
	0,
	{	QB_RSORT_U32_F64,	QB_RSORT_U32_F32,	QB_RSORT_U32_U64,	QB_RSORT_U32_S64,	QB_RSORT_U32_U32,	QB_RSORT_U32_S32,	QB_RSORT_U32_U16,	QB_RSORT_U32_S16,	QB_RSORT_U32_U08,	QB_RSORT_U32_S08	},
};

static qb_basic_op_factory factory_shuffle = {
	qb_select_opcode_basic,
	NULL,
	0,
	0,
	{	QB_SHUFFLE_U32_F64,	QB_SHUFFLE_U32_F32,	QB_SHUFFLE_U32_I64,	QB_SHUFFLE_U32_I64,	QB_SHUFFLE_U32_I32,	QB_SHUFFLE_U32_I32,	QB_SHUFFLE_U32_I16,	QB_SHUFFLE_U32_I16,	QB_SHUFFLE_U32_I08,	QB_SHUFFLE_U32_I08	},
};

static qb_basic_op_factory factory_array_sum = { 
	qb_select_opcode_basic,
	NULL,
	0,
	QB_RESULT_FROM_PURE_FUNCTION | QB_TYPE_OPERAND,
	{	QB_ASUM_F64_F64,	QB_ASUM_F32_F32,	QB_ASUM_U64_U64,	QB_ASUM_S64_S64,	QB_ASUM_U32_U32,	QB_ASUM_S32_S32,	QB_ASUM_U16_U16,	QB_ASUM_S16_S16,	QB_ASUM_U08_U08,	QB_ASUM_S08_S08	},
};

static qb_basic_op_factory factory_array_product = {
	qb_select_opcode_basic,
	NULL,
	0,
	QB_RESULT_FROM_PURE_FUNCTION | QB_TYPE_OPERAND,
	{	QB_APROD_F64_F64,	QB_APROD_F32_F32,	QB_APROD_U64_U64,	QB_APROD_S64_S64,	QB_APROD_U32_U32,	QB_APROD_S32_S32,	QB_APROD_U16_U16,	QB_APROD_S16_S16,	QB_APROD_U08_U08,	QB_APROD_S08_S08	},
};

/*
static qb_basic_op_factory factory_array_search = { 
	qb_select_opcode_basic,
	NULL,
	QB_COERCE_TO_SECOND_OPERAND_TYPE,
	QB_TYPE_I32,
	{	QB_AFIND_IDX_F64_F64_I32,	QB_AFIND_IDX_F32_F32_I32,	QB_AFIND_IDX_I64_I64_I32,	QB_AFIND_IDX_I64_I64_I32,	QB_AFIND_IDX_I32_I32_I32,	QB_AFIND_IDX_I32_I32_I32,	QB_AFIND_IDX_I16_I16_I32,	QB_AFIND_IDX_I16_I16_I32,	QB_AFIND_IDX_I08_I08_I32,	QB_AFIND_IDX_I08_I08_I32	},
};
*/

static qb_copy_op_factory factory_array_merge = {
	qb_select_copy_opcode,
	qb_set_result_dimensions_array_merge,
	QB_COERCE_TO_HIGHEST_RANK,
	QB_TYPE_OPERAND,
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

static qb_copy_op_factory factory_array_fill = {
	qb_select_copy_opcode,
	qb_set_result_dimensions_array_fill,
	0,
	QB_TYPE_OPERAND,
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

static qb_basic_op_factory factory_subarray_pos = {
	qb_select_opcode_basic,
	NULL,
	QB_COERCE_TO_FIRST_OPERAND_TYPE,
	QB_RESULT_FROM_PURE_FUNCTION | QB_TYPE_I32,
	{	QB_APOS_F64_F64_I32_I32,	QB_APOS_F32_F32_I32_I32,	QB_APOS_I64_I64_I32_I32,	QB_APOS_I64_I64_I32_I32,	QB_APOS_I32_I32_I32_I32,	QB_APOS_I32_I32_I32_I32,	QB_APOS_I16_I16_I32_I32,	QB_APOS_I16_I16_I32_I32,	QB_APOS_I08_I08_I32_I32,	QB_APOS_I08_I08_I32_I32	},
};

static qb_basic_op_factory factory_subarray_rpos = {
	qb_select_opcode_basic,
	NULL,
	QB_COERCE_TO_FIRST_OPERAND_TYPE,
	QB_RESULT_FROM_PURE_FUNCTION | QB_TYPE_I32,
	{	QB_ARPOS_F64_F64_I32_I32,	QB_ARPOS_F32_F32_I32_I32,	QB_ARPOS_I64_I64_I32_I32,	QB_ARPOS_I64_I64_I32_I32,	QB_ARPOS_I32_I32_I32_I32,	QB_ARPOS_I32_I32_I32_I32,	QB_ARPOS_I16_I16_I32_I32,	QB_ARPOS_I16_I16_I32_I32,	QB_ARPOS_I08_I08_I32_I32,	QB_ARPOS_I08_I08_I32_I32	},
};

static qb_basic_op_factory factory_array_reverse = {
	qb_select_opcode_basic,
	qb_set_result_dimensions_matching,
	QB_COERCE_TO_FIRST_OPERAND_TYPE,
	QB_RESULT_FROM_PURE_FUNCTION | QB_TYPE_OPERAND,
	{	QB_AREV_F64_U32_F64,	QB_AREV_F32_U32_F32,	QB_AREV_I64_U32_I64,	QB_AREV_I64_U32_I64,	QB_AREV_I32_U32_I32,	QB_AREV_I32_U32_I32,	QB_AREV_I16_U32_I16,	QB_AREV_I16_U32_I16,	QB_AREV_I08_U32_I08,	QB_AREV_I08_U32_I08		},
};

static qb_basic_op_factory factory_array_column = {
	qb_select_opcode_basic,
	qb_set_result_dimensions_array_column,
	0,
	0,
	{	QB_ACOL_F64_U32_U32_U32_F64,	QB_ACOL_F32_U32_U32_U32_F32,	QB_ACOL_I64_U32_U32_U32_I64,	QB_ACOL_I64_U32_U32_U32_I64,	QB_ACOL_I32_U32_U32_U32_I32,	QB_ACOL_I32_U32_U32_U32_I32,	QB_ACOL_I16_U32_U32_U32_I16,	QB_ACOL_I16_U32_U32_U32_I16,	QB_ACOL_I08_U32_U32_U32_I08,	QB_ACOL_I08_U32_U32_U32_I08	},
};

static qb_basic_op_factory factory_array_diff = {
	qb_select_opcode_basic,
	qb_set_result_dimensions_array_pad,
	QB_COERCE_TO_FIRST_OPERAND_TYPE,
	QB_TYPE_OPERAND,
	{	QB_ADIFF_F64_F64_U32_F64,	QB_ADIFF_F32_F32_U32_F32,	QB_ADIFF_I64_I64_U32_I64,	QB_ADIFF_I64_I64_U32_I64,	QB_ADIFF_I32_I32_U32_I32,	QB_ADIFF_I32_I32_U32_I32,	QB_ADIFF_I16_I16_U32_I16,	QB_ADIFF_I16_I16_U32_I16,	QB_ADIFF_I08_I08_U32_I08,	QB_ADIFF_I08_I08_U32_I08	},
};

static qb_basic_op_factory factory_array_intersect = {
	qb_select_opcode_basic,
	qb_set_result_dimensions_array_pad,
	QB_COERCE_TO_FIRST_OPERAND_TYPE,
	QB_TYPE_OPERAND,
	{	QB_AISECT_F64_F64_U32_F64,	QB_AISECT_F32_F32_U32_F32,	QB_AISECT_I64_I64_U32_I64,	QB_AISECT_I64_I64_U32_I64,	QB_AISECT_I32_I32_U32_I32,	QB_AISECT_I32_I32_U32_I32,	QB_AISECT_I16_I16_U32_I16,	QB_AISECT_I16_I16_U32_I16,	QB_AISECT_I08_I08_U32_I08,	QB_AISECT_I08_I08_U32_I08	},
};

static qb_basic_op_factory factory_range = {
	qb_select_opcode_basic,
	qb_set_result_dimensions_range,
	QB_COERCE_TO_HIGHEST_RANK | QB_COERCE_TO_LVALUE_TYPE,
	QB_RESULT_FROM_PURE_FUNCTION | QB_TYPE_OPERAND,
	{	QB_RANGE_F64_F64_F64_F64,	QB_RANGE_F32_F32_F32_F32,	QB_RANGE_U64_U64_S64_U64,	QB_RANGE_S64_S64_S64_S64,	QB_RANGE_U32_U32_S32_U32,	QB_RANGE_S32_S32_S32_S32,	QB_RANGE_U16_U16_S16_U16,	QB_RANGE_S16_S16_S16_S16,	QB_RANGE_U08_U08_S08_U08,	QB_RANGE_S08_S08_S08_S08	},
};

static qb_basic_op_factory factory_array_unique = {
	qb_select_opcode_basic,
	qb_set_result_dimensions_array_pad,
	QB_COERCE_TO_FIRST_OPERAND_TYPE,
	QB_RESULT_FROM_PURE_FUNCTION | QB_TYPE_OPERAND,
	{	QB_AUNIQ_F64_U32_F64,	QB_AUNIQ_F32_U32_F32,	QB_AUNIQ_I64_U32_I64,	QB_AUNIQ_I64_U32_I64,	QB_AUNIQ_I32_U32_I32,	QB_AUNIQ_I32_U32_I32,	QB_AUNIQ_I16_U32_I16,	QB_AUNIQ_I16_U32_I16,	QB_AUNIQ_I08_U32_I08,	QB_AUNIQ_I08_U32_I08	},
};

static qb_simple_op_factory factory_array_rand = {
	qb_select_opcode_simple,
	qb_set_result_dimensions_array_rand,
	0,
	QB_RESULT_FROM_PURE_FUNCTION | QB_TYPE_OPERAND,
	QB_ARAND_U32_U32_U32,
};

/*
static qb_basic_op_factory factory_array_resize = {
	qb_append_array_resize,
	NULL,
	0,
	0,
	{	QB_ARESIZE_F64,	QB_ARESIZE_F32,	QB_ARESIZE_I64,	QB_ARESIZE_I64,	QB_ARESIZE_I32,	QB_ARESIZE_I32,	QB_ARESIZE_I16,	QB_ARESIZE_I16,	QB_ARESIZE_I08,	QB_ARESIZE_I08	},
};
*/

static qb_float_op_factory factory_complex_abs = {
	qb_select_opcode_complex_number,
	qb_set_result_dimensions_vector,
	QB_COERCE_TO_HIGHEST_RANK | QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION | QB_TYPE_OPERAND,
	{	QB_CABS_F64_F64,	QB_CABS_F32_F32	},
};

static qb_float_op_factory factory_complex_arg = {
	qb_select_opcode_complex_number,
	qb_set_result_dimensions_vector,
	QB_COERCE_TO_HIGHEST_RANK | QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION | QB_TYPE_OPERAND,
	{	QB_CARG_F64_F64,	QB_CARG_F32_F32	},
};


static qb_float_op_factory factory_complex_multiply = {
	qb_select_opcode_complex_number,
	qb_set_result_dimensions_matching,
	QB_COERCE_TO_HIGHEST_RANK | QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION | QB_TYPE_OPERAND,
	{	QB_CMUL_F64_F64_F64,	QB_CMUL_F32_F32_F32	},
};

static qb_float_op_factory factory_complex_divide = {
	qb_select_opcode_complex_number,
	qb_set_result_dimensions_matching,
	QB_COERCE_TO_HIGHEST_RANK | QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION | QB_TYPE_OPERAND,
	{	QB_CDIV_F64_F64_F64,	QB_CDIV_F32_F32_F32	},
};

static qb_float_op_factory factory_complex_exp = {
	qb_select_opcode_complex_number,
	qb_set_result_dimensions_matching,
	QB_COERCE_TO_HIGHEST_RANK | QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION | QB_TYPE_OPERAND,
	{	QB_CEXP_F64_F64,	QB_CEXP_F32_F32	},
};

static qb_float_op_factory factory_complex_log = {
	qb_select_opcode_complex_number,
	qb_set_result_dimensions_matching,
	QB_COERCE_TO_HIGHEST_RANK | QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION | QB_TYPE_OPERAND,
	{	QB_CLOG_F64_F64,	QB_CLOG_F32_F32	},
};

static qb_float_op_factory factory_complex_sqrt = {
	qb_select_opcode_complex_number,
	qb_set_result_dimensions_matching,
	QB_COERCE_TO_HIGHEST_RANK | QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION | QB_TYPE_OPERAND,
	{	QB_CSQRT_F64_F64,	QB_CSQRT_F32_F32	},
};

static qb_float_op_factory factory_complex_pow = {
	qb_select_opcode_complex_number,
	qb_set_result_dimensions_matching,
	QB_COERCE_TO_HIGHEST_RANK | QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION | QB_TYPE_OPERAND,
	{	QB_CPOW_F64_F64_F64,	QB_CPOW_F32_F32_F32	},
};

static qb_float_op_factory factory_complex_sin = {
	qb_select_opcode_complex_number,
	qb_set_result_dimensions_matching,
	QB_COERCE_TO_HIGHEST_RANK | QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION | QB_TYPE_OPERAND,
	{	QB_CSIN_F64_F64,	QB_CSIN_F32_F32	},
};

static qb_float_op_factory factory_complex_cos = {
	qb_select_opcode_complex_number,
	qb_set_result_dimensions_matching,
	QB_COERCE_TO_HIGHEST_RANK | QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION | QB_TYPE_OPERAND,
	{	QB_CCOS_F64_F64,	QB_CCOS_F32_F32	},
};

static qb_float_op_factory factory_complex_tan = {
	qb_select_opcode_complex_number,
	qb_set_result_dimensions_matching,
	QB_COERCE_TO_HIGHEST_RANK | QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION | QB_TYPE_OPERAND,
	{	QB_CTAN_F64_F64,	QB_CTAN_F32_F32	},
};

static qb_float_op_factory factory_complex_sinh = {
	qb_select_opcode_complex_number,
	qb_set_result_dimensions_matching,
	QB_COERCE_TO_HIGHEST_RANK | QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION | QB_TYPE_OPERAND,
	{	QB_CSINH_F64_F64,	QB_CSINH_F32_F32	},
};

static qb_float_op_factory factory_complex_cosh = {
	qb_select_opcode_complex_number,
	qb_set_result_dimensions_matching,
	QB_COERCE_TO_HIGHEST_RANK | QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION | QB_TYPE_OPERAND,
	{	QB_CCOSH_F64_F64,	QB_CCOSH_F32_F32	},
};

static qb_float_op_factory factory_complex_tanh = {
	qb_select_opcode_complex_number,
	qb_set_result_dimensions_matching,
	QB_COERCE_TO_HIGHEST_RANK | QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION | QB_TYPE_OPERAND,
	{	QB_CTANH_F64_F64,	QB_CTANH_F32_F32	},
};

static qb_pixel_op_factory factory_apply_premult = {
	qb_select_opcode_pixel,
	qb_set_result_dimensions_matching,
	QB_COERCE_TO_FLOATING_POINT,
	QB_RESULT_FROM_PURE_FUNCTION | QB_TYPE_OPERAND,
	{
		{	0,							0						},
		{	QB_PREMULT_F64_F64,			QB_PREMULT_F32_F32		},
	}
};

static qb_pixel_op_factory factory_remove_premult = {
	qb_select_opcode_pixel,
	qb_set_result_dimensions_matching,
	QB_COERCE_TO_FLOATING_POINT,
	QB_RESULT_FROM_PURE_FUNCTION | QB_TYPE_OPERAND,
	{
		{	0,							0						},
		{	QB_UNPREMULT_F64_F64,		QB_UNPREMULT_F32_F32	},
	}
};

static qb_pixel_op_factory factory_rgb2hsv = {
	qb_select_opcode_pixel,
	qb_set_result_dimensions_matching,
	QB_COERCE_TO_FLOATING_POINT,
	QB_RESULT_FROM_PURE_FUNCTION | QB_TYPE_OPERAND,
	{
		{	QB_RGB2HSV_3X_F64_F64,		QB_RGB2HSV_3X_F32_F32	},
		{	QB_RGB2HSV_4X_F64_F64,		QB_RGB2HSV_4X_F32_F32	},
	}
};

static qb_pixel_op_factory factory_hsv2rgb = {
	qb_select_opcode_pixel,
	qb_set_result_dimensions_matching,
	QB_COERCE_TO_FLOATING_POINT,
	QB_RESULT_FROM_PURE_FUNCTION | QB_TYPE_OPERAND,
	{
		{	QB_HSV2RGB_3X_F64_F64,		QB_HSV2RGB_3X_F32_F32	},
		{	QB_HSV2RGB_4X_F64_F64,		QB_HSV2RGB_4X_F32_F32	},
	}
};

static qb_pixel_op_factory factory_rgb2hsl = {
	qb_select_opcode_pixel,
	qb_set_result_dimensions_matching,
	QB_COERCE_TO_FLOATING_POINT,
	QB_RESULT_FROM_PURE_FUNCTION | QB_TYPE_OPERAND,
	{
		{	QB_RGB2HSL_3X_F64_F64,		QB_RGB2HSL_3X_F32_F32	},
		{	QB_RGB2HSL_4X_F64_F64,		QB_RGB2HSL_4X_F32_F32	},
	}
};

static qb_pixel_op_factory factory_hsl2rgb = {
	qb_select_opcode_pixel,
	qb_set_result_dimensions_matching,
	QB_COERCE_TO_FLOATING_POINT,
	QB_RESULT_FROM_PURE_FUNCTION | QB_TYPE_OPERAND,
	{
		{	QB_HSL2RGB_3X_F64_F64,		QB_HSL2RGB_3X_F32_F32	},
		{	QB_HSL2RGB_4X_F64_F64,		QB_HSL2RGB_4X_F32_F32	},
	}
};
