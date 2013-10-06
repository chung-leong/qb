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

static void qb_transfer_operands_derived(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_operand *dest, uint32_t dest_count) {
	qb_derived_op_factory *df = (qb_derived_op_factory *) f;
	f = df->parent;
	f->transfer_operands(cxt, f, operands, operand_count, result, dest, dest_count);
}

static void qb_transfer_operands_assign(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_operand *dest, uint32_t dest_count) {
	dest[0] = operands[operand_count - 1];
	dest[1] = *result;
}

static void qb_transfer_operands_guard_array_index(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_operand *dest, uint32_t dest_count) {
	qb_operand *container = &operands[0], *index = &operands[1];
	qb_address *index_limit_address = container->address->dimension_addresses[0];
	dest[0].address = index->address;
	dest[0].type = QB_OPERAND_ADDRESS;
	dest[1].address = index_limit_address;
	dest[1].type = QB_OPERAND_ADDRESS;
}

static void qb_transfer_operands_guard_array_index_add(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_operand *dest, uint32_t dest_count) {
	qb_operand *container = &operands[0], *index = &operands[1];
	qb_address *index_limit_address = container->address->dimension_addresses[0];
	qb_address *offset_address = container->address->array_index_address;
	dest[0].address = index->address;
	dest[0].type = QB_OPERAND_ADDRESS;
	dest[1].address = index_limit_address;
	dest[1].type = QB_OPERAND_ADDRESS;
	dest[2].address = offset_address;
	dest[2].type = QB_OPERAND_ADDRESS;
	dest[3] = *result;
}

static void qb_transfer_operands_guard_array_index_multiply(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_operand *dest, uint32_t dest_count) {
	qb_operand *container = &operands[0], *index = &operands[1];
	qb_address *index_limit_address = container->address->dimension_addresses[0];
	qb_address *sub_array_size_address = container->address->array_size_addresses[1];
	dest[0].address = index->address;
	dest[0].type = QB_OPERAND_ADDRESS;
	dest[1].address = index_limit_address;
	dest[1].type = QB_OPERAND_ADDRESS;
	dest[2].address = sub_array_size_address;
	dest[2].type = QB_OPERAND_ADDRESS;
	dest[3] = *result;
}

static void qb_transfer_operands_guard_array_index_multiply_add(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_operand *dest, uint32_t dest_count) {
	qb_operand *container = &operands[0], *index = &operands[1];
	qb_address *index_limit_address = container->address->dimension_addresses[0];
	qb_address *sub_array_size_address = container->address->array_size_addresses[1];
	qb_address *offset_address = container->address->array_index_address;
	dest[0].address = index->address;
	dest[0].type = QB_OPERAND_ADDRESS;
	dest[1].address = index_limit_address;
	dest[1].type = QB_OPERAND_ADDRESS;
	dest[2].address = sub_array_size_address;
	dest[2].type = QB_OPERAND_ADDRESS;
	dest[3].address = offset_address;
	dest[3].type = QB_OPERAND_ADDRESS;
	dest[4] = *result;
}

static void qb_transfer_operands_guard_array_size(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_operand *dest, uint32_t dest_count) {
	qb_operand *container = &operands[0], *size = &operands[1];
	qb_address *array_size_address = (container->address->array_size_address) ? container->address->array_size_address : cxt->one_address;
	dest[0].address = size->address;
	dest[0].type = QB_OPERAND_ADDRESS;
	dest[1].address = array_size_address;
	dest[1].type = QB_OPERAND_ADDRESS;
}

static void qb_transfer_operands_check_array_index_add(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_operand *dest, uint32_t dest_count) {
	qb_operand *container = &operands[0], *index = &operands[1], *predicate = &operands[2];
	qb_address *index_limit_address = container->address->dimension_addresses[0];
	qb_address *offset_address = container->address->array_index_address;
	dest[0].address = index->address;
	dest[0].type = QB_OPERAND_ADDRESS;
	dest[1].address = index_limit_address;
	dest[1].type = QB_OPERAND_ADDRESS;
	dest[2].address = offset_address;
	dest[2].type = QB_OPERAND_ADDRESS;
	dest[3] = *predicate;
	dest[4] = *result;
}

static void qb_transfer_operands_check_array_index_multiply(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_operand *dest, uint32_t dest_count) {
	qb_operand *container = &operands[0], *index = &operands[1], *predicate = &operands[2];
	qb_address *index_limit_address = container->address->dimension_addresses[0];
	qb_address *sub_array_size_address = container->address->array_size_addresses[1];
	dest[0].address = index->address;
	dest[0].type = QB_OPERAND_ADDRESS;
	dest[1].address = index_limit_address;
	dest[1].type = QB_OPERAND_ADDRESS;
	dest[2].address = sub_array_size_address;
	dest[2].type = QB_OPERAND_ADDRESS;
	dest[3] = *predicate;
	dest[4] = *result;
}

static void qb_transfer_operands_check_array_index_multiply_add(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_operand *dest, uint32_t dest_count) {
	qb_operand *container = &operands[0], *index = &operands[1], *predicate = &operands[2];
	qb_address *index_limit_address = container->address->dimension_addresses[0];
	qb_address *sub_array_size_address = container->address->array_size_addresses[1];
	qb_address *offset_address = container->address->array_index_address;
	dest[0].address = index->address;
	dest[0].type = QB_OPERAND_ADDRESS;
	dest[1].address = index_limit_address;
	dest[1].type = QB_OPERAND_ADDRESS;
	dest[2].address = sub_array_size_address;
	dest[2].type = QB_OPERAND_ADDRESS;
	dest[3].address = offset_address;
	dest[3].type = QB_OPERAND_ADDRESS;
	dest[4] = *predicate;
	dest[5] = *result;
}

static void qb_transfer_operands_accommodate_array_index(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_operand *dest, uint32_t dest_count) {
	qb_operand *container = &operands[0], *index = &operands[1];
	qb_address *index_limit_address = container->address->dimension_addresses[0];
	dest[0].address = index->address;
	dest[0].type = QB_OPERAND_ADDRESS;
	dest[1].address = index_limit_address;
	dest[1].type = QB_OPERAND_ADDRESS;
	dest[2].address = container->address;
	dest[2].type = QB_OPERAND_SEGMENT_SELECTOR;
	dest[3].number = BYTE_COUNT(1, container->address->type);
	dest[3].type = QB_OPERAND_NUMBER;
}

static void qb_transfer_operands_accommodate_array_index_multiply(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_operand *dest, uint32_t dest_count) {
	qb_operand *container = &operands[0], *index = &operands[1];
	qb_address *index_limit_address = container->address->dimension_addresses[0];
	qb_address *array_size_address = container->address->array_size_addresses[0];
	qb_address *sub_array_size_address = container->address->array_size_addresses[1];
	dest[0].address = index->address;
	dest[0].type = QB_OPERAND_ADDRESS;
	dest[1].address = index_limit_address;
	dest[1].type = QB_OPERAND_ADDRESS;
	dest[2].address = sub_array_size_address;
	dest[2].type = QB_OPERAND_ADDRESS;
	dest[3].address = array_size_address;
	dest[3].type = QB_OPERAND_ADDRESS;
	dest[4].address = container->address;
	dest[4].type = QB_OPERAND_SEGMENT_SELECTOR;
	dest[5].number = BYTE_COUNT(1, container->address->type);
	dest[5].type = QB_OPERAND_NUMBER;
	dest[6] = *result;
}

static void qb_transfer_operands_accommodate_array_push(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_operand *dest, uint32_t dest_count) {
	qb_operand *container = &operands[0];
	qb_address *array_size_address = container->address->array_size_address;
	dest[0].address = array_size_address;
	dest[0].type = QB_OPERAND_ADDRESS;
	dest[1].address = container->address;
	dest[1].type = QB_OPERAND_SEGMENT_SELECTOR;
	dest[2].number = BYTE_COUNT(1, container->address->type);
	dest[2].type = QB_OPERAND_NUMBER;
	dest[3] = *result;
}

static void qb_transfer_operands_accommodate_array_size(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_operand *dest, uint32_t dest_count) {
	qb_operand *container = &operands[0], *size = &operands[1];
	qb_address *array_size_address = container->address->array_size_addresses[0];
	dest[0].address = size->address;
	dest[0].type = QB_OPERAND_ADDRESS;
	dest[1].address = array_size_address;
	dest[1].type = QB_OPERAND_ADDRESS;
	dest[2].address = container->address;
	dest[2].type = QB_OPERAND_SEGMENT_SELECTOR;
	dest[3].number = BYTE_COUNT(1, container->address->type);
	dest[3].type = QB_OPERAND_NUMBER;
}

static void qb_transfer_operands_accommodate_array_size_update_dimension(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_operand *dest, uint32_t dest_count) {
	qb_operand *container = &operands[0], *size = &operands[1];
	qb_address *array_size_address = container->address->array_size_addresses[0];
	qb_address *dimension_address = container->address->dimension_addresses[0];
	qb_address *sub_array_size_address = container->address->array_size_addresses[1];
	dest[0].address = size->address;
	dest[0].type = QB_OPERAND_ADDRESS;
	dest[1].address = array_size_address;
	dest[1].type = QB_OPERAND_ADDRESS;
	dest[2].address = dimension_address;
	dest[2].type = QB_OPERAND_ADDRESS;
	dest[3].address = sub_array_size_address;
	dest[3].type = QB_OPERAND_ADDRESS;
	dest[4].address = container->address;
	dest[4].type = QB_OPERAND_SEGMENT_SELECTOR;
	dest[5].number = BYTE_COUNT(1, container->address->type);
	dest[5].type = QB_OPERAND_NUMBER;
}

static void qb_transfer_operands_choose_size_of_larger_array(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_operand *dest, uint32_t dest_count) {
	qb_operand *array1_size = &operands[0], *array1_value = &operands[1];
	qb_operand *array2_size = &operands[2], *array2_value = &operands[3];
	dest[0].address = array1_size->address;
	dest[0].type = QB_OPERAND_ADDRESS;
	dest[1].address = array1_value->address;
	dest[1].type = QB_OPERAND_ADDRESS;
	dest[2].address = array2_size->address;
	dest[2].type = QB_OPERAND_ADDRESS;
	dest[3].address = array2_value->address;
	dest[3].type = QB_OPERAND_ADDRESS;
	dest[4] = *result;
}

static void qb_transfer_operands_choose_size_of_larger_array_top_level(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_operand *dest, uint32_t dest_count) {
	qb_operand *array1_size = &operands[0], *array2_size = &operands[2];
	dest[0].address = array1_size->address;
	dest[0].type = QB_OPERAND_ADDRESS;
	dest[1].address = array2_size->address;
	dest[1].type = QB_OPERAND_ADDRESS;
	dest[2] = *result;
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

static void qb_transfer_operands_isset_array_element(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_operand *dest, uint32_t dest_count) {
	qb_operand *container = &operands[0], *index = &operands[1];
	qb_address *variable_address, *predicate_address;

	if(container->address->dimension_count == 1) {
		variable_address = qb_obtain_array_element(cxt, container->address, index->address, QB_ARRAY_BOUND_CHECK_ISSET);
	} else {
		variable_address = container->address->array_size_addresses[1];
	}
	predicate_address = qb_find_predicate_address(cxt, container->address);
	if(!predicate_address) {
		// if there's no bound check was done then just use true
		predicate_address = cxt->true_address;
	}

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
			// need a predicate, since the unset() might performed on a sub-array (whose existence is not guaranteed)
			qb_address *predicate_address = qb_find_predicate_address(cxt, variable->address);
			if(!predicate_address) {
				// just use true if there isn't one
				predicate_address = cxt->true_address;
			}
			dest[0].address = predicate_address;
			dest[1].type = QB_OPERAND_ADDRESS;
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
		dest[2].address = container->address;
		dest[2].type = QB_OPERAND_SEGMENT_SELECTOR;
		dest[3] = *container;
	} else {
		qb_address *predicate_address = qb_find_predicate_address(cxt, container->address);
		if(!predicate_address) {
			predicate_address = cxt->true_address;
		}
		dest[2].address = predicate_address;
		dest[2].type = QB_OPERAND_ADDRESS;
		dest[3] = *container;
	}
}

static void qb_transfer_operands_isset_object_property(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_operand *dest, uint32_t dest_count) {
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
		assigment_operands[0].type = QB_OPERAND_ADDRESS;
		assigment_operands[0].address = cxt->return_variable->address;
		assigment_operands[1].type = QB_OPERAND_ADDRESS;
		assigment_operands[1].address = value->address;
		assignment_result.type = QB_OPERAND_ADDRESS;
		assignment_result.address = cxt->return_variable->address;
		qb_create_op(cxt, &factory_assign, assigment_operands, 2, &assignment_result, NULL, 0, TRUE);
	}
}

static void qb_transfer_operands_minmax(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_operand *dest, uint32_t dest_count) {
	if(operand_count == 1) {
		dest[0] = operands[0];
		dest[1] = *result;
	} else {
		if(operand_count == 2) {
			dest[0] = operands[0];
			dest[1] = operands[1];
			dest[2] = *result;
		} else {
			qb_create_op(cxt, f, operands, operand_count - 1, result, NULL, 0, TRUE);
			dest[0] = *result;
			dest[1] = operands[operand_count - 1];
			dest[2] = *result;
		}
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
		qb_create_op(cxt, &factory_assign, assigment_operands, 2, result, NULL, 0, TRUE);
	}
	dest[0] = *variable;
}

static void qb_transfer_operands_modify_assign(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_operand *dest, uint32_t dest_count) {
	dest[0] = *result;
	dest[1] = operands[operand_count - 1];
	dest[2] = *result;
}

static void qb_transfer_operands_one_vector(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_operand *dest, uint32_t dest_count) {
	qb_operand *vector = &operands[0];
	dest[0] = *vector;
	if(dest_count == 3) {
		dest[1].type = QB_OPERAND_ADDRESS;
		dest[1].address = vector->address;
		dest[2] = *result;
	} else {
		dest[1] = *result;
	}
}

static void qb_transfer_operands_two_vector(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_operand *dest, uint32_t dest_count) {
	dest[0] = operands[0];
	if(dest_count == 3) {
		// need the dimension
		qb_address *address = operands[0].address;
		dest[1].type = QB_OPERAND_ADDRESS;
		dest[1].address = address->dimension_addresses[address->dimension_count - 1];
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
		dest[2].address = address1->dimension_addresses[address1->dimension_count - 1];
		dest[3] = *result;

		if(address1->dimension_addresses[address1->dimension_count - 1] != address2->dimension_addresses[address2->dimension_count - 1]) {
			// TODO add check for equality
		}
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
		dest[3].address = address1->dimension_addresses[address1->dimension_count - 1];
		dest[4] = *result;

		if(address1->dimension_addresses[address1->dimension_count - 1] != address2->dimension_addresses[address2->dimension_count - 1]) {
			// TODO: add check for equality
		}
	} else {
		dest[3] = *result;
	}
}

static void qb_transfer_operands_tranpose(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_operand *dest, uint32_t dest_count) {
	dest[0] = operands[0];
	if(dest_count == 4) {
		qb_address *address = operands[0].address;
		dest[1].address = address->dimension_addresses[address->dimension_count - 2];
		dest[1].type = QB_OPERAND_ADDRESS;
		dest[2].address = address->dimension_addresses[address->dimension_count - 1];
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
		dest[1].address = address->dimension_addresses[address->dimension_count - 1];
		dest[1].type = QB_OPERAND_ADDRESS;
		dest[2] = *result;

		if(address->dimension_addresses[address->dimension_count - 1] != address->dimension_addresses[address->dimension_count - 2]) {
			// TODO: add check for squareness
		}
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
		qb_address *m1_row_address = m1_address->dimension_addresses[m1_address->dimension_count - 1];
		qb_address *m1_col_address = m1_address->dimension_addresses[m1_address->dimension_count - 2];
		qb_address *m2_row_address = m2_address->dimension_addresses[m2_address->dimension_count - 1];
		qb_address *m2_col_address = m2_address->dimension_addresses[m2_address->dimension_count - 2];
		dest[2].address = m1_row_address;
		dest[2].type = QB_OPERAND_ADDRESS;
		dest[3].address = m1_col_address;
		dest[3].type = QB_OPERAND_ADDRESS;
		dest[4].address = m2_col_address;
		dest[4].type = QB_OPERAND_ADDRESS;
		dest[5] = *result;

		if(m1_col_address != m2_row_address) {
			// TODO: add check for equality
		}
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
		qb_address *m1_row_address = m1_address->dimension_addresses[m1_address->dimension_count - 1];
		qb_address *m1_col_address = m1_address->dimension_addresses[m1_address->dimension_count - 2];
		qb_address *m2_row_address = m2_address->dimension_addresses[m2_address->dimension_count - 1];
		dest[2].address = m1_row_address;
		dest[2].type = QB_OPERAND_ADDRESS;
		dest[3].address = m1_col_address;
		dest[3].type = QB_OPERAND_ADDRESS;
		dest[4] = *result;

		if(m1_col_address != m2_row_address) {
			// TODO: add check for equality
		}
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
		qb_address *m1_col_address = m2_address->dimension_addresses[m2_address->dimension_count - 1];
		qb_address *m2_row_address = m1_address->dimension_addresses[m1_address->dimension_count - 1];
		qb_address *m2_col_address = m1_address->dimension_addresses[m1_address->dimension_count - 2];
		dest[2].address = m2_row_address;
		dest[2].type = QB_OPERAND_ADDRESS;
		dest[3].address = m2_col_address;
		dest[3].type = QB_OPERAND_ADDRESS;
		dest[4] = *result;

		if(m1_col_address != m2_row_address) {
			// TODO: add check for equality
		}
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
	qb_operand *image = &operands[0];
	dest[0] = operands[0];
	dest[1].address = image->address->dimension_addresses[0];
	dest[1].type = QB_OPERAND_ADDRESS;
	dest[2].address = image->address->dimension_addresses[1];
	dest[2].type = QB_OPERAND_ADDRESS;
	dest[3] = operands[1];
	dest[4] = operands[2];
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

static void qb_transfer_operands_sort(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_operand *dest, uint32_t dest_count) {
	qb_operand *container = &operands[0];
	dest[0].address = (container->address->dimension_count > 1) ? container->address->array_size_addresses[1] : cxt->one_address;
	dest[0].type = QB_OPERAND_ADDRESS;
	dest[1] = *container;
}

static void qb_transfer_operands_intrinsic(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_operand *dest, uint32_t dest_count) {
	qb_operand *func = &operands[0], *arguments = &operands[1], *argument_count = &operands[2];
	f = func->intrinsic_function->extra;
	if(f->transfer_operands) {
		f->transfer_operands(cxt, f, arguments->arguments, argument_count->number, result, dest, dest_count);
	}
}

static uint32_t qb_get_operand_count_intrinsic(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count) {
	qb_operand *func = &operands[0], *arguments = &operands[1], *argument_count = &operands[2];
	f = func->intrinsic_function->extra;
	if(f->get_operand_count) {
		return f->get_operand_count(cxt, f, operands, operand_count);
	}
	return 0;
}

