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

#ifndef QB_COMPILER_H_
#define QB_COMPILER_H_

typedef struct qb_type_declaration			qb_type_declaration;
typedef struct qb_function_declaration		qb_function_declaration;
typedef struct qb_class_declaration			qb_class_declaration;
typedef struct qb_op						qb_op;
typedef struct qb_operand					qb_operand;
typedef struct qb_array_initializer			qb_array_initializer;
typedef struct qb_compiler_data_pool		qb_compiler_data_pool;
typedef struct qb_compiler_context			qb_compiler_context;
typedef struct qb_build_context				qb_build_context;
typedef struct qb_diagnostics				qb_diagnostics;
typedef struct qb_result_prototype			qb_result_prototype;
typedef struct qb_result_destination		qb_result_destination;
typedef struct qb_temporary_variable		qb_temporary_variable;
typedef struct qb_variable_dimensions		qb_variable_dimensions;

typedef enum qb_operand_type				qb_operand_type;
typedef enum qb_stage						qb_stage;
typedef enum qb_opcode						qb_opcode;
typedef enum qb_diagnostic_type				qb_diagnostic_type;
typedef enum qb_result_destination_type		qb_result_destination_type;
typedef enum qb_matrix_order				qb_matrix_order;

struct qb_type_declaration {
	pcre *regexp;
	const char *name;
	uint32_t name_length;
	ulong hash_value;
	uint32_t type;
	uint32_t flags;
	uint32_t *dimensions;
	uint32_t dimension_count;
	qb_index_alias_scheme **index_alias_schemes;
};

struct qb_result_prototype {
	qb_primitive_type preliminary_type;
	qb_primitive_type final_type;
	uint32_t operand_flags;
	uint32_t address_flags;
	qb_result_prototype *parent;
	qb_result_destination *destination;
};

enum qb_operand_type {
	QB_OPERAND_NONE					= 0,
	QB_OPERAND_ADDRESS_VAR			= 1,
	QB_OPERAND_ADDRESS_ELV			= 2,
	QB_OPERAND_ADDRESS_ARR			= 3,
	QB_OPERAND_ADDRESS_ELC			= 4,
	QB_OPERAND_JUMP_TARGET			= 5,	
	QB_OPERAND_EXTERNAL_SYMBOL		= 6,
	QB_OPERAND_ARGUMENT_COUNT,
	QB_OPERAND_TOTAL_LENGTH,
	QB_OPERAND_ADDRESS,
	QB_OPERAND_ARRAY_INITIALIZER,
	QB_OPERAND_ZEND_CLASS,
	QB_OPERAND_ZVAL,
	QB_OPERAND_ADDRESS_EXT_VAR,
	QB_OPERAND_ADDRESS_EXT_ELV,
	QB_OPERAND_ADDRESS_EXT_ARR,
	QB_OPERAND_GLOBAL_STATIC,
	QB_OPERAND_EMPTY,
	QB_OPERAND_RESULT_PROTOTYPE,

	QB_OPERAND_WRITABLE				= 0x08
};

enum {
	QB_INSTRUCTION_OFFSET			= 0x40000000,
	QB_INSTRUCTION_NEXT 			= 1 | QB_INSTRUCTION_OFFSET,
	QB_INSTRUCTION_END				= 0xFFFFFFFF,
};

enum {
	QB_OP_INDEX_NONE				= 0xFFFFFFFF,
	QB_OP_INDEX_JUMP_TARGET			= 0xFFFFFFFE,
};

#include "qb_opcodes.h"

struct qb_operand {
	qb_operand_type type;
	union {
		qb_address *address;
		zval *constant;
		qb_function *function;
		uint32_t symbol_index;
		uint32_t jump_target_index;
		uint32_t operand_size;
		uint32_t argument_count;
		zend_class_entry *zend_class;
		qb_array_initializer *array_initializer;
		qb_result_prototype *result_prototype;
		void *generic_pointer;
	};
};

struct qb_array_initializer {
	qb_operand *elements;
	uint32_t element_count;
	qb_primitive_type desired_type;
};

struct qb_variable_dimensions {
	uint32_t dimension_count;
	uint32_t array_size;
	qb_address *dimension_addresses[64];
};

enum qb_result_destination_type {
	QB_RESULT_DESTINATION_TEMPORARY	= 0,
	QB_RESULT_DESTINATION_VARIABLE,
	QB_RESULT_DESTINATION_ELEMENT,
	QB_RESULT_DESTINATION_PROPERTY,
	QB_RESULT_DESTINATION_ARGUMENT,
	QB_RESULT_DESTINATION_PRINT,
	QB_RESULT_DESTINATION_FREE,
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

struct qb_temporary_variable {
	qb_operand operand;
	uint32_t last_access_op_index;
};

#include "qb_compiler_php.h"
#include "qb_compiler_pbj.h"

enum {
	// intrinsic properties of an op
	QB_OP_MULTI_ADDRESS				= 0x80000000,
	QB_OP_VARIABLE_LENGTH			= 0x40000000,
	QB_OP_NEED_LINE_NUMBER			= 0x20000000,
	QB_OP_JUMP 						= 0x10000000,
	QB_OP_ISSET 					= 0x01000000,
	QB_OP_UNSET 					= 0x02000000,
	QB_OP_PERFORM_WRAP_AROUND		= 0x04000000,
	QB_OP_VECTORIZED				= 0x08000000,
	QB_OP_NEED_MATRIX_DIMENSIONS	= 0x00100000,

	// compile time properties
	QB_OP_JUMP_TARGET 				= 0x00001000,
	QB_OP_CANNOT_REMOVE				= 0x00002000,
};

#define MM_DIMENSIONS(m1_rows, m1_cols, m2_rows, m2_cols)		(((m1_rows & 0x000003FF) << 20) | ((m1_cols & 0x000003FF) << 10) | (m2_cols & 0x000003FF))
#define MV_DIMENSIONS(m_rows, m_cols, v_width)					MM_DIMENSIONS(m_rows, m_cols, v_width, 1)
#define VM_DIMENSIONS(v_width, m_rows, m_cols)					MM_DIMENSIONS(1, v_width, m_rows, m_cols)
#define V_DIMENSIONS(v_width)									MM_DIMENSIONS(1, v_width, v_width, 1)

struct qb_op {
	uint32_t flags;
	qb_opcode opcode;
	uint32_t operand_count;
	qb_operand *operands;
	uint32_t instruction_offset;
	uint32_t matrix_dimensions;
	uint32_t line_number;
};

struct qb_class_declaration {
	qb_type_declaration **declarations;
	uint32_t declaration_count;
	zend_class_entry *zend_class;
};

struct qb_function_declaration {
	qb_type_declaration **declarations;
	uint32_t declaration_count;
	uint32_t flags;
	const char *import_path;
	uint32_t import_path_length;
	qb_class_declaration *class_declaration;
	zend_function *zend_function;
};

struct qb_compiler_data_pool {
	void ***arrays;
	uint32_t array_count;

	qb_block_allocator *op_allocator;
	qb_block_allocator *address_allocator;
	qb_block_allocator *pointer_allocator;
	qb_block_allocator *operand_allocator;
	qb_block_allocator *array_initializer_allocator;
	qb_block_allocator *index_alias_scheme_allocator;
	qb_block_allocator *string_allocator;
	qb_block_allocator *uint32_allocator;
	qb_block_allocator *type_declaration_allocator;
	qb_block_allocator *variable_allocator;
	qb_block_allocator *function_declaration_allocator;
	qb_block_allocator *class_declaration_allocator;
	qb_block_allocator *result_destination_allocator;

	char * const *op_names;
	char * const *op_actions;
	char * const *op_result_size_codes;
	char * const *op_result_size_variables;
	int32_t * const *op_function_usages;
	char * const *function_prototypes;
	char * const *zend_op_names;
	char * const *pbj_op_names;
};

enum qb_stage {
	QB_STAGE_VARIABLE_INITIALIZATION,
	QB_STAGE_RESULT_TYPE_RESOLUTION,
	QB_STAGE_OPCODE_TRANSLATION,
};

enum qb_matrix_order {
	QB_MATRIX_ORDER_COLUMN_MAJOR	= 0,
	QB_MATRIX_ORDER_ROW_MAJOR		= 1,
};

struct qb_compiler_context {
	qb_op **ops;
	uint32_t op_count;
	uint32_t *op_translations;
	uint32_t line_number;
	uint32_t initialization_op_count;

	qb_compiler_data_pool *pool;

	qb_function function_prototype;
	qb_function_declaration *function_declaration;
	uint32_t function_flags;

	qb_operand **stack_items;
	uint32_t stack_item_buffer_size;
	uint32_t stack_item_count;
	uint32_t stack_item_offset;
	qb_temporary_variable *temp_variables;
	uint32_t temp_variable_count;
	qb_address *foreach_index_address;
	qb_result_prototype *result_prototypes;
	uint32_t result_prototype_count;

	qb_variable *return_variable;
	qb_variable **variables;
	uint32_t variable_count;
	uint32_t argument_count;
	uint32_t required_argument_count;
	qb_storage *storage;
	qb_address **scalars;
	uint32_t scalar_count;
	qb_address **arrays;
	uint32_t array_count;
	qb_external_symbol *external_symbols;
	uint32_t external_symbol_count;
	qb_variable_dimensions result_dimensions;

	zend_function *zend_function;
	zend_function *zend_function_being_called;
	zval *zend_this_object;
	zval *zend_class_name;
	zend_class_entry *zend_class;
	zend_op_array *zend_op_array;
	zend_op *zend_op;
	uint32_t zend_op_index;
	uint32_t jump_target_index1;
	uint32_t jump_target_index2;

	qb_pbj_op *pbj_ops;
	uint32_t pbj_op_count;
	qb_pbj_op *pbj_op;
	uint32_t pbj_op_index;
	uint32_t pbj_op_offset;
	qb_pbj_op **pbj_conditionals;
	uint32_t pbj_conditional_count; 
	qb_pbj_parameter *pbj_parameters;
	uint32_t pbj_parameter_count;
	qb_pbj_parameter *pbj_out_coord;
	qb_pbj_parameter *pbj_out_pixel;
	qb_pbj_texture *pbj_textures;
	uint32_t pbj_texture_count;
	qb_pbj_register *pbj_float_registers;
	uint32_t pbj_float_register_count;
	qb_pbj_register *pbj_int_registers;
	uint32_t pbj_int_register_count;
	qb_address **pbj_int_numerals;
	qb_address **pbj_float_numerals;
	qb_pbj_address pbj_comparison_register;
	const char *pbj_name;
	uint32_t pbj_name_length;
	const char *pbj_vendor;
	const char *pbj_display_name;
	const char *pbj_description;
	uint32_t pbj_version;
	uint8_t *pbj_data;
	uint8_t *pbj_data_end;
	int32_t matrix_padding;
	qb_matrix_order matrix_order;

	char *external_code;
	uint32_t external_code_length;

	uint32_t instruction_length;
	uint64_t instruction_crc64;
	int8_t *instructions;
	void *native_proc;

	int32_t silence;
	qb_stage stage;

	zend_function *previous_function;
	zend_class_entry *previous_class;
	zval *previous_object;
	zval *previous_function_name;

	void ***tsrm_ls;
};

struct qb_build_context {
	qb_compiler_context *compiler_contexts;
	uint32_t compiler_context_count;
	qb_function_declaration **function_declarations;
	uint32_t function_declaration_count;
	qb_class_declaration **class_declarations;
	uint32_t class_declaration_count;

	uint32_t deferral_count;

	qb_compiler_data_pool *pool;
	qb_compiler_data_pool _pool;

	void ***tsrm_ls;
};

enum qb_diagnostic_type {
	QB_DIAGNOSTIC_EMPTY,
	QB_DIAGNOSTIC_INT_ADD,
	QB_DIAGNOSTIC_INT_MUL,
	QB_DIAGNOSTIC_INT_DIV,
	QB_DIAGNOSTIC_INT_MAC,
	QB_DIAGNOSTIC_FLOAT_ADD,
	QB_DIAGNOSTIC_FLOAT_MUL,
	QB_DIAGNOSTIC_FLOAT_DIV,
	QB_DIAGNOSTIC_FLOAT_MAC,
	QB_DIAGNOSTIC_VECTOR_ADD,
	QB_DIAGNOSTIC_VECTOR_MUL,
	QB_DIAGNOSTIC_VECTOR_DIV,
	QB_DIAGNOSTIC_VECTOR_MAC,

	QB_DIAGNOSTIC_SPEED_TEST_COUNTS,
};

struct qb_diagnostics {
	float64_t instruction_speeds[QB_DIAGNOSTIC_SPEED_TEST_COUNTS];
};

enum {
	QB_COERCE_TO_HIGHEST_RANK			= 0x00000001,
	QB_COERCE_TO_LVALUE_TYPE			= 0x00000002,
	QB_COERCE_TO_FIRST_OPERAND_TYPE		= 0x00000004,
	QB_COERCE_TO_SECOND_OPERAND_TYPE	= 0x00000008,
	QB_COERCE_TO_INTEGER				= 0x00000010,
	QB_COERCE_TO_FLOATING_POINT			= 0x00000020,
	QB_COERCE_TO_INTEGER_TO_DOUBLE		= 0x00000040,
	QB_COERCE_TO_BOOLEAN				= 0x00000080,
	QB_COERCE_TO_SIGNED					= 0x00000100,
	QB_COERCE_TO_UNSIGNED				= 0x00000200,
};

#define QB_RESULT_TYPE(flags)			(flags & 0xFF)

enum {
	QB_RESULT_IS_BOOLEAN			= 0x00800000,
	QB_RESULT_IS_STRING				= 0x00400000,

	QB_RESULT_IS_ROW_MAJOR			= 0x00200000,
	QB_RESULT_IS_COLUMN_MAJOR		= 0x00100000,

	QB_RESULT_FROM_PURE_FUNCTION	= 0x00010000,
};

enum {
	QB_CREATE_EXPANDABLE			= 0x00000001,
	QB_CREATE_IN_NEW_SEGMENT		= 0x00000002,
	QB_CREATE_IN_VARIABLE_SEGMENT	= 0x00000004,
};

#define IS_SCALAR(address)						(address->dimension_count == 0)
#define IS_SCALAR_VARIABLE(address)				(address->dimension_count == 0 && address->segment_selector == QB_SELECTOR_VARIABLE && address->segment_offset != QB_OFFSET_INVALID)
#define IS_FIXED_LENGTH_ARRAY(address)			(address->dimension_count > 0 && address->array_size_address->flags & QB_ADDRESS_CONSTANT)
#define IS_VARIABLE_LENGTH_ARRAY(address)		(address->dimension_count > 0 && !(address->array_size_address->flags & QB_ADDRESS_CONSTANT))
#define IS_EXPANDABLE_ARRAY(address)			(address->dimension_count > 0 && !(address->array_size_address->flags & QB_ADDRESS_READ_ONLY))

#define IS_ARRAY_MEMBER(address)				(address->source_address && address->source_address->dimension_count > address->dimension_count)
#define IS_CAST(address)						(address->source_address && address->source_address->dimension_count == address->dimension_count && address->type != address->source_address->type)

#define ARRAY_IN(storage, type, address)		((CTYPE(type) *) (storage->segments[address->segment_selector].memory + address->segment_offset))
#define VALUE_IN(storage, type, address)		*ARRAY_IN(storage, type, address)
#define ARRAY_SIZE_IN(storage, address)			VALUE_IN(storage, U32, address->array_size_address)

#define STORAGE_TYPE_MATCH(type1, type2)		((type1 == type2) || ((type1 & ~QB_TYPE_UNSIGNED) == (type2 & ~QB_TYPE_UNSIGNED) && (type1 < QB_TYPE_F32)))

#define ARRAY(type, address)					ARRAY_IN(cxt->storage, type, address)
#define VALUE(type, address)					VALUE_IN(cxt->storage, type, address)
#define ARRAY_SIZE(address)						VALUE(U32, address->array_size_address)

#define BYTE_COUNT(element_count, type)			((element_count) << type_size_shifts[type])
#define ELEMENT_COUNT(byte_count, type)			((byte_count) >> type_size_shifts[type])

#endif
