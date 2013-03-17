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

extern zend_module_entry qb_module_entry;

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

static zend_always_inline void qb_add_function_declaration(qb_build_context *cxt, qb_function_declaration *function_decl) {
	qb_function_declaration **p = qb_enlarge_array((void **) &cxt->function_declarations, 1);
	*p = function_decl;
}

static int32_t ZEND_FASTCALL qb_find_function_declaration(qb_build_context *cxt, zend_function *zfunc) {
	uint32_t i = 0;
	for(i = 0; i < cxt->function_declaration_count; i++) {
		qb_function_declaration *decl = cxt->function_declarations[i];
		if(decl->zend_function == zfunc) {
			return i;
		}
	}
	return -1;
}

static zend_always_inline void qb_add_class_declaration(qb_build_context *cxt, qb_class_declaration *class_decl) {
	qb_class_declaration **p = qb_enlarge_array((void **) &cxt->class_declarations, 1);
	*p = class_decl;
}

static int32_t ZEND_FASTCALL qb_find_class_declaration(qb_build_context *cxt, zend_class_entry *ce) {
	uint32_t i = 0;
	for(i = 0; i < cxt->class_declaration_count; i++) {
		qb_class_declaration *decl = cxt->class_declarations[i];
		if(decl->zend_class == ce) {
			return i;
		}
	}
	return -1;
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
	a->element_type = QB_TYPE_ANY;
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

extern uint32_t global_op_flags[];
extern uint32_t global_operand_flags[];

static zend_always_inline uint32_t qb_get_op_flags(qb_compiler_context *cxt, uint32_t opcode) {
	return global_op_flags[opcode];
}

static zend_always_inline uint32_t qb_get_operand_flags(qb_compiler_context *cxt, uint32_t opcode, uint32_t operand_index) {
	return (global_operand_flags[opcode] >> (operand_index * 4)) & 0x0F;
}

extern const char compressed_table_op_names[];

static const char * ZEND_FASTCALL qb_get_op_name(qb_compiler_context *cxt, uint32_t opcode) {
	if(!cxt->pool->op_names) {
		// decompress the opname table
		qb_uncompress_table(compressed_table_op_names, (void ***) &cxt->pool->op_names, NULL, 0);
		if(!cxt->pool->op_names) {
			return "?";
		}
	}
	return cxt->pool->op_names[opcode];
}

static uint32_t ZEND_FASTCALL qb_get_operand_count(qb_compiler_context *cxt, uint32_t opcode) {
	uint32_t operand_count = 0;
	uint32_t operand_flags = global_operand_flags[opcode];
	while(operand_flags) {
		operand_count++;
		operand_flags >>= 4;
	}
	return operand_count;
}

static void ZEND_FASTCALL qb_mark_as_writable(qb_compiler_context *cxt, qb_address *address) {
	if(address->flags & QB_ADDRESS_READ_ONLY) {
		address->flags &= ~QB_ADDRESS_READ_ONLY;
		if(address->source_address) {
			qb_mark_as_writable(cxt, address->source_address);
		}
	}
}

static void ZEND_FASTCALL qb_lock_address(qb_compiler_context *cxt, qb_address *address) {
	if(!(address->flags & QB_ADDRESS_IN_USE) && (address->flags & (QB_ADDRESS_TEMPORARY | QB_ADDRESS_QM_TEMPORARY))) {
		address->flags |= QB_ADDRESS_IN_USE;
		if(address->source_address) {
			qb_lock_address(cxt, address->source_address);
		}
		if(address->array_index_address) {
			qb_lock_address(cxt, address->array_index_address);
		}
	}
}

static void ZEND_FASTCALL qb_unlock_address(qb_compiler_context *cxt, qb_address *address) {
	if((address->flags & QB_ADDRESS_IN_USE) && (address->flags & (QB_ADDRESS_TEMPORARY | QB_ADDRESS_QM_TEMPORARY))) {
		address->flags &= ~QB_ADDRESS_IN_USE;
		if(address->source_address) {
			qb_unlock_address(cxt, address->source_address);
		}
		if(address->array_index_address) {
			qb_unlock_address(cxt, address->array_index_address);
		}
	}
}

static void ZEND_FASTCALL qb_mark_as_non_local(qb_compiler_context *cxt, qb_address *address) {
	if(!(address->flags & QB_ADDRESS_NON_LOCAL)) {
		address->flags |= QB_ADDRESS_NON_LOCAL;
		if(address->source_address) {
			qb_mark_as_non_local(cxt, address->source_address);
		}
		if(IS_EXPANDABLE_ARRAY(address)) {
			qb_mark_as_non_local(cxt, address->array_size_addresses[0]);
			qb_mark_as_non_local(cxt, address->dimension_addresses[0]);
		}
	}
}

static uint32_t ZEND_FASTCALL qb_get_jump_target_absolute_index(qb_compiler_context *cxt, uint32_t current_qop_index, uint32_t target_index) {
	if(target_index & QB_INSTRUCTION_OFFSET) {
		int16_t offset = target_index & ~QB_INSTRUCTION_OFFSET;
		return current_qop_index + offset;
	} else {
		if(cxt->op_translations) {
			return cxt->op_translations[target_index];
		} else {
			return target_index;
		}
	}
}

static void ZEND_FASTCALL qb_mark_jump_target(qb_compiler_context *cxt, uint32_t current_qop_index, uint32_t target_index) {
	if(target_index != QB_INSTRUCTION_NEXT && target_index != QB_OP_INDEX_NONE) {
		// flag the target as a jump target or put a placeholder there where it'll go
		uint32_t target_qop_index = qb_get_jump_target_absolute_index(cxt, current_qop_index, target_index);
		if(target_qop_index == QB_OP_INDEX_NONE) {
			cxt->op_translations[target_index] = QB_OP_INDEX_JUMP_TARGET;
		} else if(target_qop_index != QB_OP_INDEX_JUMP_TARGET) {
			qb_op *target = cxt->ops[target_qop_index];
			target->flags |= QB_OP_JUMP_TARGET;
		}
	}
}

static void ZEND_FASTCALL qb_validate_address(qb_compiler_context *cxt, qb_address *address) {
	if(address->type >= QB_TYPE_COUNT) {
		qb_abort("Invalid runtime type id: %x", address->type);
	}
	if(address->segment_selector > 255) {
		qb_abort("Invalid segment selector: %d", address->segment_selector);
	}
	if(!address->array_index_address) {
		if(address->segment_selector == QB_SELECTOR_VARIABLE) {
			qb_memory_segment *segment = &cxt->storage->segments[address->segment_selector];
			uint32_t segment_length = BYTE_COUNT(segment->element_count, segment->type);
			if(address->segment_offset > segment_length) {
				qb_abort("Segment offset exceeds length of segment: %d > %d", address->segment_offset, segment->element_count);
			}
		}
	}
	if(address->source_address) {
		qb_validate_address(cxt, address->source_address);
		if(address->source_address->dimension_count > address->dimension_count) {
			if(address->source_address->type != address->type) {
				qb_abort("Type mismatch");
			}
		} else if(address->source_address->dimension_count < address->dimension_count) {
			qb_abort("Invalid source address");
		}
	}
	if(address->array_size_address) {
		uint32_t i;
		for(i = 0; i < address->dimension_count; i++) {
			qb_validate_address(cxt, address->array_size_addresses[i]);
			qb_validate_address(cxt, address->dimension_addresses[i]);
		}
		if(address->array_size_address != address->array_size_addresses[0]) {
			qb_abort("Invalid size address");
		}
	}
	if(address->array_index_address) {
		qb_validate_address(cxt, address->array_index_address);
	}
	if(address->index_alias_schemes) {
		uint32_t i, j;
		for(i = 0; i < address->dimension_count; i++) {
			qb_index_alias_scheme *scheme = address->index_alias_schemes[i];
			if(scheme) {
				uint32_t dimension = VALUE(U32, address->dimension_addresses[i]);
				if(scheme->dimension != dimension) {
					qb_abort("Index range mismatch");
				}
				for(j = 0; j < scheme->dimension; j++) {
					const char *alias = scheme->aliases[j];
					uint32_t len = strlen(alias);
					if(len != scheme->alias_lengths[j]) {
						qb_abort("String length is incorrect");
					}
				}
			}
		}
	}
}

static qb_op * ZEND_FASTCALL qb_append_op(qb_compiler_context *cxt, uint32_t opcode, uint32_t operand_count) {
	qb_op *op = qb_allocate_op(cxt->pool);
	op->flags = qb_get_op_flags(cxt, opcode);
	op->opcode = opcode;
	op->operands = qb_allocate_operands(cxt->pool, operand_count);
	op->operand_count = operand_count;
	op->line_number = cxt->line_number;
	qb_add_op(cxt, op);
	return op;
}

#include "qb_compiler_op_factories.c"

static void ZEND_FASTCALL qb_initialize_array_address(qb_compiler_context *cxt, uint32_t segment_selector, uint32_t segment_offset, uint32_t type, int32_t owns_segment, qb_address *size_address, qb_address *address) {
	address->mode = QB_ADDRESS_MODE_ARR;
	address->type = type;
	address->flags = (owns_segment) ? QB_ADDRESS_READ_ONLY | QB_ADDRESS_SEGMENT : QB_ADDRESS_READ_ONLY;
	address->segment_selector = segment_selector;
	address->segment_offset = segment_offset;
	address->dimension_count = 1;
	address->dimension_addresses = &address->array_size_address;
	address->array_size_addresses = &address->array_size_address;
	address->array_size_address = size_address;
}

static void ZEND_FASTCALL qb_initialize_subarray_address(qb_compiler_context *cxt, qb_address *container_address, qb_address *address) {
	address->mode = QB_ADDRESS_MODE_ARR;
	address->type = container_address->type;
	address->flags = container_address->flags & ~QB_ADDRESS_SEGMENT;
	address->segment_selector = container_address->segment_selector;
	address->segment_offset = container_address->segment_offset;
	address->dimension_count = container_address->dimension_count - 1;
	address->dimension_addresses = container_address->dimension_addresses + 1;
	address->array_size_addresses = container_address->array_size_addresses + 1;
	address->array_size_address = address->array_size_addresses[0];
	address->source_address = container_address;
	if(container_address->index_alias_schemes) {
		address->index_alias_schemes = container_address->index_alias_schemes + 1;
	}
}

static void ZEND_FASTCALL qb_initialize_element_address(qb_compiler_context *cxt, qb_address *container_address, qb_address *address) {
	address->mode = QB_ADDRESS_MODE_ELC;
	address->type = container_address->type;
	address->flags = container_address->flags & ~QB_ADDRESS_SEGMENT;
	address->segment_selector = container_address->segment_selector;
	address->segment_offset = container_address->segment_offset;
	address->source_address = container_address;
}

static void ZEND_FASTCALL qb_initialize_variable_address(qb_compiler_context *cxt, uint32_t segment_offset, uint32_t type, qb_address *address) {
	address->mode = QB_ADDRESS_MODE_VAR;
	address->type = type;
	address->flags = QB_ADDRESS_READ_ONLY;
	address->segment_selector = QB_SELECTOR_VARIABLE;
	address->segment_offset = segment_offset;
}

static qb_address * ZEND_FASTCALL qb_create_scalar(qb_compiler_context *cxt, uint32_t desired_type) {
	qb_memory_segment *segment = &cxt->storage->segments[QB_SELECTOR_VARIABLE];
	uint32_t element_size = type_sizes[desired_type];
	uint32_t start = ALIGN(segment->element_count, (element_size > 4) ? element_size : 4);
	uint32_t end = start + element_size;
	qb_address *address;
#if ZEND_DEBUG
	if(desired_type >= QB_TYPE_COUNT) {
		qb_abort("Invalid type id");
	}
#endif
	if(end > segment->current_allocation) {
		segment->current_allocation += 256;
		segment->memory = erealloc(segment->memory, segment->current_allocation * sizeof(int8_t));
	}
	segment->element_count = end;

	address = qb_allocate_address(cxt->pool);
	qb_initialize_variable_address(cxt, start, desired_type, address);
	qb_add_scalar(cxt, address);
	return address;
}

static qb_address * ZEND_FASTCALL qb_obtain_constant_I08(qb_compiler_context *cxt, int8_t value, uint32_t sign_flag) {
	qb_address *address;
	uint32_t i, type = QB_TYPE_I08 | sign_flag;
	for(i = 0; i < cxt->scalar_count; i++) {
		address = cxt->scalars[i];
		if((address->flags & QB_ADDRESS_CONSTANT) && (address->type == type)) {
			if(VALUE(I08, address) == value) {
				return address;
			}
		}
	}
	for(i = 0; i < cxt->scalar_count; i++) {
		address = cxt->scalars[i];
		if(address->flags & QB_ADDRESS_CONSTANT && (address->type &~ QB_TYPE_UNSIGNED) == QB_TYPE_I08) {
			if(VALUE(I08, address) == value) {
				qb_address *new_address = qb_allocate_address(cxt->pool);
				*new_address = *address;
				new_address->type = type;
				return new_address;
			}
		}
	}

	address = qb_create_scalar(cxt, type);
	VALUE(I08, address) = value;
	address->flags |= QB_ADDRESS_CONSTANT | QB_ADDRESS_INITIALIZED;
	return address;
}

static qb_address * ZEND_FASTCALL qb_obtain_constant_S08(qb_compiler_context *cxt, int8_t value) {
	return qb_obtain_constant_I08(cxt, value, 0);
}

static qb_address * ZEND_FASTCALL qb_obtain_constant_U08(qb_compiler_context *cxt, uint8_t value) {
	return qb_obtain_constant_I08(cxt, value, QB_TYPE_UNSIGNED);
}

static qb_address * ZEND_FASTCALL qb_obtain_constant_I16(qb_compiler_context *cxt, int16_t value, uint32_t sign_flag) {
	qb_address *address;
	uint32_t i, type = QB_TYPE_I16 | sign_flag;
	for(i = 0; i < cxt->scalar_count; i++) {
		address = cxt->scalars[i];
		if(address->flags & QB_ADDRESS_CONSTANT && address->type == type) {
			if(VALUE(I16, address) == value) {
				return address;
			}
		}
	}
	for(i = 0; i < cxt->scalar_count; i++) {
		address = cxt->scalars[i];
		if(address->flags & QB_ADDRESS_CONSTANT && (address->type &~ QB_TYPE_UNSIGNED) == QB_TYPE_I16) {
			if(VALUE(I16, address) == value) {
				qb_address *new_address = qb_allocate_address(cxt->pool);
				*new_address = *address;
				new_address->type = type;
				return new_address;
			}
		}
	}

	address = qb_create_scalar(cxt, type);
	VALUE(I16, address) = value;
	address->flags |= QB_ADDRESS_CONSTANT | QB_ADDRESS_INITIALIZED;
	return address;
}

static qb_address * ZEND_FASTCALL qb_obtain_constant_S16(qb_compiler_context *cxt, int16_t value) {
	return qb_obtain_constant_I16(cxt, value, 0);
}

static qb_address * ZEND_FASTCALL qb_obtain_constant_U16(qb_compiler_context *cxt, uint16_t value) {
	return qb_obtain_constant_I16(cxt, value, QB_TYPE_UNSIGNED);
}

static qb_address * ZEND_FASTCALL qb_obtain_constant_I32(qb_compiler_context *cxt, int32_t value, uint32_t sign_flag) {
	qb_address *address;
	uint32_t i, type = QB_TYPE_I32 | sign_flag;
	for(i = 0; i < cxt->scalar_count; i++) {
		address = cxt->scalars[i];
		if(address->flags & QB_ADDRESS_CONSTANT && address->type == type) {
			if(VALUE(I32, address) == value) {
				return address;
			}
		}
	}
	for(i = 0; i < cxt->scalar_count; i++) {
		address = cxt->scalars[i];
		if(address->flags & QB_ADDRESS_CONSTANT && (address->type &~ QB_TYPE_UNSIGNED) == QB_TYPE_I32) {
			if(VALUE(I32, address) == value) {
				qb_address *new_address = qb_allocate_address(cxt->pool);
				*new_address = *address;
				new_address->type = type;
				return new_address;
			}
		}
	}

	address = qb_create_scalar(cxt, type);
	VALUE(I32, address) = value;
	address->flags |= QB_ADDRESS_CONSTANT | QB_ADDRESS_INITIALIZED;
	return address;
}

static qb_address * ZEND_FASTCALL qb_obtain_constant_S32(qb_compiler_context *cxt, int32_t value) {
	return qb_obtain_constant_I32(cxt, value, 0);
}

static qb_address * ZEND_FASTCALL qb_obtain_constant_U32(qb_compiler_context *cxt, uint32_t value) {
	return qb_obtain_constant_I32(cxt, value, QB_TYPE_UNSIGNED);
}

static qb_address * ZEND_FASTCALL qb_obtain_constant_I64(qb_compiler_context *cxt, int64_t value, uint32_t sign_flag) {
	qb_address *address;
	uint32_t i, type = QB_TYPE_I64 | sign_flag;
	for(i = 0; i < cxt->scalar_count; i++) {
		address = cxt->scalars[i];
		if(address->flags & QB_ADDRESS_CONSTANT && address->type == type) {
			if(VALUE(I64, address) == value) {
				return address;
			}
		}
	}
	for(i = 0; i < cxt->scalar_count; i++) {
		address = cxt->scalars[i];
		if(address->flags & QB_ADDRESS_CONSTANT && (address->type &~ QB_TYPE_UNSIGNED) == QB_TYPE_I64) {
			if(VALUE(I64, address) == value) {
				qb_address *new_address = qb_allocate_address(cxt->pool);
				*new_address = *address;
				new_address->type = type;
				return new_address;
			}
		}
	}

	address = qb_create_scalar(cxt, type);
	VALUE(I64, address) = value;
	address->flags |= QB_ADDRESS_CONSTANT | QB_ADDRESS_INITIALIZED;
	return address;
}

static qb_address * ZEND_FASTCALL qb_obtain_constant_S64(qb_compiler_context *cxt, int64_t value) {
	return qb_obtain_constant_I64(cxt, value, 0);
}

static qb_address * ZEND_FASTCALL qb_obtain_constant_U64(qb_compiler_context *cxt, uint64_t value) {
	return qb_obtain_constant_I64(cxt, value, QB_TYPE_UNSIGNED);
}

static qb_address * ZEND_FASTCALL qb_obtain_constant_F32(qb_compiler_context *cxt, float32_t value) {
	qb_address *address;
	uint32_t i;
	for(i = 0; i < cxt->scalar_count; i++) {
		address = cxt->scalars[i];
		if(address->flags & QB_ADDRESS_CONSTANT && address->type == QB_TYPE_F32) {
			if(VALUE(F32, address) == value) {
				return address;
			}
		}
	}
	address = qb_create_scalar(cxt, QB_TYPE_F32);
	VALUE(F32, address) = value;
	address->flags |= QB_ADDRESS_CONSTANT | QB_ADDRESS_INITIALIZED;
	return address;
}

static qb_address * ZEND_FASTCALL qb_obtain_constant_F64(qb_compiler_context *cxt, float64_t value) {
	qb_address *address;
	uint32_t i;
	for(i = 0; i < cxt->scalar_count; i++) {
		address = cxt->scalars[i];
		if(address->flags & QB_ADDRESS_CONSTANT && address->type == QB_TYPE_F64) {
			if(VALUE(F64, address) == value) {
				return address;
			}
		}
	}
	address = qb_create_scalar(cxt, QB_TYPE_F64);
	VALUE(F64, address) = value;
	address->flags |= QB_ADDRESS_CONSTANT | QB_ADDRESS_INITIALIZED;
	return address;
}

static qb_address * ZEND_FASTCALL qb_obtain_constant_integer(qb_compiler_context *cxt, int64_t value, uint32_t desired_type) {
	switch(desired_type) {
		case QB_TYPE_S08: return qb_obtain_constant_S08(cxt, (CTYPE(S08)) value);
		case QB_TYPE_U08: return qb_obtain_constant_U08(cxt, (CTYPE(U08)) value);
		case QB_TYPE_S16: return qb_obtain_constant_S16(cxt, (CTYPE(S16)) value);
		case QB_TYPE_U16: return qb_obtain_constant_U16(cxt, (CTYPE(U16)) value);
		case QB_TYPE_S32: return qb_obtain_constant_S32(cxt, (CTYPE(S32)) value);
		case QB_TYPE_U32: return qb_obtain_constant_U32(cxt, (CTYPE(U32)) value);
		case QB_TYPE_S64: return qb_obtain_constant_S64(cxt, (CTYPE(S64)) value);
		case QB_TYPE_U64: return qb_obtain_constant_U64(cxt, (CTYPE(U64)) value);
	}
	return NULL;
}

static qb_address *ZEND_FASTCALL qb_obtain_constant_float(qb_compiler_context *cxt, float64_t value, uint32_t desired_type) {
	switch(desired_type) {
		case QB_TYPE_F32: return qb_obtain_constant_F32(cxt, (CTYPE(F32)) value);
		case QB_TYPE_F64: return qb_obtain_constant_F64(cxt, (CTYPE(F64)) value);
	}
	return NULL;
}

static qb_address * ZEND_FASTCALL qb_obtain_constant(qb_compiler_context *cxt, int64_t value, uint32_t desired_type) {
#if ZEND_DEBUG
	if(desired_type >= QB_TYPE_COUNT) {
		qb_abort("Invalid type id");
	}
#endif
	if(desired_type >= QB_TYPE_F32) {
		return qb_obtain_constant_float(cxt, (float64_t) value, desired_type);
	} else {
		return qb_obtain_constant_integer(cxt, value, desired_type);
	}
}

static qb_address * ZEND_FASTCALL qb_obtain_temporary_scalar(qb_compiler_context *cxt, uint32_t desired_type) {
	qb_address *address;
	uint32_t i;
	for(i = 0; i < cxt->scalar_count; i++) {
		qb_address *address = cxt->scalars[i];
		if((address->flags & QB_ADDRESS_TEMPORARY) && !(address->flags & (QB_ADDRESS_NON_LOCAL | QB_ADDRESS_IN_USE))) {
			if(address->type == desired_type) {
				qb_lock_address(cxt, address);
				return address;
			}
		}
	}
	address = qb_create_scalar(cxt, desired_type);
	address->flags |= QB_ADDRESS_TEMPORARY;
	qb_lock_address(cxt, address);
	return address;
}

static qb_address * ZEND_FASTCALL qb_obtain_qm_temporary_scalar(qb_compiler_context *cxt, uint32_t desired_type) {
	qb_address *address;
	uint32_t i;
	for(i = 0; i < cxt->scalar_count; i++) {
		qb_address *address = cxt->scalars[i];
		if((address->flags & QB_ADDRESS_QM_TEMPORARY) && !(address->flags & QB_ADDRESS_IN_USE)) {
			if(address->type == desired_type) {
				qb_lock_address(cxt, address);
				return address;
			}
		}
	}
	address = qb_create_scalar(cxt, desired_type);
	address->flags |= QB_ADDRESS_QM_TEMPORARY;
	qb_lock_address(cxt, address);
	return address;
}

static qb_address * ZEND_FASTCALL qb_create_fixed_length_array(qb_compiler_context *cxt, uint32_t element_type, uint32_t element_count, uint32_t flags) {
	qb_address *address, *size_address;
	qb_memory_segment *segment;
	uint32_t selector, start, end, element_size = BYTE_COUNT(1, element_type);

#if ZEND_DEBUG
	if(element_type >= QB_TYPE_COUNT) {
		qb_abort("Invalid type id");
	}
#endif
	address = qb_allocate_address(cxt->pool);
	size_address = qb_obtain_constant_U32(cxt, element_count);

	if(flags & QB_CREATE_IN_NEW_SEGMENT) {
		if(cxt->storage->segment_count + 1 < MAX_SEGMENT_COUNT) {
			selector = cxt->storage->segment_count++;
			segment = &cxt->storage->segments[selector];
			segment->element_count = element_count;
			segment->dimension_pointer = ARRAY(U32, size_address);
			segment->array_size_pointer = ARRAY(U32, size_address);
			segment->type = element_type;
			if(BYTE_COUNT(element_count, element_type) <= 8192) {
				segment->flags |= QB_SEGMENT_PREALLOCATED;
			}
			start = 0;
			end = element_count;

			// no memory is allocated at compile-time
		} else {
			qb_abort("Too many memory segments");
		}
	} else {
		if(flags & QB_CREATE_IN_VARIABLE_SEGMENT) {
			selector = QB_SELECTOR_VARIABLE;
		} else {
			selector = QB_SELECTOR_CONSTANT_ARRAY;
		}
		// the first two segments hold objects of various types
		// segment->element_count and current_allocation are in number of bytes
		// this is slightly weird, since bound-checking isn't supposed to be
		// necessary, it's okay for the code to sometimes think that these segments
		// are larger than they actually are
		segment = &cxt->storage->segments[selector];
		start = ALIGN(segment->element_count, (element_size > 4) ? element_size : 4);
		end = start + element_size * element_count;

		if(end > segment->current_allocation) {
			segment->current_allocation += 1024;
			if(end > segment->current_allocation) {
				segment->current_allocation = end;
			}
			segment->memory = erealloc(segment->memory, segment->current_allocation * sizeof(int8_t));
		}
		segment->element_count = end;
	}

	qb_initialize_array_address(cxt, selector, start, element_type, (flags & QB_CREATE_IN_NEW_SEGMENT), size_address, address);
	qb_add_array(cxt, address);
	if(flags & QB_CREATE_IN_NEW_SEGMENT) {
		address->flags |= QB_ADDRESS_SEGMENT;
	}
	return address;
}

static qb_address * ZEND_FASTCALL qb_create_fixed_size_multidimensional_array(qb_compiler_context *cxt, uint32_t element_type, uint32_t *dimensions, uint32_t dimension_count, uint32_t flags) {
	qb_address *address;
	uint32_t array_sizes[MAX_DIMENSION], array_size = 1;
	uint32_t i;

	// figure out the size first
	for(i = dimension_count - 1; (int) i >= 0; i--) {
		array_size *= dimensions[i];
		array_sizes[i] = array_size;
	}
	
	// create array
	address = qb_create_fixed_length_array(cxt, element_type, array_size, flags);

	// if it's multidimensional, create additional constants to store the info
	if(dimension_count > 1) {
		address->dimension_count = dimension_count;
		address->dimension_addresses = qb_allocate_address_pointers(cxt->pool, dimension_count);
		address->array_size_addresses = qb_allocate_address_pointers(cxt->pool, dimension_count);
		for(i = 0; i < dimension_count; i++) {
			qb_address *dimension_address;
			qb_address *array_size_address;
			uint32_t dimension = dimensions[i];
			dimension_address = qb_obtain_constant_U32(cxt, dimension);
			if(i == dimension_count - 1) {
				// at the lowest level, the size is the same as the dimension
				array_size_address = dimension_address;
			} else if(i == 0) {
				// we have the top level size address already
				array_size_address = address->array_size_address;
			} else {
				array_size_address = qb_obtain_constant_U32(cxt, array_sizes[i]);
			}
			address->dimension_addresses[i] = dimension_address;
			address->array_size_addresses[i] = array_size_address;
		}
	}
	return address;
}

static qb_address * ZEND_FASTCALL qb_obtain_string_constant(qb_compiler_context *cxt, const char *string) {
	uint32_t length = strlen(string);
	qb_address *address = qb_create_fixed_length_array(cxt, QB_TYPE_U08, length, 0);
	address->flags |= QB_ADDRESS_CONSTANT | QB_ADDRESS_STRING;
	memcpy(ARRAY(I08, address), string, length);
	return address;
}

static qb_address * ZEND_FASTCALL qb_obtain_temporary_fixed_length_array(qb_compiler_context *cxt, int32_t element_type, uint32_t element_count) {
	qb_address *address;
	uint32_t i;
	for(i = 0; i < cxt->array_count; i++) {
		qb_address *address = cxt->arrays[i];
		if((address->flags & QB_ADDRESS_TEMPORARY) && !(address->flags & QB_ADDRESS_IN_USE)) {
			if(IS_FIXED_LENGTH_ARRAY(address)) {
				if(address->type == element_type) {
					if(VALUE(U32, address->array_size_address) == element_count) {
						qb_lock_address(cxt, address);
						return address;
					}
				}
			}
		}
	}
	address = qb_create_fixed_length_array(cxt, element_type, element_count, FALSE);
	address->flags |= QB_ADDRESS_TEMPORARY;
	qb_lock_address(cxt, address);
	return address;
}

static qb_address * ZEND_FASTCALL qb_obtain_qm_temporary_fixed_length_array(qb_compiler_context *cxt, int32_t element_type, uint32_t element_count) {
	qb_address *address;
	uint32_t i;
	for(i = 0; i < cxt->array_count; i++) {
		qb_address *address = cxt->arrays[i];
		if((address->flags & QB_ADDRESS_QM_TEMPORARY) && !(address->flags & QB_ADDRESS_IN_USE)) {
			if(IS_FIXED_LENGTH_ARRAY(address)) {
				if(address->type == element_type) {
					if(VALUE(U32, address->array_size_address) == element_count) {
						qb_lock_address(cxt, address);
						return address;
					}
				}
			}
		}
	}
	address = qb_create_fixed_length_array(cxt, element_type, element_count, FALSE);
	address->flags |= QB_ADDRESS_QM_TEMPORARY;
	qb_lock_address(cxt, address);
	return address;
}

static qb_address * ZEND_FASTCALL qb_create_variable_length_array(qb_compiler_context *cxt, uint32_t element_type, uint32_t flags) {
	uint32_t selector;
	qb_memory_segment *segment;
	qb_address *address, *size_address;

#if ZEND_DEBUG
	if(element_type >= QB_TYPE_COUNT) {
		qb_abort("Invalid type id");
	}
#endif
	address = qb_allocate_address(cxt->pool);
	size_address = qb_create_scalar(cxt, QB_TYPE_U32);
	VALUE(U32, size_address) = 0;
	if(flags & QB_CREATE_EXPANDABLE) {
		// mark the address as one that can go up automatically
		size_address->flags &= ~QB_ADDRESS_READ_ONLY;
		size_address->flags |= QB_ADDRESS_AUTO_INCREMENT;
	}

	if(cxt->storage->segment_count + 1 < MAX_SEGMENT_COUNT) {
		selector = cxt->storage->segment_count++;
		segment = &cxt->storage->segments[selector];
		segment->type = element_type;
		segment->flags = (flags & QB_CREATE_EXPANDABLE) ? QB_SEGMENT_EXPANDABLE : 0;
		segment->element_count = 0;
		segment->array_size_pointer = ARRAY(U32, size_address);
		segment->dimension_pointer = ARRAY(U32, size_address);
	} else {
		qb_abort("Too many memory segments");
	}

	qb_initialize_array_address(cxt, selector, 0, element_type, TRUE, size_address, address);
	qb_add_array(cxt, address);
	return address;
}

static qb_address * ZEND_FASTCALL qb_create_variable_length_multidimensional_array(qb_compiler_context *cxt, uint32_t element_type, uint32_t *dimensions, uint32_t dimension_count, uint32_t flags) {
	qb_address *address;
	uint32_t array_sizes[MAX_DIMENSION], array_size = 1;
	uint32_t i;

	// figure out the array size at each level first
	for(i = dimension_count - 1; (int) i >= 0; i--) {
		array_size *= dimensions[i];
		array_sizes[i] = array_size;
	}
	
	// create the array
	address = qb_create_variable_length_array(cxt, element_type, flags);

	if(dimension_count > 1) {
		address->dimension_count = dimension_count;
		address->dimension_addresses = qb_allocate_address_pointers(cxt->pool, dimension_count);
		address->array_size_addresses = qb_allocate_address_pointers(cxt->pool, dimension_count);
		// set the dimensions and the array size at each level
		for(i = 0; i < dimension_count; i++) {
			uint32_t dimension = dimensions[i];
			qb_address *dimension_address;
			qb_address *array_size_address;
			if(!dimension) {
				// dimension isn't constant--store it in a variable
				// it'll be set when the function is called
				dimension_address = qb_create_scalar(cxt, QB_TYPE_U32);
				VALUE(U32, dimension_address) = 0;
				if(i == 0 && flags & QB_CREATE_EXPANDABLE) {
					// array can expand in size, meaning so the top-level dimension can change
					dimension_address->flags &= ~QB_ADDRESS_READ_ONLY;
					dimension_address->flags |= QB_ADDRESS_AUTO_INCREMENT;
				}
			} else {
				// dimension is constant
				dimension_address = qb_obtain_constant_U32(cxt, dimension);
			}

			if(i == dimension_count - 1) {
				// at the lowest level, the size is the same as the dimension
				array_size_address = dimension_address;
			} else if(i == 0) {
				// we have the top level already
				array_size_address = address->array_size_address;
			} else {
				uint32_t array_size = array_sizes[i];
				if(!array_size) {
					array_size_address = qb_create_scalar(cxt, QB_TYPE_U32);
					VALUE(U32, array_size_address) = 0;
				} else {
					array_size_address = qb_obtain_constant_U32(cxt, array_size);
				}
			}
			address->array_size_addresses[i] = array_size_address;
			address->dimension_addresses[i] = dimension_address;
		}
	}
	return address;
}

static qb_address * ZEND_FASTCALL qb_obtain_temporary_variable_length_array(qb_compiler_context *cxt, uint32_t desired_type) {
	qb_address *address;
	uint32_t i;
	for(i = 0; i < cxt->array_count; i++) {
		address = cxt->arrays[i];
		if((address->flags & QB_ADDRESS_TEMPORARY) && !(address->flags & QB_ADDRESS_IN_USE)) {
			if(IS_EXPANDABLE_ARRAY(address)) {
				if(address->type == desired_type) {
					address->flags &= ~QB_ADDRESS_STRING;
					qb_create_nullary_op(cxt, &factory_unset, address);
					qb_lock_address(cxt, address);
					return address;
				}
			}
		}
	}
	address = qb_create_variable_length_array(cxt, desired_type, QB_CREATE_EXPANDABLE);
	address->flags |= QB_ADDRESS_TEMPORARY;
	qb_create_nullary_op(cxt, &factory_unset, address);
	qb_lock_address(cxt, address);
	return address;
}

static qb_address * ZEND_FASTCALL qb_obtain_qm_temporary_variable_length_array(qb_compiler_context *cxt, uint32_t desired_type) {
	qb_address *address;
	uint32_t i;
	for(i = 0; i < cxt->array_count; i++) {
		address = cxt->arrays[i];
		if((address->flags & QB_ADDRESS_QM_TEMPORARY) && !(address->flags & QB_ADDRESS_IN_USE)) {
			if(IS_EXPANDABLE_ARRAY(address)) {
				if(address->type == desired_type) {
					qb_lock_address(cxt, address);
					return address;
				}
			}
		}
	}
	address = qb_create_variable_length_array(cxt, desired_type, QB_CREATE_EXPANDABLE);
	address->flags |= QB_ADDRESS_QM_TEMPORARY;
	qb_lock_address(cxt, address);
	return address;
}

static qb_address * ZEND_FASTCALL qb_obtain_temporary_variable(qb_compiler_context *cxt, uint32_t desired_type, qb_address *size_address) {
	if(size_address) {
		uint32_t element_count = VALUE(U32, size_address);
		if(size_address->flags & QB_ADDRESS_CONSTANT) {
			return qb_obtain_temporary_fixed_length_array(cxt, desired_type, element_count);
		} else {
			return qb_obtain_temporary_variable_length_array(cxt, desired_type);
		}
	} else {
		return qb_obtain_temporary_scalar(cxt, desired_type);
	}
}

static qb_address * ZEND_FASTCALL qb_obtain_qm_temporary_variable(qb_compiler_context *cxt, uint32_t desired_type, qb_address *size_address) {
	if(size_address) {
		uint32_t element_count = VALUE(U32, size_address);
		if(size_address->flags & QB_ADDRESS_CONSTANT) {
			return qb_obtain_qm_temporary_fixed_length_array(cxt, desired_type, element_count);
		} else {
			return qb_obtain_qm_temporary_variable_length_array(cxt, desired_type);
		}
	} else {
		return qb_obtain_qm_temporary_scalar(cxt, desired_type);
	}
}

static qb_address * ZEND_FASTCALL qb_allocate_constant(qb_compiler_context *cxt, uint32_t desired_type, qb_address *size_address) {
	qb_address *address;
	if(size_address) {
		uint32_t element_count = VALUE(U32, size_address);
		address = qb_create_fixed_length_array(cxt, desired_type, element_count, FALSE);
		address->flags |= QB_ADDRESS_CONSTANT | QB_ADDRESS_INITIALIZED;
	} else {
		address = qb_create_scalar(cxt, desired_type);
		address->flags |= QB_ADDRESS_CONSTANT | QB_ADDRESS_INITIALIZED;
	}
	return address;
}

static qb_operand * ZEND_FASTCALL qb_push_stack_item(qb_compiler_context *cxt) {
	if(cxt->stack_item_offset + cxt->stack_item_count + 1 > cxt->stack_item_buffer_size) {
		uint32_t i;
		cxt->stack_item_buffer_size += 2;		// TODO: make it bigger
		cxt->stack_items = erealloc(cxt->stack_items, cxt->stack_item_buffer_size * sizeof(qb_operand *));
		for(i = 0; i < 2; i++) {
			cxt->stack_items[cxt->stack_item_offset + cxt->stack_item_count + i] = qb_allocate_operands(cxt->pool, 1);
		}
	}
	return cxt->stack_items[cxt->stack_item_offset + cxt->stack_item_count++];
}

static qb_operand ** ZEND_FASTCALL qb_get_stack_items(qb_compiler_context *cxt, int32_t index) {
	if(index < 0) {
		qb_abort("Stack underflow");
	}
	return &cxt->stack_items[cxt->stack_item_offset + index];
}

static qb_operand * ZEND_FASTCALL qb_pop_stack_item(qb_compiler_context *cxt) {
	qb_operand **items = qb_get_stack_items(cxt, cxt->stack_item_count - 1);
	qb_operand *item = items[0];
	cxt->stack_item_count--;
	return item;
}

static void ZEND_FASTCALL qb_preserve_stack(qb_compiler_context *cxt, uint32_t *p_original_offset, uint32_t *p_original_count) {
	uint32_t current_stack_item_offset = cxt->stack_item_offset;
	uint32_t current_stack_item_count = cxt->stack_item_count;
	uint32_t i;
	cxt->stack_item_offset = cxt->stack_item_offset + cxt->stack_item_count;
	cxt->stack_item_count = 0;
	for(i = 0; i < current_stack_item_count; i++) {
		qb_operand *stack_item = cxt->stack_items[current_stack_item_offset + i];
		qb_operand *new_stack_item = qb_push_stack_item(cxt);
		*new_stack_item = *stack_item;
	}
	*p_original_offset = current_stack_item_offset;
	*p_original_count = current_stack_item_count;
}

static void ZEND_FASTCALL qb_restore_stack(qb_compiler_context *cxt, uint32_t original_offset, uint32_t original_count) {
	uint32_t i;

	// unlock temporary addresses
	for(i = 0; i < cxt->stack_item_count; i++) {
		qb_operand *new_stack_item = cxt->stack_items[cxt->stack_item_offset + i];

		if(new_stack_item->type == QB_OPERAND_ADDRESS) {
			qb_unlock_address(cxt, new_stack_item->address);
		}
	}

	// lock temporary addresses on the original stack
	for(i = 0; i < original_count; i++) {
		qb_operand *old_stack_item = cxt->stack_items[original_offset + i];

		if(old_stack_item->type == QB_OPERAND_ADDRESS) {
			// not restoring the lock on QM temporary, so that the address
			// pushed on the stack by the JMP_SET handler will get reused by
			// the QM_ASSIGN handler
			if(!(old_stack_item->address->flags & QB_ADDRESS_QM_TEMPORARY)) {
				qb_lock_address(cxt, old_stack_item->address);
			}
		}
	}

	cxt->stack_item_offset = original_offset;
	cxt->stack_item_count = original_count;
}

static void ZEND_FASTCALL qb_push_address(qb_compiler_context *cxt, qb_address *address) {
	qb_operand *stack_item = qb_push_stack_item(cxt);
#if ZEND_DEBUG
	qb_validate_address(cxt, address);
#endif
	stack_item->type = QB_OPERAND_ADDRESS;
	stack_item->address = address;
}

static qb_operand * ZEND_FASTCALL qb_expand_array_initializer(qb_compiler_context *cxt, qb_array_initializer *initializer, uint32_t required_index) {
	int32_t addition = (required_index + 1) - initializer->element_count;
	if(addition > 0) {
		qb_operand *new_elements = qb_enlarge_array((void **) &initializer->elements, addition);
		return &new_elements[addition - 1];
	} else {
		// it's been allocated already
		return &initializer->elements[required_index];
	}
}

static void ZEND_FASTCALL qb_copy_element_from_zval(qb_compiler_context *cxt, zval *zvalue, qb_address *address);
static void ZEND_FASTCALL qb_copy_elements_from_zend_array(qb_compiler_context *cxt, zval *zvalue, qb_address *address);
static uint32_t ZEND_FASTCALL qb_get_zend_array_dimension_count(qb_compiler_context *cxt, zval *zvalue, int32_t element_type);
static void ZEND_FASTCALL qb_get_zend_array_dimensions(qb_compiler_context *cxt, zval *zvalue, int32_t element_type, uint32_t *dimensions, uint32_t dimension_count);
static uint32_t ZEND_FASTCALL qb_get_zval_type(qb_compiler_context *cxt, zval *zvalue, uint32_t flags);
static qb_address * ZEND_FASTCALL qb_obtain_zval_constant(qb_compiler_context *cxt, zval *zvalue, uint32_t desired_type);

static uint32_t ZEND_FASTCALL qb_get_array_initializer_dimension_count(qb_compiler_context *cxt, qb_array_initializer *initializer, uint32_t element_type) {
	uint32_t overall_sub_array_dimension_count = 0, i;

	for(i = 0; i < initializer->element_count; i++) {
		qb_operand *element = &initializer->elements[i];
		uint32_t sub_array_dimension_count;

		if(element->type == QB_OPERAND_ARRAY_INITIALIZER) {
			sub_array_dimension_count = qb_get_array_initializer_dimension_count(cxt, element->array_initializer, element_type);
		} else if(element->type == QB_OPERAND_ZVAL) {
			sub_array_dimension_count = qb_get_zend_array_dimension_count(cxt, element->constant, element_type);
		} else if(element->type == QB_OPERAND_ADDRESS && !IS_SCALAR(element->address)) {
			sub_array_dimension_count = element->address->dimension_count;
		} else {
			continue;
		}
		if(overall_sub_array_dimension_count) {
			if(overall_sub_array_dimension_count != sub_array_dimension_count) {
				qb_abort("Array has irregular structure");
			}
		} else {
			overall_sub_array_dimension_count = sub_array_dimension_count;
		}
	}
	if(overall_sub_array_dimension_count + 1 > MAX_DIMENSION) {
		qb_abort("Array initializer has too many dimensions");
	}
	return overall_sub_array_dimension_count + 1;
}

static void ZEND_FASTCALL qb_get_array_initializer_dimensions(qb_compiler_context *cxt, qb_array_initializer *initializer, uint32_t element_type, uint32_t *dimensions, uint32_t dimension_count) {
	uint32_t i, j;
	if(initializer->element_count > dimensions[0]) {
		dimensions[0] = initializer->element_count;
	}
	if(dimension_count > 1) {
		dimensions[1] = 0;
		for(i = 0; i < initializer->element_count; i++) {
			qb_operand *element = &initializer->elements[i];
			if(element->type == QB_OPERAND_ARRAY_INITIALIZER) {
				qb_get_array_initializer_dimensions(cxt, element->array_initializer, element_type, dimensions + 1, dimension_count - 1);
			} else if(element->type == QB_OPERAND_ZVAL) {
				qb_get_zend_array_dimensions(cxt, element->constant, element_type, dimensions + 1, dimension_count - 1);
			} else if(element->type == QB_OPERAND_ADDRESS && !IS_SCALAR(element->address)) {
				for(j = 0; j < element->address->dimension_count; j++) {
					qb_address *dimension_address = element->address->dimension_addresses[j];
					uint32_t sub_array_dimension = VALUE(U32, dimension_address);
					if(sub_array_dimension > dimensions[1 + j]) {
						dimensions[1 + j] = sub_array_dimension;
					}
				}
			}
		}
	}
}

static void ZEND_FASTCALL qb_do_assignment(qb_compiler_context *cxt, qb_address *value_address, qb_address *variable_address);
static void ZEND_FASTCALL qb_do_type_coercion(qb_compiler_context *cxt, qb_operand *operand, uint32_t desired_type);

static void ZEND_FASTCALL qb_copy_elements_from_array_initializer(qb_compiler_context *cxt, qb_array_initializer *initializer, qb_address *address) {
	qb_address *dimension_address = address->dimension_addresses[0];
	// put a qb_address structure on the call stack instead of allocating new ones
	qb_address _item_address, *item_address = &_item_address;
	uint32_t item_element_count, item_byte_count, dimension = VALUE(U32, dimension_address), i;
	memset(item_address, 0, sizeof(qb_address));

	item_address->dimension_count = address->dimension_count - 1;
	item_address->type = address->type;
	item_address->flags = address->flags;
	item_address->segment_selector = address->segment_selector;
	item_address->segment_offset = address->segment_offset;
	if(item_address->dimension_count > 0) {
		item_address->mode = QB_ADDRESS_MODE_ARR;
		item_address->dimension_addresses = address->dimension_addresses + 1;
		item_address->array_size_addresses = address->array_size_addresses + 1;
		item_address->array_size_address = item_address->array_size_addresses[0];
		if(item_address->index_alias_schemes) {
			item_address->index_alias_schemes = address->index_alias_schemes + 1;
		}
		item_element_count = ARRAY_SIZE(item_address);
	} else {
		item_address->mode = QB_ADDRESS_MODE_ELC;
		item_element_count = 1;
	}
	item_byte_count = BYTE_COUNT(item_element_count, item_address->type);

	if(address->source_address) {
		// make sure source_address points to an address created by qb_create_address()
		// instead of the structure on the stack we're using here (_sub_array_address)
		item_address->source_address = address->source_address;
	} else {
		// the first call
		item_address->source_address = address;
	}

	for(i = 0; i < dimension; i++) {
		qb_operand *element = &initializer->elements[i];
		if(element->type == QB_OPERAND_ARRAY_INITIALIZER) {
			qb_copy_elements_from_array_initializer(cxt, element->array_initializer, item_address);
		} else if(element->type == QB_OPERAND_ZVAL) {
			if(item_address->dimension_count > 0) {
				qb_copy_elements_from_zend_array(cxt, element->constant, item_address);
			} else {
				qb_copy_element_from_zval(cxt, element->constant, item_address);
			}
		} else if(element->type == QB_OPERAND_ADDRESS) {
			if(element->address->flags & QB_ADDRESS_CONSTANT) {
				if(item_address->dimension_count > 0) {
					uint32_t initializer_element_count = ARRAY_SIZE(element->address);
					qb_copy_elements(element->address->type, ARRAY(I08, element->address), initializer_element_count, item_address->type, ARRAY(I08, item_address), item_element_count);
				} else {
					qb_copy_element(element->address->type, ARRAY(I08, element->address), item_address->type, ARRAY(I08, item_address));
				}
			} else {
				// need an address from qb_create_address() instead of this fake one on the stack
				// since we're attaching it to an op
				qb_address *item_address_copy = qb_allocate_address(cxt->pool);
				item_address->flags &= ~(QB_ADDRESS_CONSTANT | QB_ADDRESS_READ_ONLY);
				item_address->flags |= QB_ADDRESS_TEMPORARY | QB_ADDRESS_NON_LOCAL;
				*item_address_copy = *item_address;
				qb_do_assignment(cxt, element->address, item_address_copy);
			}
		} else {
			memset(ARRAY(I08, item_address), 0, item_byte_count);
		}
		item_address->segment_offset += item_byte_count;
	}
	if(!(item_address->flags & QB_ADDRESS_CONSTANT)) {
		// if an item isn't constant, the array itself isn't constant either
		address->flags &= ~(QB_ADDRESS_CONSTANT | QB_ADDRESS_READ_ONLY);
		address->flags |= QB_ADDRESS_TEMPORARY;
	}
}

static qb_address * ZEND_FASTCALL qb_obtain_array_from_initializer(qb_compiler_context *cxt, qb_array_initializer *initializer, uint32_t desired_type) {
	qb_address *address;

	// figure out the dimensions of the array first
	uint32_t dimensions[MAX_DIMENSION];
	uint32_t dimension_count = qb_get_array_initializer_dimension_count(cxt, initializer, desired_type);
	dimensions[0] = 0;
	qb_get_array_initializer_dimensions(cxt, initializer, desired_type, dimensions, dimension_count);

	// create a local array for it
	address = qb_create_fixed_size_multidimensional_array(cxt, desired_type, dimensions, dimension_count, FALSE);
	address->flags |= QB_ADDRESS_CONSTANT | QB_ADDRESS_INITIALIZED;

	// copy the elements
	qb_copy_elements_from_array_initializer(cxt, initializer, address);
	return address;
}

#define FOUND_GROUP(group)		(offsets[group * 2] != offsets[group * 2 + 1])
#define GROUP_OFFSET(group)		(offsets[group * 2])
#define GROUP_LENGTH(group)		(offsets[group * 2 + 1] - offsets[group * 2])

#define DOC_COMMENT_FUNCTION_REGEXP	"\\*\\s*@(?:(engine)|(import)|(param)|(local)|(static|staticvar)|(global)|(var)|(property)|(return))\\s+(.*?)\\s*(?:\\*+\\/)?$"

enum {
	FUNC_DECL_ENGINE = 1,
	FUNC_DECL_IMPORT,
	FUNC_DECL_PARAM,
	FUNC_DECL_LOCAL,
	FUNC_DECL_STATIC,
	FUNC_DECL_GLOBAL,
	FUNC_DECL_VAR,
	FUNC_DECL_PROPERTY,
	FUNC_DECL_RETURN,
	FUNC_DECL_DATA,
};

ulong func_decl_hashes[10];

#define TYPE_DECL_REGEXP			"^\\s*(?:(?:(u?int(\\d*))|(float(\\d*)))|(void)|(integer)|(double)|(char)|(bool|boolean)|(string)|(image[34]?)|(array)|(object)|(resource)|(mixed)|(callback))\\s*((?:\\[.*?\\])*)\\s*"

enum {
	TYPE_DECL_INT = 1,
	TYPE_DECL_INT_WIDTH,
	TYPE_DECL_FLOAT,
	TYPE_DECL_FLOAT_WIDTH,
	TYPE_DECL_VOID,
	TYPE_DECL_INTEGER,
	TYPE_DECL_DOUBLE,
	TYPE_DECL_CHAR,
	TYPE_DECL_BOOLEAN,
	TYPE_DECL_STRING,
	TYPE_DECL_IMAGE,
	TYPE_DECL_ARRAY,
	TYPE_DECL_OBJECT,
	TYPE_DECL_RESOURCE,
	TYPE_DECL_MIXED,
	TYPE_DECL_CALLBACK,
	TYPE_DECL_DIMENSIONS,
};

#define TYPE_DIM_REGEXP				"\\[\\s*(?:(0x[0-9a-f]+|\\d*)|([A-Z_][A-Z0-9_]*)|(\\*?))\\s*\\]\\s*"

enum {
	TYPE_DIM_INT = 1,
	TYPE_DIM_CONSTANT,
	TYPE_DIM_ASTERISK,
};

#define TYPE_DIM_ALIAS_REGEXP		"\\[\\s*(?:(\\w+)\\s*:)?\\s*((?:[A-Z_][A-Z0-9_]*\\s*,\\s*)+[A-Z_][A-Z0-9_]*)\\s*\\]"

enum {
	TYPE_DIM_CLASS = 1,
	TYPE_DIM_NAMES,
};

#define IDENTIFIER_REGEXP			"^\\$(?:(\\w+)|(\\S+))"

enum {
	ID_NORMAL = 1,
	ID_PATTERN = 2,
};

pcre *doc_comment_function_regexp;
pcre *type_decl_regexp;
pcre *type_dim_regexp;
pcre *type_dim_alias_regexp;
pcre *identifier_regexp;

static int32_t ZEND_FASTCALL qb_parse_type_dimension(qb_compiler_data_pool *pool, const char *s, uint32_t len, qb_type_declaration *decl, uint32_t dimension_index) {
	int offsets[64], matches;
	uint32_t dimension = 0;

	matches = pcre_exec(type_dim_regexp, NULL, s, len, 0, 0, offsets, sizeof(offsets) / sizeof(int));
	if(matches > 0) {
		if(FOUND_GROUP(TYPE_DIM_INT)) {
			const char *number = s + GROUP_OFFSET(TYPE_DIM_INT);
			dimension = strtol(number, NULL, 0);
		} else if(FOUND_GROUP(TYPE_DIM_CONSTANT)) {
			TSRMLS_FETCH();
			zend_constant *zconst;
			uint32_t name_len = GROUP_LENGTH(TYPE_DIM_CONSTANT);
			ALLOCA_FLAG(use_heap)
			char *name = do_alloca(name_len + 1, use_heap);
			memcpy(name, s + GROUP_OFFSET(TYPE_DIM_CONSTANT), name_len);
			name[name_len] = '\0';
			if(zend_hash_find(EG(zend_constants), name, name_len + 1, (void **) &zconst) != FAILURE) {
				if(Z_TYPE(zconst->value) == IS_LONG) {
					long const_value = Z_LVAL(zconst->value);
					if(const_value <= 0) {
						qb_abort("Constant '%s' is not a positive integer", name);
					}
					dimension = const_value;
				} else if(Z_TYPE(zconst->value) == IS_STRING) {
					char *expanded;
					uint32_t expanded_len = spprintf(&expanded, 0, "[%.*s]", Z_STRLEN(zconst->value), Z_STRVAL(zconst->value));
					int32_t processed = qb_parse_type_dimension(pool, expanded, expanded_len, decl, dimension_index);
					efree(expanded);
					return (processed == -1) ? -1 : offsets[1];
				} else {
					decl = NULL;
				}
			} else {
				qb_abort("Undefined constant '%s'", name);
			}
			free_alloca(name, use_heap);
		} else if(FOUND_GROUP(TYPE_DIM_ASTERISK)) {
			if(dimension_index == 0) {
				decl->flags |= QB_TYPE_DECL_EXPANDABLE;
			}
			dimension = 0;
		} else {
			dimension = 0;
		}
	} else {
		matches = pcre_exec(type_dim_alias_regexp, NULL, s, len, 0, 0, offsets, sizeof(offsets) / sizeof(int));
		if(matches > 0 && FOUND_GROUP(TYPE_DIM_NAMES)) {
			const char *names = s + GROUP_OFFSET(TYPE_DIM_NAMES), *class_name = s + GROUP_OFFSET(TYPE_DIM_CLASS);
			uint32_t names_len = GROUP_LENGTH(TYPE_DIM_NAMES), class_name_len = GROUP_LENGTH(TYPE_DIM_CLASS);
			uint32_t alias_count, alias_start, alias_len, i;
			qb_index_alias_scheme *scheme;						

			// count the number of commas
			for(i = 0, dimension = 1; i < names_len; i++) {
				if(names[i] == ',') {
					dimension++;
				}
			}
			scheme = qb_allocate_index_alias_scheme(pool);
			scheme->aliases = qb_allocate_pointers(pool, dimension);
			scheme->alias_lengths = qb_allocate_indices(pool, dimension);
			scheme->dimension = dimension;
			// divide the string into substrings
			for(i = 0, alias_count = 0, alias_start = 0, alias_len = 0; i <= names_len; i++) {
				if(names[i] == ' ' || names[i] == '\t' || names[i] == '\n' || names[i] == '\r' || names[i] == ',' || i == names_len) {
					if(alias_len > 0) {
						scheme->aliases[alias_count] = qb_allocate_string(pool, names + alias_start, alias_len);
						scheme->alias_lengths[alias_count] = alias_len;
						alias_count++;
						alias_len = 0;
					}
					alias_start = i + 1;
				} else {
					alias_len++;
				}
			}
			if(class_name_len) {
				scheme->class_name = qb_allocate_string(pool, class_name, class_name_len);
				scheme->class_name_length = class_name_len;
			}
			if(!decl->index_alias_schemes) {
				decl->index_alias_schemes = qb_allocate_pointers(pool, decl->dimension_count);
				memset(decl->index_alias_schemes, 0, sizeof(qb_index_alias_scheme *) * decl->dimension_count);
			}
			decl->dimensions[dimension_index] = dimension;
			decl->index_alias_schemes[dimension_index] = scheme;
			decl->flags |= QB_TYPE_DECL_HAS_ALIAS_SCHEMES;
		} else {
			return -1;
		}
	}
	decl->dimensions[dimension_index] = dimension;
	return offsets[1];
}

static qb_type_declaration * ZEND_FASTCALL qb_parse_type_declaration(qb_compiler_data_pool *pool, const char *s, uint32_t len, uint32_t var_type) {
	qb_type_declaration *decl = NULL;
	int offsets[64], matches;

	matches = pcre_exec(type_decl_regexp, NULL, s, len, 0, 0, offsets, sizeof(offsets) / sizeof(int));
	if(matches != -1) {
		uint32_t end_index = offsets[1];

		decl = qb_allocate_type_declaration(pool);
		decl->flags = var_type;

		if(FOUND_GROUP(TYPE_DECL_INT)) {
			const char *inttype = s + GROUP_OFFSET(TYPE_DECL_INT);
			const char *width = s + GROUP_OFFSET(TYPE_DECL_INT_WIDTH);
			uint32_t width_len = GROUP_LENGTH(TYPE_DECL_INT_WIDTH);
			uint32_t sign_flag = (inttype[0] == 'u') ? QB_TYPE_UNSIGNED : 0;
			if(width_len == 2 && width[0] == '6' && width[1] == '4') {
				decl->type = QB_TYPE_I64 | sign_flag;
			} else if(width_len == 2 && width[0] == '3' && width[1] == '2') {
				decl->type = QB_TYPE_I32 | sign_flag;
			} else if(width_len == 2 && width[0] == '1' && width[1] == '6') {
				decl->type = QB_TYPE_I16 | sign_flag;
			} else if(width_len == 1 && width[0] == '8') {
				decl->type = QB_TYPE_I08 | sign_flag;
			} else if(width_len == 0) {
				decl->type = QB_TYPE_I32;
			} else {
				return NULL;
			}
		} else if(FOUND_GROUP(TYPE_DECL_FLOAT)) {
			const char *width = s + GROUP_OFFSET(TYPE_DECL_FLOAT_WIDTH);
			uint32_t width_len = GROUP_LENGTH(TYPE_DECL_FLOAT_WIDTH);
			if(width_len == 2 && width[0] == '6' && width[1] == '4') {
				decl->type = QB_TYPE_F64;
			} else if(width_len == 2 && width[0] == '3' && width[1] == '2') {
				decl->type = QB_TYPE_F32;
			} else if(width_len == 0) {
				decl->type = QB_TYPE_F32;
			} else {
				return NULL;
			}
		} else if(FOUND_GROUP(TYPE_DECL_VOID)) {
			decl->type = QB_TYPE_VOID;
		} else if(FOUND_GROUP(TYPE_DECL_INTEGER)) {
			decl->type = QB_TYPE_I32;
		} else if(FOUND_GROUP(TYPE_DECL_DOUBLE)) {
			decl->type = QB_TYPE_F64;
		} else if(FOUND_GROUP(TYPE_DECL_CHAR)) {
			decl->type = QB_TYPE_U08;
			decl->flags |= QB_TYPE_DECL_STRING;
		} else if(FOUND_GROUP(TYPE_DECL_BOOLEAN)) {
			decl->type = QB_TYPE_I32;
		} else if(FOUND_GROUP(TYPE_DECL_STRING) && !FOUND_GROUP(TYPE_DECL_DIMENSIONS)) {
			decl->type = QB_TYPE_U08;
			decl->dimension_count = 1;
			decl->dimensions = qb_allocate_indices(pool, 1);
			decl->dimensions[0] = 0;
			decl->flags |= QB_TYPE_DECL_EXPANDABLE | QB_TYPE_DECL_STRING;
		} else if(FOUND_GROUP(TYPE_DECL_IMAGE) && !FOUND_GROUP(TYPE_DECL_DIMENSIONS)) {
			uint32_t identifier_len = GROUP_LENGTH(TYPE_DECL_IMAGE);
			decl->type = QB_TYPE_F32;
			decl->dimension_count = 3;
			decl->dimensions = qb_allocate_indices(pool, 3);
			decl->dimensions[0] = 0;
			decl->dimensions[1] = 0;
			if(identifier_len == 6) {
				const char *identifier = s + GROUP_OFFSET(TYPE_DECL_IMAGE);
				decl->dimensions[2] = (identifier[identifier_len - 1] == '3') ? 3 : 4;
			} else {
				decl->dimensions[2] = 4;
			}
		} else {
			return NULL;
		}

		if(FOUND_GROUP(TYPE_DECL_DIMENSIONS)) {
			uint32_t dimension_count = 0, i;
			uint32_t dimension_len = GROUP_LENGTH(TYPE_DECL_DIMENSIONS);
			const char *dimension_string = s + GROUP_OFFSET(TYPE_DECL_DIMENSIONS);

			// see how many brackets there are
			for(i = 0; i < dimension_len; i++) {
				if(dimension_string[i] == '[') {
					dimension_count++;
				}
			}

			decl->dimensions = qb_allocate_indices(pool, dimension_count);
			decl->dimension_count = dimension_count;

			for(i = 0; i < dimension_count; i++) {
				int32_t processed = qb_parse_type_dimension(pool, dimension_string, dimension_len, decl, i);
				if(processed == -1) {
					return NULL;
				}
				dimension_string += processed;
				dimension_len -= processed;
			}
		}

		if(var_type && var_type != QB_VARIABLE_RETURN_VALUE) {
			const char *identifier = s + end_index;
			uint32_t identifier_len = len - end_index;
			matches = pcre_exec(identifier_regexp, NULL, identifier, identifier_len, 0, 0, offsets, sizeof(offsets) / sizeof(int));
			if(FOUND_GROUP(ID_NORMAL)) {
				const char *name = identifier + GROUP_OFFSET(ID_NORMAL);
				uint32_t name_len = GROUP_LENGTH(ID_NORMAL);
				decl->name = qb_allocate_string(pool, name, name_len);
				decl->name_length = name_len;
				decl->hash_value = zend_inline_hash_func(decl->name, name_len + 1);
			} else if(FOUND_GROUP(ID_PATTERN)) {
				// string contains special characters--try to compile it as a regexp
				const char *pattern = identifier + GROUP_OFFSET(ID_PATTERN);
				uint32_t pattern_len = GROUP_LENGTH(ID_PATTERN);
				const char *pcre_error = NULL;
				int pcre_error_offset = 0, c = pattern[pattern_len];
				pcre *re;
				char *constricted_pattern;
				ALLOCA_FLAG(use_heap);

				// put ^ at the beginning and $ at the end to force complete match
				constricted_pattern = do_alloca(pattern_len + 3, use_heap);
				constricted_pattern[0] = '^';
				memcpy(constricted_pattern + 1, pattern, pattern_len);
				constricted_pattern[1 + pattern_len] = '$';
				constricted_pattern[1 + pattern_len + 1] = '\0';

				re = pcre_compile(constricted_pattern, 0, &pcre_error, &pcre_error_offset, NULL);
				free_alloca(constricted_pattern, use_heap);
				if(!re) {
					return NULL;
				}
				decl->regexp = re;
			} else {
				return NULL;
			}
		}
	}
	return decl;
}

static void ZEND_FASTCALL qb_do_static_init(qb_compiler_context *cxt, qb_address *address1, qb_address *result_address) {
	if(!cxt->op_count) {
		qb_op *qop = qb_append_op(cxt, QB_IF_INIT, 2);
		qop->operands[0].jump_target_index = 0;
		qop->operands[0].type = QB_OPERAND_JUMP_TARGET;
		qop->operands[1].jump_target_index = QB_INSTRUCTION_NEXT;
		qop->operands[1].type = QB_OPERAND_JUMP_TARGET;
	}
	qb_do_assignment(cxt, address1, result_address);
}

static qb_type_declaration * ZEND_FASTCALL qb_find_variable_declaration_in_list(qb_compiler_context *cxt, qb_variable *qvar, qb_type_declaration **declarations, uint32_t declaration_count) {
	uint32_t i;
	for(i = 0; i < declaration_count; i++) {
		qb_type_declaration *decl = declarations[i];
		if((decl->flags & QB_VARIABLE_TYPES) & (qvar->flags & QB_VARIABLE_TYPES)) {
			if(qvar->name) {
				if(decl->name) {
					if(decl->hash_value == qvar->hash_value && decl->name_length == qvar->name_length) {
						if(strncmp(decl->name, qvar->name, qvar->name_length) == 0) {
							return decl;
						}
					}
				} else if(decl->regexp) {
					int offsets[128];
					int count = pcre_exec(decl->regexp, NULL, qvar->name, qvar->name_length, 0, PCRE_NOTEMPTY, offsets, sizeof(offsets) / sizeof(int));
					if(count > 0) {
						return decl;
					}
				}
			} else {
				if(!decl->name && !decl->regexp) {
					return decl;
				}
			}
		}
	}
	return NULL;
}

static qb_type_declaration * ZEND_FASTCALL qb_find_variable_declaration(qb_compiler_context *cxt, qb_variable *qvar) {
	qb_type_declaration *decl = NULL;
	qb_function_declaration *f_decl = cxt->function_declaration;
	if(qvar->flags & (QB_VARIABLE_CLASS | QB_VARIABLE_CLASS_INSTANCE)) {
		if(f_decl->class_declaration) {
			decl = qb_find_variable_declaration_in_list(cxt, qvar, f_decl->class_declaration->declarations, f_decl->class_declaration->declaration_count);
			if(decl) {
				return decl;
			}
		}
		// type declaration for class variables can also appear in the function's Doc Comments
	}
	decl = qb_find_variable_declaration_in_list(cxt, qvar, f_decl->declarations, f_decl->declaration_count);
	return decl;
}

static void ZEND_FASTCALL qb_initialize_dimensions(qb_compiler_context *cxt, qb_address *initializer_address, qb_address *address) {
	int32_t i;
	int32_t has_undefined = 0;
	for(i = 0; (uint32_t) i < address->dimension_count; i++) {
		qb_address *dimension_address = address->dimension_addresses[i];
		if((dimension_address->flags & QB_ADDRESS_READ_ONLY) && VALUE(U32, dimension_address) == 0) {
			has_undefined = 1;
			break;
		}
	}
	if(has_undefined) {
		uint32_t array_size = 1;
		if(address->dimension_count != initializer_address->dimension_count) {
			qb_abort("The number of dimensions do not match");
		}
		for(i = address->dimension_count - 1; i >= 0; i--) {
			qb_address *dimension_address = address->dimension_addresses[i];
			qb_address *array_size_address = address->array_size_addresses[i];
			// initialize the dimension if it's not defined (read-only with a value of zero)
			if((dimension_address->flags & QB_ADDRESS_READ_ONLY) && VALUE(U32, dimension_address) == 0) {
				qb_address *init_dimension_address = initializer_address->dimension_addresses[i];
				uint32_t dimension = VALUE(U32, init_dimension_address);
				VALUE(U32, dimension_address) = dimension;
				dimension_address->flags = init_dimension_address->flags;
				array_size *= dimension;
				if(i != address->dimension_count - 1) {
					VALUE(U32, array_size_address) = array_size;
					array_size_address->flags = init_dimension_address->flags;
				}
			} else {
				array_size *= VALUE(U32, dimension_address);
			}
		}
		if(IS_EXPANDABLE_ARRAY(address)) {
			// update the segment flag
			qb_memory_segment *segment = &cxt->storage->segments[address->segment_selector];
			segment->flags |= QB_SEGMENT_EXPANDABLE;
		}
	}
}

static void ZEND_FASTCALL qb_set_variable_type(qb_compiler_context *cxt, qb_variable *qvar) {
	qb_type_declaration *decl = qb_find_variable_declaration(cxt, qvar);

	if(decl) {
		if(decl->type != QB_TYPE_VOID) {
			qb_address *address;
			if(decl->dimension_count == 0) {
				address = qb_create_scalar(cxt, decl->type);
			} else {
				uint32_t i;
				int32_t element_count = 1;
				for(i = 0; i < decl->dimension_count; i++) {
					element_count *= decl->dimensions[i];
				}
				if(element_count) {
					address = qb_create_fixed_size_multidimensional_array(cxt, decl->type, decl->dimensions, decl->dimension_count, QB_CREATE_IN_NEW_SEGMENT);
				} else {
					address = qb_create_variable_length_multidimensional_array(cxt, decl->type, decl->dimensions, decl->dimension_count, (decl->flags & QB_TYPE_DECL_EXPANDABLE) ? QB_CREATE_EXPANDABLE : 0);
				}
			}
			if(decl->flags & QB_TYPE_DECL_STRING) {
				address->flags |= QB_ADDRESS_STRING;
			}
			if(decl->flags & QB_TYPE_DECL_HAS_ALIAS_SCHEMES) {
				address->index_alias_schemes = decl->index_alias_schemes;
			}
			if(qvar->flags & (QB_VARIABLE_ARGUMENT | QB_VARIABLE_GLOBAL | QB_VARIABLE_CLASS | QB_VARIABLE_CLASS_INSTANCE)) {
				// not adjustment of array size when the source of the variable is external
				address->flags |= QB_ADDRESS_INITIALIZED;
			}
			if(qvar->flags & (QB_VARIABLE_PASSED_BY_REF | QB_VARIABLE_RETURN_VALUE | QB_VARIABLE_STATIC | QB_VARIABLE_GLOBAL | QB_VARIABLE_CLASS | QB_VARIABLE_CLASS_INSTANCE)) {
				// indicate that the value of the variable can be read outside the function 
				// (for optimization purpose during generation of C source code)
				qb_mark_as_non_local(cxt, address);
			}
			qvar->address = address;
		}
	} else {
		if(qvar->flags & QB_VARIABLE_RETURN_VALUE) {
			// assume to be void if missing
		} else {
			qb_abort("Missing type declaration: %s", qvar->name);
		}
	}
}

static void ZEND_FASTCALL qb_add_variables(qb_compiler_context *cxt) {
	zend_compiled_variable *zvars = cxt->zend_function->op_array.vars;
	zend_arg_info *zargs = cxt->zend_function->common.arg_info;
	HashTable *static_variable_table = cxt->zend_function->op_array.static_variables;
	uint32_t i, variable_count = (uint32_t) cxt->zend_function->op_array.last_var;
	qb_variable *qvar;

	cxt->argument_count = cxt->zend_function->common.num_args;
	cxt->required_argument_count = cxt->zend_function->common.required_num_args;

	for(i = 0; i < variable_count; i++) {
		zend_compiled_variable *zvar = &zvars[i];

		qvar = qb_allocate_variable(cxt->pool);
		qvar->name = zvar->name;
		qvar->name_length = zvar->name_len;
		qvar->hash_value = zvar->hash_value;
		qvar->zend_class = NULL;
		if(i < cxt->argument_count) {
			// it's a function argument
			zend_arg_info *zarg = (i < cxt->argument_count) ? &zargs[i] : NULL;
			qvar->flags = QB_VARIABLE_ARGUMENT;
			if(zarg->pass_by_reference) {
				qvar->flags |= QB_VARIABLE_PASSED_BY_REF;
			}
			qb_set_variable_type(cxt, qvar);
		} else {
			// see if it's static variable
			qb_address *static_initializer = NULL;
			zval **p_static_value, *static_value;
			if(static_variable_table && zend_hash_quick_find(static_variable_table, zvar->name, zvar->name_len + 1, zvar->hash_value, (void **) &p_static_value) == SUCCESS) {
				static_value = *p_static_value;
				qvar->flags = QB_VARIABLE_STATIC;
				qb_set_variable_type(cxt, qvar);

				if(qvar->address->type == QB_TYPE_S64 || qvar->address->type == QB_TYPE_U64) {
					// initializing 64-bit integer might require special handling
					uint32_t desired_type = qvar->address->type;
					uint32_t dimension_count = qb_get_zend_array_dimension_count(cxt, static_value, desired_type);
					if(qvar->address->dimension_count + 1 == dimension_count) {
						uint32_t dimensions[MAX_DIMENSION];
						dimensions[0] = 0;
						qb_get_zend_array_dimensions(cxt, static_value, desired_type, dimensions, dimension_count);
						if(dimensions[dimension_count - 1] == 2) {
							// treat the last level as scalars
							dimension_count--;
							if(dimension_count > 0) {
								static_initializer = qb_create_fixed_size_multidimensional_array(cxt, desired_type, dimensions, dimension_count, FALSE);
								qb_copy_elements_from_zend_array(cxt, static_value, static_initializer);
							} else {
								static_initializer = qb_create_scalar(cxt, desired_type);
								qb_copy_element_from_zval(cxt, static_value, static_initializer);
							}
							static_initializer->flags |= QB_ADDRESS_CONSTANT | QB_ADDRESS_INITIALIZED;
						}
					}
				}
				if(!static_initializer) {
					// handle it in the regular manner
					static_initializer = qb_obtain_zval_constant(cxt, *p_static_value, qvar->address->type);
				}

				if(IS_VARIABLE_LENGTH_ARRAY(qvar->address)) {
					qb_initialize_dimensions(cxt, static_initializer, qvar->address);
				}
				qb_do_static_init(cxt, static_initializer, qvar->address);
			} else {
				// we don't know whether qvar a local or a global variable at this point
				qvar->flags = 0;
				qvar->address = NULL;
			}
		}
		qb_add_variable(cxt, qvar);
	}

	// return value
	qvar = qb_allocate_variable(cxt->pool);
	qvar->name = NULL;
	qvar->name_length = 0;
	qvar->hash_value = 0;
	qvar->flags = QB_VARIABLE_RETURN_VALUE;
	qb_set_variable_type(cxt, qvar);
	cxt->return_variable = qvar;
	qb_add_variable(cxt, qvar);
}

static void ZEND_FASTCALL qb_initialize_function_prototype(qb_compiler_context *cxt) {
	cxt->function_prototype.argument_count = cxt->argument_count;
	cxt->function_prototype.required_argument_count = cxt->required_argument_count;
	cxt->function_prototype.variables = cxt->variables;
	cxt->function_prototype.return_variable = cxt->return_variable;
	cxt->function_prototype.zend_function = cxt->zend_function;
	cxt->function_prototype.name = cxt->zend_function->common.function_name;
	cxt->function_prototype.filename = cxt->zend_function->op_array.filename;
	cxt->function_prototype.local_storage = cxt->storage;
}

static uint32_t ZEND_FASTCALL qb_import_external_symbol(qb_compiler_context *cxt, uint32_t type, const char *name, uint32_t name_len, void *pointer) {
	uint32_t i;
	qb_external_symbol *symbol;
	if(cxt->external_symbols) {
		for(i = 0; i < cxt->external_symbol_count; i++) {
			symbol = &cxt->external_symbols[i];
			if(symbol->pointer == pointer) {
				return i;
			}
		}
	} else {
		qb_attach_new_array(cxt->pool, (void **) &cxt->external_symbols, &cxt->external_symbol_count, sizeof(qb_external_symbol), 4);
	}
	i = cxt->external_symbol_count;
	symbol = qb_enlarge_array((void **) &cxt->external_symbols, 1);
	symbol->type = type;
	symbol->name = name;
	symbol->name_length = name_len;
	symbol->pointer = pointer;
	return i;
}

static void ZEND_FASTCALL qb_do_assignment(qb_compiler_context *cxt, qb_address *value_address, qb_address *variable_address) {
	if(!cxt->resolving_result_type) {
		if(!(variable_address->flags & QB_ADDRESS_INITIALIZED)) {
			// initialize the dimensions of an array
			if(variable_address->dimension_count > 0) {
				qb_initialize_dimensions(cxt, value_address, variable_address);
			}
			variable_address->flags |= QB_ADDRESS_INITIALIZED;
		}

		if(value_address->flags & QB_ADDRESS_TEMPORARY) {
			qb_op *prev_qop = cxt->ops[cxt->op_count - 1];
			if(prev_qop->operand_count > 0 && prev_qop->operands[prev_qop->operand_count - 1].address == value_address) {
				// the previous op probably created the value and placed it into the temp var
				// double check just to be sure
				uint32_t operand_flags;
				switch(prev_qop->opcode) {
					case QB_FCALL_VAR:
					case QB_FCALL_MIX:
						// function calls employ variable number of operands so qb_get_operand_flags() 
						// won't tell us anything; we do know that the last operand is the return value
						operand_flags = QB_OPERAND_WRITABLE;
						break;
					default:
						operand_flags = qb_get_operand_flags(cxt, prev_qop->opcode, prev_qop->operand_count - 1);
				}
				if(operand_flags & QB_OPERAND_WRITABLE) {
					// we can probably let the previous op write into the variable instead of putting 
					// the value into the temp var first
					int32_t substitute = FALSE;
					if(STORAGE_TYPE_MATCH(value_address->type, variable_address->type)) {
						// okay, the storage types do match (i.e. they are the same or differ only by signedness)
						// if the op can is capable of performing the same wrap-around behavior that the MOV
						// instruction performs, then a substituion can always occur
						uint32_t op_flags = qb_get_op_flags(cxt, prev_qop->opcode);
						if(op_flags & QB_OP_PERFORM_WRAP_AROUND) {
							substitute = TRUE;
						} else {
							// in absence of wrap-around handling, the sizes of the two must match
							if(IS_SCALAR(value_address)) {
								if(IS_SCALAR(variable_address)) {
									// both are scalars
									substitute = TRUE;
								} 
							} else if(!IS_SCALAR(variable_address)) {
								// both are arrays--check their sizes
								if(IS_FIXED_LENGTH_ARRAY(value_address) && IS_FIXED_LENGTH_ARRAY(variable_address) && ARRAY_SIZE(value_address) == ARRAY_SIZE(variable_address)) {
									substitute = TRUE;
								}
							}
						}
					} 
					if(substitute) {
						prev_qop->operands[prev_qop->operand_count - 1].address = variable_address;
						qb_mark_as_writable(cxt, variable_address);
						return;
					}
				}
			}
		}

		// add copy op
		qb_create_unary_op(cxt, &factory_copy, value_address, variable_address);
	}
}

static uint32_t ZEND_FASTCALL qb_get_array_initializer_type(qb_compiler_context *cxt, qb_array_initializer *initializer) {
	uint32_t i, highest_rank_type = 0;
	for(i = 0; i < initializer->element_count; i++) {
		qb_operand *element = &initializer->elements[i];
		uint32_t element_type;
		if(element->type == QB_OPERAND_ARRAY_INITIALIZER) {
			element_type = qb_get_array_initializer_type(cxt, element->array_initializer);
		} else if(element->type == QB_OPERAND_ZVAL) {
			element_type = qb_get_zval_type(cxt, element->constant, 0);
		} else if(element->type == QB_OPERAND_ADDRESS) {
			element_type = element->address->type;
		}
		if(highest_rank_type == QB_TYPE_UNKNOWN || element_type > highest_rank_type) {
			highest_rank_type = element_type;
		}
	}
	if(highest_rank_type == QB_TYPE_UNKNOWN) {
		// the array is empty--doesn't really matter what it is
		highest_rank_type = QB_TYPE_I32;
	}
	return highest_rank_type;
}

static uint32_t ZEND_FASTCALL qb_get_operand_type(qb_compiler_context *cxt, qb_operand *operand, uint32_t flags) {
	if(operand->type == QB_OPERAND_ADDRESS) {
		if(flags & QB_COERCE_TO_INTEGER && operand->address->type >= QB_TYPE_F32) {
			if(flags & QB_COERCE_TO_UNSIGNED) {
				return QB_TYPE_U64;
			} else {
				return QB_TYPE_I64;
			}
		} else if(flags & QB_COERCE_TO_FLOATING_POINT && operand->address->type < QB_TYPE_F32) {
			if(flags & QB_COERCE_TO_INTEGER_TO_DOUBLE) {
				return QB_TYPE_F64;
			} else {
				return (operand->address->type <= QB_TYPE_U16) ? QB_TYPE_F32 : QB_TYPE_F64;
			}
		} else if(flags & QB_COERCE_TO_SIGNED) {
			return operand->address->type & ~QB_TYPE_UNSIGNED;
		} else if(flags & QB_COERCE_TO_UNSIGNED) {
			return operand->address->type | QB_TYPE_UNSIGNED;
		}
		return operand->address->type;
	} else if(operand->type == QB_OPERAND_ZVAL) {
		return qb_get_zval_type(cxt, operand->constant, flags);
	} else if(operand->type == QB_OPERAND_ARRAY_INITIALIZER) {
		return qb_get_array_initializer_type(cxt, operand->array_initializer);
	} else if(operand->type == QB_OPERAND_PREVIOUS_RESULT) {
		if(flags & QB_COERCE_TO_INTEGER_TO_DOUBLE) {
			return QB_TYPE_F64;
		} else if(flags & (QB_COERCE_TO_INTEGER | QB_COERCE_TO_BOOLEAN)) {
			if(flags & QB_COERCE_TO_UNSIGNED) {
				return QB_TYPE_I32;
			} else {
				return QB_TYPE_U32;
			}
		} 
	}
	return QB_TYPE_UNKNOWN;
}

static uint32_t ZEND_FASTCALL qb_get_highest_rank_type(qb_compiler_context *cxt, qb_operand *operands, uint32_t count, uint32_t flags) {
	uint32_t type1 = QB_TYPE_UNKNOWN, type2, i;
	for(i = 0; i < count; i++) {
		qb_operand *operand = &operands[i];
		type2 = qb_get_operand_type(cxt, operand, flags);
		if(type1 == QB_TYPE_UNKNOWN || type1 < type2) {
			type1 = type2;
		}
	}
	if(type1 == QB_TYPE_UNKNOWN) {
		// default to I32
		type1 = QB_TYPE_I32;
	}
	return type1;
}

static void ZEND_FASTCALL qb_do_type_coercion(qb_compiler_context *cxt, qb_operand *operand, uint32_t desired_type) {
	if(operand->type == QB_OPERAND_ADDRESS) {
		if(desired_type != QB_TYPE_VOID) {
			qb_address *new_address = NULL;
			if(operand->address->type != desired_type && desired_type != QB_TYPE_ANY) {
				if(STORAGE_TYPE_MATCH(operand->address->type, desired_type)) {
					if(operand->address->flags & QB_ADDRESS_CAST) {
						// use the original address
						operand->address = operand->address->source_address;
					} else {
						// storage type is the same--just create a new address
						new_address = qb_allocate_address(cxt->pool);
						*new_address = *operand->address;
						new_address->type = desired_type;
						new_address->source_address = operand->address;
						new_address->flags |= QB_ADDRESS_CAST;
						operand->address = new_address;
					}
				} else {
					// the bit pattern is different--need to do a copy
					if(operand->address->flags & QB_ADDRESS_CONSTANT) {
						uint32_t element_count = IS_SCALAR(operand->address) ? 1 : ARRAY_SIZE(operand->address);
						new_address = qb_allocate_constant(cxt, desired_type, operand->address->array_size_address);
						qb_copy_elements(operand->address->type, ARRAY(I08, operand->address), element_count, new_address->type, ARRAY(I08, new_address), element_count);
						operand->address = new_address;
					} else {
						new_address = qb_obtain_temporary_variable(cxt, desired_type, operand->address->array_size_address);
						qb_do_assignment(cxt, operand->address, new_address);
						operand->address = new_address;
					}
				}
			}
		} else {
			operand->address = NULL;
			operand->type = QB_OPERAND_NONE;
		}
	} else if(operand->type == QB_OPERAND_ZVAL) {
		if(desired_type != QB_TYPE_VOID) {
			if(desired_type == QB_TYPE_ANY) {
				desired_type = qb_get_zval_type(cxt, operand->constant, 0);
			}
			operand->address = qb_obtain_zval_constant(cxt, operand->constant, desired_type);
			operand->type = QB_OPERAND_ADDRESS;
		} else {
			operand->address = NULL;
			operand->type = QB_OPERAND_NONE;
		}
	} else if(operand->type == QB_OPERAND_ARRAY_INITIALIZER) {
		if(desired_type == QB_TYPE_ANY) {
			desired_type = qb_get_array_initializer_type(cxt, operand->array_initializer);
		}
		operand->address = qb_obtain_array_from_initializer(cxt, operand->array_initializer, desired_type);
		operand->type = QB_OPERAND_ADDRESS;
	} else if(operand->type == QB_OPERAND_PREVIOUS_RESULT) {
		if(desired_type == QB_TYPE_ANY) {
			desired_type = cxt->default_result_type;
		}
		*operand->result_type = desired_type;
		if(desired_type != QB_TYPE_VOID) {
			if(desired_type == QB_TYPE_ANY) {
				// the type doesn't really matter--the address is just a placeholder
				desired_type = QB_TYPE_U32;
			}
			operand->address = qb_obtain_temporary_scalar(cxt, desired_type);
			operand->type = QB_OPERAND_ADDRESS;
		} else {
			operand->address = NULL;
			operand->type = QB_OPERAND_NONE;
		}
	}
}

static qb_address * ZEND_FASTCALL qb_get_array_slice(qb_compiler_context *cxt, qb_address *container_address, qb_address *offset_address, qb_address *length_address) {
	qb_address *slice_address;

	if(IS_SCALAR(container_address)) {
		qb_abort("Illegal operation: not an array");
	}
	if(!IS_SCALAR(offset_address)) {
		qb_abort("Cannot use an array as an index");
	}
	if(length_address && !IS_SCALAR(length_address)) {
		qb_abort("Cannot use an array as length");
	}
	if(!IS_SCALAR_VARIABLE(offset_address)) {
		// need to copy the offset value to a temporary variable first
		qb_address *new_address = qb_obtain_temporary_variable(cxt, QB_TYPE_U32, NULL);
		qb_create_unary_op(cxt, &factory_copy, offset_address, new_address);
		offset_address = new_address;
	}
	if(length_address) {
		if(!IS_SCALAR_VARIABLE(length_address)) {
			qb_address *new_address = qb_obtain_temporary_variable(cxt, QB_TYPE_U32, NULL);
			qb_create_unary_op(cxt, &factory_copy, length_address, new_address);
			length_address = new_address;
		}
	} else {
		qb_address *size_address = container_address->dimension_addresses[0];
		if((offset_address->flags & QB_ADDRESS_CONSTANT) && (size_address->flags & QB_ADDRESS_CONSTANT)) {
			uint32_t length_value = VALUE(U32, size_address) - VALUE(U32, offset_address);
			length_address = qb_obtain_constant_U32(cxt, length_value);
		} else {
			length_address = qb_obtain_temporary_variable(cxt, QB_TYPE_U32, NULL);
			qb_create_binary_op(cxt, &factory_subtract, size_address, offset_address, length_address);
		}
	}
	slice_address = qb_allocate_address(cxt->pool);
	slice_address->type = container_address->type;
	slice_address->flags = QB_ADDRESS_TEMPORARY | QB_ADDRESS_NON_LOCAL | (container_address->flags & QB_ADDRESS_STRING);
	slice_address->mode = QB_ADDRESS_MODE_ARR;
	slice_address->segment_selector = container_address->segment_selector;
	if(offset_address->flags & QB_ADDRESS_CONSTANT) {
		slice_address->segment_offset = VALUE(U32, offset_address) << type_size_shifts[slice_address->type];
	} else {
		slice_address->array_index_address = offset_address;
		slice_address->segment_offset = QB_OFFSET_INVALID;
	}
	slice_address->dimension_count = container_address->dimension_count;
	if(slice_address->dimension_count > 1) {
		qb_address *array_size_address, *sub_array_size_address = container_address->array_size_addresses[1];
		uint32_t i;
		if((length_address->flags & QB_ADDRESS_CONSTANT) && (sub_array_size_address->flags & QB_ADDRESS_CONSTANT)) {
			uint32_t array_size_value = VALUE(U32, length_address) * VALUE(U32, sub_array_size_address);
			array_size_address = qb_obtain_constant_U32(cxt, array_size_value);
		} else {
			array_size_address = qb_obtain_qm_temporary_variable(cxt, QB_TYPE_U32, NULL);
			qb_create_binary_op(cxt, &factory_multiply, length_address, sub_array_size_address, array_size_address);
		}
		slice_address->dimension_addresses = qb_allocate_address_pointers(cxt->pool, slice_address->dimension_count);
		slice_address->array_size_addresses = qb_allocate_address_pointers(cxt->pool, slice_address->dimension_count);
		for(i = 1; i < slice_address->dimension_count; i++) {
			slice_address->dimension_addresses[i] = container_address->dimension_addresses[i];
			slice_address->array_size_addresses[i] = container_address->array_size_addresses[i];
		}
		slice_address->dimension_addresses[0] = length_address;
		slice_address->array_size_addresses[0] = slice_address->array_size_address = array_size_address;
	} else {
		slice_address->dimension_addresses = slice_address->array_size_addresses = &slice_address->array_size_address;
		slice_address->array_size_address = length_address;
	}
	slice_address->source_address = container_address;
	return slice_address;
}

static qb_address * ZEND_FASTCALL qb_get_array_element(qb_compiler_context *cxt, qb_address *container_address, qb_address *index_address) {
	qb_address *result_address = qb_allocate_address(cxt->pool);
	qb_address *offset_address = NULL;
	int32_t element_type = container_address->type;

	if(IS_SCALAR(container_address)) {
		qb_abort("Illegal operation: not an array");
	}
	if(!IS_SCALAR(index_address)) {
		qb_abort("Cannot use an array as an index");
	}

	if(!IS_SCALAR_VARIABLE(index_address)) {
		// need to copy the index value to a temporary variable first
		qb_address *new_address = qb_obtain_temporary_variable(cxt, QB_TYPE_U32, NULL);
		qb_do_assignment(cxt, index_address, new_address);
		index_address = new_address;
	}
	if(container_address->dimension_count == 1) {
		qb_initialize_element_address(cxt, container_address, result_address);
	} else {
		qb_initialize_subarray_address(cxt, container_address, result_address);

		// multiple by the size of the sub-array if index is not zero
		if(!(index_address->flags & QB_ADDRESS_CONSTANT) || VALUE(U32, index_address) > 0) {
			qb_address *size_address = result_address->array_size_address;
			qb_address *product_address;
			if((index_address->flags & QB_ADDRESS_CONSTANT) && (size_address->flags & QB_ADDRESS_CONSTANT)) {
				// just do the calculation
				uint32_t product = VALUE(U32, index_address) * VALUE(U32, size_address);
				product_address = qb_obtain_constant_U32(cxt, product);
			} else {
				if(index_address == container_address->dimension_addresses[0]) {
					// dimension times the sub-array size is the array-size 
					product_address = container_address->array_size_address;
				} else {
					// add multiplication op
					product_address = qb_obtain_temporary_variable(cxt, QB_TYPE_I32, NULL);
					qb_create_binary_op(cxt, &factory_multiply, index_address, size_address, product_address);
				}
			}
			index_address = product_address;
		}
	}

	if(container_address->array_index_address) {
		offset_address = container_address->array_index_address;
	} else if(container_address->segment_offset > 0) {
		offset_address = qb_obtain_constant_U32(cxt, container_address->segment_offset >> type_size_shifts[element_type]);
	}
	if(offset_address) {
		qb_address *sum_address;
		// add the index if it isn't zero
		if(!(index_address->flags & QB_ADDRESS_CONSTANT) || VALUE(U32, index_address) > 0) { 
			if(index_address->flags & QB_ADDRESS_CONSTANT && offset_address->flags & QB_ADDRESS_CONSTANT) {
				uint32_t sum = VALUE(U32, index_address) + VALUE(U32, offset_address);
				sum_address = qb_obtain_constant_U32(cxt, sum);
			} else {
				sum_address = qb_obtain_temporary_variable(cxt, QB_TYPE_I32, NULL);
				qb_create_binary_op(cxt, &factory_add, index_address, offset_address, sum_address);
			}
			index_address = sum_address;
		} else {
			index_address = offset_address;
		}
	}
	if(index_address->flags & QB_ADDRESS_CONSTANT) {
		uint32_t index = VALUE(U32, index_address);
		if(IS_FIXED_LENGTH_ARRAY(container_address)) {
			if(index < ARRAY_SIZE(container_address)) {
				result_address->flags |= QB_ADDRESS_ALWAYS_IN_BOUND;
			}
		}
		result_address->segment_offset = index << type_size_shifts[element_type];
		if(result_address->dimension_count == 0) {
			result_address->mode = QB_ADDRESS_MODE_ELC;
		}
	} else {
		result_address->segment_offset = QB_OFFSET_INVALID;
		result_address->array_index_address = index_address;
		if(result_address->dimension_count == 0) {
			result_address->mode = QB_ADDRESS_MODE_ELV;
		}
	}
	result_address->source_address = container_address;
	return result_address;
}

static qb_address *ZEND_FASTCALL qb_get_subarray_sizes(qb_compiler_context *cxt, qb_address *address) {
	qb_address *subarray_sizes_address = qb_obtain_temporary_fixed_length_array(cxt, QB_TYPE_U32, address->dimension_count - 1);
	uint32_t i;
	for(i = 1; i < address->dimension_count; i++) {
		qb_address *index_address = qb_obtain_constant_U32(cxt, i - 1);
		qb_address *src_array_size_address = address->array_size_addresses[i];
		qb_address *dst_array_size_address = qb_get_array_element(cxt, subarray_sizes_address, index_address);
		qb_create_unary_op(cxt, &factory_copy, src_array_size_address, dst_array_size_address);
	}
	return subarray_sizes_address;
}

static int32_t ZEND_FASTCALL qb_find_index_alias(qb_compiler_context *cxt, qb_index_alias_scheme *scheme, const char *name, uint32_t name_length) {
	uint32_t i;
	for(i = 0; i < scheme->dimension; i++) {
		if(strcmp(scheme->aliases[i], name) == 0) {
			return i;
		}
	}
	return -1;
}

static qb_address * ZEND_FASTCALL qb_get_named_element(qb_compiler_context *cxt, qb_address *container_address, zval *name) {
	qb_address *index_address, *value_address;
	const char *alias = Z_STRVAL_P(name);
	uint32_t alias_len = Z_STRLEN_P(name);
	int32_t index;

	if(IS_SCALAR(container_address)) {
		qb_abort("Illegal operation: not an array");
	}
	if(!container_address->index_alias_schemes || !container_address->index_alias_schemes[0]) {
		qb_abort("Array elements are not named");
	}
	
	index = qb_find_index_alias(cxt, container_address->index_alias_schemes[0], alias, alias_len);
	if(index == -1) {
		qb_abort("No element by the name of '%s'", alias);
	}
	index_address = qb_obtain_constant_U32(cxt, index);
	value_address = qb_get_array_element(cxt, container_address, index_address);
	return value_address;
}

static qb_address * ZEND_FASTCALL qb_get_largest_array_size(qb_compiler_context *cxt, qb_operand *operands, uint32_t operand_count) {
	uint32_t i, array_size = 0;
	qb_address *array_size_address = NULL;
	for(i = 0; i < operand_count; i++) {
		qb_address *address = operands[i].address;
		if(IS_VARIABLE_LENGTH_ARRAY(address)) {
			return address->array_size_address;
		} else if(!IS_SCALAR(address)) {
			if(!array_size_address || array_size < ARRAY_SIZE(address)) {
				array_size = ARRAY_SIZE(address);
				array_size_address = address->array_size_address;
			}
		}
	}
	return array_size_address;
}

static qb_address * ZEND_FASTCALL qb_get_largest_vector_count(qb_compiler_context *cxt, qb_operand *operands, uint32_t operand_count) {
	qb_address *array_size_address = qb_get_largest_array_size(cxt, operands, operand_count);
	qb_address *vector_address;
	uint32_t element_count, vector_width, vector_count;
	if(!(array_size_address->flags & QB_ADDRESS_CONSTANT)) {
		// return the variable to indicate variable-length
		return array_size_address;
	}
	vector_address = operands[0].address;
	vector_width = VALUE(U32, vector_address->dimension_addresses[vector_address->dimension_count - 1]);
	element_count = VALUE(U32, array_size_address);
	if(element_count > vector_width) {
		vector_count = element_count / vector_width;
		return qb_obtain_constant_U32(cxt, vector_count);
	}
	return NULL;
}

static qb_address * ZEND_FASTCALL qb_get_matrix_matrix_product_size(qb_compiler_context *cxt, qb_operand *operand1, qb_operand *operand2) {
	qb_address *m1_address = operand1->address;
	qb_address *m2_address = operand2->address;
	if(IS_EXPANDABLE_ARRAY(m1_address)) {
		// return the none-constant address to indicate the result will be variable-length
		// the value at the address wouldn't actually be correct
		return m1_address->array_size_address;
	} else if(IS_EXPANDABLE_ARRAY(m2_address)) {
		return m2_address->array_size_address;
	} else {
		uint32_t m1_rows = qb_get_matrix_row_count(cxt, m1_address);
		uint32_t m1_cols = qb_get_matrix_column_count(cxt, m1_address);
		uint32_t m2_rows = qb_get_matrix_row_count(cxt, m1_address);
		uint32_t m2_cols = qb_get_matrix_column_count(cxt, m2_address);
		uint32_t m1_count = 1, m2_count = 1, res_count, res_size;
		if(m1_address->dimension_count > 2) {
			m1_count = ARRAY_SIZE(m1_address) / (m1_rows * m1_cols);
		}
		if(m2_address->dimension_count > 2) {
			m2_count = ARRAY_SIZE(m2_address) / (m2_rows * m2_cols);
		}
		res_count = max(m1_count, m2_count);
		res_size = (m1_rows * m2_cols) * res_count;
		if(res_size > 1) {
			return qb_obtain_constant_U32(cxt, res_size);
		}
	}
	return NULL;
}

static qb_address * ZEND_FASTCALL qb_get_vector_matrix_product_size(qb_compiler_context *cxt, qb_operand *operand1, qb_operand *operand2) {
	qb_address *v_address = operand1->address;
	qb_address *m_address = operand2->address;
	if(IS_EXPANDABLE_ARRAY(m_address)) {
		return m_address->array_size_address;
	} else if(IS_EXPANDABLE_ARRAY(v_address)) {
		return v_address->array_size_address;
	} else {
		// both are fixed-length
		uint32_t m_rows = qb_get_matrix_row_count(cxt, m_address);
		uint32_t m_cols = qb_get_matrix_column_count(cxt, m_address);
		uint32_t v_width = qb_get_vector_width(cxt, v_address);
		uint32_t m_count = 1, v_count = 1, res_count, res_size;
		if(m_address->dimension_count > 2) {
			m_count = ARRAY_SIZE(m_address) / (m_rows * m_cols);
		}
		if(v_address->dimension_count > 1) {
			v_count = ARRAY_SIZE(v_address) / v_width;
		}
		res_count = max(m_count, v_count);
		res_size = (m_rows * 1) * res_count;
		if(res_size > 1) {
			return qb_obtain_constant_U32(cxt, res_size);
		}
	}
	return NULL;
}

static qb_address * ZEND_FASTCALL qb_get_matrix_vector_product_size(qb_compiler_context *cxt, qb_operand *operand1, qb_operand *operand2) {
	qb_address *m_address = operand1->address;
	qb_address *v_address = operand2->address;
	if(IS_EXPANDABLE_ARRAY(v_address)) {
		return v_address->array_size_address;
	} else if(IS_EXPANDABLE_ARRAY(m_address)) {
		return m_address->array_size_address;
	} else {
		uint32_t v_width = qb_get_vector_width(cxt, v_address);
		uint32_t m_rows = qb_get_matrix_row_count(cxt, m_address);
		uint32_t m_cols = qb_get_matrix_column_count(cxt, m_address);
		uint32_t m_count = 1, v_count = 1, res_count, res_size;
		if(v_address->dimension_count > 1) {
			v_count = ARRAY_SIZE(v_address) / v_width;
		}
		if(m_address->dimension_count > 2) {
			m_count = ARRAY_SIZE(m_address) / (m_rows * m_cols);
		}
		res_count = max(v_count, m_count);
		res_size = (1 * m_cols) * res_count;
		if(res_size > 1) {
			return qb_obtain_constant_U32(cxt, res_size);
		}
	}
	return NULL;
}

static void ZEND_FASTCALL qb_validate_op(qb_compiler_context *cxt, qb_op *qop, uint32_t qop_index) {
	if(qop->opcode > QB_OPCODE_COUNT) {
		qb_abort("Invalid opcode: %d", qop->opcode);
	} else {
		uint32_t flags = qb_get_op_flags(cxt, qop->opcode);
		if(flags & QB_OP_VARIABLE_LENGTH) {
			if(qop->operands[0].type != QB_OPERAND_TOTAL_LENGTH) {
				qb_abort("The first operand must be the length of the instruction");
			}
		} else {
			qb_operand *operand;
			uint32_t i, operand_flags, operand_type;
			if(qb_get_operand_count(cxt, qop->opcode) != qop->operand_count) {
				qb_abort("Wrong number of operands: %s", qb_get_op_name(cxt, qop->opcode));
			}
			for(i = 0; i < qop->operand_count; i++) {
				operand = &qop->operands[i];
				operand_flags = qb_get_operand_flags(cxt, qop->opcode, i);
				operand_type = operand_flags & 0x07;
				if(operand->type != operand_type) {
					qb_abort("Operand %d is incorrect: %s", i + 1, qb_get_op_name(cxt, qop->opcode));
				}
				if(operand_type == QB_OPERAND_JUMP_TARGET) {
					uint32_t target_qop_index = qb_get_jump_target_absolute_index(cxt, qop_index, operand->jump_target_index);
					qb_op *targeted_op;
					if(target_qop_index >= cxt->op_count) {
						qb_abort("Jump target index is not valid: %s", qb_get_op_name(cxt, qop->opcode));
					}
					targeted_op = cxt->ops[target_qop_index];
					if(target_qop_index != qop_index + 1) {
						if(!(targeted_op->flags & QB_OP_JUMP_TARGET)) {
							qb_abort("Missing flag on jump target: %s", qb_get_op_name(cxt, qop->opcode));
						}
					}
				} else if(operand_type >= QB_OPERAND_ADDRESS_VAR && operand_type <= QB_OPERAND_ADDRESS_ARR) {
					qb_validate_address(cxt, operand->address);
				}
				if(operand_flags & QB_OPERAND_WRITABLE) {
					if(operand->address->flags & QB_ADDRESS_READ_ONLY) {
						// the value of a constant may be set using the interpreter
						// during constant expression evaluation
						if(!(operand->address->flags & QB_ADDRESS_CONSTANT)) {
							qb_abort("Write-target is marked read-only: %s", qb_get_op_name(cxt, qop->opcode));
						}
					}
				}
			}
		}
	}
}

static void ZEND_FASTCALL qb_resolve_address_modes(qb_compiler_context *cxt) {
	uint32_t i, j;
	for(i = 0; i < cxt->op_count; i++) {
		qb_op *qop = cxt->ops[i];

		if(qop->opcode != QB_NOP) {
			uint32_t flags = qb_get_op_flags(cxt, qop->opcode);
			uint32_t operand_address_mode, operand_type, operand_flags;
			if(flags & QB_OP_MULTI_ADDRESS) {
				if(qop->opcode == QB_FCALL) {
					// FCALL can work in one of two manners: either all addresses point to scalar variables
					// or the addresses point to both scalars or arrays
					for(j = 3; j < qop->operand_count; j++) {
						qb_operand *operand = &qop->operands[j];
						if(operand->address->mode != QB_ADDRESS_MODE_VAR) {
							qop->opcode = QB_FCALL_MIX;
							break;
						}
					}
				} else {
					uint32_t required_address_mode = QB_ADDRESS_MODE_VAR;
					for(j = 0; j < qop->operand_count; j++) {
						qb_operand *operand = &qop->operands[j];
						if(operand->type == QB_OPERAND_ADDRESS) {
							operand_flags = qb_get_operand_flags(cxt, qop->opcode, j);
							operand_type = operand_flags & 0x07;
							operand_address_mode = operand_type - 1;
							if(operand->address->mode > operand_address_mode) {
								if(operand->address->mode > required_address_mode) {
									// need use to a higher address mode
									required_address_mode = operand->address->mode;
								}
							}
						}
					}
					// promote the opcode if necessary and set the type of each operand to what it ought to be;
					// if the promotion ends up pushing the opcode into the range of a different op altogether,
					// (for instance, because an array is given whereas the handler expects only scalars), the mistake
					// will likely trip the address-mode check down below
					//
					// note that QB_ADDRESS_MODE_VAR = 0
					if(required_address_mode) {
						qop->opcode += required_address_mode;
					}
				}

				// get new set of flags
				flags = qb_get_op_flags(cxt, qop->opcode);
				qop->flags &= ~0xFF000000;
				qop->flags |= flags;
			}

			if(qop->opcode == QB_FCALL_VAR) {
				for(j = 3; j < qop->operand_count; j++) {
					qop->operands[j].type = QB_OPERAND_ADDRESS_EXT_VAR;
				}
			}
			if(qop->opcode == QB_FCALL_MIX) {
				for(j = 3; j < qop->operand_count; j++) {
					// scalars employ the extended ELV format while arrays use ARR
					if(IS_SCALAR(qop->operands[j].address)) {
						qop->operands[j].type = QB_OPERAND_ADDRESS_EXT_ELV;
					} else {
						qop->operands[j].type = QB_OPERAND_ADDRESS_EXT_ARR;
					}
				}
			} else {
				for(j = 0; j < qop->operand_count; j++) {
					qb_operand *operand = &qop->operands[j];
					if(operand->type == QB_OPERAND_ADDRESS) {
						operand_flags = qb_get_operand_flags(cxt, qop->opcode, j);
						operand_type = operand_flags & 0x07;
						operand_address_mode = operand_type - 1;
						// we can only go from VAR > ELC > ELV > ARR (an array cannot be passed as a scalar, for instance)
						// doing a signed comparison here so that if operand_address_mode is -1 
						// (i.e. there isn't supposed be an operand), we'd fall into the else
						if((int32_t) operand->address->mode <= (int32_t) operand_address_mode) {
							operand->type = operand_address_mode + 1;
						} else {
							// shouldn't really happen at runtime but we'll leave the error check in here just in case
							const char *op_name = qb_get_op_name(cxt, qop->opcode);
							cxt->line_number = qop->line_number;
							qb_abort("Invalid operand for %s", op_name);
						}
					}
				}
			}

#if ZEND_DEBUG
			qb_validate_op(cxt, qop, i);
#endif
		}
	}
}

static int32_t ZEND_FASTCALL qb_find_array_address(qb_op *qop) {
	uint32_t i;
	for(i = 0; i < qop->operand_count; i++) {
		qb_operand *operand = &qop->operands[i];
		if(operand->type == QB_OPERAND_ADDRESS) {
			if(operand->address->mode == QB_ADDRESS_MODE_ARR) {
				return 1;
			}
		}
	}
	return 0;
}

static void ZEND_FASTCALL qb_fuse_instructions(qb_compiler_context *cxt, int32_t pass) {
	uint32_t i, j;
	if(pass == 1) {
		// opcodes are not address mode specific at this point
		for(i = 0; i < cxt->op_count; i++) {
			qb_op *qop = cxt->ops[i];

			if(qop->opcode == QB_IF_T_I32 || qop->opcode == QB_IF_F_I32) {
				qb_address *condition_address = qop->operands[2].address;

				// don't fuse the instructions if the condition is going to be reused (in a short-circuited expression)
				if((condition_address->flags & QB_ADDRESS_TEMPORARY) && !(condition_address->flags & QB_ADDRESS_REUSED)) {
					qb_op *prev_qop = NULL;
					for(j = i; j > 0; j--) {
						prev_qop = cxt->ops[j - 1];
						if(prev_qop->opcode != QB_NOP) {
							break;
						}
					}

					if(prev_qop && prev_qop->operand_count == 3 && prev_qop->operands[2].address == condition_address) {
						uint32_t new_opcode = 0;
						int32_t on_true = (qop->opcode == QB_IF_T_I32);

						// combine the comparison op with the branch op
						// (only if no arrays are involved since the combined instructions aren't implemented for them)
						if(!qb_find_array_address(prev_qop)) {
							switch(prev_qop->opcode) {
								case QB_EQ_I08_I08_I32:	new_opcode = (on_true) ? QB_IF_EQ_I08_I08 : QB_IF_NE_I08_I08; break;
								case QB_EQ_I16_I16_I32:	new_opcode = (on_true) ? QB_IF_EQ_I16_I16 : QB_IF_NE_I16_I16; break;
								case QB_EQ_I32_I32_I32:	new_opcode = (on_true) ? QB_IF_EQ_I32_I32 : QB_IF_NE_I32_I32; break;
								case QB_EQ_I64_I64_I32:	new_opcode = (on_true) ? QB_IF_EQ_I64_I64 : QB_IF_NE_I64_I64; break;
								case QB_EQ_F32_F32_I32: new_opcode = (on_true) ? QB_IF_EQ_F32_F32 : QB_IF_NE_F32_F32; break;
								case QB_EQ_F64_F64_I32: new_opcode = (on_true) ? QB_IF_EQ_F64_F64 : QB_IF_NE_F64_F64; break;

								case QB_NE_I08_I08_I32:	new_opcode = (on_true) ? QB_IF_NE_I08_I08 : QB_IF_EQ_I08_I08; break;
								case QB_NE_I16_I16_I32:	new_opcode = (on_true) ? QB_IF_NE_I16_I16 : QB_IF_EQ_I16_I16; break;
								case QB_NE_I32_I32_I32:	new_opcode = (on_true) ? QB_IF_NE_I32_I32 : QB_IF_EQ_I32_I32; break;
								case QB_NE_I64_I64_I32:	new_opcode = (on_true) ? QB_IF_NE_I64_I64 : QB_IF_EQ_I64_I64; break;
								case QB_NE_F32_F32_I32: new_opcode = (on_true) ? QB_IF_NE_F32_F32 : QB_IF_EQ_F32_F32; break;
								case QB_NE_F64_F64_I32: new_opcode = (on_true) ? QB_IF_NE_F64_F64 : QB_IF_EQ_F64_F64; break;

								case QB_LT_S08_S08_I32:	new_opcode = (on_true) ? QB_IF_LT_S08_S08 : QB_IF_GE_S08_S08; break;
								case QB_LT_S16_S16_I32:	new_opcode = (on_true) ? QB_IF_LT_S16_S16 : QB_IF_GE_S16_S16; break;
								case QB_LT_S32_S32_I32:	new_opcode = (on_true) ? QB_IF_LT_S32_S32 : QB_IF_GE_S32_S32; break;
								case QB_LT_S64_S64_I32:	new_opcode = (on_true) ? QB_IF_LT_S64_S64 : QB_IF_GE_S64_S64; break;
								case QB_LT_U08_U08_I32:	new_opcode = (on_true) ? QB_IF_LT_U08_U08 : QB_IF_GE_U08_U08; break;
								case QB_LT_U16_U16_I32:	new_opcode = (on_true) ? QB_IF_LT_U16_U16 : QB_IF_GE_U16_U16; break;
								case QB_LT_U32_U32_I32:	new_opcode = (on_true) ? QB_IF_LT_U32_U32 : QB_IF_GE_U32_U32; break;
								case QB_LT_U64_U64_I32:	new_opcode = (on_true) ? QB_IF_LT_U64_U64 : QB_IF_GE_U64_U64; break;
								case QB_LT_F32_F32_I32: new_opcode = (on_true) ? QB_IF_LT_F32_F32 : QB_IF_GE_F32_F32; break;
								case QB_LT_F64_F64_I32: new_opcode = (on_true) ? QB_IF_LT_F64_F64 : QB_IF_GE_F64_F64; break;

								case QB_LE_S08_S08_I32:	new_opcode = (on_true) ? QB_IF_LE_S08_S08 : QB_IF_GT_S08_S08; break;
								case QB_LE_S16_S16_I32:	new_opcode = (on_true) ? QB_IF_LE_S16_S16 : QB_IF_GT_S16_S16; break;
								case QB_LE_S32_S32_I32:	new_opcode = (on_true) ? QB_IF_LE_S32_S32 : QB_IF_GT_S32_S32; break;
								case QB_LE_S64_S64_I32:	new_opcode = (on_true) ? QB_IF_LE_S64_S64 : QB_IF_GT_S64_S64; break;
								case QB_LE_U08_U08_I32:	new_opcode = (on_true) ? QB_IF_LE_U08_U08 : QB_IF_GT_U08_U08; break;
								case QB_LE_U16_U16_I32:	new_opcode = (on_true) ? QB_IF_LE_U16_U16 : QB_IF_GT_U16_U16; break;
								case QB_LE_U32_U32_I32:	new_opcode = (on_true) ? QB_IF_LE_U32_U32 : QB_IF_GT_U32_U32; break;
								case QB_LE_U64_U64_I32:	new_opcode = (on_true) ? QB_IF_LE_U64_U64 : QB_IF_GT_U64_U64; break;
								case QB_LE_F32_F32_I32: new_opcode = (on_true) ? QB_IF_LE_F32_F32 : QB_IF_GT_F32_F32; break;
								case QB_LE_F64_F64_I32: new_opcode = (on_true) ? QB_IF_LE_F64_F64 : QB_IF_GT_F64_F64; break;
							}
							if(new_opcode) {
								qb_operand *new_operands = qb_allocate_operands(cxt->pool, 4);
								new_operands[0].jump_target_index = qop->operands[0].jump_target_index;
								new_operands[0].type = QB_OPERAND_JUMP_TARGET;
								new_operands[1].jump_target_index = qop->operands[1].jump_target_index;
								new_operands[1].type = QB_OPERAND_JUMP_TARGET;
								new_operands[2].address = prev_qop->operands[0].address;
								new_operands[2].type = QB_OPERAND_ADDRESS;
								new_operands[3].address = prev_qop->operands[1].address;
								new_operands[3].type = QB_OPERAND_ADDRESS;
								prev_qop->operands = new_operands;
								prev_qop->operand_count = 4;
								prev_qop->opcode = new_opcode;
								prev_qop->flags |= QB_OP_JUMP;

								qop->opcode = QB_NOP;
							}
						}
					}
				}
			}
		}
		for(i = 0; i < cxt->op_count; i++) {
			qb_op *qop = cxt->ops[i];

			if(qop->opcode == QB_JMP) {
				uint32_t target_qop_index = qb_get_jump_target_absolute_index(cxt, i, qop->operands[0].jump_target_index);

				// skip over nop's
				while(cxt->ops[target_qop_index]->opcode == QB_NOP) {
					target_qop_index++;
				}
				// if the jump lands on another jump, change the target to that jump's target
				while(cxt->ops[target_qop_index]->opcode == QB_JMP) {
					uint32_t target_index = cxt->ops[target_qop_index]->operands[0].jump_target_index;
					qop->operands[0].jump_target_index = target_index;
					target_qop_index = qb_get_jump_target_absolute_index(cxt, target_qop_index, target_index);
				}

				// if the only thing sitting between the jump and the target
				// are nop's then eliminate the jump altogether
				if(target_qop_index > i) {
					int32_t needed = 0;
					uint32_t j;

					for(j = i + 1; j < target_qop_index; j++) {
						if(cxt->ops[j]->opcode != QB_NOP) {
							needed = TRUE;
							break;
						}
					}
					if(!needed) {
						qop->opcode = QB_NOP;
					}
				}
			}
		}
	} else if(pass == 2) {
		// opcodes are address mode specific here
	}
}

static uint32_t ZEND_FASTCALL qb_get_op_encoded_length(qb_compiler_context *cxt, qb_op *qop) {
	uint32_t length;
	uint32_t flags = qb_get_op_flags(cxt, qop->opcode);
	if(flags & QB_OP_JUMP) {
		// jumping to somewhere, possibly out of the function
		length = 0;
	} else {
		// pointer to the handler of the next instruction
		length = sizeof(void *);
	}
	if(flags & QB_OP_VARIABLE_LENGTH) {
		length += qop->operands[0].operand_size;
	} else {
		// mask out the four high bits used for the op itself
		uint32_t i, operand_flags, operand_type;
		for(i = 0; (operand_flags = qb_get_operand_flags(cxt, qop->opcode, i)); i++) {
			// 3 bits is used to store the type
			operand_type = operand_flags & 0x07;
			if(operand_type == QB_OPERAND_JUMP_TARGET) {
				// a jump target consists of a pointer to the next handler and a pointer to the next instruction
				length += sizeof(void *) + sizeof(int8_t *);
			} else {
				// it's an memory location otherwise
				length += sizeof(uint32_t);
			}
		}
		if(flags & QB_OP_NEED_MATRIX_DIMENSIONS) {
			length += sizeof(uint32_t);
		}
		if(flags & QB_OP_NEED_LINE_NUMBER) {
			length += sizeof(uint32_t);
		}
	}
	return length;
}

static uint32_t ZEND_FASTCALL qb_get_instruction_length(qb_compiler_context *cxt) {
	uint32_t instruction_offset, i;

	// determine the offsets of each instruction in the stream
	instruction_offset = sizeof(void *);
	for(i = 0; i < cxt->op_count; i++) {
		qb_op *qop = cxt->ops[i];
		qop->instruction_offset = instruction_offset;
		if(qop->opcode != QB_NOP) {
			uint32_t instruction_length = qb_get_op_encoded_length(cxt, qop);
			instruction_offset += instruction_length;
		}
	}
	return instruction_offset;
}

static void ZEND_FASTCALL qb_encode_address(qb_compiler_context *cxt, qb_address *address, uint32_t address_mode, int8_t **p_ip) {
	switch(address_mode) {
		case QB_ADDRESS_MODE_VAR: {
			uint32_t index, operand;

			index = address->segment_offset >> type_size_shifts[address->type];
			operand = index;
			*((uint32_t *) *p_ip) = operand; *p_ip += sizeof(uint32_t);
		}	break;
		case QB_ADDRESS_MODE_ELC: {
			uint32_t index, operand;

			index = address->segment_offset >> type_size_shifts[address->type];
			operand = (index << 8) | address->segment_selector;
			*((uint32_t *) *p_ip) = operand; *p_ip += sizeof(uint32_t);
		}	break;
		case QB_ADDRESS_MODE_ELV: {
			uint32_t index_index, operand;
			qb_address *index_address;

			if(address->array_index_address) {
				index_address = address->array_index_address;
			} else {
				uint32_t value = ELEMENT_COUNT(address->segment_offset, address->type);
				index_address = qb_obtain_constant_U32(cxt, value);
			}
			index_index = ELEMENT_COUNT(index_address->segment_offset, index_address->type);
			operand = (index_index << 16) | (index_address->segment_selector << 8) | address->segment_selector;
			*((uint32_t *) *p_ip) = operand; *p_ip += sizeof(uint32_t);
		}	break;
		case QB_ADDRESS_MODE_ARR: {
			uint32_t index_index, size_index, operand;
			qb_address *index_address, *size_address;

			if(address->array_size_address) {
				size_address = address->array_size_address;
			} else {
				size_address = qb_obtain_constant_U32(cxt, 1);
			}
			if(address->array_index_address) {
				index_address = address->array_index_address;
			} else {
				uint32_t value = ELEMENT_COUNT(address->segment_offset, address->type);
				index_address = qb_obtain_constant_U32(cxt, value);
			}
			size_index = ELEMENT_COUNT(size_address->segment_offset, size_address->type);
			index_index = ELEMENT_COUNT(index_address->segment_offset, index_address->type);
			operand = (size_index << 20) | (index_index << 8) | address->segment_selector;
			*((uint32_t *) *p_ip) = operand; *p_ip += sizeof(uint32_t);
		}	break;
		default:
			qb_abort("Invalid address type");
	}
}

static zend_always_inline void qb_encode_address_attributes(qb_compiler_context *cxt, qb_address *address, int8_t **p_ip) {
	uint32_t flags = address->type | (address->dimension_count << 8) | ((address->flags & QB_ADDRESS_RUNTIME_FLAGS) << 16);
	*((uint32_t *) *p_ip) = flags; *p_ip += sizeof(uint32_t);
}

static void ZEND_FASTCALL qb_encode_handler(qb_compiler_context *cxt, uint32_t target_qop_index, int8_t **p_ip) {
	qb_op *target_qop;

	while(target_qop = cxt->ops[target_qop_index], target_qop->opcode == QB_NOP) {
		target_qop_index++;
	}

	// put in the inverse of the opcode--it will be replaced with the actual pointer during relocation
	*((void **) *p_ip) = (void *) ~((uintptr_t) target_qop->opcode); *p_ip += sizeof(void *);
}

static void ZEND_FASTCALL qb_encode_jump_target(qb_compiler_context *cxt, uint32_t jump_target_index, uint32_t current_op_index, int8_t **p_ip) {
	uint32_t target_qop_index = qb_get_jump_target_absolute_index(cxt, current_op_index, jump_target_index);
	qb_op *target_qop;

	while(target_qop = cxt->ops[target_qop_index], target_qop->opcode == QB_NOP) {
		target_qop_index++;
	}

	// the opcode and offset will be replaced with actual pointers during relocation
	*((void **) *p_ip) = (void *) ~((uintptr_t) target_qop->opcode); *p_ip += sizeof(void *);
	*((int8_t **) *p_ip) = (int8_t *) (uintptr_t) target_qop->instruction_offset; *p_ip += sizeof(int8_t *);
}

static void ZEND_FASTCALL qb_encode_instructions(qb_compiler_context *cxt) {
	uint32_t i, j, k;
	int8_t *ip;

	if(!cxt->instructions) {
		cxt->instruction_length = qb_get_instruction_length(cxt);
		cxt->instructions = emalloc(cxt->instruction_length);
	}
	ip = cxt->instructions;

	//  encode the instruction stream in the following manner:
	//  [op1 handler][op2 handler][op1 operands][op3 handler][op2 operands][op3 operands]...
	//  each instruction thus contains the handler of the *next* instruction
	for(i = 0; i < cxt->op_count; i++) {
		qb_op *qop = cxt->ops[i];

		if(qop->opcode != QB_NOP) {

			if(ip == cxt->instructions) {
				// setting the handler for the "zeroth" instruction
				// before we relocate it, it's just the inverse of the opcode
				*((void **) ip) = (void *) ~((uintptr_t) qop->opcode); ip += sizeof(void *);
			}

#if ZEND_DEBUG
			if(ip != cxt->instructions + qop->instruction_offset) {
				qb_op *prev_qop;
				const char *op_name;
				uint32_t op_length;
				j = i;
				do {
					prev_qop = cxt->ops[--j];
				} while(prev_qop->opcode == QB_NOP);
				op_name = qb_get_op_name(cxt, prev_qop->opcode);
				op_length = qb_get_op_encoded_length(cxt, prev_qop);
				qb_abort("Incorrect instruction offset (previous op: %s, length = %d)", op_name, op_length);
			}
#endif

			// need the handler to the next instruction if it's not a jump operation
			if(!(qop->flags & QB_OP_JUMP)) {
				qb_encode_handler(cxt, i + 1, &ip);
			}

			for(j = 0; j < qop->operand_count; j++) {
				qb_operand *operand = &qop->operands[j];

				switch(operand->type) {
					case QB_OPERAND_ADDRESS_VAR: {
						qb_encode_address(cxt, operand->address, QB_ADDRESS_MODE_VAR, &ip);
					}	break;
					case QB_OPERAND_ADDRESS_ELC: {
						qb_encode_address(cxt, operand->address, QB_ADDRESS_MODE_ELC, &ip);
					}	break;
					case QB_OPERAND_ADDRESS_ELV: {
						qb_encode_address(cxt, operand->address, QB_ADDRESS_MODE_ELV, &ip);
					}	break;
					case QB_OPERAND_ADDRESS_ARR: {
						qb_encode_address(cxt, operand->address, QB_ADDRESS_MODE_ARR, &ip);
					}	break;
					case QB_OPERAND_ADDRESS_EXT_VAR: {
						// the extended operand types are used for function calls, when we need 
						// specify the type and other attributes 
						qb_encode_address_attributes(cxt, operand->address, &ip);
						qb_encode_address(cxt, operand->address, QB_ADDRESS_MODE_VAR, &ip);
					}	break;
					case QB_OPERAND_ADDRESS_EXT_ELV: {
						qb_encode_address_attributes(cxt, operand->address, &ip);
						qb_encode_address(cxt, operand->address, QB_ADDRESS_MODE_ELV, &ip);
					}	break;
					case QB_OPERAND_ADDRESS_EXT_ARR: {
						qb_encode_address_attributes(cxt, operand->address, &ip);
						qb_encode_address(cxt, operand->address, QB_ADDRESS_MODE_ARR, &ip);
						// encode the flags and addresses of each dimension
						for(k = 0; k < operand->address->dimension_count; k++) {
							qb_address *dimension_address = operand->address->dimension_addresses[k];
							qb_address *size_address = operand->address->array_size_addresses[k];
							qb_encode_address_attributes(cxt, dimension_address, &ip);
							qb_encode_address(cxt, dimension_address, QB_ADDRESS_MODE_VAR, &ip);
							qb_encode_address(cxt, size_address, QB_ADDRESS_MODE_VAR, &ip);
						}
					}	break;
					case QB_OPERAND_TOTAL_LENGTH: {
						*((uint16_t *) ip) = operand->operand_size; ip += sizeof(uint16_t);
					}	break;
					case QB_OPERAND_ARGUMENT_COUNT: {
						*((uint16_t *) ip) = operand->argument_count; ip += sizeof(uint16_t);
						if(qop->flags & QB_OP_NEED_LINE_NUMBER) {
							*((uint32_t *) ip) = qop->line_number; ip += sizeof(uint32_t);
						}
					}	break;
					case QB_OPERAND_EXTERNAL_SYMBOL: {
						*((uint32_t *) ip) = operand->symbol_index; ip += sizeof(uint32_t);
					}	break;
					case QB_OPERAND_JUMP_TARGET: {
						qb_encode_jump_target(cxt, operand->jump_target_index, i, &ip);
					}	break;
#if ZEND_DEBUG
					case QB_OPERAND_ADDRESS: {
						qb_abort("Unresolved address");
					}	break;
					default: {
						qb_abort("Unknown operand type: %d", operand->type);
					}	break;
#endif
				}
			}
			if(qop->flags & QB_OP_NEED_MATRIX_DIMENSIONS) {
				*((uint32_t *) ip) = qop->matrix_dimensions; ip += sizeof(uint32_t);
			}
			// put the line number at the end if it's needed (unless it's a variable length op)
			if(qop->flags & QB_OP_NEED_LINE_NUMBER) {
				if(!(qop->flags & QB_OP_VARIABLE_LENGTH)) {
					*((uint32_t *) ip) = qop->line_number; ip += sizeof(uint32_t);
				}
			}
		}
	}
}

static void ZEND_FASTCALL qb_relocate_instruction_range(qb_compiler_context *cxt, int8_t *ip_base, int8_t *ip_start, int8_t *ip_end, uint32_t current_opcode) {
	int8_t *ip = ip_start;
	uintptr_t base_address = (uintptr_t) ip_base;
#ifdef __GNUC__
	uintptr_t handler_address;
#endif
	uint32_t i;

	// Relocation is somewhat tricky. Due to instruction interleaving,
	// an opcode is separated from its operands by some unknown distance.
	// Typically, it's the length of the previous instruction--but not always.
	// The operands can actually be located ahead of the opcode if we arrive
	// there via a backward jump. To iterate through all instructions, we have
	// to carefully follow the program flow, branching if necessary.

	// We use uintptr_t here so that addresses show up as decimal numbers
	// in the debugger. Makes life a little easier.

	// For Microsoft Visual C, the VM is implemented as a giant switch statement.
	// We just leave the opcode in the instructions.

	while(ip < ip_end) {
		uintptr_t *p_next_handler = (uintptr_t *) ip;
		uint32_t next_opcode = (uint32_t) ~*p_next_handler;
		uint32_t op_flags = qb_get_op_flags(cxt, current_opcode), operand_flags;
		if(next_opcode >= QB_OPCODE_COUNT) {
			// it's already relocated
			break;
		}
#ifdef __GNUC__
		handler_address = (uintptr_t) op_handlers[next_opcode];
		*p_next_handler = handler_address;
#else
		*p_next_handler = next_opcode;
#endif
		ip += sizeof(uintptr_t);
		if(op_flags & QB_OP_JUMP) {
			uintptr_t *p_instruction_pointer = (uintptr_t *) ip;
			uintptr_t offset = *p_instruction_pointer;
			uintptr_t target_address = base_address + offset;
			*p_instruction_pointer = target_address;
			ip += sizeof(uintptr_t);

			// if other jump targets follow, perform relocation and branch to each of them
			for(i = 1; (operand_flags = qb_get_operand_flags(cxt, current_opcode, i)); i++) {
				if((operand_flags & 0x07) == QB_OPERAND_JUMP_TARGET) {
					uintptr_t *p_next_handler_alt = (uintptr_t *) ip;
					uintptr_t *p_instruction_pointer_alt = (uintptr_t *) (ip + sizeof(uintptr_t));
					uint32_t next_opcode_alt = (uint32_t) ~*p_next_handler_alt;
					uintptr_t offset_alt = *p_instruction_pointer_alt;
					uintptr_t target_address_alt = base_address + offset_alt;

#ifdef __GNUC__
					handler_address = (uintptr_t) op_handlers[next_opcode_alt];
					*p_next_handler_alt = handler_address;
#else
					*p_next_handler_alt = next_opcode_alt;
#endif
					*p_instruction_pointer_alt = target_address_alt;

					if(next_opcode_alt != QB_RET) {
						qb_relocate_instruction_range(cxt, ip_base, (int8_t *) target_address_alt, ip_end, next_opcode_alt);
					}
				} else {
					// jump targets are always encoded ahead of other operands
					// no need to continue
					break;
				}
			}

			// go to the first target
			ip = (int8_t *) target_address;
		} else {
			// nothing to relocation; just hop over the operands
			if(op_flags & QB_OP_VARIABLE_LENGTH) {
				uint16_t *p_total_operand_length = (uint16_t *) ip;
				uint16_t total_operand_length = *p_total_operand_length;
				ip += total_operand_length;
			} else {
				for(i = 0; (operand_flags = qb_get_operand_flags(cxt, current_opcode, i)); i++) {
					ip += sizeof(uint32_t);
				}
				if(op_flags & QB_OP_NEED_MATRIX_DIMENSIONS) {
					ip += sizeof(uint32_t);
				}
				if(op_flags & QB_OP_NEED_LINE_NUMBER) {
					ip += sizeof(uint32_t);
				}
			}
		}
		if(next_opcode != QB_RET) {
			current_opcode = next_opcode;
		} else {
			// the return instruction is zero-length: it doesn't have a next handler
			// so the loop has to end
			break;
		}
	}
}

static void ZEND_FASTCALL qb_relocate_instructions(qb_compiler_context *cxt) {
	int8_t *ip_start = cxt->instructions;
	int8_t *ip_end = ip_start + cxt->instruction_length;

	// The "zeroth" instruction is essentially a NOP.
	// It contains a pointer to the first instruction and nothing else.
	qb_relocate_instruction_range(cxt, ip_start, ip_start, ip_end, QB_NOP);
}

static void ZEND_FASTCALL qb_execute_op(qb_compiler_context *cxt, qb_op *op) {
	USE_TSRM
	qb_function _qfunc, *qfunc = &_qfunc;
	qb_variable _retval, *retval = &_retval;
	zend_function _zfunc, *zfunc = &_zfunc;
	qb_op **current_ops, *ops[2], _ret_op, *ret_op = &_ret_op;
	uint32_t current_op_count, current_segment_count;
	int8_t instructions[256];

	// change properties temporarily
	current_ops = cxt->ops;
	current_op_count = cxt->op_count;
	current_segment_count = cxt->storage->segment_count;
	cxt->ops = ops;
	cxt->op_count = 2;
	cxt->storage->segment_count = 2;

	ops[0] = op;
	ops[1] = ret_op;
	ret_op->flags = QB_OP_JUMP;
	ret_op->opcode = QB_RET;
	ret_op->operand_count = 0;
	ret_op->operands = NULL;
	qb_resolve_address_modes(cxt);

	memset(qfunc, 0, sizeof(qb_function));
	memset(retval, 0, sizeof(qb_variable));
	qfunc->local_storage = cxt->storage;
	qfunc->return_variable = retval;
	qfunc->variables = &retval;
	qfunc->instructions = cxt->instructions = instructions;
	qfunc->instruction_length = cxt->instruction_length = qb_get_instruction_length(cxt);
	qb_encode_instructions(cxt);
	qb_relocate_instructions(cxt);
	cxt->instructions = NULL;
	cxt->instruction_length = 0;

	zfunc->type = ZEND_INTERNAL_FUNCTION;
	zfunc->op_array.reserved[0] = qfunc;
#ifndef ZEND_ENGINE_2_1
	zfunc->internal_function.module = &qb_module_entry;
#endif
	zfunc->internal_function.handler = PHP_FN(qb_execute);

	qb_execute(zfunc, NULL, NULL, 0, NULL TSRMLS_CC);

	// restore properties
	cxt->ops = current_ops;
	cxt->op_count = current_op_count;
	cxt->storage->segment_count = current_segment_count;
}

extern PHP_FUNCTION(qb_execute);

static uint32_t ZEND_FASTCALL qb_get_address_length(qb_address *address) {
	uint32_t i, j;
	uint32_t length = sizeof(qb_address);	// the address itself
	if(!IS_SCALAR(address)) {
		if(address->dimension_count > 1) {
			length += sizeof(qb_address *) * address->dimension_count * 2;
			length += sizeof(qb_address) * (address->dimension_count * 2 - 1);
		} else {
			length += sizeof(qb_address);
		}
		if(address->index_alias_schemes) {
			// need a pointer per dimension
			length += sizeof(qb_index_alias_scheme *) * address->dimension_count;	
			for(i = 0; i < address->dimension_count; i++) {
				qb_index_alias_scheme *scheme = address->index_alias_schemes[i];
				if(scheme) {
					length += sizeof(qb_index_alias_scheme);
					// a string pointer and an uint32 per alias 
					length += (sizeof(const char *) + sizeof(uint32_t)) * scheme->dimension;
					for(j = 0; j < scheme->dimension; j++) {
						length += scheme->alias_lengths[j] + 1;
					}
					if(scheme->class_name) {
						length += scheme->class_name_length + 1;
					}
				}
			}
		}
	}
	return length;
}

static uint8_t * ZEND_FASTCALL qb_copy_address(qb_address *address, uint8_t *memory) {
#if ZEND_DEBUG
	uint32_t length = qb_get_address_length(address);
#endif
	uint8_t *p = memory;
	qb_address *src = address;
	qb_address *dst = (qb_address *) p; p += sizeof(qb_address);
	dst->type = src->type;
	dst->flags = src->flags;
	dst->segment_selector = src->segment_selector;
	dst->segment_offset = src->segment_offset;
	dst->dimension_count = src->dimension_count;

	if(IS_SCALAR(src)) {
		dst->mode = QB_ADDRESS_MODE_VAR;
	} else {
		uint32_t i, j;
		if(src->dimension_count > 1) {
			dst->dimension_addresses = (qb_address **) p; p += sizeof(qb_address *) * src->dimension_count;
			dst->array_size_addresses = (qb_address **) p; p += sizeof(qb_address *) * src->dimension_count;
			for(i = 0; i < src->dimension_count; i++) {
				dst->dimension_addresses[i] = (qb_address *) p; p += sizeof(qb_address);
				*dst->dimension_addresses[i] = *src->dimension_addresses[i];
				if(i == src->dimension_count - 1) {
					dst->array_size_addresses[i] = dst->dimension_addresses[i];
				} else {
					dst->array_size_addresses[i] = (qb_address *) p; p += sizeof(qb_address);
					*dst->array_size_addresses[i] = *src->array_size_addresses[i];
				}
			}
			dst->array_size_address = dst->array_size_addresses[0];
		} else {
			dst->dimension_addresses = &dst->array_size_address;
			dst->array_size_addresses = &dst->array_size_address;
			dst->array_size_address = (qb_address *) p; p += sizeof(qb_address);
			*dst->array_size_address = *src->array_size_address;
		}
		if(src->index_alias_schemes) {
			dst->index_alias_schemes = (qb_index_alias_scheme **) p; p += sizeof(qb_index_alias_scheme *) * src->dimension_count;
			for(i = 0; i < src->dimension_count; i++) {
				qb_index_alias_scheme *dst_scheme, *src_scheme = src->index_alias_schemes[i];
				if(src_scheme) {
					dst_scheme = (qb_index_alias_scheme *) p; p += sizeof(qb_index_alias_scheme);
					dst_scheme->dimension = src_scheme->dimension;
					dst_scheme->zend_class = src_scheme->zend_class;
					dst_scheme->aliases = (char **) p; p += sizeof(char *) * src_scheme->dimension;
					dst_scheme->alias_lengths = (uint32_t *) p; p += sizeof(uint32_t) * src_scheme->dimension;
					for(j = 0; j < src_scheme->dimension; j++) {
						dst_scheme->aliases[j] = (char *) p; p += src_scheme->alias_lengths[j] + 1;
						memcpy(dst_scheme->aliases[j], src_scheme->aliases[j], src_scheme->alias_lengths[j] + 1);
						dst_scheme->alias_lengths[j] = src_scheme->alias_lengths[j];
					}
					if(src_scheme->class_name) {
						dst_scheme->class_name = (char *) p; p+= src_scheme->class_name_length + 1;
						memcpy((char *) dst_scheme->class_name, src_scheme->class_name, src_scheme->class_name_length + 1);
						dst_scheme->class_name_length = src_scheme->class_name_length;
					}
					dst->index_alias_schemes[i] = dst_scheme;
				}
			}
		}
		dst->mode = QB_ADDRESS_MODE_ARR;
	}
#if ZEND_DEBUG
	if(memory + length != p) {
		qb_abort("Length mismatch");
	}
#endif
	return p;
}

static uint32_t ZEND_FASTCALL qb_get_variable_length(qb_variable *qvar) {
	uint32_t length = sizeof(qb_variable);
	if(qvar->name) {
		length += qvar->name_length + 1;
	}
	if(qvar->address) {
		length += qb_get_address_length(qvar->address);
		if(qvar->default_value_address) {
			length += qb_get_address_length(qvar->default_value_address);
		}
	}
	return length;
}

static uint8_t * ZEND_FASTCALL qb_copy_variable(qb_variable *qvar, uint8_t *memory) {
	uint8_t *p = memory;
#if ZEND_DEBUG
	uint32_t length = qb_get_variable_length(qvar);
#endif
	qb_variable *src = qvar;
	qb_variable *dst = (qb_variable *) p; p += sizeof(qb_variable);
	dst->flags = src->flags;
	dst->zend_class = src->zend_class;
	if(src->name) {
		dst->name = (char *) p; p += src->name_length + 1;
		memcpy((char *) dst->name, src->name, src->name_length + 1);
		dst->hash_value = src->hash_value;
		dst->name_length = src->name_length;

		// since we're going to free the op array later, update the pointer so
		// cxt->variables continue to point to a valid string (to avoid seeing garbage in the debugger)
		src->name = dst->name;
	}
	if(src->address) {
		dst->address = (qb_address *) p;
		p = qb_copy_address(src->address, p);
		if(src->default_value_address) {
			dst->default_value_address = (qb_address *) p;
			p = qb_copy_address(src->default_value_address, p);
		}
	}

#if ZEND_DEBUG
	if(memory + length != p) {
		qb_abort("Length mismatch");
	}
#endif
	return p;
}

static uint32_t qb_get_external_symbol_length(qb_external_symbol *symbol) {
	uint32_t length = sizeof(qb_external_symbol);
	length += symbol->name_length + 1;
	return length;
}

static uint8_t * ZEND_FASTCALL qb_copy_external_symbol(qb_external_symbol *symbol, uint8_t *memory) {
	uint8_t *p = memory;
	qb_external_symbol *src = symbol;
	qb_external_symbol *dst = (qb_external_symbol *) p; p += sizeof(qb_external_symbol);
	dst->type = src->type;
	dst->name = (char *) p; p += src->name_length + 1;
	memcpy((char *) dst->name, src->name, src->name_length + 1);
	dst->name_length = src->name_length;
	dst->pointer = src->pointer;

	// in case the function name disappears when the op_array is freed
	src->name = dst->name;
	return p;
}

static qb_function * ZEND_FASTCALL qb_replace_function(qb_compiler_context *cxt) {
	USE_TSRM

	// see how much memory is needed for storing everything except for the instructions
	uint32_t total_size, storage_size, func_name_len, filename_len, fn_flags, i;
	uint8_t *memory, *p;
	qb_function *qfunc, **p_qfunc;
	qb_storage *storage;
	char *func_name, *filename;
	zend_arg_info *arg_info;
	zend_function *zfunc = cxt->zend_function;
	zend_class_entry *scope;

	func_name_len = strlen(zfunc->common.function_name);
	filename_len = strlen(zfunc->op_array.filename);
	fn_flags = zfunc->common.fn_flags;
	scope = zfunc->common.scope;

	total_size = 0;
	total_size += sizeof(qb_function);
	total_size += sizeof(qb_variable *) * cxt->variable_count;
	for(i = 0; i < cxt->variable_count; i++) {
		total_size += qb_get_variable_length(cxt->variables[i]);
	}
	total_size += sizeof(qb_external_symbol *) * cxt->external_symbol_count;
	for(i = 0; i < cxt->external_symbol_count; i++) {
		total_size += qb_get_external_symbol_length(&cxt->external_symbols[i]);
	}
	total_size += sizeof(zend_arg_info) * zfunc->common.num_args;
	total_size += func_name_len + 1;
	total_size += filename_len + 1;

	storage_size = sizeof(qb_storage);
	storage_size += sizeof(qb_memory_segment) * cxt->storage->segment_count;
	for(i = 0; i < cxt->storage->segment_count; i++) {
		qb_memory_segment *segment = &cxt->storage->segments[i];
		if(segment->flags & QB_SEGMENT_PREALLOCATED) {
			storage_size = ALIGN(storage_size, 16);
			storage_size += BYTE_COUNT(segment->element_count, segment->type); 
		}
	}
	total_size = ALIGN(total_size, 16);
	total_size += storage_size;

	// allocate memory and copy everything into a continuous block of memory
	memory = emalloc(total_size);
	memset(memory, 0, total_size);
	p = memory;
	qfunc = (qb_function *) p; p += sizeof(qb_function);

	// copy variables
	qfunc->variables = (qb_variable **) p; p += sizeof(qb_variable *) * cxt->variable_count;
	qfunc->variable_count = cxt->variable_count;
	for(i = 0; i < cxt->variable_count; i++) {
		qfunc->variables[i] = (qb_variable *) p;
		p = qb_copy_variable(cxt->variables[i], p);
	}

	// copy external symbols
	qfunc->external_symbols = (qb_external_symbol **) p; p += sizeof(qb_external_symbol *) * cxt->external_symbol_count;
	qfunc->external_symbol_count = cxt->external_symbol_count;
	for(i = 0; i < cxt->external_symbol_count; i++) {
		qfunc->external_symbols[i] = (qb_external_symbol *) p;
		p = qb_copy_external_symbol(&cxt->external_symbols[i], p);
	}

	// copy argument info
	arg_info = (zend_arg_info *) p; p += sizeof(zend_arg_info) * zfunc->common.num_args;
	for(i = 0; i < zfunc->common.num_args ; i++) {
		arg_info[i].pass_by_reference = zfunc->common.arg_info[i].pass_by_reference;
		arg_info[i].allow_null = zfunc->common.arg_info[i].allow_null;
		arg_info[i].name = qfunc->variables[i]->name;
		arg_info[i].name_len = qfunc->variables[i]->name_length;
	}

	// copy function name
	func_name = (char *) p; p += func_name_len + 1;
	memcpy(func_name, zfunc->common.function_name, func_name_len + 1);

	// copy script name
	filename = (char *) p; p += filename_len + 1;
	memcpy(filename, zfunc->op_array.filename, filename_len + 1);
	
	// set up local storage
	p = memory + ALIGN(p - memory, 16);
	storage = (qb_storage *) p;	p += sizeof(qb_storage);
	storage->flags = 0;
	storage->size = storage_size;
	storage->segment_count = cxt->storage->segment_count;
	storage->segments = (qb_memory_segment *) p; p += sizeof(qb_memory_segment) * cxt->storage->segment_count;
	memset(storage->segments, 0, sizeof(qb_memory_segment) * cxt->storage->segment_count);
	qfunc->local_storage = storage;

	// set up memory segments 
	for(i = 0; i < cxt->storage->segment_count; i++) {
		qb_memory_segment *src = &cxt->storage->segments[i];
		qb_memory_segment *dst = &storage->segments[i];
		dst->flags = src->flags;
		dst->type = src->type;
		dst->element_count = src->element_count;
		dst->array_size_pointer = &dst->element_count;
		dst->stack_ref_memory = &dst->memory;
		dst->stack_ref_element_count = &dst->element_count;

		// memory is preallocated
		if(src->flags & QB_SEGMENT_PREALLOCATED) {
			p = memory + ALIGN(p - memory, 16);
			dst->memory = (int8_t *) p; p += BYTE_COUNT(src->element_count, src->type);
			dst->current_allocation = src->element_count;
			if(src->memory) {
				memcpy(dst->memory, src->memory, BYTE_COUNT(src->element_count, src->type));
			}
		}
	}

#if ZEND_DEBUG
	if(memory + total_size != p) {
		qb_abort("Length mismatch");
	}
#endif

	// connect segments to arrays that make sure of them
	for(i = 0; i < cxt->array_count; i++) {
		qb_address *address = cxt->arrays[i];
		if(address->flags & QB_ADDRESS_SEGMENT) {
			qb_memory_segment *segment = &storage->segments[address->segment_selector];
			segment->array_size_pointer = ARRAY_IN(storage, U32, address->array_size_address);
			segment->dimension_pointer = ARRAY_IN(storage, U32, address->dimension_addresses[0]);

			// this is no redundant as the size of the array could be unknown initially (i.e. set to 0)
			// and then gets set by qb_initialize_dimensions()
			segment->element_count = *segment->array_size_pointer;

			if(IS_EXPANDABLE_ARRAY(address)) {
				if(address->dimension_count > 1) {
					// a multidimensional array always enlarges by the size of the subarray one dimension down
					segment->increment_pointer = ARRAY_IN(storage, U32, address->array_size_addresses[1]);
				}
			}
		}
	}

	for(i = 0; i < cxt->variable_count; i++) {
		qb_variable *qvar = qfunc->variables[i];
		if(qvar->address && qvar->address->flags & QB_ADDRESS_SEGMENT) {
			qb_memory_segment *segment = &storage->segments[qvar->address->segment_selector];
			if(qvar->flags & QB_VARIABLE_STATIC) {
				// flag segments used by static variables so they don't get freed at the end of function call
				segment->flags |= QB_SEGMENT_STATIC;
				// clear the initialization flag
				//qvar->address->flags &= ~QB_ADDRESS_INITIALIZED;
			}
		}
		if(qvar->flags & QB_VARIABLE_RETURN_VALUE) {
			qfunc->return_variable = qvar;
		}
#if ZEND_DEBUG
		if(qvar->address) {
			qb_validate_address(cxt, qvar->address);
		}
#endif
	}
	qfunc->argument_count = cxt->argument_count;
	qfunc->required_argument_count = cxt->required_argument_count;
	qfunc->instructions = cxt->instructions;
	qfunc->instruction_length = cxt->instruction_length;
	qfunc->instruction_crc64 = cxt->instruction_crc64;
	qfunc->name = func_name;
	qfunc->filename = filename;
	qfunc->native_proc = cxt->native_proc;
	qfunc->zend_function = zfunc;
	qfunc->flags = cxt->function_flags;
	cxt->instructions = NULL;

	// free the op array
	destroy_op_array(&zfunc->op_array TSRMLS_CC);
	memset(&zfunc->op_array, 0, sizeof(zend_op_array));

	// add the function to the global table so we can free it afterward
	if(!QB_G(compiled_functions)) {
		qb_create_array((void **) &QB_G(compiled_functions), &QB_G(compiled_function_count), sizeof(qb_function *), 16);
	}
	p_qfunc = qb_enlarge_array((void **) &QB_G(compiled_functions), 1);
	*p_qfunc = qfunc;

	// make the function internal function
	zfunc->type = ZEND_INTERNAL_FUNCTION;
	zfunc->common.fn_flags = fn_flags;
	zfunc->common.scope = scope;
#ifdef ZEND_ACC_DONE_PASS_TWO
	zfunc->common.fn_flags &= ~ZEND_ACC_DONE_PASS_TWO;
#endif
	zfunc->common.function_name = func_name;
	zfunc->common.arg_info = arg_info;
	zfunc->internal_function.handler = PHP_FN(qb_execute);
#if !ZEND_ENGINE_2_1
	zfunc->internal_function.module = &qb_module_entry;
#endif

	// store the pointer to the qb function in space vacated by the op array
	zfunc->op_array.reserved[0] = qfunc;
	return qfunc;
}

void ZEND_FASTCALL qb_initialize_compiler_data_pool(qb_compiler_data_pool *pool) {
	memset(pool, 0, sizeof(qb_compiler_data_pool));

	// have an array that keeps track of all other arrays
	qb_create_array((void **) &pool->arrays, &pool->array_count, sizeof(void *), 64);
	
	qb_create_block_allocator(&pool->op_allocator, sizeof(qb_op), 256);
	qb_create_block_allocator(&pool->address_allocator, sizeof(qb_address), 1024);
	qb_create_block_allocator(&pool->pointer_allocator, sizeof(void *), 256);
	qb_create_block_allocator(&pool->operand_allocator, sizeof(qb_operand), 1024);
	qb_create_block_allocator(&pool->array_initializer_allocator, sizeof(qb_array_initializer), 64);
	qb_create_block_allocator(&pool->index_alias_scheme_allocator, sizeof(qb_index_alias_scheme), 16);
	qb_create_block_allocator(&pool->string_allocator, sizeof(char), 1024);
	qb_create_block_allocator(&pool->uint32_allocator, sizeof(uint32_t), 64);
	qb_create_block_allocator(&pool->type_declaration_allocator, sizeof(qb_type_declaration), 256);
	qb_create_block_allocator(&pool->variable_allocator, sizeof(qb_variable), 256);
	qb_create_block_allocator(&pool->function_declaration_allocator, sizeof(qb_function_declaration), 16);
	qb_create_block_allocator(&pool->class_declaration_allocator, sizeof(qb_class_declaration), 16);
}

void ZEND_FASTCALL qb_free_compiler_data_pool(qb_compiler_data_pool *pool) {
	uint32_t i;
	for(i = pool->array_count - 1; (int32_t) i >= 0; i--) {
		void **array = pool->arrays[i];
		qb_destroy_array(array);
	}
	qb_destroy_array((void **) &pool->arrays);

	qb_destroy_block_allocator(&pool->op_allocator);
	qb_destroy_block_allocator(&pool->address_allocator);
	qb_destroy_block_allocator(&pool->pointer_allocator);
	qb_destroy_block_allocator(&pool->operand_allocator);
	qb_destroy_block_allocator(&pool->array_initializer_allocator);
	qb_destroy_block_allocator(&pool->index_alias_scheme_allocator);
	qb_destroy_block_allocator(&pool->string_allocator);
	qb_destroy_block_allocator(&pool->uint32_allocator);
	qb_destroy_block_allocator(&pool->type_declaration_allocator);
	qb_destroy_block_allocator(&pool->variable_allocator);
	qb_destroy_block_allocator(&pool->function_declaration_allocator);
	qb_destroy_block_allocator(&pool->class_declaration_allocator);

	if(pool->op_actions) {
		efree((void *) pool->op_actions);
	}
	if(pool->op_function_usages) {
		efree((void *) pool->op_function_usages);
	}
	if(pool->op_names) {
		efree((void *) pool->op_names);
	}
	if(pool->op_result_size_codes) {
		efree((void *) pool->op_result_size_codes);
	}
	if(pool->op_result_size_variables) {
		efree((void *) pool->op_result_size_variables);
	}
	if(pool->function_prototypes) {
		efree((void *) pool->function_prototypes);
	}
	if(pool->pbj_op_names) {
		efree((void *) pool->pbj_op_names);
	}
	if(pool->zend_op_names) {
		efree((void *) pool->zend_op_names);
	}
}

void ZEND_FASTCALL qb_initialize_compiler_context(qb_compiler_context *cxt, qb_compiler_data_pool *pool, qb_function_declaration *function_decl TSRMLS_DC) {
	uint32_t i;

	cxt->pool = pool;
	if(function_decl) {
		cxt->function_flags = function_decl->flags;
		if(QB_G(compile_to_native) && !(function_decl->flags & QB_ENGINE_NEVER_COMPILE)) {
			cxt->function_flags |= QB_ENGINE_COMPILE_IF_POSSIBLE;
		}
		if(QB_G(allow_debug_backtrace)) {
			cxt->function_flags |= QB_ENGINE_GO_THRU_ZEND;
		}
		cxt->function_declaration = function_decl;
		cxt->zend_function = function_decl->zend_function;
		cxt->zend_op_array = &cxt->zend_function->op_array;
		cxt->zend_class = cxt->zend_function->op_array.scope;
	}
	SAVE_TSRMLS();

	qb_attach_new_array(pool, (void **) &cxt->variables, &cxt->variable_count, sizeof(qb_variable *), 16);
	qb_attach_new_array(pool, (void **) &cxt->ops, &cxt->op_count, sizeof(qb_op *), 256);
	qb_attach_new_array(pool, (void **) &cxt->scalars, &cxt->scalar_count, sizeof(qb_address *), 64);
	qb_attach_new_array(pool, (void **) &cxt->arrays, &cxt->array_count, sizeof(qb_address *), 64);

	cxt->storage = emalloc(sizeof(qb_storage));
	cxt->storage->flags = 0;
	cxt->storage->size = 0;
	cxt->storage->segment_count = 2;
	cxt->storage->segments = emalloc(sizeof(qb_memory_segment) * MAX_SEGMENT_COUNT);
	memset(cxt->storage->segments, 0, sizeof(qb_memory_segment) * MAX_SEGMENT_COUNT);
	for(i = 0; i < MAX_SEGMENT_COUNT; i++) {
		qb_memory_segment *segment = &cxt->storage->segments[i];
		segment->stack_ref_element_count = segment->array_size_pointer = &segment->element_count;
		segment->stack_ref_memory = &segment->memory;
		if(i == QB_SELECTOR_VARIABLE || i == QB_SELECTOR_CONSTANT_ARRAY) {
			segment->flags |= QB_SEGMENT_PREALLOCATED;
		}
	}
}

void ZEND_FASTCALL qb_free_compiler_context(qb_compiler_context *cxt) {
	uint32_t i;

	if(cxt->storage) {
		for(i = 0; i < cxt->storage->segment_count; i++) {
			qb_memory_segment *segment = &cxt->storage->segments[i];
			if(segment->memory) {
				efree(segment->memory);
			}
		}
		efree(cxt->storage->segments);
		efree(cxt->storage);
	}
	if(cxt->instructions) {
		efree(cxt->instructions);
	}
	if(cxt->stack_items) {
		efree(cxt->stack_items);
	}
	if(cxt->external_code) {
		efree(cxt->external_code);
	}
}

static void ZEND_FASTCALL qb_initialize_build_context(qb_build_context *cxt TSRMLS_DC) {
	cxt->deferral_count = 0;
	cxt->pool = &cxt->_pool;
	qb_initialize_compiler_data_pool(cxt->pool);
	qb_attach_new_array(cxt->pool, (void **) &cxt->function_declarations, &cxt->function_declaration_count, sizeof(qb_function_declaration *), 16);
	qb_attach_new_array(cxt->pool, (void **) &cxt->class_declarations, &cxt->class_declaration_count, sizeof(qb_class_declaration *), 16);
	qb_attach_new_array(cxt->pool, (void **) &cxt->compiler_contexts, &cxt->compiler_context_count, sizeof(qb_compiler_context), 16);
	SAVE_TSRMLS();
}

void ZEND_FASTCALL qb_free_build_context(qb_build_context *cxt) {
	uint32_t i;
	for(i = 0; i < cxt->compiler_context_count; i++) {
		qb_compiler_context *compiler_cxt = &cxt->compiler_contexts[i];
		qb_free_compiler_context(compiler_cxt);
	}
	qb_free_compiler_data_pool(cxt->pool);
}

static void ZEND_FASTCALL qb_print_value(qb_compiler_context *cxt, int8_t *bytes, uint32_t type) {
	USE_TSRM
	char buffer[64];
	uint32_t len;
	len = qb_element_to_string(buffer, sizeof(buffer), bytes, type);
	php_write(buffer, len TSRMLS_CC);
}

static qb_variable * ZEND_FASTCALL qb_find_variable_with_address(qb_compiler_context *cxt, qb_address *address) {
	uint32_t i;
	for(i = 0; i < cxt->variable_count; i++) {
		qb_variable *qvar = cxt->variables[i];
		if(qvar->address == address) {
			return qvar;
		}
	}
	return NULL;
}

static qb_variable * ZEND_FASTCALL qb_find_variable_with_size_address(qb_compiler_context *cxt, qb_address *address, uint32_t *p_depth, int32_t *p_recursive) {
	uint32_t i, j;
	for(i = 0; i < cxt->variable_count; i++) {
		qb_variable *qvar = cxt->variables[i];
		if(qvar->address) {
			for(j = 0; j < qvar->address->dimension_count; j++) {
				if(qvar->address->dimension_addresses[j] == address) {
					*p_depth = j + 1;
					*p_recursive = FALSE;
					return qvar;
				} else if(qvar->address->array_size_addresses[j] == address) {
					*p_depth = j + 1;
					*p_recursive = TRUE;
					return qvar;
				}
			}
		}
	}
	return NULL;
}

static void ZEND_FASTCALL qb_print_address(qb_compiler_context *cxt, qb_address *address) {
	uint32_t i;
	if(address->flags & QB_ADDRESS_CONSTANT) {
		if(!IS_SCALAR(address)) {
			if(address->flags & QB_ADDRESS_STRING) {
				uint32_t len = VALUE(U32, address->array_size_address);
				char *string = (char *) ARRAY(U08, address);
				php_printf("\"%.*s\"", len, string);
			} else {
				uint32_t count = ARRAY_SIZE(address);
				int8_t *bytes = ARRAY(I08, address);
				php_printf("[");
				for(i = 0; i < count; i++) {
					qb_print_value(cxt, bytes, address->type);
					if(i < count - 1) {
						php_printf(", ");
					}
					if(i == 3 && count > 5) {
						php_printf("...(%d more)", count - 4);
						break;
					}
					bytes += BYTE_COUNT(1, address->type);
				}
				php_printf("]");
			}
		} else {
			if(address->flags & QB_ADDRESS_STRING) {
				char *string = (char *) ARRAY(U08, address);
				php_printf("\"%.*s\"", 1, string);
			} else{
				qb_print_value(cxt, ARRAY(I08, address), address->type);
			}
		}
	} else if(address->flags & (QB_ADDRESS_TEMPORARY | QB_ADDRESS_QM_TEMPORARY)) {
		const char *type_name = type_names[address->type], letter = type_name[0];
		uint32_t id = address->segment_selector * 1000 + address->segment_offset / 4;
		if(IS_SCALAR(address)) {
			php_printf("(%c%u)", letter, id);
		} else {
			if(IS_VARIABLE_LENGTH_ARRAY(address)) {
				php_printf("(%c%u...)", letter, id);
			} else {
				php_printf("(%c%u..%c%u)", letter, id, letter, id + (ARRAY_SIZE(address) - 1));
			}
		}
	} else if(address->flags & QB_ADDRESS_FOREACH_INDEX) {
		uint32_t id = address->segment_offset / 4;
		php_printf("(fe%u)", id);
	} else if(address->source_address) {
		if(address->source_address->dimension_count == address->dimension_count + 1) {
			// array element
			qb_print_address(cxt, address->source_address);
			php_printf("[");
			if(address->array_index_address) {
				qb_print_address(cxt, address->array_index_address);
			} else {
				php_printf("%u", ELEMENT_COUNT(address->segment_offset, address->type));
			}
			php_printf("]");
		} else if(address->source_address->type != address->type) {
			// a cast
			php_printf("(%s) ", type_names[address->type]);
			qb_print_address(cxt, address->source_address); 
		}
	} else {
		qb_variable *qvar;
		uint32_t depth;
		int32_t recursive;
		if((qvar = qb_find_variable_with_address(cxt, address))) {
			if(qvar->name) {
				if(qvar->flags & QB_VARIABLE_CLASS) {
					php_printf("self::$%s", qvar->name);
				} else if(qvar->flags & QB_VARIABLE_CLASS_INSTANCE) {
					php_printf("$this->%s", qvar->name);
				} else {
					php_printf("$%s", qvar->name);
				}
			} else {
				php_printf("(retval)");
			}
		} else if((qvar = qb_find_variable_with_size_address(cxt, address, &depth, &recursive))) {
			php_printf("count($%s", qvar->name);
			while(--depth > 0) {
				php_printf("[]");
			}
			php_printf((recursive) ? ", true)" : ")");
		} else {
			php_printf("(unknown)");
		}
	}
}

static void ZEND_FASTCALL qb_print_op(qb_compiler_context *cxt, qb_op *qop, uint32_t index) {
	uint32_t i;
	const char *name = qb_get_op_name(cxt, qop->opcode);
	php_printf("%04d: %s ", index, name);
	for(i = 0; i < qop->operand_count; i++) {
		qb_operand *operand = &qop->operands[i];
		switch(operand->type) {
			case QB_OPERAND_ADDRESS:
			case QB_OPERAND_ADDRESS_VAR:
			case QB_OPERAND_ADDRESS_ELC:
			case QB_OPERAND_ADDRESS_ELV:
			case QB_OPERAND_ADDRESS_ARR: {
				qb_print_address(cxt, operand->address);
			}	break;
			case QB_OPERAND_JUMP_TARGET: {
				uint32_t target_qop_index = qb_get_jump_target_absolute_index(cxt, index, operand->jump_target_index);
				php_printf("[%04d]", target_qop_index);
			}	break;
			case QB_OPERAND_ARGUMENT_COUNT: {
				php_printf("{args: %d}", operand->argument_count);
			}	break;
			case QB_OPERAND_TOTAL_LENGTH: {
				php_printf("{len: %d}", operand->operand_size);
			}	break;
			case QB_OPERAND_EXTERNAL_SYMBOL: {
				qb_external_symbol *symbol = &cxt->external_symbols[operand->symbol_index];
				php_printf("[%s]", symbol->name);
			}	break;
			default: {
				php_printf("(ERROR)");
				break;
			}
		}
		php_printf(" ");
	}
	php_printf("\n");
}

static void ZEND_FASTCALL qb_print_ops(qb_compiler_context *cxt) {
	uint32_t i;
	for(i = 0; i < cxt->op_count; i++) {
		qb_op *qop = cxt->ops[i];
		if(qop->opcode != QB_NOP) {
			qb_print_op(cxt, qop, i);
		}
	}
}

void ZEND_FASTCALL qb_load_external_code(qb_compiler_context *cxt, const char *import_path) {
	USE_TSRM
	php_stream *stream = php_stream_open_wrapper_ex((char *) import_path, "rb", USE_PATH | ENFORCE_SAFE_MODE | REPORT_ERRORS, NULL, NULL);
	if(stream) {
		char *data = NULL;
		size_t len = php_stream_copy_to_mem(stream, &data, PHP_STREAM_COPY_ALL, FALSE);
		php_stream_close(stream);

		if(len > 0) {
			cxt->external_code = data;
			cxt->external_code_length = len;
		}
	}
	if(!cxt->external_code) {
		qb_abort("Unable to load file containing external code");
	}
}

void ZEND_FASTCALL qb_free_external_code(qb_compiler_context *cxt) {
	if(cxt->external_code) {
		efree(cxt->external_code);
		cxt->external_code = NULL;
		cxt->external_code_length = 0;
	}
}

static zend_function * ZEND_FASTCALL qb_get_function(qb_build_context *cxt, zval *callable) {
	USE_TSRM

	zend_fcall_info_cache fcc;
	zend_function *zfunc = NULL;

#if !ZEND_ENGINE_2_2 && !ZEND_ENGINE_2_1
	char *error;
	if(!zend_is_callable_ex(callable, NULL, 0, NULL, NULL, &fcc, &error TSRMLS_CC)) {
		qb_abort("%s", error);
	}
	if(error) {
		efree(error);
	}
#else
	if(!zend_is_callable_ex(callable, 0, NULL, NULL, NULL, &fcc.function_handler, &fcc.object_pp TSRMLS_CC)) {
		qb_abort("Cannot find function");
	}
#endif
	zfunc = fcc.function_handler;
	if(zfunc->type != ZEND_USER_FUNCTION) {
		if(qb_is_compiled_function(zfunc)) {
			return NULL;
		} else {
			qb_abort("Cannot compile an internal PHP function: %s", zfunc->common.function_name);
		}
	}
	return zfunc;
}

#include "qb_compiler_php.c"
#include "qb_compiler_pbj.c"

int ZEND_FASTCALL qb_compile(zval *arg1, zval *arg2 TSRMLS_DC) {
	qb_build_context *cxt = QB_G(build_context);
	qb_compiler_context *compiler_cxt;
	Bucket *p, *q;
	uint32_t i, native_compile = FALSE;
	uint32_t action = QB_SCAN_FILE;
	zend_function *specified_function = NULL;
	const char *current_filename = EG(active_op_array)->filename;

	if(!cxt) {
		cxt = emalloc(sizeof(qb_build_context));
		qb_initialize_build_context(cxt TSRMLS_CC);
		QB_G(build_context) = cxt;
	}

	if(arg1 && Z_TYPE_P(arg1) != IS_NULL) {
		if(Z_TYPE_P(arg1) == IS_LONG) {
			action = Z_LVAL_P(arg1);
		} else if(Z_TYPE_P(arg1) == IS_BOOL) {
			action = Z_LVAL_P(arg1) ? QB_SCAN_FILE : QB_SCAN_ALL;
		} else if(Z_TYPE_P(arg1) == IS_DOUBLE) {
			action = (ulong) Z_DVAL_P(arg1);
		} else {
			// if it's a callable, then process it
			specified_function = qb_get_function(cxt, arg1);
		}
	}

	if(specified_function) {
		HashTable *ht = (Z_TYPE_P(arg2) == IS_ARRAY) ? Z_ARRVAL_P(arg2) : NULL;
		qb_function_declaration *function_decl = NULL;
		zend_class_entry *ce = specified_function->common.scope;
		int32_t fd_index = qb_find_function_declaration(cxt, specified_function);
		if(ht) {
			function_decl = qb_parse_function_declaration_table(cxt->pool, specified_function, ht);
		} else {
			// scan for declaration only if there wasn't one already
			if(fd_index == -1) {
				function_decl = qb_parse_function_doc_comment(cxt->pool, specified_function);
				if(!function_decl) {
					qb_abort("%s() does not have a Doc Comments block containing a valid declaration", specified_function->common.function_name);
				}
			} else {
				return SUCCESS;
			}
		}
		if(ce) {
			qb_class_declaration *class_decl = qb_parse_class_doc_comment(cxt->pool, ce);
			qb_add_class_declaration(cxt, class_decl);
			function_decl->class_declaration = class_decl;
		}
		qb_add_function_declaration(cxt, function_decl);
	} else {
		if(action == QB_START_DEFERRAL) {
			cxt->deferral_count++;
			return SUCCESS;
		} else if(action == QB_END_DEFERRAL){
			if(cxt->deferral_count > 1) {
				cxt->deferral_count--;
				return SUCCESS;
			} else {
				cxt->deferral_count = 0;
				// start compiling
			}
		} else if(action == QB_SCAN_FILE || action == QB_SCAN_ALL) {
			// look for user functions that need to be compiled
			for(p = EG(function_table)->pListTail; p; p = p->pListLast) {
				zend_function *zfunc = p->pData;
				if(zfunc->type == ZEND_USER_FUNCTION) {
					if(action == QB_SCAN_ALL || zfunc->op_array.filename == current_filename) {
						int32_t fd_index = qb_find_function_declaration(cxt, zfunc);
						if(fd_index == -1) {
							qb_function_declaration *function_decl = qb_parse_function_doc_comment(cxt->pool, zfunc);
							if(function_decl) {
								qb_add_function_declaration(cxt, function_decl);
							}
						}
					}
				} else {
					if(!qb_is_compiled_function(zfunc)) {
						break;
					}
				}
			}

			// look for class methods
			for(q = EG(class_table)->pListTail; q; q = q->pListLast) {
				zend_class_entry **p_ce = q->pData, *ce = *p_ce;
				if(ce->type == ZEND_USER_CLASS) {
					if(action == QB_SCAN_ALL || Z_CLASS_INFO(ce, filename)) {
						int32_t cd_index = qb_find_class_declaration(cxt, ce);
						if(cd_index == -1) {
							qb_class_declaration *class_decl = NULL;
							for(p = ce->function_table.pListHead; p; p = p->pListNext) {
								zend_function *zfunc = p->pData;
								int32_t fd_index = qb_find_function_declaration(cxt, zfunc);
								if(fd_index == -1) {
									qb_function_declaration *function_decl = qb_parse_function_doc_comment(cxt->pool, zfunc);
									if(function_decl) {
										if(!class_decl) {
											// parse the class doc comment if there's a method that's going to be translated
											class_decl = qb_parse_class_doc_comment(cxt->pool, ce);
											qb_add_class_declaration(cxt, class_decl);
										}
										function_decl->class_declaration = class_decl;
										qb_add_function_declaration(cxt, function_decl);
									}
								}
							}
						}
					}
				} else {
					break;
				}
			}

		}
	}
	if(cxt->deferral_count > 0) {
		return SUCCESS;
	}

	if(cxt->function_declaration_count) {
		// create the compiler contexts for all functions to be compiled 
		for(i = 0; i < cxt->function_declaration_count; i++) {
			compiler_cxt = qb_enlarge_array((void **) &cxt->compiler_contexts, 1);
			qb_initialize_compiler_context(compiler_cxt, cxt->pool, cxt->function_declarations[i] TSRMLS_CC);

			QB_G(current_filename) = compiler_cxt->zend_function->op_array.filename;

			// add variables used within function
			qb_add_variables(compiler_cxt);

			// set up function prototypes so the functions can resolved against each other
			qb_initialize_function_prototype(compiler_cxt);
		}

		// translate the functions
		for(i = 0; i < cxt->compiler_context_count; i++) {
			compiler_cxt = &cxt->compiler_contexts[i];

			if(!compiler_cxt->function_declaration->import_path) {
				// show the zend opcodes if turned on
				if(QB_G(show_zend_opcodes)) {
					qb_print_zend_ops(compiler_cxt);
				}

				// translate the zend ops to intermediate qb ops
				qb_translate_instructions(compiler_cxt);
			} else {
				// load the code into memory
				qb_load_external_code(compiler_cxt, compiler_cxt->function_declaration->import_path);

				// decode the pbj data
				qb_pbj_decode(compiler_cxt);

				// show the pb opcodes if turned on
				if(QB_G(show_pbj_opcodes)) {
					qb_pbj_print_ops(compiler_cxt);
				}

				// map function arguments to PB kernel parameters
				qb_pbj_map_arguments(compiler_cxt);

				// create the main loop and translate the PB instructions
				qb_pbj_translate_instructions(compiler_cxt);

				// free the binary
				qb_free_external_code(compiler_cxt);
			}

			// fuse basic instructions into compound ones
			qb_fuse_instructions(compiler_cxt, 1);

			// make opcodes address-mode-specific
			qb_resolve_address_modes(compiler_cxt);

			// try to fuse more instructions
			qb_fuse_instructions(compiler_cxt, 2);

			// encode the instruction stream
			qb_encode_instructions(compiler_cxt);

			compiler_cxt->instruction_crc64 = qb_calculate_crc64((uint8_t *) compiler_cxt->instructions, compiler_cxt->instruction_length, 0);
			if(compiler_cxt->function_flags & QB_ENGINE_COMPILE_IF_POSSIBLE) {
				native_compile = TRUE;
			}

			// show the qb opcodes if turned on
			if(QB_G(show_opcodes)) {
				qb_print_ops(compiler_cxt);
			}
		}

#ifdef NATIVE_COMPILE_ENABLED
		if(native_compile) {
			if(QB_G(allow_native_compilation)) {
				qb_native_compile(TSRMLS_C);
			}
		}
		if(!QB_G(allow_bytecode_interpretation)) {
			for(i = 0; i < cxt->compiler_context_count; i++) {
				compiler_cxt = &cxt->compiler_contexts[i];
				if(!compiler_cxt->native_proc) {
					qb_abort("Unable to compile to native code");
				}
			}
		}
#endif

		for(i = 0; i < cxt->compiler_context_count; i++) {
			compiler_cxt = &cxt->compiler_contexts[i];

			// relocate the instruction stream
			qb_relocate_instructions(compiler_cxt);

			// create function object
			qb_replace_function(compiler_cxt);
		}
		qb_free_build_context(cxt);
		efree(cxt);
		QB_G(build_context) = NULL;
	} else {
		qb_abort("No qb functions found");
	}
	return SUCCESS;
}

void ZEND_FASTCALL qb_open_diagnostic_loop(qb_compiler_context *cxt) {
	// nothing needs to be done
}

#define DIAGNOSTIC_ITERATION	1000000

void ZEND_FASTCALL qb_close_diagnostic_loop(qb_compiler_context *cxt) {
	qb_variable *loop_counter = qb_allocate_variable(cxt->pool);
	qb_address * iteration_address = qb_obtain_constant_U32(cxt, DIAGNOSTIC_ITERATION);
	loop_counter->name = "i";
	loop_counter->name_length = 1;
	loop_counter->address = qb_create_scalar(cxt, QB_TYPE_U32);
	qb_add_variable(cxt, loop_counter);
	qb_create_nullary_op(cxt, &factory_increment, loop_counter->address);
	qb_create_comparison_branch_op(cxt, &factory_branch_on_less_than, 0, QB_INSTRUCTION_NEXT, loop_counter->address, iteration_address);
	qb_create_op(cxt, &factory_return, NULL, 0, NULL);
}

void ZEND_FASTCALL qb_create_diagnostic_loop(qb_compiler_context *cxt, uint32_t test_type) {
	qb_address *value1_address, *value2_address, *result_address, *intermediate_address;
	qb_open_diagnostic_loop(cxt);
	if(test_type != QB_DIAGNOSTIC_EMPTY) {
		switch(test_type) {
			case QB_DIAGNOSTIC_INT_ADD:
			case QB_DIAGNOSTIC_INT_MUL:
			case QB_DIAGNOSTIC_INT_DIV: 
			case QB_DIAGNOSTIC_INT_MAC: {
				value1_address = qb_obtain_constant_U32(cxt, 12345);
				value2_address = qb_obtain_constant_U32(cxt, 7);
				break;
			}
			case QB_DIAGNOSTIC_FLOAT_ADD:
			case QB_DIAGNOSTIC_FLOAT_MUL:
			case QB_DIAGNOSTIC_FLOAT_DIV:
			case QB_DIAGNOSTIC_FLOAT_MAC: {
				value1_address = qb_obtain_constant_F64(cxt, M_PI);
				value2_address = qb_obtain_constant_F64(cxt, M_E);
				break;
			}
			case QB_DIAGNOSTIC_VECTOR_ADD:
			case QB_DIAGNOSTIC_VECTOR_MUL:
			case QB_DIAGNOSTIC_VECTOR_DIV: 
			case QB_DIAGNOSTIC_VECTOR_MAC: {
				uint32_t i;
				float32_t v;
				value1_address = qb_create_fixed_length_array(cxt, QB_TYPE_F32, 4, 0);
				value2_address = qb_create_fixed_length_array(cxt, QB_TYPE_F32, 4, 0);
				for(i = 0, v = (float32_t) M_E; i < 4; i++) {
					ARRAY(F32, value1_address)[i] = v * 3;
					ARRAY(F32, value2_address)[i] = v * 8000;
					v += 10;
				}
				break;
			}
		}
		result_address = qb_obtain_temporary_variable(cxt, value1_address->type, value1_address->array_size_address);

		switch(test_type) {
			case QB_DIAGNOSTIC_INT_ADD:
			case QB_DIAGNOSTIC_FLOAT_ADD:
			case QB_DIAGNOSTIC_VECTOR_ADD: {
				qb_create_binary_op(cxt, &factory_add, value1_address, value2_address, result_address);
			}	break;
			case QB_DIAGNOSTIC_INT_MUL:
			case QB_DIAGNOSTIC_FLOAT_MUL:
			case QB_DIAGNOSTIC_VECTOR_MUL: {
				qb_create_binary_op(cxt, &factory_multiply, value1_address, value2_address, result_address);
			}	break;
			case QB_DIAGNOSTIC_INT_DIV:
			case QB_DIAGNOSTIC_FLOAT_DIV:
			case QB_DIAGNOSTIC_VECTOR_DIV: { 
				qb_create_binary_op(cxt, &factory_divide, value1_address, value2_address, result_address);
			}	break;
			case QB_DIAGNOSTIC_INT_MAC:
			case QB_DIAGNOSTIC_FLOAT_MAC:
			case QB_DIAGNOSTIC_VECTOR_MAC: {
				intermediate_address = qb_obtain_temporary_variable(cxt, value1_address->type, value1_address->array_size_address);
				qb_create_binary_op(cxt, &factory_multiply, value1_address, value2_address, intermediate_address);
				qb_create_binary_op(cxt, &factory_add, result_address, intermediate_address, result_address);
			}	break;
		}
	}
	qb_close_diagnostic_loop(cxt);
}

void ZEND_FASTCALL qb_run_diagnostic_loop(qb_compiler_context *cxt) {
	USE_TSRM
	qb_function _qfunc, *qfunc = &_qfunc;

	qb_fuse_instructions(cxt, 1);
	qb_resolve_address_modes(cxt);
	qb_fuse_instructions(cxt, 2);

	memset(qfunc, 0, sizeof(qb_function));
	qfunc->local_storage = cxt->storage;
	qfunc->variables = cxt->variables;
	qfunc->variable_count = cxt->variable_count;
	qfunc->instruction_length = cxt->instruction_length = qb_get_instruction_length(cxt);
	qfunc->instructions = cxt->instructions = emalloc(cxt->instruction_length);
	qb_encode_instructions(cxt);
	qb_relocate_instructions(cxt);
	qb_execute_internal(qfunc TSRMLS_CC);
}

int ZEND_FASTCALL qb_run_diagnostics(qb_diagnostics *info TSRMLS_DC) {
	qb_build_context _cxt, *cxt = &_cxt;
	qb_compiler_context *compiler_cxt;
	uint32_t i;

	qb_initialize_build_context(cxt TSRMLS_CC);

	for(i = 0; i < QB_DIAGNOSTIC_SPEED_TEST_COUNTS; i++) {
		double start_time, end_time, duration, instruction_per_sec;
		compiler_cxt = qb_enlarge_array((void **) &cxt->compiler_contexts, 1);
		qb_initialize_compiler_context(compiler_cxt, cxt->pool, NULL TSRMLS_CC);
		qb_create_diagnostic_loop(compiler_cxt, i);

		start_time = qb_get_high_res_timestamp();
		qb_run_diagnostic_loop(compiler_cxt);
		end_time = qb_get_high_res_timestamp();
		duration = end_time - start_time;
		if(duration > 0) {
			instruction_per_sec = DIAGNOSTIC_ITERATION / duration;
		} else {
			instruction_per_sec = NAN;
		}
		info->instruction_speeds[i] = instruction_per_sec;
	}

	qb_free_build_context(cxt);
	return SUCCESS;
}

int ZEND_FASTCALL qb_initialize_compiler(TSRMLS_D) {
	uint32_t i;
	const char *pcre_error = NULL;
	int pcre_error_offset = 0;

	// calculate the hash of inline function names for quicker look-ups
	for(i = 0; i < sizeof(intrinsic_functions) / sizeof(qb_intrinsic_function); i++) {
		qb_intrinsic_function *f = &intrinsic_functions[i];
		f->hash_value = zend_inline_hash_func(f->name, strlen(f->name) + 1);
	}

	func_decl_hashes[FUNC_DECL_ENGINE] = zend_inline_hash_func("engine", 7);
	func_decl_hashes[FUNC_DECL_IMPORT] = zend_inline_hash_func("import", 7);
	func_decl_hashes[FUNC_DECL_PARAM] = zend_inline_hash_func("param", 6);
	func_decl_hashes[FUNC_DECL_LOCAL] = zend_inline_hash_func("local", 6);
	func_decl_hashes[FUNC_DECL_STATIC] = zend_inline_hash_func("static", 7);
	func_decl_hashes[FUNC_DECL_GLOBAL] = zend_inline_hash_func("global", 7);
	func_decl_hashes[FUNC_DECL_VAR] = zend_inline_hash_func("var", 4);
	func_decl_hashes[FUNC_DECL_PROPERTY] = zend_inline_hash_func("property", 9);
	func_decl_hashes[FUNC_DECL_RETURN] = zend_inline_hash_func("return", 7);

	// compile regular expressions
	doc_comment_function_regexp = pcre_compile(DOC_COMMENT_FUNCTION_REGEXP, PCRE_MULTILINE, &pcre_error, &pcre_error_offset, NULL);
	type_decl_regexp = pcre_compile(TYPE_DECL_REGEXP, 0, &pcre_error, &pcre_error_offset, NULL);
	type_dim_regexp = pcre_compile(TYPE_DIM_REGEXP, PCRE_CASELESS, &pcre_error, &pcre_error_offset, NULL);
	type_dim_alias_regexp = pcre_compile(TYPE_DIM_ALIAS_REGEXP, PCRE_CASELESS, &pcre_error, &pcre_error_offset, NULL);
	identifier_regexp = pcre_compile(IDENTIFIER_REGEXP, 0, &pcre_error, &pcre_error_offset, NULL);

	return (doc_comment_function_regexp && type_decl_regexp && type_dim_regexp && type_dim_alias_regexp && identifier_regexp) ? SUCCESS : FAILURE;
}

void ZEND_FASTCALL qb_free_function(qb_function *qfunc) {
	uint32_t i;

	// set the type to a invalid value so clean_non_persistent_function() doesn't return ZEND_HASH_APPLY_STOP
	// when user functions are removed at the end of the request
	qfunc->zend_function->type = 255;

	// free memory segments
	// normally, only static arrays would be be present at this point
	// if we had bailed out during execution though, local arrays would remain 
	for(i = QB_SELECTOR_DYNAMIC_ARRAY_START; i < qfunc->local_storage->segment_count; i++) {
		qb_memory_segment *segment = &qfunc->local_storage->segments[i];
		if(segment->memory) {
			if(segment->flags & QB_SEGMENT_MAPPED) {
				// PHP should have clean it already
			} else if(!(segment->flags & (QB_SEGMENT_BORROWED | QB_SEGMENT_PREALLOCATED))) {
				efree(segment->memory);
			}
		}
	}

	// free the two memory blocks
	efree(qfunc->instructions);
	efree(qfunc);
}
