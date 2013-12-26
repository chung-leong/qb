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

			if(prev_qop && prev_qop->operand_count == 3 && (prev_qop->operands[2].address == condition_address || prev_qop->operands[2].address->source_address == condition_address)) {
				qb_opcode new_opcode = 0;

				// combine the comparison op with the branch op
				// (only if no arrays are involved since the combined instructions aren't implemented for them)
				if(!qb_find_array_address(prev_qop)) {
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
						prev_qop->flags |= QB_OP_BRANCH;
						prev_qop->jump_target_indices = qop->jump_target_indices;
						prev_qop->jump_target_count = qop->jump_target_count;
						prev_qop->operand_count = 2;
						qop->flags &= ~QB_OP_BRANCH;
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

	if(qop->operand_count == 3 && !(qop->flags & QB_OP_JUMP) && qop->operands[2].type == QB_OPERAND_ADDRESS && TEMPORARY(qop->operands[2].address)) {
		qb_op *next_qop = qb_get_next_op(cxt, index);

		if(next_qop && next_qop->operand_count == 3 && !(next_qop->flags & (QB_OP_JUMP | QB_OP_BRANCH | QB_OP_EXIT))) {
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
							if(!(qop_ahead->flags & (QB_OP_JUMP | QB_OP_BRANCH | QB_OP_EXIT))) {
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
					new_operands[0] = qop->operands[0];
					new_operands[1] = qop->operands[1];
					if(next_qop->operands[0].address == qop->operands[2].address) {
						new_operands[2] = next_qop->operands[1];
					} else {
						new_operands[2] = next_qop->operands[0];
					}
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

	if(qop->flags & (QB_OP_JUMP | QB_OP_BRANCH)) {
		uint32_t target_index;
		qb_op *target_qop;
		uint32_t i;
		for(i = 0; i < qop->jump_target_count; i++) {
			target_index = qop->jump_target_indices[i];
			target_qop = cxt->ops[target_index];

			// if the jump lands on another jump, change the target to that jump's target
			while(target_qop->opcode == QB_JMP) {
				if(target_index != target_qop->jump_target_indices[0]) {
					target_index = target_qop->jump_target_indices[0];
					target_qop = cxt->ops[target_index];
					qop->jump_target_indices[i] = target_index;
				} else {
					// the jump goes to itself
					break;
				}
			}
		}

		if(qop->opcode == QB_JMP) {
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
}

static int64_t qb_get_constant_value(qb_compiler_context *cxt, qb_address *address) {
	switch(address->type) {
		case QB_TYPE_S08: 
		case QB_TYPE_U08: return VALUE(S08, address);
		case QB_TYPE_S16: 
		case QB_TYPE_U16: return VALUE(S16, address);
		case QB_TYPE_S32: 
		case QB_TYPE_U32: return VALUE(S32, address);
		case QB_TYPE_S64: 
		case QB_TYPE_U64: return VALUE(S64, address);
		default: return 0;
	}
}

uint32_t qb_convert_switch_statement(qb_compiler_context *cxt, uint32_t index) {
	qb_op *qop = cxt->ops[index];

	if(qop->flags & QB_OP_BRANCH) {
		qb_op *first_case = NULL;
		qb_address *variable_address;
		switch(qop->opcode) {
			case QB_IF_EQ_I08_I08:
			case QB_IF_EQ_I16_I16:
			case QB_IF_EQ_I32_I32:
			case QB_IF_EQ_I64_I64: {
				qb_address *address1 = qop->operands[0].address;
				qb_address *address2 = qop->operands[1].address;
				if(SCALAR(address1) && CONSTANT(address2)) {
					first_case = qop;
					variable_address = address1;
				}
			}	break;
		}
		if(first_case) {
			uint32_t case_count = 1, last_index = index;
			uint32_t previous_jump_target = qop->jump_target_indices[1];
			uint32_t i;
			for(i = index + 1; i < cxt->op_count - 1; i++) {
				qb_op *next_qop = cxt->ops[i];
				if(next_qop->opcode != QB_NOP) {
					if(i == previous_jump_target) {
						if(next_qop->opcode == first_case->opcode) {
							qb_address *address1 = next_qop->operands[0].address;
							qb_address *address2 = next_qop->operands[1].address;
							if(address1 == variable_address && CONSTANT(address2)) {
								case_count++;
								previous_jump_target = next_qop->jump_target_indices[1];
								last_index = i;
								continue;
							}
						}
					}
					break;
				}
			}
			if(case_count > 1) {
				int32_t use_table = FALSE;
				int64_t *case_constants;
				uint32_t *jump_indices;
				uint32_t default_jump_index;
				int64_t range_offset;
				uint64_t range_length;
				uint32_t j;
				ALLOCA_FLAG(use_heap1)
				ALLOCA_FLAG(use_heap2)

				// collect the jump target indices and constants
				jump_indices = do_alloca(sizeof(uint32_t) * case_count, use_heap1);
				case_constants = do_alloca(sizeof(int64_t) * case_count, use_heap2);
				for(i = index, j = 0; i <= last_index; i++) {
					qop = cxt->ops[i];
					if(qop->opcode != QB_NOP) {
						qb_address *address2 = qop->operands[1].address;
						case_constants[j] = qb_get_constant_value(cxt, address2);	
						jump_indices[j] = qop->jump_target_indices[0];
						j++;
						if(j == case_count) {
							default_jump_index = qop->jump_target_indices[1];
						}
					}
				}

				// see how large is the range
				// don't bother unless there are more than two cases
				if(case_count > 2) {
					int32_t variable_signed = !(variable_address->type & QB_TYPE_UNSIGNED);
					uint64_t range_min_u = UINT64_MAX, range_max_u = 0;
					int64_t range_min_s = INT64_MAX, range_max_s = INT64_MIN;
					for(i = 0; i < case_count; i++) {
						if(variable_signed) {
							int64_t constant_s = case_constants[i];
							if(constant_s > range_max_s) {
								range_max_s = constant_s;
							} 
							if(constant_s < range_min_s) {
								range_min_s = constant_s;
							}
						} else {
							uint64_t constant_u = case_constants[i];
							if(constant_u > range_max_u) {
								range_max_u = constant_u;
							} 
							if(constant_u < range_min_u) {
								range_min_u = constant_u;
							}
						}
					}
					if(variable_signed) {
						range_length = range_max_s - range_min_s + 1;
						range_offset = range_min_s;
					} else {
						range_length = range_max_u - range_min_u + 1;
						range_offset = range_min_u;
					}
					if(case_count > range_length * 0.25) {
						// do the substitution if more than a quarter of slots will be used
						use_table = TRUE;
					} else if(range_length - case_count < 16) {
						// do the substitution if less than 16 slots will be wasted
						use_table = TRUE;
					}
					if(range_length > 1024) {
						// the maximum table size is 1024
						use_table = FALSE;
					}
				}

				if(use_table) {
					qb_address *offset_address = qb_obtain_constant(cxt, range_offset, variable_address->type & ~QB_TYPE_UNSIGNED); 
					uint32_t table_size = qb_get_switch_table_size((uint32_t) range_length);
					uint32_t *table = qb_allocate_indices(cxt->pool, table_size);
					qb_opcode new_opcode = qb_get_switch_opcode((uint32_t) range_length, offset_address->type);
					qb_op *switch_op = first_case;

					// fill the table with the default index first
					for(i = 0; i < table_size; i++) {
						table[i] = default_jump_index;
					}
					// put in the cases
					for(i = 0; i < case_count; i++) {
						int64_t constant = case_constants[i];
						uint32_t jump_index = jump_indices[i];
						uint32_t table_index = (uint32_t) (constant - range_offset);
						table[table_index] = jump_index;
					}

					// replace first case with switch op
					switch_op->flags &= ~QB_OP_BRANCH;
					switch_op->flags |= QB_OP_BRANCH_TABLE;
					switch_op->opcode = new_opcode;
					switch_op->operand_count = 2;
					switch_op->operands[0].address = variable_address;
					switch_op->operands[1].address = offset_address;
					switch_op->jump_target_count = table_size;
					switch_op->jump_target_indices = table;

					for(i = index + 1; i <= last_index; i++) {
						qop = cxt->ops[i];
						if(qop->opcode != QB_NOP) {
							qop->flags &= ~QB_OP_BRANCH;
							qop->opcode = QB_NOP;
						}
					}
				}

				free_alloca(jump_indices, use_heap1);
				free_alloca(case_constants, use_heap2);
			}
			return last_index - index;
		}
	}
	return 0;
}

void qb_mark_op_reachability(qb_compiler_context *cxt, uint32_t start_index) {
	uint32_t i, j;
	for(i = start_index; i < cxt->op_count - 1; i++) {
		qb_op *qop = cxt->ops[i];
		if(qop->flags & QB_OP_REACHABLE) {
			break;
		} else {
			qop->flags |= QB_OP_REACHABLE;
		}
		if(qop->opcode == QB_RET) {
			break;
		}
		if(qop->jump_target_count > 0) {
			for(j = 0; j < qop->jump_target_count; j++) {
				qb_mark_op_reachability(cxt, qop->jump_target_indices[j]);
			}
			break;
		}
	}
}

void qb_fuse_instructions(qb_compiler_context *cxt, int32_t pass) {
	uint32_t i;
	if(pass == 1) {
		// opcodes are not address mode specific at this point
		// the last op is always RET: there's no need to scan it
		for(i = 0; i < cxt->op_count - 1; i++) {
			qb_simplify_jump(cxt, i);
			if(qb_fuse_conditional_branch(cxt, i)) {
				continue;
			}
			if(qb_fuse_multiply_accumulate(cxt, i)) {
				continue;
			}
		}
		for(i = 0; i < cxt->op_count - 1; i++) {
			uint32_t skip = qb_convert_switch_statement(cxt, i);
			i += skip;
		}
	} else if(pass == 2) {
		// opcodes are address mode specific here
		qb_mark_op_reachability(cxt, 0);
		for(i = 0; i < cxt->op_count - 1; i++) {
			qb_op *qop = cxt->ops[i];
			if(!(qop->flags & QB_OP_REACHABLE)) {
				qop->opcode = QB_NOP;
				qop->flags &= ~QB_OP_JUMP_TARGET;
			}
		}
	}
}
