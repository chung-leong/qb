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

#ifndef QB_TRANSLATOR_PBJ_H_
#define QB_TRANSLATOR_PBJ_H_

typedef struct qb_pbj_translator_context	qb_pbj_translator_context;
typedef struct qb_pbj_address				qb_pbj_address;
typedef struct qb_pbj_address_bundle		qb_pbj_address_bundle;
typedef struct qb_pbj_constant				qb_pbj_constant;
typedef struct qb_pbj_register				qb_pbj_register;
typedef struct qb_pbj_value					qb_pbj_value;
typedef struct qb_pbj_parameter				qb_pbj_parameter;
typedef struct qb_pbj_texture				qb_pbj_texture;
typedef struct qb_pbj_op					qb_pbj_op;
typedef struct qb_pbj_translator			qb_pbj_translator;
typedef struct qb_pbj_register				qb_pbj_register;
typedef struct qb_pbj_register_slot			qb_pbj_register_slot;

typedef enum qb_pbj_opcode					qb_pbj_opcode;
typedef enum qb_pbj_channel_id				qb_pbj_channel_id;

typedef void (*qb_pbj_translator_proc)(qb_pbj_translator_context *cxt, qb_pbj_translator *t, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_result_prototype *result_prototype);

struct qb_pbj_address {
	uint32_t dimension;
	uint32_t register_id;
	uint32_t channel_count;
	uint32_t channel_mask;
	qb_pbj_channel_id channel_id;
};

struct qb_pbj_constant {
	uint32_t dimension_zero;
	uint32_t channel_count_zero;
	uint32_t type;
	union {
		float32_t float_value;
		int32_t int_value;
	};
};

enum qb_pbj_channel_id {
	PBJ_CHANNEL_R			= 0,
	PBJ_CHANNEL_G,
	PBJ_CHANNEL_B,
	PBJ_CHANNEL_A,
	PBJ_CHANNEL_RG,
	PBJ_CHANNEL_GB,
	PBJ_CHANNEL_BA,
	PBJ_CHANNEL_RGB,
	PBJ_CHANNEL_GBA,
	PBJ_CHANNEL_RGBA,
	PBJ_CHANNEL_MATRIX,
	PBJ_CHANNEL_NOT_CONTINUOUS = -1,
};

struct qb_pbj_register {
	qb_address *matrix_address;
	qb_address *channel_addresses[PBJ_CHANNEL_RGBA + 1];
	uint32_t span;
	int32_t loaded;
};

struct qb_pbj_value {
	uint32_t type;
	union {
		float32_t float1;
		float32_t float2[2];
		float32_t float3[3];
		float32_t float4[3];
		float32_t float2x2[4];
		float32_t float3x3[9];
		float32_t float4x4[16];
		int32_t int1;
		int32_t int2[2];
		int32_t int3[3];
		int32_t int4[4];
		const char *string;
	};
};

struct qb_pbj_parameter {
	const char *name;
	uint32_t type;
	uint32_t qualifier;
	qb_pbj_address destination;
	qb_pbj_value default_value;
	qb_pbj_value min_value;
	qb_pbj_value max_value;
	const char *parameter_type;
	const char *description;
	const char *input_size_name;
	const char *display_name;
	qb_address *address;
};

struct qb_pbj_texture {
	const char *name;
	uint32_t image_id;
	uint32_t channel_count;
	qb_pbj_parameter *input_size;
	qb_address *address;
};

enum {
	PBJ_SOURCE_IN_USE				= 0x00000001,
	PBJ_DESTINATION_IN_USE			= 0x00000002,
	PBJ_IMAGE_ID_IN_USE				= 0x00000004,
};

struct qb_pbj_op {
	qb_pbj_opcode opcode;
	uint32_t flags;
	uint32_t image_id;
	union {
		qb_pbj_address destination;
		qb_pbj_address source3;
		uint32_t branch_target_index;
	};
	union {
		qb_pbj_address source;
		qb_pbj_address source2;
		qb_pbj_constant constant;
	};
};

enum {
	PBJ_READ_SOURCE					= 0x00000001,
	PBJ_READ_DESTINATION_FIRST		= 0x00000002,
	PBJ_READ_DESTINATION			= 0x00000004,
	PBJ_READ_SOURCE2				= 0x00000008,
	PBJ_READ_SOURCE3				= 0x00000010,
	PBJ_READ_IMAGE					= 0x00000020,
	PBJ_WRITE_DESTINATION			= 0x00000040,
	PBJ_WRITE_SOURCE				= 0x00000080,
	PBJ_WRITE_BOOLEAN				= 0x00000100,
	PBJ_WRITE_SCALAR				= 0x00000200,
};

struct qb_pbj_translator {
	qb_pbj_translator_proc translate;
	uint32_t flags;
	void *extra;
};

struct qb_pbj_translator_context {
	qb_compiler_context *compiler_context;
	qb_data_pool *pool;
	qb_storage *storage;

	qb_result_prototype *result_prototypes;
	uint32_t result_prototype_count;

	qb_pbj_register *float_registers;
	uint32_t float_register_count;

	qb_pbj_register *int_registers;
	uint32_t int_register_count;

	qb_pbj_op *pbj_ops;
	uint32_t pbj_op_count;
	qb_pbj_op *pbj_op;
	uint32_t pbj_op_index;
	int32_t jump_target_index;

	qb_pbj_op **conditionals;
	uint32_t conditional_count; 

	qb_pbj_parameter *parameters;
	uint32_t parameter_count;
	qb_pbj_parameter *out_coord;
	qb_pbj_parameter *out_pixel;

	qb_pbj_texture *textures;
	uint32_t texture_count;

	uint32_t loop_op_index;
	uint32_t outer_loop_start_index;
	uint32_t inner_loop_start_index;

	qb_address *x_address;
	qb_address *y_address;
	qb_address *active_pixel_address;

	qb_address *output_image_address;
	qb_address *output_image_width_address;
	qb_address *output_image_height_address;
	qb_address *output_image_channel_count_address;
	qb_address *output_image_scanline_address;
	qb_address *output_image_pixel_address;

	qb_address *out_coord_address;
	qb_address *out_coord_x_address;
	qb_address *out_coord_y_address;

	const char *pbj_name;
	uint32_t name_length;
	const char *vendor;
	const char *display_name;
	const char *description;
	uint32_t version;

	uint8_t *pbj_data;
	uint8_t *pbj_data_end;

	void ***tsrm_ls;
};

enum qb_pbj_opcode {
	PBJ_NOP							= 0x00,
	PBJ_ADD							= 0x01,
	PBJ_SUBTRACT					= 0x02,
	PBJ_MULTIPLY					= 0x03,
	PBJ_RECIPROCAL					= 0x04,
	PBJ_DIVIDE						= 0x05,
	PBJ_ATAN2						= 0x06,
	PBJ_POW							= 0x07,
	PBJ_MOD							= 0x08,
	PBJ_MIN							= 0x09,
	PBJ_MAX							= 0x0A,
	PBJ_STEP						= 0x0B,
	PBJ_SIN							= 0x0C,
	PBJ_COS							= 0x0D,
	PBJ_TAN							= 0x0E,
	PBJ_ASIN						= 0x0F,
	PBJ_ACOS						= 0x10,
	PBJ_ATAN						= 0x11,
	PBJ_EXP							= 0x12,
	PBJ_EXP2						= 0x13,
	PBJ_LOG							= 0x14,
	PBJ_LOG2						= 0x15,
	PBJ_SQRT						= 0x16,
	PBJ_RSQRT						= 0x17,
	PBJ_ABS							= 0x18,
	PBJ_SIGN						= 0x19,
	PBJ_FLOOR						= 0x1A,
	PBJ_CEIL						= 0x1B,
	PBJ_FRACT						= 0x1C,
	PBJ_COPY						= 0x1D,
	PBJ_FLOAT_TO_INT				= 0x1E,
	PBJ_INT_TO_FLOAT				= 0x1F,
	PBJ_MATRIX_MATRIX_MULTIPLY		= 0x20,
	PBJ_VECTOR_MATRIX_MULTIPLY		= 0x21,
	PBJ_MATRIX_VECTOR_MULTIPLY		= 0x22,
	PBJ_NORMALIZE					= 0x23,
	PBJ_LENGTH						= 0x24,
	PBJ_DISTANCE					= 0x25,
	PBJ_DOT_PRODUCT					= 0x26,
	PBJ_CROSS_PRODUCT				= 0x27,
	PBJ_EQUAL						= 0x28,
	PBJ_NOT_EQUAL					= 0x29,
	PBJ_LESS_THAN					= 0x2A,
	PBJ_LESS_THAN_EQUAL				= 0x2B,
	PBJ_LOGICAL_NOT					= 0x2C,
	PBJ_LOGICAL_AND					= 0x2D,
	PBJ_LOGICAL_OR					= 0x2E,
	PBJ_LOGICAL_XOR					= 0x2F,
	PBJ_SAMPLE_NEAREST				= 0x30,
	PBJ_SAMPLE_BILINEAR				= 0x31,
	PBJ_LOAD_CONSTANT				= 0x32,
	PBJ_SELECT						= 0x33,
	PBJ_IF							= 0x34,
	PBJ_ELSE						= 0x35,
	PBJ_END_IF						= 0x36,
	PBJ_FLOAT_TO_BOOL				= 0x37,
	PBJ_BOOL_TO_FLOAT				= 0x38,
	PBJ_INT_TO_BOOL					= 0x39,
	PBJ_BOOL_TO_INT					= 0x3A,
	PBJ_VECTOR_EQUAL				= 0x3B,
	PBJ_VECTOR_NOT_EQUAL			= 0x3C,
	PBJ_ANY							= 0x3D,
	PBJ_ALL							= 0x3E,

	PBJ_SMOOTH_STEP					= 0x3F,			// custom opcodes
	PBJ_OP_DATA						= 0x40,			//
	PBJ_MAX_OPCODE					= 0x40,

	PBJ_KERNEL_METADATA				= 0xA0,
	PBJ_PARAMETER_DATA				= 0xA1,
	PBJ_PARAMETER_METADATA			= 0xA2,
	PBJ_TEXTURE_DATA				= 0xA3,
	PBJ_KERNEL_NAME					= 0xA4,
	PBJ_VERSION_DATA				= 0xA5,

};

enum {
	PBJ_TYPE_FLOAT					= 0x01,
	PBJ_TYPE_FLOAT2					= 0x02,
	PBJ_TYPE_FLOAT3					= 0x03,
	PBJ_TYPE_FLOAT4					= 0x04,
	PBJ_TYPE_FLOAT2X2				= 0x05,
	PBJ_TYPE_FLOAT3X3				= 0x06,
	PBJ_TYPE_FLOAT4X4				= 0x07,
	PBJ_TYPE_INT					= 0x08,
	PBJ_TYPE_INT2					= 0x09,
	PBJ_TYPE_INT3					= 0x0A,
	PBJ_TYPE_INT4					= 0x0B,
	PBJ_TYPE_STRING					= 0x0C,
	PBJ_TYPE_BOOL					= 0x0D,
	PBJ_TYPE_BOOL2					= 0x0E,
	PBJ_TYPE_BOOL3					= 0x0F,
	PBJ_TYPE_BOOL4					= 0x10,
};

enum {
	PBJ_REGISTER_INT				= 0x8000,
};

enum {
	PBJ_PARAMETER_IN				= 0x0001,
	PBJ_PARAMETER_OUT				= 0x0002,
};

void qb_initialize_pbj_translator_context(qb_pbj_translator_context *cxt, qb_compiler_context *compiler_cxt TSRMLS_DC);
void qb_free_pbj_translator_context(qb_pbj_translator_context *cxt);

void qb_decode_pbj_binary(qb_pbj_translator_context *cxt);
void qb_survey_pbj_instructions(qb_pbj_translator_context *cxt);
void qb_translate_pbj_instructions(qb_pbj_translator_context *cxt);

#endif
