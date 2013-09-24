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
typedef struct qb_op_info					qb_op_info;
typedef struct qb_compiler_data_pool		qb_compiler_data_pool;
typedef struct qb_compiler_context			qb_compiler_context;
typedef struct qb_build_context				qb_build_context;
typedef struct qb_diagnostics				qb_diagnostics;
typedef struct qb_variable_dimensions		qb_variable_dimensions;

typedef enum qb_stage						qb_stage;
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
	uint32_t coercion_flags;
	uint32_t address_flags;
	qb_result_prototype *parent;
	qb_result_destination *destination;
};

#include "qb_opcodes.h"

#pragma pack(push,1)

struct qb_op_info {
	uint16_t format_index;
	uint16_t flags;
};

#pragma pack(pop)

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
	qb_block_allocator *index_alias_scheme_allocator;
	qb_block_allocator *string_allocator;
	qb_block_allocator *uint32_allocator;
	qb_block_allocator *type_declaration_allocator;
	qb_block_allocator *variable_allocator;
	qb_block_allocator *function_declaration_allocator;
	qb_block_allocator *class_declaration_allocator;
	qb_block_allocator *array_initializer_allocator;
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

	qb_stage stage;
	qb_compiler_data_pool *pool;

	zend_function *zend_function;

	qb_function function_prototype;
	qb_function_declaration *function_declaration;
	uint32_t function_flags;

	qb_operand **stack_items;
	uint32_t stack_item_buffer_size;
	uint32_t stack_item_count;
	uint32_t stack_item_offset;

	qb_storage *storage;

	qb_variable *return_variable;
	qb_variable **variables;
	uint32_t variable_count;
	uint32_t argument_count;
	uint32_t required_argument_count;
	
	qb_address **scalars;
	uint32_t scalar_count;

	qb_address **arrays;
	uint32_t array_count;

	qb_external_symbol *external_symbols;
	uint32_t external_symbol_count;

	const char *intrinsic_function_name;

	int32_t matrix_padding;
	qb_matrix_order matrix_order;

	char *external_code;
	uint32_t external_code_length;

	uint32_t instruction_length;
	uint32_t instruction_op_count;
	uint64_t instruction_crc64;
	int8_t *instructions;
	uint16_t *instruction_opcodes;
	void *native_proc;

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
	QB_COERCE_TO_LVALUE_TYPE			= 0x00000002,
	QB_COERCE_TO_INTEGER				= 0x00000010,
	QB_COERCE_TO_FLOATING_POINT			= 0x00000020,
	QB_COERCE_TO_INTEGER_TO_DOUBLE		= 0x00000040,
	QB_COERCE_TO_BOOLEAN				= 0x00000080,
	QB_COERCE_TO_SIGNED					= 0x00000100,
	QB_COERCE_TO_UNSIGNED				= 0x00000200,

	QB_RETRIEVE_DEFINITE_TYPE_ONLY		= 0x10000000,
};

#define QB_RESULT_TYPE(flags)			(flags & 0xFF)

enum {
	QB_RESULT_FROM_PURE_FUNCTION	= 0x00000001,
	QB_RESULT_HAS_SIDE_EFFECT		= 0x00000002,
	QB_RESULT_IS_ROW_MAJOR			= 0x00000004,
	QB_RESULT_IS_COLUMN_MAJOR		= 0x00000008,
};

enum {
	QB_CREATE_EXPANDABLE			= 0x00000001,
	QB_CREATE_IN_NEW_SEGMENT		= 0x00000002,
	QB_CREATE_IN_VARIABLE_SEGMENT	= 0x00000004,
};

static zend_always_inline void qb_attach_new_array(qb_compiler_data_pool *pool, void **p_array, uint32_t *p_count, uint32_t item_size, uint32_t initial_capacity) {
	void ***pp_array = (void ***) qb_enlarge_array((void **) &pool->arrays, 1);
	qb_create_array(p_array, p_count, item_size, initial_capacity);
	*pp_array = p_array;
}

static zend_always_inline void *qb_allocate_pointers(qb_compiler_data_pool *pool, uint32_t count) {
	return qb_allocate_items(&pool->pointer_allocator, count);
}

static zend_always_inline char *qb_allocate_string(qb_compiler_data_pool *pool, const char *s, uint32_t len) {
	char *string = qb_allocate_items(&pool->string_allocator, len + 1);
	if(s) {
		memcpy(string, s, len);
	} 
	return string;
}

static zend_always_inline uint32_t *qb_allocate_indices(qb_compiler_data_pool *pool, uint32_t count) {
	return qb_allocate_items(&pool->uint32_allocator, count);
}

static zend_always_inline qb_index_alias_scheme *qb_allocate_index_alias_scheme(qb_compiler_data_pool *pool) {
	return qb_allocate_items(&pool->index_alias_scheme_allocator, 1);
}

static zend_always_inline qb_type_declaration *qb_allocate_type_declaration(qb_compiler_data_pool *pool) {
	return qb_allocate_items(&pool->type_declaration_allocator, 1);
}

static zend_always_inline qb_function_declaration *qb_allocate_function_declaration(qb_compiler_data_pool *pool) {
	qb_function_declaration *f = qb_allocate_items(&pool->function_declaration_allocator, 1);
	qb_attach_new_array(pool, (void **) &f->declarations, &f->declaration_count, sizeof(qb_type_declaration *), 16);
	return f;
}

static zend_always_inline qb_class_declaration *qb_allocate_class_declaration(qb_compiler_data_pool *pool) {
	qb_class_declaration *c = qb_allocate_items(&pool->class_declaration_allocator, 1);
	qb_attach_new_array(pool, (void **) &c->declarations, &c->declaration_count, sizeof(qb_type_declaration *), 16);
	return c;
}

static zend_always_inline qb_result_destination *qb_allocate_result_destination(qb_compiler_data_pool *pool) {
	return qb_allocate_items(&pool->result_destination_allocator, 1);
}

static zend_always_inline void qb_add_function_declaration(qb_build_context *cxt, qb_function_declaration *function_decl) {
	qb_function_declaration **p = qb_enlarge_array((void **) &cxt->function_declarations, 1);
	*p = function_decl;
}

static zend_always_inline void qb_add_variable_declaration(qb_function_declaration *function_decl, qb_type_declaration *var_decl) {
	qb_type_declaration **p = qb_enlarge_array((void **) &function_decl->declarations, 1);
	*p = var_decl;
}

static zend_always_inline void qb_add_class_variable_declaration(qb_class_declaration *class_decl, qb_type_declaration *var_decl) {
	qb_type_declaration **p = qb_enlarge_array((void **) &class_decl->declarations, 1);
	*p = var_decl;
}

static zend_always_inline qb_op *qb_allocate_op(qb_compiler_data_pool *pool) {
	return qb_allocate_items(&pool->op_allocator, 1);
}

static zend_always_inline qb_operand *qb_allocate_operands(qb_compiler_data_pool *pool, uint32_t operand_count) {
	return qb_allocate_items(&pool->operand_allocator, operand_count);
}

static zend_always_inline void qb_add_op(qb_compiler_context *cxt, qb_op *op) {
	qb_op **p = qb_enlarge_array((void **) &cxt->ops, 1);
	*p = op;
}

static zend_always_inline qb_array_initializer *qb_allocate_array_initializer(qb_compiler_data_pool *pool) {
	qb_array_initializer *a = qb_allocate_items(&pool->array_initializer_allocator, 1);
	qb_attach_new_array(pool, (void **) &a->elements, &a->element_count, sizeof(qb_operand), 16);
	a->desired_type = QB_TYPE_ANY;
	return a;
}

static zend_always_inline qb_address *qb_allocate_address(qb_compiler_data_pool *pool) {
	return qb_allocate_items(&pool->address_allocator, 1);
}

static zend_always_inline qb_address **qb_allocate_address_pointers(qb_compiler_data_pool *pool, uint32_t count) {
	return (qb_address **) qb_allocate_pointers(pool, count);
}

static zend_always_inline void qb_add_scalar(qb_compiler_context *cxt, qb_address *address) {
	qb_address **p = qb_enlarge_array((void **) &cxt->scalars, 1);
	*p = address;
}

static zend_always_inline void qb_add_array(qb_compiler_context *cxt, qb_address *address) {
	qb_address **p = qb_enlarge_array((void **) &cxt->arrays, 1);
	*p = address;
}

static zend_always_inline qb_variable *qb_allocate_variable(qb_compiler_data_pool *pool) {
	return qb_allocate_items(&pool->variable_allocator, 1);
}

static zend_always_inline qb_variable *qb_allocate_variables(qb_compiler_data_pool *pool, uint32_t count) {
	return qb_allocate_items(&pool->variable_allocator, count);
}

static zend_always_inline void qb_add_variable(qb_compiler_context *cxt, qb_variable *variable) {
	qb_variable **p = qb_enlarge_array((void **) &cxt->variables, 1);
	*p = variable;
}

uint32_t ZEND_FASTCALL qb_get_op_flags(qb_compiler_context *cxt, uint32_t opcode);
uint32_t ZEND_FASTCALL qb_get_operand_count(qb_compiler_context *cxt, uint32_t opcode);

void ZEND_FASTCALL qb_mark_jump_target(qb_compiler_context *cxt, uint32_t current_qop_index, uint32_t target_index);
void ZEND_FASTCALL qb_mark_as_writable(qb_compiler_context *cxt, qb_address *address);

void ZEND_FASTCALL qb_lock_address(qb_compiler_context *cxt, qb_address *address);
void ZEND_FASTCALL qb_unlock_address(qb_compiler_context *cxt, qb_address *address);
void ZEND_FASTCALL qb_lock_operand(qb_compiler_context *cxt, qb_operand *operand);
void ZEND_FASTCALL qb_unlock_operand(qb_compiler_context *cxt, qb_operand *operand);

qb_primitive_type ZEND_FASTCALL qb_get_highest_rank_type(qb_compiler_context *cxt, qb_operand *operands, uint32_t count, uint32_t flags);
qb_primitive_type ZEND_FASTCALL qb_get_operand_type(qb_compiler_context *cxt, qb_operand *operand, uint32_t flags);

qb_address * ZEND_FASTCALL qb_obtain_constant_S08(qb_compiler_context *cxt, int8_t value);
qb_address * ZEND_FASTCALL qb_obtain_constant_U08(qb_compiler_context *cxt, uint8_t value);
qb_address * ZEND_FASTCALL qb_obtain_constant_S16(qb_compiler_context *cxt, int16_t value);
qb_address * ZEND_FASTCALL qb_obtain_constant_U16(qb_compiler_context *cxt, uint16_t value);
qb_address * ZEND_FASTCALL qb_obtain_constant_S32(qb_compiler_context *cxt, int32_t value);
qb_address * ZEND_FASTCALL qb_obtain_constant_U32(qb_compiler_context *cxt, uint32_t value);
qb_address * ZEND_FASTCALL qb_obtain_constant_S64(qb_compiler_context *cxt, int64_t value);
qb_address * ZEND_FASTCALL qb_obtain_constant_U64(qb_compiler_context *cxt, uint64_t value);
qb_address * ZEND_FASTCALL qb_obtain_constant_F32(qb_compiler_context *cxt, float32_t value);
qb_address * ZEND_FASTCALL qb_obtain_constant_F64(qb_compiler_context *cxt, float64_t value);
qb_address * ZEND_FASTCALL qb_obtain_constant_integer(qb_compiler_context *cxt, int64_t value, qb_primitive_type desired_type);
qb_address * ZEND_FASTCALL qb_obtain_constant_float(qb_compiler_context *cxt, float64_t value, qb_primitive_type desired_type);
qb_address * ZEND_FASTCALL qb_obtain_constant_boolean(qb_compiler_context *cxt, int32_t value);
qb_address * ZEND_FASTCALL qb_obtain_constant(qb_compiler_context *cxt, int64_t value, qb_primitive_type desired_type);
qb_address * ZEND_FASTCALL qb_obtain_constant_zval(qb_compiler_context *cxt, zval *zvalue, qb_primitive_type desired_type);
qb_address * ZEND_FASTCALL qb_obtain_instance_variable(qb_compiler_context *cxt, zval *name);
qb_address * ZEND_FASTCALL qb_obtain_class_variable(qb_compiler_context *cxt, zend_class_entry *class, zval *name);
qb_address * ZEND_FASTCALL qb_obtain_write_target(qb_compiler_context *cxt, qb_primitive_type desired_type, qb_variable_dimensions *dim, qb_result_prototype *result_prototype, uint32_t result_flags);

qb_address * ZEND_FASTCALL qb_obtain_temporary_scalar(qb_compiler_context *cxt, qb_primitive_type desired_type);

int32_t ZEND_FASTCALL qb_find_index_alias(qb_compiler_context *cxt, qb_index_alias_scheme *scheme, zval *name);
qb_variable * ZEND_FASTCALL qb_find_variable(qb_compiler_context *cxt, zend_class_entry *class, zval *name, uint32_t type_mask);
qb_variable * ZEND_FASTCALL qb_find_class_variable(qb_compiler_context *cxt, zend_class_entry *class, zval *name);
qb_variable * ZEND_FASTCALL qb_find_instance_variable(qb_compiler_context *cxt, zval *name);

void ZEND_FASTCALL qb_set_variable_type(qb_compiler_context *cxt, qb_variable *qvar);

qb_address * ZEND_FASTCALL qb_retrieve_array_element(qb_compiler_context *cxt, qb_address *container_address, qb_address *index_address);
qb_address * ZEND_FASTCALL qb_retrieve_named_element(qb_compiler_context *cxt, qb_address *container_address, zval *name);
qb_address * ZEND_FASTCALL qb_retrieve_array_dimensions(qb_compiler_context *cxt, qb_address *address);
qb_address * ZEND_FASTCALL qb_retrieve_array_slice(qb_compiler_context *cxt, qb_address *container_address, qb_address *offset_address, qb_address *length_address);

qb_address * ZEND_FASTCALL qb_retrieve_temporary_copy(qb_compiler_context *cxt, qb_address *address, qb_primitive_type type);
qb_address * ZEND_FASTCALL qb_retrieve_unary_op_result(qb_compiler_context *cxt, void *factory, qb_address *address);
qb_address * ZEND_FASTCALL qb_retrieve_binary_op_result(qb_compiler_context *cxt, void *factory, qb_address *address1, qb_address *address2);

void ZEND_FASTCALL qb_perform_type_coercion(qb_compiler_context *cxt, qb_operand *operand, qb_primitive_type desired_type);
void ZEND_FASTCALL qb_perform_boolean_coercion(qb_compiler_context *cxt, qb_operand *operand);

void ZEND_FASTCALL qb_produce_op(qb_compiler_context *cxt, void *factory, qb_operand *operands, uint32_t operand_count, qb_operand *result, uint32_t *jump_target_indices, uint32_t jump_target_count, qb_result_prototype *result_prototype);
void ZEND_FASTCALL qb_create_op(qb_compiler_context *cxt, void *factory, qb_operand *operands, uint32_t operand_count, qb_operand *result, uint32_t *jump_target_indices, uint32_t jump_target_count, int32_t result_used);
void ZEND_FASTCALL qb_execute_op(qb_compiler_context *cxt, qb_op *op);

#endif
