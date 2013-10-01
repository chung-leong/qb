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

static qb_opcode qb_select_type_dependent_opcode(qb_compiler_context *cxt, qb_opcode opcodes[], qb_operand *operand) {
	qb_address *address = operand->address;
	qb_opcode opcode = opcodes[QB_TYPE_F64 - address->type];
	return opcode;
}

static qb_opcode qb_select_opcode_simple(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_operand *result) {
	qb_simple_op_factory *sf = (qb_simple_op_factory *) f;
	return sf->opcode;
}

static qb_opcode qb_select_opcode_basic(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_operand *result) {
	qb_basic_op_factory *bf = (qb_basic_op_factory *) f;
	return qb_select_type_dependent_opcode(cxt, bf->opcodes, (operand_count > 0) ? &operands[0] : result);
}

static qb_opcode qb_select_opcode_basic_result(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_operand *result) {
	qb_basic_op_factory *bf = (qb_basic_op_factory *) f;
	return qb_select_type_dependent_opcode(cxt, bf->opcodes, result);
}

static qb_opcode qb_select_opcode_derived(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_operand *result) {
	qb_derived_op_factory *df = (qb_derived_op_factory *) f;
	f = df->parent;
	return f->select_opcode(cxt, f, operands, operand_count, result);
}

static qb_opcode qb_select_multidata_opcode(qb_compiler_context *cxt, qb_opcode opcode) {
	uint32_t op_flags = qb_get_op_flags(cxt, opcode);
	if(op_flags & QB_OP_VERSION_AVAILABLE_MIO) {
		if(op_flags & QB_OP_VERSION_AVAILABLE_ELE) {
			opcode += 2;
		} else {
			opcode += 1;
		}
	}
	return opcode;
}

static uint32_t qb_get_minimum_width(qb_compiler_context *cxt, qb_operand *operand) {
	qb_address *address = operand->address;
	uint32_t i, width = 0;
	for(i = 0; i < address->dimension_count; i++) {
		qb_address *array_size_address = address->array_size_addresses[i];
		if(CONSTANT(array_size_address)) {
			width = VALUE(U32, array_size_address);
			break;
		}
	}
	return width;
}

static qb_opcode qb_select_vectorized_opcode(qb_compiler_context *cxt, qb_opcode opcodes[][2], qb_operand *operands, uint32_t operand_count, qb_operand *result) {
	qb_address *address = operands[0].address;
	if(address->type >= QB_TYPE_F32) {
		uint32_t i, j;

		// see if the operands are divisible by 2, 3, or 4
		int32_t divisible[5] = { FALSE, TRUE, TRUE, TRUE, TRUE };
		for(i = 0; i <= operand_count; i++) {
			uint32_t width = qb_get_minimum_width(cxt, (i < operand_count) ? &operands[i] : result);
			if(width) {
				divisible[2] = divisible[2] && !(width & 0x0001);
				divisible[3] = divisible[3] && !(width % 3);
				divisible[4] = divisible[4] && !(width & 0x0003);
			} else {
				return QB_NOP;
			}
		}
		for(j = 4; j >= 2; j--) {
			if(divisible[j]) {
				qb_opcode opcode = opcodes[j - 2][QB_TYPE_F64 - address->type];

				// see if the multiple-input-output version of the op is needed
				int32_t multiple_data = FALSE;
				for(i = 0; i <= operand_count; i++) {
					qb_address *address = (i < operand_count) ? operands[i].address : result->address;
					if(VARIABLE_LENGTH_ARRAY(address) || ARRAY_SIZE(address) > j) {
						// the MIO version is right behind the regular one
						opcode++;
						break;
					}
				}
				return opcode;
			}
		}
	}
	return QB_NOP;
}

static qb_opcode qb_select_opcode_assignment(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_operand *result) {
	qb_copy_op_factory *cf = (qb_copy_op_factory *) f;
	qb_address *src_address = operands[operand_count - 1].address;
	qb_address *dst_address = result->address;	
	qb_opcode opcode = QB_NOP;
	if(result->type != QB_OPERAND_NONE) {
		if(src_address->type == dst_address->type) {
			// vectorized instructions are available only for copying between variables of the same type
			opcode = qb_select_vectorized_opcode(cxt, cf->vector_opcodes, operands, operand_count, result);
		}
		if(opcode == QB_NOP) {
			opcode = cf->opcodes[QB_TYPE_F64 - src_address->type][QB_TYPE_F64 - dst_address->type];
		}
	}
	return opcode;
}

static qb_opcode qb_select_opcode_arithmetic(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_operand *result) {
	qb_arithmetic_op_factory *af = (qb_arithmetic_op_factory *) f;
	qb_opcode opcode = qb_select_vectorized_opcode(cxt, af->vector_opcodes, operands, operand_count, result);
	if(opcode == QB_NOP) {
		opcode = qb_select_type_dependent_opcode(cxt, af->regular_opcodes, (operand_count > 0) ? &operands[0] : result);
	}
	return opcode;
}

static qb_opcode qb_select_opcode_concat_variable(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_operand *result) {
	qb_string_op_factory *sf = (qb_string_op_factory *) f;
	qb_address *address = operands[1].address;
	qb_opcode opcode;

	if(address->dimension_count > 1) {
		opcode = qb_select_type_dependent_opcode(cxt, sf->multidim_opcodes, &operands[1]);
	} else {
		if(address->flags & QB_ADDRESS_STRING) {
			opcode = sf->text_opcode;
		} else {
			opcode = qb_select_type_dependent_opcode(cxt, sf->opcodes, &operands[1]);
		}
	}
	return opcode;
}

static qb_opcode qb_select_opcode_print(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_operand *result) {
	qb_string_op_factory *sf = (qb_string_op_factory *) f;
	qb_address *address = operands[0].address;
	qb_opcode opcode;

	if(address->dimension_count > 1) {
		opcode = qb_select_type_dependent_opcode(cxt, sf->multidim_opcodes, &operands[0]);
	} else {
		if(address->flags & QB_ADDRESS_STRING) {
			opcode = sf->text_opcode;
		} else {
			opcode = qb_select_type_dependent_opcode(cxt, sf->opcodes, &operands[0]);
		}
	}
	return opcode;
}

static qb_opcode qb_select_opcode_pixel(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_operand *result) {
	qb_pixel_op_factory *pf = (qb_pixel_op_factory *) f;
	qb_address *address = operands[0].address;
	uint32_t channel_count = DIMENSION(address, -1);
	qb_opcode opcode = pf->opcodes[channel_count - 3][QB_TYPE_F64 - address->type];

	if(address->dimension_count > 1) {
		// handling multiple pixels
		opcode = qb_select_multidata_opcode(cxt, opcode);
	}
	return opcode;
}

static qb_opcode qb_select_opcode_one_vector(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_operand *result) {
	qb_vector_op_factory *vf = (qb_vector_op_factory *) f;
	qb_address *address = operands[0].address;
	qb_opcode opcode = QB_NOP;
	if(CONSTANT_DIMENSION(address, -1)) {
		uint32_t dimension = DIMENSION(address, -1);
		if(2 <= dimension && dimension <= 4) {
			opcode = vf->opcodes_fixed_size[dimension - 2][QB_TYPE_F64 - address->type];
		}
	}
	if(opcode == QB_NOP) {
		opcode = vf->opcodes_any_size[QB_TYPE_F64 - address->type];
	}
	if(address->dimension_count > 1) {
		opcode = qb_select_multidata_opcode(cxt, opcode);
	}
	return opcode;
}

static qb_opcode qb_select_opcode_two_vectors(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_operand *result) {
	qb_vector_op_factory *vf = (qb_vector_op_factory *) f;
	qb_address *address1 = operands[0].address;
	qb_address *address2 = operands[1].address;
	qb_opcode opcode = QB_NOP;
	if(CONSTANT_DIMENSION(address1, -1) && CONSTANT_DIMENSION(address2, -1)) {
		uint32_t dimension = DIMENSION(address1, -1);

		if(2 <= dimension && dimension <= 4) {
			opcode = vf->opcodes_fixed_size[dimension - 2][QB_TYPE_F64 - address1->type];
		}
	}
	if(opcode == QB_NOP) {
		opcode = vf->opcodes_any_size[QB_TYPE_F64 - address1->type];
	}
	if(address1->dimension_count > 1 || address2->dimension_count > 1) {
		opcode = qb_select_multidata_opcode(cxt, opcode);
	}
	return opcode;
}

static qb_opcode qb_select_fixed_size_matrix_opcode(qb_compiler_context *cxt, qb_opcode opcodes[][2], qb_operand *operand) {
	qb_address *address = operand->address;
	uint32_t rows = DIMENSION(address, -2);
	uint32_t cols = DIMENSION(address, -1);
	qb_opcode opcode; 
	if(rows == cols && 2 <= rows && rows <= 4) {
		opcode = opcodes[rows - 2][QB_TYPE_F64 - address->type];
	} else {
		opcode = QB_NOP;
	}
	return opcode;
}

static qb_opcode qb_select_opcode_mm_mult_cm(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_operand *result) {
	qb_matrix_op_factory *mf = (qb_matrix_op_factory *) f;
	qb_address *address1 = operands[0].address;
	qb_address *address2 = operands[1].address;
	uint32_t m1_cols = DIMENSION(address1, -2);
	uint32_t m1_rows = DIMENSION(address1, -1);
	uint32_t m2_cols = DIMENSION(address2, -2);
	uint32_t m2_rows = DIMENSION(address2, -1);
	qb_opcode opcode;
	if(cxt->matrix_padding && m1_rows == 3 && m1_cols == 4 && m2_rows == 3 && m2_cols == 4) {
		opcode = mf->opcode_3x3_padded;
	} else if(m1_rows == m1_cols && m2_rows == m2_cols) {
		opcode = qb_select_fixed_size_matrix_opcode(cxt, mf->opcodes_fixed_size, &operands[0]);
	} 
	if(opcode == QB_NOP) {
		opcode = qb_select_type_dependent_opcode(cxt, mf->opcodes_any_size, &operands[0]);
	}
	return opcode;
}

static qb_opcode qb_select_opcode_mv_mult_cm(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_operand *result) {
	qb_matrix_op_factory *mf = (qb_matrix_op_factory *) f;
	qb_address *address1 = operands[0].address;
	uint32_t m_cols = DIMENSION(address1, -2);
	uint32_t m_rows = DIMENSION(address1, -1);
	qb_opcode opcode;
	if(cxt->matrix_padding && m_rows == 3 && m_cols == 4) {
		opcode = mf->opcode_3x3_padded;
	} else {
		opcode = qb_select_fixed_size_matrix_opcode(cxt, mf->opcodes_fixed_size, &operands[0]);
		if(opcode == QB_NOP) {
			opcode = qb_select_type_dependent_opcode(cxt, mf->opcodes_any_size, &operands[0]);
		}
	} 
	return opcode;
}

static qb_opcode qb_select_opcode_vm_mult_cm(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_operand *result) {
	qb_matrix_op_factory *mf = (qb_matrix_op_factory *) f;
	qb_address *address2 = operands[1].address;
	uint32_t m_cols = DIMENSION(address2, -2);
	uint32_t m_rows = DIMENSION(address2, -1);
	qb_opcode opcode;
	if(cxt->matrix_padding && m_rows == 3 && m_cols == 4) {
		opcode = mf->opcode_3x3_padded;
	} else {
		opcode = qb_select_fixed_size_matrix_opcode(cxt, mf->opcodes_fixed_size, &operands[1]);
		if(opcode == QB_NOP) {
			opcode = qb_select_type_dependent_opcode(cxt, mf->opcodes_any_size, &operands[1]);
		}
	} 
	return opcode;
}

static qb_opcode qb_select_opcode_transpose_equivalent(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_operand *result) {
	qb_derived_op_factory *df = (qb_derived_op_factory *) f;
	qb_operand reversed_operands[2];
	reversed_operands[0] = operands[1];
	reversed_operands[1] = operands[0];
	f = df->parent;
	return f->select_opcode(cxt, f, reversed_operands, operand_count, result);
}

static qb_opcode qb_select_opcode_matrix_current_mode(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_operand *result) {
	qb_matrix_op_factory_selector *s = (qb_matrix_op_factory_selector *) f;
	if(cxt->matrix_order == QB_MATRIX_ORDER_COLUMN_MAJOR) {
		f = s->cm_factory;
	} else {
		f = s->rm_factory;
	}
	return f->select_opcode(cxt, f, operands, operand_count, result);
}

static qb_opcode qb_select_opcode_matrix_unary(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_operand *result) {
	qb_vector_op_factory *vf = (qb_vector_op_factory *) f;
	qb_address *address = operands[0].address;
	qb_opcode opcode = qb_select_fixed_size_matrix_opcode(cxt, vf->opcodes_fixed_size, &operands[0]);
	if(opcode == QB_NOP) {
		opcode = qb_select_type_dependent_opcode(cxt, vf->opcodes_any_size, &operands[0]);
	}
	if(address->dimension_count > 2) {
		opcode = qb_select_multidata_opcode(cxt, opcode);
	}
	return opcode;
}

static qb_opcode qb_select_opcode_transform_cm(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_operand *result) {
	qb_matrix_op_factory *mf = (qb_matrix_op_factory *) f;
	qb_address *address1 = operands[0].address;
	qb_address *address2 = operands[1].address;
	uint32_t m_cols = DIMENSION(address1, -2);
	uint32_t m_rows = DIMENSION(address1, -1);
	qb_opcode opcode;

	if(2 <= m_rows && m_rows <= 4) {
		opcode = mf->opcodes_fixed_size[m_rows - 2][QB_TYPE_F64 - address1->type];
	} else {
		opcode = mf->opcodes_any_size[QB_TYPE_F64 - address1->type];
	}
	if(address1->dimension_count > 1 || address2->dimension_count > 2) {
		// handling multiple matrices or vectors		
		opcode = qb_select_multidata_opcode(cxt, opcode);
	}
	return opcode;
}

static qb_opcode qb_select_opcode_transform_rm(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_operand *result) {
	qb_matrix_op_factory *mf = (qb_matrix_op_factory *) f;
	qb_address *address1 = operands[0].address;
	qb_address *address2 = operands[1].address;
	uint32_t m_cols = DIMENSION(address1, -1);
	uint32_t m_rows = DIMENSION(address1, -2);
	qb_opcode opcode;

	if(2 <= m_rows && m_rows <= 4) {
		opcode = mf->opcodes_fixed_size[m_rows - 2][QB_TYPE_F64 - address1->type];
	} else {
		opcode = mf->opcodes_any_size[QB_TYPE_F64 - address1->type];
	}
	if(address1->dimension_count > 1 || address2->dimension_count > 2) {
		// handling multiple matrices or vectors		
		opcode = qb_select_multidata_opcode(cxt, opcode);
	}
	return opcode;
}

static qb_opcode qb_select_opcode_complex_number(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_operand *result) {
	qb_opcode opcode = qb_select_opcode_basic(cxt, f, operands, operand_count, result);
	uint32_t i;
	for(i = 0; i < operand_count; i++) {
		qb_address *address = operands[i].address;
		if(address->dimension_count > 1) {
			opcode = qb_select_multidata_opcode(cxt, opcode);
			break;
		}
	}
	return opcode;
}

static qb_opcode qb_select_opcode_intrinsic(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_operand *result) {
	qb_operand *func = &operands[0], *arguments = &operands[1], *argument_count = &operands[2];
	f = func->intrinsic_function->extra;
	if(f->select_opcode) {
		return f->select_opcode(cxt, f, arguments->arguments, argument_count->number, result);
	}
	return QB_NOP;
}