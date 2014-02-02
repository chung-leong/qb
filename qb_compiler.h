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

typedef struct qb_compiler_context			qb_compiler_context;
typedef struct qb_diagnostics				qb_diagnostics;
typedef struct qb_variable_dimensions		qb_variable_dimensions;
typedef struct qb_jump_target				qb_jump_target;

typedef enum qb_stage						qb_stage;
typedef enum qb_diagnostic_type				qb_diagnostic_type;
typedef enum qb_matrix_order				qb_matrix_order;
typedef enum qb_derived_variable_type		qb_derived_variable_type;
typedef enum qb_translation_type			qb_translation_type;

struct qb_variable_dimensions {
	uint32_t dimension_count;
	qb_address *array_size_address;
	qb_address *dimension_addresses[64];
	qb_address *array_size_addresses[64];
};

struct qb_jump_target {
	uint32_t jump_target_index;
};

#define JUMP_TARGET_INDEX(source_index, offset)				(((offset) << 24) | (source_index))
#define OP_INDEX_OFFSET(jump_target_index)					((int8_t) ((jump_target_index) >> 24))
#define OP_INDEX(jump_target_index)							((jump_target_index) & 0x00FFFFFF)

enum qb_stage {
	QB_STAGE_VARIABLE_INITIALIZATION,
	QB_STAGE_RESULT_TYPE_RESOLUTION,
	QB_STAGE_CONSTANT_EXPRESSION_EVALUATION,
	QB_STAGE_OPCODE_TRANSLATION,
};

enum qb_matrix_order {
	QB_MATRIX_ORDER_COLUMN_MAJOR	= 0,
	QB_MATRIX_ORDER_ROW_MAJOR		= 1,
};

enum qb_translation_type {
	QB_TRANSLATION_PHP				= 1,
	QB_TRANSLATION_PBJ,
};

struct qb_compiler_context {
	qb_op **ops;
	uint32_t op_count;
	uint32_t *op_translation_table;
	uint32_t op_translation_table_size;
	uint32_t source_op_index;
	uint32_t source_file_id;
	uint32_t source_file_line_number;
	uint32_t line_id;

	qb_stage stage;
	qb_data_pool *pool;

	zend_op_array *zend_op_array;

	qb_function *compiled_function;
	qb_function function_prototype;
	qb_function_declaration *function_declaration;
	uint32_t function_flags;

	qb_storage *storage;

	qb_variable *return_variable;
	qb_variable *return_key_variable;
	qb_variable *sent_variable;
	qb_variable **variables;
	uint32_t variable_count;
	uint32_t argument_count;
	uint32_t required_argument_count;
	
	qb_address **constant_scalars;
	uint32_t constant_scalar_count;

	qb_address **writable_scalars;
	uint32_t writable_scalar_count;

	qb_address **constant_arrays;
	uint32_t constant_array_count;

	qb_address **writable_arrays;
	uint32_t writable_array_count;

	qb_address **address_aliases;
	uint32_t address_alias_count;

	qb_expression **on_demand_expressions;
	uint32_t on_demand_expression_count;

	qb_jump_target *jump_targets;
	uint32_t jump_target_count;

	qb_address *zero_address;
	qb_address *one_address;
	qb_address *false_address;
	qb_address *true_address;
	qb_address *empty_array;

	qb_intrinsic_function *intrinsic_function;
	uint32_t argument_offset;
	qb_operand foreach_index;

	int32_t matrix_padding;

	zval **compile_time_results;
	uint32_t compile_time_result_count;

	char *external_code;
	uint32_t external_code_length;

	uint32_t dependency_index;
	int8_t *dependencies;

	qb_translation_type translation;
	void *translator_context;

	int32_t debugger_present;

#ifdef ZTS
	void ***tsrm_ls;
#endif
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

// NOTE: never attach a stack variable
static zend_always_inline void qb_attach_new_array(qb_data_pool *pool, void **p_array, uint32_t *p_count, uint32_t item_size, uint32_t initial_capacity) {
	void ***pp_array = (void ***) qb_enlarge_array((void **) &pool->arrays, 1);
	qb_create_array(p_array, p_count, item_size, initial_capacity);
	*pp_array = p_array;
}

static zend_always_inline void *qb_allocate_pointers(qb_data_pool *pool, uint32_t count) {
	return qb_allocate_items(&pool->pointer_allocator, count);
}

static zend_always_inline char *qb_allocate_string(qb_data_pool *pool, const char *s, uint32_t len) {
	char *string = qb_allocate_items(&pool->string_allocator, len + 1);
	if(s) {
		memcpy(string, s, len);
	} 
	return string;
}

static zend_always_inline uint32_t *qb_allocate_indices(qb_data_pool *pool, uint32_t count) {
	return qb_allocate_items(&pool->uint32_allocator, count);
}

static zend_always_inline qb_index_alias_scheme *qb_allocate_index_alias_scheme(qb_data_pool *pool) {
	return qb_allocate_items(&pool->index_alias_scheme_allocator, 1);
}

static zend_always_inline qb_type_declaration *qb_allocate_type_declaration(qb_data_pool *pool) {
	return qb_allocate_items(&pool->type_declaration_allocator, 1);
}

static zend_always_inline qb_function_declaration *qb_allocate_function_declaration(qb_data_pool *pool) {
	qb_function_declaration *f = qb_allocate_items(&pool->function_declaration_allocator, 1);
	qb_attach_new_array(pool, (void **) &f->declarations, &f->declaration_count, sizeof(qb_type_declaration *), 16);
	return f;
}

static zend_always_inline qb_class_declaration *qb_allocate_class_declaration(qb_data_pool *pool) {
	qb_class_declaration *c = qb_allocate_items(&pool->class_declaration_allocator, 1);
	qb_attach_new_array(pool, (void **) &c->declarations, &c->declaration_count, sizeof(qb_type_declaration *), 16);
	return c;
}

static zend_always_inline qb_result_destination *qb_allocate_result_destination(qb_data_pool *pool) {
	return qb_allocate_items(&pool->result_destination_allocator, 1);
}

static zend_always_inline qb_op *qb_allocate_op(qb_data_pool *pool) {
	return qb_allocate_items(&pool->op_allocator, 1);
}

static zend_always_inline qb_operand *qb_allocate_operands(qb_data_pool *pool, uint32_t operand_count) {
	return qb_allocate_items(&pool->operand_allocator, operand_count);
}

static zend_always_inline void qb_add_op(qb_compiler_context *cxt, qb_op *op) {
	qb_op **p = qb_enlarge_array((void **) &cxt->ops, 1);
	*p = op;
}

static zend_always_inline qb_array_initializer *qb_allocate_array_initializer(qb_data_pool *pool) {
	qb_array_initializer *a = qb_allocate_items(&pool->array_initializer_allocator, 1);
	qb_attach_new_array(pool, (void **) &a->elements, &a->element_count, sizeof(qb_operand), 16);
	a->desired_type = QB_TYPE_ANY;
	return a;
}

static zend_always_inline qb_address *qb_allocate_address(qb_data_pool *pool) {
	return qb_allocate_items(&pool->address_allocator, 1);
}

static zend_always_inline qb_address **qb_allocate_address_pointers(qb_data_pool *pool, uint32_t count) {
	return (qb_address **) qb_allocate_pointers(pool, count);
}

static zend_always_inline qb_expression *qb_allocate_expression(qb_data_pool *pool) {
	return qb_allocate_items(&pool->expression_allocator, 1);
}

static zend_always_inline void qb_add_constant_scalar(qb_compiler_context *cxt, qb_address *address) {
	qb_address **p = qb_enlarge_array((void **) &cxt->constant_scalars, 1);
	*p = address;
}

static zend_always_inline void qb_add_writable_scalar(qb_compiler_context *cxt, qb_address *address) {
	qb_address **p = qb_enlarge_array((void **) &cxt->writable_scalars, 1);
	*p = address;
}

static zend_always_inline void qb_add_constant_array(qb_compiler_context *cxt, qb_address *address) {
	qb_address **p = qb_enlarge_array((void **) &cxt->constant_arrays, 1);
	*p = address;
}

static zend_always_inline void qb_add_writable_array(qb_compiler_context *cxt, qb_address *address) {
	qb_address **p = qb_enlarge_array((void **) &cxt->writable_arrays, 1);
	*p = address;
}

static zend_always_inline qb_variable *qb_allocate_variable(qb_data_pool *pool) {
	return qb_allocate_items(&pool->variable_allocator, 1);
}

static zend_always_inline qb_variable *qb_allocate_variables(qb_data_pool *pool, uint32_t count) {
	return qb_allocate_items(&pool->variable_allocator, count);
}

void qb_mark_as_constant(qb_compiler_context *cxt, qb_address *address);
void qb_mark_as_writable(qb_compiler_context *cxt, qb_address *address);
void qb_mark_as_temporary(qb_compiler_context *cxt, qb_address *address);
void qb_mark_as_shared(qb_compiler_context *cxt, qb_address *address);

void qb_lock_address(qb_compiler_context *cxt, qb_address *address);
void qb_unlock_address(qb_compiler_context *cxt, qb_address *address);
void qb_lock_operand(qb_compiler_context *cxt, qb_operand *operand);
void qb_unlock_operand(qb_compiler_context *cxt, qb_operand *operand);

qb_primitive_type qb_get_highest_rank_type(qb_compiler_context *cxt, qb_operand *operands, uint32_t count, uint32_t flags);
qb_primitive_type qb_get_operand_type(qb_compiler_context *cxt, qb_operand *operand, uint32_t flags);
qb_primitive_type qb_get_property_type(qb_compiler_context *cxt, qb_operand *container, qb_operand *name);
qb_primitive_type qb_get_result_destination_type(qb_compiler_context *cxt, qb_result_destination *destination);

qb_address * qb_obtain_constant_S08(qb_compiler_context *cxt, int8_t value);
qb_address * qb_obtain_constant_U08(qb_compiler_context *cxt, uint8_t value);
qb_address * qb_obtain_constant_S16(qb_compiler_context *cxt, int16_t value);
qb_address * qb_obtain_constant_U16(qb_compiler_context *cxt, uint16_t value);
qb_address * qb_obtain_constant_S32(qb_compiler_context *cxt, int32_t value);
qb_address * qb_obtain_constant_U32(qb_compiler_context *cxt, uint32_t value);
qb_address * qb_obtain_constant_S64(qb_compiler_context *cxt, int64_t value);
qb_address * qb_obtain_constant_U64(qb_compiler_context *cxt, uint64_t value);
qb_address * qb_obtain_constant_F32(qb_compiler_context *cxt, float32_t value);
qb_address * qb_obtain_constant_F64(qb_compiler_context *cxt, float64_t value);
qb_address * qb_obtain_constant_integer(qb_compiler_context *cxt, int64_t value, qb_primitive_type desired_type);
qb_address * qb_obtain_constant_float(qb_compiler_context *cxt, float64_t value, qb_primitive_type desired_type);
qb_address * qb_obtain_constant_boolean(qb_compiler_context *cxt, int32_t value);
qb_address * qb_obtain_constant(qb_compiler_context *cxt, int64_t value, qb_primitive_type desired_type);
qb_address * qb_obtain_constant_indices(qb_compiler_context *cxt, uint32_t *indices, uint32_t index_count);

qb_address * qb_obtain_local_variable(qb_compiler_context *cxt, zval *name);
qb_address * qb_obtain_global_variable(qb_compiler_context *cxt, zval *name);
qb_address * qb_obtain_static_variable(qb_compiler_context *cxt, zval *name);
qb_address * qb_obtain_constant_zval(qb_compiler_context *cxt, zval *zvalue, qb_primitive_type desired_type);
qb_address * qb_obtain_instance_variable(qb_compiler_context *cxt, zval *name);
qb_address * qb_obtain_class_variable(qb_compiler_context *cxt, zend_class_entry *ce, zval *name);
qb_address * qb_obtain_class_static_constant(qb_compiler_context *cxt, zval *name, qb_primitive_type type);
qb_address * qb_obtain_write_target(qb_compiler_context *cxt, qb_primitive_type desired_type, qb_variable_dimensions *dim, uint32_t address_flags, qb_result_prototype *result_prototype, int32_t resizing);
qb_address * qb_obtain_object_property(qb_compiler_context *cxt, qb_operand *container, qb_operand *name, uint32_t bound_check_flags);
qb_address * qb_obtain_result_destination_address(qb_compiler_context *cxt, qb_result_destination *destination);

qb_address * qb_obtain_temporary_variable(qb_compiler_context *cxt, qb_primitive_type element_type, qb_variable_dimensions *dim);
qb_address * qb_create_temporary_variable(qb_compiler_context *cxt, qb_primitive_type element_type, qb_variable_dimensions *dim);

qb_address * qb_create_writable_scalar(qb_compiler_context *cxt, qb_primitive_type element_type);
qb_operand * qb_expand_array_initializer(qb_compiler_context *cxt, qb_array_initializer *initializer, uint32_t required_index);

uint32_t qb_find_index_alias(qb_compiler_context *cxt, qb_index_alias_scheme *scheme, zval *name);
uint32_t qb_get_swizzle_mask(qb_compiler_context *cxt, qb_index_alias_scheme *scheme, zval *name);

qb_variable * qb_find_variable(qb_compiler_context *cxt, zend_class_entry *class, zval *name, uint32_t type_mask);
qb_variable * qb_get_local_variable(qb_compiler_context *cxt, zval *name);
qb_variable * qb_get_global_variable(qb_compiler_context *cxt, zval *name);
qb_variable * qb_get_static_variable(qb_compiler_context *cxt, zval *name);
qb_variable * qb_get_class_variable(qb_compiler_context *cxt, zend_class_entry *ce, zval *name);
qb_variable * qb_get_instance_variable(qb_compiler_context *cxt, zval *name);
uint32_t qb_get_variable_index(qb_compiler_context *cxt, qb_address *address);
uint32_t qb_find_variable_index(qb_compiler_context *cxt, qb_address *address);

int32_t qb_apply_type_declaration(qb_compiler_context *cxt, qb_variable *qvar);

enum {
	QB_ARRAY_BOUND_CHECK_NONE	= 0x0000,
	QB_ARRAY_BOUND_CHECK_READ	= 0x0001,
	QB_ARRAY_BOUND_CHECK_WRITE	= 0x0002,
	QB_ARRAY_BOUND_CHECK_ISSET	= 0x0004,
};

qb_address * qb_obtain_predicate_address(qb_compiler_context *cxt, qb_address *container_address, int32_t writable);
qb_address * qb_obtain_array_element(qb_compiler_context *cxt, qb_address *container_address, qb_address *index_address, uint32_t bound_check_flags);
qb_address * qb_obtain_named_element(qb_compiler_context *cxt, qb_address *container_address, zval *name, uint32_t bound_check_flags);
qb_address * qb_obtain_array_slice(qb_compiler_context *cxt, qb_address *container_address, qb_address *offset_address, qb_address *length_address, uint32_t bound_check_flags);

void qb_attach_bound_checking_expression(qb_compiler_context *cxt, qb_address *address, qb_variable_dimensions *dim, int32_t resizing);
void qb_invalidate_all_on_demand_expressions(qb_compiler_context *cxt);

qb_address * qb_obtain_on_demand_value(qb_compiler_context *cxt, void *op_factory, qb_operand *operands, uint32_t operand_count);
qb_address * qb_obtain_on_demand_sum(qb_compiler_context *cxt, qb_address *augend_address, qb_address *addend_address);
qb_address * qb_obtain_on_demand_difference(qb_compiler_context *cxt, qb_address *minuend_address, qb_address *subtrahend_address);
qb_address * qb_obtain_on_demand_product(qb_compiler_context *cxt, qb_address *multiplicand_address, qb_address *multiplier_address);
qb_address * qb_obtain_on_demand_quotient(qb_compiler_context *cxt, qb_address *numerator_address, qb_address *denominator_address);

qb_address * qb_obtain_string_alias(qb_compiler_context *cxt, qb_address *address);
qb_address * qb_obtain_boolean_alias(qb_compiler_context *cxt, qb_address *address);
qb_address * qb_obtain_reused_alias(qb_compiler_context *cxt, qb_address *address);
qb_address * qb_obtain_cast_alias(qb_compiler_context *cxt, qb_address *address, qb_primitive_type type);
qb_address * qb_obtain_array_alias(qb_compiler_context *cxt, qb_address *address);

qb_address * qb_retrieve_array_dimensions(qb_compiler_context *cxt, qb_address *address);

qb_address * qb_retrieve_temporary_copy(qb_compiler_context *cxt, qb_address *address, qb_primitive_type type);
qb_address * qb_retrieve_unary_op_result(qb_compiler_context *cxt, void *factory, qb_address *address);
qb_address * qb_retrieve_binary_op_result(qb_compiler_context *cxt, void *factory, qb_address *address1, qb_address *address2);

qb_address * qb_create_writable_array(qb_compiler_context *cxt, qb_primitive_type element_type, uint32_t *dimensions, uint32_t dimension_count);
qb_address * qb_create_constant_array(qb_compiler_context *cxt, qb_primitive_type element_type, uint32_t *dimensions, uint32_t dimension_count);
qb_address * qb_create_constant_scalar(qb_compiler_context *cxt, qb_primitive_type element_type);
qb_address * qb_create_address_alias(qb_compiler_context *cxt, qb_address *address);

void qb_perform_type_coercion(qb_compiler_context *cxt, qb_operand *operand, qb_primitive_type desired_type, uint32_t coercion_flags);
void qb_perform_boolean_coercion(qb_compiler_context *cxt, qb_operand *operand);
void qb_update_on_demand_result(qb_compiler_context *cxt, qb_address *address, uint32_t flags);
void qb_allocate_storage_space(qb_compiler_context *cxt, qb_address *address, int32_t need_actual_memory);

int32_t qb_is_source_op_translated(qb_compiler_context *cxt, uint32_t source_index);
uint32_t qb_set_source_op_index(qb_compiler_context *cxt, uint32_t source_index, uint32_t line_number);
void qb_add_jump_target(qb_compiler_context *cxt, uint32_t jump_target_index);
int32_t qb_is_jump_target(qb_compiler_context *cxt, uint32_t jump_target_index);

int32_t qb_produce_op(qb_compiler_context *cxt, void *factory, qb_operand *operands, uint32_t operand_count, qb_operand *result, uint32_t *jump_target_indices, uint32_t jump_target_count, qb_result_prototype *result_prototype);
void qb_create_op(qb_compiler_context *cxt, void *factory, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count, qb_operand *result, uint32_t *jump_target_indices, uint32_t jump_target_count, int32_t result_used);
void qb_execute_op(qb_compiler_context *cxt, void *factory, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count, qb_operand *result);

int32_t qb_add_variables(qb_compiler_context *cxt);
void qb_initialize_function_prototype(qb_compiler_context *cxt);
int32_t qb_load_external_code(qb_compiler_context *cxt, const char *import_path);
void qb_free_external_code(qb_compiler_context *cxt);
void qb_resolve_jump_targets(qb_compiler_context *cxt);
void qb_fuse_instructions(qb_compiler_context *cxt, int32_t pass);
void qb_assign_storage_space(qb_compiler_context *cxt);
void qb_resolve_address_modes(qb_compiler_context *cxt);
void qb_resolve_reference_counts(qb_compiler_context *cxt);
int32_t qb_check_thread_safety(qb_compiler_context *cxt);

void qb_initialize_compiler_context(qb_compiler_context *cxt, qb_data_pool *pool, qb_function_declaration *function_decl, uint32_t dependency_index, uint32_t max_dependency_index TSRMLS_DC);
void qb_free_compiler_context(qb_compiler_context *cxt);

#endif
