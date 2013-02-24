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

static uint32_t copy_opcodes[QB_TYPE_COUNT][QB_TYPE_COUNT] = {
	{ QB_MOV_I08_I08, QB_MOV_I08_I08, QB_MOV_S08_I16, QB_MOV_S08_I16, QB_MOV_S08_I32, QB_MOV_S08_I32, QB_MOV_S08_I64, QB_MOV_S08_I64, QB_MOV_S08_F32, QB_MOV_S08_F64 },
	{ QB_MOV_I08_I08, QB_MOV_I08_I08, QB_MOV_S08_I16, QB_MOV_U08_I16, QB_MOV_U08_I32, QB_MOV_U08_I32, QB_MOV_U08_I64, QB_MOV_U08_I64, QB_MOV_U08_F32, QB_MOV_U08_F64 },
	{ QB_MOV_I16_I08, QB_MOV_I16_I08, QB_MOV_I16_I16, QB_MOV_I16_I16, QB_MOV_S16_I32, QB_MOV_S16_I32, QB_MOV_S16_I64, QB_MOV_S16_I64, QB_MOV_S16_F32, QB_MOV_S16_F64 },
	{ QB_MOV_I16_I08, QB_MOV_I16_I08, QB_MOV_I16_I16, QB_MOV_I16_I16, QB_MOV_U16_I32, QB_MOV_U16_I32, QB_MOV_U16_I64, QB_MOV_U16_I64, QB_MOV_U16_F32, QB_MOV_U16_F64 },
	{ QB_MOV_I32_I08, QB_MOV_I32_I08, QB_MOV_I32_I16, QB_MOV_I32_I16, QB_MOV_I32_I32, QB_MOV_I32_I32, QB_MOV_S32_I64, QB_MOV_S32_I64, QB_MOV_S32_F32, QB_MOV_S32_F64 },
	{ QB_MOV_I32_I08, QB_MOV_I32_I08, QB_MOV_I32_I16, QB_MOV_I32_I16, QB_MOV_I32_I32, QB_MOV_I32_I32, QB_MOV_U32_I64, QB_MOV_U32_I64, QB_MOV_U32_F32, QB_MOV_U32_F64 },
	{ QB_MOV_I64_I08, QB_MOV_I64_I08, QB_MOV_I64_I16, QB_MOV_I64_I16, QB_MOV_I64_I32, QB_MOV_I64_I32, QB_MOV_I64_I64, QB_MOV_I64_I64, QB_MOV_S64_F32, QB_MOV_S64_F64 },
	{ QB_MOV_I64_I08, QB_MOV_I64_I08, QB_MOV_I64_I16, QB_MOV_I64_I16, QB_MOV_I64_I32, QB_MOV_I64_I32, QB_MOV_I64_I64, QB_MOV_I64_I64, QB_MOV_U64_F32, QB_MOV_U64_F64 },
	{ QB_MOV_F32_S08, QB_MOV_F32_U08, QB_MOV_F32_S16, QB_MOV_F32_U16, QB_MOV_F32_S32, QB_MOV_F32_U32, QB_MOV_F32_S64, QB_MOV_F32_U64, QB_MOV_F32_F32, QB_MOV_F32_F64 },
	{ QB_MOV_F64_S08, QB_MOV_F64_U08, QB_MOV_F64_S16, QB_MOV_F64_U16, QB_MOV_F64_S32, QB_MOV_F64_U32, QB_MOV_F64_S64, QB_MOV_F64_U64, QB_MOV_F64_F32, QB_MOV_F64_F64 }
};

static uint32_t increment_opcodes[] = { QB_INC_I08, QB_INC_I08, QB_INC_I16, QB_INC_I16, QB_INC_I32, QB_INC_I32, QB_INC_I64, QB_INC_I64, QB_INC_F32, QB_INC_F64 };
static uint32_t decrement_opcodes[] = { QB_DEC_I08, QB_DEC_I08, QB_DEC_I16, QB_DEC_I16, QB_DEC_I32, QB_DEC_I32, QB_DEC_I64, QB_DEC_I64, QB_DEC_F32, QB_DEC_F64 };

static uint32_t bitwise_not_opcodes[]	= { QB_BW_NOT_I08_I08, QB_BW_NOT_I08_I08, QB_BW_NOT_I16_I16, QB_BW_NOT_I16_I16, QB_BW_NOT_I32_I32, QB_BW_NOT_I32_I32, QB_BW_NOT_I64_I64, QB_BW_NOT_I64_I64, 0, 0 };
static uint32_t logical_not_opcodes[] 	= { 0, 0, 0, 0, QB_NOT_I32_I32, QB_NOT_I32_I32, 0, 0, 0, 0 };

static uint32_t isset_opcodes[] 		= { QB_ISSET_I08_I32_ELV, QB_ISSET_I08_I32_ELV, QB_ISSET_I16_I32_ELV, QB_ISSET_I16_I32_ELV, QB_ISSET_I32_I32_ELV, QB_ISSET_I32_I32_ELV, QB_ISSET_I64_I32_ELV, QB_ISSET_I64_I32_ELV, QB_ISSET_F32_I32_ELV, QB_ISSET_F64_I32_ELV };
static uint32_t unset_opcodes[] 		= { QB_UNSET_I08, QB_UNSET_I08, QB_UNSET_I16, QB_UNSET_I16, QB_UNSET_I32, QB_UNSET_I32, QB_UNSET_I64, QB_UNSET_I64, QB_UNSET_F32, QB_UNSET_F64 };

static uint32_t add_opcodes[] 			= { QB_ADD_I08_I08_I08, QB_ADD_I08_I08_I08, QB_ADD_I16_I16_I16, QB_ADD_I16_I16_I16, QB_ADD_I32_I32_I32, QB_ADD_I32_I32_I32, QB_ADD_I64_I64_I64, QB_ADD_I64_I64_I64, QB_ADD_F32_F32_F32, QB_ADD_F64_F64_F64 };
static uint32_t subtract_opcodes[] 		= { QB_SUB_I08_I08_I08, QB_SUB_I08_I08_I08, QB_SUB_I16_I16_I16, QB_SUB_I16_I16_I16, QB_SUB_I32_I32_I32, QB_SUB_I32_I32_I32, QB_SUB_I64_I64_I64, QB_SUB_I64_I64_I64, QB_SUB_F32_F32_F32, QB_SUB_F64_F64_F64 };
static uint32_t multiply_opcodes[] 		= { QB_MUL_S08_S08_S08, QB_MUL_S08_S08_S08, QB_MUL_S16_S16_S16, QB_MUL_S16_S16_S16, QB_MUL_U32_U32_U32, QB_MUL_U32_U32_U32, QB_MUL_U64_U64_U64, QB_MUL_U64_U64_U64, QB_MUL_F32_F32_F32, QB_MUL_F64_F64_F64 };
static uint32_t divide_opcodes[] 		= { QB_DIV_S08_S08_S08, QB_DIV_U08_U08_U08, QB_DIV_S16_S16_S16, QB_DIV_U16_U16_U16, QB_DIV_S32_S32_S32, QB_DIV_U32_U32_U32, QB_DIV_S64_S64_S64, QB_DIV_U64_U64_U64, QB_DIV_F32_F32_F32, QB_DIV_F64_F64_F64 };
static uint32_t modulo_opcodes[] 		= { QB_MOD_S08_S08_S08, QB_MOD_U08_U08_U08, QB_MOD_S16_S16_S16, QB_MOD_U16_U16_U16, QB_MOD_S32_S32_S32, QB_MOD_U32_U32_U32, QB_MOD_S64_S64_S64, QB_MOD_U64_U64_U64, QB_MOD_F32_F32_F32, QB_MOD_F64_F64_F64 };
static uint32_t floor_modulo_opcodes[] 	= { 0, 0, 0, 0, 0, 0, 0, 0, QB_MOD_FLR_F32_F32_F32, QB_MOD_FLR_F64_F64_F64 };
static uint32_t shift_left_opcodes[] 	= { QB_SHL_S08_S08_S08, QB_SHL_U08_U08_U08, QB_SHL_S16_S16_S16, QB_SHL_U16_U16_U16, QB_SHL_S32_S32_S32, QB_SHL_U32_U32_U32, QB_SHL_S64_S64_S64, QB_SHL_U64_U64_U64, 0, 0 };
static uint32_t shift_right_opcodes[] 	= { QB_SHR_S08_S08_S08, QB_SHR_U08_U08_U08, QB_SHR_S16_S16_S16, QB_SHR_U16_U16_U16, QB_SHR_S32_S32_S32, QB_SHR_U32_U32_U32, QB_SHR_S64_S64_S64, QB_SHR_U64_U64_U64, 0, 0 };
static uint32_t bitwise_or_opcodes[] 	= { QB_BW_OR_I08_I08_I08, QB_BW_OR_I08_I08_I08, QB_BW_OR_I16_I16_I16, QB_BW_OR_I16_I16_I16, QB_BW_OR_I32_I32_I32, QB_BW_OR_I32_I32_I32, QB_BW_OR_I64_I64_I64, QB_BW_OR_I64_I64_I64, 0, 0 };
static uint32_t bitwise_and_opcodes[] 	= { QB_BW_AND_I08_I08_I08, QB_BW_AND_I08_I08_I08, QB_BW_AND_I16_I16_I16, QB_BW_AND_I16_I16_I16, QB_BW_AND_I32_I32_I32, QB_BW_AND_I32_I32_I32, QB_BW_AND_I64_I64_I64, QB_BW_AND_I64_I64_I64, 0, 0 };
static uint32_t bitwise_xor_opcodes[] 	= { QB_BW_XOR_I08_I08_I08, QB_BW_XOR_I08_I08_I08, QB_BW_XOR_I16_I16_I16, QB_BW_XOR_I16_I16_I16, QB_BW_XOR_I32_I32_I32, QB_BW_XOR_I32_I32_I32, QB_BW_XOR_I64_I64_I64, QB_BW_XOR_I64_I64_I64, 0, 0 };
static uint32_t logical_and_opcodes[] 	= { 0, 0, 0, 0, QB_AND_I32_I32_I32, QB_AND_I32_I32_I32, 0, 0, 0, 0 };
static uint32_t logical_or_opcodes[] 	= { 0, 0, 0, 0, QB_OR_I32_I32_I32,	QB_OR_I32_I32_I32,	0, 0, 0, 0 };
static uint32_t logical_xor_opcodes[] 	= { 0, 0, 0, 0, QB_XOR_I32_I32_I32, QB_XOR_I32_I32_I32, 0, 0, 0, 0 };

static uint32_t equal_opcodes[]			= {	QB_EQ_I08_I08_I32, QB_EQ_I08_I08_I32, QB_EQ_I16_I16_I32, QB_EQ_I16_I16_I32, QB_EQ_I32_I32_I32, QB_EQ_I32_I32_I32, QB_EQ_I64_I64_I32, QB_EQ_I64_I64_I32, QB_EQ_F32_F32_I32, QB_EQ_F64_F64_I32, 
											QB_EQ_SET_I08_I08_I32, QB_EQ_SET_I08_I08_I32, QB_EQ_SET_I16_I16_I32, QB_EQ_SET_I16_I16_I32, QB_EQ_SET_I32_I32_I32, QB_EQ_SET_I32_I32_I32, QB_EQ_SET_I64_I64_I32, QB_EQ_SET_I64_I64_I32, QB_EQ_SET_F32_F32_I32, QB_EQ_SET_F64_F64_I32 };
static uint32_t not_equal_opcodes[]		= { QB_NE_I08_I08_I32, QB_NE_I08_I08_I32, QB_NE_I16_I16_I32, QB_NE_I16_I16_I32, QB_NE_I32_I32_I32, QB_NE_I32_I32_I32, QB_NE_I64_I64_I32, QB_NE_I64_I64_I32, QB_NE_F32_F32_I32, QB_NE_F64_F64_I32,
											QB_NE_SET_I08_I08_I32, QB_NE_SET_I08_I08_I32, QB_NE_SET_I16_I16_I32, QB_NE_SET_I16_I16_I32, QB_NE_SET_I32_I32_I32, QB_NE_SET_I32_I32_I32, QB_NE_SET_I64_I64_I32, QB_NE_SET_I64_I64_I32, QB_NE_SET_F32_F32_I32, QB_NE_SET_F64_F64_I32 };
static uint32_t less_than_opcodes[]		= { QB_LT_S08_S08_I32, QB_LT_S08_S08_I32, QB_LT_S16_S16_I32, QB_LT_S16_S16_I32, QB_LT_S32_S32_I32, QB_LT_S32_S32_I32, QB_LT_S64_S64_I32, QB_LT_S64_S64_I32, QB_LT_F32_F32_I32, QB_LT_F64_F64_I32, 
											QB_LT_SET_S08_S08_I32, QB_LT_SET_U08_U08_I32, QB_LT_SET_S16_S16_I32, QB_LT_SET_U16_U16_I32, QB_LT_SET_S32_S32_I32, QB_LT_SET_U32_U32_I32, QB_LT_SET_S64_S64_I32, QB_LT_SET_U64_U64_I32, QB_LT_SET_F32_F32_I32, QB_LT_SET_F64_F64_I32 };
static uint32_t less_or_equal_opcodes[]	= { QB_LE_S08_S08_I32, QB_LE_S08_S08_I32, QB_LE_S16_S16_I32, QB_LE_S16_S16_I32,	QB_LE_S32_S32_I32, QB_LE_S32_S32_I32, QB_LE_S64_S64_I32, QB_LE_S64_S64_I32, QB_LE_F32_F32_I32, QB_LE_F64_F64_I32,
											QB_LE_SET_S08_S08_I32, QB_LE_SET_U08_U08_I32, QB_LE_SET_S16_S16_I32, QB_LE_SET_U16_U16_I32, QB_LE_SET_S32_S32_I32, QB_LE_SET_U32_U32_I32, QB_LE_SET_S64_S64_I32, QB_LE_SET_U64_U64_I32, QB_LE_SET_F32_F32_I32, QB_LE_SET_F64_F64_I32 };

static uint32_t all_opcode	=	QB_ALL_I32_I32;
static uint32_t any_opcode	=	QB_ANY_I32_I32;
static uint32_t not_opcode	=	QB_NOT_SET_I32_I32;

static uint32_t concat_string_opcode	= QB_CAT_STR_U08_U08;
static uint32_t concat_opcodes[] 		= { QB_CAT_S08_U08, QB_CAT_U08_U08, QB_CAT_S16_U08, QB_CAT_U16_U08, QB_CAT_S32_U08, QB_CAT_U32_U08, QB_CAT_S64_U08, QB_CAT_U64_U08, QB_CAT_F32_U08, QB_CAT_F64_U08 };
static uint32_t print_opcodes[]			= { QB_PRN_S08, QB_PRN_U08, QB_PRN_S16, QB_PRN_U16, QB_PRN_S32, QB_PRN_U32, QB_PRN_S64, QB_PRN_U64, QB_PRN_F32, QB_PRN_F64 };

static uint32_t return_opcode			= QB_RET;
static uint32_t jump_opcode				= QB_JMP;
static uint32_t branch_on_true_opcode	= QB_IF_T_I32;
static uint32_t branch_on_false_opcode	= QB_IF_F_I32;
static uint32_t fcall_opcodes[]			= {	QB_FCALL_VAR, QB_FCALL_MIX };

static uint32_t sin_opcodes[] 			= { 0, 0, 0, 0, 0, 0, 0, 0,	QB_SIN_F32_F32, 	QB_SIN_F64_F64  };
static uint32_t asin_opcodes[] 			= { 0, 0, 0, 0, 0, 0, 0, 0,	QB_ASIN_F32_F32, 	QB_ASIN_F64_F64 };
static uint32_t cos_opcodes[] 			= { 0, 0, 0, 0, 0, 0, 0, 0,	QB_COS_F32_F32, 	QB_COS_F64_F64 };
static uint32_t acos_opcodes[] 			= { 0, 0, 0, 0, 0, 0, 0, 0,	QB_ACOS_F32_F32, 	QB_ACOS_F64_F64 };
static uint32_t tan_opcodes[] 			= { 0, 0, 0, 0, 0, 0, 0, 0,	QB_TAN_F32_F32, 	QB_TAN_F64_F64 };
static uint32_t atan_opcodes[] 			= { 0, 0, 0, 0, 0, 0, 0, 0,	QB_ATAN_F32_F32, 	QB_ATAN_F64_F64 };
static uint32_t atan2_opcodes[] 		= { 0, 0, 0, 0, 0, 0, 0, 0,	QB_ATAN2_F32_F32_F32, 	QB_ATAN2_F32_F32_F32 };
static uint32_t sinh_opcodes[] 			= { 0, 0, 0, 0, 0, 0, 0, 0,	QB_SINH_F32_F32, 	QB_SINH_F64_F64 };
static uint32_t asinh_opcodes[]			= { 0, 0, 0, 0, 0, 0, 0, 0,	QB_ASINH_F32_F32, 	QB_ASINH_F64_F64 };
static uint32_t cosh_opcodes[] 			= { 0, 0, 0, 0, 0, 0, 0, 0,	QB_COSH_F32_F32, 	QB_COSH_F64_F64 };
static uint32_t acosh_opcodes[]			= { 0, 0, 0, 0, 0, 0, 0, 0,	QB_ACOSH_F32_F32, 	QB_ACOSH_F64_F64 };
static uint32_t tanh_opcodes[] 			= { 0, 0, 0, 0, 0, 0, 0, 0,	QB_TANH_F32_F32, 	QB_TANH_F64_F64 };
static uint32_t atanh_opcodes[] 		= { 0, 0, 0, 0, 0, 0, 0, 0,	QB_ATANH_F32_F32, 	QB_ATANH_F64_F64 };
static uint32_t ceil_opcodes[] 			= { 0, 0, 0, 0, 0, 0, 0, 0,	QB_CEIL_F32_F32, 	QB_CEIL_F64_F64 };
static uint32_t floor_opcodes[] 		= { 0, 0, 0, 0, 0, 0, 0, 0,	QB_FLOOR_F32_F32, 	QB_FLOOR_F64_F64 };
static uint32_t fract_opcodes[]			= { 0, 0, 0, 0, 0, 0, 0, 0,	QB_FRACT_F32_F32,	QB_FRACT_F64_F64 };
static uint32_t round_opcodes[] 		= { 0, 0, 0, 0, 0, 0, 0, 0,	QB_ROUND_F32_I32_I32_F32, 	QB_ROUND_F64_I32_I32_F64 };
static uint32_t log_opcodes[] 			= { 0, 0, 0, 0, 0, 0, 0, 0,	QB_LOG_F32_F32, 	QB_LOG_F64_F64 };
static uint32_t log1p_opcodes[] 		= { 0, 0, 0, 0, 0, 0, 0, 0,	QB_LOG1P_F32_F32, 	QB_LOG1P_F64_F64 };
static uint32_t log10_opcodes[] 		= { 0, 0, 0, 0, 0, 0, 0, 0,	QB_LOG10_F32_F32, 	QB_LOG10_F64_F64 };
static uint32_t log2_opcodes[] 			= { 0, 0, 0, 0, 0, 0, 0, 0,	QB_LOG2_F32_F32, 	QB_LOG2_F64_F64 };
static uint32_t exp_opcodes[] 			= { 0, 0, 0, 0, 0, 0, 0, 0,	QB_EXP_F32_F32, 	QB_EXP_F64_F64 };
static uint32_t exp2_opcodes[] 			= { 0, 0, 0, 0, 0, 0, 0, 0,	QB_EXP2_F32_F32, 	QB_EXP2_F64_F64 };
static uint32_t expm1_opcodes[]			= { 0, 0, 0, 0, 0, 0, 0, 0,	QB_EXPM1_F32_F32, 	QB_EXPM1_F64_F64 };
static uint32_t sqrt_opcodes[] 			= { 0, 0, 0, 0, 0, 0, 0, 0,	QB_SQRT_F32_F32, 	QB_SQRT_F64_F64 };
static uint32_t inv_sqrt_opcodes[]		= { 0, 0, 0, 0, 0, 0, 0, 0,	QB_ISQRT_F32_F32,	QB_ISQRT_F64_F64 };
static uint32_t lcg_opcodes[] 			= { 0, 0, 0, 0, 0, 0, 0, 0,	QB_LCG_F32, 		QB_LCG_F64 };
static uint32_t pow_opcodes[]			= { 0, 0, 0, 0, 0, 0, 0, 0,	QB_POW_F32_F32_F32,	QB_POW_F64_F64_F64 };
static uint32_t hypot_opcodes[]			= { 0, 0, 0, 0, 0, 0, 0, 0,	QB_HYPOT_F32_F32_F32,	QB_HYPOT_F64_F64_F64 };
static uint32_t sign_opcodes[]			= { 0, 0, 0, 0, 0, 0, 0, 0,	QB_SIGN_F32_F32,	QB_SIGN_F64_F64 };
static uint32_t step_opcodes[]			= { 0, 0, 0, 0, 0, 0, 0, 0,	QB_STEP_F32_F32_F32,		QB_STEP_F64_F64_F64 };
static uint32_t clamp_opcodes[]			= { 0, 0, 0, 0, 0, 0, 0, 0,	QB_CLAMP_F32_F32_F32_F32,	QB_CLAMP_F64_F64_F64_F64 };
static uint32_t mix_opcodes[]			= { 0, 0, 0, 0, 0, 0, 0, 0,	QB_MIX_F32_F32_F32_F32,		QB_MIX_F64_F64_F64_F64 };
static uint32_t smooth_step_opcodes[]	= { 0, 0, 0, 0, 0, 0, 0, 0,	QB_SSTEP_F32_F32_F32_F32,	QB_SSTEP_F64_F64_F64_F64 };
static uint32_t deg2rad_opcodes[]		= { 0, 0, 0, 0, 0, 0, 0, 0,	QB_DEG2RAD_F32_F32,		QB_DEG2RAD_F64_F64 };
static uint32_t rad2deg_opcodes[]		= { 0, 0, 0, 0, 0, 0, 0, 0,	QB_RAD2DEG_F32_F32,		QB_RAD2DEG_F64_F64 };

static uint32_t is_finite_opcodes[]		= { 0, 0, 0, 0, 0, 0, 0, 0,	QB_FIN_F32_I32,		QB_FIN_F64_I32 };
static uint32_t is_infinite_opcodes[]	= { 0, 0, 0, 0, 0, 0, 0, 0,	QB_INF_F32_I32,		QB_INF_F64_I32 };
static uint32_t is_nan_opcodes[]		= { 0, 0, 0, 0, 0, 0, 0, 0,	QB_NAN_F32_I32,		QB_NAN_F64_I32 };	

static uint32_t abs_opcodes[] 		= { QB_ABS_S08_S08,	0,	QB_ABS_S16_S16, 0, QB_ABS_S32_S32, 0, QB_ABS_S64_S64, 0,	QB_ABS_F32_F32,	QB_ABS_F64_F64 };
static uint32_t min_opcodes[]		= { QB_MIN_S08_S08_S08,	QB_MIN_U08_U08_U08,	QB_MIN_S16_S16_S16,	QB_MIN_U16_U16_U16,	QB_MIN_S32_S32_S32,	QB_MIN_U32_U32_U32,	QB_MIN_S64_S64_S64,	QB_MIN_U64_U64_U64,	QB_MIN_F32_F32_F32,	QB_MIN_F64_F64_F64,
										QB_AMIN_S08_S08,	QB_AMIN_U08_U08,	QB_AMIN_S16_S16,	QB_AMIN_U16_U16,	QB_AMIN_S32_S32,	QB_AMIN_U32_U32,	QB_AMIN_S64_S64,	QB_AMIN_U64_U64,	QB_AMIN_F32_F32,	QB_AMIN_F64_F64 };
static uint32_t max_opcodes[]		= { QB_MAX_S08_S08_S08,	QB_MAX_U08_U08_U08,	QB_MAX_S16_S16_S16,	QB_MAX_U16_U16_U16,	QB_MAX_S32_S32_S32,	QB_MAX_U32_U32_U32,	QB_MAX_S64_S64_S64,	QB_MAX_U64_U64_U64,	QB_MAX_F32_F32_F32,	QB_MAX_F64_F64_F64,
										QB_AMAX_S08_S08,	QB_AMAX_U08_U08,	QB_AMAX_S16_S16,	QB_AMAX_U16_U16,	QB_AMAX_S32_S32,	QB_AMAX_U32_U32,	QB_AMAX_S64_S64,	QB_AMAX_U64_U64,	QB_AMAX_F32_F32,	QB_AMAX_F64_F64 };

static uint32_t rand_opcodes[]		= { QB_RAND_S08_S08_S08,	QB_RAND_U08_U08_U08,	QB_RAND_S16_S16_S16,	QB_RAND_U16_U16_U16,	QB_RAND_S32_S32_S32,	QB_RAND_U32_U32_U32,	QB_RAND_S64_S64_S64,	QB_RAND_U64_U64_U64,	0, 0 };
static uint32_t mt_rand_opcodes[]	= { QB_MT_RAND_S08_S08_S08, QB_MT_RAND_U08_U08_U08,	QB_MT_RAND_S16_S16_S16,	QB_MT_RAND_U16_U16_U16,	QB_MT_RAND_S32_S32_S32,	QB_MT_RAND_U32_U32_U32,	QB_MT_RAND_S64_S64_S64,	QB_MT_RAND_U64_U64_U64, 0, 0 };

static uint32_t sample_nearest_opcodes[]	= { 0, 0, 0, 0, 0, 0, 0, 0,	QB_SAMPLE_NN_F32_U32_U32_F32_F32_F32, QB_SAMPLE_NN_F64_U32_U32_F64_F64_F64 };
static uint32_t sample_bilinear_opcodes[]	= { 0, 0, 0, 0, 0, 0, 0, 0,	QB_SAMPLE_BL_F32_U32_U32_F32_F32_F32, QB_SAMPLE_BL_F64_U32_U32_F64_F64_F64 };

static uint32_t dot_product_1x_opcodes[]	= {	0, 0, 0, 0, 0, 0, 0, 0,	QB_DOT_1X_F32_F32_F32,	QB_DOT_1X_F64_F64_F64 };
static uint32_t length_1x_opcodes[]			= {	0, 0, 0, 0, 0, 0, 0, 0,	QB_LEN_1X_F32_F32,		QB_LEN_1X_F64_F64 };
static uint32_t distance_1x_opcodes[]		= {	0, 0, 0, 0, 0, 0, 0, 0,	QB_DIS_1X_F32_F32_F32,	QB_DIS_1X_F64_F64_F64 };
static uint32_t normalize_1x_opcodes[]		= {	0, 0, 0, 0, 0, 0, 0, 0,	QB_NORM_1X_F32_F32,		QB_NORM_1X_F64_F64 };

static uint32_t mm_multiply_opcodes[]	= { 0, 0, 0, 0, 0, 0, 0, 0,	QB_MUL_MM_F32_U32_U32_F32_U32_U32_F32,	QB_MUL_MM_F64_U32_U32_F64_U32_U32_F64 };
static uint32_t mv_multiply_opcodes[]	= { 0, 0, 0, 0, 0, 0, 0, 0,	QB_MUL_MV_F32_U32_U32_F32_F32,	QB_MUL_MV_F64_U32_U32_F64_F64 };
static uint32_t vm_multiply_opcodes[]	= { 0, 0, 0, 0, 0, 0, 0, 0,	QB_MUL_VM_F32_F32_U32_U32_F32,	QB_MUL_VM_F64_F64_U32_U32_F64 };
static uint32_t dot_product_opcodes[]	= {	0, 0, 0, 0, 0, 0, 0, 0,	QB_DOT_F32_F32_F32,	QB_DOT_F64_F64_F64 };
static uint32_t length_opcodes[]		= {	0, 0, 0, 0, 0, 0, 0, 0,	QB_LEN_F32_F32,		QB_LEN_F64_F64 };
static uint32_t distance_opcodes[]		= {	0, 0, 0, 0, 0, 0, 0, 0,	QB_DIS_F32_F32_F32,	QB_DIS_F64_F64_F64 };
static uint32_t normalize_opcodes[]		= {	0, 0, 0, 0, 0, 0, 0, 0,	QB_NORM_F32_F32,	QB_NORM_F64_F64 };

static uint32_t mm_multiply_2x2_opcodes[]	= { 0, 0, 0, 0, 0, 0, 0, 0,	QB_MUL_MM_2X2_F32_F32_F32,	QB_MUL_MM_2X2_F64_F64_F64 };
static uint32_t mv_multiply_2x2_opcodes[]	= { 0, 0, 0, 0, 0, 0, 0, 0,	QB_MUL_MV_2X2_F32_F32_F32,	QB_MUL_MV_2X2_F64_F64_F64 };
static uint32_t vm_multiply_2x2_opcodes[]	= { 0, 0, 0, 0, 0, 0, 0, 0,	QB_MUL_VM_2X2_F32_F32_F32,	QB_MUL_VM_2X2_F64_F64_F64 };
static uint32_t dot_product_2x_opcodes[]	= {	0, 0, 0, 0, 0, 0, 0, 0,	QB_DOT_2X_F32_F32_F32,	QB_DOT_2X_F64_F64_F64 };
static uint32_t length_2x_opcodes[]			= {	0, 0, 0, 0, 0, 0, 0, 0,	QB_LEN_2X_F32_F32,		QB_LEN_2X_F64_F64 };
static uint32_t distance_2x_opcodes[]		= {	0, 0, 0, 0, 0, 0, 0, 0,	QB_DIS_2X_F32_F32_F32,	QB_DIS_2X_F64_F64_F64 };
static uint32_t normalize_2x_opcodes[]		= {	0, 0, 0, 0, 0, 0, 0, 0,	QB_NORM_2X_F32_F32,		QB_NORM_2X_F64_F64 };

static uint32_t mm_multiply_3x3_opcodes[]	= { 0, 0, 0, 0, 0, 0, 0, 0,	QB_MUL_MM_3X3_F32_F32_F32,	QB_MUL_MM_3X3_F64_F64_F64 };
static uint32_t mv_multiply_3x3_opcodes[]	= { 0, 0, 0, 0, 0, 0, 0, 0,	QB_MUL_MV_3X3_F32_F32_F32,	QB_MUL_MV_3X3_F64_F64_F64 };
static uint32_t vm_multiply_3x3_opcodes[]	= { 0, 0, 0, 0, 0, 0, 0, 0,	QB_MUL_VM_3X3_F32_F32_F32,	QB_MUL_VM_3X3_F64_F64_F64 };
static uint32_t mm_multiply_3x3_padded_opcodes[]	= { 0, 0, 0, 0, 0, 0, 0, 0,	QB_MUL_MM_3X3P_F32_F32_F32,	QB_MUL_MM_3X3P_F64_F64_F64 };
static uint32_t mv_multiply_3x3_padded_opcodes[]	= { 0, 0, 0, 0, 0, 0, 0, 0,	QB_MUL_MV_3X3P_F32_F32_F32,	QB_MUL_MV_3X3P_F64_F64_F64 };
static uint32_t vm_multiply_3x3_padded_opcodes[]	= { 0, 0, 0, 0, 0, 0, 0, 0,	QB_MUL_VM_3X3P_F32_F32_F32,	QB_MUL_VM_3X3P_F64_F64_F64 };
static uint32_t dot_product_3x_opcodes[]	= {	0, 0, 0, 0, 0, 0, 0, 0,	QB_DOT_3X_F32_F32_F32,	QB_DOT_3X_F64_F64_F64 };
static uint32_t length_3x_opcodes[]			= {	0, 0, 0, 0, 0, 0, 0, 0,	QB_LEN_3X_F32_F32,		QB_LEN_3X_F64_F64 };
static uint32_t distance_3x_opcodes[]		= {	0, 0, 0, 0, 0, 0, 0, 0,	QB_DIS_3X_F32_F32_F32,	QB_DIS_3X_F64_F64_F64 };
static uint32_t normalize_3x_opcodes[]		= {	0, 0, 0, 0, 0, 0, 0, 0,	QB_NORM_3X_F32_F32,		QB_NORM_3X_F64_F64 };
static uint32_t cross_product_opcodes[]		= {	0, 0, 0, 0, 0, 0, 0, 0,	QB_CROSS_F32_F32_F32,	QB_CROSS_F64_F64_F64 };

static uint32_t mm_multiply_4x4_opcodes[]	= { 0, 0, 0, 0, 0, 0, 0, 0,	QB_MUL_MM_4X4_F32_F32_F32,	QB_MUL_MM_4X4_F64_F64_F64 };
static uint32_t mv_multiply_4x4_opcodes[]	= { 0, 0, 0, 0, 0, 0, 0, 0,	QB_MUL_MV_4X4_F32_F32_F32,	QB_MUL_MV_4X4_F64_F64_F64 };
static uint32_t vm_multiply_4x4_opcodes[]	= { 0, 0, 0, 0, 0, 0, 0, 0,	QB_MUL_VM_4X4_F32_F32_F32,	QB_MUL_VM_4X4_F64_F64_F64 };
static uint32_t dot_product_4x_opcodes[]	= {	0, 0, 0, 0, 0, 0, 0, 0,	QB_DOT_4X_F32_F32_F32,	QB_DOT_4X_F64_F64_F64 };
static uint32_t length_4x_opcodes[]			= {	0, 0, 0, 0, 0, 0, 0, 0,	QB_LEN_4X_F32_F32,		QB_LEN_4X_F64_F64 };
static uint32_t distance_4x_opcodes[]		= {	0, 0, 0, 0, 0, 0, 0, 0,	QB_DIS_4X_F32_F32_F32,	QB_DIS_4X_F64_F64_F64 };
static uint32_t normalize_4x_opcodes[]		= {	0, 0, 0, 0, 0, 0, 0, 0,	QB_NORM_4X_F32_F32,		QB_NORM_4X_F64_F64 };

static uint32_t utf8_decode_opcodes[]	= { 0, 0, QB_UTF8_DEC_U08_U16,	QB_UTF8_DEC_U08_U16,	QB_UTF8_DEC_U08_U32,	QB_UTF8_DEC_U08_U32, 0, 0, 0, 0 };
static uint32_t utf8_encode_opcodes[]	= { 0, 0, QB_UTF8_ENC_U16_U08,	QB_UTF8_ENC_U16_U08,	QB_UTF8_ENC_U32_U08,	QB_UTF8_ENC_U32_U08, 0, 0, 0, 0 };

static uint32_t pack_le_opcodes[]	= { 0, 0, QB_PACK_LE_I16_U08, QB_PACK_LE_I16_U08, QB_PACK_LE_I32_U08, QB_PACK_LE_I32_U08, QB_PACK_LE_I64_U08, QB_PACK_LE_I64_U08, QB_PACK_LE_F32_U08, QB_PACK_LE_F64_U08 };
static uint32_t pack_be_opcodes[]	= { 0, 0, QB_PACK_BE_I16_U08, QB_PACK_BE_I16_U08, QB_PACK_BE_I32_U08, QB_PACK_BE_I32_U08, QB_PACK_BE_I64_U08, QB_PACK_BE_I64_U08, QB_PACK_BE_F32_U08, QB_PACK_BE_F64_U08 };

static uint32_t unpack_le_opcodes[]	= { 0, 0, QB_UNPACK_LE_U08_I16, QB_UNPACK_LE_U08_I16, QB_UNPACK_LE_U08_I32, QB_UNPACK_LE_U08_I32, QB_UNPACK_LE_U08_I64, QB_UNPACK_LE_U08_I64, QB_UNPACK_LE_U08_F32, QB_UNPACK_LE_U08_F64 };
static uint32_t unpack_be_opcodes[]	= { 0, 0, QB_UNPACK_BE_U08_I16, QB_UNPACK_BE_U08_I16, QB_UNPACK_BE_U08_I32, QB_UNPACK_BE_U08_I32, QB_UNPACK_BE_U08_I64, QB_UNPACK_BE_U08_I64, QB_UNPACK_BE_U08_F32, QB_UNPACK_BE_U08_F64 };

static uint32_t sort_opcodes[]			= { QB_SORT_S08,	QB_SORT_U08,	QB_SORT_S16,	QB_SORT_U16,	QB_SORT_S32,	QB_SORT_U32,	QB_SORT_S64,	QB_SORT_U64,	QB_SORT_F32,	QB_SORT_F64 };
static uint32_t rsort_opcodes[]			= { QB_RSORT_S08,	QB_RSORT_U08,	QB_RSORT_S16,	QB_RSORT_U16,	QB_RSORT_S32,	QB_RSORT_U32,	QB_RSORT_S64,	QB_RSORT_U64,	QB_RSORT_F32,	QB_RSORT_F64 };

static uint32_t array_sum_opcodes[]		= { QB_ASUM_S08_S08,	QB_ASUM_U08_U08,	QB_ASUM_S16_S16,	QB_ASUM_U16_U16,	QB_ASUM_S32_S32,	QB_ASUM_U32_U32,	QB_ASUM_S64_S64,	QB_ASUM_U64_U64,	QB_ASUM_F32_F32,	QB_ASUM_F64_F64 };
static uint32_t array_product_opcodes[]	= { QB_APROD_S08_S08,	QB_APROD_U08_U08,	QB_APROD_S16_S16,	QB_APROD_U16_U16,	QB_APROD_S32_S32,	QB_APROD_U32_U32,	QB_APROD_S64_S64,	QB_APROD_U64_U64,	QB_APROD_F32_F32,	QB_APROD_F64_F64 };

static uint32_t array_search_opcodes[]	= { QB_AFIND_IDX_I08_I08_I32,	QB_AFIND_IDX_I08_I08_I32,	QB_AFIND_IDX_I16_I16_I32,	QB_AFIND_IDX_I16_I16_I32,	QB_AFIND_IDX_I32_I32_I32,	QB_AFIND_IDX_I32_I32_I32,	QB_AFIND_IDX_I64_I64_I32,	QB_AFIND_IDX_I64_I64_I32,	QB_AFIND_IDX_F32_F32_I32,	QB_AFIND_IDX_F64_F64_I32 };
static uint32_t in_array_opcodes[]		= { QB_AFIND_I08_I08_I32,	QB_AFIND_I08_I08_I32,	QB_AFIND_I16_I16_I32,	QB_AFIND_I16_I16_I32,	QB_AFIND_I32_I32_I32,	QB_AFIND_I32_I32_I32,	QB_AFIND_I64_I64_I32,	QB_AFIND_I64_I64_I32,	QB_AFIND_F32_F32_I32,	QB_AFIND_F64_F64_I32 };
static uint32_t subarray_pos_opcodes[]	= { QB_APOS_I08_I08_I32_I32,	QB_APOS_I08_I08_I32_I32,	QB_APOS_I16_I16_I32_I32,	QB_APOS_I16_I16_I32_I32,	QB_APOS_I32_I32_I32_I32,	QB_APOS_I32_I32_I32_I32,	QB_APOS_I64_I64_I32_I32,	QB_APOS_I64_I64_I32_I32,	QB_APOS_F32_F32_I32_I32,	QB_APOS_F64_F64_I32_I32 };
static uint32_t subarray_rpos_opcodes[]	= { QB_ARPOS_I08_I08_I32_I32,	QB_ARPOS_I08_I08_I32_I32,	QB_ARPOS_I16_I16_I32_I32,	QB_ARPOS_I16_I16_I32_I32,	QB_ARPOS_I32_I32_I32_I32,	QB_ARPOS_I32_I32_I32_I32,	QB_ARPOS_I64_I64_I32_I32,	QB_ARPOS_I64_I64_I32_I32,	QB_ARPOS_F32_F32_I32_I32,	QB_ARPOS_F64_F64_I32_I32 };
