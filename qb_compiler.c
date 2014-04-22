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

#include "qb_compiler_optimization.c"

static void qb_add_variable(qb_compiler_context *cxt, qb_variable *variable) {
	qb_variable **p = qb_enlarge_array((void **) &cxt->variables, 1);
	*p = variable;
	cxt->function_prototype.variables = cxt->variables;
	cxt->function_prototype.variable_count = cxt->variable_count;
}

void qb_mark_as_tagged(qb_compiler_context *cxt, qb_address *address) {
	address->flags |= QB_ADDRESS_TAGGED;
	if(address->source_address) {
		qb_mark_as_tagged(cxt, address->source_address);
	}
}

void qb_mark_as_constant(qb_compiler_context *cxt, qb_address *address) {
	address->flags |= QB_ADDRESS_CONSTANT;
	if(address->source_address) {
		qb_mark_as_constant(cxt, address->source_address);
	}
}

void qb_mark_as_temporary(qb_compiler_context *cxt, qb_address *address) {
	address->flags |= QB_ADDRESS_TEMPORARY | QB_ADDRESS_NON_REUSABLE | QB_ADDRESS_IN_USE;
	if(address->source_address) {
		qb_mark_as_temporary(cxt, address->source_address);
	}
}

void qb_mark_as_writable(qb_compiler_context *cxt, qb_address *address) {
	address->flags &= ~QB_ADDRESS_READ_ONLY;
	if(address->source_address) {
		qb_mark_as_writable(cxt, address->source_address);
	}
}

void qb_mark_as_static(qb_compiler_context *cxt, qb_address *address) {
	if(IS_READ_ONLY(address)) {
		address->flags |= QB_ADDRESS_STATIC;

		if(IS_VARIABLE_LENGTH(address)) {
			// dimensions that aren't constant must become static as well 
			uint32_t i;
			for(i = 0; i < address->dimension_count; i++) {
				qb_address *dimension_address = address->dimension_addresses[i];
				qb_address *array_size_address = address->dimension_addresses[i];
				if(!IS_IMMUTABLE(dimension_address)) {
					dimension_address->flags |= QB_ADDRESS_STATIC;
				}
				if(!IS_IMMUTABLE(array_size_address)) {
					array_size_address->flags |= QB_ADDRESS_STATIC;
				}
			}
		}
	}
}

void qb_mark_as_shared(qb_compiler_context *cxt, qb_address *address) {
	address->flags |= QB_ADDRESS_SHARED;

	if(IS_VARIABLE_LENGTH(address)) {
		// dimensions that aren't constant must become shared as well 
		uint32_t i;
		for(i = 0; i < address->dimension_count; i++) {
			qb_address *dimension_address = address->dimension_addresses[i];
			qb_address *array_size_address = address->dimension_addresses[i];
			if(!IS_IMMUTABLE(dimension_address)) {
				dimension_address->flags |= QB_ADDRESS_SHARED;
			}
			if(!IS_IMMUTABLE(array_size_address)) {
				array_size_address->flags |= QB_ADDRESS_SHARED;
			}
		}
	}
}

void qb_lock_address(qb_compiler_context *cxt, qb_address *address) {
	if(!IS_IN_USE(address) && IS_TEMPORARY(address)) {
		address->flags |= QB_ADDRESS_IN_USE;
		if(address->source_address) {
			qb_lock_address(cxt, address->source_address);
		}
		if(IS_TEMPORARY(address->array_size_address)) {
			qb_lock_address(cxt, address->array_size_address);
		}
		if(address->dimension_count > 1) {
			uint32_t i;
			for(i = 0; i < address->dimension_count; i++) {
				qb_lock_address(cxt, address->array_size_addresses[i]);
				qb_lock_address(cxt, address->dimension_addresses[i]);
			}
		}
	}
	if(IS_TEMPORARY(address->array_index_address)) {
		qb_lock_address(cxt, address->array_index_address);
	}
}

void qb_unlock_address(qb_compiler_context *cxt, qb_address *address) {
	if(IS_IN_USE(address) && IS_TEMPORARY(address)) {
		address->flags &= ~QB_ADDRESS_IN_USE;
		if(address->source_address) {
			qb_unlock_address(cxt, address->source_address);
		}
		if(IS_TEMPORARY(address->array_index_address)) {
			qb_unlock_address(cxt, address->array_index_address);
		}
		if(IS_TEMPORARY(address->array_size_address)) {
			qb_unlock_address(cxt, address->array_size_address);
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

int32_t qb_is_source_op_translated(qb_compiler_context *cxt, uint32_t source_index) {
	if(source_index < cxt->op_translation_table_size) {
		uint32_t qop_index = cxt->op_translation_table[source_index];
		if(qop_index < cxt->op_count) {
			return TRUE;
		}
	}
	return FALSE;
}

uint32_t qb_set_source_file(qb_compiler_context *cxt, const char *file_path) {
	return cxt->source_file_id;
}

uint32_t qb_set_source_op_index(qb_compiler_context *cxt, uint32_t source_index, uint32_t line_number) {
	uint32_t op_index;
	if(cxt->stage == QB_STAGE_OPCODE_TRANSLATION) {
		if(source_index >= cxt->op_translation_table_size) {
			// expand the table
			uint32_t original_size = cxt->op_translation_table_size;
			uint32_t addition = (source_index + 1) - original_size;
			uint32_t *new_entries = qb_enlarge_array((void **) &cxt->op_translation_table, addition);
			uint32_t i;
			for(i = 0; i < addition; i++) {
				new_entries[i] = INVALID_INDEX;
			}
		}
		cxt->op_translation_table[source_index] = op_index = cxt->op_count;
	} else {
		op_index = INVALID_INDEX;
	}
	cxt->source_file_line_number = line_number;
	cxt->source_op_index = source_index;
	cxt->line_id = LINE_ID(cxt->source_file_id, cxt->source_file_line_number);
	return op_index;
}

static uint32_t qb_get_translated_op_index(qb_compiler_context *cxt, uint32_t jump_target_index) {
	int32_t offset = OP_INDEX_OFFSET(jump_target_index);
	uint32_t source_index = OP_INDEX(jump_target_index);
	uint32_t qop_index = cxt->op_translation_table[source_index] + offset;
	return qop_index;
}

void qb_add_jump_target(qb_compiler_context *cxt, uint32_t jump_target_index) {
	qb_jump_target *jump_target;
	if(!cxt->jump_targets) {
		qb_attach_new_array(cxt->pool, (void **) &cxt->jump_targets, &cxt->jump_target_count, sizeof(qb_jump_target), 16);
	}
	jump_target = qb_enlarge_array((void **) &cxt->jump_targets, 1);
	jump_target->jump_target_index = jump_target_index;
}

int32_t qb_is_jump_target(qb_compiler_context *cxt, uint32_t source_op_index) {
	uint32_t i = 0;
	for(i = 0; i < cxt->jump_target_count; i++) {
		qb_jump_target *jump_target = &cxt->jump_targets[i];
		if(OP_INDEX(jump_target->jump_target_index) == source_op_index) {
			return TRUE;
		}
	}
	return FALSE;
}

static void qb_invalidate_expression(qb_compiler_context *cxt, qb_expression *expr) {
	uint32_t i;
	for(i = 0; i < expr->operand_count + 1; i++) {
		qb_unlock_operand(cxt, &expr->operands[i]);
	}
	expr->flags &= ~QB_EXPR_RESULT_IS_STILL_VALID;
	expr->result->type = QB_OPERAND_EMPTY;
	expr->result->generic_pointer = NULL;
}

static int32_t qb_match_address(qb_compiler_context *cxt, qb_address *address1, qb_address *address2) {
	qb_address *a, *b;
	for(a = address1; a; a = a->source_address) {
		for(b = address2; b; b = b->source_address) {
			if(a == b) {
				return TRUE;
			}
		}
	}
	return FALSE;
}

static void qb_invalidate_on_demand_expressions(qb_compiler_context *cxt, qb_address *address) {
	int32_t is_tagged = FALSE;
	qb_address *a;
	for(a = address; a; a = a->source_address) {
		if(IS_TAGGED(a)) {
			is_tagged = TRUE;
			break;
		}
	}
	if(is_tagged) {
		uint32_t i, j;
		for(i = 0; i < cxt->on_demand_expression_count; i++) {
			qb_expression *expr = cxt->on_demand_expressions[i];
			if(expr->flags & QB_EXPR_RESULT_IS_STILL_VALID) {
				for(j = 0; j < expr->operand_count + 1; j++) {
					qb_operand *operand = &expr->operands[j];
					if(operand->type == QB_OPERAND_ADDRESS) {
						if(qb_match_address(cxt, address, operand->address)) {
							qb_invalidate_expression(cxt, expr);
							break;
						}
					}
				}
			}
		}
	}
}

void qb_invalidate_all_on_demand_expressions(qb_compiler_context *cxt) {
	uint32_t i;
	for(i = 0; i < cxt->on_demand_expression_count; i++) {
		qb_expression *expr = cxt->on_demand_expressions[i];
		if(expr->flags & QB_EXPR_RESULT_IS_STILL_VALID) {
			qb_invalidate_expression(cxt, expr);
		}
	}
}

qb_expression * qb_get_on_demand_expression(qb_compiler_context *cxt, void *op_factory, qb_operand *operands, uint32_t operand_count) {
	qb_expression *expr, **p_expr;
	uint32_t i, j;

	// see if there's an existing expression
	for(i = 0; i < cxt->on_demand_expression_count; i++) {
		expr = cxt->on_demand_expressions[i];
		if(expr->op_factory == op_factory && expr->operand_count == operand_count) {
			int32_t match = TRUE;
			for(j = 0; j < expr->operand_count; j++) {
				if(expr->operands[j].type == operands[j].type) {
					if(operands[j].type == QB_OPERAND_ADDRESS) {
						if(expr->operands[j].address != operands[j].address) {
							match = FALSE;
						}
					} else if(operands[j].type == QB_OPERAND_NUMBER) {
						if(expr->operands[j].number != operands[j].number) {
							match = FALSE;
						}
					} else if(operands[j].type == QB_OPERAND_ZEND_CLASS) {
						if(expr->operands[j].zend_class != operands[j].zend_class) {
							match = FALSE;
						}
					}
				} else {
					match = FALSE;
				}
			}
			if(match) {
				return expr;
			}
		}
	}

	// create the expression
	expr = qb_allocate_expression(cxt->pool);
	expr->flags = QB_EXPR_EXECUTE_BEFORE;
	expr->operands = qb_allocate_operands(cxt->pool, operand_count + 1);
	expr->operand_count = operand_count;
	expr->op_factory = op_factory;
	for(i = 0; i < operand_count; i++) {
		qb_operand *operand = &operands[i];
		expr->operands[i] = *operand;
		if(operand->type == QB_OPERAND_ADDRESS) {
			// tag the address so we know it is being used in an on-demand expression
			qb_mark_as_tagged(cxt, operand->address);
		}
	}
	expr->result = &expr->operands[operand_count];
	expr->result->type = QB_OPERAND_EMPTY;
	expr->result->address = NULL;
	p_expr = qb_enlarge_array((void **) &cxt->on_demand_expressions, 1);
	*p_expr = expr;
	return expr;
}

// the qb_obtain_on_demand_??? functions only works with U32 scalar addresses
qb_address * qb_obtain_on_demand_sum(qb_compiler_context *cxt, qb_address *augend_address, qb_address *addend_address) {
	if(IS_IMMUTABLE(augend_address) && IS_IMMUTABLE(addend_address)) {
		uint32_t sum = VALUE(U32, augend_address) + VALUE(U32, addend_address);
		return qb_obtain_constant_U32(cxt, sum);
	} else if(IS_IMMUTABLE(augend_address) && VALUE(U32, augend_address) == 0) {
		// 0 + addend = addend
		return addend_address;
	} else if(IS_IMMUTABLE(addend_address) && VALUE(U32, addend_address) == 0) {
		// augend + 0 = augend
		return augend_address;
	} else {
		qb_operand operands[2] = { { QB_OPERAND_ADDRESS, { augend_address } }, { QB_OPERAND_ADDRESS, { addend_address } } };
		return qb_obtain_on_demand_value(cxt, &factory_add, operands, 2);
	}
}

qb_address * qb_obtain_on_demand_difference(qb_compiler_context *cxt, qb_address *minuend_address, qb_address *subtrahend_address) {
	if(IS_IMMUTABLE(minuend_address) && IS_IMMUTABLE(subtrahend_address)) {
		uint32_t difference = VALUE(U32, minuend_address) - VALUE(U32, subtrahend_address);
		return qb_obtain_constant_U32(cxt, difference);
	} else if(IS_IMMUTABLE(subtrahend_address) && VALUE(U32, subtrahend_address) == 0) {
		// minuend - 0 = minuend
		return minuend_address;
	} else {
		qb_operand operands[2] = { { QB_OPERAND_ADDRESS, { minuend_address } }, { QB_OPERAND_ADDRESS, { subtrahend_address } } };
		return qb_obtain_on_demand_value(cxt, &factory_subtract, operands, 2);
	}
}

qb_address * qb_obtain_on_demand_product(qb_compiler_context *cxt, qb_address *multiplicand_address, qb_address *multiplier_address) {
	if(IS_IMMUTABLE(multiplicand_address) && IS_IMMUTABLE(multiplier_address)) {
		uint32_t product = VALUE(U32, multiplicand_address) * VALUE(U32, multiplier_address);
		return qb_obtain_constant_U32(cxt, product);
	} else if(IS_IMMUTABLE(multiplicand_address) && VALUE(U32, multiplicand_address) == 0) {
		// 0 * multiplier = 0
		return qb_obtain_constant_U32(cxt, 0);
	} else if(IS_IMMUTABLE(multiplicand_address) && VALUE(U32, multiplicand_address) == 1) {
		// 1 * multiplier = multiplier
		return multiplier_address;
	} else if(IS_IMMUTABLE(multiplier_address) && VALUE(U32, multiplier_address) == 0) {
		// multiplicand * 0 = 0
		return qb_obtain_constant_U32(cxt, 0);
	} else if(IS_IMMUTABLE(multiplier_address) && VALUE(U32, multiplier_address) == 1) {
		// multiplicand * 1 = multiplicand
		return multiplicand_address;
	} else {
		qb_operand operands[2] = { { QB_OPERAND_ADDRESS, { multiplicand_address } }, { QB_OPERAND_ADDRESS, { multiplier_address } } };
		return qb_obtain_on_demand_value(cxt, &factory_real_multiply, operands, 2);
	}
}

qb_address * qb_obtain_on_demand_quotient(qb_compiler_context *cxt, qb_address *numerator_address, qb_address *denominator_address) {
	if(IS_IMMUTABLE(numerator_address) && IS_IMMUTABLE(denominator_address)) {
		uint32_t quotient = VALUE(U32, numerator_address) / VALUE(U32, denominator_address);
		return qb_obtain_constant_U32(cxt, quotient);
	} else {
		qb_operand operands[2] = { { QB_OPERAND_ADDRESS, { numerator_address } }, { QB_OPERAND_ADDRESS, { denominator_address } } };
		return qb_obtain_on_demand_value(cxt, &factory_divide, operands, 2);
	}
}

static uint32_t qb_get_undefined_dimension_count(qb_compiler_context *cxt, qb_address *address) {
	uint32_t i;
	uint32_t count = 0;
	for(i = 0; i < address->dimension_count; i++) {
		qb_address *dimension_address = DIMENSION_ADDRESS(address, i);
		if(!IS_IMMUTABLE(dimension_address)) {
			count++;
		}
	}
	return count;
}

void qb_attach_bound_checking_expression(qb_compiler_context *cxt, qb_address *address, qb_variable_dimensions *dim, int32_t resizing) {
	qb_expression *expr = NULL;
	if(address->array_size_address == dim->array_size_address) {
		// size match: no bound-checking needed
		return;
	} else {
		if(IS_IMMUTABLE(address->array_size_address) && IS_IMMUTABLE(dim->array_size_address)) {
			uint32_t dst_size = VALUE(U32, address->array_size_address);
			uint32_t src_size = VALUE(U32, dim->array_size_address);
			// the destination is large enough
			if(dst_size > src_size) {
				return;
			}
		}
		if(resizing && IS_RESIZABLE(address)) {
			// accommodate the input by resizing the array
			// if it's multidimensional, the dimension has to be updated as well
			if(address->dimension_count > 1) {
				USE_TSRM
				uint32_t undefined_dimension_count = qb_get_undefined_dimension_count(cxt, address);
				uint32_t defined_dimension_count = address->dimension_count - undefined_dimension_count;
				if(undefined_dimension_count == 1) {
					qb_operand operands[6] = { { QB_OPERAND_ADDRESS, { dim->array_size_address } }, { QB_OPERAND_ADDRESS, { address->array_size_address } }, { QB_OPERAND_ADDRESS, { address->dimension_addresses[0] } }, { QB_OPERAND_ADDRESS, { address->array_size_addresses[1] } }, { QB_OPERAND_SEGMENT_SELECTOR, { address } }, { QB_OPERAND_ELEMENT_SIZE, { address } } };
					expr = qb_get_on_demand_expression(cxt, &factory_accommodate_array_size_update_dimension, operands, 6);
				} else {
					uint32_t i, j;
					if(address->dimension_count == dim->dimension_count) {
						qb_operand operands[MAX_DIMENSION * 4 + 2];
						for(i = undefined_dimension_count; i < address->dimension_count; i++) {
							if(address->dimension_addresses[i] != dim->dimension_addresses[i]) {
								uint32_t dimension1 = VALUE(U32, address->dimension_addresses[i]);
								uint32_t dimension2 = VALUE(U32, dim->dimension_addresses[i]);
								qb_report_dimension_mismatch_exception(cxt->line_id, dimension1, dimension2);
								qb_dispatch_exceptions(TSRMLS_C);
							}
						}
						for(i = 0; i < undefined_dimension_count; i++) {
							operands[i * 4 + 0].address = dim->array_size_addresses[i];
							operands[i * 4 + 0].type = QB_OPERAND_ADDRESS;
							operands[i * 4 + 1].address = dim->dimension_addresses[i];
							operands[i * 4 + 1].type = QB_OPERAND_ADDRESS;
							operands[i * 4 + 2].address = address->array_size_addresses[i];
							operands[i * 4 + 2].type = QB_OPERAND_ADDRESS;
							operands[i * 4 + 3].address = address->dimension_addresses[i];
							operands[i * 4 + 3].type = QB_OPERAND_ADDRESS;
						}
						operands[undefined_dimension_count * 4 + 0].address = address;
						operands[undefined_dimension_count * 4 + 0].type = QB_OPERAND_SEGMENT_SELECTOR;
						operands[undefined_dimension_count * 4 + 1].address = address;
						operands[undefined_dimension_count * 4 + 1].type = QB_OPERAND_ELEMENT_SIZE;
						expr = qb_get_on_demand_expression(cxt, &factory_accommodate_array_size_copy_dimension, operands, undefined_dimension_count * 4 + 2);
					} else if(dim->dimension_count == 0) {
						// assignment from a scalar is okay
					} else if(defined_dimension_count == dim->dimension_count) {
						// no resizing
						for(i = undefined_dimension_count, j = 0; i < address->dimension_count; i++, j++) {
							if(address->dimension_addresses[i] != dim->dimension_addresses[j]) {
								uint32_t dimension1 = VALUE(U32, address->dimension_addresses[i]);
								uint32_t dimension2 = VALUE(U32, dim->dimension_addresses[i]);
								qb_report_dimension_mismatch_exception(cxt->line_id, dimension1, dimension2);
								qb_dispatch_exceptions(TSRMLS_C);
							}
						}
					} else {
						qb_report_dimension_count_mismatch_exception(cxt->line_id, address->dimension_count, dim->dimension_count);
						qb_dispatch_exceptions(TSRMLS_C);
					}
				}
			} else {
				qb_operand operands[4] = { { QB_OPERAND_ADDRESS, { dim->array_size_address } }, { QB_OPERAND_ADDRESS, { address->array_size_address } }, { QB_OPERAND_SEGMENT_SELECTOR, { address } }, { QB_OPERAND_ELEMENT_SIZE, { address } } };
				expr = qb_get_on_demand_expression(cxt, &factory_accommodate_array_size, operands, 4);
			}
		} else {
			// a scalar will not cause overrun
			if(cxt->one_address == dim->array_size_address) {
				return;
			} else {
				// need to guard against overrun
				qb_operand operands[2] = { { QB_OPERAND_ADDRESS, { dim->array_size_address } }, { QB_OPERAND_ADDRESS, { address->array_size_address } } };
				expr = qb_get_on_demand_expression(cxt, &factory_guard_array_size, operands, 2);
			}
		}
	}
	if(expr) {
		// don't attach it if the result of a previous check is still valid
		if(!(expr->flags & QB_EXPR_RESULT_IS_STILL_VALID)) {
			address->expression = expr;
		}
	}
}

static void qb_update_storage_location(qb_compiler_context *cxt, qb_address *address) {
	if(address->source_address) {
		qb_update_storage_location(cxt, address->source_address);
		if(address->segment_selector == QB_SELECTOR_INVALID) {
			address->segment_selector = address->source_address->segment_selector;
		}
		if(address->segment_offset == QB_OFFSET_INVALID) {
			address->segment_offset = address->source_address->segment_offset;
		}
		if(address->mode == QB_ADDRESS_MODE_SCA && address->array_index_address != cxt->zero_address) {
			// add the offset
			uint32_t index = VALUE(U32, address->array_index_address);
			address->segment_offset += BYTE_COUNT(index, address->type);
			address->array_index_address = cxt->zero_address;
		}
	}
}

// allocate segment memory for address
void qb_allocate_storage_space(qb_compiler_context *cxt, qb_address *address, int32_t need_actual_memory) {
	if(address->source_address) {
		if(address->source_address->segment_selector == QB_SELECTOR_INVALID) {
			qb_allocate_storage_space(cxt, address->source_address, need_actual_memory);
		}
		qb_update_storage_location(cxt, address);
	} else {
		uint32_t selector, element_count, byte_count, new_segment_flags, alignment;
		uint32_t start_offset, end_offset;
		qb_memory_segment *segment;

		if(IS_SCALAR(address)) {
			element_count = 1;
			byte_count = BYTE_COUNT(1, address->type);
			alignment = max(byte_count, 4);
		} else {
			if(IS_FIXED_LENGTH(address)) {
				element_count = ARRAY_SIZE(address);
				byte_count = BYTE_COUNT(element_count, address->type);
			} else {
				element_count = 0;
				byte_count = 0;
			}
			alignment = 16;
		}

		// determine which segment should be used
		if(IS_SCALAR(address)) {
			if(IS_IMMUTABLE(address)) {
				selector = QB_SELECTOR_CONSTANT_SCALAR;
			} else if(IS_TEMPORARY(address)) {
				selector = QB_SELECTOR_TEMPORARY_SCALAR;
			} else if(IS_STATIC(address)) {
				selector = QB_SELECTOR_STATIC_SCALAR;
			} else if(IS_SHARED(address)) {
				selector = QB_SELECTOR_SHARED_SCALAR;
			} else {
				selector = QB_SELECTOR_LOCAL_SCALAR;
			}
		} else if(element_count > 0 && byte_count <= 10240) {
			// the array is fixed-length and isn't bigger than 10K
			if(IS_IMMUTABLE(address)) {
				selector = QB_SELECTOR_CONSTANT_ARRAY;
			} else if(IS_TEMPORARY(address)) {
				selector = QB_SELECTOR_TEMPORARY_ARRAY;
			} else if(IS_STATIC(address)) {
				selector = QB_SELECTOR_STATIC_ARRAY;
			} else if(IS_SHARED(address)) {
				selector = QB_SELECTOR_SHARED_ARRAY;
			} else {
				selector = QB_SELECTOR_LOCAL_ARRAY;
			}
		} else {
			// it's a variable-length array or a big fixed-length array
			selector = cxt->storage->segment_count;
			if(IS_TEMPORARY(address)) {
				new_segment_flags = QB_SEGMENT_FREE_ON_RETURN | QB_SEGMENT_SEPARATE_ON_FORK | QB_SEGMENT_SEPARATE_ON_REENTRY;
			} else if(IS_STATIC(address)) {
				new_segment_flags = 0;
			} else if(IS_SHARED(address)) {
				new_segment_flags = QB_SEGMENT_FREE_ON_RETURN | QB_SEGMENT_CLEAR_ON_CALL | QB_SEGMENT_SEPARATE_ON_REENTRY;
			} else {
				new_segment_flags = QB_SEGMENT_FREE_ON_RETURN | QB_SEGMENT_CLEAR_ON_CALL | QB_SEGMENT_SEPARATE_ON_FORK | QB_SEGMENT_SEPARATE_ON_REENTRY;
			}
			if(byte_count > 0) {
					new_segment_flags |= QB_SEGMENT_REALLOCATE_ON_CALL;
			} else {
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
			segment->flags = new_segment_flags;
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
			if(start_offset > segment->byte_count) {
				// clear the padding bytes since we might calculate the checksum of the memory segment
				// (and we want the same value for the same constants)
				memset(segment->memory + segment->byte_count, 0, start_offset - segment->byte_count);
			}
		}
		segment->byte_count = end_offset;

		// fit in the info
		address->segment_selector = selector;
		address->segment_offset = start_offset;
	}
}

void qb_allocate_external_storage_space(qb_compiler_context *cxt, qb_variable *var) {
	USE_TSRM
	qb_import_scope *scope = qb_get_import_scope(cxt->storage, var, NULL TSRMLS_CC);
	qb_variable *ivar = qb_get_import_variable(cxt->storage, var, scope TSRMLS_CC);
	uint32_t selector, start_offset;

	if(ivar->address->segment_selector >= QB_SELECTOR_ARRAY_START) {
		// use a new segment
		selector = cxt->storage->segment_count;
		start_offset = ivar->address->segment_offset;
	} else {
		// use the same segment as in the scope
		selector = ivar->address->segment_selector;
		start_offset = ivar->address->segment_offset;
	}

	if(var->address->dimension_count > 0) {
		// set the locations of the dimensional variables as well if they aren't constant
		if(var->address->dimension_count == 1) {
			if(!IS_IMMUTABLE(var->address->array_size_address)) {
				var->address->array_size_address->segment_selector = ivar->address->array_size_address->segment_selector;
				var->address->array_size_address->segment_offset = ivar->address->array_size_address->segment_offset;
			}
		} else {
			uint32_t i;
			for(i = 0; i < var->address->dimension_count; i++) {
				if(!IS_IMMUTABLE(var->address->array_size_addresses[i])) {
					var->address->array_size_addresses[i]->segment_selector = ivar->address->array_size_addresses[i]->segment_selector;
					var->address->array_size_addresses[i]->segment_offset = ivar->address->array_size_addresses[i]->segment_offset;
					var->address->dimension_addresses[i]->segment_selector = ivar->address->dimension_addresses[i]->segment_selector;
					var->address->dimension_addresses[i]->segment_offset = ivar->address->dimension_addresses[i]->segment_offset;
				}
			}
		}
	}

	if(selector >= cxt->storage->segment_count) {
		qb_memory_segment *segment;
		cxt->storage->segment_count = selector + 1;
		cxt->storage->segments = erealloc(cxt->storage->segments, sizeof(qb_memory_segment) * cxt->storage->segment_count);
		segment = &cxt->storage->segments[selector];
		memset(segment, 0, sizeof(qb_memory_segment));
	}

	var->address->segment_selector = selector;
	var->address->segment_offset = start_offset;
}

// assign storage space to variables
void qb_assign_storage_space(qb_compiler_context *cxt) {
	uint32_t i;

	// set current_allocation to the byte_count so we know how much real 
	// data there's in each segment
	for(i = 0; i < cxt->storage->segment_count; i++) {
		qb_memory_segment *segment = &cxt->storage->segments[i];
		segment->current_allocation = segment->byte_count;
	}

	// deal with external variables first
	for(i = 0; i < cxt->variable_count; i++) {
		qb_variable *qvar = cxt->variables[i];
		if(qvar->flags & (QB_VARIABLE_GLOBAL | QB_VARIABLE_LEXICAL)) {
			qb_allocate_external_storage_space(cxt, qvar);
		} else if((qvar->flags & QB_VARIABLE_CLASS) || (qvar->flags & QB_VARIABLE_CLASS_INSTANCE) || (qvar->flags & QB_VARIABLE_CLASS_CONSTANT)) {
			if(!qvar->zend_class) {
				// static:: qualifier--give it a scope temporarily
				qvar->zend_class = cxt->zend_op_array->scope;
				qb_allocate_external_storage_space(cxt, qvar);
				qvar->zend_class = NULL;
			} else {
				qb_allocate_external_storage_space(cxt, qvar);
			}
		}
	}

	for(i = 0; i < cxt->writable_scalar_count; i++) {
		qb_address *address = cxt->writable_scalars[i];
		if(address->segment_selector == QB_SELECTOR_INVALID) {
			// if it's temporary yet remains read-only, then it was never used
			if(!IS_TEMPORARY(address) || !IS_READ_ONLY(address)) {
				qb_allocate_storage_space(cxt, address, FALSE);
			}
		}
	}
	for(i = 0; i < cxt->writable_array_count; i++) {
		qb_address *address = cxt->writable_arrays[i];
		if(address->segment_selector == QB_SELECTOR_INVALID) {
			if(!IS_TEMPORARY(address) || !IS_READ_ONLY(address)) {
				qb_allocate_storage_space(cxt, address, FALSE);
			}
		}
	}

	// update the address aliases
	for(i = 0; i < cxt->address_alias_count; i++) {
		qb_address *address = cxt->address_aliases[i];
		qb_update_storage_location(cxt, address);
	}
}

qb_address * qb_create_address_alias(qb_compiler_context *cxt, qb_address *address) {
	qb_address *alias = qb_allocate_address(cxt->pool);
	qb_address **p_alias = qb_enlarge_array((void **) &cxt->address_aliases, 1);
	*alias = *address;
	alias->source_address = address;
	alias->flags |= QB_ADDRESS_ALIAS;
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
				if(alias->flags & flag) {
					if(address->expression) {
						// attach the expression onto the alias
						alias->expression = address->expression;
					}
					if(address->flags & QB_ADDRESS_IN_USE) {
						alias->flags |= QB_ADDRESS_IN_USE;
					} else {
						alias->flags &= ~QB_ADDRESS_IN_USE;
					}
					return alias;
				}
			}
		}
	}
	alias = qb_create_address_alias(cxt, address);
	alias->flags |= flag;
	return alias;
}

qb_address * qb_obtain_string_alias(qb_compiler_context *cxt, qb_address *address) {
	return qb_obtain_alias_by_address_flag(cxt, address, QB_ADDRESS_STRING);
}

qb_address * qb_obtain_boolean_alias(qb_compiler_context *cxt, qb_address *address) {
	return qb_obtain_alias_by_address_flag(cxt, address, QB_ADDRESS_BOOLEAN);
}

qb_address * qb_obtain_reused_alias(qb_compiler_context *cxt, qb_address *address) {
	return qb_obtain_alias_by_address_flag(cxt, address, QB_ADDRESS_REUSED);
}

qb_address * qb_obtain_cast_alias(qb_compiler_context *cxt, qb_address *address, qb_primitive_type type) {
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

qb_address * qb_obtain_array_alias(qb_compiler_context *cxt, qb_address *address) {
	qb_address *alias;
	uint32_t i; 
	for(i = 0; i < cxt->address_alias_count; i++) {
		alias = cxt->address_aliases[i];
		if(alias->source_address == address) {
			if(alias->dimension_count == 1) {
				return alias;
			}
		}
	}
	alias = qb_create_address_alias(cxt, address);
	alias->dimension_count = 1;
	alias->mode = QB_ADDRESS_MODE_ARR;
	return alias;
}

qb_address * qb_create_constant_scalar(qb_compiler_context *cxt, qb_primitive_type element_type) {
	qb_address *address = qb_allocate_address(cxt->pool);
	address->mode = QB_ADDRESS_MODE_SCA;
	address->type = element_type;
	address->flags = QB_ADDRESS_READ_ONLY | QB_ADDRESS_CONSTANT;
	address->array_index_address = cxt->zero_address;
	address->array_size_address = cxt->one_address;
	address->array_size_addresses =
	address->dimension_addresses = &address->array_size_address;
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
		default: return NULL;
	}
}

qb_address * qb_obtain_constant_float(qb_compiler_context *cxt, float64_t value, qb_primitive_type element_type) {
	switch(element_type) {
		case QB_TYPE_F32: return qb_obtain_constant_F32(cxt, (CTYPE(F32)) value);
		case QB_TYPE_F64: return qb_obtain_constant_F64(cxt, (CTYPE(F64)) value);
		default: return NULL;
	}
}

qb_address * qb_obtain_constant_boolean(qb_compiler_context *cxt, int32_t value) {
	return (value) ? cxt->true_address : cxt->false_address;
}

qb_address * qb_create_writable_scalar(qb_compiler_context *cxt, qb_primitive_type element_type);

static void qb_attach_dimensions(qb_compiler_context *cxt, uint32_t *dimensions, uint32_t dimension_count, qb_address *address, int32_t zero_means_unknown) {
	if(dimension_count == 1) {
		if(dimensions[0] == 0 && zero_means_unknown) {
			// if it's zero, it's unknown at compile time
			address->array_size_address = qb_create_writable_scalar(cxt, QB_TYPE_U32);
			address->array_size_address->flags |= QB_ADDRESS_DIMENSION;
			address->flags |= QB_ADDRESS_RESIZABLE;
		} else {
			address->array_size_address = qb_obtain_constant_U32(cxt, dimensions[0]);
		}
		address->dimension_addresses = &address->array_size_address;
		address->array_size_addresses = &address->array_size_address;
	} else {
		uint32_t i;
		uint32_t array_size = 1;
		qb_address *dimension_address;
		qb_address *array_size_address;

		address->dimension_addresses = qb_allocate_address_pointers(cxt->pool, dimension_count);
		address->array_size_addresses = qb_allocate_address_pointers(cxt->pool, dimension_count);
		for(i = dimension_count - 1; (int32_t) i >= 0; i--) {
			uint32_t dimension = dimensions[i];
			array_size *= dimension;
			if(dimension == 0 && zero_means_unknown) {
				dimension_address = qb_create_writable_scalar(cxt, QB_TYPE_U32);
				dimension_address->flags |= QB_ADDRESS_DIMENSION;
				address->flags |= QB_ADDRESS_RESIZABLE;
			} else {
				dimension_address = qb_obtain_constant_U32(cxt, dimension);
			}
			if(i == dimension_count - 1) {
				array_size_address = dimension_address;
			} else if(dimension == 0 && zero_means_unknown) {
				array_size_address = qb_create_writable_scalar(cxt, QB_TYPE_U32);
				array_size_address->flags |= QB_ADDRESS_DIMENSION;
			} else {
				array_size_address = qb_obtain_constant_U32(cxt, array_size);
			}
			address->dimension_addresses[i] = dimension_address;
			address->array_size_addresses[i] = array_size_address;
		}
		address->array_size_address = array_size_address;
	}
	address->dimension_count = dimension_count;
}

qb_address * qb_create_constant_array(qb_compiler_context *cxt, qb_primitive_type element_type, uint32_t *dimensions, uint32_t dimension_count) {
	qb_address *address = qb_allocate_address(cxt->pool);
	address->mode = QB_ADDRESS_MODE_ARR;
	address->type = element_type;
	address->flags = QB_ADDRESS_READ_ONLY | QB_ADDRESS_CONSTANT;	
	address->array_index_address = cxt->zero_address;
	qb_attach_dimensions(cxt, dimensions, dimension_count, address, FALSE);
	qb_allocate_storage_space(cxt, address, TRUE);
	qb_add_constant_array(cxt, address);
	return address;
}

qb_address * qb_obtain_constant_indices(qb_compiler_context *cxt, uint32_t *indices, uint32_t index_count) {
	qb_address *address;
	uint32_t i, j, *values;
	for(i = 0; i < cxt->constant_array_count; i++) {
		address = cxt->constant_arrays[i];
		if(address->dimension_count == 1 && address->type == QB_TYPE_U32) {
			if(ARRAY_SIZE(address) == index_count) {
				int32_t match = TRUE;
				values = ARRAY(U32, address);
				for(j = 0; j < index_count; j++) {
					if(values[j] != indices[j]) {
						match = FALSE;
					}
				}
				if(match) {
					return address;
				}
			}
		}
	}
	address = qb_create_constant_array(cxt, QB_TYPE_U32, &index_count, 1);
	values = ARRAY(U32, address);
	for(j = 0; j < index_count; j++) {
		values[j] = indices[j];
	}
	return address;
}

uint32_t qb_get_zend_array_dimension_count(qb_compiler_context *cxt, zval *zvalue) {
	if((Z_TYPE_P(zvalue) == IS_ARRAY || Z_TYPE_P(zvalue) == IS_CONSTANT_ARRAY)) {
		USE_TSRM
		HashTable *ht = Z_ARRVAL_P(zvalue);
		Bucket *p;
		uint32_t overall_sub_array_dimension_count = 0;
		for(p = ht->pListHead; p; p = p->pNext) {
			zval **p_element = p->pData;
			uint32_t sub_array_dimension_count = qb_get_zend_array_dimension_count(cxt, *p_element);
			if(overall_sub_array_dimension_count) {
				if(overall_sub_array_dimension_count != sub_array_dimension_count) {
					qb_report_illegal_array_structure_exception(cxt->line_id);
					qb_dispatch_exceptions(TSRMLS_C);
				}
			} else {
				overall_sub_array_dimension_count = sub_array_dimension_count;
			}
		}
		if(overall_sub_array_dimension_count + 1 > MAX_DIMENSION) {
			qb_report_illegal_dimension_count_exception(cxt->line_id, overall_sub_array_dimension_count + 1);
			qb_dispatch_exceptions(TSRMLS_C);
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
		uint32_t dimension = ht->nNextFreeElement;
		if(dimension > dimensions[0]) {
			dimensions[0] = dimension;
		}
		if(dimension_count > 1) {
			dimensions[1] = 0;
			for(p = ht->pListHead; p; p = p->pListNext) {
				if((long) p->h >= 0 && !p->nKeyLength) {
					zval **p_element = p->pData;
					qb_get_zend_array_dimensions(cxt, *p_element, element_type, dimensions + 1, dimension_count - 1);
				}
			}
		}
	} else if(Z_TYPE_P(zvalue) == IS_STRING) {
		USE_TSRM
		uint32_t byte_count = Z_STRLEN_P(zvalue);
		uint32_t dimension = byte_count >> type_size_shifts[element_type];
		if(byte_count != dimension * type_sizes[element_type]) {
			qb_report_binary_string_size_mismatch_exception(cxt->line_id, byte_count, element_type);
			qb_dispatch_exceptions(TSRMLS_C);
		}
		if(dimension > dimensions[0]) {
			dimensions[0] = dimension;
		}
	}
}

static int32_t qb_copy_elements_from_zend_array(qb_compiler_context *cxt, zval *zvalue, qb_address *address);

static int32_t qb_copy_element_from_zval(qb_compiler_context *cxt, zval *zvalue, qb_address *address) {
	if(Z_TYPE_P(zvalue) == IS_LONG) {
		long value = Z_LVAL_P(zvalue);
		switch(address->type) {
			case QB_TYPE_S08: VALUE(S08, address) = (CTYPE(S08)) value; break;
			case QB_TYPE_U08: VALUE(U08, address) = (CTYPE(U08)) value; break;
			case QB_TYPE_S16: VALUE(S16, address) = (CTYPE(S16)) value; break;
			case QB_TYPE_U16: VALUE(U16, address) = (CTYPE(U16)) value; break;
			case QB_TYPE_S32: VALUE(S32, address) = (CTYPE(S32)) value; break;
			case QB_TYPE_U32: VALUE(U32, address) = (CTYPE(U32)) value; break;
			case QB_TYPE_S64: VALUE(S64, address) = (CTYPE(S64)) value; break;
			case QB_TYPE_U64: VALUE(U64, address) = (CTYPE(U64)) value; break;
			case QB_TYPE_F32: VALUE(F32, address) = (CTYPE(F32)) value; break;
			case QB_TYPE_F64: VALUE(F64, address) = (CTYPE(F64)) value; break;
		}
	} else if(Z_TYPE_P(zvalue) == IS_DOUBLE) {
		double value = Z_DVAL_P(zvalue);
		if(zend_isnan(value)) {
			value = NAN;
		}
		switch(address->type) {
			case QB_TYPE_S08: VALUE(S08, address) = (CTYPE(S08)) (CTYPE(S64)) value; break;
			case QB_TYPE_U08: VALUE(U08, address) = (CTYPE(U08)) (CTYPE(S64)) value; break;
			case QB_TYPE_S16: VALUE(S16, address) = (CTYPE(S16)) (CTYPE(S64)) value; break;
			case QB_TYPE_U16: VALUE(U16, address) = (CTYPE(U16)) (CTYPE(S64)) value; break;
			case QB_TYPE_S32: VALUE(S32, address) = (CTYPE(S32)) (CTYPE(S64)) value; break;
			case QB_TYPE_U32: VALUE(U32, address) = (CTYPE(U32)) (CTYPE(S64)) value; break;
			case QB_TYPE_S64: VALUE(S64, address) = (CTYPE(S64)) value; break;
			case QB_TYPE_U64: VALUE(U64, address) = (CTYPE(U64)) value; break;
			case QB_TYPE_F32: VALUE(F32, address) = (CTYPE(F32)) value; break;
			case QB_TYPE_F64: VALUE(F64, address) = (CTYPE(F64)) value; break;
		}
	} else if(Z_TYPE_P(zvalue) == IS_STRING) {
		uint32_t type_size = type_sizes[address->type];
		uint32_t string_len = Z_STRLEN_P(zvalue);
		const char *string = Z_STRVAL_P(zvalue);
		if(type_size != string_len) {
			qb_report_binary_string_size_mismatch_exception(cxt->line_id, string_len, address->type);
			return FALSE;
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
		}
	} else if(Z_TYPE_P(zvalue) == IS_ARRAY || Z_TYPE_P(zvalue) == IS_CONSTANT_ARRAY) {
		switch(address->type) {
			case QB_TYPE_S64:
			case QB_TYPE_U64: {
				if(!qb_zval_array_to_int64(zvalue, &VALUE(I64, address))) {
					qb_report_illegal_conversion_from_array_exception(cxt->line_id, type_names[address->type]);
					return FALSE;
				}
			}	break;
			default: {
				qb_report_illegal_conversion_from_array_exception(cxt->line_id, type_names[address->type]);
				return FALSE;
			}
		}
	}
	return TRUE;
}

static int32_t qb_copy_elements_from_zend_array(qb_compiler_context *cxt, zval *zvalue, qb_address *address) {
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
					if(!qb_copy_elements_from_zend_array(cxt, *p_element, sub_array_address)) {
						return FALSE;
					}
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
					if(!qb_copy_element_from_zval(cxt, *p_element, element_address)) {
						return FALSE;
					}
				} else {
					memset(ARRAY(I08, element_address), 0, element_size);
				}
				element_address->segment_offset += element_size;
			}
		}
	} else if(Z_TYPE_P(zvalue) == IS_STRING) {
		uint32_t src_element_count = ELEMENT_COUNT(Z_STRLEN_P(zvalue), element_type);
		uint32_t src_byte_count = BYTE_COUNT(src_element_count, element_type);
		uint32_t dst_byte_count = dimension * element_size;
		int8_t *src_memory = Z_STRVAL_P(zvalue);
		int8_t *dst_memory = ARRAY(I08, address);
		memcpy(dst_memory, src_memory, src_byte_count);
		memset(dst_memory + src_byte_count, 0, dst_byte_count - src_byte_count);
	}
	return TRUE;
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

		// figure out the dimensions of the array first
		uint32_t dimensions[MAX_DIMENSION] = { 0 };
		uint32_t dimension_count = qb_get_zend_array_dimension_count(cxt, zvalue);
		qb_get_zend_array_dimensions(cxt, zvalue, desired_type, dimensions, dimension_count);

		// create a local array for it
		address = qb_create_constant_array(cxt, desired_type, dimensions, dimension_count);

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
		}
	}
	return NULL;
}

qb_address * qb_obtain_constant_zval_utf8(qb_compiler_context *cxt, zval *zvalue, qb_primitive_type desired_type) {
	qb_address *address;
	uint8_t *bytes = Z_STRVAL_P(zvalue);
	uint32_t byte_count = Z_STRLEN_P(zvalue);
	uint32_t element_count = 0;
	uint32_t state = 0, codepoint, i, j;

	// see how many codepoints there are
	for(i = 0; i < byte_count; i++) {
		if(!decode(&state, &codepoint, bytes[i])) {
			element_count++;
		}
	}

	// create the array
	address = qb_create_constant_array(cxt, desired_type, &element_count, 1);

	// decode the codepoints into it
	state = 0;
	if(STORAGE_TYPE_MATCH(desired_type, QB_TYPE_I16)) {
		uint16_t *elements = ARRAY(U16, address);
		for(i = 0, j = 0; i < byte_count; i++) {
			if(!decode(&state, &codepoint, bytes[i])) {
				elements[j] = codepoint;
				j++;
			}
		}
	} else if(STORAGE_TYPE_MATCH(desired_type, QB_TYPE_I32)) {
		uint32_t *elements = ARRAY(U32, address);
		for(i = 0, j = 0; i < byte_count; i++) {
			if(!decode(&state, &codepoint, bytes[i])) {
				elements[j] = codepoint;
				j++;
			}
		}
	}

	// mark it as something that should print out as text
	address->flags |= QB_ADDRESS_STRING;
	return address;
}

qb_address * qb_obtain_constant(qb_compiler_context *cxt, int64_t value, qb_primitive_type element_type) {
#if ZEND_DEBUG
	if(element_type >= QB_TYPE_COUNT) {
		qb_debug_abort("invalid type id");
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
	address->array_index_address = cxt->zero_address;
	address->array_size_address = cxt->one_address;
	address->array_size_addresses =
	address->dimension_addresses = &address->array_size_address;

	// here "read-only" doesn't mean we're not supposed to write to this address
	// it just means no operation has done so yet
	address->flags = QB_ADDRESS_READ_ONLY;
	qb_add_writable_scalar(cxt, address);
	return address;
}

qb_address * qb_create_writable_array(qb_compiler_context *cxt, qb_primitive_type element_type, uint32_t *dimensions, uint32_t dimension_count) {
	qb_address *address = qb_allocate_address(cxt->pool);
	address->mode = QB_ADDRESS_MODE_ARR;
	address->type = element_type;
	address->flags = QB_ADDRESS_READ_ONLY;
	address->segment_selector = QB_SELECTOR_INVALID;
	address->segment_offset = QB_OFFSET_INVALID;
	address->array_index_address = cxt->zero_address;
	qb_attach_dimensions(cxt, dimensions, dimension_count, address, TRUE);
	qb_add_writable_array(cxt, address);
	return address;
}

qb_address * qb_create_temporary_variable(qb_compiler_context *cxt, qb_primitive_type element_type, qb_variable_dimensions *dim) {
	qb_address *address;
	if(dim && dim->dimension_count > 0) {
		uint32_t array_size = 0;
		if(dim->array_size_address && IS_IMMUTABLE(dim->array_size_address)) {
			array_size = VALUE(U32, dim->array_size_address);
		}
		address = qb_create_writable_array(cxt, element_type, &array_size, 1);
	} else {
		address = qb_create_writable_scalar(cxt, element_type);
	}
	address->flags |= QB_ADDRESS_TEMPORARY;
	return address;
}

static qb_address * qb_obtain_multidimensional_alias(qb_compiler_context *cxt, qb_address *address, qb_variable_dimensions *dim) {
	qb_address *alias;
	uint32_t i; 
	// look for an existing one
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
					if(address->expression) {
						alias->expression = address->expression;
					}
					return alias;
				}
			}
		}
	}

	alias = qb_create_address_alias(cxt, address);

	// transfer the dimensions into the alias
	alias->dimension_count = dim->dimension_count;
	alias->dimension_addresses = qb_allocate_address_pointers(cxt->pool, dim->dimension_count);
	alias->array_size_addresses = qb_allocate_address_pointers(cxt->pool, dim->dimension_count);
	for(i = 0; i < dim->dimension_count; i++) {
		alias->dimension_addresses[i] = dim->dimension_addresses[i];
		alias->array_size_addresses[i] = (i == 0) ? address->array_size_address : dim->array_size_addresses[i];
	}
	if(IS_IN_USE(address)) {
		for(i = 0; i < dim->dimension_count; i++) {
			qb_lock_address(cxt, alias->dimension_addresses[i]);
			qb_lock_address(cxt, alias->array_size_addresses[i]);
		}
	}
	return alias;
}

qb_address * qb_obtain_temporary_variable(qb_compiler_context *cxt, qb_primitive_type element_type, qb_variable_dimensions *dim) {
	uint32_t i;
	qb_address *address = NULL;
	qb_address *usable_address = NULL;
	if(dim && dim->dimension_count > 0) {
		int32_t need_variable_length = !dim->array_size_address || IS_VARIABLE_LENGTH(dim);
		for(i = 0; i < cxt->writable_array_count; i++) {
			qb_address *address = cxt->writable_arrays[i];
			if(IS_TEMPORARY(address) && !IS_IN_USE(address) && !IS_NON_REUSABLE(address)) {
				if(address->type == element_type) {
					if((address->array_size_address == dim->array_size_address)	|| (IS_VARIABLE_LENGTH(address) && need_variable_length)) {
						usable_address = address;
						break;
					}
				}
			}
		}
	} else {
		for(i = 0; i < cxt->writable_scalar_count; i++) {
			qb_address *address = cxt->writable_scalars[i];
			if(IS_TEMPORARY(address) && !IS_IN_USE(address) && !IS_NON_REUSABLE(address)) {
				if(address->type == element_type) {
					usable_address = address;
					break;
				}
			}
		}
	}
	if(!usable_address) {
		usable_address = qb_create_temporary_variable(cxt, element_type, dim);
	}
	qb_lock_address(cxt, usable_address);
	if(dim && dim->dimension_count > 1) {
		address = qb_obtain_multidimensional_alias(cxt, usable_address, dim);
	} else {
		address = usable_address;
	}
	return address;
}

qb_address * qb_obtain_write_target(qb_compiler_context *cxt, qb_primitive_type element_type, qb_variable_dimensions *dim, uint32_t address_flags, qb_result_prototype *result_prototype, int32_t resizing) {
	qb_address *target_address = NULL;

	if(result_prototype && result_prototype->destination) {
		qb_result_destination *destination = result_prototype->destination;
		qb_primitive_type lvalue_type = QB_TYPE_UNKNOWN;
		qb_address *lvalue_size_address = NULL, *lvalue_address = NULL;

		// figure out what kind of lvalue it is
		switch(destination->type) {
			case QB_RESULT_DESTINATION_VARIABLE: {
				if(destination->variable.type == QB_OPERAND_ADDRESS) {
					qb_address *address = destination->variable.address;
					lvalue_type = address->type;
					lvalue_size_address = address->array_size_address;
				}
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
					if(lvalue_size_address && !IS_IMMUTABLE(lvalue_size_address)) {
						// substitution always happens since the lvalue will can be resized to match
						substitute = TRUE;
					} else {
						if(dim) {
							if(dim->array_size_address == lvalue_size_address) {
								// the size is the same
								substitute = TRUE;
							}
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
							if(IS_RESIZABLE(container->address)) {
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
					if(destination->prototype) {
						destination->prototype->final_type = QB_TYPE_VOID;
						target_address = lvalue_address;
					}
				}
			}
		}
	}

	if(!target_address) {
		target_address = qb_obtain_temporary_variable(cxt, element_type, dim);
	}

	if(IS_RESIZABLE(target_address) && resizing) {
		// attach an expression to make it expand/contract
		qb_attach_bound_checking_expression(cxt, target_address, dim, TRUE);
	}
	if(IS_TEMPORARY(target_address)) {
		if(dim->dimension_count > 1) {
			// obtain an alias with dimensional info 
			target_address = qb_obtain_multidimensional_alias(cxt, target_address, dim);
		}
		if(address_flags & QB_ADDRESS_BOOLEAN) {
			target_address = qb_obtain_boolean_alias(cxt, target_address);
		} else if(address_flags & QB_ADDRESS_STRING) {
			target_address = qb_obtain_string_alias(cxt, target_address);
		}
	}
	return target_address;
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

uint32_t qb_get_array_initializer_dimension_count(qb_compiler_context *cxt, qb_array_initializer *initializer) {
	uint32_t overall_sub_array_dimension_count = 0, i;

	for(i = 0; i < initializer->element_count; i++) {
		qb_operand *element = &initializer->elements[i];
		uint32_t sub_array_dimension_count;

		if(element->type == QB_OPERAND_ARRAY_INITIALIZER) {
			sub_array_dimension_count = qb_get_array_initializer_dimension_count(cxt, element->array_initializer);
		} else if(element->type == QB_OPERAND_ZVAL) {
			sub_array_dimension_count = qb_get_zend_array_dimension_count(cxt, element->constant);
		} else if(element->type == QB_OPERAND_ADDRESS && !IS_SCALAR(element->address)) {
			sub_array_dimension_count = element->address->dimension_count;
		} else {
			continue;
		}
		if(overall_sub_array_dimension_count) {
			if(overall_sub_array_dimension_count != sub_array_dimension_count) {
				USE_TSRM
				qb_report_illegal_array_structure_exception(cxt->line_id);
				qb_dispatch_exceptions(TSRMLS_C);
			}
		} else {
			overall_sub_array_dimension_count = sub_array_dimension_count;
		}
	}
	if(overall_sub_array_dimension_count + 1 > MAX_DIMENSION) {
		USE_TSRM
		qb_report_illegal_dimension_count_exception(cxt->line_id, overall_sub_array_dimension_count + 1);
		qb_dispatch_exceptions(TSRMLS_C);
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
			if(IS_IMMUTABLE(element->address)) {
				if(item_address->dimension_count > 0) {
					uint32_t initializer_element_count = ARRAY_SIZE(element->address);
					qb_copy_elements(element->address->type, ARRAY(I08, element->address), initializer_element_count, item_address->type, ARRAY(I08, item_address), item_element_count);
				} else {
					qb_copy_element(element->address->type, ARRAY(I08, element->address), item_address->type, ARRAY(I08, item_address));
				}
			} else {
				// need an address from qb_allocate_address() instead of this fake one sitting on the stack
				// since we're attaching it to an op
				qb_address *item_address_copy = qb_create_address_alias(cxt, item_address);
				item_address_copy->source_address = item_address->source_address;
				item_address_copy->array_size_addresses = 
				item_address_copy->dimension_addresses = &item_address_copy->array_size_address;
				item_address_copy->array_index_address = cxt->zero_address;
				item_address_copy->array_size_address = cxt->one_address;
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
	uint32_t dimensions[MAX_DIMENSION] = { 0 };
	uint32_t dimension_count;
	
	// figure out the dimensions of the array first
	dimension_count = qb_get_array_initializer_dimension_count(cxt, initializer);
	qb_get_array_initializer_dimensions(cxt, initializer, element_type, dimensions, dimension_count);

	if(initializer->flags & QB_ARRAY_INITIALIZER_VARIABLE_ELEMENTS) {
		// need to use temporary fixed-length array, since the elements can change 
		// not using qb_obtain_temporary_variable() since the address cannot be reused
		address = qb_create_writable_array(cxt, element_type, dimensions, dimension_count);
		qb_mark_as_temporary(cxt, address);

		// allocate space for it so we can copy constants into the area
		qb_allocate_storage_space(cxt, address, TRUE);
	} else {
		// the initializer is constant
		address = qb_create_constant_array(cxt, element_type, dimensions, dimension_count);
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

uint32_t qb_find_variable_index(qb_compiler_context *cxt, qb_address *address) {
	uint32_t i;
	qb_variable *qvar;
	if(address) {
		while(address->source_address) {
			if(address->source_address->dimension_count == address->dimension_count) {
				address = address->source_address;
			} else {
				break;
			}
		}
		for(i = 0; i < cxt->variable_count; i++) {
			qvar = cxt->variables[i];
			if(qvar->address == address) {
				return i;
			}
		}
	} else {
		for(i = 0; i < cxt->variable_count; i++) {
			qvar = cxt->variables[i];
			if(qvar->flags & QB_VARIABLE_THIS) {
				return i;
			}
		}
	}
	return INVALID_INDEX;
}

uint32_t qb_get_variable_index(qb_compiler_context *cxt, qb_address *address) {
	uint32_t index = qb_find_variable_index(cxt, address);
	if(index == INVALID_INDEX) {
		qb_variable *qvar = qb_allocate_variable(cxt->pool);
		qvar->address = address;
		index = cxt->variable_count;
		qb_add_variable(cxt, qvar);
	}
	return index;
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

int32_t qb_apply_type_declaration(qb_compiler_context *cxt, qb_variable *qvar) {
	qb_type_declaration *decl = qb_find_variable_declaration(cxt, qvar);
	if(decl) {
		// clear the type flags that don't match
		qvar->flags &= (decl->flags & QB_VARIABLE_TYPES) | ~QB_VARIABLE_TYPES;

		if(decl->type != QB_TYPE_VOID) {
			qb_address *address;
			if(decl->dimension_count == 0) {
				address = qb_create_writable_scalar(cxt, decl->type);
			} else {
				address = qb_create_writable_array(cxt, decl->type, decl->dimensions, decl->dimension_count);
				if(decl->flags & QB_TYPE_DECL_AUTOVIVIFICIOUS) {
					address->flags |= QB_ADDRESS_AUTOVIVIFICIOUS;
				}
			}
			if(decl->flags & QB_TYPE_DECL_STRING) {
				address->flags |= QB_ADDRESS_STRING;
			} else if(decl->flags & QB_TYPE_DECL_BOOLEAN) {
				address->flags |= QB_ADDRESS_BOOLEAN;
			} else if(decl->flags & QB_TYPE_DECL_IMAGE) {
				address->flags |= QB_ADDRESS_IMAGE;
			} else if(decl->flags & QB_TYPE_DECL_COMPLEX) {
				address->flags |= QB_ADDRESS_COMPLEX;
			} else if(decl->flags & QB_TYPE_DECL_VECTOR) {
				address->flags |= QB_ADDRESS_VECTOR;
			} else if(decl->flags & QB_TYPE_DECL_MATRIX) {
				address->flags |= QB_ADDRESS_MATRIX;
			}
			if(decl->flags & QB_TYPE_DECL_HAS_ALIAS_SCHEMES) {
				address->index_alias_schemes = decl->index_alias_schemes;
			}
			if(qvar->flags & QB_VARIABLE_SHARED) {
				qb_mark_as_shared(cxt, address);
			}
			qvar->address = address;
		}
	} else {
		if(qvar->flags & QB_VARIABLE_RETURN_VALUE) {
			// function returns void by default if declaration is missing
		} else if(qvar->flags & QB_VARIABLE_RETURN_KEY_VALUE) {
			// keys are always U32
			qvar->address = qb_create_writable_scalar(cxt, QB_TYPE_U32);
		} else if(qvar->flags & QB_VARIABLE_SENT_VALUE) {
			// yield does not produce a value by default
		} else {
			qb_report_missing_type_declaration_exception(cxt->line_id, qvar);
			return FALSE;
		}
	}
	return TRUE;
}

int32_t qb_perform_static_initialization(qb_compiler_context *cxt, qb_variable *qvar, zval *initial_value) {
	qb_operand assignment_operands[2];
	qb_operand assignment_result = { QB_OPERAND_EMPTY, { NULL } };
	qb_address *initial_value_address = NULL;

	qvar->flags = QB_VARIABLE_STATIC | QB_VARIABLE_LEXICAL;
	if(!qb_apply_type_declaration(cxt, qvar)) {
		return FALSE;
	}

	if(qvar->flags & QB_VARIABLE_STATIC) {
		qb_mark_as_static(cxt, qvar->address);

		// function that uses static variables cannot be inlined
		cxt->function_flags &= ~QB_FUNCTION_INLINEABLE;

		if(qvar->address->type == QB_TYPE_S64 || qvar->address->type == QB_TYPE_U64) {
			// initializing 64-bit integer might require special handling
			qb_primitive_type desired_type = qvar->address->type;
			uint32_t dimension_count = qb_get_zend_array_dimension_count(cxt, initial_value);
			if(qvar->address->dimension_count + 1 == dimension_count) {
				// the array has one less dimension than the variable
				uint32_t dimensions[MAX_DIMENSION] = { 0 };
				qb_get_zend_array_dimensions(cxt, initial_value, desired_type, dimensions, dimension_count);
				if(dimensions[dimension_count - 1] == 2) {
					// treat the last level as scalars
					dimension_count--;
					if(dimension_count > 0) {
						initial_value_address = qb_create_constant_array(cxt, desired_type, dimensions, dimension_count);
						if(!qb_copy_elements_from_zend_array(cxt, initial_value, initial_value_address)) {
							return FALSE;
						}
					} else {
						initial_value_address = qb_create_constant_scalar(cxt, desired_type);
						if(!qb_copy_element_from_zval(cxt, initial_value, initial_value_address)) {
							return FALSE;
						}
					}
				}
			}
		}

		assignment_operands[0].address = qvar->address;
		assignment_operands[0].type = QB_OPERAND_ADDRESS;
		if(initial_value_address) {
			assignment_operands[1].address = initial_value_address;
			assignment_operands[1].type = QB_OPERAND_ADDRESS;
		} else {
			assignment_operands[1].constant = initial_value;
			assignment_operands[1].type = QB_OPERAND_ZVAL;
		}
		return qb_produce_op(cxt, &factory_assign, assignment_operands, 2, &assignment_result, NULL, 0, NULL);
	} else {
		return TRUE;
	}
}

int32_t qb_add_variables(qb_compiler_context *cxt) {
	zend_compiled_variable *zvars = cxt->zend_op_array->vars;
	zend_arg_info *zargs = cxt->zend_op_array->arg_info;
	HashTable *static_variable_table = cxt->zend_op_array->static_variables;
	uint32_t i, variable_count = (uint32_t) cxt->zend_op_array->last_var;
	qb_variable *qvar;

	cxt->argument_count = cxt->zend_op_array->num_args;
	cxt->required_argument_count = cxt->zend_op_array->required_num_args;

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
				qvar->flags |= QB_VARIABLE_BY_REF;
			}
			if(!qb_apply_type_declaration(cxt, qvar)) {
				return FALSE;
			}
			if(!IS_FIXED_LENGTH(qvar->address)) {
				// functions with variable-length array as arguments are not inlineable
				cxt->function_flags &= ~QB_FUNCTION_INLINEABLE;
			}

			// parameters are shared between forked copies of the function
			qb_mark_as_shared(cxt, qvar->address);
		} else {
			// see if it's a static variable
			zval **p_static_value;
			if(static_variable_table && zend_hash_quick_find(static_variable_table, zvar->name, zvar->name_len + 1, zvar->hash_value, (void **) &p_static_value) == SUCCESS) {
				if(Z_TYPE_PP(p_static_value) & (IS_LEXICAL_VAR|IS_LEXICAL_REF)) {
					qvar->flags = QB_VARIABLE_LEXICAL;
					if(Z_TYPE_PP(p_static_value) & IS_LEXICAL_REF) {
						qvar->flags |= QB_VARIABLE_BY_REF;
					}
					if(!qb_apply_type_declaration(cxt, qvar)) {
						return FALSE;
					}
				} else {
					if(!qb_perform_static_initialization(cxt, qvar, *p_static_value)) {
						return FALSE;
					}
				}
			} else {
				// we don't know whether it is a local or a global variable at this point
				qvar->flags = 0;
				qvar->address = NULL;
				if(zvar->name_len == 4 && strcmp(zvar->name, "this") == 0) {
					qvar->flags = QB_VARIABLE_THIS;
				}
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
	if(qvar->address) {
		qb_mark_as_shared(cxt, qvar->address);
	}

#ifdef ZEND_ACC_GENERATOR
	// yield key (PHP 5.5 above)
	if(cxt->zend_op_array->fn_flags & ZEND_ACC_GENERATOR) {
		qvar = qb_allocate_variable(cxt->pool);
		qvar->name = NULL;
		qvar->name_length = 0;
		qvar->hash_value = 0;
		qvar->flags = QB_VARIABLE_RETURN_KEY_VALUE;
		qb_apply_type_declaration(cxt, qvar);
		qb_add_variable(cxt, qvar);
		cxt->return_key_variable = qvar;
		if(qvar->address) {
			qb_mark_as_shared(cxt, qvar->address);
		}

		qvar = qb_allocate_variable(cxt->pool);
		qvar->name = NULL;
		qvar->name_length = 0;
		qvar->hash_value = 0;
		qvar->flags = QB_VARIABLE_SENT_VALUE;
		qb_apply_type_declaration(cxt, qvar);
		qb_add_variable(cxt, qvar);
		cxt->sent_variable = qvar;
		if(qvar->address) {
			qb_mark_as_shared(cxt, qvar->address);
		}
	}
#endif

	if(cxt->op_count > 0) {
		// there're static assignment--need to add the end static op
		qb_operand end_static_result = { QB_OPERAND_EMPTY, { NULL } };
		if(!qb_produce_op(cxt, &factory_end_static, NULL, 0, &end_static_result, NULL, 0, NULL)) {
			return FALSE;
		}
	}
	return TRUE;
}

qb_variable * qb_find_variable(qb_compiler_context *cxt, zend_class_entry *ce, zval *name, uint32_t type_mask) {
	uint32_t i;
	ulong hash_value = Z_HASH_P(name);
	for(i = 0; i < cxt->variable_count; i++) {
		qb_variable *qvar = cxt->variables[i];
		if(qvar->hash_value == hash_value && qvar->name_length == Z_STRLEN_P(name)) {
			if(strncmp(qvar->name, Z_STRVAL_P(name), Z_STRLEN_P(name)) == 0) {
				if(qvar->zend_class == ce) {
					if((qvar->flags & type_mask) || (!type_mask && !(qvar->flags & (QB_VARIABLE_CLASS | QB_VARIABLE_CLASS_INSTANCE | QB_VARIABLE_CLASS_CONSTANT)))) {
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
			if(!qb_apply_type_declaration(cxt, qvar)) {
				return NULL;
			}
		}
	}
	return qvar;
}

qb_variable * qb_get_global_variable(qb_compiler_context *cxt, zval *name) {
	qb_variable *qvar = qb_find_variable(cxt, NULL, name, 0);
	if(qvar) {
		if(!(qvar->flags & QB_VARIABLE_GLOBAL)) {
			qvar->flags |= QB_VARIABLE_GLOBAL;
			if(!qb_apply_type_declaration(cxt, qvar)) {
				return NULL;
			}
		}
	}
	return qvar;
}

qb_variable * qb_get_static_variable(qb_compiler_context *cxt, zval *name) {
	qb_variable *qvar = qb_find_variable(cxt, NULL, name, QB_VARIABLE_STATIC);
	if(!qvar) {
		qvar = qb_find_variable(cxt, NULL, name, QB_VARIABLE_LEXICAL);
	}
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
		if(!qb_apply_type_declaration(cxt, qvar)) {
			return NULL;
		}
		qb_add_variable(cxt, qvar);
	}
	return qvar;
}

qb_variable * qb_get_instance_variable(qb_compiler_context *cxt, zval *name) {
	zend_class_entry *ce = cxt->zend_op_array->scope;
	qb_variable *qvar = qb_find_variable(cxt, ce, name, QB_VARIABLE_CLASS_INSTANCE);
	if(!qvar) {
		qvar = qb_allocate_variable(cxt->pool);
		qvar->flags = QB_VARIABLE_CLASS_INSTANCE;
		qvar->name = Z_STRVAL_P(name);
		qvar->name_length = Z_STRLEN_P(name);
		qvar->hash_value = Z_HASH_P(name);
		qvar->zend_class = ce;
		if(!qb_apply_type_declaration(cxt, qvar)) {
			return NULL;
		}
		qb_add_variable(cxt, qvar);
	}
	return qvar;
}

qb_address * qb_obtain_local_variable(qb_compiler_context *cxt, zval *name) {
	qb_variable *qvar = qb_get_local_variable(cxt, name);
	return (qvar) ? qvar->address : NULL;
}

qb_address * qb_obtain_global_variable(qb_compiler_context *cxt, zval *name) {
	qb_variable *qvar = qb_get_global_variable(cxt, name);
	return (qvar) ? qvar->address : NULL;
}

qb_address * qb_obtain_static_variable(qb_compiler_context *cxt, zval *name) {
	qb_variable *qvar = qb_get_static_variable(cxt, name);
	return (qvar) ? qvar->address : NULL;
}

qb_address * qb_obtain_class_variable(qb_compiler_context *cxt, zend_class_entry *ce, zval *name) {
	qb_variable *qvar = qb_get_class_variable(cxt, ce, name);
	return (qvar) ? qvar->address : NULL;
}

qb_address * qb_obtain_instance_variable(qb_compiler_context *cxt, zval *name) {
	qb_variable *qvar = qb_get_instance_variable(cxt, name);
	return (qvar) ? qvar->address : NULL;
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

void qb_initialize_function_prototype(qb_compiler_context *cxt) {
	cxt->function_prototype.argument_count = cxt->argument_count;
	cxt->function_prototype.variables = cxt->variables;
	cxt->function_prototype.return_variable = cxt->return_variable;
	cxt->function_prototype.zend_op_array = cxt->zend_op_array;
	cxt->function_prototype.name = cxt->zend_op_array->function_name;
	cxt->function_prototype.line_id = cxt->line_id;
	cxt->function_prototype.local_storage = cxt->storage;
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
	qb_primitive_type type = QB_TYPE_ANY;
	if(operand->type == QB_OPERAND_ADDRESS) {
		type = operand->address->type;
	} else if(operand->type == QB_OPERAND_ZVAL) {
		if(!(flags & QB_RETRIEVE_DEFINITE_TYPE_ONLY)) {
			type = qb_get_zval_type(cxt, operand->constant, flags);
		}
	} else if(operand->type == QB_OPERAND_ARRAY_INITIALIZER) {
		type = qb_get_array_initializer_type(cxt, operand->array_initializer, flags);
	} else if(operand->type == QB_OPERAND_RESULT_PROTOTYPE) {
		if(operand->result_prototype->final_type != QB_TYPE_UNKNOWN) {
			type = operand->result_prototype->final_type;
		} 
		if(!(flags & QB_RETRIEVE_DEFINITE_TYPE_ONLY)) {
			if(operand->result_prototype->preliminary_type != QB_TYPE_UNKNOWN) {
				type = operand->result_prototype->preliminary_type;
			}
		}
	}
	if(type != QB_TYPE_ANY) {
		if(flags & QB_COERCE_TO_INTEGER) {
			if(type >= QB_TYPE_F32) {
				if(flags & QB_COERCE_TO_UNSIGNED) {
					type = QB_TYPE_U64;
				} else {
					type = QB_TYPE_I64;
				}
			} 
		} else if(flags & QB_COERCE_TO_FLOATING_POINT) {
			if(type < QB_TYPE_F32) {
				if(flags & QB_COERCE_TO_INTEGER_TO_DOUBLE) {
					type = QB_TYPE_F64;
				} else if(type >= QB_TYPE_S64) {
					type = QB_TYPE_F64;
				} else {
					type = QB_TYPE_F32;
				}
			}
		}
		if(type <= QB_TYPE_S64) {
			if(flags & QB_COERCE_TO_SIGNED) {
				type &= ~QB_TYPE_UNSIGNED;
			} else if(flags & QB_COERCE_TO_UNSIGNED) {
				type |= ~QB_TYPE_UNSIGNED;
			}
		}
	}
	return type;
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
			}	break;
			case QB_RESULT_DESTINATION_ARGUMENT: {
				// TODO:
			}	break;
			default: break;
		}
	}
	return QB_TYPE_UNKNOWN;
}

qb_address * qb_obtain_object_property(qb_compiler_context *cxt, qb_operand *container, qb_operand *name, uint32_t bound_check_flags) {
	qb_address *address = NULL;
	if(container->type == QB_OPERAND_NONE) {
		address = qb_obtain_instance_variable(cxt, name->constant);
	} else if(container->type == QB_OPERAND_ADDRESS) {
		address = qb_obtain_named_element(cxt, container->address, name->constant, bound_check_flags);
	}
	return address;
}

qb_address * qb_obtain_result_destination_address(qb_compiler_context *cxt, qb_result_destination *destination) {
	if(destination) {
		switch(destination->type) {
			case QB_RESULT_DESTINATION_VARIABLE: {
				return destination->variable.address;
			}
			case QB_RESULT_DESTINATION_ELEMENT: {
				return qb_obtain_array_element(cxt, destination->element.container.address, destination->element.index.address, QB_ARRAY_BOUND_CHECK_WRITE);
			}
			case QB_RESULT_DESTINATION_PROPERTY: {
				return qb_obtain_object_property(cxt, &destination->property.container, &destination->property.name, QB_ARRAY_BOUND_CHECK_WRITE);
			}
			case QB_RESULT_DESTINATION_RETURN: {
				if(cxt->return_variable->address) {
					return cxt->return_variable->address;
				}
			}
			default: break;
		}
	}
	return NULL;
}

qb_primitive_type qb_get_operand_default_type(qb_compiler_context *cxt, qb_operand *operand) {
	if(operand->type == QB_OPERAND_RESULT_PROTOTYPE) {
		qb_result_prototype *p = operand->result_prototype;
		do {
			// look at the coercion flag
			if(p->coercion_flags & QB_COERCE_TO_FLOATING_POINT) {
				if(p->coercion_flags & QB_COERCE_TO_INTEGER_TO_DOUBLE) {
					return QB_TYPE_F64;
				} else {
					return QB_TYPE_F32;
				}
			} else if(p->coercion_flags & QB_COERCE_TO_INTEGER) {
				if(p->coercion_flags & QB_COERCE_TO_UNSIGNED) {
					return QB_TYPE_U32;
				} else {
					return QB_TYPE_S32;
				}
			}
			p = p->parent;
		} while(p);
	}
	// default to S32
	return QB_TYPE_S32;
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
			// use the coercion flags of the operands
			for(i = 0; i < count; i++) {
				qb_operand *operand = &operands[i];
				type2 = qb_get_operand_default_type(cxt, operand);
				if(type1 == QB_TYPE_UNKNOWN || type1 < type2) {
					type1 = type2;
				}
			}
		}
	}
	return type1;
}

qb_address * qb_retrieve_unary_op_result(qb_compiler_context *cxt, void *factory, qb_address *address) {
	qb_operand operand = { QB_OPERAND_ADDRESS, { address } };
	qb_operand result = { QB_OPERAND_EMPTY, { NULL } };
	qb_produce_op(cxt, factory, &operand, 1, &result, NULL, 0, NULL);
	if(result.type == QB_OPERAND_ADDRESS) {
		qb_lock_address(cxt, result.address);
	}
	return result.address;
}

qb_address * qb_retrieve_binary_op_result(qb_compiler_context *cxt, void *factory, qb_address *address1, qb_address *address2) {
	qb_operand operands[2] = { { QB_OPERAND_ADDRESS, { address1 } }, { QB_OPERAND_ADDRESS, { address2 } } };
	qb_operand result = { QB_OPERAND_EMPTY, { NULL } };
	qb_produce_op(cxt, factory, operands, 2, &result, NULL, 0, NULL);
	if(result.type == QB_OPERAND_ADDRESS) {
		qb_lock_address(cxt, result.address);
	}
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

int32_t qb_perform_type_coercion(qb_compiler_context *cxt, qb_operand *operand, qb_primitive_type desired_type, uint32_t coercion_flags) {
	switch(operand->type) {
		case QB_OPERAND_RESULT_PROTOTYPE: {
			if(cxt->stage == QB_STAGE_RESULT_TYPE_RESOLUTION) {
				// no type information to record if we don't have any
				if(desired_type != QB_TYPE_ANY) {
					// change the type only if there's flexibility 
					if(operand->result_prototype->final_type == QB_TYPE_ANY || operand->result_prototype->final_type == QB_TYPE_UNKNOWN) {
						if(desired_type >= QB_TYPE_F32 && (operand->result_prototype->coercion_flags & QB_COERCE_TO_INTEGER)) {
							// operand cannot be floating point (e.g. result of bitwise operator) 
							// use the largest integer type instead
							desired_type = QB_TYPE_I64;
						} else if(desired_type < QB_TYPE_F32 && (operand->result_prototype->coercion_flags & QB_COERCE_TO_FLOATING_POINT)) {
							desired_type = QB_TYPE_F64;
						}
						if(desired_type > operand->result_prototype->preliminary_type || operand->result_prototype->preliminary_type == QB_TYPE_ANY || operand->result_prototype->preliminary_type == QB_TYPE_UNKNOWN) {
							operand->result_prototype->preliminary_type = desired_type;
							if(!(operand->result_prototype->coercion_flags & QB_COERCE_TO_LVALUE_TYPE)) {
								operand->result_prototype->final_type = desired_type;
							}
						}
					}
				}
			} else {
				qb_report_internal_error(cxt->line_id, "Invalid operand");
				return FALSE;
			}
		}	break;
		case QB_OPERAND_ADDRESS: {
			if(cxt->stage == QB_STAGE_RESULT_TYPE_RESOLUTION) {
				// don't need to do anything yet
				return TRUE;
			}
			if(coercion_flags & QB_NO_COERCION_OF_VARIABLE) {
				return TRUE;
			}
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
						if(cxt->stage == QB_STAGE_OPCODE_TRANSLATION) {
							new_address = qb_retrieve_temporary_copy(cxt, operand->address, desired_type);
						} else if(cxt->stage == QB_STAGE_CONSTANT_EXPRESSION_EVALUATION && IS_IMMUTABLE(operand->address)) {
							uint32_t element_count;
							if(IS_SCALAR(operand->address)) {
								new_address = qb_create_constant_scalar(cxt, desired_type);
								element_count = 1;
							} else {
								uint32_t dimension_count = operand->address->dimension_count, i;
								uint32_t dimensions[MAX_DIMENSION];
								for(i = 0; i < dimension_count; i++) {
									dimensions[i] = VALUE(U32, operand->address->dimension_addresses[i]);
								}
								new_address = qb_create_constant_array(cxt, desired_type, dimensions, dimension_count);
								element_count = ARRAY_SIZE(new_address);
							}
							qb_copy_elements(operand->address->type, ARRAY(I08, operand->address), element_count, new_address->type, ARRAY(I08, new_address), element_count);
						} else {
							qb_report_internal_error(cxt->line_id, "Invalid operand");
							return FALSE;
						}
					}
					operand->address = new_address;
				}
			} else {
				operand->address = NULL;
				operand->type = QB_OPERAND_NONE;
			}
		}	break;
		case QB_OPERAND_ZVAL: {
			if(cxt->stage == QB_STAGE_RESULT_TYPE_RESOLUTION) {
				// don't need to do anything yet
				return TRUE;
			}
			if(desired_type != QB_TYPE_VOID) {
				if(desired_type == QB_TYPE_ANY) {
					desired_type = qb_get_zval_type(cxt, operand->constant, coercion_flags);
				}
				if((coercion_flags & QB_DECODE_LITERAL_STRING) && Z_TYPE_P(operand->constant) == IS_STRING && desired_type >= QB_TYPE_I16) {
					operand->address = qb_obtain_constant_zval_utf8(cxt, operand->constant, desired_type);
				} else {
					operand->address = qb_obtain_constant_zval(cxt, operand->constant, desired_type);
				}
				operand->type = QB_OPERAND_ADDRESS;
			} else {
				operand->address = NULL;
				operand->type = QB_OPERAND_NONE;
			}
		}	break;
		case QB_OPERAND_ARRAY_INITIALIZER: {
			if(desired_type == QB_TYPE_ANY) {
				desired_type = qb_get_array_initializer_type(cxt, operand->array_initializer, coercion_flags);
			}
			if(cxt->stage == QB_STAGE_OPCODE_TRANSLATION || !(operand->array_initializer->flags & QB_ARRAY_INITIALIZER_VARIABLE_ELEMENTS)) {
				operand->address = qb_retrieve_array_from_initializer(cxt, operand->array_initializer, desired_type);
				operand->type = QB_OPERAND_ADDRESS;
			}
		}	break;
		case QB_OPERAND_THIS: {
			qb_report_illegal_use_of_this(cxt->line_id);
		}	break;
		default: {
			qb_report_internal_error(cxt->line_id, "Invalid operand");
			return FALSE;
		}
	}
	return TRUE;
}

int32_t qb_perform_boolean_coercion(qb_compiler_context *cxt, qb_operand *operand) {
	switch(operand->type) {
		case QB_OPERAND_RESULT_PROTOTYPE: {
			if(cxt->stage == QB_STAGE_RESULT_TYPE_RESOLUTION) {
				operand->result_prototype->preliminary_type = operand->result_prototype->final_type = QB_TYPE_I32;
				operand->result_prototype->address_flags |= QB_ADDRESS_BOOLEAN;
			} else {
				return FALSE;
			}
		}	break;
		case QB_OPERAND_ADDRESS: {
			if(cxt->stage == QB_STAGE_RESULT_TYPE_RESOLUTION) {
				// don't need to do anything yet
				return TRUE;
			}
			if(!(operand->address->flags & QB_ADDRESS_BOOLEAN)) {
				if(IS_IMMUTABLE(operand->address)) {
					int32_t is_true;
					if(IS_SCALAR(operand->address)) {
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
						}
					} else {
						is_true = TRUE;
					}
					operand->address = qb_obtain_constant_boolean(cxt, is_true);
				} else {
					if(cxt->stage == QB_STAGE_OPCODE_TRANSLATION) {
						operand->address = qb_retrieve_unary_op_result(cxt, &factory_boolean_cast, operand->address);
					} else {
						// can't do it
						return FALSE;
					}
				}
			}
		}	break;
		case QB_OPERAND_ZVAL: {
			int32_t is_true = zend_is_true(operand->constant);
			operand->type = QB_OPERAND_ADDRESS;
			operand->address = qb_obtain_constant_boolean(cxt, is_true);
		}	break;
		case QB_OPERAND_ARRAY_INITIALIZER: {
			int32_t is_true = (operand->array_initializer->element_count > 0);
			operand->type = QB_OPERAND_ADDRESS;
			operand->address = qb_obtain_constant_boolean(cxt, is_true);
		}	break;
		case QB_OPERAND_THIS: {
			operand->type = QB_OPERAND_ADDRESS;
			operand->address = qb_obtain_constant_boolean(cxt, TRUE);
		}	break;
		default: {
			return FALSE;
		}
	}
	return TRUE;
}

qb_address * qb_obtain_scalar_value(qb_compiler_context *cxt, qb_address *address) {
	if(address->mode != QB_ADDRESS_MODE_SCA) {
		return qb_retrieve_temporary_copy(cxt, address, QB_TYPE_ANY);
	}
	return address;
}

qb_address * qb_obtain_on_demand_value(qb_compiler_context *cxt, void *factory, qb_operand *operands, uint32_t operand_count) {
	qb_expression *expr = qb_get_on_demand_expression(cxt, factory, operands, operand_count);
	if(expr->flags & QB_EXPR_RESULT_IS_STILL_VALID) {
		// return the still-valid result
		return expr->result->address;
	} else {
		qb_op_factory *f = factory;
		qb_address *result_address;
		qb_primitive_type expr_type;
		uint32_t address_flags;
		// ask the factory to set the result and return that 
		f->resolve_flags(cxt, f, expr->operands, expr->operand_count, &address_flags);
		f->resolve_type(cxt, f, address_flags, expr->operands, expr->operand_count, &expr_type);
		f->set_final_result(cxt, f, expr_type, address_flags, expr->operands, expr->operand_count, expr->result, NULL);
		result_address = expr->result->address;
		qb_mark_as_tagged(cxt, result_address);
		result_address->expression = expr;
		return result_address;
	}
}

qb_address * qb_obtain_predicate_address(qb_compiler_context *cxt, qb_address *container_address, int32_t writable) {
	while(container_address) {
		if(container_address->array_index_address) {
			if(container_address->array_index_address->expression) {
				qb_expression *expr = container_address->array_index_address->expression;
				if(expr->op_factory == &factory_check_array_index_add || expr->op_factory == &factory_check_array_index_multiply || expr->op_factory == &factory_check_array_index_multiply_add) {
					return expr->operands[expr->operand_count - 1].address;
				}
			}
		}
		container_address = container_address->source_address;
	}
	if(writable) {
		qb_address *predicate_address;
		predicate_address = qb_obtain_temporary_variable(cxt, QB_TYPE_I32, NULL);
		predicate_address = qb_retrieve_binary_op_result(cxt, &factory_assign, predicate_address, cxt->true_address);
		return predicate_address;
	} else {
		return cxt->true_address;
	}
}

qb_address * qb_obtain_bound_checked_array_index(qb_compiler_context *cxt, qb_address *container_address, qb_address *index_address, uint32_t bound_check_flags) {
	qb_address *index_limit_address = container_address->dimension_addresses[0];
	qb_address *array_offset_address = container_address->array_index_address;
	qb_address *sub_array_size_address = (container_address->dimension_count > 1) ? container_address->array_size_addresses[1] : NULL;
	int32_t can_expand = FALSE;

	if(IS_AUTOVIVIFICIOUS(container_address)) {
		can_expand = TRUE;
	} else if(IS_RESIZABLE(container_address)) {
		if(index_limit_address == index_address) {
			// an append operation
			can_expand = TRUE;
		}
	}

	if(sub_array_size_address) {
		// if everything is constant, perform the calculation now if it's within bounds
		if(IS_IMMUTABLE(index_address) && IS_IMMUTABLE(sub_array_size_address) && IS_IMMUTABLE(index_limit_address) && IS_IMMUTABLE(array_offset_address)) {
			if(VALUE(U32, index_address) < VALUE(U32, index_limit_address)) {
				uint32_t product = VALUE(U32, index_address) * VALUE(U32, sub_array_size_address);
				uint32_t sum = product + VALUE(U32, array_offset_address);
				return qb_obtain_constant_U32(cxt, sum);
			}
		}
		if(bound_check_flags & QB_ARRAY_BOUND_CHECK_ISSET) {
			// isset() involves only a check of the index
			// the third operand is the predicate variable, which the op factory will obtain
			// use multiply-add if there's an offset, otherwise just multiply the index by the sub-array size
			qb_address *predicate_address = qb_obtain_predicate_address(cxt, container_address, TRUE);
			if(array_offset_address == cxt->zero_address) {
				qb_operand operands[4] = { { QB_OPERAND_ADDRESS, { index_address } }, { QB_OPERAND_ADDRESS, { index_limit_address } }, { QB_OPERAND_ADDRESS, { sub_array_size_address } }, { QB_OPERAND_ADDRESS, { predicate_address } } };
				return qb_obtain_on_demand_value(cxt, &factory_check_array_index_multiply, operands, 4);
			} else {
				qb_operand operands[5] = { { QB_OPERAND_ADDRESS, { index_address } }, { QB_OPERAND_ADDRESS, { index_limit_address } }, { QB_OPERAND_ADDRESS, { sub_array_size_address } }, { QB_OPERAND_ADDRESS, { array_offset_address } }, { QB_OPERAND_ADDRESS, { predicate_address } } };
				return qb_obtain_on_demand_value(cxt, &factory_check_array_index_multiply_add, operands, 5);
			}
		} else if((bound_check_flags & QB_ARRAY_BOUND_CHECK_WRITE) && can_expand) {
			// a write operation and the array can expand
			// enlarge the array to accommodate an index larger than the current size
			// since sub-arrays are never expandable, the offset should always be zero

			qb_operand operands[6] = { { QB_OPERAND_ADDRESS, { index_address } }, { QB_OPERAND_ADDRESS, { index_limit_address } }, { QB_OPERAND_ADDRESS, { sub_array_size_address } }, { QB_OPERAND_ADDRESS, { container_address->array_size_address } }, { QB_OPERAND_SEGMENT_SELECTOR, { container_address } }, { QB_OPERAND_ELEMENT_SIZE, { container_address } } };
			return qb_obtain_on_demand_value(cxt, &factory_accommodate_array_index_multiply, operands, 6);

		} else if(bound_check_flags & (QB_ARRAY_BOUND_CHECK_WRITE | QB_ARRAY_BOUND_CHECK_READ)) {
			// a read operation or a write to a non-expanding array
			if(array_offset_address == cxt->zero_address) {
				qb_operand operands[3] = { { QB_OPERAND_ADDRESS, { index_address } }, { QB_OPERAND_ADDRESS, { index_limit_address } }, { QB_OPERAND_ADDRESS, { sub_array_size_address } } };
				return qb_obtain_on_demand_value(cxt, &factory_guard_array_index_multiply, operands, 3);
			} else {
				qb_operand operands[4] = { { QB_OPERAND_ADDRESS, { index_address } }, { QB_OPERAND_ADDRESS, { index_limit_address } }, { QB_OPERAND_ADDRESS, { sub_array_size_address } }, { QB_OPERAND_ADDRESS, { array_offset_address } } };
				return qb_obtain_on_demand_value(cxt, &factory_guard_array_index_multiply_add, operands, 4);
			}
		} else {
			if(array_offset_address == cxt->zero_address) {
				qb_operand operands[2] = { { QB_OPERAND_ADDRESS, { index_address } }, { QB_OPERAND_ADDRESS, { sub_array_size_address } } };
				return qb_obtain_on_demand_value(cxt, &factory_real_multiply, operands, 2);
			} else {
				qb_operand operands[3] = { { QB_OPERAND_ADDRESS, { index_address } }, { QB_OPERAND_ADDRESS, { sub_array_size_address } }, { QB_OPERAND_ADDRESS, { array_offset_address } } };
				return qb_obtain_on_demand_value(cxt, &factory_multiply_add, operands, 3);
			}
		}
	} else {
		// for retrieve a scalar, the logic is similar
		if(IS_IMMUTABLE(index_address) && IS_IMMUTABLE(index_limit_address) && IS_IMMUTABLE(array_offset_address)) {
			if(VALUE(U32, index_address) < VALUE(U32, index_limit_address)) {
				uint32_t sum = VALUE(U32, index_address) + VALUE(U32, array_offset_address);
				return qb_obtain_constant_U32(cxt, sum);
			}
		}
		if(bound_check_flags & QB_ARRAY_BOUND_CHECK_ISSET) {
			// see if there's a check done on higher dimension 
			// use the same predicate so that if one check fails, the whole chain fails
			qb_address *predicate_address = qb_obtain_predicate_address(cxt, container_address, TRUE);
			qb_operand operands[4] = { { QB_OPERAND_ADDRESS, { index_address } }, { QB_OPERAND_ADDRESS, { index_limit_address } }, { QB_OPERAND_ADDRESS, { array_offset_address } }, { QB_OPERAND_ADDRESS, { predicate_address } } };
			return qb_obtain_on_demand_value(cxt, &factory_check_array_index_add, operands, 4);
		} else if((bound_check_flags & QB_ARRAY_BOUND_CHECK_WRITE) && can_expand) {
			if(index_address == index_limit_address) {
				// done slightly differently, since we need to put the original size of the array in a temporary variable
				qb_operand operands[3] = { { QB_OPERAND_ADDRESS, { index_limit_address } }, { QB_OPERAND_SEGMENT_SELECTOR, { container_address } }, { QB_OPERAND_ELEMENT_SIZE, { container_address } } };
				return qb_obtain_on_demand_value(cxt, &factory_accommodate_array_push, operands, 3);
			} else {
				qb_operand operands[4] = { { QB_OPERAND_ADDRESS, { index_address } }, { QB_OPERAND_ADDRESS, { index_limit_address } }, { QB_OPERAND_SEGMENT_SELECTOR, { container_address } }, { QB_OPERAND_ELEMENT_SIZE, { container_address }  } };
				return qb_obtain_on_demand_value(cxt, &factory_accommodate_array_index, operands, 4);
			}
		} else if(bound_check_flags & (QB_ARRAY_BOUND_CHECK_WRITE | QB_ARRAY_BOUND_CHECK_READ)) {
			if(array_offset_address == cxt->zero_address) {
				qb_operand operands[2] = { { QB_OPERAND_ADDRESS, { index_address } }, { QB_OPERAND_ADDRESS, { index_limit_address } } };
				return qb_obtain_on_demand_value(cxt, &factory_guard_array_index, operands, 2);
			} else {
				qb_operand operands[3] = { { QB_OPERAND_ADDRESS, { index_address } }, { QB_OPERAND_ADDRESS, { index_limit_address } }, { QB_OPERAND_ADDRESS, { array_offset_address } } };
				return qb_obtain_on_demand_value(cxt, &factory_guard_array_index_add, operands, 3);
			}
		} else {
			return index_address;
		}
	}
}

qb_address * qb_obtain_bound_checked_array_extent(qb_compiler_context *cxt, qb_address *container_address, qb_address *index_address, qb_address *length_address, uint32_t bound_check_flags) {
	qb_address *extent_limit_address = container_address->dimension_addresses[0];
	qb_address *sub_array_size_address = (container_address->dimension_count > 1) ? container_address->array_size_addresses[1] : NULL;

	if(length_address) {
		if(sub_array_size_address) {
			// if everything is constant, perform the calculation now if it's within bounds
			if(IS_IMMUTABLE(index_address) && IS_IMMUTABLE(sub_array_size_address) && IS_IMMUTABLE(extent_limit_address) && IS_IMMUTABLE(length_address)) {
				uint32_t extent = VALUE(U32, index_address) + VALUE(U32, length_address);
				uint32_t extent_limit = VALUE(U32, extent_limit_address);
				if(extent <= extent_limit) {
					uint32_t product = VALUE(U32, length_address) * VALUE(U32, sub_array_size_address);
					return qb_obtain_constant_U32(cxt, product);
				}
			}
			if(bound_check_flags & QB_ARRAY_BOUND_CHECK_READ) {
				qb_operand operands[4] = { { QB_OPERAND_ADDRESS, { index_address } }, { QB_OPERAND_ADDRESS, { length_address } }, { QB_OPERAND_ADDRESS, { extent_limit_address } }, { QB_OPERAND_ADDRESS, { sub_array_size_address } } };
				return qb_obtain_on_demand_value(cxt, &factory_guard_array_extent_multiply, operands, 4);
#ifdef ZEND_DEBUG
			} else {
				qb_debug_abort("Not implemented");
#endif
			}
		} else {
			if(IS_IMMUTABLE(index_address) && IS_IMMUTABLE(extent_limit_address) && IS_IMMUTABLE(length_address)) {
				uint32_t extent = VALUE(U32, index_address) + VALUE(U32, length_address);
				uint32_t extent_limit = VALUE(U32, extent_limit_address);
				if(extent <= extent_limit) {
					return length_address;
				}
			}
			if(bound_check_flags & QB_ARRAY_BOUND_CHECK_READ) {
				qb_operand operands[3] = { { QB_OPERAND_ADDRESS, { index_address } }, { QB_OPERAND_ADDRESS, { length_address } }, { QB_OPERAND_ADDRESS, { extent_limit_address } } };
				return qb_obtain_on_demand_value(cxt, &factory_guard_array_extent, operands, 3);
#ifdef ZEND_DEBUG
			} else {
				qb_debug_abort("Not implemented");
#endif
			}
		}
	} else {
		// the length is the size minus the index
		if(sub_array_size_address) {
			if(IS_IMMUTABLE(index_address) && IS_IMMUTABLE(sub_array_size_address) && IS_IMMUTABLE(extent_limit_address)) {
				uint32_t length = VALUE(U32, extent_limit_address) - VALUE(U32, index_address);
				uint32_t extent_limit = VALUE(U32, extent_limit_address);
				// check for integer overflow
				if(length <= extent_limit) {
					uint32_t product = length * VALUE(U32, sub_array_size_address);
					return qb_obtain_constant_U32(cxt, product);
				}
			}
			if(bound_check_flags & QB_ARRAY_BOUND_CHECK_READ) {
				qb_operand operands[3] = { { QB_OPERAND_ADDRESS, { extent_limit_address } }, { QB_OPERAND_ADDRESS, { index_address } }, { QB_OPERAND_ADDRESS, { sub_array_size_address } } };
				return qb_obtain_on_demand_value(cxt, &factory_guard_array_extent_subtract_multiply, operands, 3);
#ifdef ZEND_DEBUG
			} else {
				qb_debug_abort("Not implemented");
#endif
			}
		} else {
			if(IS_IMMUTABLE(index_address) && IS_IMMUTABLE(extent_limit_address)) {
				uint32_t length = VALUE(U32, extent_limit_address) - VALUE(U32, index_address);
				uint32_t extent_limit = VALUE(U32, extent_limit_address);
				if(length <= extent_limit) {
					return qb_obtain_constant_U32(cxt, length);
				}
			}
			if(bound_check_flags & QB_ARRAY_BOUND_CHECK_READ) {
				qb_operand operands[2] = { { QB_OPERAND_ADDRESS, { extent_limit_address } }, { QB_OPERAND_ADDRESS, { index_address } } };
				return qb_obtain_on_demand_value(cxt, &factory_guard_array_extent_subtract, operands, 2);
#ifdef ZEND_DEBUG
			} else {
				qb_debug_abort("Not implemented");
#endif
			}
		}
	}
	return NULL;
}

qb_address * qb_obtain_bound_checked_array_remainder_length(qb_compiler_context *cxt, qb_address *container_address, qb_address *index_address, uint32_t bound_check_flags) {
	qb_address *extent_limit_address = container_address->dimension_addresses[0];
	if(IS_IMMUTABLE(index_address) && IS_IMMUTABLE(extent_limit_address)) {
		uint32_t length = VALUE(U32, extent_limit_address) - VALUE(U32, index_address);
		uint32_t extent_limit = VALUE(U32, extent_limit_address);
		if(length <= extent_limit) {
			return qb_obtain_constant_U32(cxt, length);
		}
	}
	if(bound_check_flags & QB_ARRAY_BOUND_CHECK_READ) {
		qb_operand operands[2] = { { QB_OPERAND_ADDRESS, { extent_limit_address } }, { QB_OPERAND_ADDRESS, { index_address } } };
		return qb_obtain_on_demand_value(cxt, &factory_guard_array_extent_subtract, operands, 2);
	} else {
		// not implemented
	}
	return NULL;
}

qb_address * qb_obtain_array_element(qb_compiler_context *cxt, qb_address *container_address, qb_address *index_address, uint32_t bound_check_flags) {
	qb_address *result_address;
	uint32_t i;

	if(container_address->dimension_count == 0) {
		return NULL;
	}

	// if the index is to an array element, copy the value to a temporary variable 
	if(index_address->mode == QB_ADDRESS_MODE_ELE) {
		index_address = qb_obtain_scalar_value(cxt, index_address);
	}

	// obtain the expanded index, multiplying it by the sub-array size and adding in the array offset
	index_address = qb_obtain_bound_checked_array_index(cxt, container_address, index_address, bound_check_flags);

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
	result_address->flags &= ~QB_ADDRESS_RESIZABLE;
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
		if(IS_IMMUTABLE(index_address)) {
			// SCA address mode can be used when the index is constant
			result_address->mode = QB_ADDRESS_MODE_SCA;
		} else {
			// otherwise ELE is needed
			result_address->mode = QB_ADDRESS_MODE_ELE;
		}
		result_address->array_size_address = cxt->one_address;
		result_address->array_size_addresses = 
		result_address->dimension_addresses = &result_address->array_size_address;
		result_address->index_alias_schemes = NULL;
	}
	return result_address;
}

qb_address * qb_obtain_array_slice(qb_compiler_context *cxt, qb_address *container_address, qb_address *index_address, qb_address *length_address, uint32_t bound_check_flags) {
	qb_address *result_address, *extent_address, *offset_address;
	uint32_t i;

	if(index_address->mode == QB_ADDRESS_MODE_ELE) {
		index_address = qb_obtain_scalar_value(cxt, index_address);
	}
	if(length_address && length_address->mode == QB_ADDRESS_MODE_ELE) {
		length_address = qb_obtain_scalar_value(cxt, length_address);
	}

	// obtain the slice's extent (i.e. length * sub-array size)
	extent_address = qb_obtain_bound_checked_array_extent(cxt, container_address, index_address, length_address, bound_check_flags);

	offset_address = qb_obtain_bound_checked_array_index(cxt, container_address, index_address, bound_check_flags);

	// see if we have created the address earlier
	for(i = 0; i < cxt->address_alias_count; i++) {
		qb_address *alias = cxt->address_aliases[i];
		if(alias->source_address == container_address) {
			if(alias->array_index_address == index_address && alias->array_size_address == extent_address && alias->dimension_count == container_address->dimension_count) {
				return alias;
			}
		}
	}

	result_address = qb_create_address_alias(cxt, container_address);
	result_address->flags &= ~QB_ADDRESS_RESIZABLE;
	result_address->array_index_address = offset_address;
	result_address->array_size_address = extent_address;
	if(result_address->dimension_count > 1) {
		if(!length_address) {
			length_address = qb_obtain_bound_checked_array_remainder_length(cxt, container_address, index_address, bound_check_flags);
		}
		result_address->array_size_addresses = qb_allocate_address_pointers(cxt->pool, result_address->dimension_count);
		result_address->dimension_addresses = qb_allocate_address_pointers(cxt->pool, result_address->dimension_count);
		result_address->array_size_addresses[0] = extent_address;
		result_address->dimension_addresses[0] = length_address;
		for(i = 1; i < result_address->dimension_count; i++) {
			result_address->array_size_addresses[i] = container_address->array_size_addresses[i];
			result_address->dimension_addresses[i] = container_address->dimension_addresses[i];
		}
		result_address->array_size_address = result_address->array_size_addresses[0];
	} else {
		result_address->array_size_addresses =
		result_address->dimension_addresses = &result_address->array_size_address;
	}

	return result_address;
}

qb_address * qb_retrieve_array_dimensions(qb_compiler_context *cxt, qb_address *address) {
	qb_address *dimension_count_address = qb_obtain_constant_U32(cxt, address->dimension_count);
	qb_variable_dimensions dim = { 1, dimension_count_address };
	qb_address *dimensions_address = qb_obtain_temporary_variable(cxt, QB_TYPE_U32, &dim);
	uint32_t i;
	for(i = 0; i < address->dimension_count; i++) {
		qb_address *index_address = qb_obtain_constant_U32(cxt, i);
		qb_address *src_dimension_address = address->dimension_addresses[i];
		qb_address *dst_dimension_address = qb_obtain_array_element(cxt, dimensions_address, index_address, FALSE);
		qb_retrieve_binary_op_result(cxt, &factory_assign, dst_dimension_address, src_dimension_address);
	}
	return dimensions_address;
}

uint32_t qb_find_index_alias(qb_compiler_context *cxt, qb_index_alias_scheme *scheme, zval *name) {
	const char *alias = Z_STRVAL_P(name);
	uint32_t i;
	for(i = 0; i < scheme->dimension; i++) {
		if(strcmp(scheme->aliases[i], alias) == 0) {
			return i;
		}
	}
	return INVALID_INDEX;
}

uint32_t qb_get_swizzle_mask(qb_compiler_context *cxt, qb_index_alias_scheme *scheme, zval *name) {
	uint32_t i, j, mask = 0;
	if(scheme->dimension > 8) {
		return INVALID_INDEX;
	}
	for(i = 0; i < scheme->dimension; i++) {
		// all names have be single-character
		if(scheme->alias_lengths[i] != 1) {
			return INVALID_INDEX;
		}
	}
	for(j = 0; j < (uint32_t) Z_STRLEN_P(name); j++) {
		char c1 = Z_STRVAL_P(name)[j];
		uint32_t index = INVALID_INDEX;
		for(i = 0; i < scheme->dimension; i++) {
			char c2 = scheme->aliases[i][0];
			if(c1 == c2) {
				index = i;
			}
		}
		if(index == INVALID_INDEX) {
			return INVALID_INDEX;
		} else {
			mask |= index << (j * 3);
		}
	}
	return mask;
}

qb_address * qb_obtain_named_element(qb_compiler_context *cxt, qb_address *container_address, zval *name, uint32_t bound_check_flags) {
	 if(!IS_SCALAR(container_address)) {
		if(container_address->index_alias_schemes && container_address->index_alias_schemes[0]) {
			uint32_t index = qb_find_index_alias(cxt, container_address->index_alias_schemes[0], name);
			if(index != INVALID_INDEX) {
				qb_address *index_address = qb_obtain_constant_U32(cxt, index);
				qb_address *value_address = qb_obtain_array_element(cxt, container_address, index_address, bound_check_flags);
				return value_address;
			} else {
				uint32_t swizzle_mask = qb_get_swizzle_mask(cxt, container_address->index_alias_schemes[0], name);
				uint32_t len = Z_STRLEN_P(name);
				qb_operand operands[3];
				qb_variable_dimensions dim = { 1, qb_obtain_constant_U32(cxt, len) };
				qb_address *value_address = qb_obtain_temporary_variable(cxt, container_address->type, &dim);				
				if(bound_check_flags & QB_ARRAY_BOUND_CHECK_READ) {
					operands[0].type = QB_OPERAND_ADDRESS;
					operands[0].address = value_address;
					operands[1].type = QB_OPERAND_ADDRESS;
					operands[1].address = container_address;
					operands[2].type = QB_OPERAND_NUMBER;
					operands[2].number = swizzle_mask;
					value_address->expression = qb_get_on_demand_expression(cxt, &factory_gather, operands, 3);
				} else {
					operands[0].type = QB_OPERAND_ADDRESS;
					operands[0].address = container_address;
					operands[1].type = QB_OPERAND_ADDRESS;
					operands[1].address = value_address;
					operands[2].type = QB_OPERAND_NUMBER;
					operands[2].number = swizzle_mask;
					value_address->expression = qb_get_on_demand_expression(cxt, &factory_scatter, operands, 3);
					value_address->expression->flags &= ~QB_EXPR_EXECUTE_BEFORE;
					value_address->expression->flags |= QB_EXPR_EXECUTE_AFTER;
				}
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
					if(parent_type != QB_TYPE_ANY && parent_type != QB_TYPE_UNKNOWN) {
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
			} else if(result_prototype->address_flags & QB_ADDRESS_STRING) {
				// we're probably echoing it
				expr_type = QB_TYPE_U08;
			}
		}
		result_prototype->final_type = expr_type;
	}
}

void qb_create_on_demand_op(qb_compiler_context *cxt, qb_op *qop, uint32_t flags);

int32_t qb_create_op(qb_compiler_context *cxt, void *factory, qb_primitive_type expr_type, uint32_t flags, qb_operand *operands, uint32_t operand_count, qb_operand *result, uint32_t *jump_target_indices, uint32_t jump_target_count, int32_t result_used) {
	qb_op_factory *f = factory;
	qb_opcode opcode = QB_NOP;
	qb_op *qop;
	uint32_t op_flags;
	uint32_t i;

	// get the opcode for the operands 
	// at this point, the operands should have the correct type
	if(f->select_opcode) {
		if(!f->select_opcode(cxt, f, expr_type, flags, operands, operand_count, result, &opcode)) {
			return FALSE;
		}
	}
	op_flags = qb_get_op_flags(opcode);

	// make it a NOP if the result isn't used and the op has no side effect
	if(!(result_used || (op_flags & (QB_OP_JUMP | QB_OP_BRANCH | QB_OP_EXIT)) || (f->result_flags & QB_RESULT_HAS_SIDE_EFFECT))) {
		opcode = QB_NOP;
	}

	// create the op
	qop = qb_allocate_op(cxt->pool);
	qop->flags = op_flags;
	qop->opcode = opcode;
	qop->line_id = cxt->line_id;

	if(opcode != QB_NOP) {
		// figure out how many operands there will be 
		qop->operand_count = qb_get_operand_count(opcode);

		// move the operands into the op
		qop->operands = qb_allocate_operands(cxt->pool, qop->operand_count);
		if(f->transfer_operands) {
			if(!f->transfer_operands(cxt, f, flags, operands, operand_count, result, qop->operands, qop->operand_count)) {
				return FALSE;
			}
		}

		// add the ops for calculating on-demand values 
		qb_create_on_demand_op(cxt, qop, QB_EXPR_EXECUTE_BEFORE);

		// add the op
		qb_add_op(cxt, qop);

		// add the ops for calculating on-demand values 
		qb_create_on_demand_op(cxt, qop, QB_EXPR_EXECUTE_AFTER);

		// copy the jump target indices
		qop->jump_target_count = jump_target_count;
		if(jump_target_count > 0) {
			qop->jump_target_indices = qb_allocate_indices(cxt->pool, jump_target_count);
			for(i = 0; i < jump_target_count; i++) {
				qop->jump_target_indices[i] = jump_target_indices[i];
			}
		}

		for(i = 0; i < qop->operand_count; i++) {
			// mark result address as writable
			if(qop->operands[i].type == QB_OPERAND_ADDRESS) {
				if(qb_is_operand_write_target(qop->opcode, i)) {
					qb_address *address = qop->operands[i].address;
					qb_mark_as_writable(cxt, address);
					qb_invalidate_on_demand_expressions(cxt, address);
				}
			}
		}

		if(cxt->debugger_present) {
			if(debug_compatibility_mode) {
				USE_TSRM
				if(QB_G(allow_debugger_inspection)) {
					for(i = 0; i < qop->operand_count; i++) {
						// sync the variable a
						if(qop->operands[i].type == QB_OPERAND_ADDRESS) {
							if(qb_is_operand_write_target(qop->opcode, i)) {
								uint32_t variable_index = qb_find_variable_index(cxt, qop->operands[i].address);
								if(variable_index != INVALID_INDEX) {
									qb_operand operand;
									operand.address = qb_obtain_constant_U32(cxt, variable_index);
									operand.type = QB_OPERAND_ADDRESS;
									qb_create_op(cxt, &factory_synchronize_debug_variable, QB_TYPE_VOID, 0, &operand, 1, NULL, NULL, 0, FALSE);
								}
							}
						}
					}
				}
			}
		}

		// set function flags
		if(f->set_function_flags) {
			if(!f->set_function_flags(cxt, f, operands, operand_count, result)) {
				return FALSE;
			}
		}
	} else {
		// it's a nop
		qb_add_op(cxt, qop);
	}
	return TRUE;
}

int32_t qb_execute_op(qb_compiler_context *cxt, void *factory, qb_primitive_type expr_type, uint32_t flags, qb_operand *operands, uint32_t operand_count, qb_operand *result) {
	USE_TSRM
	int8_t instructions[256];
	qb_op *ops[2], target_op, ret_op;
	qb_opcode opcode;
	qb_function _qfunc, *qfunc = &_qfunc;
	qb_compiler_context _compiler_cxt, *compiler_cxt = &_compiler_cxt;
	qb_encoder_context _encoder_cxt, *encoder_cxt = &_encoder_cxt;
	qb_interpreter_context _interpreter_cxt, *interpreter_cxt = &_interpreter_cxt;
	qb_op_factory *f = factory;
	ALLOCA_FLAG(use_heap);

	if(!f->select_opcode(cxt, f, expr_type, flags, operands, operand_count, result, &opcode)) {
		return FALSE;
	}

	target_op.opcode = opcode;
	target_op.flags = qb_get_op_flags(target_op.opcode);
	target_op.operand_count = qb_get_operand_count(target_op.opcode);
	target_op.operands = do_alloca(sizeof(qb_operand) * target_op.operand_count, use_heap);
	target_op.jump_target_indices = NULL;
	target_op.jump_target_count = 0;
	target_op.instruction_offset = 0;
	target_op.line_id = 0;

	if(f->transfer_operands) {
		if(!f->transfer_operands(cxt, f, flags, operands, operand_count, result, target_op.operands, target_op.operand_count)) {
			return FALSE;
		}
	}

	ret_op.opcode = QB_RET;
	ret_op.flags = QB_OP_EXIT;
	ret_op.operand_count = 0;
	ret_op.operands = NULL;
	ret_op.jump_target_indices = NULL;
	ret_op.jump_target_count = 0;
	ret_op.instruction_offset = 0;
	ret_op.line_id = 0;

	*compiler_cxt = *cxt;
	ops[0] = &target_op;
	ops[1] = &ret_op;
	compiler_cxt->ops = ops;
	compiler_cxt->op_count = 2;
	*qfunc = cxt->function_prototype;
	qfunc->instructions = qfunc->instruction_start = instructions;
	qfunc->in_use = FALSE;
	
	qb_resolve_address_modes(compiler_cxt);
	qb_initialize_encoder_context(encoder_cxt, compiler_cxt, FALSE TSRMLS_CC);
	qb_set_instruction_offsets(encoder_cxt);
	qb_encode_instruction_stream(encoder_cxt, instructions);
	qb_free_encoder_context(encoder_cxt);

	qb_initialize_interpreter_context(interpreter_cxt, qfunc, NULL TSRMLS_CC);
	qb_main(interpreter_cxt);
	qb_free_interpreter_context(interpreter_cxt);

	free_alloca(target_op.operands, use_heap);
	return TRUE;
}

static void qb_update_on_demand_result_no_recursion(qb_compiler_context *cxt, qb_address *address, uint32_t flags) {
	if(address->expression) {
		qb_expression *expr = address->expression;
		if(expr->flags & flags) {
			qb_primitive_type expr_type = (expr->result->type == QB_OPERAND_ADDRESS) ? expr->result->address->type : QB_TYPE_VOID;
			uint32_t flags = (expr->result->type == QB_OPERAND_ADDRESS) ? expr->result->address->flags : 0;
			address->expression = NULL;
			// set the flag first, in case the op invalidates itself
			expr->flags |= QB_EXPR_RESULT_IS_STILL_VALID;
			qb_create_op(cxt, expr->op_factory, expr_type, flags, expr->operands, expr->operand_count, expr->result, NULL, 0, TRUE);
		}
	}
}

void qb_update_on_demand_result(qb_compiler_context *cxt, qb_address *address, uint32_t flags) {
	if(address) {
		qb_update_on_demand_result_no_recursion(cxt, address, flags);
		qb_update_on_demand_result_no_recursion(cxt, address->array_index_address, flags);
		qb_update_on_demand_result_no_recursion(cxt, address->array_size_address, flags);
	}
}

void qb_create_on_demand_op(qb_compiler_context *cxt, qb_op *qop, uint32_t flags) {
	uint32_t i;
	if(qop->opcode != QB_FCALL_U32_U32_U32) {
		for(i = 0; i < qop->operand_count; i++) {
			qb_operand *operand = &qop->operands[i];
			if(operand->type == QB_OPERAND_ADDRESS) {
				qb_update_on_demand_result(cxt, operand->address, flags);
			}
		}
	} else {
		// function arguments are referenced by indices
		qb_address *argument_indices_address = qop->operands[1].address;
		qb_address *retvar_index_address = qop->operands[2].address;
		uint32_t *argument_indices = ARRAY(U32, argument_indices_address);
		uint32_t argument_count = ARRAY_SIZE(argument_indices_address);
		uint32_t retvar_index = VALUE(U32, retvar_index_address);
	
		for(i = 0; i < argument_count; i++) {
			uint32_t argument_index = argument_indices[i];
			qb_variable *qvar = cxt->variables[argument_index];
			qb_update_on_demand_result(cxt, qvar->address, flags);
		}
		if(retvar_index != INVALID_INDEX) {
			qb_variable *qvar = cxt->variables[retvar_index];
			qb_update_on_demand_result(cxt, qvar->address, flags);
		}
	}
}

int32_t qb_is_constant_expression(qb_compiler_context *cxt, qb_operand *operands, uint32_t operand_count) {
	uint32_t i;
	for(i = 0; i < operand_count; i++) {
		qb_operand *operand = &operands[i];
		if(operand->type == QB_OPERAND_ADDRESS) {
			if(!IS_IMMUTABLE(operand->address)) {
				return FALSE;
			}
		} else if(operand->type == QB_OPERAND_ARRAY_INITIALIZER) {
			if(operand->array_initializer->flags & QB_ARRAY_INITIALIZER_VARIABLE_ELEMENTS) {
				return FALSE;
			}
		} else if(operand->type == QB_OPERAND_RESULT_PROTOTYPE) {
			if(!(operand->result_prototype->address_flags & QB_ADDRESS_CONSTANT)) {
				return FALSE;
			}
		}
	}
	return TRUE;
}

#ifdef ZEND_DEBUG
void qb_validate_address(qb_compiler_context *cxt, qb_address *address) {
	if(!address) {
		qb_debug_abort("Address is null");
	}
	if(address->dimension_count > 0) {
		uint32_t i;
		if(!address->dimension_addresses) {
			qb_debug_abort("Array address doesn't have dimensions");
		}
		if(!address->array_size_addresses) {
			qb_debug_abort("Array address doesn't have sizes");
		}
		if(!address->array_size_address) {
			qb_debug_abort("Array address doesn't have a size");
		}
		qb_validate_address(cxt, address->array_size_address);
		for(i = 0; i < address->dimension_count; i++) {
			qb_validate_address(cxt, address->dimension_addresses[i]);
			qb_validate_address(cxt, address->array_size_addresses[i]);
		}
	} else {
		if(address->array_size_address != cxt->one_address) {
			qb_debug_abort("Scalar address has incorrect size");
		}
	}
}
#endif

int32_t qb_produce_op(qb_compiler_context *cxt, void *factory, qb_operand *operands, uint32_t operand_count, qb_operand *result, uint32_t *jump_target_indices, uint32_t jump_target_count, qb_result_prototype *result_prototype) {
	qb_op_factory *f = factory;
	if(f->produce_composite) {
		return f->produce_composite(cxt, factory, operands, operand_count, result, jump_target_indices, jump_target_count, result_prototype);
	} else {
		qb_primitive_type expr_type = QB_TYPE_VOID;
		int32_t result_used = (!result || result->type != QB_OPERAND_NONE);
		uint32_t address_flags = 0;
		uint32_t i;

		switch(cxt->stage) {
			case QB_STAGE_VARIABLE_INITIALIZATION: {
				if(f->resolve_flags) {
					if(!f->resolve_flags(cxt, f, operands, operand_count, &address_flags)) {
						return FALSE;
					}
				}
				if(f->resolve_type) {
					if(!f->resolve_type(cxt, f, address_flags, operands, operand_count, &expr_type)) {
						return FALSE;
					}
				}

				if(f->coerce_operands) {
					if(!f->coerce_operands(cxt, f, expr_type, address_flags, operands, operand_count)) {
						return FALSE;
					}
				}

				if(f->set_final_result) {
					if(!f->set_final_result(cxt, f, expr_type, address_flags, operands, operand_count, result, result_prototype)) {
						return FALSE;
					}
				}

				qb_create_op(cxt, factory, expr_type, address_flags, operands, operand_count, result, jump_target_indices, jump_target_count, TRUE);
			}	break;
			case QB_STAGE_RESULT_TYPE_RESOLUTION: {
				if(f->resolve_flags) {
					if(!f->resolve_flags(cxt, f, operands, operand_count, &address_flags)) {
						return FALSE;
					}
				}

				// determine the expression type
				if(f->resolve_type) {
					if(!f->resolve_type(cxt, f, address_flags, operands, operand_count, &expr_type)) {
						return FALSE;
					}
					if(result_prototype) {
						// indicate in the prototype that the expression has this type
						result_prototype->preliminary_type = expr_type;
						if(!(f->coercion_flags & QB_COERCE_TO_LVALUE_TYPE) || result->type == QB_OPERAND_NONE) {
							// as the result doesn't depend on the context (or there is no context at all)
							// we're certain about the type 
							result_prototype->final_type = expr_type;
						}
						result_prototype->address_flags = address_flags;
						result_prototype->coercion_flags = f->coercion_flags;
					}
				}

				if(f->coerce_operands) {
					if(!f->coerce_operands(cxt, f, expr_type, address_flags, operands, operand_count)) {
						return FALSE;
					}
				}

				if(f->link_results) {
					if(!f->link_results(cxt, f, address_flags, operands, operand_count, result_prototype)) {
						return FALSE;
					}
				}

				if(f->set_preliminary_result) {
					if(!f->set_preliminary_result(cxt, f, expr_type, address_flags, operands, operand_count, result, result_prototype)) {
						return FALSE;
					}
				}

				if(result_prototype) {
					if(f->result_flags & QB_RESULT_FROM_PURE_FUNCTION) {
						// see if the expression is constant
						if(qb_is_constant_expression(cxt, operands, operand_count)) {
							result_prototype->address_flags &= ~QB_ADDRESS_TEMPORARY;
							result_prototype->address_flags |= QB_ADDRESS_CONSTANT;
						}
					}
				}

				// mark the jump targets
				for(i = 0; i < jump_target_count; i++) {
					qb_add_jump_target(cxt, jump_target_indices[i]);
				}
			}	break;
			case QB_STAGE_CONSTANT_EXPRESSION_EVALUATION: {
				if(result_prototype && (result_prototype->address_flags & QB_ADDRESS_CONSTANT) && !(f->result_flags & QB_RESULT_HAS_SIDE_EFFECT)) {
					qb_finalize_result_prototype(cxt, result_prototype);
					expr_type = result_prototype->final_type;
					address_flags = result_prototype->address_flags;
					
					if(f->coerce_operands) {
						if(!f->coerce_operands(cxt, f, expr_type, address_flags, operands, operand_count)) {
							return FALSE;
						}
					}

					if(f->validate_operands) {
						if(!f->validate_operands(cxt, f, expr_type, address_flags, operands, operand_count, result_prototype->destination)) {
							return FALSE;
						}
					}

					if(f->set_final_result) {
						if(!f->set_final_result(cxt, f, expr_type, address_flags, operands, operand_count, result, result_prototype)) {
							return FALSE;
						}
					}

					if(result->type == QB_OPERAND_ADDRESS) {
						if(f->select_opcode) {
							qb_execute_op(cxt, f, expr_type, address_flags, operands, operand_count, result);
						}
						result_prototype->constant_result_address = result->address;
					}
				}
			}	break;
			case QB_STAGE_OPCODE_TRANSLATION: {
				// use the result from the previous stage if it's available 
				if(result_prototype) {
					if(result_prototype->constant_result_address) {
						// the result is a constant expression evaluated earlier
						result->address = result_prototype->constant_result_address;
						result->type = QB_OPERAND_ADDRESS;
						return TRUE;
					} else {
						qb_finalize_result_prototype(cxt, result_prototype);
						expr_type = result_prototype->final_type;
						address_flags = result_prototype->address_flags;
					}
				} else {
					if(f->resolve_flags) {
						if(!f->resolve_flags(cxt, f, operands, operand_count, &address_flags)) {
							return FALSE;
						}
					}
					if(f->resolve_type) {
						if(!f->resolve_type(cxt, f, address_flags, operands, operand_count, &expr_type)) {
							return FALSE;
						}
					}
				}

				// perform type coercion on operands
				if(f->coerce_operands) {
					// note that the handler might not necessarily convert all the operands to expr_type
					if(!f->coerce_operands(cxt, f, expr_type, address_flags, operands, operand_count)) {
						return FALSE;
					}
				}

				// perform validation
				if(f->validate_operands) {
					if(!f->validate_operands(cxt, f, expr_type, address_flags, operands, operand_count, (result_prototype) ? result_prototype->destination : NULL)) {
						return FALSE;
					}
				}

				// then, assign result to result object
				if(f->set_final_result) {
					if(expr_type == QB_TYPE_ANY) {
						// couldn't figure it out the first time around
						// after type coercion, we should know what the expresion is
						if(f->resolve_type) {
							if(!f->resolve_type(cxt, f, address_flags, operands, operand_count, &expr_type)) {
								return FALSE;
							}
						}
					}
					if(!f->set_final_result(cxt, f, expr_type, address_flags, operands, operand_count, result, result_prototype)) {
						return FALSE;
					}

					if(result->type == QB_OPERAND_ADDRESS) {
						if(!result->address) {
							result->type = QB_OPERAND_EMPTY;
							qb_report_internal_error(cxt->line_id, "Null address");
							return FALSE;
						}
					}
				}

				// create the op
				qb_create_op(cxt, factory, expr_type, address_flags, operands, operand_count, result, jump_target_indices, jump_target_count, result_used);

				// unlock the operands after the op is created
				for(i = 0; i < operand_count; i++) {
					qb_operand *operand = &operands[i];
					if(operand->type == QB_OPERAND_ADDRESS) {
						qb_unlock_address(cxt, operand->address);
					}
				}
				if(result && result->type == QB_OPERAND_ADDRESS) {
					qb_unlock_address(cxt, result->address);
				}
			}	break;
		}
	}
	return TRUE;
}

void qb_resolve_jump_targets(qb_compiler_context *cxt) {
	uint32_t i, j;
	for(i = 0; i < cxt->jump_target_count; i++) {
		qb_jump_target *target = &cxt->jump_targets[i];
		uint32_t qop_index = qb_get_translated_op_index(cxt, target->jump_target_index);
		if(qop_index < cxt->op_count) {
			qb_op *qop = cxt->ops[qop_index];
			qop->flags |= QB_OP_JUMP_TARGET;
		}
	}
	for(i = 0; i < cxt->op_count; i++) {
		qb_op *qop = cxt->ops[i];
		if(!(qop->flags & QB_OP_JUMP_TARGETS_RESOLVED)) {
			for(j = 0; j < qop->jump_target_count; j++) {
				uint32_t qop_index = qb_get_translated_op_index(cxt, qop->jump_target_indices[j]);
				// skip to the next op if it's pointing to a NOP
				while(cxt->ops[qop_index]->opcode == QB_NOP) {
					cxt->ops[qop_index]->flags &= ~QB_OP_JUMP_TARGET;
					qop_index++;
					cxt->ops[qop_index]->flags |= QB_OP_JUMP_TARGET;
				}
				qop->jump_target_indices[j] = qop_index;
			}
			qop->flags |= QB_OP_JUMP_TARGETS_RESOLVED;
		}
	}
}

static qb_address * qb_promote_address_mode(qb_compiler_context *cxt, qb_address_mode mode, qb_address *original_address) {
	qb_address *address = qb_create_address_alias(cxt, original_address);
	address->mode = mode;
	return address;
}

void qb_resolve_address_modes(qb_compiler_context *cxt) {
	uint32_t i, j;
	for(i = 0; i < cxt->op_count; i++) {
		qb_op *qop = cxt->ops[i];

		if(qop->opcode != QB_NOP) {
			int32_t operands_are_valid = TRUE;
			if(qop->flags & (QB_OP_VERSION_AVAILABLE_ELE | QB_OP_VERSION_AVAILABLE_MIO)) {
				qb_address_mode required_address_mode = QB_ADDRESS_MODE_SCA;
				for(j = 0; j < qop->operand_count; j++) {
					qb_operand *operand = &qop->operands[j];
					if(operand->type == QB_OPERAND_ADDRESS) {
						qb_address_mode operand_address_mode = qb_get_operand_address_mode(qop->opcode, j);
						if(operand->address->mode > operand_address_mode) {
							// need use to a higher address mode
							required_address_mode = operand->address->mode;
						}
					}
				}

				if(required_address_mode != QB_ADDRESS_MODE_SCA) {
					if(required_address_mode == QB_ADDRESS_MODE_ELE) {
						if(qop->flags & QB_OP_VERSION_AVAILABLE_ELE) {
							// opcode is one away
							qop->opcode += 1;
						}
					} else if(required_address_mode == QB_ADDRESS_MODE_ARR) {
						if(qop->flags & QB_OP_VERSION_AVAILABLE_MIO) {
							if(qop->flags & QB_OP_VERSION_AVAILABLE_ELE) {
								// opcode is two away
								qop->opcode += 2;
							} else {
								// opcode is just one away
								qop->opcode += 1;
							}
						}
					}
					// get new set of flags
					qop->flags = qb_get_op_flags(qop->opcode) | (qop->flags & QB_OP_COMPILE_TIME_FLAGS);
				}
			}

			// set the addresses to the correct mode
			for(j = 0; j < qop->operand_count; j++) {
				qb_operand *operand = &qop->operands[j];
				if(operand->type == QB_OPERAND_ADDRESS) {
					qb_address_mode operand_address_mode = qb_get_operand_address_mode(qop->opcode, j);
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
					} else {
						operands_are_valid = FALSE;
						break;
					}
				}
			}
#ifdef ZEND_DEBUG
			if(!operands_are_valid) {
				qb_debug_abort("invalid operands");
			}
#endif
		}
	}
}

void qb_resolve_reference_counts(qb_compiler_context *cxt) {
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
					if(address->mode == QB_ADDRESS_MODE_ELE || address->mode == QB_ADDRESS_MODE_ARR) {
						qb_memory_segment *index_segment = &cxt->storage->segments[address->array_index_address->segment_selector];
						if(!(index_segment->flags & QB_SEGMENT_PREALLOCATED)) {
							index_segment->reference_count++;
						}
						if(address->mode == QB_ADDRESS_MODE_ARR) {
							qb_memory_segment *size_segment = &cxt->storage->segments[address->array_size_address->segment_selector];
							if(!(size_segment->flags & QB_SEGMENT_PREALLOCATED)) {
								size_segment->reference_count++;
							}
						}
					}
				}
			}
		}
	}
}

int32_t qb_check_thread_safety_in_range(qb_compiler_context *cxt, uint32_t start_index, uint32_t end_index, int32_t forked) {
	uint32_t i, j;
	for(i = start_index; i <= end_index; i++) {
		qb_op *qop = cxt->ops[i];
		if(qop->flags & QB_OP_CHECKED) {
			break;
		}
		qop->flags |= QB_OP_CHECKED;
		if(forked) {
			if(qop->opcode == QB_RET) {
				break;
			} else if(qop->opcode == QB_SPOON) {
				forked = FALSE;
			} else if(qop->opcode == QB_FORK_U32) {
				qb_report_fork_in_fork_exception(qop->line_id);
				return FALSE;
			} else {
				if(qop->flags & QB_OP_NOT_THREAD_SAFE) {
					// make sure none of the operands are shared
					for(j = 0; j < qop->operand_count; j++) {
						qb_operand *operand = &qop->operands[j];
						if(operand->type == QB_OPERAND_ADDRESS) {
							if(IS_SHARED(operand->address)) {
								if(qb_is_operand_write_target(qop->opcode, j)) {
									qb_report_resize_in_fork_exception(qop->line_id);
									return FALSE;
								}
							}
						}
					}
				}
			}
			for(j = 0; j < qop->jump_target_count; j++) {
				if(!qb_check_thread_safety_in_range(cxt, qop->jump_target_indices[j], end_index, forked)) {
					return FALSE;
				}
			}
		} else {
			if(qop->opcode == QB_FORK_U32) {
				forked = TRUE;
			}
		}
	}
	return TRUE;
}

int32_t qb_check_thread_safety(qb_compiler_context *cxt) {
	return qb_check_thread_safety_in_range(cxt, 0, cxt->op_count - 1, FALSE);
}

void qb_initialize_compiler_context(qb_compiler_context *cxt, qb_data_pool *pool, qb_function_declaration *function_decl, uint32_t dependency_index, uint32_t max_dependency_index TSRMLS_DC) {
	uint32_t zero = 0;

	memset(cxt, 0, sizeof(qb_compiler_context));
	cxt->pool = pool;
	if(function_decl) {
		cxt->function_flags = function_decl->flags;
		if(QB_G(compile_to_native) && !(function_decl->flags & QB_FUNCTION_NEVER_NATIVE)) {
			cxt->function_flags |= QB_FUNCTION_NATIVE_IF_POSSIBLE;
		}
		if(QB_G(allow_debug_backtrace)) {
			cxt->function_flags |= QB_FUNCTION_GO_THRU_ZEND;
		}

		cxt->function_declaration = function_decl;
		cxt->zend_op_array = function_decl->zend_op_array;

#ifdef ZEND_ACC_GENERATOR
		if(cxt->zend_op_array->fn_flags & ZEND_ACC_GENERATOR) {
			cxt->function_flags |= QB_FUNCTION_GENERATOR;
		}
#endif
#ifdef ZEND_ACC_CLOSURE
		if(cxt->zend_op_array->fn_flags & ZEND_ACC_CLOSURE) {
			cxt->function_flags |= QB_FUNCTION_CLOSURE;
		}
#endif
	}
	SAVE_TSRMLS

	qb_attach_new_array(pool, (void **) &cxt->op_translation_table, &cxt->op_translation_table_size, sizeof(uint32_t), 64);
	qb_attach_new_array(pool, (void **) &cxt->variables, &cxt->variable_count, sizeof(qb_variable *), 16);
	qb_attach_new_array(pool, (void **) &cxt->ops, &cxt->op_count, sizeof(qb_op *), 256);
	qb_attach_new_array(pool, (void **) &cxt->constant_scalars, &cxt->constant_scalar_count, sizeof(qb_address *), 64);
	qb_attach_new_array(pool, (void **) &cxt->writable_scalars, &cxt->writable_scalar_count, sizeof(qb_address *), 64);
	qb_attach_new_array(pool, (void **) &cxt->constant_arrays, &cxt->constant_array_count, sizeof(qb_address *), 16);
	qb_attach_new_array(pool, (void **) &cxt->writable_arrays, &cxt->writable_array_count, sizeof(qb_address *), 16);
	qb_attach_new_array(pool, (void **) &cxt->address_aliases, &cxt->address_alias_count, sizeof(qb_address *), 64);
	qb_attach_new_array(pool, (void **) &cxt->on_demand_expressions, &cxt->on_demand_expression_count, sizeof(qb_address *), 64);

	// only set up segments for scalars and fixed-length arrays initially
	cxt->storage = emalloc(sizeof(qb_storage));
	cxt->storage->flags = 0;
	cxt->storage->segment_count = QB_SELECTOR_ARRAY_START;
	cxt->storage->segments = emalloc(sizeof(qb_memory_segment) * cxt->storage->segment_count);
	memset(cxt->storage->segments, 0, sizeof(qb_memory_segment) * cxt->storage->segment_count);

	cxt->storage->segments[QB_SELECTOR_CONSTANT_SCALAR].flags = QB_SEGMENT_PREALLOCATED;
	cxt->storage->segments[QB_SELECTOR_STATIC_SCALAR].flags = QB_SEGMENT_PREALLOCATED;
	cxt->storage->segments[QB_SELECTOR_SHARED_SCALAR].flags = QB_SEGMENT_PREALLOCATED | QB_SEGMENT_CLEAR_ON_CALL | QB_SEGMENT_SEPARATE_ON_REENTRY;
	cxt->storage->segments[QB_SELECTOR_LOCAL_SCALAR].flags = QB_SEGMENT_PREALLOCATED | QB_SEGMENT_CLEAR_ON_CALL | QB_SEGMENT_SEPARATE_ON_FORK | QB_SEGMENT_SEPARATE_ON_REENTRY;
	cxt->storage->segments[QB_SELECTOR_TEMPORARY_SCALAR].flags = QB_SEGMENT_PREALLOCATED | QB_SEGMENT_SEPARATE_ON_FORK | QB_SEGMENT_SEPARATE_ON_REENTRY;
	cxt->storage->segments[QB_SELECTOR_CONSTANT_ARRAY].flags = QB_SEGMENT_PREALLOCATED;
	cxt->storage->segments[QB_SELECTOR_STATIC_ARRAY].flags = QB_SEGMENT_PREALLOCATED;
	cxt->storage->segments[QB_SELECTOR_SHARED_ARRAY].flags = QB_SEGMENT_PREALLOCATED | QB_SEGMENT_CLEAR_ON_CALL | QB_SEGMENT_SEPARATE_ON_REENTRY;
	cxt->storage->segments[QB_SELECTOR_LOCAL_ARRAY].flags = QB_SEGMENT_PREALLOCATED | QB_SEGMENT_CLEAR_ON_CALL | QB_SEGMENT_SEPARATE_ON_FORK | QB_SEGMENT_SEPARATE_ON_REENTRY;
	cxt->storage->segments[QB_SELECTOR_TEMPORARY_ARRAY].flags = QB_SEGMENT_PREALLOCATED | QB_SEGMENT_SEPARATE_ON_FORK | QB_SEGMENT_SEPARATE_ON_REENTRY;

	// make sure these constants exist
	cxt->zero_address = qb_obtain_constant_U32(cxt, 0);
	cxt->one_address = qb_obtain_constant_U32(cxt, 1);
	cxt->zero_address->array_index_address = cxt->zero_address;
	cxt->zero_address->array_size_address = cxt->one_address;
	cxt->one_address->array_index_address = cxt->zero_address;
	cxt->one_address->array_size_address = cxt->one_address;
	cxt->false_address = qb_obtain_constant_S32(cxt, 0);
	cxt->false_address->flags |= QB_ADDRESS_BOOLEAN;
	cxt->true_address = qb_obtain_constant_S32(cxt, 1);
	cxt->true_address->flags |= QB_ADDRESS_BOOLEAN;
	cxt->empty_array = qb_create_constant_array(cxt, QB_TYPE_I08, &zero, 1);

	cxt->dependency_index = dependency_index;
	cxt->function_flags |= QB_FUNCTION_INLINEABLE;
	if(max_dependency_index > 1) {
		cxt->dependencies = ecalloc(max_dependency_index, sizeof(int8_t));
	}

	if(cxt->zend_op_array) {
		cxt->source_file_id = qb_get_source_file_id(cxt->zend_op_array->filename TSRMLS_CC);
		cxt->source_file_line_number = cxt->zend_op_array->line_start;
		cxt->line_id = LINE_ID(cxt->source_file_id, cxt->source_file_line_number);
	}
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
	if(cxt->dependencies) {
		efree(cxt->dependencies);
	}
	if(cxt->external_code) {
		qb_free_external_code(cxt);
	}

	for(i = 0; i < cxt->compile_time_result_count; i++) {
		zval **compile_time_result = &cxt->compile_time_results[i];
		zval_ptr_dtor(compile_time_result);
	}
	qb_destroy_array((void **) &cxt->compile_time_results);
}

int32_t qb_load_external_code(qb_compiler_context *cxt, const char *import_path) {
	USE_TSRM
	php_stream *stream;

	// set active op array to the function to whom the code belong, so that relative paths are resolved correctly
	zend_op_array *active_op_array = EG(active_op_array);
	zend_op_array *target_op_array = (cxt->function_declaration) ? cxt->function_declaration->zend_op_array : NULL;
	if(target_op_array) {
		EG(active_op_array) = target_op_array;
	}
	stream = php_stream_open_wrapper_ex((char *) import_path, "rb", USE_PATH | ENFORCE_SAFE_MODE | REPORT_ERRORS, NULL, NULL);
	EG(active_op_array) = active_op_array;

	if(stream) {
		char *data = NULL;
		size_t len = php_stream_copy_to_mem(stream, &data, PHP_STREAM_COPY_ALL, FALSE);
		php_stream_close(stream);

		if(len > 0) {
			cxt->external_code = data;
			cxt->external_code_length = (uint32_t) len;
		}
	}
	if(!cxt->external_code && target_op_array) {
		qb_report_external_code_load_failure_exception(cxt->line_id, import_path);
		return FALSE;
	}
	return TRUE;
}

void qb_free_external_code(qb_compiler_context *cxt) {
	if(cxt->external_code) {
		efree(cxt->external_code);
		cxt->external_code = NULL;
		cxt->external_code_length = 0;
	}
}

void qb_open_diagnostic_loop(qb_compiler_context *cxt) {
	cxt->stage = QB_STAGE_OPCODE_TRANSLATION;
}

#define DIAGNOSTIC_ITERATION	500000

void qb_close_diagnostic_loop(qb_compiler_context *cxt) {
	qb_operand iteration, counter;
	uint32_t jump_target_indices[2];

	iteration.address = qb_obtain_constant_U32(cxt, DIAGNOSTIC_ITERATION);
	iteration.type = QB_OPERAND_ADDRESS;
	counter.address = qb_create_writable_scalar(cxt, QB_TYPE_U32);
	counter.type = QB_OPERAND_ADDRESS;
	jump_target_indices[0] = JUMP_TARGET_INDEX(0, 0);
	jump_target_indices[1] = JUMP_TARGET_INDEX(0, cxt->op_count + 1);

	qb_create_op(cxt, &factory_loop, QB_TYPE_VOID, 0, &iteration, 1, &counter, jump_target_indices, 2, FALSE);
	qb_create_op(cxt, &factory_leave, QB_TYPE_VOID, 0, NULL, 0, NULL, NULL, 0, FALSE);
}

void qb_create_diagnostic_loop(qb_compiler_context *cxt, qb_diagnostic_type test_type) {
	qb_address *value1_address, *value2_address, *intermediate_address;
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
				uint32_t i, dimension = 4;
				float32_t v;
				value1_address = qb_create_constant_array(cxt, QB_TYPE_F32, &dimension, 1);
				value2_address = qb_create_constant_array(cxt, QB_TYPE_F32, &dimension, 1);
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
				qb_retrieve_binary_op_result(cxt, &factory_add, value1_address, value2_address);
			}	break;
			case QB_DIAGNOSTIC_INT_MUL:
			case QB_DIAGNOSTIC_FLOAT_MUL:
			case QB_DIAGNOSTIC_VECTOR_MUL: {
				qb_retrieve_binary_op_result(cxt, &factory_multiply, value1_address, value2_address);
			}	break;
			case QB_DIAGNOSTIC_INT_DIV:
			case QB_DIAGNOSTIC_FLOAT_DIV:
			case QB_DIAGNOSTIC_VECTOR_DIV: { 
				qb_retrieve_binary_op_result(cxt, &factory_divide, value1_address, value2_address);
			}	break;
			case QB_DIAGNOSTIC_INT_MAC:
			case QB_DIAGNOSTIC_FLOAT_MAC:
			case QB_DIAGNOSTIC_VECTOR_MAC: {
				intermediate_address = qb_retrieve_binary_op_result(cxt, &factory_multiply, value1_address, value2_address);
				qb_retrieve_binary_op_result(cxt, &factory_add, value2_address, intermediate_address);
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
	qb_interpreter_context _interpreter_cxt, *interpreter_cxt = &_interpreter_cxt;

	qb_resolve_jump_targets(cxt);
	qb_fuse_instructions(cxt, 1);
	qb_assign_storage_space(cxt);
	qb_resolve_address_modes(cxt);
	qb_fuse_instructions(cxt, 2);

	qb_initialize_encoder_context(encoder_cxt, cxt, FALSE TSRMLS_CC);
	qfunc = qb_encode_function(encoder_cxt);
	qb_free_encoder_context(encoder_cxt);

	qb_initialize_interpreter_context(interpreter_cxt, qfunc, NULL TSRMLS_CC);
	qb_execute_internal(interpreter_cxt);
	qb_free_interpreter_context(interpreter_cxt);

	qb_free_function(qfunc);
}

int qb_run_diagnostics(qb_diagnostics *info TSRMLS_DC) {
	qb_build_context _cxt, *cxt = &_cxt;
	uint32_t i;

	qb_initialize_build_context(cxt TSRMLS_CC);
	cxt->compiler_contexts = emalloc(sizeof(qb_compiler_context *) * QB_DIAGNOSTIC_SPEED_TEST_COUNTS);

	for(i = 0; i < QB_DIAGNOSTIC_SPEED_TEST_COUNTS; i++) {
		qb_compiler_context *compiler_cxt = cxt->compiler_contexts[cxt->compiler_context_count++] = emalloc(sizeof(qb_compiler_context));
		qb_initialize_compiler_context(compiler_cxt, cxt->pool, NULL, 0, 0 TSRMLS_CC);
		qb_create_diagnostic_loop(compiler_cxt, i);
	}
		
	for(i = 0; i < QB_DIAGNOSTIC_SPEED_TEST_COUNTS; i++) {
		qb_compiler_context *compiler_cxt = cxt->compiler_contexts[i];
		double start_time, end_time, duration, instruction_per_sec;
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
