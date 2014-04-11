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

static int32_t qb_select_type_dependent_opcode(qb_compiler_context *cxt, qb_opcode opcodes[], qb_primitive_type expr_type, qb_opcode *p_opcode) {
	if(expr_type < QB_TYPE_COUNT) {
		*p_opcode = opcodes[QB_TYPE_F64 - expr_type];
		return TRUE;
	} else {
		qb_report_internal_error(cxt->line_id, "Invalid type");
		return FALSE;
	}
}

static int32_t qb_select_opcode_simple(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_opcode *p_opcode) {
	qb_simple_op_factory *sf = (qb_simple_op_factory *) f;
	*p_opcode = sf->opcode;
	return TRUE;
}

static int32_t qb_select_opcode_basic(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_opcode *p_opcode) {
	qb_basic_op_factory *bf = (qb_basic_op_factory *) f;
	return qb_select_type_dependent_opcode(cxt, bf->opcodes, expr_type, p_opcode);
}

static int32_t qb_select_opcode_basic_first_operand(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_opcode *p_opcode) {
	qb_basic_op_factory *bf = (qb_basic_op_factory *) f;
	qb_primitive_type operand_type = operands[0].address->type;
	return qb_select_type_dependent_opcode(cxt, bf->opcodes, operand_type, p_opcode);
}

static int32_t qb_select_opcode_derived(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_opcode *p_opcode) {
	qb_derived_op_factory *df = (qb_derived_op_factory *) f;
	f = df->parent;
	return f->select_opcode(cxt, f, expr_type, operands, operand_count, result, p_opcode);
}

static int32_t qb_select_opcode_derived_modify_assign(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_opcode *p_opcode) {
	qb_operand *value = &operands[operand_count - 1];
	qb_operand binary_operands[2];
	qb_derived_op_factory *df = (qb_derived_op_factory *) f;
	binary_operands[0] = *result;
	binary_operands[1] = *value;
	f = df->parent;
	return f->select_opcode(cxt, f, expr_type, binary_operands, 2, result, p_opcode);
}

static int32_t qb_select_opcode_boolean_cast(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_opcode *p_opcode) {
	qb_basic_op_factory *bf = (qb_basic_op_factory *) f;
	qb_operand *variable = &operands[0];
	qb_primitive_type operand_type = variable->address->type;
	if(IS_SCALAR(variable->address)) {
		return qb_select_type_dependent_opcode(cxt, bf->opcodes, operand_type, p_opcode);
	} else {
		// will be checking the length of the array
		*p_opcode = bf->opcodes[QB_TYPE_F64 - QB_TYPE_U32];
		return TRUE;
	}
}

static int32_t qb_select_opcode_array_element_isset(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_opcode *p_opcode) {
	qb_basic_op_factory *bf = (qb_basic_op_factory *) f;
	qb_operand *container = &operands[0];
	qb_primitive_type operand_type = container->address->type;
	if(container->address->dimension_count == 1) {
		return qb_select_type_dependent_opcode(cxt, bf->opcodes, operand_type, p_opcode);
	} else {
		// will be checking the length of the sub-array
		*p_opcode = bf->opcodes[QB_TYPE_F64 - QB_TYPE_U32];
		return TRUE;
	}
}

static int32_t qb_select_opcode_object_property_isset(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_opcode *p_opcode) {
	qb_basic_op_factory *bf = (qb_basic_op_factory *) f;
	qb_operand *container = &operands[0], *name = &operands[1];
	qb_address *address = qb_obtain_object_property(cxt, container, name, 0);
	qb_primitive_type operand_type = container->address->type;

	if(IS_SCALAR(address)) {
		return qb_select_type_dependent_opcode(cxt, bf->opcodes, operand_type, p_opcode);
	} else {
		*p_opcode = bf->opcodes[QB_TYPE_F64 - QB_TYPE_U32];
		return TRUE;
	}
}

static int32_t qb_select_opcode_unset(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_opcode *p_opcode) {
	qb_unset_op_factory *uf = (qb_unset_op_factory *) f;
	qb_operand *variable = &operands[0];
	qb_primitive_type operand_type = variable->address->type;
	if(IS_SCALAR(variable->address)) {
		return qb_select_type_dependent_opcode(cxt, uf->scalar_opcodes, operand_type, p_opcode);
	} else {
		if(IS_RESIZABLE(variable->address)) {
			if(IS_MULTIDIMENSIONAL(variable->address)) {
				return qb_select_type_dependent_opcode(cxt, uf->resizing_dim_opcodes, operand_type, p_opcode);
			} else {
				return qb_select_type_dependent_opcode(cxt, uf->resizing_opcodes, operand_type, p_opcode);
			}
		} else {
			return qb_select_type_dependent_opcode(cxt, uf->no_resizing_opcodes, operand_type, p_opcode);
		}
	}
}

static int32_t qb_select_opcode_unset_array_element(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_opcode *p_opcode) {
	qb_unset_element_op_factory *uf = (qb_unset_element_op_factory *) f;
	qb_operand *container = &operands[0];
	qb_primitive_type operand_type = container->address->type;
	if(IS_RESIZABLE(container->address)) {
		if(IS_MULTIDIMENSIONAL(container->address)) {
			return qb_select_type_dependent_opcode(cxt, uf->resizing_dim_opcodes, operand_type, p_opcode);
		} else {
			return qb_select_type_dependent_opcode(cxt, uf->resizing_opcodes, operand_type, p_opcode);
		}
	} else {
		return qb_select_type_dependent_opcode(cxt, uf->no_resizing_opcodes, operand_type, p_opcode);
	}
}

static int32_t qb_select_opcode_unset_object_property(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_opcode *p_opcode) {
	qb_unset_op_factory *uf = (qb_unset_op_factory *) f;
	qb_operand *container = &operands[0], *name = &operands[1];
	qb_address *address = qb_obtain_object_property(cxt, container, name, 0);
	qb_primitive_type operand_type = address->type;
	if(IS_SCALAR(address)) {
		return qb_select_type_dependent_opcode(cxt, uf->scalar_opcodes, operand_type, p_opcode);
	} else {
		if(IS_RESIZABLE(address)) {
			if(IS_MULTIDIMENSIONAL(address)) {
				return qb_select_type_dependent_opcode(cxt, uf->resizing_dim_opcodes, operand_type, p_opcode);
			} else {
				return qb_select_type_dependent_opcode(cxt, uf->resizing_opcodes, operand_type, p_opcode);
			}
		} else {
			return qb_select_type_dependent_opcode(cxt, uf->no_resizing_opcodes, operand_type, p_opcode);
		}
	}
}

static int32_t qb_select_multidata_opcode(qb_compiler_context *cxt, qb_opcode *p_opcode) {
	uint32_t op_flags = qb_get_op_flags(*p_opcode);
	if(op_flags & QB_OP_VERSION_AVAILABLE_MIO) {
		if(op_flags & QB_OP_VERSION_AVAILABLE_ELE) {
			*p_opcode += 2;
		} else {
			*p_opcode += 1;
		}
		return TRUE;
	} else {
		return FALSE;
	}
}

static uint32_t qb_get_minimum_width(qb_compiler_context *cxt, qb_operand *operand) {
	qb_address *address = operand->address;
	uint32_t i, width = 1;
	for(i = 0; i < address->dimension_count; i++) {
		qb_address *array_size_address = address->array_size_addresses[i];
		if(IS_IMMUTABLE(array_size_address)) {
			width = VALUE(U32, array_size_address);
			break;
		}
	}
	return width;
}

static int32_t qb_select_vectorized_nullary_opcode(qb_compiler_context *cxt, qb_opcode opcodes[][2], qb_operand *operand1, qb_opcode *p_opcode) {
	if(operand1->address->type >= QB_TYPE_F32) {
		uint32_t width1 = qb_get_minimum_width(cxt, operand1);
		uint32_t denominator;

		if((width1 % 4) == 0) {
			denominator = 4;
		} else if((width1 % 3) == 0) {
			denominator = 3;
		} else if((width1 % 2) == 0) {
			denominator = 2;
		} else {
			return FALSE;
		}
		if(!qb_select_type_dependent_opcode(cxt, opcodes[denominator - 2], operand1->address->type, p_opcode)) {
			return FALSE;
		}
		if(width1 > denominator) {
			qb_select_multidata_opcode(cxt, p_opcode);
		}
		return TRUE;
	} else {
		return FALSE;
	}
}

static int32_t qb_select_opcode_nullary_arithmetic(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_opcode *p_opcode) {
	qb_arithmetic_op_factory *af = (qb_arithmetic_op_factory *) f;
	if(qb_select_vectorized_nullary_opcode(cxt, af->vector_opcodes, &operands[0], p_opcode)) {
		return TRUE;
	}
	return qb_select_type_dependent_opcode(cxt, af->regular_opcodes, expr_type, p_opcode);
}

static int32_t qb_select_vectorized_unary_opcode(qb_compiler_context *cxt, qb_opcode opcodes[][2], qb_operand *operand1, qb_operand *result, qb_opcode *p_opcode) {
	if(operand1->address->type >= QB_TYPE_F32) {
		uint32_t width1 = qb_get_minimum_width(cxt, operand1);
		uint32_t width2 = qb_get_minimum_width(cxt, result);
		uint32_t denominator;

		if((width1 % 4) == 0 && (width2 % 4) == 0) {
			denominator = 4;
		} else if((width1 % 3) == 0 && (width2 % 3) == 0) {
			denominator = 3;
		} else if((width1 % 2) == 0 && (width2 % 2) == 0) {
			denominator = 2;
		} else {
			return FALSE;
		}
		if(!qb_select_type_dependent_opcode(cxt, opcodes[denominator - 2], operand1->address->type, p_opcode)) {
			return FALSE;
		}
		if(!IS_FIXED_LENGTH(operand1->address) || !IS_FIXED_LENGTH(result->address) || width1 > denominator || width2 > denominator) {
			qb_select_multidata_opcode(cxt, p_opcode);
		}
		return TRUE;
	} else {
		return FALSE;
	}
}

static int32_t qb_select_opcode_reciprocal(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_opcode *p_opcode) {
	qb_derived_op_factory *df = (qb_derived_op_factory *) f;
	qb_arithmetic_op_factory *af = (qb_arithmetic_op_factory *) df->parent;
	return qb_select_type_dependent_opcode(cxt, af->regular_opcodes, expr_type, p_opcode);
}

static int32_t qb_select_vectorized_binary_opcode(qb_compiler_context *cxt, qb_opcode opcodes[][2], qb_operand *operand1, qb_operand *operand2, qb_operand *result, qb_opcode *p_opcode) {
	if(operand1->address->type >= QB_TYPE_F32) {
		uint32_t width1 = qb_get_minimum_width(cxt, operand1);
		uint32_t width2 = qb_get_minimum_width(cxt, operand2);
		uint32_t width3 = qb_get_minimum_width(cxt, result);
		uint32_t denominator;

		if((width1 % 4) == 0 && (width2 % 4) == 0 && (width3 % 4) == 0) {
			denominator = 4;
		} else if((width1 % 3) == 0 && (width2 % 3) == 0 && (width3 % 3) == 0) {
			denominator = 3;
		} else if((width1 % 2) == 0 && (width2 % 2) == 0 && (width3 % 2) == 0) {
			denominator = 2;
		} else {
			return FALSE;
		}
		if(!qb_select_type_dependent_opcode(cxt, opcodes[denominator - 2], operand1->address->type, p_opcode)) {
			return FALSE;
		}
		if(!IS_FIXED_LENGTH(operand1->address) || !IS_FIXED_LENGTH(operand2->address) || !IS_FIXED_LENGTH(result->address) || width1 > denominator || width2 > denominator || width3 > denominator) {
			qb_select_multidata_opcode(cxt, p_opcode);
		}
		return TRUE;
	} else {
		return FALSE;
	}
}

static int32_t qb_select_opcode_binary_arithmetic(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_opcode *p_opcode) {
	qb_arithmetic_op_factory *af = (qb_arithmetic_op_factory *) f;
	if(qb_select_vectorized_binary_opcode(cxt, af->vector_opcodes, &operands[0], &operands[1], result, p_opcode)) {
		return TRUE;
	}
	return qb_select_type_dependent_opcode(cxt, af->regular_opcodes, expr_type, p_opcode);
}

static int32_t qb_select_opcode_assign(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_opcode *p_opcode) {
	qb_operand *value = &operands[operand_count - 1];
	qb_copy_op_factory *cf = (qb_copy_op_factory *) f;
	qb_address *src_address = value->address;
	qb_address *dst_address = result->address;	
	if(src_address != dst_address) {
		// if the expression type was set to void, then an earlier op has used the r-value as write target
		// so there's no need to perform the assignment
		if(expr_type != QB_TYPE_VOID) {
			if(src_address->type == dst_address->type) {
				// vectorized instructions are available only for copying between variables of the same type
				if(qb_select_vectorized_unary_opcode(cxt, cf->vector_opcodes, value, result, p_opcode)) {
					return TRUE;
				}
			}
			if(src_address->type < QB_TYPE_COUNT) {
				return qb_select_type_dependent_opcode(cxt, cf->opcodes[QB_TYPE_F64 - src_address->type], dst_address->type, p_opcode);
			} else {
				qb_report_internal_error(cxt->line_id, "Invalid type");
				return FALSE;
			}
		}
	}
	*p_opcode = QB_NOP;
	return TRUE;
}

static int32_t qb_select_opcode_assign_return_value(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_opcode *p_opcode) {
	if(cxt->return_variable->address) {
		return qb_select_opcode_derived(cxt, f, expr_type, operands, operand_count, result, p_opcode);
	} else {
		*p_opcode = QB_NOP;
		return TRUE;
	}
}

static int32_t qb_select_opcode_assign_generator_key(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_opcode *p_opcode) {
	if(cxt->return_key_variable->address) {
		return qb_select_opcode_derived(cxt, f, expr_type, operands, operand_count, result, p_opcode);
	} else {
		*p_opcode = QB_NOP;
		return TRUE;
	}
}

static int32_t qb_select_opcode_increment_generator_key(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_opcode *p_opcode) {
	if(cxt->return_key_variable->address) {
		qb_derived_op_factory *df = (qb_derived_op_factory *) f;
		qb_arithmetic_op_factory *af = (qb_arithmetic_op_factory *) df->parent;
		return qb_select_type_dependent_opcode(cxt, af->regular_opcodes, cxt->return_key_variable->address->type, p_opcode);
	} else {
		*p_opcode = QB_NOP;
		return TRUE;
	}
}

static int32_t qb_select_opcode_gather(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_opcode *p_opcode) {
	qb_gather_op_factory *gf = (qb_gather_op_factory *) f;
	qb_operand *dest = &operands[0];
	uint32_t width = DIMENSION(dest->address, -1);
	return qb_select_type_dependent_opcode(cxt, gf->opcodes[width - 2], dest->address->type, p_opcode);
}

static int32_t qb_select_opcode_scatter(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_opcode *p_opcode) {
	qb_gather_op_factory *gf = (qb_gather_op_factory *) f;
	qb_operand *source = &operands[1];
	uint32_t width = DIMENSION(source->address, -1);
	return qb_select_type_dependent_opcode(cxt, gf->opcodes[width - 2], source->address->type, p_opcode);
}

static int32_t qb_select_opcode_copy_dimension(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_opcode *p_opcode) {
	qb_basic_op_factory *bf = (qb_basic_op_factory *) f;
	uint32_t dimension_count = (operand_count - 2) / 4;
	*p_opcode = bf->opcodes[dimension_count - 2];
	return TRUE;
}

static int32_t qb_select_opcode_add_variable(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_opcode *p_opcode) {
	qb_string_op_factory *sf = (qb_string_op_factory *) f;
	qb_address *address = operands[1].address;

	if(address->dimension_count > 1) {
		return qb_select_type_dependent_opcode(cxt, sf->multidim_opcodes, address->type, p_opcode);
	} else {
		if(address->flags & QB_ADDRESS_STRING) {
			return qb_select_type_dependent_opcode(cxt, sf->text_opcodes, address->type, p_opcode);
		} else {
			return qb_select_type_dependent_opcode(cxt, sf->opcodes, address->type, p_opcode);
		}
	}
}

static int32_t qb_select_opcode_print(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_opcode *p_opcode) {
	qb_string_op_factory *sf = (qb_string_op_factory *) f;
	qb_address *address = operands[0].address;

	if(address->dimension_count > 1) {
		return qb_select_type_dependent_opcode(cxt, sf->multidim_opcodes, address->type, p_opcode);
	} else {
		if(address->flags & QB_ADDRESS_STRING) {
			return qb_select_type_dependent_opcode(cxt, sf->text_opcodes, address->type, p_opcode);
		} else {
			return qb_select_type_dependent_opcode(cxt, sf->opcodes, address->type, p_opcode);
		}
	}
}

static int32_t qb_select_opcode_sampling(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_opcode *p_opcode) {
	qb_pixel_op_factory *pf = (qb_pixel_op_factory *) f;
	qb_address *image_address = operands[0].address;
	qb_address *address_x = operands[1].address;
	qb_address *address_y = operands[2].address;
	uint32_t channel_count = DIMENSION(image_address, -1);

	*p_opcode = pf->opcodes[channel_count - 1][QB_TYPE_F64 - image_address->type];

	if(!qb_select_type_dependent_opcode(cxt, pf->opcodes[channel_count - 1], image_address->type, p_opcode)) {
		return FALSE;
	}

	if(address_x->dimension_count > 1 || address_y->dimension_count > 1) {
		// handling multiple pixels
		qb_select_multidata_opcode(cxt, p_opcode);
	}
	return TRUE;
}

static int32_t qb_select_opcode_sampling_vector(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_opcode *p_opcode) {
	qb_derived_op_factory *df = (qb_derived_op_factory *) f;
	qb_pixel_op_factory *pf = (qb_pixel_op_factory *) df->parent;
	qb_address *image_address = operands[0].address;
	uint32_t channel_count = DIMENSION(image_address, -1);
	return qb_select_type_dependent_opcode(cxt, pf->opcodes[channel_count - 1], image_address->type, p_opcode);
}

static int32_t qb_select_opcode_array_resize(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_opcode *p_opcode) {
	qb_array_resize_op_factory *af = (qb_array_resize_op_factory *) f;
	qb_address *address = operands[0].address;
	return qb_select_type_dependent_opcode(cxt, af->opcodes[address->dimension_count - 1], address->type, p_opcode);
}

static int32_t qb_select_opcode_pixel(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_opcode *p_opcode) {
	qb_pixel_op_factory *pf = (qb_pixel_op_factory *) f;
	qb_address *address = operands[0].address;
	uint32_t channel_count = DIMENSION(address, -1);

	if(!qb_select_type_dependent_opcode(cxt, pf->opcodes[channel_count - 1], address->type, p_opcode)) {
		return FALSE;
	}

	if(address->dimension_count > 1) {
		// handling multiple pixels
		qb_select_multidata_opcode(cxt, p_opcode);
	}
	return TRUE;
}

static int32_t qb_select_opcode_one_vector(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_opcode *p_opcode) {
	qb_matrix_op_factory *mf = (qb_matrix_op_factory *) f;
	qb_address *address = operands[0].address;
	uint32_t dimension = 0;
	if(HAS_CONSTANT_DIMENSION(address, -1)) {
		dimension = DIMENSION(address, -1);
	}
	if(2 <= dimension && dimension <= 4) {
		if(!qb_select_type_dependent_opcode(cxt, mf->opcodes_fixed_size[dimension - 2], address->type, p_opcode)) {
			return FALSE;
		}
	} else {
		if(!qb_select_type_dependent_opcode(cxt, mf->opcodes_any_size, address->type, p_opcode)) {
			return FALSE;
		}
	}

	if(address->dimension_count > 1) {
		qb_select_multidata_opcode(cxt, p_opcode);
	}
	return TRUE;
}

static int32_t qb_select_opcode_two_vectors(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_opcode *p_opcode) {
	qb_matrix_op_factory *mf = (qb_matrix_op_factory *) f;
	qb_address *address1 = operands[0].address;
	qb_address *address2 = operands[1].address;
	uint32_t dimension = 0;
	if(HAS_CONSTANT_DIMENSION(address1, -1)) {
		dimension = DIMENSION(address1, -1);
	} else if(HAS_CONSTANT_DIMENSION(address2, -1)) {
		dimension = DIMENSION(address2, -1);
	}
	if(2 <= dimension && dimension <= 4) {
		if(!qb_select_type_dependent_opcode(cxt, mf->opcodes_fixed_size[dimension - 2], address1->type, p_opcode)) {
			return FALSE;
		}
	} else {
		if(!qb_select_type_dependent_opcode(cxt, mf->opcodes_any_size, address1->type, p_opcode)) {
			return FALSE;
		}
	}

	if(address1->dimension_count > 1 || address2->dimension_count > 1) {
		qb_select_multidata_opcode(cxt, p_opcode);
	}
	return TRUE;
}

static int32_t qb_select_opcode_cross_product(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_opcode *p_opcode) {
	qb_matrix_op_factory *mf = (qb_matrix_op_factory *) f;
	if(operand_count == 3) {
		qb_address *address1 = operands[0].address;
		qb_address *address2 = operands[1].address;
		qb_address *address3 = operands[2].address;
		uint32_t dimension = 4;

		if(!qb_select_type_dependent_opcode(cxt, mf->opcodes_fixed_size[dimension - 2], address1->type, p_opcode)) {
			return FALSE;
		}

		if(address1->dimension_count > 1 || address2->dimension_count > 1 || address3->dimension_count > 1) {
			qb_select_multidata_opcode(cxt, p_opcode);
		}
	} else {
		qb_address *address1 = operands[0].address;
		qb_address *address2 = operands[1].address;
		uint32_t dimension = 0;
		if(HAS_CONSTANT_DIMENSION(address1, -1)) {
			dimension = DIMENSION(address1, -1);
		} else if(HAS_CONSTANT_DIMENSION(address2, -1)) {
			dimension = DIMENSION(address2, -1);
		}
		if(!(2 <= dimension && dimension <= 3)) {
			return FALSE;
		}
		if(!qb_select_type_dependent_opcode(cxt, mf->opcodes_fixed_size[dimension - 2], address1->type, p_opcode)) {
			return FALSE;
		}

		if(address1->dimension_count > 1 || address2->dimension_count > 1) {
			qb_select_multidata_opcode(cxt, p_opcode);
		}
	}
	return TRUE;
}

static int32_t qb_select_matrix_opcode(qb_compiler_context *cxt, qb_matrix_op_factory *mf, uint32_t m1_cols, uint32_t m1_rows, uint32_t m2_cols, uint32_t m2_rows, int32_t multidata, qb_primitive_type expr_type, qb_opcode *p_opcode) {
	if(cxt->matrix_padding && m1_rows == 4 && m1_cols == 3) {
		*p_opcode = mf->opcode_3x3_padded;
		return TRUE;
	} else {
		int32_t m1_in_range = FALSE, m2_in_range = FALSE;
		if(m1_cols == m1_rows && (2 <= m1_cols && m1_cols <= 4)) {
			m1_in_range = TRUE;
		}
		if(m2_cols == m2_rows && (2 <= m2_cols && m2_cols <= 4)) {
			m2_in_range = TRUE;
		} else if(m2_rows == 0) {
			if((2 <= m2_cols && m2_cols <= 4) || m2_cols == 0) {
				m2_in_range = TRUE;
			}
		}

		if(m1_in_range && m2_in_range) {
			if(!qb_select_type_dependent_opcode(cxt, mf->opcodes_fixed_size[m1_rows - 2], expr_type, p_opcode)) {
				return FALSE;
			}
		} else {
			if(!qb_select_type_dependent_opcode(cxt, mf->opcodes_any_size, expr_type, p_opcode)) {
				return FALSE;
			}
		}

		if(multidata) {
			qb_select_multidata_opcode(cxt, p_opcode);
		}
		return TRUE;
	}
}

static int32_t qb_select_opcode_mm_mult_cm(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_opcode *p_opcode) {
	qb_matrix_op_factory *mf = (qb_matrix_op_factory *) f;
	qb_address *address1 = operands[0].address;
	qb_address *address2 = operands[1].address;
	uint32_t m1_cols = HAS_CONSTANT_DIMENSION(address1, -2) ? DIMENSION(address1, -2) : INVALID_INDEX;
	uint32_t m1_rows = HAS_CONSTANT_DIMENSION(address1, -1) ? DIMENSION(address1, -1) : INVALID_INDEX;
	uint32_t m2_cols = HAS_CONSTANT_DIMENSION(address2, -2) ? DIMENSION(address2, -2) : INVALID_INDEX;
	uint32_t m2_rows = HAS_CONSTANT_DIMENSION(address2, -1) ? DIMENSION(address2, -1) : INVALID_INDEX;
	int32_t multidata = (address1->dimension_count > 2 || address2->dimension_count > 2);

	return qb_select_matrix_opcode(cxt, mf, m1_cols, m1_rows, m2_cols, m2_rows, multidata, expr_type, p_opcode);
}

static int32_t qb_select_opcode_mv_mult_cm(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_opcode *p_opcode) {
	qb_matrix_op_factory *mf = (qb_matrix_op_factory *) f;
	qb_address *address1 = operands[0].address;
	qb_address *address2 = operands[1].address;
	uint32_t m1_cols = HAS_CONSTANT_DIMENSION(address1, -2) ? DIMENSION(address1, -2) : INVALID_INDEX;
	uint32_t m1_rows = HAS_CONSTANT_DIMENSION(address1, -1) ? DIMENSION(address1, -1) : INVALID_INDEX;
	uint32_t m2_cols = HAS_CONSTANT_DIMENSION(address2, -1) ? DIMENSION(address2, -1) : INVALID_INDEX;
	uint32_t m2_rows = 0;
	int32_t multidata = (address1->dimension_count > 2 || address2->dimension_count > 1);

	return qb_select_matrix_opcode(cxt, mf, m1_cols, m1_rows, m2_cols, m2_rows, multidata, expr_type, p_opcode);
}

static int32_t qb_select_opcode_vm_mult_cm(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_opcode *p_opcode) {
	qb_matrix_op_factory *mf = (qb_matrix_op_factory *) f;
	qb_address *address1 = operands[0].address;
	qb_address *address2 = operands[1].address;
	uint32_t m1_cols = HAS_CONSTANT_DIMENSION(address2, -2) ? DIMENSION(address2, -2) : INVALID_INDEX;
	uint32_t m1_rows = HAS_CONSTANT_DIMENSION(address2, -1) ? DIMENSION(address2, -1) : INVALID_INDEX;
	uint32_t m2_cols = HAS_CONSTANT_DIMENSION(address1, -1) ? DIMENSION(address1, -1) : INVALID_INDEX;
	uint32_t m2_rows = 0;
	int32_t multidata = (address1->dimension_count > 1 || address2->dimension_count > 2);

	return qb_select_matrix_opcode(cxt, mf, m1_cols, m1_rows, m2_cols, m2_rows, multidata, expr_type, p_opcode);
}

static int32_t qb_select_opcode_transpose_equivalent(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_opcode *p_opcode) {
	qb_derived_op_factory *df = (qb_derived_op_factory *) f;
	qb_operand reversed_operands[2];
	reversed_operands[0] = operands[1];
	reversed_operands[1] = operands[0];
	f = df->parent;
	return f->select_opcode(cxt, f, expr_type, reversed_operands, operand_count, result, p_opcode);
}

static int32_t qb_select_opcode_matrix_current_mode(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_opcode *p_opcode) {
	USE_TSRM
	qb_matrix_op_factory_selector *s = (qb_matrix_op_factory_selector *) f;
	if(QB_G(column_major_matrix)) {
		f = s->cm_factory;
	} else {
		f = s->rm_factory;
	}
	return f->select_opcode(cxt, f, expr_type, operands, operand_count, result, p_opcode);
}

static int32_t qb_select_opcode_matrix_unary(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_opcode *p_opcode) {
	qb_matrix_op_factory *mf = (qb_matrix_op_factory *) f;
	qb_address *address = operands[0].address;
	uint32_t m1_cols = HAS_CONSTANT_DIMENSION(address, -2) ? DIMENSION(address, -2) : INVALID_INDEX;
	uint32_t m1_rows = HAS_CONSTANT_DIMENSION(address, -1) ? DIMENSION(address, -1) : INVALID_INDEX;
	uint32_t m2_cols = 0;
	uint32_t m2_rows = 0;
	int32_t multidata = address->dimension_count > 2;

	return qb_select_matrix_opcode(cxt, mf, m1_cols, m1_rows, m2_cols, m2_rows, multidata, expr_type, p_opcode);
}

static int32_t qb_select_opcode_transform_cm(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_opcode *p_opcode) {
	qb_matrix_op_factory *mf = (qb_matrix_op_factory *) f;
	qb_address *address1 = operands[0].address;
	qb_address *address2 = operands[1].address;
	uint32_t m1_cols = HAS_CONSTANT_DIMENSION(address1, -2) ? DIMENSION(address1, -2) - 1 : INVALID_INDEX;
	uint32_t m1_rows = HAS_CONSTANT_DIMENSION(address1, -1) ? DIMENSION(address1, -1) : INVALID_INDEX;
	uint32_t m2_cols = HAS_CONSTANT_DIMENSION(address2, -1) ? DIMENSION(address2, -1) : INVALID_INDEX;
	uint32_t m2_rows = 0;
	int32_t multidata = (address1->dimension_count > 2 || address2->dimension_count > 1);

	return qb_select_matrix_opcode(cxt, mf, m1_cols, m1_rows, m2_cols, m2_rows, multidata, expr_type, p_opcode);
}

static int32_t qb_select_opcode_transform_rm(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_opcode *p_opcode) {
	qb_matrix_op_factory *mf = (qb_matrix_op_factory *) f;
	qb_address *address1 = operands[0].address;
	qb_address *address2 = operands[1].address;
	uint32_t m1_cols = HAS_CONSTANT_DIMENSION(address1, -1) ? DIMENSION(address1, -1) - 1 : INVALID_INDEX;
	uint32_t m1_rows = HAS_CONSTANT_DIMENSION(address1, -2) ? DIMENSION(address1, -2) : INVALID_INDEX;
	uint32_t m2_cols = HAS_CONSTANT_DIMENSION(address2, -1) ? DIMENSION(address2, -1) : INVALID_INDEX;
	uint32_t m2_rows = 0;
	int32_t multidata = (address1->dimension_count > 2 || address2->dimension_count > 1);

	return qb_select_matrix_opcode(cxt, mf, m1_cols, m1_rows, m2_cols, m2_rows, multidata, expr_type, p_opcode);
}

static int32_t qb_select_opcode_complex_number(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_opcode *p_opcode) {
	uint32_t i;
	if(!qb_select_opcode_basic(cxt, f, expr_type, operands, operand_count, result, p_opcode)) {
		return FALSE;
	}

	for(i = 0; i < operand_count; i++) {
		qb_address *address = operands[i].address;
		if(address->dimension_count > 1) {
			qb_select_multidata_opcode(cxt, p_opcode);
			break;
		}
	}
	return TRUE;
}

static int32_t qb_select_opcode_utf8_decode(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_opcode *p_opcode) {
	qb_utf8_op_factory *uf = (qb_utf8_op_factory *) f;
	if(result->address->type == QB_TYPE_U32) {
		*p_opcode = uf->ucs32_opcode;
	} else {
		*p_opcode = uf->ucs16_opcode;
	}
	return TRUE;
}

static int32_t qb_select_opcode_utf8_encode(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_opcode *p_opcode) {
	qb_operand *codepoints = &operands[0];
	qb_utf8_op_factory *uf = (qb_utf8_op_factory *) f;
	if(codepoints->address->type == QB_TYPE_U32) {
		*p_opcode = uf->ucs32_opcode;
	} else {
		*p_opcode = uf->ucs16_opcode;
	}
	return TRUE;
}

static int32_t qb_select_opcode_expression_type(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_opcode *p_opcode) {
	qb_basic_op_factory *bf = (qb_basic_op_factory *) f;
	return qb_select_type_dependent_opcode(cxt, bf->opcodes, expr_type, p_opcode);
}

static int32_t qb_select_opcode_ext(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_opcode *p_opcode) {
	USE_TSRM
	if(QB_G(allow_debugger_inspection)) {
		qb_simple_op_factory *sf = (qb_simple_op_factory *) f;
		*p_opcode = sf->opcode;
	} else {
		*p_opcode = QB_NOP;
	}
	return TRUE;
}
