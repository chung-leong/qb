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

// the expression yields a temporary variable
static int32_t qb_resolve_expression_flags_temporary(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, uint32_t *p_flags) {
	*p_flags |= QB_ADDRESS_TEMPORARY;
	return TRUE;
}

// the expression yields a temporary variable (keeping flags of operands)
static int32_t qb_resolve_expression_flags_temporary_pass_thru(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, uint32_t *p_flags) {
	uint32_t special_flags = QB_ADDRESS_VECTOR | QB_ADDRESS_MATRIX | QB_ADDRESS_COMPLEX;
	uint32_t i;
	for(i = 0; i < operand_count; i++) {
		qb_operand *operand = &operands[i];
		if(operand->type == QB_OPERAND_ADDRESS) {
			special_flags &= operand->address->flags;
		} else if(operand->type == QB_OPERAND_RESULT_PROTOTYPE) {
			special_flags &= operand->result_prototype->address_flags;
		} else {
			special_flags = 0;
		}
	}
	*p_flags |= QB_ADDRESS_TEMPORARY | special_flags;
	return TRUE;
}

// the expression yields a temporary variable (keeping flags of first operand)
static int32_t qb_resolve_expression_flags_temporary_pass_thru_first(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, uint32_t *p_flags) {
	uint32_t special_flags = QB_ADDRESS_VECTOR | QB_ADDRESS_MATRIX | QB_ADDRESS_COMPLEX;
	qb_operand *operand = &operands[0];
	if(operand->type == QB_OPERAND_ADDRESS) {
		special_flags &= operand->address->flags;
	} else if(operand->type == QB_OPERAND_RESULT_PROTOTYPE) {
		special_flags &= operand->result_prototype->address_flags;
	} else {
		special_flags = 0;
	}
	*p_flags |= QB_ADDRESS_TEMPORARY | special_flags;
	return TRUE;
}

// the expression yields a temporary variable (keeping flags of second operand)
static int32_t qb_resolve_expression_flags_temporary_pass_thru_second(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, uint32_t *p_flags) {
	uint32_t special_flags = QB_ADDRESS_VECTOR | QB_ADDRESS_MATRIX | QB_ADDRESS_COMPLEX;
	qb_operand *operand = &operands[1];
	if(operand->type == QB_OPERAND_ADDRESS) {
		special_flags &= operand->address->flags;
	} else if(operand->type == QB_OPERAND_RESULT_PROTOTYPE) {
		special_flags &= operand->result_prototype->address_flags;
	} else {
		special_flags = 0;
	}
	*p_flags |= QB_ADDRESS_TEMPORARY | special_flags;
	return TRUE;
}

// the expression yields a temporary boolean variable
static int32_t qb_resolve_expression_flags_temporary_boolean(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, uint32_t *p_flags) {
	*p_flags |= QB_ADDRESS_TEMPORARY | QB_ADDRESS_BOOLEAN;
	return TRUE;
}

// the expression yields a temporary string variable
static int32_t qb_resolve_expression_flags_temporary_string(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, uint32_t *p_flags) {
	*p_flags |= QB_ADDRESS_TEMPORARY | QB_ADDRESS_STRING;
	return TRUE;
}

// the expression always yields a constant
static int32_t qb_resolve_expression_flags_constant(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, uint32_t *p_flags) {
	*p_flags |= QB_ADDRESS_CONSTANT;
	return TRUE;
}

// the expression always yields a string constant
static int32_t qb_resolve_expression_flags_constant_string(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, uint32_t *p_flags) {
	*p_flags |= QB_ADDRESS_CONSTANT | QB_ADDRESS_STRING;
	return TRUE;
}

// the expression always yields a boolean constant
static int32_t qb_resolve_expression_flags_constant_boolean(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, uint32_t *p_flags) {
	*p_flags |= QB_ADDRESS_CONSTANT | QB_ADDRESS_BOOLEAN;
	return TRUE;
}

static void qb_get_address_flags(qb_compiler_context *cxt, qb_operand *operands, uint32_t operand_count, int32_t *flags) {
	uint32_t i;
	uint32_t flags_set = 0;
	int32_t unresolved = FALSE;
	for(i = 0; i < operand_count; i++) {
		qb_operand *operand = &operands[i];
		if(operand->type == QB_OPERAND_ADDRESS) {
			flags[i] = operand->address->flags & (QB_ADDRESS_COMPLEX |  QB_ADDRESS_VECTOR | QB_ADDRESS_MATRIX);
		} else if(operand->type == QB_OPERAND_RESULT_PROTOTYPE) {
			flags[i] = operand->result_prototype->address_flags & (QB_ADDRESS_COMPLEX |  QB_ADDRESS_VECTOR | QB_ADDRESS_MATRIX); 
			if(!flags[i]) {
				if(operand->result_prototype->address_flags & QB_ADDRESS_LITERAL_ARRAY) {
					unresolved = TRUE;
				}
			}
		} else {
			flags[i] = 0;
			unresolved = TRUE;
		}
		flags_set |= flags[i];
	}
	if(unresolved && flags_set) {
		// try to figure out what literal values ought to be
		for(i = 0; i < operand_count; i++) {
			qb_operand *operand = &operands[i];

			// see how many dimension there are
			uint32_t dimension_count = 0;
			if(operand->type == QB_OPERAND_ZVAL) {
				dimension_count = qb_get_zend_array_dimension_count(cxt, operand->constant);
			} else if(operand->type == QB_OPERAND_ARRAY_INITIALIZER) {
				dimension_count = qb_get_array_initializer_dimension_count(cxt, operand->array_initializer);
			} else if(operand->type == QB_OPERAND_RESULT_PROTOTYPE) {
				if(operand->result_prototype->address_flags & QB_ADDRESS_LITERAL_ARRAY) {
					dimension_count = (operand->result_prototype->address_flags & QB_ADDRESS_MULTIDIMENSIONAL_LITERAL_ARRAY) ? 2 : 1;
				}
			}
			if(dimension_count > 0) {
				if(dimension_count > 1 && flags_set & QB_ADDRESS_MATRIX) {
					// there's more than one dimension and the other operand is a matrix,
					// treat the literal array as a matrix
					flags[i] = QB_ADDRESS_MATRIX;
				} else if(flags_set & QB_ADDRESS_VECTOR) {
					// the other operand is a vector--treat it as a vector
					flags[i] = QB_ADDRESS_VECTOR;
				} else if(flags_set & QB_ADDRESS_COMPLEX) {
					// the other operand is a complex number--treat it as a complex number
					flags[i] = QB_ADDRESS_COMPLEX;
				}
			}
		}
	}
}

static int32_t qb_resolve_expression_flags_real_or_complex(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, uint32_t *p_flags) {
	int32_t flags[2];
	qb_get_address_flags(cxt, operands, operand_count, flags);
	if(((flags[0] & QB_ADDRESS_COMPLEX) && (flags[1] & QB_ADDRESS_COMPLEX) && (operand_count == 2))	|| ((flags[0] & QB_ADDRESS_COMPLEX) && (operand_count == 1))) {
		*p_flags |= QB_ADDRESS_COMPLEX;
	}
	return TRUE;
}

static int32_t qb_resolve_expression_flags_multiply(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, uint32_t *p_flags) {
	int32_t flags[2];
	qb_get_address_flags(cxt, operands, operand_count, flags);
	if((flags[0] & QB_ADDRESS_MATRIX) && (flags[1] & QB_ADDRESS_MATRIX)) {
		*p_flags |= QB_ADDRESS_MATRIX | QB_ADDRESS_MM_RESULT;
	} else if((flags[0] & QB_ADDRESS_MATRIX) && (flags[1] & QB_ADDRESS_VECTOR)) {
		*p_flags |= QB_ADDRESS_VECTOR | QB_ADDRESS_MV_RESULT;
	} else if((flags[0] & QB_ADDRESS_VECTOR) && (flags[1] & QB_ADDRESS_MATRIX)) {
		*p_flags |= QB_ADDRESS_VECTOR | QB_ADDRESS_VM_RESULT;
	} else if((flags[0] & QB_ADDRESS_VECTOR) && (flags[1] & QB_ADDRESS_VECTOR)) {
		*p_flags |= QB_ADDRESS_VV_RESULT;
	} else if((flags[0] & QB_ADDRESS_COMPLEX) && (flags[1] & QB_ADDRESS_COMPLEX)) {
		*p_flags |= QB_ADDRESS_COMPLEX;
	}
	return TRUE;
}

static int32_t qb_resolve_expression_type_real_or_complex(qb_compiler_context *cxt, qb_op_factory *f, uint32_t flags, qb_operand *operands, uint32_t operand_count, qb_primitive_type *p_type) {
	f = qb_select_complex_op_factory(f, flags);
	return (f->resolve_type) ? f->resolve_type(cxt, f, flags, operands, operand_count, p_type) : TRUE;
}

static int32_t qb_resolve_expression_type_multiply(qb_compiler_context *cxt, qb_op_factory *f, uint32_t flags, qb_operand *operands, uint32_t operand_count, qb_primitive_type *p_type) {
	f = qb_select_multiply_factory(f, flags);
	return (f->resolve_type) ? f->resolve_type(cxt, f, flags, operands, operand_count, p_type) : TRUE;
}

// the expression is always a boolean
static int32_t qb_resolve_expression_type_boolean(qb_compiler_context *cxt, qb_op_factory *f, uint32_t flags, qb_operand *operands, uint32_t operand_count, qb_primitive_type *p_type) {
	*p_type = QB_TYPE_I32;
	return TRUE;
}

// the expresion is always an index (uint32_t currently)
static int32_t qb_resolve_expression_type_index(qb_compiler_context *cxt, qb_op_factory *f, uint32_t flags, qb_operand *operands, uint32_t operand_count, qb_primitive_type *p_type) {
	*p_type = QB_TYPE_U32;
	return TRUE;
}

// the expresion is always an signed index (int32_t currently)
static int32_t qb_resolve_expression_type_search_index(qb_compiler_context *cxt, qb_op_factory *f, uint32_t flags, qb_operand *operands, uint32_t operand_count, qb_primitive_type *p_type) {
	*p_type = QB_TYPE_S32;
	return TRUE;
}

// the expression is always a U08
static int32_t qb_resolve_expression_type_string(qb_compiler_context *cxt, qb_op_factory *f, uint32_t flags, qb_operand *operands, uint32_t operand_count, qb_primitive_type *p_type) {
	*p_type = QB_TYPE_U08;
	return TRUE;
}

// the expression is always a F64
static int32_t  qb_resolve_expression_type_double(qb_compiler_context *cxt, qb_op_factory *f, uint32_t flags, qb_operand *operands, uint32_t operand_count, qb_primitive_type *p_type) {
	*p_type = QB_TYPE_F64;
	return TRUE;
}

// the expresion has the same type as the highest-rank operand
static int32_t qb_resolve_expression_type_highest_rank(qb_compiler_context *cxt, qb_op_factory *f, uint32_t flags, qb_operand *operands, uint32_t operand_count, qb_primitive_type *p_type) {
	*p_type = qb_get_highest_rank_type(cxt, operands, operand_count, f->coercion_flags);
	return TRUE;
}

// the expression will have the same type as the l-value
static int32_t qb_resolve_expression_type_lvalue(qb_compiler_context *cxt, qb_op_factory *f, uint32_t flags, qb_operand *operands, uint32_t operand_count, qb_primitive_type *p_type) {
	*p_type = QB_TYPE_ANY;
	return TRUE;
}

// the expression has the same type as the first operand
static int32_t qb_resolve_expression_type_first_operand(qb_compiler_context *cxt, qb_op_factory *f, uint32_t flags, qb_operand *operands, uint32_t operand_count, qb_primitive_type *p_type) {
	*p_type = qb_get_operand_type(cxt, &operands[0], f->coercion_flags);
	return TRUE;
}

// the expression has the same type as the third operand
static int32_t qb_resolve_expression_type_third_operand(qb_compiler_context *cxt, qb_op_factory *f, uint32_t flags, qb_operand *operands, uint32_t operand_count, qb_primitive_type *p_type) {
	*p_type = qb_get_operand_type(cxt, &operands[2], f->coercion_flags);
	return TRUE;
}

static int32_t qb_resolve_expression_type_assign(qb_compiler_context *cxt, qb_op_factory *f, uint32_t flags, qb_operand *operands, uint32_t operand_count, qb_primitive_type *p_type) {
	qb_operand *variable = &operands[0], *value = &operands[1];
	if(variable->type == QB_OPERAND_EMPTY) {
		// the variable doesn't necessarily have to exist
		// the type is determined by the value
		*p_type = qb_get_operand_type(cxt, value, f->coercion_flags);
	} else {
		*p_type = qb_get_operand_type(cxt, variable, f->coercion_flags);
	}
	return TRUE;
}

static int32_t qb_resolve_expression_flags_assign_branching(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, uint32_t *p_flags) {
	qb_operand *value = &operands[0];
	if(value->type == QB_OPERAND_ADDRESS) {
		*p_flags = value->address->flags & (QB_ADDRESS_COMPLEX |  QB_ADDRESS_VECTOR | QB_ADDRESS_MATRIX | QB_ADDRESS_STRING | QB_ADDRESS_BOOLEAN);
	} else if(value->type == QB_OPERAND_RESULT_PROTOTYPE) {
		*p_flags = value->result_prototype->address_flags & (QB_ADDRESS_COMPLEX |  QB_ADDRESS_VECTOR | QB_ADDRESS_MATRIX | QB_ADDRESS_STRING | QB_ADDRESS_BOOLEAN); 
	} else {
		*p_flags = 0;
	}
	return TRUE;
}

static int32_t qb_resolve_expression_type_assign_branching(qb_compiler_context *cxt, qb_op_factory *f, uint32_t flags, qb_operand *operands, uint32_t operand_count, qb_primitive_type *p_type) {
	qb_operand *value = &operands[0];
	uint32_t retrival_flags = 0;
	if(cxt->stage == QB_STAGE_RESULT_TYPE_RESOLUTION) {
		// use the type of the value only when it's definite
		retrival_flags = QB_RETRIEVE_DEFINITE_TYPE_ONLY;
	}
	*p_type = qb_get_operand_type(cxt, value, retrival_flags);
	return TRUE;
}

static int32_t qb_resolve_expression_type_assign_return_value(qb_compiler_context *cxt, qb_op_factory *f, uint32_t flags, qb_operand *operands, uint32_t operand_count, qb_primitive_type *p_type) {
	if(cxt->return_variable->address) {
		*p_type = cxt->return_variable->address->type;
	} else {
		*p_type = QB_TYPE_VOID;
	}
	return TRUE;
}

static int32_t qb_resolve_expression_type_assign_generator_key(qb_compiler_context *cxt, qb_op_factory *f, uint32_t flags, qb_operand *operands, uint32_t operand_count, qb_primitive_type *p_type) {
	if(cxt->return_key_variable->address) {
		*p_type = cxt->return_key_variable->address->type;
	} else {
		*p_type = QB_TYPE_VOID;
	}
	return TRUE;
}

static int32_t qb_resolve_expression_type_sent_value(qb_compiler_context *cxt, qb_op_factory *f, uint32_t flags, qb_operand *operands, uint32_t operand_count, qb_primitive_type *p_type) {
	if(cxt->sent_variable->address) {
		*p_type = cxt->sent_variable->address->type;
	} else {
		*p_type = QB_TYPE_VOID;
	}
	return TRUE;
}

static int32_t qb_resolve_expression_type_cast(qb_compiler_context *cxt, qb_op_factory *f, uint32_t flags, qb_operand *operands, uint32_t operand_count, qb_primitive_type *p_type) {
	qb_cast_op_factory *cf = (qb_cast_op_factory *) f;
	*p_type = cf->type;
	return TRUE;
}

static int32_t qb_resolve_expression_flags_array_size(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, uint32_t *p_flags) {
	qb_operand *container = &operands[0], *recursive = &operands[1];
	*p_flags = 0;
	if(qb_is_constant_expression(cxt, recursive, 1)) {
		if(container->type == QB_OPERAND_ADDRESS && IS_FIXED_LENGTH(container->address)) {
			*p_flags |= QB_ADDRESS_CONSTANT;
		} else if(container->type == QB_OPERAND_ARRAY_INITIALIZER) {
			*p_flags |= QB_ADDRESS_CONSTANT;
		}
	}
	return TRUE;
}

static int32_t qb_resolve_expression_type_object_property(qb_compiler_context *cxt, qb_op_factory *f, uint32_t flags, qb_operand *operands, uint32_t operand_count, qb_primitive_type *p_type) {
	qb_operand *container = &operands[0], *name = &operands[1];
	if(container->type == QB_OPERAND_RESULT_PROTOTYPE) {
		*p_type = container->result_prototype->preliminary_type;
	} else {
		qb_address *address = qb_obtain_object_property(cxt, container, name, 0);
		if(address) {
			*p_type = address->type;
		} else {
			// just set it to something--it'll fail during validation
			*p_type = QB_TYPE_I32;
		}
	}
	return TRUE;
}

static int32_t qb_resolve_expression_type_fetch_local(qb_compiler_context *cxt, qb_op_factory *f, uint32_t flags, qb_operand *operands, uint32_t operand_count, qb_primitive_type *p_type) {
	qb_operand *name = &operands[0];
	qb_variable *qvar = qb_get_local_variable(cxt, name->constant);
	if(qvar && qvar->address) {
		*p_type = qvar->address->type;
	} else {
		*p_type = QB_TYPE_VOID;
	}
	return TRUE;
}

static int32_t qb_resolve_expression_type_fetch_global(qb_compiler_context *cxt, qb_op_factory *f, uint32_t flags, qb_operand *operands, uint32_t operand_count, qb_primitive_type *p_type) {
	qb_operand *name = &operands[0];
	qb_variable *qvar = qb_get_global_variable(cxt, name->constant);
	if(qvar && qvar->address) {
		*p_type = qvar->address->type;
	} else {
		*p_type = QB_TYPE_VOID;
	}
	return TRUE;
}

static int32_t qb_resolve_expression_type_fetch_static(qb_compiler_context *cxt, qb_op_factory *f, uint32_t flags, qb_operand *operands, uint32_t operand_count, qb_primitive_type *p_type) {
	qb_operand *name = &operands[0];
	qb_variable *qvar = qb_get_static_variable(cxt, name->constant);
	if(qvar && qvar->address) {
		*p_type = qvar->address->type;
	} else {
		*p_type = QB_TYPE_VOID;
	}
	return TRUE;
}

static int32_t qb_resolve_expression_type_fetch_class(qb_compiler_context *cxt, qb_op_factory *f, uint32_t flags, qb_operand *operands, uint32_t operand_count, qb_primitive_type *p_type) {
	qb_operand *name = &operands[0], *scope = &operands[1];
	qb_variable *qvar = qb_get_class_variable(cxt, scope->zend_class, name->constant);
	if(qvar && qvar->address) {
		*p_type = qvar->address->type;
	} else {
		*p_type = QB_TYPE_VOID;
	}
	return TRUE;
}

static int32_t qb_resolve_expression_flags_array_init(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, uint32_t *p_flags) {
	*p_flags = QB_ADDRESS_LITERAL_ARRAY;
	return TRUE;
}

static int32_t qb_resolve_expression_flags_array_append(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, uint32_t *p_flags) {
	qb_operand *value = &operands[0];
	*p_flags = QB_ADDRESS_LITERAL_ARRAY;
	if(value->type == QB_OPERAND_ADDRESS) {
		if(value->address->dimension_count > 0) {
			*p_flags |= QB_ADDRESS_MULTIDIMENSIONAL_LITERAL_ARRAY;
		}
		*p_flags |= value->address->flags & (QB_ADDRESS_COMPLEX |  QB_ADDRESS_VECTOR | QB_ADDRESS_MATRIX);
	} else if(value->type == QB_OPERAND_RESULT_PROTOTYPE) {
		if(value->result_prototype->address_flags & QB_ADDRESS_LITERAL_ARRAY) {
			*p_flags |= QB_ADDRESS_MULTIDIMENSIONAL_LITERAL_ARRAY;
		}
		*p_flags |= value->result_prototype->address_flags & (QB_ADDRESS_COMPLEX |  QB_ADDRESS_VECTOR | QB_ADDRESS_MATRIX);
	}
	return TRUE;
}

static int32_t qb_resolve_expression_type_array_append(qb_compiler_context *cxt, qb_op_factory *f, uint32_t flags, qb_operand *operands, uint32_t operand_count, qb_primitive_type *p_type) {
	*p_type = QB_TYPE_ANY;
	return TRUE;
}

static int32_t qb_resolve_expression_type_rand(qb_compiler_context *cxt, qb_op_factory *f, uint32_t flags, qb_operand *operands, uint32_t operand_count, qb_primitive_type *p_type) {
	if(operand_count == 2) {
		return qb_resolve_expression_type_highest_rank(cxt, f, flags, operands, operand_count, p_type);
	} else {
		return qb_resolve_expression_type_lvalue(cxt, f, flags, operands, operand_count, p_type);
	}
}

static int32_t qb_resolve_expression_type_utf8_decode(qb_compiler_context *cxt, qb_op_factory *f, uint32_t flags, qb_operand *operands, uint32_t operand_count, qb_primitive_type *p_type) {
	*p_type = QB_TYPE_U16;
	return TRUE;
}

static int32_t qb_resolve_expression_type_unpack(qb_compiler_context *cxt, qb_op_factory *f, uint32_t flags, qb_operand *operands, uint32_t operand_count, qb_primitive_type *p_type) {
	qb_operand *type = &operands[2];
	*p_type = QB_TYPE_ANY;
	if(type->type == QB_OPERAND_ZVAL && type->constant->type == IS_LONG) {
		uint32_t value = Z_LVAL_P(type->constant);
		if(value < QB_TYPE_COUNT) {
			*p_type = value;
		}
	}
	return TRUE;
}

static int32_t qb_resolve_expression_type_append_string(qb_compiler_context *cxt, qb_op_factory *f, uint32_t flags, qb_operand *operands, uint32_t operand_count, qb_primitive_type *p_type) {
	qb_operand *string = &operands[0], *addend = &operands[1];
	qb_primitive_type string_type;
	if(string->type != QB_OPERAND_NONE) {
		string_type = qb_get_operand_type(cxt, string, 0);
	} else {
		string_type = QB_TYPE_U08;
	}
	if(addend->type == QB_OPERAND_ADDRESS && addend->address->flags & QB_ADDRESS_STRING) {
		if(addend->address->type > string_type) {
			string_type = addend->address->type;
		}
	}
	*p_type = string_type;
	return TRUE;
}

static int32_t qb_resolve_expression_type_append_char(qb_compiler_context *cxt, qb_op_factory *f, uint32_t flags, qb_operand *operands, uint32_t operand_count, qb_primitive_type *p_type) {
	qb_operand *string = &operands[0];
	qb_primitive_type string_type;
	if(string->type != QB_OPERAND_NONE) {
		string_type = qb_get_operand_type(cxt, string, 0);
	} else {
		string_type = QB_TYPE_U08;
	}
	*p_type = string_type;
	return TRUE;
}

static int32_t qb_get_string_append_type(qb_compiler_context *cxt, qb_operand *operand, qb_primitive_type expr_type) {
	if(operand->type == QB_OPERAND_ADDRESS && operand->address->flags & QB_ADDRESS_STRING) {
		return expr_type;
	} else if(operand->type == QB_OPERAND_RESULT_PROTOTYPE && operand->result_prototype->address_flags & QB_ADDRESS_STRING) {
		return expr_type;
	} else if(operand->type == QB_OPERAND_ZVAL && Z_TYPE_P(operand->constant) == IS_STRING) {
		return expr_type;
	}
	return QB_TYPE_ANY;
}

static int32_t qb_get_string_concat_type(qb_compiler_context *cxt, qb_operand *operand) {
	if(operand->type == QB_OPERAND_ADDRESS && operand->address->flags & QB_ADDRESS_STRING) {
		return operand->address->type;
	} else if(operand->type == QB_OPERAND_RESULT_PROTOTYPE && operand->result_prototype->address_flags & QB_ADDRESS_STRING) {
		return operand->result_prototype->final_type;
	}
	return QB_TYPE_U08;
}

static int32_t qb_resolve_expression_type_concat(qb_compiler_context *cxt, qb_op_factory *f, uint32_t flags, qb_operand *operands, uint32_t operand_count, qb_primitive_type *p_type) {
	qb_operand *augend = &operands[0], *addend = &operands[1];
	qb_primitive_type augend_type = qb_get_string_concat_type(cxt, augend);
	qb_primitive_type addend_type = qb_get_string_concat_type(cxt, addend);
	*p_type = (addend_type > augend_type) ? addend_type : augend_type;
	return TRUE;
}

static int32_t qb_resolve_expression_flags_function_call(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, uint32_t *p_flags) {
	USE_TSRM
	qb_operand *func = &operands[0];
	qb_function *qfunc = qb_find_compiled_function(func->zend_function TSRMLS_CC);
	if(qfunc->return_variable->address) {
		*p_flags = qfunc->return_variable->address->flags & (QB_ADDRESS_BOOLEAN | QB_ADDRESS_STRING | QB_ADDRESS_MATRIX | QB_ADDRESS_VECTOR | QB_ADDRESS_COMPLEX | QB_ADDRESS_IMAGE);
	} else {
		*p_flags = 0;
	}
	return TRUE;
}

static int32_t qb_resolve_expression_type_function_call(qb_compiler_context *cxt, qb_op_factory *f, uint32_t flags, qb_operand *operands, uint32_t operand_count, qb_primitive_type *p_type) {
	USE_TSRM
	qb_operand *func = &operands[0];
	qb_function *qfunc = qb_find_compiled_function(func->zend_function TSRMLS_CC);
	if(qfunc->return_variable->address) {
		*p_type = qfunc->return_variable->address->type;
	} else {
		*p_type = QB_TYPE_VOID;
	}
	return TRUE;
}

static int32_t qb_resolve_expression_type_zend_function_call(qb_compiler_context *cxt, qb_op_factory *f, uint32_t flags, qb_operand *operands, uint32_t operand_count, qb_primitive_type *p_type) {
	*p_type = QB_TYPE_ANY;
	return TRUE;
}

static int32_t qb_resolve_expression_flags_version_compare(qb_compiler_context *cxt, qb_op_factory *f, qb_operand *operands, uint32_t operand_count, uint32_t *p_flags) {
	if(operand_count == 3) {
		*p_flags = QB_ADDRESS_CONSTANT | QB_ADDRESS_BOOLEAN;
	} else {
		*p_flags = QB_ADDRESS_CONSTANT;
	}
	return TRUE;
}
