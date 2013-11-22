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

static void qb_produce_intrinsic_op(qb_compiler_context *cxt, void *factory, qb_operand *operands, uint32_t operand_count, qb_operand *result, uint32_t *jump_target_indices, uint32_t jump_target_count, qb_result_prototype *result_prototype) {
	qb_operand *func = &operands[0], *arguments = &operands[1], *argument_count = &operands[2];
	qb_op_factory *ff = func->intrinsic_function->extra;
	qb_intrinsic_function *ifunc = func->intrinsic_function;
	cxt->function_name = ifunc->name;
	if((uint32_t) argument_count->number < ifunc->argument_count_min || (uint32_t) argument_count->number > ifunc->argument_count_max) {
		if(ifunc->argument_count_min == ifunc->argument_count_max) {
			qb_abort("%s() expects %d arguments but %d was passed", ifunc->name, ifunc->argument_count_min, argument_count->number);
		} else {
			qb_abort("%s() expects %d to %d arguments but %d was passed", ifunc->name, ifunc->argument_count_min, ifunc->argument_count_max, argument_count->number);
		}
	}
	qb_produce_op(cxt, ff, arguments->arguments, argument_count->number, result, NULL, 0, result_prototype);
	cxt->function_name = NULL;
}

static void qb_inline_function(qb_compiler_context *cxt, void *factory, qb_operand *operands, uint32_t operand_count, qb_operand *result, uint32_t *jump_target_indices, uint32_t jump_target_count, qb_result_prototype *result_prototype) {
	USE_TSRM
	qb_operand *func = &operands[0], *arguments = &operands[1], *argument_count = &operands[2];
	qb_function *qfunc = qb_find_compiled_function(func->zend_function);
	qb_compiler_context *callee_cxt = qb_find_compiler_context(QB_G(build_context), qfunc);
	qb_function_inliner_context _inliner_cxt, *inliner_cxt = &_inliner_cxt;

	qb_initialize_function_inliner_context(inliner_cxt, cxt, callee_cxt, arguments->arguments, argument_count->number, result TSRMLS_CC);
	qb_transfer_inlined_function_ops(inliner_cxt);
	qb_free_function_inliner_context(inliner_cxt);
}

static void qb_decompose_pairwise_op_series(qb_compiler_context *cxt, void *factory, qb_operand *operands, uint32_t operand_count, qb_operand *result, uint32_t *jump_target_indices, uint32_t jump_target_count, qb_result_prototype *result_prototype) {
	qb_op_decomposer *d = factory;
	qb_operand pairwise_result;
	uint32_t i;

	pairwise_result = operands[0];
	for(i = 1; i < operand_count; i++) {
		qb_operand operand_pair[2];
		operand_pair[0] = pairwise_result;
		operand_pair[1] = operands[i];
		pairwise_result.type = QB_OPERAND_EMPTY;
		pairwise_result.generic_pointer = NULL;
		cxt->argument_offset = i - 1;
		qb_produce_op(cxt, d->factory, operand_pair, 2, &pairwise_result, NULL, 0, result_prototype);
		qb_lock_operand(cxt, &pairwise_result);
	}
	*result = pairwise_result;
}

static void qb_decompose_fetch_do_op(qb_compiler_context *cxt, void *factory, qb_operand *operands, uint32_t operand_count, qb_operand *result, uint32_t *jump_target_indices, uint32_t jump_target_count, qb_result_prototype *result_prototype) {
	qb_fetch_do_op_decomposer *d = factory;
	qb_operand fetch_result = { QB_OPERAND_EMPTY, { NULL } };
	qb_result_prototype fetch_result_prototype;
	qb_operand do_operands[2];
	
	// do the fetch first
	memset(&fetch_result_prototype, 0, sizeof(qb_result_prototype));
	qb_produce_op(cxt, d->fetch_factory, operands, 2, &fetch_result, NULL, 0, &fetch_result_prototype);

	// perform whatever action
	do_operands[0] = fetch_result;
	do_operands[1] = operands[2];
	qb_produce_op(cxt, d->do_factory, do_operands, operand_count - 1, result, NULL, 0, result_prototype);
}

static void qb_decompose_branch_set(qb_compiler_context *cxt, void *factory, qb_operand *operands, uint32_t operand_count, qb_operand *result, uint32_t *jump_target_indices, uint32_t jump_target_count, qb_result_prototype *result_prototype) {
	qb_op_decomposer *d = factory;
	qb_operand branch_condition = operands[0];
	qb_operand branch_result = { QB_OPERAND_EMPTY, { NULL } };
	qb_result_prototype branch_result_prototype;

	// do the branch first
	memset(&branch_result_prototype, 0, sizeof(qb_result_prototype));
	qb_produce_op(cxt, d->factory, &branch_condition, 1, &branch_result, jump_target_indices, jump_target_count, &branch_result_prototype);

	// do the assignment
	qb_produce_op(cxt, &factory_assign_branching, operands, operand_count, result, NULL, 0, result_prototype);
}

static void qb_decompose_minmax(qb_compiler_context *cxt, void *factory, qb_operand *operands, uint32_t operand_count, qb_operand *result, uint32_t *jump_target_indices, uint32_t jump_target_count, qb_result_prototype *result_prototype) {
	qb_minmax_decomposer *d = factory;
	if(operand_count > 1) {
		qb_decompose_pairwise_op_series(cxt, factory, operands, operand_count, result, NULL, 0, result_prototype);
	} else {
		qb_produce_op(cxt, d->array_factory, operands, operand_count, result, NULL, 0, result_prototype);
	}
}

static void qb_decompose_select(qb_compiler_context *cxt, void *factory, qb_operand *operands, uint32_t operand_count, qb_operand *result, uint32_t *jump_target_indices, uint32_t jump_target_count, qb_result_prototype *result_prototype) {
	qb_op_decomposer *d = factory;
	qb_operand branch_condition = operands[0];
	qb_operand branch_result = { QB_OPERAND_NONE, { NULL } }, jump_result = { QB_OPERAND_NONE, { NULL } };
	qb_result_prototype branch_result_prototype, jump_result_prototype;
	uint32_t branch_indices[3] = { JUMP_TARGET_INDEX(cxt->source_op_index, 3), JUMP_TARGET_INDEX(cxt->source_op_index, 1) };
	uint32_t jump_indices[1] = { JUMP_TARGET_INDEX(cxt->source_op_index, 4) };

	// branch to assignment to first value on true (offset = 0)
	memset(&branch_result_prototype, 0, sizeof(qb_result_prototype));
	qb_produce_op(cxt, d->factory, &branch_condition, 1, &branch_result, branch_indices, 2, &branch_result_prototype);

	// perform assignment to second value (offset = 1)
	qb_produce_op(cxt, &factory_assign_select, &operands[2], 1, result, NULL, 0, result_prototype);

	// jump over assignment to the first value (offset = 2)
	memset(&jump_result_prototype, 0, sizeof(qb_result_prototype));
	qb_produce_op(cxt, &factory_jump, NULL, 0, &jump_result, jump_indices, 1, &jump_result_prototype);

	// perform assignment to first value (offset = 3)
	qb_produce_op(cxt, &factory_assign_select, &operands[1], 1, result, NULL, 0, result_prototype);
}

static void qb_choose_set_or_scalar_op(qb_compiler_context *cxt, void *factory, qb_operand *operands, uint32_t operand_count, qb_operand *result, uint32_t *jump_target_indices, uint32_t jump_target_count, qb_result_prototype *result_prototype) {
	qb_set_op_chooser *c = factory;
	if(SCALAR(operands[0].address)) {
		qb_produce_op(cxt, c->scalar_factory, operands, operand_count, result, jump_target_indices, jump_target_count, result_prototype);
	} else {
		qb_produce_op(cxt, c->set_factory, operands, operand_count, result, jump_target_indices, jump_target_count, result_prototype);
	}
}

static void qb_decompose_fork(qb_compiler_context *cxt, void *factory, qb_operand *operands, uint32_t operand_count, qb_operand *result, uint32_t *jump_target_indices, uint32_t jump_target_count, qb_result_prototype *result_prototype) {
	qb_fork_decomposer *d = factory;
	qb_operand init_result = { QB_OPERAND_NONE, { NULL } }, resume_result = { QB_OPERAND_NONE, { NULL } };
	qb_result_prototype init_result_prototype, resume_result_prototype;

	// do the fork 
	memset(&init_result_prototype, 0, sizeof(qb_result_prototype));
	qb_produce_op(cxt, d->init_factory, operands, operand_count, &init_result, NULL, 0, &init_result_prototype);

	// add resumption point (empty op that holds the next_handler pointer)
	memset(&resume_result_prototype, 0, sizeof(qb_result_prototype));
	qb_produce_op(cxt, d->resume_factory, NULL, 0, &resume_result, NULL, 0, &resume_result_prototype);

	// add result op
	qb_produce_op(cxt, d->result_factory, NULL, 0, result, NULL, 0, result_prototype);
}

static void qb_decompose_array_pop(qb_compiler_context *cxt, void *factory, qb_operand *operands, uint32_t operand_count, qb_operand *result, uint32_t *jump_target_indices, uint32_t jump_target_count, qb_result_prototype *result_prototype) {
	qb_operand *container = &operands[0];
	qb_address *index_address = (cxt->stage == QB_STAGE_OPCODE_TRANSLATION) ? qb_retrieve_binary_op_result(cxt, &factory_subtract, DIMENSION_ADDRESS(container->address, 0), cxt->one_address) : cxt->zero_address;
	qb_operand fetch_operands[2] = { { QB_OPERAND_ADDRESS, { container->address } }, { QB_OPERAND_ADDRESS, { index_address } } };
	qb_operand fetch_result = { QB_OPERAND_EMPTY, { NULL } }, unset_result = { QB_OPERAND_EMPTY, { NULL } };
	qb_result_prototype fetch_result_prototype, unset_result_prototype;

	memset(&fetch_result_prototype, 0, sizeof(qb_result_prototype));
	qb_produce_op(cxt, &factory_fetch_array_element_read, fetch_operands, 2, &fetch_result, NULL, 0, &fetch_result_prototype);

	qb_produce_op(cxt, &factory_assign_temporary, &fetch_result, 1, result, NULL, 0, result_prototype);

	memset(&unset_result_prototype, 0, sizeof(qb_result_prototype));
	qb_produce_op(cxt, &factory_unset_array_element, fetch_operands, 2, &unset_result, NULL, 0, &unset_result_prototype);
}

static void qb_decompose_array_shift(qb_compiler_context *cxt, void *factory, qb_operand *operands, uint32_t operand_count, qb_operand *result, uint32_t *jump_target_indices, uint32_t jump_target_count, qb_result_prototype *result_prototype) {
	qb_operand *container = &operands[0];
	qb_address *index_address = cxt->zero_address;
	qb_operand fetch_operands[2] = { { QB_OPERAND_ADDRESS, { container->address } }, { QB_OPERAND_ADDRESS, { index_address } } };
	qb_operand fetch_result = { QB_OPERAND_EMPTY, { NULL } }, unset_result = { QB_OPERAND_EMPTY, { NULL } };
	qb_result_prototype fetch_result_prototype, unset_result_prototype;

	memset(&fetch_result_prototype, 0, sizeof(qb_result_prototype));
	qb_produce_op(cxt, &factory_fetch_array_element_read, fetch_operands, 2, &fetch_result, NULL, 0, &fetch_result_prototype);

	qb_produce_op(cxt, &factory_assign_temporary, &fetch_result, 1, result, NULL, 0, result_prototype);

	memset(&unset_result_prototype, 0, sizeof(qb_result_prototype));
	qb_produce_op(cxt, &factory_unset_array_element, fetch_operands, 2, &unset_result, NULL, 0, &unset_result_prototype);
}

static void qb_decompose_array_push(qb_compiler_context *cxt, void *factory, qb_operand *operands, uint32_t operand_count, qb_operand *result, uint32_t *jump_target_indices, uint32_t jump_target_count, qb_result_prototype *result_prototype) {
	qb_operand *container = &operands[0];
	uint32_t i;

	for(i = 1; i < operand_count; i++) {
		qb_operand replace_operands[4], replace_result = { QB_OPERAND_EMPTY, { NULL } };
		replace_operands[0].address = container->address;
		replace_operands[0].type = QB_OPERAND_ADDRESS;
		replace_operands[1].address = container->address->dimension_addresses[0];
		replace_operands[1].type = QB_OPERAND_ADDRESS;
		replace_operands[2].address = cxt->zero_address;
		replace_operands[2].type = QB_OPERAND_ADDRESS;
		replace_operands[3] = operands[i];
		qb_produce_op(cxt, &factory_array_replace, replace_operands, 4, &replace_result, NULL, 0, NULL);
	}
	result->address = container->address->dimension_addresses[0];
	result->type = QB_OPERAND_ADDRESS;
}

static void qb_decompose_array_unshift(qb_compiler_context *cxt, void *factory, qb_operand *operands, uint32_t operand_count, qb_operand *result, uint32_t *jump_target_indices, uint32_t jump_target_count, qb_result_prototype *result_prototype) {
	qb_operand *container = &operands[0];
	uint32_t i;

	for(i = operand_count - 1; i >= 1; i--) {
		qb_operand replace_operands[4], replace_result = { QB_OPERAND_EMPTY, { NULL } };
		replace_operands[0].address = container->address;
		replace_operands[0].type = QB_OPERAND_ADDRESS;
		replace_operands[1].address = cxt->zero_address;
		replace_operands[1].type = QB_OPERAND_ADDRESS;
		replace_operands[2].address = cxt->zero_address;
		replace_operands[2].type = QB_OPERAND_ADDRESS;
		replace_operands[3] = operands[i];
		qb_produce_op(cxt, &factory_array_replace, replace_operands, 4, &replace_result, NULL, 0, NULL);
	}
	result->address = container->address->dimension_addresses[0];
	result->type = QB_OPERAND_ADDRESS;
}

static void qb_decompose_array_merge(qb_compiler_context *cxt, void *factory, qb_operand *operands, uint32_t operand_count, qb_operand *result, uint32_t *jump_target_indices, uint32_t jump_target_count, qb_result_prototype *result_prototype) {
	qb_operand *container1 = &operands[0];
	uint32_t i;

	if(cxt->stage == QB_STAGE_RESULT_TYPE_RESOLUTION) {
		result_prototype->final_type = result_prototype->preliminary_type = qb_get_highest_rank_type(cxt, operands, operand_count, 0);
		result->type = QB_OPERAND_RESULT_PROTOTYPE;
		result->result_prototype = result_prototype;
	} else if(cxt->stage == QB_STAGE_OPCODE_TRANSLATION) {
		qb_variable_dimensions dim = { 1, NULL };
		result->address = qb_obtain_temporary_variable(cxt, result_prototype->final_type, &dim);
		result->type = QB_OPERAND_ADDRESS;

		for(i = 0; i < operand_count; i++) {
			qb_operand replace_operands[4], replace_result = { QB_OPERAND_EMPTY, { NULL } };
			replace_operands[0].address = result->address;
			replace_operands[0].type = QB_OPERAND_ADDRESS;
			replace_operands[1].address = result->address->dimension_addresses[0];
			replace_operands[1].type = QB_OPERAND_ADDRESS;
			replace_operands[2].address = cxt->zero_address;
			replace_operands[2].type = QB_OPERAND_ADDRESS;
			replace_operands[3] = operands[i];
			qb_produce_op(cxt, &factory_array_replace, replace_operands, 4, &replace_result, NULL, 0, NULL);
		}
	}
}

static void qb_decompose_in_array(qb_compiler_context *cxt, void *factory, qb_operand *operands, uint32_t operand_count, qb_operand *result, uint32_t *jump_target_indices, uint32_t jump_target_count, qb_result_prototype *result_prototype) {
	qb_operand search_result = { QB_OPERAND_EMPTY, { NULL } };
	qb_operand comparison_operands[2];
	qb_result_prototype search_result_prototype;

	memset(&search_result_prototype, 0, sizeof(qb_result_prototype));
	search_result_prototype.preliminary_type = search_result_prototype.final_type = QB_TYPE_I32;
	qb_produce_op(cxt, &factory_array_search, operands, operand_count, &search_result, NULL, 0, &search_result_prototype);
	comparison_operands[0] = search_result;
	comparison_operands[1].address = qb_obtain_constant_U32(cxt, -1);
	comparison_operands[1].type = QB_OPERAND_ADDRESS;
	qb_produce_op(cxt, &factory_not_equal, comparison_operands, 2, result, NULL, 0, result_prototype);
}

static void qb_decompose_array_splice(qb_compiler_context *cxt, void *factory, qb_operand *operands, uint32_t operand_count, qb_operand *result, uint32_t *jump_target_indices, uint32_t jump_target_count, qb_result_prototype *result_prototype) {
	qb_operand replace_result = { QB_OPERAND_EMPTY, { NULL } };

	if(!result_prototype->destination || result_prototype->destination->type != QB_RESULT_DESTINATION_FREE) {
		qb_produce_op(cxt, &factory_array_slice, operands, (operand_count > 3) ? 3 : operand_count, result, NULL, 0, result_prototype);
	}
	qb_produce_op(cxt, &factory_array_replace, operands, operand_count, &replace_result, NULL, 0, NULL);
}