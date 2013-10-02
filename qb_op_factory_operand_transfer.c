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

static void qb_transfer_operands_assign(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_operand *dest, uint32_t dest_count) {
	dest[0] = operands[operand_count - 1];
	dest[1] = *result;
}

static void qb_transfer_operands_bound_check_array(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_operand *dest, uint32_t dest_count) {
	qb_operand *variable = &operands[1];
	dest[0] = operands[0];
	dest[1].address = (variable->address->array_size_address) ? variable->address->array_size_address : cxt->one_address;
	dest[1].type = QB_OPERAND_ADDRESS;
}

static void qb_transfer_operands_bound_check_multiply(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_operand *dest, uint32_t dest_count) {
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

static void qb_transfer_operands_bound_expand_multiply(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_operand *dest, uint32_t dest_count) {
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
	qb_basic_op_factory *bf = (qb_basic_op_factory *) f;
	qb_operand *container = &operands[0], *index = &operands[1];
	qb_address *index_limit_address = container->address->dimension_addresses[0];
	qb_address *variable_address;

	if(container->address->dimension_count == 1) {
		variable_address = qb_obtain_array_element(cxt, container->address, index->address, FALSE);
	} else {
		variable_address = container->address->array_size_addresses[1];
	}

	dest[0].address = index->address;
	dest[0].type = QB_OPERAND_ADDRESS;
	dest[1].address = index_limit_address;
	dest[1].type = QB_OPERAND_ADDRESS;
	dest[2].address = variable_address;
	dest[2].type = QB_OPERAND_ADDRESS;
	dest[3] = *result;
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

static void qb_transfer_operands_concat(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_operand *dest, uint32_t dest_count) {
	qb_operand *value = &operands[1];
	dest[0] = *value;
	if(value->address->dimension_count > 1) {
		dest[1].type = QB_OPERAND_ADDRESS;
		dest[1].address = qb_retrieve_array_dimensions(cxt, value->address);
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

		if(!CONSTANT_DIMENSION(address1, -1) || !CONSTANT_DIMENSION(address2, -1)) {
			// TODO add check
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

		if(!CONSTANT_DIMENSION(address1, -1) || !CONSTANT_DIMENSION(address2, -1)) {
			// TODO add check
		}
	} else {
		dest[3] = *result;
	}
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

