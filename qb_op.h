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

#ifndef QB_OP_H_
#define QB_OP_H_

typedef struct qb_op						qb_op;
typedef struct qb_op_info					qb_op_info;
typedef struct qb_operand					qb_operand;
typedef struct qb_address					qb_address;
typedef struct qb_expression				qb_expression;
typedef struct qb_index_alias_scheme		qb_index_alias_scheme;
typedef struct qb_array_initializer			qb_array_initializer;
typedef struct qb_result_prototype			qb_result_prototype;
typedef struct qb_result_destination		qb_result_destination;
typedef struct qb_external_symbol			qb_external_symbol;

typedef enum qb_opcode						qb_opcode;
typedef enum qb_operand_type				qb_operand_type;
typedef enum qb_address_mode				qb_address_mode;
typedef enum qb_result_destination_type		qb_result_destination_type;

enum qb_address_mode {
	QB_ADDRESS_MODE_SCA				= 0,
	QB_ADDRESS_MODE_ELE,
	QB_ADDRESS_MODE_ARR,

	QB_ADDRESS_MODE_UNKNOWN			= 100,
};

enum {
	QB_ADDRESS_READ_ONLY			= 0x00000001,
	QB_ADDRESS_CONSTANT				= 0x00000002,
	QB_ADDRESS_STRING				= 0x00000004,
	QB_ADDRESS_BOOLEAN				= 0x00000008,
	QB_ADDRESS_STATIC				= 0x00000010,
	QB_ADDRESS_SHARED				= 0x00000020,
	QB_ADDRESS_TEMPORARY			= 0x00000040,
	QB_ADDRESS_REUSED				= 0x00000080,
	QB_ADDRESS_RESIZABLE			= 0x00000100,
	QB_ADDRESS_ALWAYS_IN_BOUND		= 0x00000200,
	QB_ADDRESS_AUTO_INCREMENT		= 0x00000400,
	QB_ADDRESS_CAST					= 0x00000800,
	QB_ADDRESS_NON_LOCAL			= 0x00001000,
	QB_ADDRESS_NON_REUSABLE			= 0x00002000,
	QB_ADDRESS_AUTO_EXPAND			= 0x00004000,
	QB_ADDRESS_FOREACH_INDEX		= 0x01000000,

	QB_ADDRESS_IN_USE				= 0x80000000,
	QB_ADDRESS_TAGGED				= 0x40000000,

	QB_ADDRESS_RUNTIME_FLAGS		= 0x0000FFFF,
	QB_ADDRESS_COMPILE_TIME_FLAGS	= 0xFFFF0000,
};

struct qb_index_alias_scheme {
	uint32_t dimension;
	char **aliases;
	uint32_t *alias_lengths;
	const char *class_name;
	uint32_t class_name_length;
	zend_class_entry *zend_class;
};

enum {
	QB_EXPR_RESULT_IS_STILL_VALID	= 0x00000001,
};

struct qb_expression {
	uint32_t flags;
	qb_operand *operands;
	qb_operand *result;
	uint32_t operand_count;
	void *op_factory;
};

struct qb_address {
	qb_address_mode mode;
	qb_primitive_type type;
	uint32_t flags;
	uint32_t dimension_count;
	uint32_t segment_selector;
	uint32_t segment_offset;
	qb_address *array_index_address;
	qb_address *array_size_address;
	qb_address **dimension_addresses;
	qb_address **array_size_addresses;
	qb_index_alias_scheme **index_alias_schemes;
	qb_address *source_address;
	qb_expression *expression;
};

enum qb_operand_type {
	QB_OPERAND_NONE					= 0,
	QB_OPERAND_ADDRESS,
	QB_OPERAND_EXTERNAL_SYMBOL,
	QB_OPERAND_ARRAY_INITIALIZER,
	QB_OPERAND_ZEND_CLASS,
	QB_OPERAND_ZEND_STATIC_CLASS,
	QB_OPERAND_ZVAL,
	QB_OPERAND_EMPTY,
	QB_OPERAND_RESULT_PROTOTYPE,
	QB_OPERAND_NUMBER,
	QB_OPERAND_INTRINSIC_FUNCTION,
	QB_OPERAND_ZEND_FUNCTION,
	QB_OPERAND_STATIC_ZEND_FUNCTION,
	QB_OPERAND_ARGUMENTS,
	QB_OPERAND_SEGMENT_SELECTOR,
	QB_OPERAND_ELEMENT_SIZE,
	QB_OPERAND_PBJ_CONSTANT,
};

typedef struct qb_function			qb_function;
typedef struct qb_pbj_constant		qb_pbj_constant;

struct qb_operand {
	qb_operand_type type;
	union {
		qb_address *address;
		uint32_t symbol_index;
		int32_t number;
		zval *constant;
		zend_class_entry *zend_class;
		zend_function *zend_function;
		qb_array_initializer *array_initializer;
		qb_result_prototype *result_prototype;
		qb_intrinsic_function *intrinsic_function;
		qb_operand *arguments;
		qb_pbj_constant *pbj_constant;
		void *generic_pointer;
	};
};

enum {
	QB_ARRAY_INITIALIZER_CONSTANT_ELEMENTS	= 0x00000001,
	QB_ARRAY_INITIALIZER_VARIABLE_ELEMENTS	= 0x00000002,
};

struct qb_array_initializer {
	qb_operand *elements;
	uint32_t element_count;
	qb_primitive_type desired_type;
	int32_t flags;
};

struct qb_result_prototype {
	qb_primitive_type preliminary_type;
	qb_primitive_type final_type;
	uint32_t coercion_flags;
	uint32_t address_flags;
	qb_result_prototype *parent;
	qb_result_destination *destination;
#ifdef ZEND_DEBUG
	zend_op *zend_op;
	void *extra;
#endif
};

enum qb_result_destination_type {
	QB_RESULT_DESTINATION_TEMPORARY	= 0,
	QB_RESULT_DESTINATION_VARIABLE,
	QB_RESULT_DESTINATION_ELEMENT,
	QB_RESULT_DESTINATION_PROPERTY,
	QB_RESULT_DESTINATION_ARGUMENT,
	QB_RESULT_DESTINATION_PRINT,
	QB_RESULT_DESTINATION_FREE,
	QB_RESULT_DESTINATION_RETURN,
};

struct qb_result_destination {
	qb_result_destination_type type;
	union {
		struct {
			qb_function *function;
			uint32_t index;
		} argument;
		struct {
			qb_operand container;
			qb_operand index;
		} element;
		struct {
			qb_operand container;
			qb_operand name;
		} property;
		qb_operand variable;
	};
	qb_result_prototype *prototype;
};

enum {
	// intrinsic properties of an op
	QB_OP_NEED_LINE_NUMBER			= 0x8000,
	QB_OP_BRANCH					= 0x4000,
	QB_OP_EXIT 						= 0x2000,
	QB_OP_JUMP 						= 0x1000,
	QB_OP_SIDE_EFFECT				= 0x0800,
	QB_OP_PERFORM_WRAP_AROUND		= 0x0400,
	QB_OP_VERSION_AVAILABLE_ELE		= 0x0200,
	QB_OP_VERSION_AVAILABLE_MIO		= 0x0100,
	QB_OP_NEED_INSTRUCTION_STRUCT	= 0x0080,

	// compile time properties
	QB_OP_JUMP_TARGET 				= 0x80000000,
	QB_OP_CANNOT_REMOVE				= 0x40000000,
	QB_OP_COMPILE_TIME_FLAGS		= 0xFFFF0000,
};

struct qb_op {
	qb_opcode opcode;
	uint32_t flags;
	uint32_t operand_count;
	qb_operand *operands;
	uint32_t jump_target_count;
	uint32_t *jump_target_indices;
	uint32_t instruction_offset;
	uint32_t line_number;
};

struct qb_op_info {
	uint16_t flags;
	uint16_t instruction_length;
	const char *instruction_format;
};

struct qb_pointer_SCA {
	void *data_pointer;
};

struct qb_pointer_ELE {
	void *data_pointer;
	uint32_t *index_pointer;
};

struct qb_pointer_ARR {
	void *data_pointer;
	uint32_t *index_pointer;
	uint32_t *count_pointer;
};

struct qb_pointer_PAR {
	uint32_t segment_selector;
	uint32_t segment_offset;
};

#define SCALAR(address)						(address->dimension_count == 0)
#define CONSTANT(address)					(address->flags & QB_ADDRESS_CONSTANT)
#define TEMPORARY(address)					(address->flags & QB_ADDRESS_TEMPORARY)
#define STATIC(address)						(address->flags & QB_ADDRESS_STATIC)
#define SHARED(address)						(address->flags & QB_ADDRESS_SHARED)
#define IN_USE(address)						(address->flags & QB_ADDRESS_IN_USE)
#define READ_ONLY(address)					(address->flags & QB_ADDRESS_READ_ONLY)
#define NON_REUSABLE(address)				(address->flags & QB_ADDRESS_NON_REUSABLE)
#define RESIZABLE(address)					(address->flags & QB_ADDRESS_RESIZABLE)
#define TAGGED(address)						(address->flags & QB_ADDRESS_TAGGED)
#define AUTO_EXPAND(address)				(address->flags & QB_ADDRESS_AUTO_EXPAND)
#define FIXED_LENGTH(address)				CONSTANT(address->array_size_address)
#define VARIABLE_LENGTH(address)			(address->dimension_count > 0 && !CONSTANT(address->array_size_address))
#define MULTIDIMENSIONAL(address)			(address->dimension_count > 1)

// TODO: fix these
#define ARRAY_MEMBER(address)				(address->source_address && address->source_address->dimension_count > address->dimension_count)
#define CAST(address)						(address->source_address && address->source_address->dimension_count == address->dimension_count && address->type != address->source_address->type)

#define CONSTANT_DIMENSION(address, i)		CONSTANT(address->dimension_addresses[(i >= 0) ? i : address->dimension_count + i])
#define DIMENSION_ADDRESS(address, i)		(address->dimension_addresses[(i >= 0) ? i : address->dimension_count + i])
#define ARRAY_SIZE_ADDRESS(address, i)		(address->array_size_addresses[(i >= 0) ? i : address->dimension_count + i])
#define DIMENSION(address, i)				VALUE(U32, DIMENSION_ADDRESS(address, i))

uint32_t qb_get_op_flags(qb_opcode opcode);
uint32_t qb_get_operand_count(qb_opcode opcode);
uint32_t qb_get_instruction_length(qb_opcode opcode);
qb_address_mode qb_get_operand_address_mode(uint32_t opcode, uint32_t operand_index);
int32_t qb_is_operand_write_target(qb_opcode opcode, uint32_t operand_index);
const char *qb_get_op_format(uint32_t opcode);

#endif