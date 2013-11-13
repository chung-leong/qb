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

static void qb_transfer_operands_all(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_operand *dest, uint32_t dest_count) {
	uint32_t i;
	for(i = 0; i < operand_count; i++) {
		dest[i] = operands[i];
	}
	if(result) {
		dest[i] = *result;
	}
}

static void qb_transfer_operands_first(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_operand *dest, uint32_t dest_count) {
	dest[0] = operands[0];
	if(result) {
		dest[1] = *result;
	}
}

static void qb_transfer_operands_derived(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_operand *dest, uint32_t dest_count) {
	qb_derived_op_factory *df = (qb_derived_op_factory *) f;
	f = df->parent;
	f->transfer_operands(cxt, f, operands, operand_count, result, dest, dest_count);
}

static void qb_transfer_operands_assign(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_operand *dest, uint32_t dest_count) {
	dest[0] = operands[operand_count - 1];
	dest[1] = *result;
}

static void qb_transfer_operands_boolean_cast(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_operand *dest, uint32_t dest_count) {
	qb_operand *variable = &operands[0];
	if(SCALAR(variable->address)) {
		dest[0] = *variable;
	} else {
		dest[0].address = variable->address->array_size_address;
		dest[0].type = QB_OPERAND_ADDRESS;
	}
	dest[1] = *result;
}

static void qb_transfer_operands_array_element_isset(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_operand *dest, uint32_t dest_count) {
	qb_operand *container = &operands[0], *index = &operands[1];
	qb_address *variable_address, *predicate_address;
	if(container->address->dimension_count == 1) {
		variable_address = qb_obtain_array_element(cxt, container->address, index->address, QB_ARRAY_BOUND_CHECK_ISSET);
	} else {
		variable_address = container->address->array_size_addresses[1];
	}
	predicate_address = qb_obtain_predicate_address(cxt, variable_address, FALSE);
	dest[0].address = variable_address;
	dest[0].type = QB_OPERAND_ADDRESS;
	dest[1].address = predicate_address;
	dest[1].type = QB_OPERAND_ADDRESS;
	dest[2] = *result;
}

static void qb_transfer_operands_unset(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_operand *dest, uint32_t dest_count) {
	qb_operand *variable = &operands[0];
	if(SCALAR(variable->address)) {
		dest[0] = *variable;
	} else {
		if(RESIZABLE(variable->address)) {
			if(MULTIDIMENSIONAL(variable->address)) {
				// need to set the first dimension to zero as well
				dest[0].address = variable->address->dimension_addresses[0];
				dest[0].type = QB_OPERAND_ADDRESS;
				dest[1].address = variable->address;
				dest[1].type = QB_OPERAND_SEGMENT_SELECTOR;
				dest[2] = *variable;
			} else {
				dest[0].address = variable->address;
				dest[0].type = QB_OPERAND_SEGMENT_SELECTOR;
				dest[1] = *variable;
			}
		} else {
			// the predicate is always use, since we're unsetting an array accessible through a variable
			dest[0].address = cxt->true_address;
			dest[0].type = QB_OPERAND_ADDRESS;
			dest[1] = *variable;
		}
	}
}

static void qb_transfer_operands_unset_array_element(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_operand *dest, uint32_t dest_count) {
	qb_operand *container = &operands[0], *index = &operands[1];
	dest[0] = *index;
	dest[1].address = (container->address->dimension_count > 1) ? container->address->array_size_addresses[1] : cxt->one_address;
	dest[1].type = QB_OPERAND_ADDRESS;
	if(RESIZABLE(container->address)) {
		if(MULTIDIMENSIONAL(container->address)) {
			dest[2].address = container->address->dimension_addresses[0];
			dest[2].type = QB_OPERAND_ADDRESS;
			dest[3].address = container->address;
			dest[3].type = QB_OPERAND_SEGMENT_SELECTOR;
			dest[4] = *container;
		} else {
			dest[2].address = container->address;
			dest[2].type = QB_OPERAND_SEGMENT_SELECTOR;
			dest[3] = *container;
		}
	} else {
		// need a predicate, since the unset() might performed on a sub-array (whose existence is not guaranteed)
		qb_address *predicate_address = qb_obtain_predicate_address(cxt, container->address, FALSE);
		dest[2].address = predicate_address;
		dest[2].type = QB_OPERAND_ADDRESS;
		dest[3] = *container;
	}
}

static void qb_transfer_operands_unset_object_property(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_operand *dest, uint32_t dest_count) {
	qb_operand *container = &operands[0], *name = &operands[1];
	qb_address *address = qb_obtain_object_property(cxt, container, name, QB_ARRAY_BOUND_CHECK_ISSET);
	if(SCALAR(address)) {
		dest[0].address = address;
		dest[0].type = QB_OPERAND_ADDRESS;
	} else {
		if(RESIZABLE(address)) {
			if(MULTIDIMENSIONAL(address)) {
				dest[0].address = address->dimension_addresses[0];
				dest[0].type = QB_OPERAND_ADDRESS;
				dest[1].address = address;
				dest[1].type = QB_OPERAND_SEGMENT_SELECTOR;
				dest[2].address = address;
				dest[2].type = QB_OPERAND_ADDRESS;
			} else {
				dest[0].address = address;
				dest[0].type = QB_OPERAND_SEGMENT_SELECTOR;
				dest[1].address = address;
				dest[1].type = QB_OPERAND_ADDRESS;
			}
		} else {
			qb_address *predicate_address = qb_obtain_predicate_address(cxt, container->address, FALSE);
			dest[0].address = predicate_address;
			dest[0].type = QB_OPERAND_ADDRESS;
			dest[1].address = address;
			dest[1].type = QB_OPERAND_ADDRESS;
		}
	}
}

static void qb_transfer_operands_object_property_isset(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_operand *dest, uint32_t dest_count) {
	qb_operand *container = &operands[0], *name = &operands[1];
	qb_address *address = qb_obtain_object_property(cxt, container, name, QB_ARRAY_BOUND_CHECK_ISSET);
	qb_address *predicate_address = qb_obtain_predicate_address(cxt, address, FALSE);
	dest[0].address = SCALAR(address) ? address : address->array_size_address;
	dest[0].type = QB_OPERAND_ADDRESS;
	dest[1].address = predicate_address;
	dest[1].type = QB_OPERAND_ADDRESS;
	dest[2] = *result;
}

static void qb_transfer_operands_result_only(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_operand *dest, uint32_t dest_count) {
	dest[0] = *result;
}

static void qb_transfer_operands_reverse_binary(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_operand *dest, uint32_t dest_count) {
	dest[0] = operands[1];
	dest[1] = operands[0];
	dest[2] = *result;
}

static void qb_transfer_operands_empty_string(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_operand *dest, uint32_t dest_count) {
	dest[0].address = result->address;
	dest[0].type = QB_OPERAND_SEGMENT_SELECTOR;
	dest[1] = *result;
}

static void qb_transfer_operands_add_string(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_operand *dest, uint32_t dest_count) {
	qb_operand *addend = &operands[1];
	dest[0] = *addend;
	if(addend->address->dimension_count > 1) {
		dest[1].type = QB_OPERAND_ADDRESS;
		dest[1].address = qb_retrieve_array_dimensions(cxt, addend->address);
		dest[2].address = result->address;
		dest[2].type = QB_OPERAND_SEGMENT_SELECTOR;
		dest[3] = *result;
	} else {
		dest[1].address = result->address;
		dest[1].type = QB_OPERAND_SEGMENT_SELECTOR;
		dest[2] = *result;
	}
}

static void qb_transfer_operands_print(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_operand *dest, uint32_t dest_count) {
	qb_operand *value = &operands[0];
	dest[0] = *value;
	if(value->address->dimension_count > 1) {
		dest[1].type = QB_OPERAND_ADDRESS;
		dest[1].address = qb_retrieve_array_dimensions(cxt, value->address);
	}
}

static void qb_transfer_operands_foreach_reset(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_operand *dest, uint32_t dest_count) {
	dest[0].address = qb_obtain_constant_S32(cxt, -1);
	dest[0].type = QB_OPERAND_ADDRESS;
	dest[1] = cxt->foreach_index;
}

static void qb_transfer_operands_foreach_fetch(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_operand *dest, uint32_t dest_count) {
	qb_operand *container = &operands[0];
	dest[0].address = container->address->dimension_addresses[0];
	dest[0].type = QB_OPERAND_ADDRESS;
	dest[1] = cxt->foreach_index;
}

static void qb_transfer_operands_return(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_operand *dest, uint32_t dest_count) {
	qb_operand *value = &operands[0];
	if(cxt->return_variable && cxt->return_variable->address != NULL && value->type == QB_OPERAND_ADDRESS && cxt->return_variable->address != value->address) {
		qb_operand assigment_operands[2];
		qb_operand assignment_result;
		qb_variable_dimensions dim;
		qb_copy_address_dimensions(cxt, value->address, 0, &dim);
		qb_attach_bound_checking_expression(cxt, cxt->return_variable->address, &dim, TRUE);
		assigment_operands[0].type = QB_OPERAND_ADDRESS;
		assigment_operands[0].address = cxt->return_variable->address;
		assigment_operands[1].type = QB_OPERAND_ADDRESS;
		assigment_operands[1].address = value->address;
		assignment_result.type = QB_OPERAND_ADDRESS;
		assignment_result.address = cxt->return_variable->address;
		qb_create_op(cxt, &factory_assign, value->address->type, assigment_operands, 2, &assignment_result, NULL, 0, TRUE);
	}
}

static void qb_transfer_operands_increment(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_operand *dest, uint32_t dest_count) {
	qb_operand *variable = &operands[0];

	// perform the assignment if there's an address
	if(result->type == QB_OPERAND_ADDRESS) {
		qb_operand assigment_operands[2];
		assigment_operands[0].type = QB_OPERAND_ADDRESS;
		assigment_operands[0].address = result->address;
		assigment_operands[1].type = QB_OPERAND_ADDRESS;
		assigment_operands[1].address = variable->address;
		qb_create_op(cxt, &factory_assign, variable->address->type, assigment_operands, 2, result, NULL, 0, TRUE);
	}
	dest[0] = *variable;
}

static void qb_transfer_operands_increment_object_property(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_operand *dest, uint32_t dest_count) {
	qb_operand *container = &operands[0], *name = &operands[1];
	qb_address *variable_address = qb_obtain_object_property(cxt, container, name, QB_ARRAY_BOUND_CHECK_WRITE);
	if(result->type == QB_OPERAND_ADDRESS) {
		if(result->address != variable_address) {
			qb_operand assigment_operands[2];
			assigment_operands[0].type = QB_OPERAND_ADDRESS;
			assigment_operands[0].address = result->address;
			assigment_operands[1].type = QB_OPERAND_ADDRESS;
			assigment_operands[1].address = variable_address;
			qb_create_op(cxt, &factory_assign, variable_address->type, assigment_operands, 2, result, NULL, 0, TRUE);
		}
	}
	dest[0].address = variable_address;
	dest[0].type = QB_OPERAND_ADDRESS;
}

static void qb_transfer_operands_modify_assign(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_operand *dest, uint32_t dest_count) {
	dest[0] = *result;
	dest[1] = operands[operand_count - 1];
	dest[2] = *result;
}

static void qb_transfer_operands_gather(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_operand *dest, uint32_t dest_count) {
	dest[0] = operands[2];
	dest[1] = operands[1];
	dest[2] = *result;
}

static void qb_transfer_operands_fork(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_operand *dest, uint32_t dest_count) {
	if(operand_count > 0) {
		dest[0] = operands[0];
	} else {
		dest[0].address = qb_obtain_constant_U32(cxt, 0);
		dest[0].type = QB_OPERAND_ADDRESS;
	}
}

static void qb_transfer_operands_rand(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_operand *dest, uint32_t dest_count) {
	if(operand_count == 2) {
		dest[0] = operands[0];
		dest[1] = operands[1];
	} else {
		qb_primitive_type expr_type = result->address->type;
		dest[0].address = qb_obtain_constant(cxt, 0, expr_type);
		dest[0].type = QB_OPERAND_ADDRESS;
		dest[1].address = qb_obtain_constant(cxt, (int64_t) integer_upper_bounds[expr_type], expr_type);
		dest[1].type = QB_OPERAND_ADDRESS;
	}
	dest[2] = *result;
}

static void qb_transfer_operands_round(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_operand *dest, uint32_t dest_count) {
	qb_operand *value = &operands[0], *precision = &operands[1], *mode = &operands[2];
	dest[0] = *value;
	dest[1].address = (precision->type == QB_OPERAND_ADDRESS) ? precision->address : qb_obtain_constant_S32(cxt, 0);
	dest[1].type = QB_OPERAND_ADDRESS;
	dest[2].address = (mode->type == QB_OPERAND_ADDRESS) ? mode->address : qb_obtain_constant_S32(cxt, 1);		// PHP_ROUND_HALF_UP
	dest[2].type = QB_OPERAND_ADDRESS;
	dest[3] = *result;
}

static void qb_transfer_operands_reciprocal(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_operand *dest, uint32_t dest_count) {
	qb_operand *value = &operands[0];
	dest[0].address = qb_obtain_constant(cxt, 1, value->address->type);
	dest[0].type = QB_OPERAND_ADDRESS;
	dest[1] = *value;
	dest[2] = *result;
}

static void qb_transfer_operands_one_vector(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_operand *dest, uint32_t dest_count) {
	dest[0] = operands[0];
	if(dest_count == 3) {
		qb_address *address1 = operands[0].address;		
		dest[1].type = QB_OPERAND_ADDRESS;
		dest[1].address = DIMENSION_ADDRESS(address1, -1);
		dest[2] = *result;
	} else {
		dest[1] = *result;
	}
}

static void qb_transfer_operands_two_vectors(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_operand *dest, uint32_t dest_count) {
	dest[0] = operands[0];
	dest[1] = operands[1];
	if(dest_count == 4) {
		// need the dimension
		qb_address *address1 = operands[0].address, *address2 = operands[1].address;		
		dest[2].type = QB_OPERAND_ADDRESS;
		dest[2].address = DIMENSION_ADDRESS(address1, -1);
		dest[3] = *result;
	} else {
		dest[2] = *result;
	}
}

static void qb_transfer_operands_refract(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_operand *dest, uint32_t dest_count) {
	dest[0] = operands[0];
	dest[1] = operands[1];
	dest[2] = operands[2];
	if(dest_count == 5) {
		// need the dimension
		qb_address *address1 = operands[0].address, *address2 = operands[1].address;		
		dest[3].type = QB_OPERAND_ADDRESS;
		dest[3].address = DIMENSION_ADDRESS(address1, -1);
		dest[4] = *result;
	} else {
		dest[3] = *result;
	}
}

static void qb_transfer_operands_tranpose(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_operand *dest, uint32_t dest_count) {
	dest[0] = operands[0];
	if(dest_count == 4) {
		qb_address *address = operands[0].address;
		dest[1].address = DIMENSION_ADDRESS(address, -2);
		dest[1].type = QB_OPERAND_ADDRESS;
		dest[2].address = DIMENSION_ADDRESS(address, -1);
		dest[2].type = QB_OPERAND_ADDRESS;
		dest[3] = *result;
	} else {
		dest[1] = *result;
	}
}

static void qb_transfer_operands_square_matrix(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_operand *dest, uint32_t dest_count) {
	dest[0] = operands[0];
	if(dest_count == 3) {
		qb_address *address = operands[0].address;
		dest[1].address = DIMENSION_ADDRESS(address, -1);
		dest[1].type = QB_OPERAND_ADDRESS;
		dest[2] = *result;
	} else {
		dest[1] = *result;
	}
}

static void qb_transfer_operands_mm_mult_cm(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_operand *dest, uint32_t dest_count) {
	dest[0] = operands[0];
	dest[1] = operands[1];
	if(dest_count == 6) {
		qb_address *m1_address = operands[0].address;
		qb_address *m2_address = operands[1].address;
		qb_address *m1_row_address = DIMENSION_ADDRESS(m1_address, -1);
		qb_address *m1_col_address = DIMENSION_ADDRESS(m1_address, -2);
		qb_address *m2_row_address = DIMENSION_ADDRESS(m2_address, -1);
		qb_address *m2_col_address = DIMENSION_ADDRESS(m2_address, -2);
		dest[2].address = m1_row_address;
		dest[2].type = QB_OPERAND_ADDRESS;
		dest[3].address = m1_col_address;
		dest[3].type = QB_OPERAND_ADDRESS;
		dest[4].address = m2_col_address;
		dest[4].type = QB_OPERAND_ADDRESS;
		dest[5] = *result;
	} else {
		dest[2] = *result;
	}
}

static void qb_transfer_operands_mv_mult_cm(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_operand *dest, uint32_t dest_count) {
	dest[0] = operands[0];
	dest[1] = operands[1];
	if(dest_count == 5) {
		qb_address *m1_address = operands[0].address;
		qb_address *m2_address = operands[1].address;
		qb_address *m1_row_address = DIMENSION_ADDRESS(m1_address, -1);
		qb_address *m1_col_address = DIMENSION_ADDRESS(m1_address, -2);
		qb_address *m2_row_address = DIMENSION_ADDRESS(m2_address, -1);
		dest[2].address = m1_row_address;
		dest[2].type = QB_OPERAND_ADDRESS;
		dest[3].address = m1_col_address;
		dest[3].type = QB_OPERAND_ADDRESS;
		dest[4] = *result;
	} else {
		dest[2] = *result;
	}
}

static void qb_transfer_operands_vm_mult_cm(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_operand *dest, uint32_t dest_count) {
	dest[0] = operands[0];
	dest[1] = operands[1];
	if(dest_count == 5) {
		qb_address *m1_address = operands[0].address;
		qb_address *m2_address = operands[1].address;
		qb_address *m1_col_address = DIMENSION_ADDRESS(m1_address, -1);
		qb_address *m2_row_address = DIMENSION_ADDRESS(m2_address, -1);
		qb_address *m2_col_address = DIMENSION_ADDRESS(m2_address, -2);
		dest[2].address = m2_row_address;
		dest[2].type = QB_OPERAND_ADDRESS;
		dest[3].address = m2_col_address;
		dest[3].type = QB_OPERAND_ADDRESS;
		dest[4] = *result;
	} else {
		dest[2] = *result;
	}
}

static void qb_transfer_operands_transpose_equivalent(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_operand *dest, uint32_t dest_count) {
	qb_derived_op_factory *df = (qb_derived_op_factory *) f;
	qb_operand reversed_operands[2];
	reversed_operands[0] = operands[1];
	reversed_operands[1] = operands[0];
	f = df->parent;
	f->transfer_operands(cxt, f, reversed_operands, 2, result, dest, dest_count);
}

static void qb_transfer_operands_matrix_current_mode(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_operand *dest, uint32_t dest_count) {
	qb_matrix_op_factory_selector *s = (qb_matrix_op_factory_selector *) f;
	if(cxt->matrix_order == QB_MATRIX_ORDER_COLUMN_MAJOR) {
		f = s->cm_factory;
	} else {
		f = s->rm_factory;
	}
	f->transfer_operands(cxt, f, operands, operand_count, result, dest, dest_count);
}

static void qb_transfer_operands_sampling(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_operand *dest, uint32_t dest_count) {
	qb_operand *image = &operands[0], *x = &operands[1], *y = &operands[2];
	dest[0] = *image;
	dest[1].address = image->address->dimension_addresses[1];
	dest[1].type = QB_OPERAND_ADDRESS;
	dest[2].address = image->address->dimension_addresses[0];
	dest[2].type = QB_OPERAND_ADDRESS;
	dest[3] = *x;
	dest[4] = *y;
	dest[5] = *result;
}

static void qb_transfer_operands_sampling_vector(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_operand *dest, uint32_t dest_count) {
	qb_operand *image = &operands[0], *coordinates = &operands[1];
	dest[0] = *image;
	dest[1].address = image->address->dimension_addresses[1];
	dest[1].type = QB_OPERAND_ADDRESS;
	dest[2].address = image->address->dimension_addresses[0];
	dest[2].type = QB_OPERAND_ADDRESS;
	dest[3].address = qb_obtain_array_element(cxt, coordinates->address, cxt->zero_address, QB_ARRAY_BOUND_CHECK_READ);
	dest[3].type = QB_OPERAND_ADDRESS;
	dest[4].address = qb_obtain_array_element(cxt, coordinates->address, cxt->one_address, QB_ARRAY_BOUND_CHECK_READ);
	dest[4].type = QB_OPERAND_ADDRESS;
	dest[5] = *result;
}

static void qb_transfer_operands_array_column(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_operand *dest, uint32_t dest_count) {
	qb_operand *container = &operands[0], *column_index = &operands[1];
	dest[0] = *container;
	dest[1].address = container->address->dimension_addresses[1];
	dest[1].type = QB_OPERAND_ADDRESS;
	dest[2].address = (container->address->dimension_count > 2) ? container->address->array_size_addresses[2] : cxt->one_address;
	dest[2].type = QB_OPERAND_ADDRESS;
	dest[3] = *column_index;
	dest[4] = *result;
}

static void qb_transfer_operands_array_diff(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_operand *dest, uint32_t dest_count) {
	qb_operand *operand1 = &operands[0], *operand2 = &operands[1];
	qb_address *width_address = (operand1->address->dimension_count > 1) ? operand1->address->array_size_addresses[1] : cxt->one_address;
	dest[0] = *operand1;
	dest[1] = *operand2;
	dest[2].address = width_address;
	dest[2].type = QB_OPERAND_ADDRESS;
	dest[3] = *result;
}

static void qb_transfer_operands_array_fill(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_operand *dest, uint32_t dest_count) {
	qb_operand *start_index = &operands[0], *value = &operands[2];
	dest[0] = *start_index;
	dest[1] = *value;
	dest[2] = *result;
}

static void qb_transfer_operands_array_pos(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_operand *dest, uint32_t dest_count) {
	qb_operand *container = &operands[0], *subarray = &operands[1], *offset = &operands[2];
	dest[0] = *container;
	dest[1] = *subarray;
	dest[2].address = (offset->type == QB_OPERAND_ADDRESS) ? offset->address : cxt->zero_address;
	dest[2].type = QB_OPERAND_ADDRESS;
	dest[3] = *result;
}

static void qb_transfer_operands_array_rand(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_operand *dest, uint32_t dest_count) {
	qb_operand *container = &operands[0], *count = &operands[1];
	dest[0].address = DIMENSION_ADDRESS(container->address, 0);
	dest[0].type = QB_OPERAND_ADDRESS;
	if(count->type == QB_OPERAND_ADDRESS) {
		dest[1] = *count;
	} else {
		dest[1].address = cxt->one_address;
		dest[1].type = QB_OPERAND_ADDRESS;
	}
	dest[2] = *result;
}

static void qb_transfer_operands_array_resize(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_operand *dest, uint32_t dest_count) {
	qb_operand *container = &operands[0];
	uint32_t i; 
	for(i = 0; i < container->address->dimension_count; i++) {
		qb_address *new_dimension_address = (i + 1 < operand_count) ? operands[i + 1].address : container->address->dimension_addresses[i];
		dest[i * 3 + 0].address = new_dimension_address;
		dest[i * 3 + 0].type = QB_OPERAND_ADDRESS;
		dest[i * 3 + 1].address = container->address->array_size_addresses[i];
		dest[i * 3 + 1].type = QB_OPERAND_ADDRESS;
		dest[i * 3 + 2].address = container->address->dimension_addresses[i];
		dest[i * 3 + 2].type = QB_OPERAND_ADDRESS;
	}
	dest[i * 3 + 0].address = container->address;
	dest[i * 3 + 0].type = QB_OPERAND_SEGMENT_SELECTOR;
	dest[i * 3 + 1] = *container;
}

static void qb_transfer_operands_array_reverse(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_operand *dest, uint32_t dest_count) {
	qb_operand *container = &operands[0];
	dest[0] = *container;
	dest[1].address = (container->address->dimension_count > 1) ? container->address->array_size_addresses[1] : cxt->one_address;
	dest[1].type = QB_OPERAND_ADDRESS;
	dest[2] = *result;
}

static void qb_transfer_operands_array_slice(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_operand *dest, uint32_t dest_count) {
	qb_operand *container = &operands[0], *offset = &operands[1], *length = (operand_count >= 3) ? &operands[2] : NULL;
	dest[0] = *offset;
	if(length) {
		dest[1] = *length;
	} else {
		dest[1].address = qb_obtain_constant_S32(cxt, INT32_MAX);
		dest[1].type = QB_OPERAND_ADDRESS;
	}
	dest[2].address = container->address->dimension_addresses[0];
	dest[2].type = QB_OPERAND_ADDRESS;
	dest[3].address = (container->address->dimension_count > 1) ? container->address->dimension_addresses[1] : cxt->one_address;
	dest[3].type = QB_OPERAND_ADDRESS;
	dest[4] = *container;
	dest[5] = *result;
}

static void qb_transfer_operands_array_slice_count(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_operand *dest, uint32_t dest_count) {
	qb_operand *container = &operands[0], *offset = &operands[1], *length = (operand_count >= 3) ? &operands[2] : NULL;
	dest[0] = *offset;
	if(length) {
		dest[1] = *length;
	} else {
		dest[1].address = qb_obtain_constant_S32(cxt, INT32_MAX);
		dest[1].type = QB_OPERAND_ADDRESS;
	}
	dest[2].address = container->address->dimension_addresses[0];
	dest[2].type = QB_OPERAND_ADDRESS;
	dest[3] = *result;
}

static void qb_transfer_operands_range(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_operand *dest, uint32_t dest_count) {
	qb_operand *start = &operands[0], *end = &operands[1], *interval = &operands[2];
	dest[0] = *start;
	dest[1] = *end;
	if(operand_count == 3) {
		dest[2] = *interval;
	} else {
		dest[2].address = qb_obtain_constant(cxt, 1, start->address->type);
		dest[2].type = QB_OPERAND_ADDRESS;
	}
	dest[3] = *result;
}

static void qb_transfer_operands_sort(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_operand *dest, uint32_t dest_count) {
	qb_operand *container = &operands[0];
	dest[0].address = (container->address->dimension_count > 1) ? container->address->array_size_addresses[1] : cxt->one_address;
	dest[0].type = QB_OPERAND_ADDRESS;
	dest[1] = *container;
}

static void qb_transfer_operands_unpack(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_operand *dest, uint32_t dest_count) {
	qb_operand *string = &operands[0], *index = &operands[1];
	qb_address *substring_address;
	if(index->type == QB_OPERAND_ADDRESS) {
		uint32_t length = BYTE_COUNT(1, result->address->type);
		qb_address *length_address = qb_obtain_constant_U32(cxt, length);
		substring_address = qb_obtain_array_slice(cxt, string->address, index->address, length_address, QB_ARRAY_BOUND_CHECK_READ);
	} else {
		substring_address = string->address;
	}
	dest[0].address = substring_address;
	dest[0].type = QB_OPERAND_ADDRESS;
	dest[1] = *result;
}

static void qb_transfer_operands_function_call(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_operand *dest, uint32_t dest_count) {
	USE_TSRM
	qb_operand *func = &operands[0], *arguments = &operands[1], *argument_count = &operands[2];
	const char *func_name = func->zend_function->common.function_name;
	qb_external_symbol_type symbol_type = (func->type == QB_OPERAND_STATIC_ZEND_FUNCTION) ? QB_EXT_SYM_STATIC_ZEND_FUNCTION : QB_EXT_SYM_ZEND_FUNCTION;
	uint32_t func_name_len = strlen(func_name);
	uint32_t func_index = qb_import_external_symbol(symbol_type, func_name, func_name_len, func->zend_function TSRMLS_CC);
	uint32_t *var_indices, ret_index = INVALID_INDEX;
	uint32_t i;
	ALLOCA_FLAG(use_heap);

	var_indices = do_alloca(sizeof(uint32_t *) * argument_count->number, use_heap);
	for(i = 0; i < (uint32_t) argument_count->number; i++) {
		qb_operand *arg = &arguments->arguments[i];
		var_indices[i] = qb_get_variable_index(cxt, arg->address);
	}

	if(result->type == QB_OPERAND_ADDRESS && result->address != cxt->zero_address) {
		ret_index = qb_get_variable_index(cxt, result->address);
		qb_mark_as_writable(cxt, result->address);
	}

	dest[0].address = qb_obtain_constant_U32(cxt, func_index);
	dest[0].type = QB_OPERAND_ADDRESS;
	dest[1].address = qb_obtain_constant_indices(cxt, var_indices, argument_count->number);
	dest[1].type = QB_OPERAND_ADDRESS;
	dest[2].address = qb_obtain_constant_U32(cxt, ret_index);
	dest[2].type = QB_OPERAND_ADDRESS;

	free_alloca(var_indices, use_heap);
}
