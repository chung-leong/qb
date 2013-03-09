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

#ifndef QB_COMPILER_PBJ_H_
#define QB_COMPILER_PBJ_H_

typedef struct qb_pbj_address			qb_pbj_address;
typedef struct qb_pbj_address_bundle	qb_pbj_address_bundle;
typedef struct qb_pbj_constant			qb_pbj_constant;
typedef struct qb_pbj_register			qb_pbj_register;
typedef struct qb_pbj_value				qb_pbj_value;
typedef struct qb_pbj_parameter			qb_pbj_parameter;
typedef struct qb_pbj_texture			qb_pbj_texture;
typedef struct qb_pbj_op				qb_pbj_op;
typedef struct qb_pbj_translator		qb_pbj_translator;

typedef void (ZEND_FASTCALL *qb_pbj_translator_proc)(qb_compiler_context *cxt, qb_pbj_translator *t, qb_pbj_address **inputs, uint32_t intput_count, qb_pbj_address *output);

struct qb_pbj_address {
	uint32_t dimension;
	uint32_t channel_count;
	union {
		uint8_t	channels[4];
		uint32_t all_channels;
	};
	uint32_t register_id;
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

#define PBJ_CHANNEL_ACCESS_TYPE(index, count)		(((count - 1) << 2) | index)
#define PBJ_CHANNEL_ACCESS_INDEX(usage)				(usage & 0x03)
#define PBJ_CHANNEL_ACCESS_COUNT(usage)				((usage >> 2) + 1)

#define PBJ_MATRIX_ACCESS_TYPE(dimension, offset)	(((dimension - 1) << 2) | offset)
#define PBJ_MATRIX_ACCESS_OFFSET(usage)				(usage & 0x03)
#define PBJ_MATRIX_ACCESS_DIMENSION(usage)			((usage >> 2) + 1)

enum {
	PBJ_CHANNEL_R						= PBJ_CHANNEL_ACCESS_TYPE(0, 1),
	PBJ_CHANNEL_G						= PBJ_CHANNEL_ACCESS_TYPE(1, 1),
	PBJ_CHANNEL_B						= PBJ_CHANNEL_ACCESS_TYPE(2, 1),
	PBJ_CHANNEL_A						= PBJ_CHANNEL_ACCESS_TYPE(3, 1),
	PBJ_CHANNEL_RG						= PBJ_CHANNEL_ACCESS_TYPE(0, 2),
	PBJ_CHANNEL_GB						= PBJ_CHANNEL_ACCESS_TYPE(1, 2),
	PBJ_CHANNEL_BA						= PBJ_CHANNEL_ACCESS_TYPE(2, 2),
	PBJ_CHANNEL_RGB						= PBJ_CHANNEL_ACCESS_TYPE(0, 3),
	PBJ_CHANNEL_GBA						= PBJ_CHANNEL_ACCESS_TYPE(1, 3),
	PBJ_CHANNEL_RGBA					= PBJ_CHANNEL_ACCESS_TYPE(0, 4),

	PBJ_MATRIX_2X2						= PBJ_MATRIX_ACCESS_TYPE(2, 0),
	PBJ_MATRIX_3X3_R1					= PBJ_MATRIX_ACCESS_TYPE(3, 0),
	PBJ_MATRIX_3X3_R2					= PBJ_MATRIX_ACCESS_TYPE(3, 1),
	PBJ_MATRIX_3X3_R3					= PBJ_MATRIX_ACCESS_TYPE(3, 2),
	PBJ_MATRIX_4X4_R1					= PBJ_MATRIX_ACCESS_TYPE(4, 0),
	PBJ_MATRIX_4X4_R2					= PBJ_MATRIX_ACCESS_TYPE(4, 1),
	PBJ_MATRIX_4X4_R3					= PBJ_MATRIX_ACCESS_TYPE(4, 2),
	PBJ_MATRIX_4X4_R4					= PBJ_MATRIX_ACCESS_TYPE(4, 3),
};

enum {
	PBJ_USE_R							= 1 << PBJ_CHANNEL_R,
	PBJ_USE_G							= 1 << PBJ_CHANNEL_G,
	PBJ_USE_B							= 1 << PBJ_CHANNEL_B,
	PBJ_USE_A							= 1 << PBJ_CHANNEL_A,
	PBJ_USE_RG							= 1 << PBJ_CHANNEL_RG,
	PBJ_USE_GB							= 1 << PBJ_CHANNEL_GB,
	PBJ_USE_BA							= 1 << PBJ_CHANNEL_BA,
	PBJ_USE_RGB							= 1 << PBJ_CHANNEL_RGB,
	PBJ_USE_GBA							= 1 << PBJ_CHANNEL_GBA,
	PBJ_USE_RGBA						= 1 << PBJ_CHANNEL_RGBA,
};

struct qb_pbj_register {
	uint32_t channel_usage;
	uint32_t matrix_usage;
	qb_address *channel_addresses[PBJ_CHANNEL_RGBA + 1];
	qb_address *matrix_address;
};

struct qb_pbj_address_bundle {
	uint32_t address_count;
	uint32_t distinct_address_count;
	qb_address *addresses[4];
	qb_address *temporary_address;
	uint32_t channel_counts[4];
	uint32_t total_channel_count;
	uint32_t access;
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

struct qb_pbj_op {
	uint32_t opcode;
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
	qb_pbj_address *source_origin;
	qb_pbj_address *destination_origin;
	qb_pbj_address *eventual_destination;
};

#define PBJ_READ_SOURCE				0x00000001
#define PBJ_READ_DESTINATION		0x00000002
#define PBJ_READ_SOURCE2			0x00000004
#define PBJ_READ_SOURCE3			0x00000008
#define PBJ_WRITE_DESTINATION		0x00000010
#define PBJ_WRITE_BOOL				0x00000020
#define PBJ_WRITE_SCALAR			0x00000040

#define PBJ_OP_READ					0x00000001
#define PBJ_OP_READ_EXACT			(0x00000001 | 0x00000002)
#define PBJ_OP_WRITE				0x00000004
#define PBJ_OP_OVERWRITE			(0x00000004 | 0x00000008)
#define PBJ_OP_OVERWRITE_EXACT		(0x00000004 | 0x00000008 | 0x00000010)

#define PBJ_AS_SOURCE				0x00000100
#define PBJ_AS_DESTINATION			0x00000200
#define PBJ_AS_SOURCE2				0x00000400
#define PBJ_AS_SOURCE3				0x00000800
#define PBJ_AS_BOOL					0x00001000

struct qb_pbj_translator {
	qb_pbj_translator_proc translate;
	uint32_t flags;
	void *extra;
};

enum {
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

extern uint32_t pbj_matrix_sizes[];

#endif
