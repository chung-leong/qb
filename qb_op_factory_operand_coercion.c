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

// coerce all operands expr_type
static void qb_coerce_operands_all(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count) {
	uint32_t i;
	for(i = 0; i < operand_count; i++) {
		qb_perform_type_coercion(cxt, &operands[i], expr_type, f->coercion_flags);
	}
}

// coerce all operands to boolean
static void qb_coerce_operands_boolean(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count) {
	uint32_t i;
	for(i = 0; i < operand_count; i++) {
		if(operands[i].type != QB_OPERAND_ADDRESS || !STORAGE_TYPE_MATCH(operands[i].address->type, QB_TYPE_I32)) {
			qb_perform_boolean_coercion(cxt, &operands[i]);
		}
	}
}

// coerce all operands to I32
static void qb_coerce_operands_int(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count) {
	uint32_t i;
	for(i = 0; i < operand_count; i++) {
		qb_perform_type_coercion(cxt, &operands[i], QB_TYPE_I32, f->coercion_flags);
	}
}

// coerce one operand to suitable type, ignoring expr_type (which is something else like boolean)
static void qb_coerce_operands_first_operand(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count) {
	qb_primitive_type operand_type = qb_get_operand_type(cxt, &operands[0], f->coercion_flags);
	qb_perform_type_coercion(cxt, &operands[0], operand_type, f->coercion_flags);
}

// coerce all operands to the type of the highest-rank operand, ignoring expr_type
static void qb_coerce_operands_highest_rank(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count) {
	qb_primitive_type operand_type = qb_get_highest_rank_type(cxt, operands, operand_count, f->coercion_flags);
	uint32_t i;
	for(i = 0; i < operand_count; i++) {
		qb_perform_type_coercion(cxt, &operands[i], operand_type, f->coercion_flags);
	}
}

// coerce operands that aren't addresses to the highest-rank type
static void qb_coerce_operands_no_cast(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count) {
	qb_primitive_type operand_type = qb_get_highest_rank_type(cxt, operands, operand_count, f->coercion_flags);
	uint32_t i;
	for(i = 0; i < operand_count; i++) {
		if(operands[i].type != QB_OPERAND_ADDRESS) {
			qb_perform_type_coercion(cxt, &operands[i], operand_type, f->coercion_flags);
		}
	}
}

static void qb_coerce_operands_all_to_first(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count) {
	qb_primitive_type operand_type = qb_get_operand_type(cxt, &operands[0], f->coercion_flags);
	uint32_t i;
	for(i = 0; i < operand_count; i++) {
		qb_perform_type_coercion(cxt, &operands[i], operand_type, f->coercion_flags);
	}
}

static void qb_coerce_operands_assign(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count) {
	qb_operand *value = &operands[1];
	if(value->type != QB_OPERAND_ADDRESS || f != (void *) &factory_assign) {
		// since there're instructions for copying from one type to another, 
		// don't perform casting on when there's an address
		qb_perform_type_coercion(cxt, value, expr_type, f->coercion_flags);
	}
}

static void qb_coerce_operands_assign_array_element(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count) {
	qb_operand *index = &operands[1];
	qb_operand *value = &operands[2];
	qb_perform_type_coercion(cxt, index, QB_TYPE_U32, 0);
	if(value->type != QB_OPERAND_ADDRESS || f != (void *) &factory_assign_array_element) {
		qb_perform_type_coercion(cxt, value, expr_type, f->coercion_flags);
	}
}

static void qb_coerce_operands_assign_object_property(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr, qb_operand *operands, uint32_t operand_count) {
	qb_operand *value = &operands[2];
	if(value->type != QB_OPERAND_ADDRESS || f != (void *) &factory_assign_object_property) {
		qb_perform_type_coercion(cxt, value, expr, f->coercion_flags);
	}
}

static void qb_coerce_operands_concat(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count) {
	qb_operand *augend = &operands[0], *addend = &operands[1];
	qb_perform_type_coercion(cxt, augend, QB_TYPE_ANY, f->coercion_flags);
	qb_perform_type_coercion(cxt, addend, QB_TYPE_ANY, f->coercion_flags);
}

static void qb_coerce_operands_add_variable(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count) {
	qb_operand *addend = &operands[1];
	qb_perform_type_coercion(cxt, addend, QB_TYPE_ANY, f->coercion_flags);
}

static void qb_coerce_operands_add_string(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count) {
	qb_operand *addend = &operands[1];
	qb_perform_type_coercion(cxt, addend, QB_TYPE_U08, f->coercion_flags);
}

static void qb_coerce_operands_print(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count) {
	qb_operand *value = &operands[0];
	qb_perform_type_coercion(cxt, value, QB_TYPE_ANY, f->coercion_flags);
}

static void qb_coerce_operands_return(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count) {
	qb_operand *value = &operands[0];
	if(cxt->stage == QB_STAGE_RESULT_TYPE_RESOLUTION || value->type != QB_OPERAND_ADDRESS) {
		qb_primitive_type retval_type;
		if(cxt->return_variable->address) {
			retval_type = cxt->return_variable->address->type;
			qb_perform_type_coercion(cxt, value, retval_type, f->coercion_flags);
		}
	}
}

static void qb_coerce_operands_fetch_array_element(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count) {
	qb_operand *index = &operands[1];
	qb_perform_type_coercion(cxt, index, QB_TYPE_U32, 0);
}

static void qb_coerce_operands_round(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count) {
	qb_operand *value = &operands[0], *precision = &operands[1], *mode = &operands[2];
	qb_perform_type_coercion(cxt, value, expr_type, f->coercion_flags);
	qb_perform_type_coercion(cxt, precision, QB_TYPE_I32, 0);
	qb_perform_type_coercion(cxt, mode, QB_TYPE_I32, 0);
}

static void qb_coerce_operands_fetch_array_size(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count) {
	qb_operand *recusive = &operands[1];
	if(recusive->type != QB_OPERAND_NONE) {
		qb_perform_boolean_coercion(cxt, recusive);
	}
}

static void qb_coerce_operands_array_column(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count) {
	qb_operand *container = &operands[0], *column_index = &operands[1];
	qb_perform_type_coercion(cxt, container, expr_type, f->coercion_flags);
	qb_perform_type_coercion(cxt, column_index, QB_TYPE_U32, 0);
}

static void qb_coerce_operands_array_fill(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count) {
	qb_operand *start_index = &operands[0], *number = &operands[1], *value = &operands[2];
	qb_perform_type_coercion(cxt, start_index, QB_TYPE_U32, 0);
	qb_perform_type_coercion(cxt, number, QB_TYPE_U32, 0);
	qb_perform_type_coercion(cxt, value, expr_type, f->coercion_flags);
}

static void qb_coerce_operands_array_pad(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count) {
	qb_operand *container = &operands[0], *size = &operands[1], *value = &operands[2];
	qb_perform_type_coercion(cxt, container, expr_type, f->coercion_flags);
	qb_perform_type_coercion(cxt, size, QB_TYPE_U32, 0);
	qb_perform_type_coercion(cxt, value, expr_type, f->coercion_flags);
}

static void qb_coerce_operands_array_pos(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count) {
	qb_operand *container = &operands[0], *subarray = &operands[1], *offset = &operands[2];
	qb_primitive_type operand_type = qb_get_operand_type(cxt, container, f->coercion_flags);
	qb_perform_type_coercion(cxt, container, operand_type, f->coercion_flags);
	qb_perform_type_coercion(cxt, subarray, operand_type, f->coercion_flags);
	if(offset->type != QB_OPERAND_NONE) {
		qb_perform_type_coercion(cxt, offset, QB_TYPE_S32, 0);
	}
}

static void qb_coerce_operands_array_push(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count) {
	qb_primitive_type operand_type = qb_get_operand_type(cxt, &operands[0], f->coercion_flags);
	uint32_t i;
	for(i = 1; i < operand_count; i++) {
		qb_operand *value = &operands[i];
		// don't coerce addresses since cross-type copying is possible
		if(cxt->stage == QB_STAGE_RESULT_TYPE_RESOLUTION || value->type != QB_OPERAND_ADDRESS) {
			qb_perform_type_coercion(cxt, value, operand_type, f->coercion_flags);
		}
	}
}

static void qb_coerce_operands_array_rand(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count) {
	qb_operand *container = &operands[0], *count = &operands[1];
	qb_perform_type_coercion(cxt, container, QB_TYPE_ANY, f->coercion_flags);
	if(count->type != QB_OPERAND_NONE) {
		qb_perform_type_coercion(cxt, count, QB_TYPE_U32, 0);
	}
}

static void qb_coerce_operands_array_resize(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count) {
	uint32_t i;
	for(i = 1; i < operand_count; i++) {
		qb_perform_type_coercion(cxt, &operands[i], QB_TYPE_U32, 0);
	}
}

static void qb_coerce_operands_array_slice(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count) {
	qb_operand *container = &operands[0], *offset = &operands[1], *length = &operands[2];
	qb_perform_type_coercion(cxt, container, expr_type, f->coercion_flags);
	qb_perform_type_coercion(cxt, offset, QB_TYPE_U32, 0);
	if(length->type != QB_OPERAND_NONE) {
		qb_perform_type_coercion(cxt, length, QB_TYPE_S32, 0);
	}
}

static void qb_coerce_operands_utf8_decode(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count) {
	qb_operand *string = &operands[0];
	qb_perform_type_coercion(cxt, string, QB_TYPE_U08, 0);
}

static void qb_coerce_operands_utf8_encode(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count) {
	qb_operand *codepoints = &operands[0];
	qb_primitive_type operand_type = qb_get_operand_type(cxt, codepoints, 0);
	if(operand_type != QB_TYPE_U16 && operand_type != QB_TYPE_U32) {
		if(STORAGE_TYPE_MATCH(operand_type, QB_TYPE_U16)) {
			operand_type = QB_TYPE_U16;
		} else {
			operand_type = QB_TYPE_U32;
		}
	}
	qb_perform_type_coercion(cxt, codepoints, operand_type, 0);
}

static void qb_coerce_operands_pack(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count) {
	qb_operand *value = &operands[0], *type = &operands[1];
	qb_primitive_type operand_type = QB_TYPE_UNKNOWN;
	if(type->type == QB_OPERAND_ZVAL && type->constant->type == IS_LONG) {
		uint32_t value = Z_LVAL_P(type->constant);
		if(value < QB_TYPE_COUNT) {
			operand_type = value;
		}
	}
	if(value->type != QB_OPERAND_ADDRESS) {
		if(operand_type != QB_TYPE_UNKNOWN) {
			qb_perform_type_coercion(cxt, value, operand_type, 0);
		}
	}
	// TODO: check the type
}

static void qb_coerce_operands_unpack(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count) {
	qb_operand *string = &operands[0], *index = &operands[1], *type = &operands[2];
	qb_perform_type_coercion(cxt, string, QB_TYPE_U08, 0);
	if(index->type != QB_OPERAND_NONE) {
		qb_perform_type_coercion(cxt, index, QB_TYPE_U32, 0);
	}
}

static void qb_coerce_operands_function_call(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count) {
	qb_operand *func = &operands[0], *arguments = &operands[1], *argument_count = &operands[2];
	qb_function *qfunc = qb_find_compiled_function(func->zend_function);
	uint32_t i;
	for(i = 0; i < (uint32_t) argument_count->number; i++) {
		qb_operand *argument = &arguments->arguments[i];
		qb_primitive_type arg_type = QB_TYPE_ANY;
		if(i < qfunc->argument_count) {
			qb_variable *arg = qfunc->variables[i];
			// do type coercion only if parameter isn't passed-by-reference
			if(!(arg->flags & QB_VARIABLE_BY_REF)) {
				arg_type = arg->address->type;
			}
		}
		qb_perform_type_coercion(cxt, argument, arg_type, 0);
	}
}

static void qb_coerce_operands_zend_function_call(qb_compiler_context *cxt, qb_op_factory *f, qb_primitive_type expr_type, qb_operand *operands, uint32_t operand_count) {
	qb_operand *func = &operands[0], *arguments = &operands[1], *argument_count = &operands[2];
	uint32_t i;
	for(i = 0; i < (uint32_t) argument_count->number; i++) {
		qb_operand *argument = &arguments->arguments[i];
		qb_perform_type_coercion(cxt, argument, QB_TYPE_ANY, 0);
	}
}