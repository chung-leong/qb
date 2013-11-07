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

	if(qop->opcode == QB_JMP) {
		uint32_t target_index = qop->jump_target_indices[0];
		qb_op *target_qop = cxt->ops[target_index];

		// if the jump lands on another jump, change the target to that jump's target
		while(target_qop->opcode == QB_JMP) {
			if(target_index != target_qop->jump_target_indices[0]) {
				target_index = target_qop->jump_target_indices[0];
				target_qop = cxt->ops[target_index];
				qop->jump_target_indices[0] = target_index;
			} else {
				// the jump goes to itself
				break;
			}
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

void qb_fuse_instructions(qb_compiler_context *cxt, int32_t pass) {
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
