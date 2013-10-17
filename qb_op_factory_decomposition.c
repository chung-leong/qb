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
	qb_produce_op(cxt, func->intrinsic_function->extra, arguments->arguments, argument_count->number, result, NULL, 0, result_prototype);
	cxt->function_name = NULL;
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

static void qb_decompose_object_property_op(qb_compiler_context *cxt, void *factory, qb_operand *operands, uint32_t operand_count, qb_operand *result, uint32_t *jump_target_indices, uint32_t jump_target_count, qb_result_prototype *result_prototype) {
	qb_fetch_do_op_decomposer *d = factory;
	qb_operand fetch_result;
	qb_result_prototype fetch_result_prototype;
	qb_operand do_operands[2];
	
	// do the fetch first
	qb_produce_op(cxt, d->fetch_factory, operands, 2, &fetch_result, NULL, 0, &fetch_result_prototype);

	// perform whatever action
	do_operands[0] = fetch_result;
	do_operands[1] = operands[2];
	qb_produce_op(cxt, d->do_factory, do_operands, operand_count - 1, result, NULL, 0, result_prototype);
}
