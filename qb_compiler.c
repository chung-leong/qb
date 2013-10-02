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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "qb.h"

extern zend_module_entry qb_module_entry;

static int32_t qb_find_function_declaration(qb_build_context *cxt, zend_function *zfunc) {
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

static int32_t qb_find_class_declaration(qb_build_context *cxt, zend_class_entry *ce) {
	uint32_t i = 0;
	for(i = 0; i < cxt->class_declaration_count; i++) {
		qb_class_declaration *decl = cxt->class_declarations[i];
		if(decl->zend_class == ce) {
			return i;
		}
	}
	return -1;
}

extern const qb_op_info global_op_info[];
extern const char *global_operand_codes[];

uint32_t qb_get_op_flags(qb_compiler_context *cxt, uint32_t opcode) {
	const qb_op_info *op = &global_op_info[opcode];
	return op->flags;
}

uint32_t qb_get_operand_count(qb_compiler_context *cxt, uint32_t opcode) {
	const qb_op_info *op = &global_op_info[opcode];
	const char *codes = global_operand_codes[op->format_index];
	return strlen(codes);
}

static qb_address_mode qb_get_operand_address_mode(qb_compiler_context *cxt, uint32_t opcode, uint32_t operand_index) {
	const qb_op_info *op = &global_op_info[opcode];
	const char *codes = global_operand_codes[op->format_index];
	char code = codes[operand_index];
	switch(code) {
		case 'S':
		case 's':
			return QB_ADDRESS_MODE_SCA;
		case 'E':
		case 'e':
			return QB_ADDRESS_MODE_ELE;
		case 'A':
		case 'a':
			return QB_ADDRESS_MODE_ARR;
		case 'c':
			return QB_ADDRESS_MODE_SCA;
	}
	return -1;
}

static int32_t qb_is_operand_write_target(qb_compiler_context *cxt, uint32_t opcode, uint32_t operand_index) {
	const qb_op_info *op = &global_op_info[opcode];
	const char *codes = global_operand_codes[op->format_index];
	char code = codes[operand_index];
	switch(code) {
		case 'S':
		case 'E':
		case 'A':
			return TRUE;
	}
	return FALSE;
}

extern const char compressed_table_op_names[];

static const char * qb_get_op_name(qb_compiler_context *cxt, uint32_t opcode) {
	if(!cxt->pool->op_names) {
		// decompress the opname table
		qb_uncompress_table(compressed_table_op_names, (void ***) &cxt->pool->op_names, NULL, 0);
		if(!cxt->pool->op_names) {
			return "?";
		}
	}
	return cxt->pool->op_names[opcode];
}

void qb_mark_as_writable(qb_compiler_context *cxt, qb_address *address) {
	if(READ_ONLY(address)) {
		address->flags &= ~QB_ADDRESS_READ_ONLY;
		if(address->source_address) {
			qb_mark_as_writable(cxt, address->source_address);
		}
	}
}

static void qb_mark_as_expandable(qb_compiler_context *cxt, qb_address *address) {
	qb_address *dimension_address = address->dimension_addresses[0];
	qb_address *array_size_address = address->array_size_address;
	dimension_address->flags &= ~QB_ADDRESS_READ_ONLY;
	array_size_address->flags &= ~QB_ADDRESS_READ_ONLY;
}

void qb_mark_as_static(qb_compiler_context *cxt, qb_address *address) {
	if(READ_ONLY(address)) {
		address->flags |= QB_ADDRESS_STATIC;

		if(VARIABLE_LENGTH_ARRAY(address)) {
			// dimensions that aren't constant must become static as well 
			uint32_t i;
			for(i = 0; i < address->dimension_count; i++) {
				qb_address *dimension_address = address->dimension_addresses[i];
				qb_address *array_size_address = address->dimension_addresses[i];
				if(!CONSTANT(dimension_address)) {
					dimension_address->flags |= QB_ADDRESS_STATIC;
				}
				if(!CONSTANT(array_size_address)) {
					array_size_address->flags |= QB_ADDRESS_STATIC;
				}
			}
		}
	}
}

void qb_mark_as_shared(qb_compiler_context *cxt, qb_address *address) {
	if(READ_ONLY(address)) {
		address->flags |= QB_ADDRESS_SHARED;

		if(VARIABLE_LENGTH_ARRAY(address)) {
			// dimensions that aren't constant must become shared as well 
			uint32_t i;
			for(i = 0; i < address->dimension_count; i++) {
				qb_address *dimension_address = address->dimension_addresses[i];
				qb_address *array_size_address = address->dimension_addresses[i];
				if(!CONSTANT(dimension_address)) {
					dimension_address->flags |= QB_ADDRESS_SHARED;
				}
				if(!CONSTANT(array_size_address)) {
					array_size_address->flags |= QB_ADDRESS_SHARED;
				}
			}
		}
	}
}

void qb_lock_address(qb_compiler_context *cxt, qb_address *address) {
	if(!ON_DEMAND(address)) {
		if(!IN_USE(address) && TEMPORARY(address)) {
			address->flags |= QB_ADDRESS_IN_USE;
			if(address->source_address) {
				qb_lock_address(cxt, address->source_address);
			}
			if(address->array_index_address) {
				qb_lock_address(cxt, address->array_index_address);
			}
		}
	}
}

void qb_unlock_address(qb_compiler_context *cxt, qb_address *address) {
	if(!ON_DEMAND(address)) {
		if(IN_USE(address) && TEMPORARY(address)) {
			address->flags &= ~QB_ADDRESS_IN_USE;
			if(address->source_address) {
				qb_unlock_address(cxt, address->source_address);
			}
			if(address->array_index_address) {
				qb_unlock_address(cxt, address->array_index_address);
			}
		}
	}
}

void qb_lock_operand(qb_compiler_context *cxt, qb_operand *operand) {
	if(operand->type == QB_OPERAND_ADDRESS) {
		qb_lock_address(cxt, operand->address);
	} else if(operand->type == QB_OPERAND_ARRAY_INITIALIZER) {
		uint32_t i;
		for(i = 0; i < operand->array_initializer->element_count; i++) {
			qb_lock_operand(cxt, &operand->array_initializer->elements[i]);
		}
	}
}

void qb_unlock_operand(qb_compiler_context *cxt, qb_operand *operand) {
	if(operand->type == QB_OPERAND_ADDRESS) {
		qb_unlock_address(cxt, operand->address);
	} else if(operand->type == QB_OPERAND_ARRAY_INITIALIZER) {
		uint32_t i;
		for(i = 0; i < operand->array_initializer->element_count; i++) {
			qb_unlock_operand(cxt, &operand->array_initializer->elements[i]);
		}
	}
}

static void qb_mark_as_non_local(qb_compiler_context *cxt, qb_address *address) {
	if(!(address->flags & QB_ADDRESS_NON_LOCAL)) {
		address->flags |= QB_ADDRESS_NON_LOCAL;
		if(address->source_address) {
			qb_mark_as_non_local(cxt, address->source_address);
		}
		if(EXPANDABLE_ARRAY(address)) {
			qb_mark_as_non_local(cxt, address->array_size_addresses[0]);
			qb_mark_as_non_local(cxt, address->dimension_addresses[0]);
		}
	}
}

static uint32_t qb_get_jump_target_absolute_index(qb_compiler_context *cxt, uint32_t current_qop_index, uint32_t target_index) {
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

void qb_mark_jump_target(qb_compiler_context *cxt, uint32_t current_qop_index, uint32_t target_index) {
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

static void qb_validate_address(qb_compiler_context *cxt, qb_address *address) {
}

static qb_address * qb_create_address_alias(qb_compiler_context *cxt, qb_address *address);

// return an address whose value is only calculated when used
static qb_address * qb_obtain_on_demand_value(qb_compiler_context *cxt, void *op_factory, qb_operand *operands, uint32_t operand_count) {
	qb_op_factory *f = op_factory;
	qb_address *address, **p_address;
	qb_expression *expr;
	qb_primitive_type expr_type;
	uint32_t i, j;

	// see if there's an existing expression
	for(i = 0; i < cxt->on_demand_expression_count; i++) {
		address = cxt->on_demand_expressions[i];
		expr = address->expression;
		if(expr->op_factory == op_factory) {
			int32_t match = TRUE;
			for(j = 0; j < expr->operand_count; j++) {
				if(expr->operands[j].type != operands[j].type || expr->operands[j].address != operands[j].address) {
					match = FALSE;
				}
			}
			if(match) {
				return address;
			}
		}
	}

	// create the expression
	expr = qb_allocate_expression(cxt->pool);
	expr->operands = qb_allocate_operands(cxt->pool, operand_count + 1);
	expr->operand_count = operand_count;
	expr->result = &expr->operands[operand_count];
	expr->op_factory = op_factory;
	for(i = 0; i < operand_count; i++) {
		expr->operands[i] = operands[i];
	}
	expr->result->type = QB_OPERAND_EMPTY;
	expr->result->address = NULL;

	// call the handler to obtain the result
	expr_type = f->resolve_type(cxt, f, expr->operands, expr->operand_count);
	f->set_final_result(cxt, f, expr_type, expr->operands, expr->operand_count, expr->result, NULL);

	// create an copy of the address and attach the expression to it
	address = qb_create_address_alias(cxt, expr->result->address);
	address->flags = QB_ADDRESS_ON_DEMAND_VALUE;
	address->expression = expr;

	// save it to the list
	p_address = qb_enlarge_array((void **) &cxt->on_demand_expressions, 1);
	*p_address = address;
	return address;
}

// the qb_obtain_on_demand_??? functions only works with U32 scalar addresses
static qb_address * qb_obtain_on_demand_sum(qb_compiler_context *cxt, qb_address *augend_address, qb_address *addend_address) {
	if(CONSTANT(augend_address) && CONSTANT(addend_address)) {
		uint32_t sum = VALUE(U32, augend_address) + VALUE(U32, addend_address);
		return qb_obtain_constant_U32(cxt, sum);
	} else if(CONSTANT(augend_address) && VALUE(U32, augend_address) == 0) {
		// 0 + addend = addend
		return addend_address;
	} else if(CONSTANT(addend_address) && VALUE(U32, addend_address) == 0) {
		// augend + 0 = augend
		return augend_address;
	} else {
		qb_operand operands[2] = { { QB_OPERAND_ADDRESS, augend_address }, { QB_OPERAND_ADDRESS, addend_address } };
		return qb_obtain_on_demand_value(cxt, &factory_add, operands, 2);
	}
}

static qb_address * qb_obtain_bound_checked_sum(qb_compiler_context *cxt, qb_address *augend_address, qb_address *addend_address, qb_address *augend_address_limit) {
	if(CONSTANT(augend_address) && CONSTANT(addend_address) && CONSTANT(augend_address_limit)) {
		if(VALUE(U32, augend_address) < VALUE(U32, augend_address_limit)) {
			uint32_t sum = VALUE(U32, augend_address) + VALUE(U32, addend_address);
			return qb_obtain_constant_U32(cxt, sum);
		}
	}
	
	if(CONSTANT(augend_address) && VALUE(U32, augend_address) == 0) {
		// 0 + addend = addend
		return addend_address;
	} else if(CONSTANT(addend_address) && VALUE(U32, addend_address) == 0) {
		// augend + 0 = augend
		qb_operand operands[2] = { { QB_OPERAND_ADDRESS, augend_address }, { QB_OPERAND_ADDRESS, augend_address_limit } };
		return qb_obtain_on_demand_value(cxt, &factory_bound_check_index, operands, 2);
	} else {
		qb_operand operands[3] = { { QB_OPERAND_ADDRESS, augend_address }, { QB_OPERAND_ADDRESS, augend_address_limit }, { QB_OPERAND_ADDRESS, addend_address } };
		return qb_obtain_on_demand_value(cxt, &factory_bound_check_add, operands, 3);
	}
}

static qb_address * qb_obtain_on_demand_difference(qb_compiler_context *cxt, qb_address *minuend_address, qb_address *subtrahend_address) {
	if(CONSTANT(minuend_address) && CONSTANT(subtrahend_address)) {
		uint32_t difference = VALUE(U32, minuend_address) - VALUE(U32, subtrahend_address);
		return qb_obtain_constant_U32(cxt, difference);
	} else if(CONSTANT(subtrahend_address) && VALUE(U32, subtrahend_address) == 0) {
		// minuend - 0 = minuend
		return minuend_address;
	} else {
		qb_operand operands[2] = { { QB_OPERAND_ADDRESS, minuend_address }, { QB_OPERAND_ADDRESS, subtrahend_address } };
		return qb_obtain_on_demand_value(cxt, &factory_subtract, operands, 2);
	}
}

qb_address * qb_obtain_bound_checked_product(qb_compiler_context *cxt, qb_address *multiplicand_address, qb_address *multiplier_address, qb_address *multiplicand_limit_address) {
	if(CONSTANT(multiplicand_address) && CONSTANT(multiplier_address) && CONSTANT(multiplicand_limit_address)) {
		if(VALUE(U32, multiplicand_address) < VALUE(U32, multiplicand_limit_address)) {
			uint32_t product = VALUE(U32, multiplicand_address) * VALUE(U32, multiplier_address);
			return qb_obtain_constant_U32(cxt, product);
		}
	} 

	if(CONSTANT(multiplicand_address) && VALUE(U32, multiplicand_address) == 0) {
		// 0 * multiplier = 0
		return qb_obtain_constant_U32(cxt, 0);
	} else if(CONSTANT(multiplier_address) && VALUE(U32, multiplier_address) == 0) {
		// multiplicand * 0 = 0
		return qb_obtain_constant_U32(cxt, 0);
	} else if(CONSTANT(multiplicand_address) && VALUE(U32, multiplicand_address) == 1) {
		// 1 * multiplier = multiplier
		qb_operand operands[2] = { { QB_OPERAND_ADDRESS, multiplicand_address }, { QB_OPERAND_ADDRESS, multiplicand_limit_address } };
		return qb_obtain_on_demand_value(cxt, &factory_bound_check_index, operands, 2);
	} else if(CONSTANT(multiplier_address) && VALUE(U32, multiplier_address) == 1) {
		// multiplicand * 1 = multiplicand
		return multiplicand_address;
	} else {
		qb_operand operands[3] = { { QB_OPERAND_ADDRESS, multiplicand_address }, { QB_OPERAND_ADDRESS, multiplier_address }, { QB_OPERAND_ADDRESS, multiplicand_limit_address } };
		return qb_obtain_on_demand_value(cxt, &factory_bound_check_multiply, operands, 3);
	}
}

static qb_address * qb_obtain_on_demand_product(qb_compiler_context *cxt, qb_address *multiplicand_address, qb_address *multiplier_address) {
	if(CONSTANT(multiplicand_address) && CONSTANT(multiplier_address)) {
		uint32_t product = VALUE(U32, multiplicand_address) * VALUE(U32, multiplier_address);
		return qb_obtain_constant_U32(cxt, product);
	} else if(CONSTANT(multiplicand_address) && VALUE(U32, multiplicand_address) == 0) {
		// 0 * multiplier = 0
		return qb_obtain_constant_U32(cxt, 0);
	} else if(CONSTANT(multiplicand_address) && VALUE(U32, multiplicand_address) == 1) {
		// 1 * multiplier = multiplier
		return multiplier_address;
	} else if(CONSTANT(multiplier_address) && VALUE(U32, multiplier_address) == 0) {
		// multiplicand * 0 = 0
		return qb_obtain_constant_U32(cxt, 0);
	} else if(CONSTANT(multiplier_address) && VALUE(U32, multiplier_address) == 1) {
		// multiplicand * 1 = multiplicand
		return multiplicand_address;
	} else {
		qb_operand operands[2] = { { QB_OPERAND_ADDRESS, multiplicand_address }, { QB_OPERAND_ADDRESS, multiplier_address } };
		return qb_obtain_on_demand_value(cxt, &factory_multiply, operands, 2);
	}
}

static qb_address * qb_obtain_on_demand_quotient(qb_compiler_context *cxt, qb_address *numerator_address, qb_address *denominator_address) {
	if(CONSTANT(numerator_address) && CONSTANT(denominator_address)) {
		uint32_t quotient = VALUE(U32, numerator_address) / VALUE(U32, denominator_address);
		return qb_obtain_constant_U32(cxt, quotient);
	} else {
		qb_operand operands[2] = { { QB_OPERAND_ADDRESS, numerator_address }, { QB_OPERAND_ADDRESS, denominator_address } };
		return qb_obtain_on_demand_value(cxt, &factory_divide, operands, 2);
	}
}

static qb_address * qb_obtain_on_demand_greater_than(qb_compiler_context *cxt, qb_address *address1, qb_address *address2) {
	qb_operand operands[2] = { { QB_OPERAND_ADDRESS, address2 }, { QB_OPERAND_ADDRESS, address1 } };
	return qb_obtain_on_demand_value(cxt, &factory_less_than, operands, 2);
}

qb_address * qb_obtain_bound_checked_address(qb_compiler_context *cxt, qb_address *size_address, qb_address *address) {
	if(address->array_size_address == size_address) {
		// size match: no bound-checking needed
		return address;
	} else {
		qb_address *src_size_address = (size_address) ? size_address : cxt->one_address;
		qb_address *dst_size_address = (address->array_size_address) ? address->array_size_address : cxt->one_address;

		if(CONSTANT(dst_size_address) && CONSTANT(src_size_address) && VALUE(U32, dst_size_address) > VALUE(U32, src_size_address)) {
			// the destination is large enough
			return address;
		} else {
			qb_operand operands[2] = { { QB_OPERAND_ADDRESS, src_size_address }, { QB_OPERAND_ADDRESS, address } };
			return qb_obtain_on_demand_value(cxt, &factory_bound_check_array, operands, 2);
		}
	}
}

/*static qb_address * qb_obtain_on_demand_predicate(qb_compiler_context *cxt, qb_address *condition_address, qb_address *address_if_true, qb_address *address_if_false) {
	qb_address *operand_addresses[3] = { condition_address, address_if_true, address_if_false };
	return qb_obtain_on_demand_value(cxt, address_if_true->type, operand_addresses, 3, &factory_predicate_copy);
}*/

// allocate segment memory for address
static void qb_allocate_storage_space(qb_compiler_context *cxt, qb_address *address, int32_t need_actual_memory) {
	uint32_t selector, element_count, byte_count, new_segment_flags, alignment;
	uint32_t start_offset, end_offset;
	qb_memory_segment *segment;

	if(SCALAR(address)) {
		element_count = 1;
		byte_count = BYTE_COUNT(1, address->type);
		alignment = max(byte_count, 4);
	} else {
		if(FIXED_LENGTH_ARRAY(address)) {
			element_count = ARRAY_SIZE(address);
			byte_count = BYTE_COUNT(element_count, address->type);
		} else {
			element_count = 0;
			byte_count = 0;
		}
		alignment = 16;
	}

	// determine which segment should be used
	if(element_count == 1) {
		if(CONSTANT(address)) {
			selector = QB_SELECTOR_CONSTANT_SCALAR;
		} else if(TEMPORARY(address)) {
			selector = QB_SELECTOR_TEMPORARY_SCALAR;
		} else if(STATIC(address)) {
			selector = QB_SELECTOR_STATIC_SCALAR;
		} else if(SHARED(address)) {
			selector = QB_SELECTOR_SHARED_SCALAR;
		} else {
			selector = QB_SELECTOR_LOCAL_SCALAR;
		}
	} else if(element_count > 0 && byte_count <= 10240) {
		// the array is fixed-length and isn't bigger than 10K
		if(CONSTANT(address)) {
			selector = QB_SELECTOR_CONSTANT_ARRAY;
		} else if(TEMPORARY(address)) {
			selector = QB_SELECTOR_TEMPORARY_ARRAY;
		} else if(STATIC(address)) {
			selector = QB_SELECTOR_STATIC_ARRAY;
		} else if(SHARED(address)) {
			selector = QB_SELECTOR_SHARED_ARRAY;
		} else {
			selector = QB_SELECTOR_LOCAL_ARRAY;
		}
	} else {
		// it's a variable-length array or a big fixed-length array
		selector = cxt->storage->segment_count;
		if(TEMPORARY(address)) {
			new_segment_flags = QB_SEGMENT_FREE_ON_RETURN;
		} else if(STATIC(address)) {
			new_segment_flags = 0;
		} else if(SHARED(address)) {
			new_segment_flags = QB_SEGMENT_FREE_ON_RETURN | QB_SEGMENT_CLEAR_ON_CALL;
		} else {
			new_segment_flags = QB_SEGMENT_FREE_ON_RETURN | QB_SEGMENT_CLEAR_ON_CALL | QB_SEGMENT_SEPARATE_ON_FORK;
		}
		if(byte_count == 0) {
			if(new_segment_flags & QB_SEGMENT_FREE_ON_RETURN) {
				// don't just free the memory--set the segment length to 0 as well
				// so it doesn't get reallocated on the next call
				new_segment_flags |= QB_SEGMENT_EMPTY_ON_RETURN;
			}
		}
	}

	// create a new segment if necessary
	if(selector >= cxt->storage->segment_count) {
		cxt->storage->segment_count = selector + 1;
		cxt->storage->segments = erealloc(cxt->storage->segments, sizeof(qb_memory_segment) * cxt->storage->segment_count);
		segment = &cxt->storage->segments[selector];
		memset(segment, 0, sizeof(qb_memory_segment));
	} else {
		segment = &cxt->storage->segments[selector];
	}

	start_offset = ALIGN_TO(segment->byte_count, alignment);
	end_offset = start_offset + byte_count;

	// allocate memory if we're going to write to the address at compile time
	// should be true except when called by qb_assign_storage_space() down below
	if(need_actual_memory) {
		if(end_offset > segment->current_allocation) {
			segment->current_allocation += 1024;
			if(end_offset > segment->current_allocation) {
				segment->current_allocation = end_offset;
			}
			segment->memory = erealloc(segment->memory, segment->current_allocation);
		}
	}
	segment->byte_count = end_offset;

	// fit in the info
	address->segment_selector = selector;
	address->segment_offset = start_offset;
}

// assign storage space to variables
static void qb_assign_storage_space(qb_compiler_context *cxt) {
	uint32_t i;
	// set current_allocation to the byte_count so we know how much real 
	// data there's in each segment
	for(i = 0; i < cxt->storage->segment_count; i++) {
		qb_memory_segment *segment = &cxt->storage->segments[i];
		segment->current_allocation = segment->byte_count;
	}
	for(i = 0; i < cxt->writable_scalar_count; i++) {
		qb_address *address = cxt->writable_scalars[i];
		if(address->segment_selector == QB_SELECTOR_INVALID) {
			// if it's temporary yet remains read-only, then it was never used
			if(!TEMPORARY(address) || !READ_ONLY(address)) {
				qb_allocate_storage_space(cxt, address, FALSE);
			}
		}
	}
	for(i = 0; i < cxt->writable_array_count; i++) {
		qb_address *address = cxt->writable_arrays[i];
		if(address->segment_selector == QB_SELECTOR_INVALID) {
			if(!TEMPORARY(address) || !READ_ONLY(address)) {
				qb_allocate_storage_space(cxt, address, FALSE);
			}
		}
	}
}

static qb_address * qb_create_address_alias(qb_compiler_context *cxt, qb_address *address) {
	qb_address *alias = qb_allocate_address(cxt->pool);
	qb_address **p_alias = qb_enlarge_array((void **) &cxt->address_aliases, 1);
	*alias = *address;
	alias->source_address = address;
	*p_alias = alias;
	return alias;
}

static qb_address * qb_obtain_alias_by_address_flag(qb_compiler_context *cxt, qb_address *address, uint32_t flag) {
	qb_address *alias;
	if(address->flags & flag) {
		return address;
	} else {
		uint32_t i; 
		for(i = 0; i < cxt->address_alias_count; i++) {
			alias = cxt->address_aliases[i];
			if(alias->source_address == address) {
				if(alias->flags & QB_ADDRESS_STRING) {
					return alias;
				}
			}
		}
	}
	alias = qb_create_address_alias(cxt, address);
	alias->flags |= flag;
	return alias;
}

static qb_address * qb_obtain_string_alias(qb_compiler_context *cxt, qb_address *address) {
	return qb_obtain_alias_by_address_flag(cxt, address, QB_ADDRESS_STRING);
}

static qb_address * qb_obtain_boolean_alias(qb_compiler_context *cxt, qb_address *address) {
	return qb_obtain_alias_by_address_flag(cxt, address, QB_ADDRESS_BOOLEAN);
}

static qb_address * qb_obtain_cast_alias(qb_compiler_context *cxt, qb_address *address, qb_primitive_type type) {
	qb_address *alias;
	uint32_t i; 
	for(i = 0; i < cxt->address_alias_count; i++) {
		alias = cxt->address_aliases[i];
		if(alias->source_address == address) {
			if(alias->flags & QB_ADDRESS_CAST) {
				return alias;
			}
		}
	}
	alias = qb_create_address_alias(cxt, address);
	alias->flags |= QB_ADDRESS_CAST;
	alias->type = type;
	return alias;
}

static void qb_copy_dimensions(qb_compiler_context *cxt, qb_variable_dimensions *dim, qb_address *address);

static qb_address * qb_obtain_multidimensional_alias(qb_compiler_context *cxt, qb_address *address, qb_variable_dimensions *dim) {
	qb_address *alias;
	uint32_t i; 
	for(i = 0; i < cxt->address_alias_count; i++) {
		alias = cxt->address_aliases[i];
		if(alias->source_address == address) {
			if(alias->dimension_count == dim->dimension_count) {
				uint32_t j;
				int32_t match = TRUE;
				for(j = 0; j < alias->dimension_count; j++) {
					if(alias->dimension_addresses[j] != dim->dimension_addresses[j]) {
						match = FALSE;
						break;
					}
				}
				if(match) {
					return alias;
				}
			}
		}
	}
	alias = qb_create_address_alias(cxt, address);
	qb_copy_dimensions(cxt, dim, address);
	return alias;
}

static qb_address * qb_create_constant_scalar(qb_compiler_context *cxt, qb_primitive_type element_type) {
	qb_address *address = qb_allocate_address(cxt->pool);
	address->mode = QB_ADDRESS_MODE_SCA;
	address->type = element_type;
	address->flags = QB_ADDRESS_READ_ONLY | QB_ADDRESS_CONSTANT | QB_ADDRESS_ALWAYS_IN_BOUND;
	qb_allocate_storage_space(cxt, address, TRUE);
	qb_add_constant_scalar(cxt, address);
	return address;
}

static qb_address * qb_obtain_constant_I08(qb_compiler_context *cxt, int8_t value, uint32_t sign_flag) {
	qb_address *address;
	uint32_t i, type = QB_TYPE_I08 | sign_flag;
	for(i = 0; i < cxt->constant_scalar_count; i++) {
		address = cxt->constant_scalars[i];
		if(STORAGE_TYPE_MATCH(address->type, QB_TYPE_I08)) {
			if(VALUE(I08, address) == value) {
				if(address->type == type) {
					return address;
				} else {
					return qb_obtain_cast_alias(cxt, address, type);
				}
			}
		}
	}
	address = qb_create_constant_scalar(cxt, type);
	VALUE(I08, address) = value;
	return address;
}

qb_address * qb_obtain_constant_S08(qb_compiler_context *cxt, int8_t value) {
	return qb_obtain_constant_I08(cxt, value, 0);
}

qb_address * qb_obtain_constant_U08(qb_compiler_context *cxt, uint8_t value) {
	return qb_obtain_constant_I08(cxt, value, QB_TYPE_UNSIGNED);
}

static qb_address * qb_obtain_constant_I16(qb_compiler_context *cxt, int16_t value, uint32_t sign_flag) {
	qb_address *address;
	uint32_t i, type = QB_TYPE_I16 | sign_flag;
	for(i = 0; i < cxt->constant_scalar_count; i++) {
		address = cxt->constant_scalars[i];
		if(STORAGE_TYPE_MATCH(address->type, QB_TYPE_I16)) {
			if(VALUE(I16, address) == value) {
				if(address->type == type) {
					return address;
				} else {
					return qb_obtain_cast_alias(cxt, address, type);
				}
			}
		}
	}
	address = qb_create_constant_scalar(cxt, type);
	VALUE(I16, address) = value;
	return address;
}

qb_address * qb_obtain_constant_S16(qb_compiler_context *cxt, int16_t value) {
	return qb_obtain_constant_I16(cxt, value, 0);
}

qb_address * qb_obtain_constant_U16(qb_compiler_context *cxt, uint16_t value) {
	return qb_obtain_constant_I16(cxt, value, QB_TYPE_UNSIGNED);
}

static qb_address * qb_obtain_constant_I32(qb_compiler_context *cxt, int32_t value, uint32_t sign_flag) {
	qb_address *address;
	uint32_t i, type = QB_TYPE_I32 | sign_flag;
	for(i = 0; i < cxt->constant_scalar_count; i++) {
		address = cxt->constant_scalars[i];
		if(STORAGE_TYPE_MATCH(address->type, QB_TYPE_I32)) {
			if(VALUE(I32, address) == value) {
				if(address->type == type) {
					return address;
				} else {
					return qb_obtain_cast_alias(cxt, address, type);
				}
			}
		}
	}
	address = qb_create_constant_scalar(cxt, type);
	VALUE(I32, address) = value;
	return address;
}

qb_address * qb_obtain_constant_S32(qb_compiler_context *cxt, int32_t value) {
	return qb_obtain_constant_I32(cxt, value, 0);
}

qb_address * qb_obtain_constant_U32(qb_compiler_context *cxt, uint32_t value) {
	return qb_obtain_constant_I32(cxt, value, QB_TYPE_UNSIGNED);
}

static qb_address * qb_obtain_constant_I64(qb_compiler_context *cxt, int64_t value, uint32_t sign_flag) {
	qb_address *address;
	uint32_t i, type = QB_TYPE_I64 | sign_flag;
	for(i = 0; i < cxt->constant_scalar_count; i++) {
		address = cxt->constant_scalars[i];
		if(STORAGE_TYPE_MATCH(address->type, QB_TYPE_I64)) {
			if(VALUE(I64, address) == value) {
				if(address->type == type) {
					return address;
				} else {
					return qb_obtain_cast_alias(cxt, address, type);
				}
			}
		}
	}
	address = qb_create_constant_scalar(cxt, type);
	VALUE(I64, address) = value;
	return address;
}

qb_address * qb_obtain_constant_S64(qb_compiler_context *cxt, int64_t value) {
	return qb_obtain_constant_I64(cxt, value, 0);
}

qb_address * qb_obtain_constant_U64(qb_compiler_context *cxt, uint64_t value) {
	return qb_obtain_constant_I64(cxt, value, QB_TYPE_UNSIGNED);
}

qb_address * qb_obtain_constant_F32(qb_compiler_context *cxt, float32_t value) {
	qb_address *address;
	uint32_t i;
	for(i = 0; i < cxt->constant_scalar_count; i++) {
		address = cxt->constant_scalars[i];
		if(address->type == QB_TYPE_F32) {
			if(VALUE(F32, address) == value) {
				return address;
			}
		}
	}
	address = qb_create_constant_scalar(cxt, QB_TYPE_F32);
	VALUE(F32, address) = value;
	return address;
}

qb_address * qb_obtain_constant_F64(qb_compiler_context *cxt, float64_t value) {
	qb_address *address;
	uint32_t i;
	for(i = 0; i < cxt->constant_scalar_count; i++) {
		address = cxt->constant_scalars[i];
		if(address->type == QB_TYPE_F64) {
			if(VALUE(F64, address) == value) {
				return address;
			}
		}
	}
	address = qb_create_constant_scalar(cxt, QB_TYPE_F64);
	VALUE(F64, address) = value;
	return address;
}

qb_address * qb_obtain_constant_integer(qb_compiler_context *cxt, int64_t value, qb_primitive_type element_type) {
	switch(element_type) {
		case QB_TYPE_S08: return qb_obtain_constant_S08(cxt, (CTYPE(S08)) value);
		case QB_TYPE_U08: return qb_obtain_constant_U08(cxt, (CTYPE(U08)) value);
		case QB_TYPE_S16: return qb_obtain_constant_S16(cxt, (CTYPE(S16)) value);
		case QB_TYPE_U16: return qb_obtain_constant_U16(cxt, (CTYPE(U16)) value);
		case QB_TYPE_S32: return qb_obtain_constant_S32(cxt, (CTYPE(S32)) value);
		case QB_TYPE_U32: return qb_obtain_constant_U32(cxt, (CTYPE(U32)) value);
		case QB_TYPE_S64: return qb_obtain_constant_S64(cxt, (CTYPE(S64)) value);
		case QB_TYPE_U64: return qb_obtain_constant_U64(cxt, (CTYPE(U64)) value);
		default: break;
	}
	return NULL;
}

qb_address * qb_obtain_constant_float(qb_compiler_context *cxt, float64_t value, qb_primitive_type element_type) {
	switch(element_type) {
		case QB_TYPE_F32: return qb_obtain_constant_F32(cxt, (CTYPE(F32)) value);
		case QB_TYPE_F64: return qb_obtain_constant_F64(cxt, (CTYPE(F64)) value);
		default: break;
	}
	return NULL;
}

qb_address * qb_obtain_constant_boolean(qb_compiler_context *cxt, int32_t value) {
	if(value == 0) {
		return cxt->true_address;
	} else {
		return cxt->false_address;
	}
}

static qb_address * qb_create_constant_fixed_length_array(qb_compiler_context *cxt, qb_primitive_type element_type, uint32_t element_count) {
	qb_address *address = qb_allocate_address(cxt->pool);
	qb_address *size_address = qb_obtain_constant_U32(cxt, element_count);
	address->mode = QB_ADDRESS_MODE_ARR;
	address->type = element_type;
	address->flags = QB_ADDRESS_READ_ONLY | QB_ADDRESS_CONSTANT | QB_ADDRESS_ALWAYS_IN_BOUND;
	address->dimension_count = 1;
	address->dimension_addresses = &address->array_size_address;
	address->array_size_addresses = &address->array_size_address;
	address->array_size_address = size_address;
	address->array_index_address = cxt->zero_address;
	qb_allocate_storage_space(cxt, address, TRUE);
	qb_add_constant_array(cxt, address);
	return address;
}

qb_address * qb_create_writable_scalar(qb_compiler_context *cxt, qb_primitive_type element_type);

static void qb_set_variable_dimensions(qb_compiler_context *cxt, uint32_t *dimensions, uint32_t dimension_count, qb_variable_dimensions *dim) {
	uint32_t array_size = 1;
	uint32_t i;
	for(i = dimension_count - 1; (int32_t) i >= 0; i--) {
		uint32_t dimension = dimensions[i];
		qb_address *dimension_address;
		array_size *= dimension;
		if(dimension == 0) {
			// if it's zero, it's unknown at compile time
			dimension_address = qb_create_writable_scalar(cxt, QB_TYPE_U32);
		} else {
			dimension_address = qb_obtain_constant_U32(cxt, dimension);
		}
		dim->dimension_addresses[i] = dimension_address;
	}
	dim->array_size = array_size;
	dim->dimension_count = dimension_count;
	dim->source_address = NULL;
}

static void qb_copy_dimensions(qb_compiler_context *cxt, qb_variable_dimensions *dim, qb_address *address) {
	uint32_t i, array_size = 1;
	qb_address *previous_array_size_address = NULL;
	address->dimension_addresses = qb_allocate_address_pointers(cxt->pool, dim->dimension_count);
	address->array_size_addresses = qb_allocate_address_pointers(cxt->pool, dim->dimension_count);
	address->dimension_count = dim->dimension_count;
	for(i = dim->dimension_count - 1; (int32_t) i >= 0; i--) {
		qb_address *dimension_address = dim->dimension_addresses[i];
		qb_address *array_size_address;

		if(i == 0) {
			array_size_address = address->array_size_address;
			if(!dimension_address) {
				// the first dimension is not known if the result is variable-length
				// calculate it on demand from the length
				dimension_address = qb_obtain_on_demand_quotient(cxt, address->array_size_address, previous_array_size_address);
			}
		} else {
			if(!CONSTANT(dimension_address)) {
				// dimension is a variable--array sizes at this level and above will also be variable
				array_size = 0;
			}
			if(array_size) {
				array_size *= VALUE(U32, dimension_address);
				if(i == dim->dimension_count - 1) {
					array_size_address = dimension_address;
				} else {
					array_size_address = qb_obtain_constant_U32(cxt, array_size);
				}
			} else {
				// calculate the size on demand
				array_size_address = qb_obtain_on_demand_product(cxt, dimension_address, previous_array_size_address);
			}
		}
		address->dimension_addresses[i] = dimension_address;
		address->array_size_addresses[i] = array_size_address;
		previous_array_size_address = array_size_address;
	} 
}

qb_address * qb_create_constant_fixed_length_multidimensional_array(qb_compiler_context *cxt, qb_primitive_type element_type, uint32_t *dimensions, uint32_t dimension_count) {
	qb_variable_dimensions dim;
	qb_address *address;
	qb_set_variable_dimensions(cxt, dimensions, dimension_count, &dim);
	address = qb_create_constant_fixed_length_array(cxt, element_type, dim.array_size);
	if(dim.dimension_count > 1) {
		qb_copy_dimensions(cxt, &dim, address);
	}
	return address;
}

static void qb_add_dimensions(qb_compiler_context *cxt, uint32_t *dimensions, uint32_t dimension_count, qb_address *address) {
	/*
	uint32_t i;
	address->dimension_count = dimension_count;
	address->dimension_addresses = qb_allocate_address_pointers(cxt->pool, dimension_count);
	address->array_size_addresses = qb_allocate_address_pointers(cxt->pool, dimension_count);
	for(i = 0; i < dimension_count; i++) {
		qb_address *dimension_address;
		qb_address *array_size_address;
		uint32_t dimension = dimensions[i];
		dimension_address = (dimension) ? qb_obtain_constant_INDEX(cxt, dimension) : qb_create_writable_scalar(cxt, QB_TYPE_U32);
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
	*/
}

static uint32_t qb_get_zend_array_dimension_count(qb_compiler_context *cxt, zval *zvalue, qb_primitive_type element_type) {
	if((Z_TYPE_P(zvalue) == IS_ARRAY || Z_TYPE_P(zvalue) == IS_CONSTANT_ARRAY)) {
		HashTable *ht = Z_ARRVAL_P(zvalue);
		Bucket *p;
		uint32_t overall_sub_array_dimension_count = 0;
		for(p = ht->pListHead; p; p = p->pNext) {
			zval **p_element = p->pData;
			uint32_t sub_array_dimension_count = qb_get_zend_array_dimension_count(cxt, *p_element, element_type);
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
	} else if(Z_TYPE_P(zvalue) == IS_STRING) {
		return 1;
	} else {
		return 0;
	}
}

static void qb_get_zend_array_dimensions(qb_compiler_context *cxt, zval *zvalue, qb_primitive_type element_type, uint32_t *dimensions, uint32_t dimension_count) {
	if((Z_TYPE_P(zvalue) == IS_ARRAY || Z_TYPE_P(zvalue) == IS_CONSTANT_ARRAY)) {
		HashTable *ht = Z_ARRVAL_P(zvalue);
		Bucket *p;
		int32_t highest_index = -1;
		uint32_t dimension;
		for(p = ht->pListHead; p; p = p->pListNext) {
			if(p->nKeyLength > 0) {
				qb_abort("String key encountered (%s)", p->arKey);
			}
			if((long) p->h < 0) {
				qb_abort("Negative index encountered (%lu)", p->h);
			}
			if((long) p->h > highest_index) {
				highest_index = p->h;
			}
		}
		dimension = highest_index + 1;
		if(dimension > dimensions[0]) {
			dimensions[0] = dimension;
		}
		if(dimension_count > 1) {
			dimensions[1] = 0;
			for(p = ht->pListHead; p; p = p->pListNext) {
				zval **p_element = p->pData;
				qb_get_zend_array_dimensions(cxt, *p_element, element_type, dimensions + 1, dimension_count - 1);
			}
		}
	} else if(Z_TYPE_P(zvalue) == IS_STRING) {
		uint32_t byte_count = Z_STRLEN_P(zvalue);
		uint32_t dimension = byte_count >> type_size_shifts[element_type];
		if(byte_count != dimension * type_sizes[element_type]) {
			qb_abort("Number of bytes in string (%d) is not divisible by %d", byte_count, type_sizes[element_type]);
		}
		if(dimension > dimensions[0]) {
			dimensions[0] = dimension;
		}
	}
}

static void qb_copy_elements_from_zend_array(qb_compiler_context *cxt, zval *zvalue, qb_address *address);

static void qb_copy_element_from_zval(qb_compiler_context *cxt, zval *zvalue, qb_address *address) {
	if(Z_TYPE_P(zvalue) == IS_LONG) {
		switch(address->type) {
			case QB_TYPE_S08: VALUE(S08, address) = (CTYPE(S08)) Z_LVAL_P(zvalue); break;
			case QB_TYPE_U08: VALUE(U08, address) = (CTYPE(U08)) Z_LVAL_P(zvalue); break;
			case QB_TYPE_S16: VALUE(S16, address) = (CTYPE(S16)) Z_LVAL_P(zvalue); break;
			case QB_TYPE_U16: VALUE(U16, address) = (CTYPE(U16)) Z_LVAL_P(zvalue); break;
			case QB_TYPE_S32: VALUE(S32, address) = (CTYPE(S32)) Z_LVAL_P(zvalue); break;
			case QB_TYPE_U32: VALUE(U32, address) = (CTYPE(U32)) Z_LVAL_P(zvalue); break;
			case QB_TYPE_S64: VALUE(S64, address) = (CTYPE(S64)) Z_LVAL_P(zvalue); break;
			case QB_TYPE_U64: VALUE(U64, address) = (CTYPE(U64)) Z_LVAL_P(zvalue); break;
			case QB_TYPE_F32: VALUE(F32, address) = (CTYPE(F32)) Z_LVAL_P(zvalue); break;
			case QB_TYPE_F64: VALUE(F64, address) = (CTYPE(F64)) Z_LVAL_P(zvalue); break;
			default: break;
		}
	} else if(Z_TYPE_P(zvalue) == IS_DOUBLE) {
		switch(address->type) {
			case QB_TYPE_S08: VALUE(S08, address) = (CTYPE(S08)) (CTYPE(S64)) Z_DVAL_P(zvalue); break;
			case QB_TYPE_U08: VALUE(U08, address) = (CTYPE(U08)) (CTYPE(S64)) Z_DVAL_P(zvalue); break;
			case QB_TYPE_S16: VALUE(S16, address) = (CTYPE(S16)) (CTYPE(S64)) Z_DVAL_P(zvalue); break;
			case QB_TYPE_U16: VALUE(U16, address) = (CTYPE(U16)) (CTYPE(S64)) Z_DVAL_P(zvalue); break;
			case QB_TYPE_S32: VALUE(S32, address) = (CTYPE(S32)) (CTYPE(S64)) Z_DVAL_P(zvalue); break;
			case QB_TYPE_U32: VALUE(U32, address) = (CTYPE(U32)) (CTYPE(S64)) Z_DVAL_P(zvalue); break;
			case QB_TYPE_S64: VALUE(S64, address) = (CTYPE(S64)) Z_DVAL_P(zvalue); break;
			case QB_TYPE_U64: VALUE(U64, address) = (CTYPE(U64)) Z_DVAL_P(zvalue); break;
			case QB_TYPE_F32: VALUE(F32, address) = (CTYPE(F32)) Z_DVAL_P(zvalue); break;
			case QB_TYPE_F64: VALUE(F64, address) = (CTYPE(F64)) Z_DVAL_P(zvalue); break;
			default: break;
		}
	} else if(Z_TYPE_P(zvalue) == IS_STRING) {
		uint32_t type_size = type_sizes[address->type];
		uint32_t string_len = Z_STRLEN_P(zvalue);
		const char *string = Z_STRVAL_P(zvalue);
		if(type_size != string_len) {
			qb_abort("cannot convert string to %s due to size mismatch", type_names[address->type]);
		}
		switch(address->type) {
			case QB_TYPE_S08: VALUE(S08, address) = *((CTYPE(S08) *) string); break;
			case QB_TYPE_U08: VALUE(U08, address) = *((CTYPE(U08) *) string); break;
			case QB_TYPE_S16: VALUE(S16, address) = *((CTYPE(S16) *) string); break;
			case QB_TYPE_U16: VALUE(U16, address) = *((CTYPE(U16) *) string); break;
			case QB_TYPE_S32: VALUE(S32, address) = *((CTYPE(S32) *) string); break;
			case QB_TYPE_U32: VALUE(U32, address) = *((CTYPE(U32) *) string); break;
			case QB_TYPE_S64: VALUE(S64, address) = *((CTYPE(S64) *) string); break;
			case QB_TYPE_U64: VALUE(U64, address) = *((CTYPE(U64) *) string); break;
			case QB_TYPE_F32: VALUE(F32, address) = *((CTYPE(F32) *) string); break;
			case QB_TYPE_F64: VALUE(F64, address) = *((CTYPE(F64) *) string); break;
			default: break;
		}
	} else if(Z_TYPE_P(zvalue) == IS_ARRAY || Z_TYPE_P(zvalue) == IS_CONSTANT_ARRAY) {
		switch(address->type) {
			case QB_TYPE_S64:
			case QB_TYPE_U64: {
				VALUE(I64, address) = qb_zval_array_to_int64(zvalue);
			}	break;
			default:
				qb_abort("Cannot convert an array to %s", type_names[address->type]);
		}
	}
}

static void qb_copy_elements_from_zend_array(qb_compiler_context *cxt, zval *zvalue, qb_address *address) {
	qb_address *dimension_address = address->dimension_addresses[0];
	uint32_t dimension = VALUE(U32, dimension_address);
	qb_primitive_type element_type = address->type;
	uint32_t element_size = type_sizes[element_type], i;

	if((Z_TYPE_P(zvalue) == IS_ARRAY || Z_TYPE_P(zvalue) == IS_CONSTANT_ARRAY)) {
		HashTable *ht = Z_ARRVAL_P(zvalue);
		if(address->dimension_count > 1) {
			qb_address _sub_array_address, *sub_array_address = &_sub_array_address;
			uint32_t sub_array_array_size, sub_array_size;

			memset(sub_array_address, 0, sizeof(qb_address));
			sub_array_address->mode = QB_ADDRESS_MODE_ARR;
			sub_array_address->type = address->type;
			sub_array_address->flags = address->flags;
			sub_array_address->segment_selector = address->segment_selector;
			sub_array_address->segment_offset = address->segment_offset;
			sub_array_address->dimension_count = address->dimension_count - 1;
			sub_array_address->dimension_addresses = address->dimension_addresses + 1;
			sub_array_address->array_size_addresses = address->array_size_addresses + 1;
			sub_array_address->array_size_address = sub_array_address->array_size_addresses[0];
			sub_array_address->source_address = address;
			sub_array_array_size = VALUE(U32, sub_array_address->array_size_address);
			sub_array_size = element_size * sub_array_array_size;

			for(i = 0; i < dimension; i++) {
				zval **p_element;
				if(zend_hash_index_find(ht, i, (void **) &p_element) == SUCCESS) {
					qb_copy_elements_from_zend_array(cxt, *p_element, sub_array_address);
				} else {
					memset(ARRAY(I08, sub_array_address), 0, sub_array_size);
				}
				sub_array_address->segment_offset += sub_array_size;
			}
		} else {
			qb_address _element_address, *element_address = &_element_address;

			memset(element_address, 0, sizeof(qb_address));
			element_address->mode = QB_ADDRESS_MODE_SCA;
			element_address->type = address->type;
			element_address->flags = address->flags;
			element_address->segment_selector = address->segment_selector;
			element_address->segment_offset = address->segment_offset;
			element_address->source_address = address;

			for(i = 0; i < dimension; i++) {
				zval **p_element;
				if(zend_hash_index_find(ht, i, (void **) &p_element) == SUCCESS) {
					qb_copy_element_from_zval(cxt, *p_element, element_address);
				} else {
					memset(ARRAY(I08, element_address), 0, element_size);
				}
				element_address->segment_offset += element_size;
			}
		}
	} else if(Z_TYPE_P(zvalue) == IS_STRING) {
		uint32_t byte_count = Z_STRLEN_P(zvalue);
		uint32_t space_available = dimension * element_size;
		int8_t *memory = ARRAY(I08, address);
		memcpy(memory, Z_STRVAL_P(zvalue), byte_count);
		memset(memory + byte_count, 0, space_available - byte_count);
	}
}

static uint32_t qb_get_zval_array_type(qb_compiler_context *cxt, zval *array, uint32_t flags);

static uint32_t qb_get_zval_type(qb_compiler_context *cxt, zval *zvalue, uint32_t flags) {
	uint32_t type;
	switch(Z_TYPE_P(zvalue)) {
		case IS_BOOL: {
			type = QB_TYPE_I32;
		}	break;
		case IS_LONG: {
			int value = Z_LVAL_P(zvalue);
			if(flags & QB_COERCE_TO_FLOATING_POINT) {
				if(flags & QB_COERCE_TO_INTEGER_TO_DOUBLE) {
					type = QB_TYPE_F64;
				} else {
					// a single precision fp has 23 bits of precision
					type = (-0x007FFFFF <= value && value <= 0x007FFFFF) ?  QB_TYPE_F32 : QB_TYPE_F64;
				}
			} else {
				if(flags & QB_COERCE_TO_UNSIGNED) {
					if(value <= UINT8_MAX) {
						type = QB_TYPE_U08;
					} else if((unsigned int) value <= UINT16_MAX) {
						type = QB_TYPE_U16;
#if SIZEOF_LONG == 8
					} else if((unsigned int) value <= UINT32_MAX) {
						type = QB_TYPE_U32;
					} else {
						type = QB_TYPE_U64;
					}
#else
					} else {
						type = QB_TYPE_U32;
					}
#endif
				} else {
					if(INT8_MIN <= value && value <= INT8_MAX) {
						type = QB_TYPE_I08;
					} else if(INT16_MIN <= value && value <= INT16_MAX) {
						type = QB_TYPE_I16;
#if SIZEOF_LONG == 8
					} else if(INT32_MIN <= value && value <= INT32_MAX) {
						type = QB_TYPE_I32;
					} else {
						type = QB_TYPE_I64;
					}
#else
					} else {
						type = QB_TYPE_I32;
					}
#endif
				}
			}	break;
		}	break;
		case IS_DOUBLE: {
			double value = Z_DVAL_P(zvalue);
			if(flags & QB_COERCE_TO_INTEGER) {
				int64_t int_value = (int64_t) value;
				if(flags & QB_COERCE_TO_UNSIGNED) {
					if((uint64_t) int_value <= UINT8_MAX) {
						type = QB_TYPE_U08;
					} else if((uint64_t) int_value <= UINT16_MAX) {
						type = QB_TYPE_I16;
					} else if((uint64_t) int_value <= UINT32_MAX) {
						type = QB_TYPE_I32;
					} else {
						type = QB_TYPE_I64;
					}
				} else {
					if(INT8_MIN <= int_value && int_value <= INT8_MAX) {
						type = QB_TYPE_I08;
					} else if(INT16_MIN <= int_value && int_value <= INT16_MAX) {
						type = QB_TYPE_I16;
					} else if(INT32_MIN <= int_value && int_value <= INT32_MAX) {
						type = QB_TYPE_I32;
					} else {
						type = QB_TYPE_I64;
					}
				}
			} else {
				float32_t value32 = (float32_t) value;
				if((double) value32 == value) {
					// value can be stored as F32
					type = QB_TYPE_F32;
				} else {
					// value must be stored as F64
					type = QB_TYPE_F64;
				}
			}
		}	break;
		case IS_STRING: {
			type = QB_TYPE_U08;
		}	break;
		case IS_ARRAY: {
			type = qb_get_zval_array_type(cxt, zvalue, flags);
		}	break;
		case IS_NULL: {
			type = QB_TYPE_U08;
		}	break;
	}
	return type;
}

static uint32_t qb_get_zval_array_type(qb_compiler_context *cxt, zval *array, uint32_t flags) {
	HashTable *ht = Z_ARRVAL_P(array);
	Bucket *p;
	uint32_t highest_rank_type = 0;

	for(p = ht->pListHead; p; p = p->pNext) {
		zval **p_element = p->pData;
		uint32_t type = qb_get_zval_type(cxt, *p_element, flags);
		if(type > highest_rank_type) {
			highest_rank_type = type;
		}
	}
	return highest_rank_type;
}

qb_address * qb_obtain_constant_zval(qb_compiler_context *cxt, zval *zvalue, qb_primitive_type desired_type) {
	if((Z_TYPE_P(zvalue) == IS_ARRAY || Z_TYPE_P(zvalue) == IS_CONSTANT_ARRAY) || Z_TYPE_P(zvalue) == IS_STRING) {
		qb_address *address;
		qb_variable_dimensions dim;

		// figure out the dimensions of the array first
		uint32_t dimensions[MAX_DIMENSION];
		uint32_t dimension_count = qb_get_zend_array_dimension_count(cxt, zvalue, desired_type);
		dimensions[0] = 0;
		qb_get_zend_array_dimensions(cxt, zvalue, desired_type, dimensions, dimension_count);

		// create a local array for it
		qb_set_variable_dimensions(cxt, dimensions, dimension_count, &dim);
		address = qb_create_constant_fixed_length_array(cxt, desired_type, dim.array_size);
		if(dim.dimension_count > 1) {
			qb_copy_dimensions(cxt, &dim, address);
		}

		// copy the elements
		qb_copy_elements_from_zend_array(cxt, zvalue, address);

		if(Z_TYPE_P(zvalue) == IS_STRING) {
			if((desired_type & ~QB_TYPE_UNSIGNED) == QB_TYPE_I08) {
				// mark it as something that should print out as text
				address->flags |= QB_ADDRESS_STRING;
			}
		}
		return address;
	} else {
		switch(desired_type) {
			case QB_TYPE_S08: return qb_obtain_constant_S08(cxt, (CTYPE(S08)) qb_zval_to_long(zvalue));
			case QB_TYPE_U08: return qb_obtain_constant_U08(cxt, (CTYPE(U08)) qb_zval_to_long(zvalue));
			case QB_TYPE_S16: return qb_obtain_constant_S16(cxt, (CTYPE(S16)) qb_zval_to_long(zvalue));
			case QB_TYPE_U16: return qb_obtain_constant_U16(cxt, (CTYPE(U16)) qb_zval_to_long(zvalue));
			case QB_TYPE_S32: return qb_obtain_constant_S32(cxt, (CTYPE(S32)) qb_zval_to_long(zvalue));
			case QB_TYPE_U32: return qb_obtain_constant_U32(cxt, (CTYPE(U32)) qb_zval_to_long(zvalue));
			case QB_TYPE_S64: return qb_obtain_constant_S64(cxt, (CTYPE(S64)) qb_zval_to_long(zvalue));
			case QB_TYPE_U64: return qb_obtain_constant_U64(cxt, (CTYPE(U64)) qb_zval_to_long(zvalue));
			case QB_TYPE_F32: return qb_obtain_constant_F32(cxt, (CTYPE(F32)) qb_zval_to_double(zvalue));
			case QB_TYPE_F64: return qb_obtain_constant_F64(cxt, (CTYPE(F64)) qb_zval_to_double(zvalue));
			default: break;
		}
	}
	qb_abort("unable to convert constant");
	return NULL;
}

qb_address * qb_obtain_constant(qb_compiler_context *cxt, int64_t value, qb_primitive_type element_type) {
#if ZEND_DEBUG
	if(element_type >= QB_TYPE_COUNT) {
		qb_abort("invalid type id");
	}
#endif
	if(element_type >= QB_TYPE_F32) {
		return qb_obtain_constant_float(cxt, (float64_t) value, element_type);
	} else {
		return qb_obtain_constant_integer(cxt, value, element_type);
	}
}

qb_address * qb_create_writable_scalar(qb_compiler_context *cxt, qb_primitive_type element_type) {
	qb_address *address = qb_allocate_address(cxt->pool);
	address->segment_selector = QB_SELECTOR_INVALID;
	address->segment_offset = QB_OFFSET_INVALID;
	address->mode = QB_ADDRESS_MODE_SCA;
	address->type = element_type;

	// here "read-only" doesn't mean we're not supposed to write to this address
	// it just means no operation has done so yet
	address->flags = QB_ADDRESS_READ_ONLY | QB_ADDRESS_ALWAYS_IN_BOUND;
	qb_add_writable_scalar(cxt, address);
	return address;
}

qb_address * qb_obtain_temporary_scalar(qb_compiler_context *cxt, qb_primitive_type element_type) {
	qb_address *address;
	uint32_t i;
	for(i = 0; i < cxt->writable_scalar_count; i++) {
		qb_address *address = cxt->writable_scalars[i];
		if(TEMPORARY(address) && !IN_USE(address)) {
			if(address->type == element_type) {
				qb_lock_address(cxt, address);
				return address;
			}
		}
	}
	address = qb_create_writable_scalar(cxt, element_type);
	address->flags |= QB_ADDRESS_TEMPORARY;
	qb_lock_address(cxt, address);
	return address;
}

qb_address * qb_create_writable_fixed_length_array(qb_compiler_context *cxt, qb_primitive_type element_type, uint32_t element_count) {
	qb_address *address = qb_allocate_address(cxt->pool);
	qb_address *size_address = qb_obtain_constant_U32(cxt, element_count);
	address->mode = QB_ADDRESS_MODE_ARR;
	address->type = element_type;
	address->flags = QB_ADDRESS_READ_ONLY | QB_ADDRESS_ALWAYS_IN_BOUND;
	address->segment_selector = QB_SELECTOR_INVALID;
	address->segment_offset = QB_OFFSET_INVALID;
	address->dimension_count = 1;
	address->dimension_addresses = &address->array_size_address;
	address->array_size_addresses = &address->array_size_address;
	address->array_size_address = size_address;
	address->array_index_address = cxt->zero_address;
	qb_add_writable_array(cxt, address);
	return address;
}

qb_address * qb_create_writable_fixed_length_multidimensional_array(qb_compiler_context *cxt, qb_primitive_type element_type, uint32_t *dimensions, uint32_t dimension_count) {
	qb_address *address;
	qb_variable_dimensions dim;
	qb_set_variable_dimensions(cxt, dimensions, dimension_count, &dim);
	address = qb_create_writable_fixed_length_array(cxt, element_type, dim.array_size);
	if(dim.dimension_count > 1) {
		qb_copy_dimensions(cxt, &dim, address);
	}
	return address;
}

qb_address * qb_obtain_temporary_fixed_length_array(qb_compiler_context *cxt, qb_primitive_type element_type, uint32_t element_count) {
	qb_address *address;
	uint32_t i;
	for(i = 0; i < cxt->writable_array_count; i++) {
		qb_address *address = cxt->writable_arrays[i];
		if(TEMPORARY(address) && !IN_USE(address)) {
			if(FIXED_LENGTH_ARRAY(address)) {
				if(address->type == element_type) {
					if(ARRAY_SIZE(address) == element_count) {
						qb_lock_address(cxt, address);
						return address;
					}
				}
			}
		}
	}

	address = qb_create_writable_fixed_length_array(cxt, element_type, element_count);
	address->flags |= QB_ADDRESS_TEMPORARY;
	qb_lock_address(cxt, address);
	return address;
}

static qb_address * qb_create_writable_variable_length_array(qb_compiler_context *cxt, qb_primitive_type element_type) {
	qb_address *address = qb_allocate_address(cxt->pool);
	qb_address *size_address = qb_create_writable_scalar(cxt, QB_TYPE_U32);
	address->mode = QB_ADDRESS_MODE_ARR;
	address->type = element_type;
	address->flags = QB_ADDRESS_READ_ONLY | QB_ADDRESS_ALWAYS_IN_BOUND;
	address->segment_selector = QB_SELECTOR_INVALID;
	address->segment_offset = QB_OFFSET_INVALID;
	address->dimension_count = 1;
	address->dimension_addresses = &address->array_size_address;
	address->array_size_addresses = &address->array_size_address;
	address->array_size_address = size_address;
	address->array_index_address = cxt->zero_address;
	qb_add_writable_array(cxt, address);
	return address;
}

qb_address * qb_obtain_temporary_variable_length_array(qb_compiler_context *cxt, qb_primitive_type element_type) {
	qb_address *address;
	uint32_t i;
	for(i = 0; i < cxt->writable_array_count; i++) {
		address = cxt->writable_arrays[i];
		if(TEMPORARY(address) && !IN_USE(address)) {
			if(EXPANDABLE_ARRAY(address)) {
				if(address->type == element_type) {
					qb_lock_address(cxt, address);
					return address;
				}
			}
		}
	}
	address = qb_create_writable_variable_length_array(cxt, element_type);
	address->flags |= QB_ADDRESS_TEMPORARY;
	qb_lock_address(cxt, address);
	return address;
}

qb_address * qb_obtain_temporary_variable(qb_compiler_context *cxt, qb_primitive_type element_type, qb_variable_dimensions *dim) {
	qb_address *address;
	if(dim && dim->dimension_count > 0) {
		if(dim->array_size != 0) {
			address = qb_obtain_temporary_fixed_length_array(cxt, element_type, dim->array_size);
		} else {
			address = qb_obtain_temporary_variable_length_array(cxt, element_type);
		}
		if(dim->dimension_count > 1) {
			address = qb_obtain_multidimensional_alias(cxt, address, dim);
		}
	} else {
		address = qb_obtain_temporary_scalar(cxt, element_type);
	}
	return address;
}

qb_address * qb_obtain_non_reusable_temporary_variable(qb_compiler_context *cxt, qb_primitive_type element_type, qb_variable_dimensions *dim) {
	qb_address *address;
	if(dim && dim->dimension_count > 0) {
		if(dim->array_size != 0) {
			address = qb_create_writable_fixed_length_array(cxt, element_type, dim->array_size);
		} else {
			address = qb_create_writable_variable_length_array(cxt, element_type);
		}
		if(dim->dimension_count > 1) {
			qb_copy_dimensions(cxt, dim, address);
		}
	} else {
		address = qb_create_writable_scalar(cxt, element_type);
	}
	address->flags |= QB_ADDRESS_TEMPORARY;
	return address;
}

qb_address * qb_obtain_write_target(qb_compiler_context *cxt, qb_primitive_type element_type, qb_variable_dimensions *dim, qb_result_prototype *result_prototype) {
	if(result_prototype && result_prototype->destination) {
		qb_result_destination *destination = result_prototype->destination;
		qb_primitive_type lvalue_type = QB_TYPE_UNKNOWN;
		qb_address *lvalue_size_address = NULL, *lvalue_address = NULL;

		// figure out what kind of lvalue it is
		switch(destination->type) {
			case QB_RESULT_DESTINATION_VARIABLE: {
				qb_address *address = destination->variable.address;
				lvalue_type = address->type;
				lvalue_size_address = address->array_size_address;
			}	break;
			case QB_RESULT_DESTINATION_ELEMENT: {
				if(destination->element.container.type == QB_OPERAND_ADDRESS) {
					qb_address *address = destination->element.container.address;
					lvalue_type = address->type;
					lvalue_size_address = (address->dimension_count > 1) ? address->array_size_addresses[1] : NULL;
				}
			}	break;
			case QB_RESULT_DESTINATION_PROPERTY: {
				if(destination->property.container.type == QB_OPERAND_ADDRESS) {
					qb_address *address = destination->property.container.address;
					lvalue_type = address->type;
					lvalue_size_address = (address->dimension_count > 1) ? address->array_size_addresses[1] : NULL;
				}
			}	break;
			case QB_RESULT_DESTINATION_RETURN: {
				if(cxt->return_variable->address) {
					lvalue_type = cxt->return_variable->address->type;
					lvalue_size_address = cxt->return_variable->address->array_size_address;
				}
			}	break;
			default: break;
		}

		if(lvalue_type != QB_TYPE_UNKNOWN) {
			int substitute = FALSE;

			// see if the storage types do match (i.e. they are the same or differ only by signedness)
			if(STORAGE_TYPE_MATCH(element_type, lvalue_type)) {
				if(lvalue_size_address) {
					// see if we're assigned to a variable length array
					if(lvalue_size_address && !READ_ONLY(lvalue_size_address)) {
						// substitution always happens since the lvalue will expand to match the size
						substitute = TRUE;
					} else {
						if(dim && dim->array_size > 0) {
							// result size is known
							if(CONSTANT(lvalue_size_address)) {
								// size of lvalue is fixed	
								uint32_t lvalue_size = VALUE(U32, lvalue_size_address);
								if(lvalue_size == dim->array_size) {
									substitute = TRUE;
								} 
							}
						} else if(dim->source_address == destination->variable.address) {
							// the destination is same as the source
							substitute = TRUE;
						}
					}
				} else {
					// lvalue is a scalar
					if(!dim || dim->dimension_count == 0) {
						// result is scalar as well
						substitute = TRUE;
					}
				}
			}
			if(substitute) {
				// get the address and return it
				switch(destination->type) {
					case QB_RESULT_DESTINATION_VARIABLE: {
						lvalue_address = destination->variable.address;
					}	break;
					case QB_RESULT_DESTINATION_ELEMENT: {
						qb_operand *container = &destination->element.container, *index = &destination->element.index;
						if(index->type == QB_OPERAND_NONE) {
							// an append operation
							if(EXPANDABLE_ARRAY(container->address)) {
								index->address = container->address->dimension_addresses[0];
								index->type = QB_OPERAND_ADDRESS;
							} else {
								// don't substitute since it's an error condition
								break;
							}
						} else {
							// make sure the index is a U32
							qb_perform_type_coercion(cxt, index, QB_TYPE_U32, 0);
						}
						lvalue_address = qb_obtain_array_element(cxt, container->address, index->address, FALSE);
					}	break;
					case QB_RESULT_DESTINATION_PROPERTY: {
						qb_operand *container = &destination->property.container, *name = &destination->property.name;

						if(container->type == QB_OPERAND_NONE) {
							lvalue_address = qb_obtain_instance_variable(cxt, name->constant);
						}
						if(name->type == QB_OPERAND_ZVAL) {
							lvalue_address = qb_obtain_named_element(cxt, container->address, name->constant, QB_ARRAY_BOUND_CHECK_WRITE);
						}
					}	break;
					case QB_RESULT_DESTINATION_RETURN: {
						lvalue_address = cxt->return_variable->address;
					}	break;
					default: break;
				}

				if(lvalue_address) {
					// indicate that the assignment won't be necessary
					destination->prototype->final_type = QB_TYPE_VOID;
					return lvalue_address;
				}
			}
		}
	}

	// need temporary variable
	return qb_obtain_temporary_variable(cxt, element_type, dim);
}

static qb_address * qb_allocate_constant(qb_compiler_context *cxt, qb_primitive_type element_type, qb_variable_dimensions *dim) {
	qb_address *address;
	if(dim->dimension_count > 0) {
		address = qb_create_constant_fixed_length_array(cxt, element_type, dim->array_size);
		if(dim->dimension_count > 1) {
			qb_copy_dimensions(cxt, dim, address);
		}
	} else {
		address = qb_create_constant_scalar(cxt, element_type);
	}
	return address;
}

qb_operand * qb_push_stack_item(qb_compiler_context *cxt) {
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

qb_operand ** qb_get_stack_items(qb_compiler_context *cxt, int32_t index) {
	if(index < 0) {
		qb_abort("stack underflow");
	}
	return &cxt->stack_items[cxt->stack_item_offset + index];
}

qb_operand ** qb_pop_stack_items(qb_compiler_context *cxt, int32_t count) {
	int32_t index = cxt->stack_item_count - count;
	cxt->stack_item_count -= count;
	return qb_get_stack_items(cxt, index);
}

qb_operand * qb_expand_array_initializer(qb_compiler_context *cxt, qb_array_initializer *initializer, uint32_t required_index) {
	int32_t addition = (required_index + 1) - initializer->element_count;
	if(addition > 0) {
		qb_operand *new_elements = qb_enlarge_array((void **) &initializer->elements, addition);
		return &new_elements[addition - 1];
	} else {
		// it's been allocated already
		return &initializer->elements[required_index];
	}
}

static uint32_t qb_get_array_initializer_dimension_count(qb_compiler_context *cxt, qb_array_initializer *initializer, qb_primitive_type element_type) {
	uint32_t overall_sub_array_dimension_count = 0, i;

	for(i = 0; i < initializer->element_count; i++) {
		qb_operand *element = &initializer->elements[i];
		uint32_t sub_array_dimension_count;

		if(element->type == QB_OPERAND_ARRAY_INITIALIZER) {
			sub_array_dimension_count = qb_get_array_initializer_dimension_count(cxt, element->array_initializer, element_type);
		} else if(element->type == QB_OPERAND_ZVAL) {
			sub_array_dimension_count = qb_get_zend_array_dimension_count(cxt, element->constant, element_type);
		} else if(element->type == QB_OPERAND_ADDRESS && !SCALAR(element->address)) {
			sub_array_dimension_count = element->address->dimension_count;
		} else {
			continue;
		}
		if(overall_sub_array_dimension_count) {
			if(overall_sub_array_dimension_count != sub_array_dimension_count) {
				qb_abort("array has irregular structure");
			}
		} else {
			overall_sub_array_dimension_count = sub_array_dimension_count;
		}
	}
	if(overall_sub_array_dimension_count + 1 > MAX_DIMENSION) {
		qb_abort("array initializer has too many dimensions");
	}
	return overall_sub_array_dimension_count + 1;
}

static void qb_get_array_initializer_dimensions(qb_compiler_context *cxt, qb_array_initializer *initializer, qb_primitive_type element_type, uint32_t *dimensions, uint32_t dimension_count) {
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
			} else if(element->type == QB_OPERAND_ADDRESS && !SCALAR(element->address)) {
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

static void qb_copy_elements_from_array_initializer(qb_compiler_context *cxt, qb_array_initializer *initializer, qb_address *address) {
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
		item_address->mode = QB_ADDRESS_MODE_SCA;
		item_element_count = 1;
	}
	item_address->source_address = address;
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
			if(CONSTANT(element->address)) {
				if(item_address->dimension_count > 0) {
					uint32_t initializer_element_count = ARRAY_SIZE(element->address);
					qb_copy_elements(element->address->type, ARRAY(I08, element->address), initializer_element_count, item_address->type, ARRAY(I08, item_address), item_element_count);
				} else {
					qb_copy_element(element->address->type, ARRAY(I08, element->address), item_address->type, ARRAY(I08, item_address));
				}
			} else {
				// need an address from qb_allocate_address() instead of this fake one sitting on the stack
				// since we're attaching it to an op
				qb_address *item_address_copy = qb_allocate_address(cxt->pool);
				*item_address_copy = *item_address;
				qb_retrieve_binary_op_result(cxt, &factory_assign, item_address_copy, element->address);
			}
		} else {
			memset(ARRAY(I08, item_address), 0, item_byte_count);
		}
		item_address->segment_offset += item_byte_count;
	}
}

static qb_address * qb_retrieve_array_from_initializer(qb_compiler_context *cxt, qb_array_initializer *initializer, qb_primitive_type element_type) {
	qb_address *address;
	uint32_t dimensions[MAX_DIMENSION];
	uint32_t dimension_count;
	
	// figure out the dimensions of the array first
	dimension_count = qb_get_array_initializer_dimension_count(cxt, initializer, element_type);
	dimensions[0] = 0;
	qb_get_array_initializer_dimensions(cxt, initializer, element_type, dimensions, dimension_count);

	if(initializer->flags & QB_ARRAY_INITIALIZER_VARIABLE_ELEMENTS) {
		// need to use temporary fixed-length array, since the elements can change 
		// not using qb_obtain_temporary_fixed_length_array() since the address cannot be reused
		address = qb_create_writable_fixed_length_multidimensional_array(cxt, element_type, dimensions, dimension_count);
		address->flags |= QB_ADDRESS_TEMPORARY;

		// allocate space for it so we can copy constants into the area
		qb_allocate_storage_space(cxt, address, TRUE);
	} else {
		// the initializer is constant
		address = qb_create_constant_fixed_length_multidimensional_array(cxt, element_type, dimensions, dimension_count);
	}

	// copy the elements
	qb_copy_elements_from_array_initializer(cxt, initializer, address);
	return address;
}

static qb_type_declaration * qb_find_variable_declaration_in_list(qb_compiler_context *cxt, qb_variable *qvar, qb_type_declaration **declarations, uint32_t declaration_count) {
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

static qb_type_declaration * qb_find_variable_declaration(qb_compiler_context *cxt, qb_variable *qvar) {
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

static void qb_initialize_dimensions(qb_compiler_context *cxt, qb_address *initializer_address, qb_address *address) {
	/* TODO: this need to be looked at
	int32_t i;
	int32_t has_undefined = 0;
	for(i = 0; (uint32_t) i < address->dimension_count; i++) {
		qb_address *dimension_address = address->dimension_addresses[i];
		if(READ_ONLY(dimension_address) && VALUE(U32, dimension_address) == 0) {
			has_undefined = 1;
			break;
		}
	}
	if(has_undefined) {
		uint32_t array_size = 1;
		if(address->dimension_count != initializer_address->dimension_count) {
			qb_abort("the number of dimensions do not match");
		}
		for(i = address->dimension_count - 1; i >= 0; i--) {
			qb_address *dimension_address = address->dimension_addresses[i];
			qb_address *array_size_address = address->array_size_addresses[i];
			// initialize the dimension if it's not defined (read-only with a value of zero)
			if(READ_ONLY(dimension_address) && VALUE(U32, dimension_address) == 0) {
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
	}
	*/
}

void qb_apply_type_declaration(qb_compiler_context *cxt, qb_variable *qvar) {
	qb_type_declaration *decl = qb_find_variable_declaration(cxt, qvar);
	if(decl) {
		if(decl->type != QB_TYPE_VOID) {
			qb_address *address;
			if(decl->dimension_count == 0) {
				address = qb_create_writable_scalar(cxt, decl->type);
			} else {
				qb_variable_dimensions dim;
				qb_set_variable_dimensions(cxt, decl->dimensions, decl->dimension_count, &dim);
				if(dim.array_size) {
					address = qb_create_writable_fixed_length_array(cxt, decl->type, dim.array_size);
				} else {
					address = qb_create_writable_variable_length_array(cxt, decl->type);
				}
				if(dim.dimension_count > 1) {
					qb_copy_dimensions(cxt, &dim, address);
				}
				if(decl->flags & QB_TYPE_DECL_EXPANDABLE) {
					qb_mark_as_expandable(cxt, address);
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
			// function returns void by default if declaration is missing
		} else {
			qb_abort("missing type declaration: %s", qvar->name);
		}
	}
}

static void qb_add_variables(qb_compiler_context *cxt) {
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
			qb_apply_type_declaration(cxt, qvar);
		} else {
			// see if it's static variable
			qb_address *static_initializer = NULL;
			zval **p_static_value, *static_value;
			if(static_variable_table && zend_hash_quick_find(static_variable_table, zvar->name, zvar->name_len + 1, zvar->hash_value, (void **) &p_static_value) == SUCCESS) {
				static_value = *p_static_value;
				qvar->flags = QB_VARIABLE_STATIC;
				qb_apply_type_declaration(cxt, qvar);

				/*
				if(qvar->address->type == QB_TYPE_S64 || qvar->address->type == QB_TYPE_U64) {
					// initializing 64-bit integer might require special handling
					qb_primitive_type desired_type = qvar->address->type;
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
				*/
				if(!static_initializer) {
					// handle it in the regular manner
					static_initializer = qb_obtain_constant_zval(cxt, *p_static_value, qvar->address->type);
				}

				if(VARIABLE_LENGTH_ARRAY(qvar->address)) {
					qb_initialize_dimensions(cxt, static_initializer, qvar->address);
				}
				//qb_do_static_init(cxt, static_initializer, qvar->address);
			} else {
				// we don't know whether it is a local or a global variable at this point
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
	qb_apply_type_declaration(cxt, qvar);
	qb_add_variable(cxt, qvar);
	cxt->return_variable = qvar;
}

qb_variable * qb_find_variable(qb_compiler_context *cxt, zend_class_entry *class, zval *name, uint32_t type_mask) {
	uint32_t i;
	ulong hash_value = Z_HASH_P(name);
	for(i = 0; i < cxt->variable_count; i++) {
		qb_variable *qvar = cxt->variables[i];
		if(qvar->hash_value == hash_value && qvar->name_length == Z_STRLEN_P(name)) {
			if(strncmp(qvar->name, Z_STRVAL_P(name), Z_STRLEN_P(name)) == 0) {
				if(qvar->zend_class == class) {
					if((qvar->flags & type_mask) || !type_mask && !(qvar->flags & (QB_VARIABLE_CLASS | QB_VARIABLE_CLASS_INSTANCE | QB_VARIABLE_CLASS_CONSTANT))) {
						return qvar;
					}
				}
			}
		}
	}
	return NULL;
}

qb_variable * qb_get_local_variable(qb_compiler_context *cxt, zval *name) {
	qb_variable *qvar = qb_find_variable(cxt, NULL, name, 0);
	if(qvar) {
		if(!(qvar->flags & QB_VARIABLE_LOCAL)) {
			qvar->flags |= QB_VARIABLE_LOCAL;
			qb_apply_type_declaration(cxt, qvar);
		}
	}
	return qvar;
}

qb_variable * qb_get_global_variable(qb_compiler_context *cxt, zval *name) {
	qb_variable *qvar = qb_find_variable(cxt, NULL, name, 0);
	if(qvar) {
		if(!(qvar->flags & QB_VARIABLE_GLOBAL)) {
			qvar->flags |= QB_VARIABLE_GLOBAL;
			qb_apply_type_declaration(cxt, qvar);
		}
	}
	return qvar;
}

qb_variable * qb_get_static_variable(qb_compiler_context *cxt, zval *name) {
	qb_variable *qvar = qb_find_variable(cxt, NULL, name, QB_VARIABLE_STATIC);
	return qvar;
}

qb_variable * qb_get_class_variable(qb_compiler_context *cxt, zend_class_entry *ce, zval *name) {
	qb_variable *qvar = qb_find_variable(cxt, ce, name, QB_VARIABLE_CLASS);
	if(!qvar) {
		qvar = qb_allocate_variable(cxt->pool);
		qvar->flags = QB_VARIABLE_CLASS;
		qvar->name = Z_STRVAL_P(name);
		qvar->name_length = Z_STRLEN_P(name);
		qvar->hash_value = Z_HASH_P(name);
		qvar->zend_class = ce;
		qb_apply_type_declaration(cxt, qvar);
		qb_add_variable(cxt, qvar);
	}
	return qvar;
}

qb_variable * qb_get_instance_variable(qb_compiler_context *cxt, zval *name) {
	qb_variable *qvar = qb_find_variable(cxt, NULL, name, QB_VARIABLE_CLASS_INSTANCE);
	if(!qvar) {
		qvar = qb_allocate_variable(cxt->pool);
		qvar->flags = QB_VARIABLE_CLASS_INSTANCE;
		qvar->name = Z_STRVAL_P(name);
		qvar->name_length = Z_STRLEN_P(name);
		qvar->hash_value = Z_HASH_P(name);
		qvar->zend_class = NULL;
		qb_apply_type_declaration(cxt, qvar);
		qb_add_variable(cxt, qvar);
	}
	return qvar;
}

qb_address * qb_obtain_local_variable(qb_compiler_context *cxt, zval *name) {
	qb_variable *qvar = qb_get_local_variable(cxt, name);
	return qvar->address;
}

qb_address * qb_obtain_global_variable(qb_compiler_context *cxt, zval *name) {
	qb_variable *qvar = qb_get_global_variable(cxt, name);
	return qvar->address;
}

qb_address * qb_obtain_static_variable(qb_compiler_context *cxt, zval *name) {
	qb_variable *qvar = qb_get_static_variable(cxt, name);
	return qvar->address;
}

qb_address * qb_obtain_class_variable(qb_compiler_context *cxt, zend_class_entry *ce, zval *name) {
	qb_variable *qvar = qb_get_class_variable(cxt, ce, name);
	return qvar->address;
}

qb_address * qb_obtain_instance_variable(qb_compiler_context *cxt, zval *name) {
	qb_variable *qvar = qb_get_instance_variable(cxt, name);
	return qvar->address;
}

qb_address * qb_obtain_class_static_constant(qb_compiler_context *cxt, zval *name, qb_primitive_type type) {
	qb_variable *qvar = NULL;
	ulong hash_value = Z_HASH_P(name);
	uint32_t i;
	for(i = cxt->argument_count; i < cxt->variable_count; i++) {
		qvar = cxt->variables[i];
		if(qvar->flags & QB_VARIABLE_CLASS_CONSTANT) {
			if(qvar->hash_value == hash_value && qvar->name_length == Z_STRLEN_P(name)) {
				if(strncmp(qvar->name, Z_STRVAL_P(name), Z_STRLEN_P(name)) == 0) {
					if(qvar->zend_class == NULL) {
						// the type has to match as well
						if(qvar->address->type == type) {
							return qvar->address;
						}
					}
				}
			}
		}
	}

	qvar = qb_allocate_variable(cxt->pool);
	qvar->flags = QB_VARIABLE_CLASS_CONSTANT;
	qvar->name = Z_STRVAL_P(name);
	qvar->name_length = Z_STRLEN_P(name);
	qvar->hash_value = Z_HASH_P(name);
	qvar->zend_class = NULL;
	qvar->address = qb_create_writable_scalar(cxt, type);
	qb_mark_as_shared(cxt, qvar->address);
	qb_add_variable(cxt, qvar);
	return qvar->address;
}

static void qb_initialize_function_prototype(qb_compiler_context *cxt) {
	cxt->function_prototype.argument_count = cxt->argument_count;
	cxt->function_prototype.required_argument_count = cxt->required_argument_count;
	cxt->function_prototype.variables = cxt->variables;
	cxt->function_prototype.return_variable = cxt->return_variable;
	cxt->function_prototype.zend_function = cxt->zend_function;
	cxt->function_prototype.name = cxt->zend_function->common.function_name;
	cxt->function_prototype.filename = cxt->zend_function->op_array.filename;
	cxt->function_prototype.local_storage = cxt->storage;
}

static uint32_t qb_import_external_symbol(qb_compiler_context *cxt, uint32_t type, const char *name, uint32_t name_len, void *pointer) {
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

static qb_primitive_type qb_get_array_initializer_type(qb_compiler_context *cxt, qb_array_initializer *initializer, uint32_t flags) {
	uint32_t i;
	qb_primitive_type highest_rank_type = 0;
	for(i = 0; i < initializer->element_count; i++) {
		qb_operand *element = &initializer->elements[i];
		qb_primitive_type element_type;
		if(element->type == QB_OPERAND_ARRAY_INITIALIZER) {
			element_type = qb_get_array_initializer_type(cxt, element->array_initializer, flags);
		} else if(element->type == QB_OPERAND_ZVAL && !(flags & QB_RETRIEVE_DEFINITE_TYPE_ONLY)) {
			element_type = qb_get_zval_type(cxt, element->constant, 0);
		} else if(element->type == QB_OPERAND_ADDRESS) {
			element_type = element->address->type;
		} else {
			continue;
		}
		if(highest_rank_type == QB_TYPE_UNKNOWN || element_type > highest_rank_type) {
			if(element_type != QB_TYPE_UNKNOWN) {
				highest_rank_type = element_type;
			}
		}
	}
	if(highest_rank_type == QB_TYPE_UNKNOWN && !(flags & QB_RETRIEVE_DEFINITE_TYPE_ONLY)) {
		// the array is empty--doesn't really matter what it is
		highest_rank_type = QB_TYPE_I32;
	}
	return highest_rank_type;
}

qb_primitive_type qb_get_operand_type(qb_compiler_context *cxt, qb_operand *operand, uint32_t flags) {
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
			} else if(operand->address->type >= QB_TYPE_S64) {
				return QB_TYPE_F64;
			} else {
				return QB_TYPE_F32;
			}
		} else if(flags & QB_COERCE_TO_SIGNED) {
			return operand->address->type & ~QB_TYPE_UNSIGNED;
		} else if(flags & QB_COERCE_TO_UNSIGNED) {
			return operand->address->type | QB_TYPE_UNSIGNED;
		}
		return operand->address->type;
	} else if(operand->type == QB_OPERAND_ZVAL) {
		if(!(flags & QB_RETRIEVE_DEFINITE_TYPE_ONLY)) {
			return qb_get_zval_type(cxt, operand->constant, flags);
		}
	} else if(operand->type == QB_OPERAND_ARRAY_INITIALIZER) {
		return qb_get_array_initializer_type(cxt, operand->array_initializer, flags);
	} else if(operand->type == QB_OPERAND_RESULT_PROTOTYPE) {
		if(operand->result_prototype->final_type != QB_TYPE_UNKNOWN) {
			return operand->result_prototype->final_type;
		} 
		if(!(flags & QB_RETRIEVE_DEFINITE_TYPE_ONLY)) {
			if(operand->result_prototype->preliminary_type != QB_TYPE_UNKNOWN) {
				return operand->result_prototype->preliminary_type;
			}
		}
	}
	return QB_TYPE_ANY;
}

qb_primitive_type qb_get_property_type(qb_compiler_context *cxt, qb_operand *container, qb_operand *name) {
	if(container->type == QB_OPERAND_NONE) {
		qb_variable *qvar = qb_get_instance_variable(cxt, name->constant);
		if(qvar) {
			return qvar->address->type;
		}
	} else if(container->type == QB_OPERAND_ADDRESS) {
		return container->address->type;
	}
	return QB_TYPE_UNKNOWN;
}

qb_primitive_type qb_get_result_destination_type(qb_compiler_context *cxt, qb_result_destination *destination) {
	if(destination) {
		switch(destination->type) {
			case QB_RESULT_DESTINATION_VARIABLE: {
				return destination->variable.address->type;
			}
			case QB_RESULT_DESTINATION_ELEMENT: {
				return destination->element.container.address->type;
			}
			case QB_RESULT_DESTINATION_PROPERTY: {
				return qb_get_property_type(cxt, &destination->property.container, &destination->property.name);
			}
			case QB_RESULT_DESTINATION_RETURN: {
				if(cxt->return_variable->address) {
					return cxt->return_variable->address->type;
				}
			}
			case QB_RESULT_DESTINATION_ARGUMENT: {
				// TODO:
			}
			
		}
	}
	return QB_TYPE_UNKNOWN;
}

qb_primitive_type qb_get_highest_rank_type(qb_compiler_context *cxt, qb_operand *operands, uint32_t count, uint32_t flags) {
	qb_primitive_type type1, type2;
	qb_primitive_type definite_type;
	uint32_t i;

	// first, consider only operands whose type we're sure about 
	type1 = QB_TYPE_UNKNOWN;
	for(i = 0; i < count; i++) {
		qb_operand *operand = &operands[i];
		type2 = qb_get_operand_type(cxt, operand, flags | QB_RETRIEVE_DEFINITE_TYPE_ONLY);
		if(type2 != QB_TYPE_ANY) {
			if(type1 == QB_TYPE_UNKNOWN || type1 < type2) {
				type1 = type2;
			}
		}
	}
	definite_type = type1;

	// then, loosen it up a bit
	type1 = QB_TYPE_UNKNOWN;
	for(i = 0; i < count; i++) {
		qb_operand *operand = &operands[i];
		type2 = qb_get_operand_type(cxt, operand, flags);
		if(type2 != QB_TYPE_ANY) {
			if(type1 == QB_TYPE_UNKNOWN || type1 < type2) {
				type1 = type2;
			}
		}
	}

	if(definite_type < type1) {
		// don't perform F32-to-F64 promotion if it's trigger by a constant
		if(definite_type == QB_TYPE_F32) {
			type1 = definite_type;
		}
	}

	if(type1 == QB_TYPE_UNKNOWN) {
		if(flags & QB_COERCE_TO_INTEGER_TO_DOUBLE) {
			type1 = QB_TYPE_F64;
		} else if(flags & (QB_COERCE_TO_INTEGER | QB_COERCE_TO_BOOLEAN)) {
			if(flags & QB_COERCE_TO_UNSIGNED) {
				type1 = QB_TYPE_S32;
			} else {
				type1 = QB_TYPE_U32;
			}
		} else {
			// default to S32
			type1 = QB_TYPE_S32;
		}
	}
	return type1;
}

qb_address * qb_retrieve_unary_op_result(qb_compiler_context *cxt, void *factory, qb_address *address) {
	qb_operand operand, result;
	operand.type = QB_OPERAND_ADDRESS;
	operand.address = address;
	result.type = QB_OPERAND_EMPTY;
	qb_produce_op(cxt, factory, &operand, 1, &result, NULL, 0, NULL);
	return result.address;
}

qb_address * qb_retrieve_binary_op_result(qb_compiler_context *cxt, void *factory, qb_address *address1, qb_address *address2) {
	qb_operand operands[2], result;
	operands[0].type = QB_OPERAND_ADDRESS;
	operands[0].address = address1;
	operands[1].type = QB_OPERAND_ADDRESS;
	operands[1].address = address2;
	result.type = QB_OPERAND_EMPTY;
	qb_produce_op(cxt, factory, operands, 2, &result, NULL, 0, NULL);
	return result.address;
}

qb_address * qb_retrieve_temporary_copy(qb_compiler_context *cxt, qb_address *address, qb_primitive_type type) {
	static qb_cast_op_factory *cast_factories[] = {
		&factory_cast_S08,	&factory_cast_U08,
		&factory_cast_S16,	&factory_cast_U16,
		&factory_cast_S32,	&factory_cast_U32,
		&factory_cast_S64,	&factory_cast_U64,
		&factory_cast_F32,	
		&factory_cast_F64,
	};
	qb_cast_op_factory *cf;
	if(type == QB_TYPE_ANY) {
		type = address->type;
	}
	cf = cast_factories[type];
	return qb_retrieve_unary_op_result(cxt, cf, address);
}

void qb_perform_type_coercion(qb_compiler_context *cxt, qb_operand *operand, qb_primitive_type desired_type, uint32_t coercion_flags) {
	if(cxt->stage == QB_STAGE_RESULT_TYPE_RESOLUTION) {
		if(operand->type == QB_OPERAND_RESULT_PROTOTYPE) {
			// no type information to record if we don't have any
			if(desired_type != QB_TYPE_ANY) {
				// change the type only if there's flexibility 
				if(operand->result_prototype->final_type == QB_TYPE_ANY || operand->result_prototype->final_type == QB_TYPE_UNKNOWN) {
					if(desired_type >= QB_TYPE_F32 && operand->result_prototype->coercion_flags & QB_COERCE_TO_INTEGER) {
						// operand cannot be floating point (e.g. result of bitwise operator) 
						// use the largest integer type instead
						desired_type = QB_TYPE_I64;
					} 
					if(desired_type > operand->result_prototype->preliminary_type || operand->result_prototype->preliminary_type == QB_TYPE_ANY || operand->result_prototype->preliminary_type == QB_TYPE_UNKNOWN) {
						operand->result_prototype->preliminary_type = desired_type;
						if(!(operand->result_prototype->coercion_flags & QB_COERCE_TO_LVALUE_TYPE)) {
							operand->result_prototype->final_type = desired_type;
						}
					}
				}
			}
		}
	} else {
		if(operand->type == QB_OPERAND_ADDRESS) {
			if(desired_type != QB_TYPE_VOID) {
				qb_address *new_address = NULL;
				if(operand->address->type != desired_type && desired_type != QB_TYPE_ANY) {
					if(STORAGE_TYPE_MATCH(operand->address->type, desired_type)) {
						if(operand->address->flags & QB_ADDRESS_CAST) {
							// use the original address
							new_address = operand->address->source_address;
						} else {
							// storage type is the same--just create a new address
							new_address = qb_obtain_cast_alias(cxt, operand->address, desired_type);
						}
					} else {
						// the bit pattern is different--need to do a copy
						new_address = qb_retrieve_temporary_copy(cxt, operand->address, desired_type);
					}
					operand->address = new_address;
				}
			} else {
				operand->address = NULL;
				operand->type = QB_OPERAND_NONE;
			}
		} else if(operand->type == QB_OPERAND_ZVAL) {
			if(desired_type != QB_TYPE_VOID) {
				if(desired_type == QB_TYPE_ANY) {
					desired_type = qb_get_zval_type(cxt, operand->constant, coercion_flags);
				}
				operand->address = qb_obtain_constant_zval(cxt, operand->constant, desired_type);
				operand->type = QB_OPERAND_ADDRESS;
			} else {
				operand->address = NULL;
				operand->type = QB_OPERAND_NONE;
			}
		} else if(operand->type == QB_OPERAND_ARRAY_INITIALIZER) {
			if(desired_type == QB_TYPE_ANY) {
				desired_type = qb_get_array_initializer_type(cxt, operand->array_initializer, coercion_flags);
			}
			operand->address = qb_retrieve_array_from_initializer(cxt, operand->array_initializer, desired_type);
			operand->type = QB_OPERAND_ADDRESS;
		}
	}
}

void qb_perform_boolean_coercion(qb_compiler_context *cxt, qb_operand *operand) {
	if(cxt->stage == QB_STAGE_RESULT_TYPE_RESOLUTION) {
		if(operand->type == QB_OPERAND_RESULT_PROTOTYPE) {
			operand->result_prototype->preliminary_type = operand->result_prototype->final_type = QB_TYPE_I32;
			operand->result_prototype->address_flags |= QB_ADDRESS_BOOLEAN;
		}
	} else if(cxt->stage == QB_STAGE_OPCODE_TRANSLATION) {
		if(operand->type == QB_OPERAND_ADDRESS) {
			if(!(operand->address->flags & QB_ADDRESS_BOOLEAN)) {
				if(CONSTANT(operand->address)) {
					int32_t is_true;
					if(SCALAR(operand->address)) {
						switch(operand->address->type) {
							case QB_TYPE_S08:
							case QB_TYPE_U08: is_true = VALUE(I08, operand->address) != 0; break;
							case QB_TYPE_S16:
							case QB_TYPE_U16: is_true = VALUE(I16, operand->address) != 0; break;
							case QB_TYPE_S32:
							case QB_TYPE_U32: is_true = VALUE(I32, operand->address) != 0; break;
							case QB_TYPE_S64:
							case QB_TYPE_U64: is_true = VALUE(I64, operand->address) != 0; break;
							case QB_TYPE_F32: is_true = VALUE(F32, operand->address) != 0.0f; break;
							case QB_TYPE_F64: is_true = VALUE(F64, operand->address) != 0.0; break;
							default: break;
						}
					} else {
						is_true = TRUE;
					}
					operand->address = qb_obtain_constant_boolean(cxt, is_true);
				} else {
					operand->address = qb_retrieve_unary_op_result(cxt, &factory_boolean_cast, operand->address);
				}
			}
		} else if(operand->type == QB_OPERAND_ZVAL) {
			int32_t is_true = zend_is_true(operand->constant);
			operand->type = QB_OPERAND_ADDRESS;
			operand->address = qb_obtain_constant_S32(cxt, is_true);
		}
	}
}

qb_address * qb_obtain_scalar_value(qb_compiler_context *cxt, qb_address *address) {
	if(address->mode != QB_ADDRESS_MODE_SCA) {
		return qb_retrieve_temporary_copy(cxt, address, QB_TYPE_ANY);
	}
	return address;
}

qb_address * qb_obtain_array_slice(qb_compiler_context *cxt, qb_address *container_address, qb_address *offset_address, qb_address *length_address) {
	qb_address *slice_address, *element_size_address;

	if(SCALAR(container_address)) {
		return NULL;
	}
	if(!SCALAR(offset_address)) {
		return NULL;
	}
	if(length_address && !SCALAR(length_address)) {
		return NULL;
	}

	if(!length_address) {
		// the length is the difference of the offset and the array size
		qb_address *size_address = container_address->dimension_addresses[0];
		length_address = qb_obtain_on_demand_difference(cxt, size_address, offset_address);
	}
	if(container_address->dimension_count > 1) {
		element_size_address = container_address->dimension_addresses[1];
	} else {
		element_size_address = NULL;
	}
	slice_address = qb_allocate_address(cxt->pool);
	slice_address->type = container_address->type;
	slice_address->flags = QB_ADDRESS_NON_LOCAL | (container_address->flags & (QB_ADDRESS_STRING | QB_ADDRESS_TEMPORARY));
	slice_address->mode = QB_ADDRESS_MODE_ARR;
	slice_address->segment_selector = container_address->segment_selector;

	if(element_size_address) {
		// need to multiply the offset by the element size
		offset_address = qb_obtain_on_demand_product(cxt, element_size_address, offset_address);
	}
	if(container_address->array_index_address) {
		// need to add the offset of the array
		offset_address = qb_obtain_on_demand_sum(cxt, container_address->array_index_address, offset_address);
	}
	slice_address->array_index_address = offset_address;
	slice_address->dimension_count = container_address->dimension_count;
	slice_address->source_address = container_address;
	if(slice_address->dimension_count > 1) {
		qb_address *sub_array_size_address = container_address->array_size_addresses[1];
		qb_address *array_size_address = qb_obtain_on_demand_product(cxt, length_address, sub_array_size_address);
		uint32_t i;

		// copy the dimensions
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
	return slice_address;
}

qb_address * qb_obtain_bound_checked_array_index_multiply(qb_compiler_context *cxt, qb_address *container_address, qb_address *index_address, uint32_t bound_check_flags) {
	qb_address *index_limit_address = container_address->dimension_addresses[0];
	qb_address *sub_array_size_address = container_address->array_size_addresses[1];

	// if the index and array dimensions are constant, perform the calculation now if it's within bounds
	if(CONSTANT(index_address) && CONSTANT(sub_array_size_address) && CONSTANT(index_limit_address)) {
		if(VALUE(U32, index_address) < VALUE(U32, index_limit_address)) {
			uint32_t product = VALUE(U32, index_address) * VALUE(U32, sub_array_size_address);
			return qb_obtain_constant_U32(cxt, product);
		}
	} 
	if(CONSTANT(index_address) && VALUE(U32, index_address) == 0) {
		// 0 * sub_array_size = 0
		return qb_obtain_constant_U32(cxt, 0);
	} else {
		if(bound_check_flags & QB_ARRAY_BOUND_CHECK_ISSET) {
			qb_operand operands[3] = { { QB_OPERAND_ADDRESS, container_address }, { QB_OPERAND_ADDRESS, index_address }, { QB_OPERAND_EMPTY, NULL } };
			return qb_obtain_on_demand_value(cxt, &factory_bound_check_predicate_multiply, operands, 3);
		} else {
			qb_operand operands[2] = { { QB_OPERAND_ADDRESS, container_address }, { QB_OPERAND_ADDRESS, index_address } };
			if((bound_check_flags & QB_ARRAY_BOUND_CHECK_WRITE) && EXPANDABLE_ARRAY(container_address)) {
				return qb_obtain_on_demand_value(cxt, &factory_bound_expand_multiply, operands, 2);
			} else {
				return qb_obtain_on_demand_value(cxt, &factory_bound_check_multiply, operands, 2);
			}
		}
	}
}

qb_address * qb_obtain_array_element(qb_compiler_context *cxt, qb_address *container_address, qb_address *index_address, uint32_t bound_check_flags) {
	qb_address *result_address;
	qb_address *offset_address = NULL;
	qb_address *index_limit_address;
	qb_primitive_type element_type = container_address->type;
	uint32_t i;

	index_limit_address = container_address->dimension_addresses[0];
	offset_address = container_address->array_index_address;
	if(container_address->dimension_count > 1) {
		// multiple by the size of the sub-array 
		index_address = qb_obtain_bound_checked_array_index_multiply(cxt, container_address, index_address, bound_check_flags);
		index_address = qb_obtain_on_demand_sum(cxt, index_address, offset_address);
	} else {
		if(READ_ONLY(index_limit_address)) {
			index_address = qb_obtain_bound_checked_sum(cxt, index_address, offset_address, index_limit_address);
		}
	}

	// see if we have created the address earlier
	for(i = 0; i < cxt->address_alias_count; i++) {
		qb_address *alias = cxt->address_aliases[i];
		if(alias->source_address == container_address) {
			if(alias->array_index_address == index_address && alias->dimension_count == container_address->dimension_count - 1) {
				return alias;
			}
		}
	}

	result_address = qb_create_address_alias(cxt, container_address);
	result_address->array_index_address = index_address;
	result_address->dimension_count--;
	if(result_address->dimension_count > 0) {
		result_address->mode = QB_ADDRESS_MODE_ARR;
		result_address->dimension_addresses++;
		result_address->array_size_addresses++;
		result_address->array_size_address = result_address->array_size_addresses[0];
		if(result_address->index_alias_schemes) {
			result_address->index_alias_schemes++;
		}
	} else {
		if(CONSTANT(index_address)) {
			// SCA address mode can be used when the index is constant
			result_address->mode = QB_ADDRESS_MODE_SCA;
		} else {
			// otherwise ELE is needed
			result_address->mode = QB_ADDRESS_MODE_ELE;
		}
		result_address->dimension_addresses = NULL;
		result_address->array_size_addresses = NULL;
		result_address->array_size_address = NULL;
		result_address->index_alias_schemes = NULL;
	}

	if(CONSTANT(index_address)) {
		// TODO: double check this
		uint32_t index = VALUE(U32, index_address);
		if(FIXED_LENGTH_ARRAY(container_address)) {
			if(index < ARRAY_SIZE(container_address)) {
				result_address->flags |= QB_ADDRESS_ALWAYS_IN_BOUND;
			}
		}
	}
	return result_address;
}

qb_address * qb_retrieve_array_dimensions(qb_compiler_context *cxt, qb_address *address) {
	qb_address *dimensions_address = qb_obtain_temporary_fixed_length_array(cxt, QB_TYPE_U32, address->dimension_count);
	uint32_t i;
	for(i = 0; i < address->dimension_count; i++) {
		qb_address *index_address = qb_obtain_constant_U32(cxt, i);
		qb_address *src_dimension_address = address->dimension_addresses[i];
		qb_address *dst_dimension_address = qb_obtain_array_element(cxt, dimensions_address, index_address, FALSE);
		qb_retrieve_binary_op_result(cxt, &factory_assign, dst_dimension_address, src_dimension_address);
	}
	return dimensions_address;
}

int32_t qb_find_index_alias(qb_compiler_context *cxt, qb_index_alias_scheme *scheme, zval *name) {
	const char *alias = Z_STRVAL_P(name);
	uint32_t i;
	for(i = 0; i < scheme->dimension; i++) {
		if(strcmp(scheme->aliases[i], alias) == 0) {
			return i;
		}
	}
	return -1;
}

qb_address * qb_obtain_named_element(qb_compiler_context *cxt, qb_address *container_address, zval *name, uint32_t bound_check_flags) {
	 if(!SCALAR(container_address)) {
		if(container_address->index_alias_schemes && container_address->index_alias_schemes[0]) {
			int32_t index = qb_find_index_alias(cxt, container_address->index_alias_schemes[0], name);
			if(index != -1) {
				qb_address *index_address = qb_obtain_constant_U32(cxt, index);
				qb_address *value_address = qb_obtain_array_element(cxt, container_address, index_address, bound_check_flags);
				return value_address;
			}
		}
	}
	return NULL;
}

static void qb_finalize_result_prototype(qb_compiler_context *cxt, qb_result_prototype *result_prototype) {
	if(result_prototype->final_type == QB_TYPE_UNKNOWN) {
		qb_primitive_type expr_type = result_prototype->preliminary_type;
		if(result_prototype->coercion_flags & QB_COERCE_TO_LVALUE_TYPE) {
			// the type is dependent on the lvalue; look at where the result will end up
			// unless the type is already the highest ranked 
			if(expr_type != QB_TYPE_F64) {
				qb_primitive_type parent_type;
				if(result_prototype->parent) {
					// make sure the parent is finalized as well
					qb_finalize_result_prototype(cxt, result_prototype->parent);
					parent_type = result_prototype->parent->final_type;

					// see what the result is--promote the expression to it if it's higher
					if(parent_type != QB_TYPE_ANY) {
						if(parent_type > expr_type || expr_type == QB_TYPE_ANY) {
							if(!(parent_type >= QB_TYPE_F32 && (result_prototype->coercion_flags & QB_COERCE_TO_INTEGER))) { 
								expr_type = parent_type;
							}
						}
					}
				}
			}
		}
		if(expr_type == QB_TYPE_ANY || expr_type == QB_TYPE_UNKNOWN) {
			// still don't know what it ought to be
			if(result_prototype->coercion_flags & QB_COERCE_TO_INTEGER) {
				expr_type = QB_TYPE_I32;
			} else if(result_prototype->coercion_flags & QB_COERCE_TO_FLOATING_POINT) {
				expr_type = QB_TYPE_F64;
			}
		}
		result_prototype->final_type = expr_type;
	}
}

void qb_create_on_demand_op(qb_compiler_context *cxt, qb_operand *operand);

void qb_create_op(qb_compiler_context *cxt, void *factory, qb_operand *operands, uint32_t operand_count, qb_operand *result, uint32_t *jump_target_indices, uint32_t jump_target_count, int32_t result_used) {
	qb_op_factory *f = factory;
	qb_opcode opcode = QB_NOP;
	qb_op *qop;
	uint32_t op_flags, op_index;
	uint32_t i;

	// get the opcode for the operands 
	// at this point, the operands should have the correct type
	if(f->select_opcode) {
		opcode = f->select_opcode(cxt, f, operands, operand_count, result);
	}
	op_flags = qb_get_op_flags(cxt, opcode);

	// make it a NOP if the result isn't used and the op has no side effect
	if(!result_used && !(op_flags & QB_OP_JUMP) && !(f->result_flags & QB_RESULT_HAS_SIDE_EFFECT)) {
		opcode = QB_NOP;
	}

	// create the op
	op_index = cxt->op_count;
	qop = qb_allocate_op(cxt->pool);
	qop->flags = op_flags;
	qop->opcode = opcode;
	qop->line_number = cxt->line_number;

	// figure out how many operands there will be 
	if(!(op_flags & QB_OP_VARIABLE_LENGTH)) {
		qop->operand_count = qb_get_operand_count(cxt, opcode);
	} else {
		qop->operand_count = f->get_operand_count(cxt, f, operands, operand_count);
	}

	// move the operands into the op
	qop->operands = qb_allocate_operands(cxt->pool, qop->operand_count);
	if(f->transfer_operands) {
		f->transfer_operands(cxt, f, operands, operand_count, result, qop->operands, qop->operand_count);
	}

	// add the ops for calculating on-demand values 
	for(i = 0; i < qop->operand_count; i++) {
		qb_create_on_demand_op(cxt, &qop->operands[i]);
	}

	/*
	for(i = 0; i < operand_count; i++) {
		qb_address *address = operands[i].address;
		if(address->mode == QB_ADDRESS_MODE_ELE) {
			// see if the opcode has an ELE version; most do, but a few do not
			uint32_t op_flags = qb_get_op_flags(cxt, opcode);
			if(!(op_flags & QB_OP_VERSION_AVAILABLE_ELE)) {
				// copy the element to an temporary variable
				// TODO
			}
		}
	}
	*/

	// add the op
	qb_add_op(cxt, qop);

	// copy the jump target indices and mark instructions they refer to
	qop->jump_target_count = jump_target_count;
	if(jump_target_count > 0) {
		qop->jump_target_indices = qb_allocate_indices(cxt->pool, jump_target_count);
		for(i = 0; i < jump_target_count; i++) {
			qop->jump_target_indices[i] = jump_target_indices[i];
			qb_mark_jump_target(cxt, op_index, jump_target_indices[i]);
		}
	}

	if(result && result->type == QB_OPERAND_ADDRESS && opcode != QB_NOP) {
		if(CONSTANT(result->address) && !(f->result_flags & QB_RESULT_HAS_SIDE_EFFECT) && FALSE) {
			// TODO: fix qb_execute_op()
			// evalulate the expression at compile-time if it's constant and side-effect-free
			qb_execute_op(cxt, qop);

			// make it a nop
			qop->opcode = QB_NOP;
			qop->operand_count = 0;
			qop->operands = NULL;
		} else {
			// mark result address as writable
			qb_mark_as_writable(cxt, result->address);
		}
	}
}

static void qb_update_on_demand_result(qb_compiler_context *cxt, qb_address *address) {
	qb_expression *expr = address->expression;
	qb_produce_op(cxt, expr->op_factory, expr->operands, expr->operand_count, expr->result, NULL, 0, NULL);
}

void qb_create_on_demand_op(qb_compiler_context *cxt, qb_operand *operand) {
	if(operand->type == QB_OPERAND_ADDRESS) {
		qb_address *address = operand->address;
		if(ON_DEMAND(address)) {
			qb_update_on_demand_result(cxt, address);
		} else {
			if(address->array_index_address && ON_DEMAND(address->array_index_address)) {
				qb_update_on_demand_result(cxt, address->array_index_address);
			}
			if(address->array_size_address && ON_DEMAND(address->array_size_address)) {
				qb_update_on_demand_result(cxt, address->array_size_address);
			}
		}
	} 
}

void qb_produce_op(qb_compiler_context *cxt, void *factory, qb_operand *operands, uint32_t operand_count, qb_operand *result, uint32_t *jump_target_indices, uint32_t jump_target_count, qb_result_prototype *result_prototype) {
	qb_op_factory *f = factory;
	qb_primitive_type expr_type = QB_TYPE_VOID;
	int32_t result_used = (result->type != QB_OPERAND_NONE);
	uint32_t i;

	if(cxt->stage == QB_STAGE_RESULT_TYPE_RESOLUTION) {
		// determine the expression type
		if(f->resolve_type) {
			expr_type = f->resolve_type(cxt, f, operands, operand_count);

			// indicate in the prototype that the expression has this type
			result_prototype->preliminary_type = expr_type;
			if(!(f->coercion_flags & QB_COERCE_TO_LVALUE_TYPE) || result->type == QB_OPERAND_NONE) {
				// as the result doesn't depend on the context (or there is no context at all)
				// we're certain about the type 
				result_prototype->final_type = expr_type;
			}
			result_prototype->coercion_flags = f->coercion_flags;
			result_prototype->address_flags = f->address_flags;
		}
	} else if(cxt->stage == QB_STAGE_OPCODE_TRANSLATION) {
		// use the result from the previous stage if it's available 
		if(result_prototype) {
			// finalize it first
			qb_finalize_result_prototype(cxt, result_prototype);
			expr_type = result_prototype->final_type;
		} else {
			if(f->resolve_type) {
				expr_type = f->resolve_type(cxt, f, operands, operand_count);
			}
		}
	}

	// perform type coercion on operand
	if(f->coerce_operands) {
		// note that the handler might not necessarily convert all the operands to expr_type
		f->coerce_operands(cxt, f, expr_type, operands, operand_count);
	}

	if(cxt->stage == QB_STAGE_OPCODE_TRANSLATION) {
		// do validation
		if(f->validate_operands) {
			f->validate_operands(cxt, f, operands, operand_count);
		}
	}

	// then, assign result to result object
	if(cxt->stage == QB_STAGE_RESULT_TYPE_RESOLUTION) {
		if(f->link_results) {
			f->link_results(cxt, f, operands, operand_count, result_prototype);
		}
		if(f->set_preliminary_result) {
			f->set_preliminary_result(cxt, f, expr_type, operands, operand_count, result, result_prototype);
		}
	} else if(cxt->stage == QB_STAGE_OPCODE_TRANSLATION) {
		if(f->set_final_result) {
			if(expr_type == QB_TYPE_ANY) {
				// couldn't figure it out the first time around
				// after type coercion, we should know what the expresion is
				if(f->resolve_type) {
					expr_type = f->resolve_type(cxt, f, operands, operand_count);
				}
			}
			f->set_final_result(cxt, f, expr_type, operands, operand_count, result, result_prototype);

			// TODO: handle this in a cleaner manner
			if(result->type == QB_OPERAND_ADDRESS) {
				if(f->address_flags & QB_ADDRESS_BOOLEAN) {
					result->address = qb_obtain_boolean_alias(cxt, result->address);
				} else if(f->address_flags & QB_ADDRESS_STRING) {
					result->address = qb_obtain_string_alias(cxt, result->address);
				}
			}
		}
	}

	if(cxt->stage == QB_STAGE_OPCODE_TRANSLATION) {
		// create the op
		qb_create_op(cxt, factory, operands, operand_count, result, jump_target_indices, jump_target_count, result_used);

		// unlock the operands after the op is created
		for(i = 0; i < operand_count; i++) {
			qb_operand *operand = &operands[i];
			if(operand->type == QB_OPERAND_ADDRESS) {
				qb_unlock_address(cxt, operand->address);
			}
		}
	}
}

static void qb_validate_op(qb_compiler_context *cxt, qb_op *qop, uint32_t qop_index) {
	if(qop->opcode > QB_OPCODE_COUNT) {
		qb_abort("invalid opcode: %d", qop->opcode);
	} else {
	}
}

static void qb_resolve_jump_targets(qb_compiler_context *cxt) {
	uint32_t i, j;

	for(i = 0; i < cxt->op_count; i++) {
		qb_op *qop = cxt->ops[i];

		for(j = 0; j < qop->jump_target_count; j++) {
			uint32_t absolute_index = qb_get_jump_target_absolute_index(cxt, i, qop->jump_target_indices[j]);

			// if skip to the next op if it's pointing to a NOP
			while(cxt->ops[absolute_index]->opcode == QB_NOP) {
				absolute_index++;
			}
			qop->jump_target_indices[j] = absolute_index;
		}
	}
}

static qb_address * qb_promote_address_mode(qb_compiler_context *cxt, qb_address_mode mode, qb_address *original_address) {
	qb_address *address = qb_allocate_address(cxt->pool);
	*address = *original_address;
	if(mode == QB_ADDRESS_MODE_ELE) {
		// SCA -> ELE
		if(!address->array_index_address) {
			// put in zero as the index
			address->array_index_address = cxt->zero_address;
		}
	} else if(mode == QB_ADDRESS_MODE_ARR) {
		// SCA -> ARR or ELE -> ARR
		if(!address->array_index_address) {
			address->array_index_address = cxt->zero_address;
		}
		if(!address->array_size_address) {
			// give it a size of one
			address->array_size_address = cxt->one_address;
		}
	}
	address->mode = mode;
	return address;
}

static qb_address * qb_get_base_address(qb_compiler_context *cxt, qb_address *address) {
	qb_address *b = address;
	while(b->source_address) {
		b = b->source_address;
	}
	return b;
}

static void qb_resolve_derived_address_storage(qb_compiler_context *cxt, qb_address *address) {
	if(address->segment_selector == QB_SELECTOR_INVALID || address->segment_offset == QB_OFFSET_INVALID) {
		// use the base address's selector and offset
		qb_address *base_address = qb_get_base_address(cxt, address);
		address->segment_selector = base_address->segment_selector;
		address->segment_offset = base_address->segment_offset;

		if(address->mode == QB_ADDRESS_MODE_SCA && address->array_index_address != NULL) {
			// add in the index (array_index_address will always be constant)
			uint32_t index = VALUE(U32, address->array_index_address);
			address->segment_offset += BYTE_COUNT(index, address->type);
		} else {
			// resolve array_index_address and array_size_address
			if(address->array_index_address) {
				qb_resolve_derived_address_storage(cxt, address->array_index_address);
			}
			if(address->array_size_address) {
				qb_resolve_derived_address_storage(cxt, address->array_size_address);
			}
		}
	}
}

static void qb_resolve_address_modes(qb_compiler_context *cxt) {
	uint32_t i, j;
	for(i = 0; i < cxt->op_count; i++) {
		qb_op *qop = cxt->ops[i];

		if(qop->opcode != QB_NOP) {
			int32_t operands_are_valid = TRUE;
			uint32_t op_flags = qb_get_op_flags(cxt, qop->opcode);
			if(op_flags & (QB_OP_VERSION_AVAILABLE_ELE | QB_OP_VERSION_AVAILABLE_MIO)) {
				qb_address_mode required_address_mode = QB_ADDRESS_MODE_SCA;
				for(j = 0; j < qop->operand_count; j++) {
					qb_operand *operand = &qop->operands[j];
					if(operand->type == QB_OPERAND_ADDRESS) {
						qb_address_mode operand_address_mode = qb_get_operand_address_mode(cxt, qop->opcode, j);
						if(operand->address->mode > operand_address_mode) {
							// need use to a higher address mode
							required_address_mode = operand->address->mode;
						}
					}
				}

				if(required_address_mode != QB_ADDRESS_MODE_SCA) {
					if(required_address_mode == QB_ADDRESS_MODE_ELE) {
						if(op_flags & QB_OP_VERSION_AVAILABLE_ELE) {
							// opcode is one away
							qop->opcode += 1;
						}
					} else if(required_address_mode == QB_ADDRESS_MODE_ARR) {
						if(op_flags & QB_OP_VERSION_AVAILABLE_MIO) {
							if(op_flags & QB_OP_VERSION_AVAILABLE_ELE) {
								// opcode is two away
								qop->opcode += 2;
							} else {
								// opcode is just one away
								qop->opcode += 1;
							}
						}
					}
					// get new set of flags
					op_flags = qb_get_op_flags(cxt, qop->opcode);
					qop->flags = op_flags | (qop->flags & QB_OP_COMPILE_TIME_FLAGS);
				}
			}

			// set the addresses to the correct mode
			for(j = 0; j < qop->operand_count; j++) {
				qb_operand *operand = &qop->operands[j];
				if(operand->type == QB_OPERAND_ADDRESS) {
					qb_address_mode operand_address_mode = qb_get_operand_address_mode(cxt, qop->opcode, j);
					qb_address *address = operand->address;
					if(operand_address_mode != QB_ADDRESS_MODE_UNKNOWN) {
						if(address->mode != operand_address_mode) {
							// we can only go from SCA > ELE > ARR (an array cannot be passed as a scalar, for instance)
							if(address->mode < operand_address_mode) {
								operand->address = qb_promote_address_mode(cxt, operand_address_mode, address);
							} else {
								operands_are_valid = FALSE;
								break;
							}
						}

						// at this point, the storage has been assigned to all base addresses
						// make sure addresses derived from them are pointing to the right place
						qb_resolve_derived_address_storage(cxt, operand->address);
					} else {
						operands_are_valid = FALSE;
						break;
					}
				}
			}

			if(!operands_are_valid) {
				// shouldn't happen at runtime but we'll leave the error check in here just in case
				const char *op_name = qb_get_op_name(cxt, qop->opcode);
				cxt->line_number = qop->line_number;
				qb_abort("invalid operands for %s", op_name);
			}
		}
	}
}

static void qb_resolve_reference_counts(qb_compiler_context *cxt) {
	uint32_t i, j;
	for(i = 0; i < cxt->op_count; i++) {
		qb_op *qop = cxt->ops[i];

		if(qop->opcode != QB_NOP) {
			for(j = 0; j < qop->operand_count; j++) {
				if(qop->operands[j].type == QB_OPERAND_ADDRESS) {
					qb_address *address = qop->operands[j].address;
					qb_memory_segment *segment = &cxt->storage->segments[address->segment_selector];
					if(!(segment->flags & QB_SEGMENT_PREALLOCATED)) {
						segment->reference_count++;
					}
				}
			}
		}
	}
}

static int32_t qb_find_array_address(qb_op *qop) {
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

static qb_op * qb_get_previous_op(qb_compiler_context *cxt, uint32_t index) {
	qb_op *prev_qop = NULL;
	uint32_t j;
	for(j = index; j > 0; j--) {
		prev_qop = cxt->ops[j - 1];
		if(prev_qop->opcode != QB_NOP) {
			break;
		}
	}
	return prev_qop;
}

static qb_op * qb_get_next_op(qb_compiler_context *cxt, uint32_t index) {
	qb_op *next_qop = NULL;
	uint32_t j;
	for(j = index; j < cxt->op_count - 1; j++) {
		next_qop = cxt->ops[j + 1];
		if(next_qop->opcode != QB_NOP) {
			break;
		}
	}
	return next_qop;
}

static int32_t qb_fuse_conditional_branch(qb_compiler_context *cxt, uint32_t index) {
	qb_op *qop = cxt->ops[index];

	if(qop->opcode == QB_IF_T_I32 || qop->opcode == QB_IF_F_I32) {
		qb_address *condition_address = qop->operands[0].address;

		// don't fuse the instructions if the condition is going to be reused (in a short-circuited expression)
		if(TEMPORARY(condition_address) && !(condition_address->flags & QB_ADDRESS_REUSED)) {
			qb_op *prev_qop = qb_get_previous_op(cxt, index);

			if(prev_qop && prev_qop->operand_count == 3 && prev_qop->operands[2].address == condition_address) {
				qb_opcode new_opcode = 0;

				// combine the comparison op with the branch op
				// (only if no arrays are involved since the combined instructions aren't implemented for them)
				if(!qb_find_array_address(prev_qop)) {
					// TODO: get rid of this switch loop
					switch(prev_qop->opcode) {
						case QB_EQ_I08_I08_I32:	new_opcode = QB_IF_EQ_I08_I08; break;
						case QB_EQ_I16_I16_I32:	new_opcode = QB_IF_EQ_I16_I16; break;
						case QB_EQ_I32_I32_I32:	new_opcode = QB_IF_EQ_I32_I32; break;
						case QB_EQ_I64_I64_I32:	new_opcode = QB_IF_EQ_I64_I64; break;
						case QB_EQ_F32_F32_I32: new_opcode = QB_IF_EQ_F32_F32; break;
						case QB_EQ_F64_F64_I32: new_opcode = QB_IF_EQ_F64_F64; break;

						case QB_NE_I08_I08_I32:	new_opcode = QB_IF_NE_I08_I08; break;
						case QB_NE_I16_I16_I32:	new_opcode = QB_IF_NE_I16_I16; break;
						case QB_NE_I32_I32_I32:	new_opcode = QB_IF_NE_I32_I32; break;
						case QB_NE_I64_I64_I32:	new_opcode = QB_IF_NE_I64_I64; break;
						case QB_NE_F32_F32_I32: new_opcode = QB_IF_NE_F32_F32; break;
						case QB_NE_F64_F64_I32: new_opcode = QB_IF_NE_F64_F64; break;

						case QB_LT_S08_S08_I32:	new_opcode = QB_IF_LT_S08_S08; break;
						case QB_LT_S16_S16_I32:	new_opcode = QB_IF_LT_S16_S16; break;
						case QB_LT_S32_S32_I32:	new_opcode = QB_IF_LT_S32_S32; break;
						case QB_LT_S64_S64_I32:	new_opcode = QB_IF_LT_S64_S64; break;
						case QB_LT_U08_U08_I32:	new_opcode = QB_IF_LT_U08_U08; break;
						case QB_LT_U16_U16_I32:	new_opcode = QB_IF_LT_U16_U16; break;
						case QB_LT_U32_U32_I32:	new_opcode = QB_IF_LT_U32_U32; break;
						case QB_LT_U64_U64_I32:	new_opcode = QB_IF_LT_U64_U64; break;
						case QB_LT_F32_F32_I32: new_opcode = QB_IF_LT_F32_F32; break;
						case QB_LT_F64_F64_I32: new_opcode = QB_IF_LT_F64_F64; break;

						case QB_LE_S08_S08_I32:	new_opcode = QB_IF_LE_S08_S08; break;
						case QB_LE_S16_S16_I32:	new_opcode = QB_IF_LE_S16_S16; break;
						case QB_LE_S32_S32_I32:	new_opcode = QB_IF_LE_S32_S32; break;
						case QB_LE_S64_S64_I32:	new_opcode = QB_IF_LE_S64_S64; break;
						case QB_LE_U08_U08_I32:	new_opcode = QB_IF_LE_U08_U08; break;
						case QB_LE_U16_U16_I32:	new_opcode = QB_IF_LE_U16_U16; break;
						case QB_LE_U32_U32_I32:	new_opcode = QB_IF_LE_U32_U32; break;
						case QB_LE_U64_U64_I32:	new_opcode = QB_IF_LE_U64_U64; break;
						case QB_LE_F32_F32_I32: new_opcode = QB_IF_LE_F32_F32; break;
						case QB_LE_F64_F64_I32: new_opcode = QB_IF_LE_F64_F64; break;
						default: break;
					}
					if(new_opcode) {
						// swap the jump targets if it was branching on false
						if(qop->opcode == QB_IF_F_I32) {
							uint32_t temp = qop->jump_target_indices[0];
							qop->jump_target_indices[0] = qop->jump_target_indices[1];
							qop->jump_target_indices[1] = temp;
						}

						// transfer the jump targets to the previous op and make the current one NOP
						prev_qop->opcode = new_opcode;
						prev_qop->flags |= QB_OP_JUMP;
						prev_qop->jump_target_indices = qop->jump_target_indices;
						prev_qop->jump_target_count = qop->jump_target_count;
						prev_qop->operand_count = 2;
						qop->opcode = QB_NOP;

						return TRUE;
					}
				}
			}
		}
	}
	return FALSE;
}

static int32_t qb_fuse_multiply_accumulate(qb_compiler_context *cxt, uint32_t index) {
	qb_op *qop = cxt->ops[index];

	if(qop->operand_count == 3 && !(qop->flags & QB_OP_JUMP) && (TEMPORARY(qop->operands[2].address))) {
		qb_op *next_qop = qb_get_next_op(cxt, index);

		if(next_qop && next_qop->operand_count == 3 && !(next_qop->flags & QB_OP_JUMP)) {
			if(next_qop->operands[0].address == qop->operands[2].address || next_qop->operands[1].address == qop->operands[2].address) {
				qb_opcode new_opcode = 0;
				if((qop->opcode == QB_MUL_S32_S32_S32) && (next_qop->opcode == QB_ADD_I32_I32_I32)) {
					new_opcode = QB_MAC_S32_S32_S32_S32;
				} else if((qop->opcode == QB_MUL_S64_S64_S64) && (next_qop->opcode == QB_ADD_I64_I64_I64)) {
					new_opcode = QB_MAC_S64_S64_S64_S64;
				} else if((qop->opcode == QB_MUL_U32_U32_U32) && (next_qop->opcode == QB_ADD_I32_I32_I32)) {
					// the temporary variable could be reused in a foreach-list construct
					// should come up with a cleaner way to deal with this
					int32_t reused = FALSE;
					uint32_t k, qop_checked;
					for(k = index + 2, qop_checked = 0; k < cxt->op_count - 1 && qop_checked <= 3; k++) {
						qb_op *qop_ahead = cxt->ops[k];
						if(qop_ahead->opcode != QB_NOP) {
							if(!(qop_ahead->flags & QB_OP_JUMP)) {
								if(qop_ahead->operand_count == 3 && qop_ahead->operands[1].address == qop->operands[2].address) {
									// an add
									reused = TRUE;
									break;
								} else if(qop_ahead->operand_count == 2 && qop_ahead->operands[0].address->array_index_address == qop->operands[2].address) {
									// an array element to variable assignment
									reused = TRUE;
									break;
								} else if(qop_ahead->operands[qop_ahead->operand_count - 1].address == qop->operands[2].address) {
									break;
								}
							}
							qop_checked++;
						}
					}
					if(!reused) {
						new_opcode = QB_MAC_U32_U32_U32_U32;
					}
				} else if((qop->opcode == QB_MUL_U64_U64_U64) && (next_qop->opcode == QB_ADD_I64_I64_I64)) {
					new_opcode = QB_MAC_U64_U64_U64_U64;
				} else if((qop->opcode == QB_MUL_F32_F32_F32) && (next_qop->opcode == QB_ADD_F32_F32_F32)) {
					new_opcode = QB_MAC_F32_F32_F32_F32;
				} else if((qop->opcode == QB_MUL_F64_F64_F64) && (next_qop->opcode == QB_ADD_F64_F64_F64)) {
					new_opcode = QB_MAC_F64_F64_F64_F64;
				} else if((qop->opcode == QB_MUL_2X_F32_F32_F32) && (next_qop->opcode == QB_ADD_2X_F32_F32_F32)) {
					new_opcode = QB_MAC_2X_F32_F32_F32_F32;
				} else if((qop->opcode == QB_MUL_2X_F64_F64_F64) && (next_qop->opcode == QB_ADD_2X_F64_F64_F64)) {
					new_opcode = QB_MAC_2X_F64_F64_F64_F64;
				} else if((qop->opcode == QB_MUL_3X_F32_F32_F32) && (next_qop->opcode == QB_ADD_3X_F32_F32_F32)) {
					new_opcode = QB_MAC_3X_F32_F32_F32_F32;
				} else if((qop->opcode == QB_MUL_3X_F64_F64_F64) && (next_qop->opcode == QB_ADD_3X_F64_F64_F64)) {
					new_opcode = QB_MAC_3X_F64_F64_F64_F64;
				} else if((qop->opcode == QB_MUL_4X_F32_F32_F32) && (next_qop->opcode == QB_ADD_4X_F32_F32_F32)) {
					new_opcode = QB_MAC_4X_F32_F32_F32_F32;
				} else if((qop->opcode == QB_MUL_4X_F64_F64_F64) && (next_qop->opcode == QB_ADD_4X_F64_F64_F64)) {
					new_opcode = QB_MAC_4X_F64_F64_F64_F64;
				}
				if(new_opcode) {
					qb_operand *new_operands = qb_allocate_operands(cxt->pool, 4);
					if(next_qop->operands[0].address == qop->operands[2].address) {
						new_operands[0] = next_qop->operands[1];
					} else {
						new_operands[0] = next_qop->operands[0];
					}
					new_operands[1] = qop->operands[0];
					new_operands[2] = qop->operands[1];
					new_operands[3] = next_qop->operands[2];
					qop->operands = new_operands;
					qop->operand_count = 4;
					qop->opcode = new_opcode;
					next_qop->opcode = QB_NOP;
					return TRUE;
				}
			}
		}
	}
	return FALSE;
}

static void qb_simplify_jump(qb_compiler_context *cxt, uint32_t index) {
	qb_op *qop = cxt->ops[index];

	if(qop->opcode == QB_JMP) {
		uint32_t target_index = qop->jump_target_indices[0];
		qb_op *target_qop = cxt->ops[target_index];

		// if the jump lands on another jump, change the target to that jump's target
		while(target_qop->opcode == QB_JMP && target_index != index) {
			target_index = target_qop->jump_target_indices[0];
			target_qop = cxt->ops[target_index];
			qop->jump_target_indices[0] = target_index;
		}

		// if the only thing sitting between the jump and the target
		// are nop's then eliminate the jump altogether
		if(target_index > index) {
			int32_t needed = 0;
			uint32_t j;

			for(j = index + 1; j < target_index; j++) {
				if(cxt->ops[j]->opcode != QB_NOP) {
					needed = TRUE;
					break;
				}
			}
			if(!needed) {
				qop->opcode = QB_NOP;
				qop->jump_target_count = 0;
			}
		}
	}
}

static void qb_fuse_instructions(qb_compiler_context *cxt, int32_t pass) {
	uint32_t i;
	if(pass == 1) {
		// opcodes are not address mode specific at this point
		// the last op is always RET: there's no need to scan it
		for(i = 0; i < cxt->op_count - 1; i++) {
			if(qb_fuse_conditional_branch(cxt, i)) {
				continue;
			}
			if(qb_fuse_multiply_accumulate(cxt, i)) {
				continue;
			}
			qb_simplify_jump(cxt, i);
		}
	} else if(pass == 2) {
		// opcodes are address mode specific here
	}
}


void qb_execute_op(qb_compiler_context *cxt, qb_op *op) {
	/*
	USE_TSRM
	qb_function _qfunc, *qfunc = &_qfunc;
	qb_variable _retval, *retval = &_retval;
	zend_function _zfunc, *zfunc = &_zfunc;
	qb_op **current_ops, *ops[2], _ret_op, *ret_op = &_ret_op;
	uint32_t current_op_count, current_segment_count;
	int8_t instructions[256];
	uint16_t opcodes[32];

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
	qfunc->instruction_opcodes = cxt->instruction_opcodes = opcodes;
	qb_set_instruction_offsets(cxt);
	qfunc->instruction_length = cxt->instruction_length;
	qfunc->instruction_opcode_count = cxt->instruction_op_count;
	qb_encode_instructions(cxt);
	cxt->instructions = NULL;
	cxt->instruction_length = 0;
	cxt->instruction_op_count = 0;

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
	*/
}

void qb_initialize_compiler_data_pool(qb_compiler_data_pool *pool) {
	memset(pool, 0, sizeof(qb_compiler_data_pool));

	// have an array that keeps track of all other arrays
	qb_create_array((void **) &pool->arrays, &pool->array_count, sizeof(void *), 64);
	
	qb_create_block_allocator(&pool->op_allocator, sizeof(qb_op), 256);
	qb_create_block_allocator(&pool->address_allocator, sizeof(qb_address), 1024);
	qb_create_block_allocator(&pool->expression_allocator, sizeof(qb_expression), 256);
	qb_create_block_allocator(&pool->pointer_allocator, sizeof(void *), 256);
	qb_create_block_allocator(&pool->operand_allocator, sizeof(qb_operand), 1024);
	qb_create_block_allocator(&pool->index_alias_scheme_allocator, sizeof(qb_index_alias_scheme), 16);
	qb_create_block_allocator(&pool->string_allocator, sizeof(char), 1024);
	qb_create_block_allocator(&pool->uint32_allocator, sizeof(uint32_t), 64);
	qb_create_block_allocator(&pool->type_declaration_allocator, sizeof(qb_type_declaration), 256);
	qb_create_block_allocator(&pool->variable_allocator, sizeof(qb_variable), 256);
	qb_create_block_allocator(&pool->function_declaration_allocator, sizeof(qb_function_declaration), 16);
	qb_create_block_allocator(&pool->class_declaration_allocator, sizeof(qb_class_declaration), 16);

	qb_create_block_allocator(&pool->result_destination_allocator, sizeof(qb_result_destination), 64);
	qb_create_block_allocator(&pool->array_initializer_allocator, sizeof(qb_array_initializer), 64);
}

void qb_free_compiler_data_pool(qb_compiler_data_pool *pool) {
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
	qb_destroy_block_allocator(&pool->result_destination_allocator);

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

void qb_initialize_compiler_context(qb_compiler_context *cxt, qb_compiler_data_pool *pool, qb_function_declaration *function_decl TSRMLS_DC) {
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
	}
	SAVE_TSRMLS

	qb_attach_new_array(pool, (void **) &cxt->variables, &cxt->variable_count, sizeof(qb_variable *), 16);
	qb_attach_new_array(pool, (void **) &cxt->ops, &cxt->op_count, sizeof(qb_op *), 256);
	qb_attach_new_array(pool, (void **) &cxt->constant_scalars, &cxt->constant_scalar_count, sizeof(qb_address *), 64);
	qb_attach_new_array(pool, (void **) &cxt->writable_scalars, &cxt->writable_scalar_count, sizeof(qb_address *), 64);
	qb_attach_new_array(pool, (void **) &cxt->constant_arrays, &cxt->constant_array_count, sizeof(qb_address *), 16);
	qb_attach_new_array(pool, (void **) &cxt->writable_arrays, &cxt->writable_array_count, sizeof(qb_address *), 16);
	qb_attach_new_array(pool, (void **) &cxt->address_aliases, &cxt->address_alias_count, sizeof(qb_address *), 64);
	qb_attach_new_array(pool, (void **) &cxt->on_demand_expressions, &cxt->on_demand_expression_count, sizeof(qb_address *), 64);

	cxt->matrix_order = QB_G(column_major_matrix) ? QB_MATRIX_ORDER_COLUMN_MAJOR : QB_MATRIX_ORDER_ROW_MAJOR;

	// only set up segments for scalars and fixed-length arrays initially
	cxt->storage = emalloc(sizeof(qb_storage));
	cxt->storage->flags = 0;
	cxt->storage->segment_count = QB_SELECTOR_ARRAY_START;
	cxt->storage->segments = emalloc(sizeof(qb_memory_segment) * cxt->storage->segment_count);
	memset(cxt->storage->segments, 0, sizeof(qb_memory_segment) * cxt->storage->segment_count);

	cxt->storage->segments[QB_SELECTOR_CONSTANT_SCALAR].flags = QB_SEGMENT_PREALLOCATED;
	cxt->storage->segments[QB_SELECTOR_STATIC_SCALAR].flags = QB_SEGMENT_PREALLOCATED;
	cxt->storage->segments[QB_SELECTOR_SHARED_SCALAR].flags = QB_SEGMENT_PREALLOCATED | QB_SEGMENT_CLEAR_ON_CALL;
	cxt->storage->segments[QB_SELECTOR_LOCAL_SCALAR].flags = QB_SEGMENT_PREALLOCATED | QB_SEGMENT_CLEAR_ON_CALL | QB_SEGMENT_SEPARATE_ON_FORK;
	cxt->storage->segments[QB_SELECTOR_TEMPORARY_SCALAR].flags = QB_SEGMENT_PREALLOCATED | QB_SEGMENT_SEPARATE_ON_FORK;
	cxt->storage->segments[QB_SELECTOR_CONSTANT_ARRAY].flags = QB_SEGMENT_PREALLOCATED;
	cxt->storage->segments[QB_SELECTOR_STATIC_ARRAY].flags = QB_SEGMENT_PREALLOCATED;
	cxt->storage->segments[QB_SELECTOR_SHARED_ARRAY].flags = QB_SEGMENT_PREALLOCATED | QB_SEGMENT_CLEAR_ON_CALL;
	cxt->storage->segments[QB_SELECTOR_LOCAL_ARRAY].flags = QB_SEGMENT_PREALLOCATED | QB_SEGMENT_CLEAR_ON_CALL | QB_SEGMENT_SEPARATE_ON_FORK;
	cxt->storage->segments[QB_SELECTOR_TEMPORARY_ARRAY].flags = QB_SEGMENT_PREALLOCATED | QB_SEGMENT_SEPARATE_ON_FORK;

	// make sure these constants exist
	cxt->zero_address = qb_obtain_constant_U32(cxt, 0);
	cxt->one_address = qb_obtain_constant_U32(cxt, 1);
	cxt->false_address = qb_obtain_constant_S32(cxt, 0);
	cxt->false_address->flags |= QB_ADDRESS_BOOLEAN;
	cxt->true_address = qb_obtain_constant_S32(cxt, 1);
	cxt->true_address->flags |= QB_ADDRESS_BOOLEAN;
}

void qb_free_compiler_context(qb_compiler_context *cxt) {
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
	if(cxt->stack_items) {
		efree(cxt->stack_items);
	}
	if(cxt->external_code) {
		efree(cxt->external_code);
	}
}

static void qb_initialize_build_context(qb_build_context *cxt TSRMLS_DC) {
	cxt->deferral_count = 0;
	cxt->pool = &cxt->_pool;
	qb_initialize_compiler_data_pool(cxt->pool);
	qb_attach_new_array(cxt->pool, (void **) &cxt->function_declarations, &cxt->function_declaration_count, sizeof(qb_function_declaration *), 16);
	qb_attach_new_array(cxt->pool, (void **) &cxt->class_declarations, &cxt->class_declaration_count, sizeof(qb_class_declaration *), 16);
	qb_attach_new_array(cxt->pool, (void **) &cxt->compiler_contexts, &cxt->compiler_context_count, sizeof(qb_compiler_context), 16);
	SAVE_TSRMLS
}

void qb_free_build_context(qb_build_context *cxt) {
	uint32_t i;
	for(i = 0; i < cxt->compiler_context_count; i++) {
		qb_compiler_context *compiler_cxt = &cxt->compiler_contexts[i];
		qb_free_compiler_context(compiler_cxt);
	}
	qb_free_compiler_data_pool(cxt->pool);
}

static void qb_print_value(qb_compiler_context *cxt, int8_t *bytes, uint32_t type) {
	USE_TSRM
	char buffer[64];
	uint32_t len;
	len = qb_element_to_string(buffer, sizeof(buffer), bytes, type);
	php_write(buffer, len TSRMLS_CC);
}

static qb_variable * qb_find_variable_with_address(qb_compiler_context *cxt, qb_address *address) {
	uint32_t i;
	for(i = 0; i < cxt->variable_count; i++) {
		qb_variable *qvar = cxt->variables[i];
		if(qvar->address == address) {
			return qvar;
		}
	}
	return NULL;
}

static qb_variable * qb_find_variable_with_size_address(qb_compiler_context *cxt, qb_address *address, uint32_t *p_depth, int32_t *p_recursive) {
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

static void qb_print_address(qb_compiler_context *cxt, qb_address *address, int32_t ignore_index) {
	uint32_t i;
	if(CONSTANT(address)) {
		if(!SCALAR(address)) {
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
	} else if(address->source_address) {
		if(address->source_address->dimension_count == address->dimension_count + 1) {
			// array element
			qb_print_address(cxt, address->source_address, TRUE);
			if(!ignore_index) {
				php_printf("[");
				if(address->array_index_address) {
					qb_print_address(cxt, address->array_index_address, FALSE);
				} else {
					php_printf("%u", ELEMENT_COUNT(address->segment_offset, address->type));
				}
				php_printf("]");
			}
		} else {
			qb_print_address(cxt, address->source_address, FALSE); 
		}
	} else {
		qb_variable *qvar;
		uint32_t depth;
		int32_t recursive;
		if((qvar = qb_find_variable_with_address(cxt, address))) {
			if(qvar->name) {
				if(qvar->flags & QB_VARIABLE_CLASS) {
					if(qvar->zend_class) {
						php_printf("self::$%s", qvar->name);
					} else {
						php_printf("static::$%s", qvar->name);
					}
				} else if(qvar->flags & QB_VARIABLE_CLASS_INSTANCE) {
					php_printf("$this->%s", qvar->name);
				} else if(qvar->flags & QB_VARIABLE_CLASS_CONSTANT) {
						php_printf("static::%s", qvar->name);
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
			if(SCALAR(address)) {
				if(address->flags & QB_ADDRESS_FOREACH_INDEX) {
					php_printf("(fe:%u:%u)", address->segment_selector, address->segment_offset);
				} else {
					php_printf("(%u:%u)", address->segment_selector, address->segment_offset);
				}
			} else {
				if(VARIABLE_LENGTH_ARRAY(address)) {
					php_printf("(%u:%u...)", address->segment_selector, address->segment_offset);
				} else {
					php_printf("(%u:%u..%u:%u)", address->segment_selector, address->segment_offset, address->segment_selector, address->segment_offset + (ARRAY_SIZE(address) - 1));
				}
			}
		}
	}
}

static void qb_print_op(qb_compiler_context *cxt, qb_op *qop, uint32_t index) {
	uint32_t i;
	const char *name = qb_get_op_name(cxt, qop->opcode);
	php_printf("%04d: %s ", index, name);
	for(i = 0; i < qop->operand_count; i++) {
		qb_operand *operand = &qop->operands[i];
		switch(operand->type) {
			case QB_OPERAND_ADDRESS: {
				qb_print_address(cxt, operand->address, FALSE);
			}	break;
			case QB_OPERAND_EXTERNAL_SYMBOL: {
				qb_external_symbol *symbol = &cxt->external_symbols[operand->symbol_index];
				php_printf("[%s]", symbol->name);
			}	break;
			case QB_OPERAND_SEGMENT_SELECTOR: {
				php_printf("@%d", operand->address->segment_selector);
			}	break;
			case QB_OPERAND_NUMBER: {
				php_printf("'%d'", operand->number);
			}	break;
			default: {
				php_printf("(ERROR)");
				break;
			}
		}
		php_printf(" ");
	}
	for(i = 0; i < qop->jump_target_count; i++) {
		uint32_t jump_target_index = qop->jump_target_indices[i];
		php_printf("<%04d> ", jump_target_index);
	}
	php_printf("\n");
}

static void qb_print_ops(qb_compiler_context *cxt) {
	uint32_t i;
	for(i = 0; i < cxt->op_count; i++) {
		qb_op *qop = cxt->ops[i];
		if(qop->opcode != QB_NOP) {
			qb_print_op(cxt, qop, i);
		}
	}
}

void qb_load_external_code(qb_compiler_context *cxt, const char *import_path) {
	USE_TSRM
	php_stream *stream;

	// set active op array to the function to whom the code belong, so that relative paths are resolved correctly
	zend_op_array *active_op_array = EG(active_op_array);
	if(cxt->function_declaration) {
		EG(active_op_array) = &cxt->function_declaration->zend_function->op_array;
	}
	stream = php_stream_open_wrapper_ex((char *) import_path, "rb", USE_PATH | ENFORCE_SAFE_MODE | REPORT_ERRORS, NULL, NULL);
	EG(active_op_array) = active_op_array;

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
		QB_G(current_filename) = cxt->function_declaration->zend_function->op_array.filename;
		QB_G(current_line_number) = cxt->function_declaration->zend_function->op_array.line_start;
		qb_abort("unable to load file containing external code");
	}
}

void qb_free_external_code(qb_compiler_context *cxt) {
	if(cxt->external_code) {
		efree(cxt->external_code);
		cxt->external_code = NULL;
		cxt->external_code_length = 0;
	}
}

static zend_function * qb_get_function(qb_build_context *cxt, zval *callable) {
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
		qb_abort("cannot find function");
	}
#endif
	zfunc = fcc.function_handler;
	if(zfunc->type != ZEND_USER_FUNCTION) {
		if(qb_is_compiled_function(zfunc)) {
			return NULL;
		} else {
			qb_abort("cannot compile an internal PHP function: %s", zfunc->common.function_name);
		}
	}
	return zfunc;
}

static void qb_replace_zend_function(zend_function *zfunc, qb_function *qfunc TSRMLS_DC) {
	// save values that will get wiped by destroy_op_array()
	zend_uint fn_flags = zfunc->common.fn_flags;
	zend_class_entry *scope = zfunc->common.scope;

	// copy argument info
	zend_arg_info *arg_info = emalloc(sizeof(zend_arg_info) * zfunc->common.num_args);
	uint32_t i;
	for(i = 0; i < zfunc->common.num_args ; i++) {
		arg_info[i].pass_by_reference = zfunc->common.arg_info[i].pass_by_reference;
		arg_info[i].allow_null = zfunc->common.arg_info[i].allow_null;
		arg_info[i].name = qfunc->variables[i]->name;
		arg_info[i].name_len = qfunc->variables[i]->name_length;
	}

	// free the op array
	destroy_op_array(&zfunc->op_array TSRMLS_CC);
	memset(&zfunc->op_array, 0, sizeof(zend_op_array));

	// make the function internal function
	zfunc->type = ZEND_INTERNAL_FUNCTION;
	zfunc->common.fn_flags = fn_flags;
	zfunc->common.scope = scope;
#ifdef ZEND_ACC_DONE_PASS_TWO
	zfunc->common.fn_flags &= ~ZEND_ACC_DONE_PASS_TWO;
#endif
	zfunc->common.function_name = qfunc->name;
	zfunc->common.arg_info = arg_info;
	zfunc->internal_function.handler = PHP_FN(qb_execute);
#if !ZEND_ENGINE_2_1
	zfunc->internal_function.module = &qb_module_entry;
#endif

	qfunc->zend_function = zfunc;

	// store the pointer to the qb function in space vacated by the op array
	zfunc->op_array.reserved[0] = qfunc;
}

static void qb_replace_zend_functions(zend_function *zfunc, qb_function *qfunc TSRMLS_DC) {
	zend_uint refcount = *zfunc->op_array.refcount - 1;
	zend_op *opcodes = zfunc->op_array.opcodes;
	zend_class_entry *scope = zfunc->common.scope;
	uint32_t function_count = 1, function_buffer_size = 16;
	qb_replace_zend_function(zfunc, qfunc TSRMLS_CC);	

	// replace the function in descendant classes as well
	if(scope && refcount > 0) {
		Bucket *q;
		const char *name = qfunc->name;
		uint32_t name_length = strlen(qfunc->name);
		ulong name_hash = zend_inline_hash_func(name, name_length + 1);
		for(q = EG(class_table)->pListTail; q && refcount > 0; q = q->pListLast) {
			zend_class_entry **p_ce = q->pData, *ce = *p_ce;
			if(ce->type == ZEND_USER_CLASS) {
				zend_class_entry *ancestor = ce->parent;
				for(ancestor = ce->parent; ancestor; ancestor = ancestor->parent) {
					if(ancestor == scope) {
						if(zend_hash_quick_find(&ce->function_table, name, name_length + 1, name_hash, (void **) &zfunc) == SUCCESS) {
							if(zfunc->common.scope == scope) {
								if(zfunc->type == ZEND_USER_FUNCTION && zfunc->op_array.opcodes == opcodes) {
									qb_replace_zend_function(zfunc, qfunc TSRMLS_CC);
									refcount--;
								}
							}
						}
						break;
					}
				}
			} else {
				break;
			}
		}
	}
}

void qb_compile_functions(qb_build_context *cxt) {
	USE_TSRM
	qb_compiler_context *compiler_cxt;
	uint32_t i, native_compile = FALSE;

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
			qb_php_translater_context _translater_cxt, *translater_cxt = &_translater_cxt;
			qb_initialize_php_translater_context(translater_cxt, compiler_cxt TSRMLS_CC);

			// show the zend opcodes if turned on
			if(QB_G(show_zend_opcodes)) {
				qb_print_zend_ops(translater_cxt);
			}

			// translate the zend ops to intermediate qb ops
			qb_translate_instructions(translater_cxt);
		} else {
			// load the code into memory
			qb_load_external_code(compiler_cxt, compiler_cxt->function_declaration->import_path);

			// decode the pbj data
			//qb_pbj_decode(compiler_cxt);

			// show the pb opcodes if turned on
			if(QB_G(show_pbj_opcodes)) {
				//qb_pbj_print_ops(compiler_cxt);
			}

			// map function arguments to PB kernel parameters
			//qb_pbj_map_arguments(compiler_cxt);

			// create the main loop and translate the PB instructions
			//qb_pbj_translate_instructions(compiler_cxt);

			// free the binary
			qb_free_external_code(compiler_cxt);
		}

		// make all jump target indices absolute
		qb_resolve_jump_targets(compiler_cxt);

		// fuse basic instructions into compound ones
		qb_fuse_instructions(compiler_cxt, 1);

		// assign storage space to variables
		qb_assign_storage_space(compiler_cxt);

		// make opcodes address-mode-specific
		qb_resolve_address_modes(compiler_cxt);

		// try to fuse more instructions
		qb_fuse_instructions(compiler_cxt, 2);

		// figure out how many references to relocatable segments there are
		qb_resolve_reference_counts(compiler_cxt);

		// show the qb opcodes if turned on
		if(QB_G(show_opcodes)) {
			qb_print_ops(compiler_cxt);
		}

		QB_G(current_filename) = NULL;
		QB_G(current_line_number) = 0;
	}

	// generate the instruction streams
	for(i = 0; i < cxt->compiler_context_count; i++) {
		qb_encoder_context _encoder_cxt, *encoder_cxt = &_encoder_cxt;
		qb_function *qfunc, **p_qfunc;

		compiler_cxt = &cxt->compiler_contexts[i];
		qb_initialize_encoder_context(encoder_cxt, compiler_cxt TSRMLS_CC);

		// encode the instruction stream
		qfunc = qb_encode_function(encoder_cxt);

		// replace the zend function with the qb version
		qb_replace_zend_functions(compiler_cxt->zend_function, qfunc TSRMLS_CC);

		// add the function to the global table so we can free it afterward
		if(!QB_G(compiled_functions)) {
			qb_create_array((void **) &QB_G(compiled_functions), &QB_G(compiled_function_count), sizeof(qb_function *), 16);
		}
		p_qfunc = qb_enlarge_array((void **) &QB_G(compiled_functions), 1);
		*p_qfunc = qfunc;

		if(compiler_cxt->function_flags & QB_ENGINE_COMPILE_IF_POSSIBLE) {
			native_compile = TRUE;
		}
	}

#ifdef NATIVE_COMPILE_ENABLED
	// compile all functions inside build in one go
	if(native_compile) {
		if(QB_G(allow_native_compilation)) {
			qb_native_compile(cxt TSRMLS_C);
		}
	}
	if(!QB_G(allow_bytecode_interpretation)) {
		for(i = 0; i < cxt->compiler_context_count; i++) {
			compiler_cxt = &cxt->compiler_contexts[i];
			if(!compiler_cxt->native_proc) {
				qb_abort("unable to compile to native code");
			}
		}
	}
#endif
}

int qb_compile(zval *arg1, zval *arg2 TSRMLS_DC) {
	qb_build_context *cxt = QB_G(build_context);
	Bucket *p, *q;
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
				function_decl = qb_parse_function_doc_comment(cxt->pool, specified_function, NULL);
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
					if(action == QB_SCAN_ALL || strcmp(zfunc->op_array.filename, current_filename) == 0) {
						int32_t fd_index = qb_find_function_declaration(cxt, zfunc);
						if(fd_index == -1) {
							qb_function_declaration *function_decl = qb_parse_function_doc_comment(cxt->pool, zfunc, NULL);
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
								// handle the function at the parent
								if(zfunc->common.scope == ce) {
									int32_t fd_index = qb_find_function_declaration(cxt, zfunc);
									if(fd_index == -1) {
										qb_function_declaration *function_decl = qb_parse_function_doc_comment(cxt->pool, zfunc, ce);
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
					}
				} else {
					break;
				}
			}
		}
	}
	if(cxt->deferral_count > 0) {
		// perform compilation later
		return SUCCESS;
	}

	if(cxt->function_declaration_count) {
		qb_compile_functions(cxt);
		qb_free_build_context(cxt);
		efree(cxt);
		QB_G(build_context) = NULL;
	} else {
		qb_abort("no qb functions found");
	}
	return SUCCESS;
}

void qb_open_diagnostic_loop(qb_compiler_context *cxt) {
	cxt->stage = QB_STAGE_OPCODE_TRANSLATION;
}

#define DIAGNOSTIC_ITERATION	2000000

void qb_close_diagnostic_loop(qb_compiler_context *cxt) {
	qb_operand iteration, counter;
	uint32_t jump_target_indices[2];

	iteration.address = qb_obtain_constant_U32(cxt, DIAGNOSTIC_ITERATION);
	iteration.type = QB_OPERAND_ADDRESS;
	counter.address = qb_create_writable_scalar(cxt, QB_TYPE_U32);
	counter.type = QB_OPERAND_ADDRESS;
	jump_target_indices[0] = 0;
	jump_target_indices[1] = QB_INSTRUCTION_NEXT;

	qb_create_op(cxt, &factory_loop, &iteration, 1, &counter, jump_target_indices, 2, FALSE);
	qb_create_op(cxt, &factory_return, NULL, 0, NULL, NULL, 0, FALSE);
}

void qb_create_diagnostic_loop(qb_compiler_context *cxt, qb_diagnostic_type test_type) {
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
				value1_address = qb_create_constant_fixed_length_array(cxt, QB_TYPE_F32, 4);
				value2_address = qb_create_constant_fixed_length_array(cxt, QB_TYPE_F32, 4);
				for(i = 0, v = (float32_t) M_E; i < 4; i++) {
					ARRAY(F32, value1_address)[i] = v * 3;
					ARRAY(F32, value2_address)[i] = v * 8000;
					v += 10;
				}
				break;
			}
			default: break;
		}

		switch(test_type) {
			case QB_DIAGNOSTIC_INT_ADD:
			case QB_DIAGNOSTIC_FLOAT_ADD:
			case QB_DIAGNOSTIC_VECTOR_ADD: {
				result_address = qb_retrieve_binary_op_result(cxt, &factory_add, value1_address, value2_address);
			}	break;
			case QB_DIAGNOSTIC_INT_MUL:
			case QB_DIAGNOSTIC_FLOAT_MUL:
			case QB_DIAGNOSTIC_VECTOR_MUL: {
				result_address = qb_retrieve_binary_op_result(cxt, &factory_multiply, value1_address, value2_address);
			}	break;
			case QB_DIAGNOSTIC_INT_DIV:
			case QB_DIAGNOSTIC_FLOAT_DIV:
			case QB_DIAGNOSTIC_VECTOR_DIV: { 
				result_address = qb_retrieve_binary_op_result(cxt, &factory_divide, value1_address, value2_address);
			}	break;
			case QB_DIAGNOSTIC_INT_MAC:
			case QB_DIAGNOSTIC_FLOAT_MAC:
			case QB_DIAGNOSTIC_VECTOR_MAC: {
				intermediate_address = qb_retrieve_binary_op_result(cxt, &factory_multiply, value1_address, value2_address);
				result_address = qb_retrieve_binary_op_result(cxt, &factory_add, value2_address, intermediate_address);
			}	break;
			default: break;
		}
	}
	qb_close_diagnostic_loop(cxt);
}

void qb_run_diagnostic_loop(qb_compiler_context *cxt) {
	USE_TSRM
	qb_function *qfunc;
	qb_encoder_context _encoder_cxt, *encoder_cxt = &_encoder_cxt;

	qb_resolve_jump_targets(cxt);
	qb_fuse_instructions(cxt, 1);
	qb_assign_storage_space(cxt);
	qb_resolve_address_modes(cxt);
	qb_fuse_instructions(cxt, 2);

	qb_initialize_encoder_context(encoder_cxt, cxt TSRMLS_CC);
	qfunc = qb_encode_function(encoder_cxt);
	qb_execute_internal(qfunc TSRMLS_CC);
	qb_free_function(qfunc);
}

int qb_run_diagnostics(qb_diagnostics *info TSRMLS_DC) {
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

int qb_initialize_compiler(TSRMLS_D) {
	return SUCCESS;
}
