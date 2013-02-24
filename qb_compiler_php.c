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

static qb_variable * ZEND_FASTCALL qb_find_variable(qb_compiler_context *cxt, zend_class_entry *class, zval *name, uint32_t type_mask) {
	uint32_t i;
	ulong hash_value = Z_HASH_P(name);
	for(i = 0; i < cxt->variable_count; i++) {
		qb_variable *qvar = cxt->variables[i];
		if(qvar->hash_value == hash_value && qvar->name_length == Z_STRLEN_P(name)) {
			if(strncmp(qvar->name, Z_STRVAL_P(name), Z_STRLEN_P(name)) == 0) {
				if(qvar->zend_class == class) {
					if(qvar->flags & type_mask || !type_mask) {
						return qvar;
					}
				}
			}
		}
	}
	return NULL;
}

static qb_variable * ZEND_FASTCALL qb_obtain_class_variable(qb_compiler_context *cxt, zend_class_entry *class, zval *name) {
	qb_variable *qvar = qb_find_variable(cxt, class, name, QB_VARIABLE_CLASS | QB_VARIABLE_CLASS_INSTANCE);
	if(!qvar) {
		qvar = qb_allocate_variable(cxt->pool);
		qvar->flags = (class) ? QB_VARIABLE_CLASS : QB_VARIABLE_CLASS_INSTANCE;
		qvar->name = Z_STRVAL_P(name);
		qvar->name_length = Z_STRLEN_P(name);
		qvar->hash_value = Z_HASH_P(name);
		qvar->zend_class = class;
		qb_set_variable_type(cxt, qvar);
		qb_add_variable(cxt, qvar);
	}
	return qvar;
}

static qb_variable * ZEND_FASTCALL qb_obtain_instance_variable(qb_compiler_context *cxt, zval *name) {
	return qb_obtain_class_variable(cxt, NULL, name);
}

static uint32_t ZEND_FASTCALL qb_get_result_type(qb_compiler_context *cxt, uint32_t default_type);

static void ZEND_FASTCALL qb_retrieve_operand(qb_compiler_context *cxt, uint32_t zoperand_type, znode_op *zoperand, qb_operand *operand) {
	if(zoperand_type == Z_OPERAND_CV) {
		uint32_t var_index = Z_OPERAND_INFO(*zoperand, var);
		qb_variable *qvar = cxt->variables[var_index];
		if(!qvar->address) {
			// the variable type hasn't been set yet
			qvar->flags |= QB_VARIABLE_LOCAL;
			qb_set_variable_type(cxt, qvar);
		}
		operand->address = qvar->address;
		operand->type = QB_OPERAND_ADDRESS;
	} else if(zoperand_type == Z_OPERAND_CONST) {
		operand->constant = Z_OPERAND_ZV(*zoperand);
		operand->type = QB_OPERAND_ZVAL;
	} else if(zoperand_type == Z_OPERAND_TMP_VAR || zoperand_type == Z_OPERAND_VAR) {
		qb_operand *item = qb_pop_stack_item(cxt);
		*operand = *item;
	} else {
		operand->type = QB_OPERAND_NONE;
		operand->address = NULL;
	}
}

static void ZEND_FASTCALL qb_do_array_element_retrieval(qb_compiler_context *cxt, qb_operand *variable, qb_operand *value) {
	// the arguments hold the array and index before we change them
	qb_operand *container = variable;
	qb_operand *index = value;
	if(index->type == QB_OPERAND_NONE) {
		// an append operation
		if(!IS_EXPANDABLE_ARRAY(container->address)) {
			qb_abort("Adding element to an array that cannot expand");
		}
		index->address = container->address->dimension_addresses[0];
		index->type = QB_OPERAND_ADDRESS;
	} else {
		// make sure the index is a U32
		qb_do_type_coercion(cxt, index, QB_TYPE_U32);
	}
	variable->address = qb_get_array_element(cxt, container->address, index->address);

	// extra info is held in the next op
	if(cxt->zend_op[1].opcode == ZEND_OP_DATA) {
		// retrieve the actual value to assign the element to
		qb_retrieve_operand(cxt, Z_OPERAND_TYPE(cxt->zend_op[1].op1), &cxt->zend_op[1].op1, value);
	}
}

static void ZEND_FASTCALL qb_do_object_property_retrieval(qb_compiler_context *cxt, qb_operand *variable, qb_operand *value) {
	// the arguments hold the container (which should be null for $this) and the name of the property
	qb_operand *container = variable;
	qb_operand *name = value;
	if(name->type != QB_OPERAND_ZVAL) {
		qb_abort("No support for variable variable-names");
	}
	if(container->type == QB_OPERAND_NONE) {
		qb_variable *qvar = qb_obtain_instance_variable(cxt, name->constant);
		variable->address = qvar->address;
		variable->type = QB_OPERAND_ADDRESS;
	} else if(container->type == QB_OPERAND_ADDRESS) {
		variable->address = qb_get_named_element(cxt, container->address, name->constant);
		variable->type = QB_OPERAND_ADDRESS;
	} else {
		qb_abort("Cannot fetch property of objects other than $this");
	}
	if(cxt->zend_op[1].opcode == ZEND_OP_DATA) {
		// retrieve the actual value to assign the property to
		qb_retrieve_operand(cxt, Z_OPERAND_TYPE(cxt->zend_op[1].op1), &cxt->zend_op[1].op1, value);
	}
}

static void ZEND_FASTCALL qb_translate_assign(qb_compiler_context *cxt, qb_translator *t, qb_operand *variable, qb_operand *value, uint32_t result_flags) {
	if(cxt->zend_op->opcode == ZEND_ASSIGN_DIM) {
		qb_do_array_element_retrieval(cxt, variable, value);
	} else if(cxt->zend_op->opcode == ZEND_ASSIGN_OBJ) {
		qb_do_object_property_retrieval(cxt, variable, value);
	}
	qb_do_type_coercion(cxt, value, variable->address->type);
	qb_do_assignment(cxt, value->address, variable->address);
	if(result_flags & QB_RESULT_EXPECT_VARIABLE) {
		qb_push_address(cxt, variable->address);
	}
}

static void ZEND_FASTCALL qb_translate_assign_ref(qb_compiler_context *cxt, qb_translator *t, qb_operand *variable, qb_operand *target, uint32_t result_flags) {
	if(target->type == QB_OPERAND_NONE) {
		// initialization of static or global variable
	} else {
		qb_abort("No support for reference");
	}
}

static void ZEND_FASTCALL qb_translate_assign_temp(qb_compiler_context *cxt, qb_translator *t, qb_operand *value, qb_operand *unused_operand, uint32_t result_flags) {
	qb_address *result_address;
	int32_t variable_type = qb_get_result_type(cxt, QB_TYPE_I32);
	qb_do_type_coercion(cxt, value, variable_type);
	result_address = qb_obtain_qm_temporary_variable(cxt, variable_type, value->address->array_size_address);
	qb_push_address(cxt, result_address);
	qb_do_assignment(cxt, value->address, result_address);
}

static void ZEND_FASTCALL qb_translate_assign_op(qb_compiler_context *cxt, qb_translator *t, qb_operand *variable, qb_operand *value, uint32_t result_flags) {
	if(cxt->zend_op->extended_value == ZEND_ASSIGN_DIM) {
		qb_do_array_element_retrieval(cxt, variable, value);
	} else if(cxt->zend_op->extended_value == ZEND_ASSIGN_OBJ) {
		qb_do_object_property_retrieval(cxt, variable, value);
	}
	qb_do_type_coercion(cxt, value, variable->address->type);
	if(!cxt->resolving_result_type) {
		uint32_t *opcode_list = t->extra, opcode = opcode_list[variable->address->type];
		qb_append_op_with_three_addresses(cxt, opcode, variable->address, value->address, variable->address);
	}
	if(result_flags & QB_RESULT_EXPECT_VARIABLE) {
		qb_push_address(cxt, variable->address);
	}
}

static void ZEND_FASTCALL qb_translate_cast(qb_compiler_context *cxt, qb_translator *t, qb_operand *value, qb_operand *unused_operand, uint32_t result_flags) {
	uint32_t zend_type = cxt->zend_op->extended_value;
	switch(zend_type) {
		case IS_NULL:
		case IS_STRING:
		case IS_OBJECT: {
			const char *type_name = zend_get_type_by_const(zend_type);
			qb_abort("Cannot convert value to %s", type_name);
		}	break;
		case IS_ARRAY: {
			qb_push_address(cxt, value->address);
		}	break;
		case IS_BOOL: {
			//qb_address *temp_address = qb_push_temporary_variable(cxt, QB_TYPE_I32, NULL);
		}	break;
		case IS_LONG: {
			qb_address *temp_address = qb_push_temporary_variable(cxt, QB_TYPE_S64, NULL);
			qb_do_copy(cxt, value->address, temp_address);
		}	break;
		case IS_DOUBLE: {
			qb_address *temp_address = qb_push_temporary_variable(cxt, QB_TYPE_F64, NULL);
			qb_do_copy(cxt, value->address, temp_address);
		}	break;
	}
}

static void ZEND_FASTCALL qb_translate_arithmetic_op(qb_compiler_context *cxt, qb_translator *t, qb_operand *operand1, qb_operand *operand2, uint32_t result_flags) {
	qb_address *result_address, *result_size_address;
	uint32_t expr_type = qb_get_higher_rank_type(cxt, operand1, operand2, 0);
	if(expr_type != QB_TYPE_U64 && expr_type != QB_TYPE_S64 && expr_type != QB_TYPE_F64) {
		// see what the result is--promote the expression to it if it's higher
		uint32_t result_type = qb_get_result_type(cxt, expr_type);
		if(result_type > expr_type) {
			expr_type = result_type;
		}
	}
	qb_do_type_coercion(cxt, operand1, expr_type);
	qb_do_type_coercion(cxt, operand2, expr_type);
	result_size_address = qb_get_larger_array_size(cxt, operand1->address, operand2->address);
	if(!cxt->resolving_result_type) {
		uint32_t *opcode_list = t->extra, opcode = opcode_list[expr_type];
		if(operand1->address->flags & QB_ADDRESS_CONSTANT && operand2->address->flags & QB_ADDRESS_CONSTANT) {
			result_address = qb_push_new_constant(cxt, expr_type, result_size_address);
			qb_evaluate_op_with_three_addresses(cxt, opcode, operand1->address, operand2->address, result_address);
		} else {
			result_address = qb_push_temporary_variable(cxt, expr_type, result_size_address);
			qb_append_op_with_three_addresses(cxt, opcode, operand1->address, operand2->address, result_address);
		}
	} else {
		result_address = qb_push_temporary_variable(cxt, expr_type, result_size_address);
	}
}

static void ZEND_FASTCALL qb_translate_bitwise_op(qb_compiler_context *cxt, qb_translator *t, qb_operand *operand1, qb_operand *operand2, uint32_t result_flags) {
	qb_address *result_address, *result_size_address;
	uint32_t expr_type = qb_do_type_promotion(cxt, operand1, operand2, QB_COERCE_TO_INTEGER);
	result_size_address = qb_get_larger_array_size(cxt, operand1->address, operand2->address);
	if(!cxt->resolving_result_type) {
		uint32_t *opcode_list = t->extra, opcode = opcode_list[expr_type];
		if(!cxt->resolving_result_type && operand1->address->flags & QB_ADDRESS_CONSTANT && operand2->address->flags & QB_ADDRESS_CONSTANT) {
			result_address = qb_push_new_constant(cxt, expr_type, result_size_address);
			qb_evaluate_op_with_three_addresses(cxt, opcode, operand1->address, operand2->address, result_address);
		} else {
			result_address = qb_push_temporary_variable(cxt, expr_type, result_size_address);
			qb_append_op_with_three_addresses(cxt, opcode, operand1->address, operand2->address, result_address);
		}
	} else {
		result_address = qb_push_temporary_variable(cxt, expr_type, result_size_address);
	}
}

static void ZEND_FASTCALL qb_translate_compare_op(qb_compiler_context *cxt, qb_translator *t, qb_operand *operand1, qb_operand *operand2, uint32_t result_flags) {
	qb_address *result_address;
	qb_do_type_promotion(cxt, operand1, operand2, 0);
	if(!cxt->resolving_result_type) {
		uint32_t *opcode_list = t->extra, opcode = opcode_list[operand1->address->type];
		if((operand1->address->flags & QB_ADDRESS_CONSTANT) && (operand2->address->flags & QB_ADDRESS_CONSTANT)) {
			result_address = qb_push_new_constant(cxt, QB_TYPE_I32, NULL);
			qb_evaluate_op_with_two_addresses_plus_bool(cxt, opcode, operand1->address, operand2->address, result_address);
		} else {
			result_address = qb_push_temporary_variable(cxt, QB_TYPE_I32, NULL);
			qb_append_op_with_two_addresses_plus_bool(cxt, opcode, operand1->address, operand2->address, result_address);
		}
	} else {
		result_address = qb_push_temporary_variable(cxt, QB_TYPE_I32, NULL);
	}
}

static void ZEND_FASTCALL qb_translate_unary_op(qb_compiler_context *cxt, qb_translator *t, qb_operand *operand, qb_operand *unused_operand, uint32_t result_flags) {
	qb_address *result_address, *result_size_address;
	uint32_t expr_type = qb_get_operand_type(cxt, operand, 0);
	if(expr_type != QB_TYPE_U64 && expr_type != QB_TYPE_S64 && expr_type != QB_TYPE_F64) {
		uint32_t result_type = qb_get_result_type(cxt, expr_type);
		if(result_type > expr_type) {
			expr_type = result_type;
		}
	}
	qb_do_type_coercion(cxt, operand, expr_type);
	result_size_address = operand->address->array_size_address;
	if(!cxt->resolving_result_type) {
		uint32_t *opcode_list = t->extra, opcode = opcode_list[operand->address->type];
		if(operand->address->flags & QB_ADDRESS_CONSTANT) {
			result_address = qb_push_new_constant(cxt, expr_type, NULL);
			qb_evaluate_op_with_two_addresses(cxt, opcode, operand->address, result_address);
		} else {
			result_address = qb_push_temporary_variable(cxt, expr_type, result_size_address);
			qb_append_op_with_two_addresses(cxt, opcode, operand->address, result_address);
		}
	} else {
		result_address = qb_push_temporary_variable(cxt, expr_type, result_size_address);
	}
}

static void ZEND_FASTCALL qb_translate_incdec_pre(qb_compiler_context *cxt, qb_translator *t, qb_operand *variable, qb_operand *extra_operand, uint32_t result_flags) {
	if(cxt->zend_op->opcode == ZEND_PRE_INC_OBJ || cxt->zend_op->opcode == ZEND_PRE_DEC_OBJ) {
		qb_do_object_property_retrieval(cxt, variable, extra_operand);
	}
	if(!cxt->resolving_result_type) {
		uint32_t *opcode_list = t->extra;
		uint32_t opcode = opcode_list[variable->address->type];
		qb_append_op_with_one_address(cxt, opcode, variable->address);
	}
	if(result_flags & QB_RESULT_EXPECT_VARIABLE) {
		qb_push_address(cxt, variable->address);
	}
}

static void ZEND_FASTCALL qb_translate_incdec_post(qb_compiler_context *cxt, qb_translator *t, qb_operand *variable, qb_operand *extra_operand, uint32_t result_flags) {
	qb_address *result_address;
	if(cxt->zend_op->opcode == ZEND_POST_INC_OBJ || cxt->zend_op->opcode == ZEND_POST_DEC_OBJ) {
		qb_do_object_property_retrieval(cxt, variable, extra_operand);
	}
	result_address = qb_push_temporary_variable(cxt, variable->address->type, variable->address->array_size_address);
	if(!cxt->resolving_result_type) {
		uint32_t *opcode_list = t->extra, opcode = opcode_list[variable->address->type];
		qb_append_op_with_one_address(cxt, opcode, variable->address);
		qb_do_copy(cxt, variable->address, result_address);
	}
}

// zend_quick_get_constant() is not exported, so we need a copy of it here 
// when the module is compiled as dynamically linked library
#ifdef COMPILE_DL_QB

#if !ZEND_ENGINE_2_3 || !ZEND_ENGINE_2_2 || !ZEND_ENGINE_2_1
static int zend_get_special_constant(const char *name, uint name_len, zend_constant **c TSRMLS_DC)
{
	int ret;
	static char haltoff[] = "__COMPILER_HALT_OFFSET__";

	if (!EG(in_execution)) {
		return 0;
	} else if (name_len == sizeof("__CLASS__")-1 &&
	          !memcmp(name, "__CLASS__", sizeof("__CLASS__")-1)) {
		zend_constant tmp;

		/* Returned constants may be cached, so they have to be stored */
		if (EG(scope) && EG(scope)->name) {
			int const_name_len;
			char *const_name;
			ALLOCA_FLAG(use_heap)
			
			const_name_len = sizeof("\0__CLASS__") + EG(scope)->name_length;
			const_name = do_alloca(const_name_len, use_heap);
			memcpy(const_name, "\0__CLASS__", sizeof("\0__CLASS__")-1);
			zend_str_tolower_copy(const_name + sizeof("\0__CLASS__")-1, EG(scope)->name, EG(scope)->name_length);
			if (zend_hash_find(EG(zend_constants), const_name, const_name_len, (void**)c) == FAILURE) {
				zend_hash_add(EG(zend_constants), const_name, const_name_len, (void*)&tmp, sizeof(zend_constant), (void**)c);
				memset(*c, 0, sizeof(zend_constant));
				Z_STRVAL((**c).value) = estrndup(EG(scope)->name, EG(scope)->name_length);
				Z_STRLEN((**c).value) = EG(scope)->name_length;
				Z_TYPE((**c).value) = IS_STRING;
			}
			free_alloca(const_name, use_heap);
		} else {
			if (zend_hash_find(EG(zend_constants), "\0__CLASS__", sizeof("\0__CLASS__"), (void**)c) == FAILURE) {
				zend_hash_add(EG(zend_constants), "\0__CLASS__", sizeof("\0__CLASS__"), (void*)&tmp, sizeof(zend_constant), (void**)c);
				memset(*c, 0, sizeof(zend_constant));
				Z_STRVAL((**c).value) = estrndup("", 0);
				Z_STRLEN((**c).value) = 0;
				Z_TYPE((**c).value) = IS_STRING;
			}
		}
		return 1;
	} else if (name_len == sizeof("__COMPILER_HALT_OFFSET__")-1 &&
	          !memcmp(name, "__COMPILER_HALT_OFFSET__", sizeof("__COMPILER_HALT_OFFSET__")-1)) {
		const char *cfilename;
		char *haltname;
		int len, clen;

		cfilename = zend_get_executed_filename(TSRMLS_C);
		clen = strlen(cfilename);
		/* check for __COMPILER_HALT_OFFSET__ */
		zend_mangle_property_name(&haltname, &len, haltoff,
			sizeof("__COMPILER_HALT_OFFSET__") - 1, cfilename, clen, 0);
		ret = zend_hash_find(EG(zend_constants), haltname, len+1, (void **) c);
		efree(haltname);
		return ret == SUCCESS;
	} else {
		return 0;
	}
}

zend_constant *zend_quick_get_constant(const zend_literal *key, ulong flags TSRMLS_DC)
{
	zend_constant *c;

	if (zend_hash_quick_find(EG(zend_constants), Z_STRVAL(key->constant), Z_STRLEN(key->constant) + 1, key->hash_value, (void **) &c) == FAILURE) {
		key++;
		if (zend_hash_quick_find(EG(zend_constants), Z_STRVAL(key->constant), Z_STRLEN(key->constant) + 1, key->hash_value, (void **) &c) == FAILURE ||
		    (c->flags & CONST_CS) != 0) {
			if ((flags & (IS_CONSTANT_IN_NAMESPACE|IS_CONSTANT_UNQUALIFIED)) == (IS_CONSTANT_IN_NAMESPACE|IS_CONSTANT_UNQUALIFIED)) {
				key++;
				if (zend_hash_quick_find(EG(zend_constants), Z_STRVAL(key->constant), Z_STRLEN(key->constant) + 1, key->hash_value, (void **) &c) == FAILURE) {
				    key++;
					if (zend_hash_quick_find(EG(zend_constants), Z_STRVAL(key->constant), Z_STRLEN(key->constant) + 1, key->hash_value, (void **) &c) == FAILURE ||
					    (c->flags & CONST_CS) != 0) {

						key--;
						if (!zend_get_special_constant(Z_STRVAL(key->constant), Z_STRLEN(key->constant), &c TSRMLS_CC)) {
							return NULL;
						}
					}
				}
			} else {
				key--;
				if (!zend_get_special_constant(Z_STRVAL(key->constant), Z_STRLEN(key->constant), &c TSRMLS_CC)) {
					return NULL;
				}
			}
		}
	}
	return c;
}
#endif

#endif

static zval * ZEND_FASTCALL qb_get_type_constant(qb_compiler_context *cxt, const char *name, uint32_t length) {
	static zval constants[QB_TYPE_COUNT];
	zval *constant;
	uint32_t i;
	for(i = 0; i < QB_TYPE_COUNT; i++) {
		const char *type = type_names[i];
		if(strcmp(name, type) == 0) {
			constant = &constants[0];
			constant->type = IS_LONG;
			constant->value.lval = i;
			return constant;
		}
	}
	return NULL;
}

static void ZEND_FASTCALL qb_translate_fetch_constant(qb_compiler_context *cxt, qb_translator *t, qb_operand *container, qb_operand *name, uint32_t result_flags) {
	USE_TSRM

#if ZEND_ENGINE_2_2 || ZEND_ENGINE_2_1
	if(container->type == QB_OPERAND_ZVAL) {
		*container = *qb_pop_stack_item(cxt);
	}
#endif

	if(container->type == QB_OPERAND_ZEND_CLASS) {
		zval *name_value = name->constant;
		ulong hash_value = Z_HASH_P(name_value);
		zend_class_entry *ce = container->zend_class;
		zval **p_value;
		if(zend_hash_quick_find(&ce->constants_table, Z_STRVAL_P(name_value), Z_STRLEN_P(name_value) + 1, hash_value, (void **) &p_value) == SUCCESS) {
			qb_push_constant_zval(cxt, *p_value);
		} else {
			qb_abort("Undefined class constant '%s'", Z_STRVAL_P(name_value));
		}
	} else {
		zval *value = NULL;
#if !ZEND_ENGINE_2_3 && !ZEND_ENGINE_2_2 && !ZEND_ENGINE_2_1
		zend_literal *key = Z_OPERAND_INFO(cxt->zend_op->op2, literal) + 1;
		zend_constant *zconst = zend_quick_get_constant(key, cxt->zend_op->extended_value TSRMLS_CC);
		zval *name = &key->constant;
#else
		zval *name = Z_OPERAND_ZV(cxt->zend_op->op2);
		zend_constant *zconst;
		if(zend_hash_find(EG(zend_constants), Z_STRVAL_P(name), Z_STRLEN_P(name) + 1, (void **) &zconst) == FAILURE) {
			zconst = NULL;
		}
#endif
		if(zconst) {
			value = &zconst->value;
		} else {
			value = qb_get_type_constant(cxt, Z_STRVAL_P(name), Z_STRLEN_P(name));
		}
		if(value) {
			qb_push_constant_zval(cxt, value);
		} else {
			qb_abort("Undefined constant '%s'", Z_STRVAL_P(name));
		}
	}

}

static void ZEND_FASTCALL qb_translate_fetch(qb_compiler_context *cxt, qb_translator *t, qb_operand *name, qb_operand *scope, uint32_t result_flags) {
	USE_TSRM

	if(name->type == QB_OPERAND_ZVAL) {
#if ZEND_ENGINE_2_2 || ZEND_ENGINE_2_1
		if(scope->type == QB_OPERAND_ZVAL) {
			*scope = *qb_pop_stack_item(cxt);
		}
#endif

		if(scope->type == QB_OPERAND_NONE) {
#if !ZEND_ENGINE_2_3 && !ZEND_ENGINE_2_2 && !ZEND_ENGINE_2_1
			uint32_t fetch_type = cxt->zend_op->extended_value & ZEND_FETCH_TYPE_MASK;
#else
			uint32_t fetch_type = cxt->zend_op->op2.u.EA.type;
#endif
			if(fetch_type == ZEND_FETCH_LOCAL) {
				// look up the variable
				qb_variable *qvar = qb_find_variable(cxt, NULL, name->constant, 0);
				if(qvar) {
					if(!(qvar->flags & QB_VARIABLE_LOCAL)) {
						qvar->flags |= QB_VARIABLE_LOCAL;
						qb_set_variable_type(cxt, qvar);
					}
					qb_push_address(cxt, qvar->address);
				} else {
					qb_abort("Undefined variable: %s", Z_STRVAL_P(name->constant));
				}
			} else if(fetch_type == ZEND_FETCH_GLOBAL || fetch_type == ZEND_FETCH_GLOBAL_LOCK) {
				// look up the variable and mark it as global
				qb_variable *qvar = qb_find_variable(cxt, NULL, name->constant, 0);
				qb_operand *stack_item;
				if(qvar) {
					if(!(qvar->flags & QB_VARIABLE_GLOBAL)) {
						qvar->flags |= QB_VARIABLE_GLOBAL;
						qb_set_variable_type(cxt, qvar);
					}
				}
				// push a null onto the stack so the ZEND_ASSIGN_REF handler knows that
				// the original op is mapping a variable from the global/static symbol table
				stack_item = qb_push_stack_item(cxt);
				stack_item->type = QB_OPERAND_NONE;
				stack_item->address = NULL;
			} else if(fetch_type == ZEND_FETCH_STATIC) {
				// static variables are already handled in qb_add_variables()
				qb_operand *stack_item;
				stack_item = qb_push_stack_item(cxt);
				stack_item->type = QB_OPERAND_NONE;
				stack_item->address = NULL;
			}
		} else if(scope->type == QB_OPERAND_ZEND_CLASS) {
			// get class variable
			qb_variable *qvar = qb_obtain_class_variable(cxt, scope->zend_class, name->constant);
			qb_push_address(cxt, qvar->address);
		}
	} else {
		qb_abort("Internal error");
	}
}

static void ZEND_FASTCALL qb_translate_fetch_element(qb_compiler_context *cxt, qb_translator *t, qb_operand *container, qb_operand *index, uint32_t result_flags) {
	qb_address *result_address;
	qb_do_type_coercion(cxt, index, QB_TYPE_U32);
	result_address = qb_get_array_element(cxt, container->address, index->address);
	qb_push_address(cxt, result_address);
}

static void ZEND_FASTCALL qb_translate_fetch_property(qb_compiler_context *cxt, qb_translator *t, qb_operand *object, qb_operand *name, uint32_t result_flags) {
	if(name->type != QB_OPERAND_ZVAL) {
		qb_abort("No support for variable variable-names");
	}
	if(object->type == QB_OPERAND_NONE) {
		// this object
		qb_variable *qvar = qb_obtain_instance_variable(cxt, name->constant);
		qb_push_address(cxt, qvar->address);
	} else if(object->type == QB_OPERAND_ADDRESS) {
		// named element
		qb_address *element_address = qb_get_named_element(cxt, object->address, name->constant);
		qb_push_address(cxt, element_address);
	} else {
		qb_abort("Cannot fetch property of objects other than $this");
	}
}

static void ZEND_FASTCALL qb_translate_fetch_class(qb_compiler_context *cxt, qb_translator *t, qb_operand *class, qb_operand *name, uint32_t result_flags) {
	qb_operand *stack_item = qb_push_stack_item(cxt);
#if !ZEND_ENGINE_2_3 && !ZEND_ENGINE_2_2 && !ZEND_ENGINE_2_1
	int32_t fetch_type = cxt->zend_op->extended_value & ZEND_FETCH_CLASS_MASK;
#else
	int32_t fetch_type = cxt->zend_op->extended_value;
#endif
	if(fetch_type == ZEND_FETCH_CLASS_SELF) {
		stack_item->zend_class = cxt->zend_class;
#if !ZEND_ENGINE_2_2 && !ZEND_ENGINE_2_1
	} else if(fetch_type == ZEND_FETCH_CLASS_STATIC) {
		stack_item->zend_class = cxt->zend_class;
#endif
	} else if(fetch_type == ZEND_FETCH_CLASS_PARENT) {
		stack_item->zend_class = (cxt->zend_class) ? cxt->zend_class->parent : NULL;
	}
	stack_item->type = QB_OPERAND_ZEND_CLASS;
}

static void ZEND_FASTCALL qb_translate_receive_argument(qb_compiler_context *cxt, qb_translator *t, qb_operand *unused_operand1, qb_operand *init_value, uint32_t result_flags) {
	if(init_value->type == QB_OPERAND_ZVAL) {
#if !ZEND_ENGINE_2_3 && !ZEND_ENGINE_2_2 && !ZEND_ENGINE_2_1
		uint32_t argument_number = Z_OPERAND_INFO(cxt->zend_op->op1, num);
#else
		uint32_t argument_number = Z_LVAL_P(Z_OPERAND_ZV(cxt->zend_op->op1));
#endif
		qb_variable *qvar = cxt->variables[argument_number - 1];
		qvar->default_value_address = qb_obtain_zval_constant(cxt, init_value->constant, qvar->address->type);
	}
}

static void ZEND_FASTCALL qb_translate_return(qb_compiler_context *cxt, qb_translator *t, qb_operand *retval, qb_operand *unused_operand, uint32_t result_flags) {
	if(cxt->return_variable->address) {
		qb_do_type_coercion(cxt, retval, cxt->return_variable->address->type);
	} else {
		if(retval->type != QB_OPERAND_NONE && !(retval->type == QB_OPERAND_ZVAL && retval->constant->type == IS_NULL)) {
			qb_abort("Returning a value in a function with return type declared to be void");
		}
	}
	if(!cxt->resolving_result_type) {
		if(cxt->return_variable->address) {
			qb_do_assignment(cxt, retval->address, cxt->return_variable->address);
		}
		qb_append_simple_op(cxt, return_opcode);
	}
	cxt->jump_target_index1 = QB_INSTRUCTION_END;
}

static void ZEND_FASTCALL qb_translate_exit(qb_compiler_context *cxt, qb_translator *t, qb_operand *status, qb_operand *unused_operand, uint32_t result_flags) {
	if(status->type != QB_OPERAND_NONE) {
		if(status->type == QB_OPERAND_ZVAL && Z_TYPE_P(status->constant) == IS_STRING) {
			qb_do_type_coercion(cxt, status, QB_TYPE_U08);
		} else if(status->type == QB_OPERAND_ADDRESS && status->address->flags & QB_ADDRESS_STRING) {
			// it's a string already
		} else {
			qb_do_type_coercion(cxt, status, QB_TYPE_S32);
			if(!IS_SCALAR(status->address)) {
				qb_abort("Exit status code must be a scalar");
			}
		}
	}
	if(!cxt->resolving_result_type) {
		if(status->type == QB_OPERAND_NONE) {
			status->address = qb_obtain_constant_S32(cxt, 0);
		} else if(status->address->flags & QB_ADDRESS_STRING) {
			qb_op *qop = qb_append_op_with_one_address(cxt, QB_PRN_STR_U08, status->address);
			qop->flags |= QB_OP_CANNOT_REMOVE;
			status->address = qb_obtain_constant_S32(cxt, 0);
		}
		qb_append_op_with_one_address(cxt, QB_EXIT_I32, status->address);
	}
	cxt->jump_target_index1 = QB_INSTRUCTION_END;
}

static void ZEND_FASTCALL qb_translate_jump(qb_compiler_context *cxt, qb_translator *t, qb_operand *unused_operand1, qb_operand *unused_operand2, uint32_t result_flags) {
	zend_op *target_op = Z_OPERAND_INFO(cxt->zend_op->op1, jmp_addr);
	uint32_t target_index = ZEND_OP_INDEX(target_op);

	if(!cxt->resolving_result_type) {
		qb_append_op_with_jump_target(cxt, jump_opcode, target_index);
	}
	cxt->jump_target_index1 = target_index;
}

static void ZEND_FASTCALL qb_translate_jump_set(qb_compiler_context *cxt, qb_translator *t, qb_operand *unused_operand1, qb_operand *unused_operand2, uint32_t result_flags) {
	// TODO
}

static void ZEND_FASTCALL qb_translate_branch(qb_compiler_context *cxt, qb_translator *t, qb_operand *condition, qb_operand *unused_operand, uint32_t result_flags) {
	zend_op *target_op1 = Z_OPERAND_INFO(cxt->zend_op->op2, jmp_addr);
	uint32_t target_index1 = ZEND_OP_INDEX(target_op1);
	uint32_t target_index2 = cxt->zend_op_index + 1;
	uint32_t *opcode_list = t->extra, opcode = opcode_list[0];

	if(condition->type == QB_OPERAND_ADDRESS) {
		if(!IS_SCALAR(condition->address)) {
			// use the size as the condition
			condition->address = condition->address->array_size_address;
		}
	} 
	qb_do_type_coercion(cxt, condition, QB_TYPE_I32);

	if(result_flags & QB_RESULT_EXPECT_TEMPORARY) {
		// push the condition back onto the stack for the purpose of short-circuiting logical statements
		if(!(condition->address->flags & QB_ADDRESS_REUSED)) {
			qb_address *new_address = qb_allocate_address(cxt->pool);
			*new_address = *condition->address;
			new_address->flags |= QB_ADDRESS_REUSED;
			condition->address = new_address;
		}
		qb_push_address(cxt, condition->address);
		cxt->pop_short_circuiting_bool = TRUE;
	}

	if(condition->address->flags & QB_ADDRESS_CONSTANT) {
		int32_t is_true = VALUE(I32, condition->address);
		if((is_true && opcode == branch_on_true_opcode) || (!is_true && opcode == branch_on_false_opcode)) {
			if(!cxt->resolving_result_type) {
				qb_append_op_with_jump_target(cxt, jump_opcode, target_index1);
			}
			cxt->jump_target_index1 = target_index1;
		}
	} else {
		if(!cxt->resolving_result_type) {
			uint32_t *opcode_list = t->extra, opcode = opcode_list[0];
			qb_append_op_with_branch_targets_plus_one_address(cxt, opcode, target_index1, target_index2, condition->address);
		}

		// start down the next instruction first before going down the branch
		cxt->jump_target_index1 = target_index2;
		cxt->jump_target_index2 = target_index1;
	}
}


static void ZEND_FASTCALL qb_translate_for_loop(qb_compiler_context *cxt, qb_translator *t, qb_operand *condition, qb_operand *unused_operand2, uint32_t result_flags) {
	uint32_t target_index1 = cxt->zend_op->extended_value;
	uint32_t target_index2 = Z_OPERAND_INFO(cxt->zend_op->op2, opline_num);
	uint32_t *opcode_list = t->extra;
	uint32_t opcode = opcode_list[0];

	qb_do_type_coercion(cxt, condition, QB_TYPE_I32);

	if(!cxt->resolving_result_type) {
		qb_append_op_with_branch_targets_plus_one_address(cxt, opcode, target_index1, target_index2, condition->address);
	}

	cxt->jump_target_index1 = target_index1;
	cxt->jump_target_index2 = target_index2;
}

static zend_brk_cont_element * ZEND_FASTCALL qb_find_break_continue_element(qb_compiler_context *cxt, int32_t nest_levels, int32_t array_offset) {
	zend_brk_cont_element *brk_cont_array = cxt->zend_op_array->brk_cont_array, *jmp_to;
	int32_t i;
	for(i = 0; i < nest_levels; i++) {
		if(array_offset == -1) {
			qb_abort("Cannot break/continue %d level%s", nest_levels, (nest_levels == 1) ? "" : "s");
		}
		jmp_to = &brk_cont_array[array_offset];
		array_offset = jmp_to->parent;
	}
	return jmp_to;
}

static void ZEND_FASTCALL qb_translate_break(qb_compiler_context *cxt, qb_translator *t, qb_operand *unused_operand1, qb_operand *unused_operand2, uint32_t result_flags) {
	zval *nest_level = Z_OPERAND_ZV(cxt->zend_op->op2);
	zend_brk_cont_element *jmp_to = qb_find_break_continue_element(cxt, Z_LVAL_P(nest_level), Z_OPERAND_INFO(cxt->zend_op->op1, opline_num));

	if(!cxt->resolving_result_type) {
		qb_append_op_with_jump_target(cxt, jump_opcode, jmp_to->brk);
	}
	cxt->jump_target_index1 = jmp_to->brk;
}

static void ZEND_FASTCALL qb_translate_continue(qb_compiler_context *cxt, qb_translator *t, qb_operand *unused_operand1, qb_operand *unused_operand2, uint32_t result_flags) {
	zval *nest_level = Z_OPERAND_ZV(cxt->zend_op->op2);
	zend_brk_cont_element *jmp_to = qb_find_break_continue_element(cxt, Z_LVAL_P(nest_level), Z_OPERAND_INFO(cxt->zend_op->op1, opline_num));

	if(!cxt->resolving_result_type) {
		qb_append_op_with_jump_target(cxt, jump_opcode, jmp_to->cont);
	}
	cxt->jump_target_index1 = jmp_to->cont;
}

static void ZEND_FASTCALL qb_translate_add_element(qb_compiler_context *cxt, qb_translator *t, qb_operand *value, qb_operand *index, uint32_t result_flags) {
	qb_operand *element, **stack_items = qb_get_stack_items(cxt, cxt->stack_item_count - 1);
	qb_array_initializer *initializer = stack_items[0]->array_initializer;
	uint32_t element_index;
	if(index->type == QB_OPERAND_NONE) {
		element_index = initializer->element_count;
	} else {
		if(Z_TYPE_P(index->constant) == IS_LONG) {
			element_index = Z_LVAL_P(index->constant);
		} else {
			qb_abort("String key encountered: %s", Z_STRVAL_P(index->constant));
		}
	}
	// don't add the item if when we're just looking for the result type
	if(!cxt->resolving_result_type) {
		element = qb_expand_array_initializer(cxt, initializer, element_index);
		*element = *value;
	}
}

static void ZEND_FASTCALL qb_translate_init_array(qb_compiler_context *cxt, qb_translator *t, qb_operand *value, qb_operand *index, uint32_t result_flags) {
	qb_array_initializer *initializer = qb_allocate_array_initializer(cxt->pool);
	qb_push_array_initializer(cxt, initializer);

	if(value->type != QB_OPERAND_NONE) {
		qb_translate_add_element(cxt, t, value, index, 0);
	}
}

static void ZEND_FASTCALL qb_translate_foreach_reset(qb_compiler_context *cxt, qb_translator *t, qb_operand *container, qb_operand *unused_operand, uint32_t result_flags) {
	// set the index to -1 (since the increment has to happen before the assignment)
	qb_address *index_address = qb_create_scalar(cxt, QB_TYPE_U32);
	qb_address *neg1_address = qb_obtain_constant_S32(cxt, -1);
	qb_do_copy(cxt, neg1_address, index_address);
	index_address->flags |= QB_ADDRESS_FOREACH_INDEX;

	// push the index first
	qb_push_address(cxt, index_address);

	// not checking if container is an array here since that'll be done in the handler for FOREACH_FETCH
	qb_push_address(cxt, container->address);
}

static void ZEND_FASTCALL qb_translate_foreach_fetch(qb_compiler_context *cxt, qb_translator *t, qb_operand *container, qb_operand *unused_operand, uint32_t result_flags) {
	uint32_t target_index = Z_OPERAND_INFO(cxt->zend_op->op2, opline_num);
	qb_operand *stack_item;
	qb_address *index_address, *dimension_address, *element_address;

	// pop the index off the stack
	stack_item = qb_pop_stack_item(cxt);
	index_address = stack_item->address;

	// push it back on if the key is needed
	if(cxt->zend_op->extended_value & ZEND_FE_FETCH_WITH_KEY) {
		qb_push_address(cxt, index_address);
	}

	if(cxt->zend_op->extended_value & ZEND_FE_FETCH_BYREF) {
		qb_abort("Reference is not supported");
	}

	// increment the index
	qb_append_op_with_one_address(cxt, increment_opcodes[QB_TYPE_U32], index_address);

	// jump out of the loop when the index equals or greater than the dimension
	dimension_address = container->address->dimension_addresses[0];
	qb_append_op_with_branch_targets_plus_two_addresses(cxt, QB_IF_LT_U32_U32, QB_INSTRUCTION_NEXT, target_index, index_address, dimension_address);

	// get the element and push it onto the stack
	element_address = qb_get_array_element(cxt, container->address, index_address);
	qb_push_address(cxt, element_address);

	cxt->jump_target_index1 = cxt->zend_op_index + 1;
	cxt->jump_target_index2 = target_index;
}

static void ZEND_FASTCALL qb_translate_case(qb_compiler_context *cxt, qb_translator *t, qb_operand *value, qb_operand *constant, uint32_t result_flags) {
	qb_address *result_address;
	if(value->type == QB_OPERAND_ZVAL) {
		// just in case someone decides to use a constant--coerce it to I64
		qb_do_type_coercion(cxt, value, QB_TYPE_I64);
	}

	// coerce the constant to the same type
	qb_do_type_coercion(cxt, constant, value->address->type);

	// push the value back onto the stack
	qb_push_address(cxt, value->address);

	result_address = qb_push_temporary_variable(cxt, QB_TYPE_I32, NULL);
	if(!cxt->resolving_result_type) {
		uint32_t opcode = equal_opcodes[value->address->type];
		qb_append_op_with_three_addresses(cxt, opcode, value->address, constant->address, result_address);
	}
}

static void ZEND_FASTCALL qb_translate_free(qb_compiler_context *cxt, qb_translator *t, qb_operand *value, qb_operand *unused_operand, uint32_t result_flags) {
	// if it's a temporary variable being freed, perhaps it's not needed in the first place
	if(value->type == QB_OPERAND_ADDRESS && value->address->flags & QB_ADDRESS_TEMPORARY) {
		qb_op *last_qop = cxt->ops[cxt->op_count - 1];
		// if the op that put the value on the stack has no side-effect, then we can remove it
		// otherwise it has to be kept, eventhough the value it produces is ignored
		if(!(last_qop->flags & QB_OP_CANNOT_REMOVE)) {
			if(last_qop->operand_count > 0) {
				qb_operand *last_operand = &last_qop->operands[last_qop->operand_count - 1];
				if(last_operand->type == QB_OPERAND_ADDRESS && last_operand->address == value->address) {
					// make it a nop
					last_qop->opcode = QB_NOP;
				}
			}
		}
	}
	qb_do_type_coercion(cxt, value, QB_TYPE_VOID);
}

static void ZEND_FASTCALL qb_translate_echo(qb_compiler_context *cxt, qb_translator *t, qb_operand *value, qb_operand *unused_operand, uint32_t result_flags) {
	qb_do_type_coercion(cxt, value, QB_TYPE_ANY);

	if(!cxt->resolving_result_type) {
		qb_op *qop;
		if(value->address->flags & QB_ADDRESS_STRING) {
			qop = qb_append_op_with_one_address(cxt, QB_PRN_STR_U08, value->address);
		} else {
			uint32_t opcode = print_opcodes[value->address->type];
			qop = qb_append_op_with_one_address(cxt, opcode, value->address);
		}
		qop->flags |= QB_OP_CANNOT_REMOVE;
	}
	if(cxt->zend_op->opcode == ZEND_PRINT) {
		qb_address *one_address = qb_obtain_constant_S32(cxt, 1);
		qb_push_address(cxt, one_address);
	}
}

static void ZEND_FASTCALL qb_translate_init_string(qb_compiler_context *cxt, qb_translator *t, qb_operand *string, qb_operand *addition, uint32_t result_flags) {
	// Zend Engine 2.2 only--high versions skip this step
	qb_address *result_address;
	result_address = qb_obtain_temporary_variable_length_array(cxt, QB_TYPE_U08);
	result_address->flags |= QB_ADDRESS_STRING;
	qb_do_unset(cxt, result_address);
	qb_push_address(cxt, result_address);
}

static void ZEND_FASTCALL qb_translate_add_string(qb_compiler_context *cxt, qb_translator *t, qb_operand *string, qb_operand *addition, uint32_t result_flags) {
	qb_address *result_address;

	if(string->type == QB_OPERAND_NONE) {
		result_address = qb_obtain_temporary_variable_length_array(cxt, QB_TYPE_U08);
		result_address->flags |= QB_ADDRESS_STRING;
		// make sure the destination is empty
		qb_do_unset(cxt, result_address);
	} else {
		result_address = string->address;
	}
	if(addition->type == QB_OPERAND_ZVAL) {
		qb_do_type_coercion(cxt, addition, QB_TYPE_U08);
		if(IS_SCALAR(addition->address)) {
			// it's an ADD_CHAR op
			addition->address->flags |= QB_ADDRESS_STRING;
		}
	}

	qb_do_concat(cxt, addition->address, result_address);
	qb_push_address(cxt, result_address);
}

static void ZEND_FASTCALL qb_translate_concat(qb_compiler_context *cxt, qb_translator *t, qb_operand *operand1, qb_operand *operand2, uint32_t result_flags) {
	qb_address *result_address;

	// handle zval constants
	if(operand1->type == QB_OPERAND_ZVAL) {
		qb_do_type_coercion(cxt, operand1, QB_TYPE_ANY);
	}
	if(operand2->type == QB_OPERAND_ZVAL) {
		qb_do_type_coercion(cxt, operand2, QB_TYPE_ANY);
	}

	result_address = qb_obtain_temporary_variable_length_array(cxt, QB_TYPE_U08);
	result_address->flags |= QB_ADDRESS_STRING;
	qb_push_address(cxt, result_address);

	if(result_address != operand1->address) {
		// if the first operand isn't a previously allocated temporary string, 
		// then clear it and append to it
		qb_do_unset(cxt, result_address);
		qb_do_concat(cxt, operand1->address, result_address);
	}

	qb_do_concat(cxt, operand2->address, result_address);
}

static void ZEND_FASTCALL qb_translate_assign_concat(qb_compiler_context *cxt, qb_translator *t, qb_operand *variable, qb_operand *operand, uint32_t result_flags) {
	if(operand->type == QB_OPERAND_ZVAL) {
		qb_do_type_coercion(cxt, operand, QB_TYPE_ANY);
	}
	if((variable->address->type & ~QB_TYPE_UNSIGNED) != QB_TYPE_I08) {
		qb_abort("Cannot append to a variable of type %s", type_names[variable->address->type]);
	}
	qb_do_concat(cxt, operand->address, variable->address);
	if(result_flags & QB_RESULT_EXPECT_VARIABLE) {
		qb_push_address(cxt, variable->address);
	}
}

static void ZEND_FASTCALL qb_translate_bool(qb_compiler_context *cxt, qb_translator *t, qb_operand *value, qb_operand *unused_operand, uint32_t result_flags) {
	qb_address *result_address;

	if(value->type == QB_OPERAND_ZVAL) {
		int32_t is_true = zend_is_true(value->constant);
		result_address = qb_obtain_constant_S32(cxt, is_true ? 1 : 0);
		qb_push_address(cxt, result_address);
	} else if(value->type == QB_OPERAND_ADDRESS) {
		qb_address *zero_address;

		// if it's a array, use the size (i.e. non-empty means true)
		if(!IS_SCALAR(value->address)) {
			value->address = value->address->array_size_address;
		}

		// comp with zero
		zero_address = qb_obtain_constant(cxt, 0, value->address->type);
		result_address = qb_push_temporary_variable(cxt, QB_TYPE_I32, NULL);
		if(!cxt->resolving_result_type) {
			uint32_t opcode = not_equal_opcodes[value->address->type];
			qb_append_op_with_two_addresses_plus_bool(cxt, opcode, value->address, zero_address, result_address);
		}
	} else if(value->type == QB_OPERAND_PREVIOUS_RESULT) {
		qb_do_type_coercion(cxt, value, QB_TYPE_ANY);
	}
}

static void ZEND_FASTCALL qb_translate_isset(qb_compiler_context *cxt, qb_translator *t, qb_operand *variable, qb_operand *unused_operand, uint32_t result_flags) {
	qb_translate_bool(cxt, t, variable, unused_operand, result_flags);
}

static void ZEND_FASTCALL qb_translate_isset_element(qb_compiler_context *cxt, qb_translator *t, qb_operand *container, qb_operand *index, uint32_t result_flags) {
	qb_address *variable_address, *result_address;

	variable_address = qb_get_array_element(cxt, container->address, index->address);
	result_address = qb_push_temporary_variable(cxt, QB_TYPE_I32, NULL);

	if(!cxt->resolving_result_type) {
		uint32_t opcode = isset_opcodes[variable_address->type];
		qb_append_op_with_two_addresses(cxt, opcode, variable_address, result_address);
	}
}

static void ZEND_FASTCALL qb_translate_unset(qb_compiler_context *cxt, qb_translator *t, qb_operand *variable, qb_operand *unused_operand, uint32_t result_flags) {
}

static void ZEND_FASTCALL qb_translate_begin_silence(qb_compiler_context *cxt, qb_translator *t, qb_operand *unused_operand1, qb_operand *unused_operand2, uint32_t result_flags) {
	cxt->silence = 1;
}

static void ZEND_FASTCALL qb_translate_end_silence(qb_compiler_context *cxt, qb_translator *t, qb_operand *value, qb_operand *unused_operand, uint32_t result_flags) {
	cxt->silence = 0;
	if(value->type != QB_OPERAND_NONE) {
		// push the value back onto the stack
		if(value->type == QB_OPERAND_ZVAL) {
			qb_push_constant_zval(cxt, value->constant);
		} else {
			qb_push_address(cxt, value->address);

		}
	}
}

#include "qb_compiler_php_intrinsic.c"

static zend_function * ZEND_FASTCALL qb_find_function(qb_compiler_context *cxt, zval *object, zend_class_entry *class, zval *name) {
	USE_TSRM

	// check to see if we've found the function already (this function is called whenever an argument is pushed)
	if(cxt->previous_object != object || cxt->previous_class != class || cxt->previous_function_name != name) {
		char *error = NULL;
		HashTable ht;
		int32_t use_array_callable = FALSE;
		zval *callable = name, *class_name;
		zval _array_callable, *array_callable = &_array_callable;
		zend_fcall_info_cache fcc;

		if(class && !object) {
			// no good way to get the class entry to zend_is_callable_ex() except by creating an array
			zend_hash_init(&ht, sizeof(zval *), NULL, NULL, 0);

			class_name = qb_string_to_zval(class->name, class->name_length);

			Z_ARRVAL_P(array_callable) = &ht;
			Z_TYPE_P(array_callable) = IS_ARRAY;

			zend_hash_next_index_insert(&ht, &class_name, sizeof(zval *), NULL);
			zend_hash_next_index_insert(&ht, &callable, sizeof(zval *), NULL);

			callable = array_callable;
			use_array_callable = TRUE;
		}
#if !ZEND_ENGINE_2_2 && !ZEND_ENGINE_2_1
		if(!zend_is_callable_ex(callable, object, 0, NULL, NULL, &fcc, &error TSRMLS_CC)) {
			qb_abort("%s", error);
		}
		if(error) {
			efree(error);
		}
#else
		if(!class && object) {
			zend_hash_init(&ht, sizeof(zval *), NULL, NULL, 0);

			Z_ARRVAL_P(array_callable) = &ht;
			Z_TYPE_P(array_callable) = IS_ARRAY;

			zend_hash_next_index_insert(&ht, &object, sizeof(zval *), NULL);
			zend_hash_next_index_insert(&ht, &callable, sizeof(zval *), NULL);

			callable = array_callable;
			use_array_callable = TRUE;
		}
		{
			// suppress the non-static function being called as static warning message
			int error_reporting_before = EG(error_reporting);
			EG(error_reporting) = 0;
			if(!zend_is_callable_ex(callable, 0, NULL, NULL, NULL, &fcc.function_handler, &fcc.object_pp TSRMLS_CC)) {
				qb_abort("Cannot find function: %s", Z_STRVAL_P(name));
			}
			EG(error_reporting) = error_reporting_before;
		}
#endif
		if(use_array_callable) {
			zend_hash_destroy(&ht);
		}
		cxt->previous_object = object;
		cxt->previous_class = class;
		cxt->previous_function_name = name;
		cxt->previous_function = fcc.function_handler;
	}
	return cxt->previous_function;
}

static qb_function * ZEND_FASTCALL qb_get_compiled_function(qb_compiler_context *cxt, zend_function *zfunc) {
	uint32_t i;
	if(qb_is_compiled_function(zfunc)) {
		return zfunc->op_array.reserved[0];
	} else {
		// see if the function in question in also in the middle of being compiled
		USE_TSRM
		qb_build_context *build_context = QB_G(build_context);
		for(i = 0; i < build_context->compiler_context_count; i++) {
			qb_compiler_context *compiler_cxt = &build_context->compiler_contexts[i];
			if(compiler_cxt->function_prototype.zend_function == zfunc) {
				return &compiler_cxt->function_prototype;
			}
		}
	}
	return NULL;
}

static uint32_t ZEND_FASTCALL qb_do_argument_coercion(qb_compiler_context *cxt, qb_operand *argument) {
	uint32_t type = QB_TYPE_UNKNOWN, send_count = 0, argument_index, i;
	zval *function_name = NULL, *object = NULL;
	zend_class_entry *class = NULL;

	// find the function call
	for(i = cxt->zend_op_index; i < cxt->zend_op_array->last && !function_name; i++) {
		zend_op *zop = &cxt->zend_op_array->opcodes[i];
		switch(zop->opcode) {
			case ZEND_SEND_VAR:
			case ZEND_SEND_REF:
			case ZEND_SEND_VAL:
			case ZEND_SEND_VAR_NO_REF: {
				send_count++;
			}	break;
			case ZEND_DO_FCALL: {
				uint32_t argument_count = zop->extended_value;
				if(argument_count >= send_count) {
					argument_index = argument_count - send_count;
					function_name = Z_OPERAND_ZV(zop->op1);
				} else {
					send_count -= argument_count;
				}
			}	break;
			case ZEND_DO_FCALL_BY_NAME: {
				uint32_t argument_count = zop->extended_value;
				if(argument_count >= send_count) {
					qb_operand **stack_items;
					argument_index = argument_count - send_count;
					stack_items = qb_get_stack_items(cxt, cxt->stack_item_count - argument_index - 2);
					if(stack_items[0]->type == QB_OPERAND_ZVAL) {
						function_name = stack_items[0]->constant;
					} else {
						qb_abort("Internal error");
					}
					if(stack_items[1]->type == QB_OPERAND_ZVAL) {
						object = stack_items[1]->constant;
					} else if(stack_items[1]->type == QB_OPERAND_ZEND_CLASS) {
						class = stack_items[1]->zend_class;
					}
				} else {
					send_count -= argument_count;
				}
			}	break;
		}
	}

	if(function_name) {
		qb_intrinsic_function *ifunc = qb_find_intrinsic_function(cxt, function_name);
		if(ifunc) {
			// try to coerce the argument to the correct type
			// the purpose of doing type coercion here is so qb_get_result_type() can return more quickly
			// otherwise all subsequent arguments will have to be evaluated before we know what type this one should have
			// sometimes, this is in fact the case: when the arguments all have to be of the same type
			if(argument_index < ifunc->argument_count_max) {
				if(ifunc->do_type_coercion) {
					ifunc->do_type_coercion(cxt, ifunc, argument, argument_index);
				}
			} else {
				// coerce it to whatever so qb_get_result_type() can exit
				qb_do_type_coercion(cxt, argument, QB_TYPE_ANY);
			}
		} else {
			zend_function *zfunc = qb_find_function(cxt, object, class, function_name);
			if(zfunc) {
				qb_function *qfunc = qb_get_compiled_function(cxt, zfunc);
				if(qfunc) {
					if(argument_index < qfunc->argument_count) {
						qb_variable *qvar = qfunc->variables[argument_index];

						if(qvar->flags & QB_VARIABLE_PASSED_BY_REF) {
							if(!STORAGE_TYPE_MATCH(qvar->address->type, argument->address->type)) {
								qb_abort("%s expects argument %d to be of the type %s, %s was passed", qfunc->name, argument_index + 1, type_names[qvar->address->type], type_names[argument->address->type]);
							}
							if(argument->address->flags & (QB_ADDRESS_TEMPORARY | QB_ADDRESS_QM_TEMPORARY)) {
								qb_abort("Only variable should be passed by reference");
							}

						} else {
							qb_do_type_coercion(cxt, argument, qvar->address->type);
						}
						if(IS_EXPANDABLE_ARRAY(qvar->address) && !IS_EXPANDABLE_ARRAY(argument->address)) {
							qb_abort("%s expects argument %d to be of a variable length array", qfunc->name, argument_index + 1);
						}
					}
				} else {
					qb_do_type_coercion(cxt, argument, QB_TYPE_ANY);
				}
			} else {
				qb_abort("Missing function");
			}
		}
	}
	return type;
}

static void ZEND_FASTCALL qb_translate_send_argument(qb_compiler_context *cxt, qb_translator *t, qb_operand *value, qb_operand *unused_operand, uint32_t result_flags) {
	qb_operand *stack_item;
	qb_do_argument_coercion(cxt, value);
	stack_item = qb_push_stack_item(cxt);
	*stack_item = *value;
}

static void ZEND_FASTCALL qb_translate_init_method_call(qb_compiler_context *cxt, qb_translator *t, qb_operand *object, qb_operand *name, uint32_t result_flags) {
	qb_operand *stack_item;

#if ZEND_ENGINE_2_2 || ZEND_ENGINE_2_1
	if(object->type == QB_OPERAND_ZVAL) {
		*object = *qb_pop_stack_item(cxt);
	}
#endif

	stack_item = qb_push_stack_item(cxt);	// function name
	*stack_item = *name;
	stack_item = qb_push_stack_item(cxt);	// object
	if(object->type == QB_OPERAND_NONE) {
		if(cxt->zend_this_object) {
			stack_item->type = QB_OPERAND_ZVAL;
			stack_item->constant = cxt->zend_this_object;
		} else if(cxt->zend_class) {
			stack_item->type = QB_OPERAND_ZEND_CLASS;
			stack_item->zend_class = cxt->zend_class;
		}
	} else if(object->type == QB_OPERAND_ZEND_CLASS) {
		stack_item->type = QB_OPERAND_ZEND_CLASS;
		stack_item->zend_class = object->zend_class;
	} else {
		qb_abort("No support for object method invocation");
	}
}

static void ZEND_FASTCALL qb_translate_function_call_init(qb_compiler_context *cxt, qb_translator *t, qb_operand *object, qb_operand *name, uint32_t result_flags) {
	qb_operand *stack_item;
	stack_item = qb_push_stack_item(cxt);	// function name
	*stack_item = *name;

	stack_item = qb_push_stack_item(cxt);	// object

#if !ZEND_ENGINE_2_3
	*stack_item = *object;
#else
	// for some reason in Zend Engine 2.3, the name of the function would show up here
	if(object->type == QB_OPERAND_ZVAL && object->constant->type == IS_OBJECT) {
		*stack_item = *object;
	} else {
		stack_item->type = QB_OPERAND_NONE;
		stack_item->address = NULL;
	}
#endif
}

static void ZEND_FASTCALL qb_translate_function_call(qb_compiler_context *cxt, qb_translator *t, qb_operand *name, qb_operand *object, uint32_t result_flags) {
	qb_intrinsic_function *ifunc;
	qb_operand arguments[256], **stack_pointer;
	uint32_t argument_count, i;

	if(name->type != QB_OPERAND_ZVAL || Z_TYPE_P(name->constant) != IS_STRING) {
		qb_abort("Function name is not a literal string");
	}

	argument_count = cxt->zend_op->extended_value;
	if(argument_count > 256) {
		qb_abort("Too many arguments");
	}
	stack_pointer = qb_get_stack_items(cxt, cxt->stack_item_count - argument_count);
	for(i = 0; i < argument_count; i++) {
		arguments[i] = *stack_pointer[i];
	}

	// remove the items from the stack
	cxt->stack_item_count -= argument_count;

	ifunc = (object->type == QB_OPERAND_NONE) ? qb_find_intrinsic_function(cxt, name->constant) : NULL;
	if(ifunc) {
		if(argument_count < ifunc->argument_count_min || argument_count > ifunc->argument_count_max) {
			if(ifunc->argument_count_min == ifunc->argument_count_max) {
				qb_abort("%s() expects %d arguments but %d was passed", ifunc->name, ifunc->argument_count_min, argument_count);
			} else {
				qb_abort("%s() expects %d to %d arguments but %d was passed", ifunc->name, ifunc->argument_count_min, ifunc->argument_count_max, argument_count);
			}
		}
		ifunc->translate(cxt, ifunc, arguments, argument_count, result_flags);
	} else {
		zend_function *zfunc;
		qb_function *qfunc;
		int32_t result_size_known, result_is_array;
		uint32_t result_type, result_size;
		qb_address *result_address;

		// no intrinsic function found--look for the function
		if(object->type == QB_OPERAND_ZVAL) {
			zfunc = qb_find_function(cxt, object->constant, NULL, name->constant);
		} else if(object->type == QB_OPERAND_ZEND_CLASS) {
			zfunc = qb_find_function(cxt, NULL, object->zend_class, name->constant);
		} else {
			zfunc = qb_find_function(cxt, NULL, NULL, name->constant);
		}

		// see if it's a qb function
		qfunc = qb_get_compiled_function(cxt, zfunc);

		// the result type is determined by the caller; it's up to the interpreter to coerce 
		// the result into the right type; query subsequent opcodes to see what is expected
		// (if a return value is expected
		if(result_flags & (QB_RESULT_EXPECT_TEMPORARY | QB_RESULT_EXPECT_VARIABLE)) {
			result_type = qb_get_result_type(cxt, QB_TYPE_ANY);
		} else {
			result_type = QB_TYPE_VOID;
		}

		// the length of the result is determined by the callee, however
		// if it's another qb function, then we might know exactly how
		// long it's going to be; if it's Zend function, then we don't
		if(result_type != QB_TYPE_VOID) {
			if(qfunc) {
				if(qfunc->return_variable->address) {
					qb_address *retval_address = qfunc->return_variable->address;
					if(IS_SCALAR(retval_address)) {
						result_size = 1;
						result_size_known = TRUE;
						result_is_array = FALSE;
					} else {
						if(IS_FIXED_LENGTH_ARRAY(retval_address)) {
							// the length is fixed
							result_size = VALUE_IN(qfunc->local_storage, U32, retval_address->array_size_address);
							result_size_known = TRUE;
							result_is_array = TRUE;
						} else {
							// it's variable
							result_size = 0;
							result_size_known = FALSE;
							result_is_array = TRUE;
						}
					}
					if(result_type == QB_TYPE_ANY) {
						// the op that eventually gets the result will accept anything, so use what the callee returns
						result_type = retval_address->type;
					}
				} else {
					if(result_type == QB_TYPE_ANY) {
						result_type = QB_TYPE_VOID;
					} else {
						qb_abort("Cannot obtain the return value from %s() as it is declared void", qfunc->name);
					}
				}
			} else {
				if(result_type == QB_TYPE_ANY) {
					qb_abort("Cannot determine the return value type from context");
				} else {
					// we know the type expected, but not the size
					result_size = 0;
					result_size_known = FALSE;
				}
			}
		} else {
			// if the return value is ignored, use an empty array
			// the interpreter will respect this
			result_type = QB_TYPE_I32;
			result_size = 0;
			result_size_known = TRUE;
		}

		if(result_size_known) {
			if(result_is_array) {
				result_address = qb_obtain_temporary_fixed_length_array(cxt, result_type, result_size);
			} else {
				result_address = qb_obtain_temporary_scalar(cxt, result_type);
			}
		} else {
			result_address = qb_obtain_temporary_variable_length_array(cxt, result_type);
		}

		qb_do_function_call(cxt, zfunc, arguments, argument_count, result_address);
		if(result_flags & (QB_RESULT_EXPECT_TEMPORARY | QB_RESULT_EXPECT_VARIABLE)) {
			qb_push_address(cxt, result_address);
		}
	}

	for(i = 0; i < argument_count; i++) {
		qb_operand *argument = &arguments[i];
		if(argument->type == QB_OPERAND_ADDRESS) {
			qb_remove_reference(cxt, argument->address);
		}
	}
}

static void ZEND_FASTCALL qb_translate_function_call_by_name(qb_compiler_context *cxt, qb_translator *t, qb_operand *unused_operand1, qb_operand *unused_operand2, uint32_t result_flags) {
	qb_operand **stack_items, *name, *object;
	uint32_t argument_count, i, name_index;

	argument_count = cxt->zend_op->extended_value;
	name_index = cxt->stack_item_count - argument_count - 2;
	stack_items = qb_get_stack_items(cxt, name_index);
	name = stack_items[0];
	object = stack_items[1];
	qb_translate_function_call(cxt, t, name, object, result_flags);

	// get rid of the name and the object
	stack_items = qb_get_stack_items(cxt, name_index);
	for(i = 2; i < cxt->stack_item_count; i++) {
		*stack_items[i - 2] = *stack_items[i];
	}
	cxt->stack_item_count -= 2; 
}

static void ZEND_FASTCALL qb_translate_user_opcode(qb_compiler_context *cxt, qb_translator *t, qb_operand *unused_operand1, qb_operand *unused_operand2, uint32_t result_flags) {
}

static void ZEND_FASTCALL qb_translate_extension_op(qb_compiler_context *cxt, qb_translator *t, qb_operand *unused_operand1, qb_operand *unused_operand2, uint32_t result_flags) {
	if(!cxt->resolving_result_type) {
		USE_TSRM
		if(QB_G(allow_debugger_inspection)) {
			qb_address *op_code_address = qb_obtain_constant_U32(cxt, cxt->zend_op->opcode);
			qb_append_op_with_one_address(cxt, QB_EXT_U32, op_code_address);
			cxt->function_flags |= QB_ENGINE_GO_THRU_ZEND;
		}
	}
}

static qb_translator op_translators[] = {
	{	NULL,								NULL						},	// ZEND_NOP
	{	qb_translate_arithmetic_op,			add_opcodes					},	// ZEND_ADD
	{	qb_translate_arithmetic_op,			subtract_opcodes			},	// ZEND_SUB
	{	qb_translate_arithmetic_op,			multiply_opcodes			},	// ZEND_MUL
	{	qb_translate_arithmetic_op,			divide_opcodes				},	// ZEND_DIV
	{	qb_translate_arithmetic_op,			modulo_opcodes				},	// ZEND_MOD
	{	qb_translate_bitwise_op,			shift_left_opcodes			},	// ZEND_SL
	{	qb_translate_bitwise_op,			shift_right_opcodes			},	// ZEND_SR
	{	qb_translate_concat,				NULL						},	// ZEND_CONCAT
	{	qb_translate_bitwise_op,			bitwise_or_opcodes			},	// ZEND_BW_OR
	{	qb_translate_bitwise_op,			bitwise_and_opcodes			},	// ZEND_BW_AND
	{	qb_translate_bitwise_op,			bitwise_xor_opcodes			},	// ZEND_BW_XOR
	{	qb_translate_unary_op,				bitwise_not_opcodes			},	// ZEND_BW_NOT
	{	qb_translate_unary_op,				logical_not_opcodes			},	// ZEND_BOOL_NOT
	{	qb_translate_bitwise_op,			logical_xor_opcodes			},	// ZEND_BOOL_XOR
	{	qb_translate_compare_op,			equal_opcodes				},	// ZEND_IS_IDENTICAL
	{	qb_translate_compare_op,			not_equal_opcodes			},	// ZEND_IS_NOT_IDENTICAL
	{	qb_translate_compare_op,			equal_opcodes				},	// ZEND_IS_EQUAL
	{	qb_translate_compare_op,			not_equal_opcodes			},	// ZEND_IS_NOT_EQUAL
	{	qb_translate_compare_op,			less_than_opcodes			},	// ZEND_IS_SMALLER
	{	qb_translate_compare_op,			less_or_equal_opcodes		},	// ZEND_IS_SMALLER_OR_EQUAL
	{	qb_translate_cast,					NULL						},	// ZEND_CAST
	{	qb_translate_assign_temp,			NULL						},	// ZEND_QM_ASSIGN
	{	qb_translate_assign_op,				add_opcodes					},	// ZEND_ASSIGN_ADD
	{	qb_translate_assign_op,				subtract_opcodes			},	// ZEND_ASSIGN_SUB
	{	qb_translate_assign_op,				multiply_opcodes			},	// ZEND_ASSIGN_MUL
	{	qb_translate_assign_op,				divide_opcodes				},	// ZEND_ASSIGN_DIV
	{	qb_translate_assign_op,				modulo_opcodes				},	// ZEND_ASSIGN_MOD
	{	qb_translate_assign_op,				shift_left_opcodes			},	// ZEND_ASSIGN_SL
	{	qb_translate_assign_op,				shift_right_opcodes			},	// ZEND_ASSIGN_SR
	{	qb_translate_assign_concat,			NULL						},	// ZEND_ASSIGN_CONCAT
	{	qb_translate_assign_op,				bitwise_or_opcodes			},	// ZEND_ASSIGN_BW_OR
	{	qb_translate_assign_op,				bitwise_and_opcodes			},	// ZEND_ASSIGN_BW_AND
	{	qb_translate_assign_op,				bitwise_xor_opcodes			},	// ZEND_ASSIGN_BW_XOR
	{	qb_translate_incdec_pre,			increment_opcodes			},	// ZEND_PRE_INC
	{	qb_translate_incdec_pre,			decrement_opcodes			},	// ZEND_PRE_DEC
	{	qb_translate_incdec_post,			increment_opcodes			},	// ZEND_POST_INC
	{	qb_translate_incdec_post,			decrement_opcodes			},	// ZEND_POST_DEC
	{	qb_translate_assign,				NULL						},	// ZEND_ASSIGN
	{	qb_translate_assign_ref,			NULL						},	// ZEND_ASSIGN_REF
	{	qb_translate_echo,					NULL						},	// ZEND_ECHO
	{	qb_translate_echo,					NULL						},	// ZEND_PRINT
	{	qb_translate_jump,					NULL						},	// ZEND_JMP
	{	qb_translate_branch,				&branch_on_false_opcode		},	// ZEND_JMPZ
	{	qb_translate_branch,				&branch_on_true_opcode		},	// ZEND_JMPNZ
	{	qb_translate_for_loop,				&branch_on_true_opcode		},	// ZEND_JMPZNZ
	{	qb_translate_branch,				&branch_on_false_opcode		},	// ZEND_JMPZ_EX
	{	qb_translate_branch,				&branch_on_true_opcode		},	// ZEND_JMPNZ_EX
	{	qb_translate_case,					NULL						},	// ZEND_CASE
	{	qb_translate_free,					NULL						},	// ZEND_SWITCH_FREE
	{	qb_translate_break,					NULL						},	// ZEND_BRK
	{	qb_translate_continue,				NULL						},	// ZEND_CONT
	{	qb_translate_bool,					NULL						},	// ZEND_BOOL
	{	qb_translate_init_string,			NULL						},	// ZEND_INIT_STRING
	{	qb_translate_add_string,			NULL						},	// ZEND_ADD_CHAR
	{	qb_translate_add_string,			NULL						},	// ZEND_ADD_STRING
	{	qb_translate_add_string,			NULL						},	// ZEND_ADD_VAR
	{	qb_translate_begin_silence,			NULL						},	// ZEND_BEGIN_SILENCE
	{	qb_translate_end_silence,			NULL						},	// ZEND_END_SILENCE
	{	qb_translate_function_call_init,	NULL						},	// ZEND_INIT_FCALL_BY_NAME
	{	qb_translate_function_call,			NULL						},	// ZEND_DO_FCALL
	{	qb_translate_function_call_by_name,	NULL						},	// ZEND_DO_FCALL_BY_NAME
	{	qb_translate_return,				NULL						},	// ZEND_RETURN
	{	qb_translate_receive_argument,		NULL						},	// ZEND_RECV
	{	qb_translate_receive_argument,		NULL						},	// ZEND_RECV_INIT
	{	qb_translate_send_argument,			NULL						},	// ZEND_SEND_VAL
	{	qb_translate_send_argument,			NULL						},	// ZEND_SEND_VAR
	{	qb_translate_send_argument,			NULL						},	// ZEND_SEND_REF
	{	NULL,								NULL						},	// ZEND_NEW
	{	NULL,								NULL						},	// ZEND_INIT_NS_FCALL_BY_NAME
	{	qb_translate_free,					NULL						},	// ZEND_FREE
	{	qb_translate_init_array,			NULL						},	// ZEND_INIT_ARRAY
	{	qb_translate_add_element,			NULL						},	// ZEND_ADD_ARRAY_ELEMENT
	{	NULL,								NULL						},	// ZEND_INCLUDE_OR_EVAL
	{	qb_translate_unset,					NULL						},	// ZEND_UNSET_VAR
	{	qb_translate_unset,					NULL						},	// ZEND_UNSET_DIM
	{	NULL,								NULL						},	// ZEND_UNSET_OBJ
	{	qb_translate_foreach_reset,			NULL						},	// ZEND_FE_RESET
	{	qb_translate_foreach_fetch,			NULL						},	// ZEND_FE_FETCH
	{	qb_translate_exit,					NULL						},	// ZEND_EXIT
	{	qb_translate_fetch,					NULL						},	// ZEND_FETCH_R
	{	qb_translate_fetch_element,			NULL						},	// ZEND_FETCH_DIM_R
	{	qb_translate_fetch_property,		NULL						},	// ZEND_FETCH_OBJ_R
	{	qb_translate_fetch,					NULL						},	// ZEND_FETCH_W
	{	qb_translate_fetch_element,			NULL						},	// ZEND_FETCH_DIM_W
	{	qb_translate_fetch_property,		NULL						},	// ZEND_FETCH_OBJ_W
	{	qb_translate_fetch,					NULL						},	// ZEND_FETCH_RW
	{	qb_translate_fetch_element,			NULL						},	// ZEND_FETCH_DIM_RW
	{	qb_translate_fetch_property,		NULL						},	// ZEND_FETCH_OBJ_RW
	{	qb_translate_fetch,					NULL						},	// ZEND_FETCH_IS
	{	qb_translate_fetch_element,			NULL						},	// ZEND_FETCH_DIM_IS
	{	qb_translate_fetch_property,		NULL						},	// ZEND_FETCH_OBJ_IS
	{	qb_translate_fetch,					NULL						},	// ZEND_FETCH_FUNC_ARG
	{	qb_translate_fetch_element,			NULL						},	// ZEND_FETCH_DIM_FUNC_ARG
	{	qb_translate_fetch_property,		NULL						},	// ZEND_FETCH_OBJ_FUNC_ARG
	{	qb_translate_fetch,					NULL						},	// ZEND_FETCH_UNSET
	{	qb_translate_fetch_element,			NULL						},	// ZEND_FETCH_DIM_UNSET
	{	qb_translate_fetch_property,		NULL						},	// ZEND_FETCH_OBJ_UNSET
	{	qb_translate_fetch_element,			NULL						},	// ZEND_FETCH_DIM_TMP_VAR
	{	qb_translate_fetch_constant,		NULL						},	// ZEND_FETCH_CONSTANT
	{	NULL,								NULL						},	// ZEND_GOTO
	{	qb_translate_extension_op,			NULL						},	// ZEND_EXT_STMT
	{	qb_translate_extension_op,			NULL						},	// ZEND_EXT_FCALL_BEGIN
	{	qb_translate_extension_op,			NULL						},	// ZEND_EXT_FCALL_END
	{	qb_translate_extension_op,			NULL						},	// ZEND_EXT_NOP
	{	NULL,								NULL						},	// ZEND_TICKS
	{	qb_translate_send_argument,			NULL						},	// ZEND_SEND_VAR_NO_REF
	{	NULL,								NULL						},	// ZEND_CATCH
	{	NULL,								NULL						},	// ZEND_THROW
	{	qb_translate_fetch_class,			NULL						},	// ZEND_FETCH_CLASS
	{	NULL,								NULL						},	// ZEND_CLONE
	{	NULL,								NULL						},	// ZEND_RETURN_BY_REF
	{	qb_translate_init_method_call,		NULL						},	// ZEND_INIT_METHOD_CALL
	{	qb_translate_init_method_call,		NULL						},	// ZEND_INIT_STATIC_METHOD_CALL
	{	qb_translate_isset,					NULL						},	// ZEND_ISSET_ISEMPTY_VAR
	{	qb_translate_isset_element,			NULL						},	// ZEND_ISSET_ISEMPTY_DIM_OBJ
	{	NULL,								NULL						},	// 116
	{	NULL,								NULL						},	// 117
	{	NULL,								NULL						},	// 118
	{	NULL,								NULL						},	// 119
	{	NULL,								NULL						},	// 120
	{	NULL,								NULL						},	// 121
	{	NULL,								NULL						},	// 122
	{	NULL,								NULL						},	// 123
	{	NULL,								NULL						},	// 124
	{	NULL,								NULL						},	// 125
	{	NULL,								NULL						},	// 126
	{	NULL,								NULL						},	// 127
	{	NULL,								NULL						},	// 128
	{	NULL,								NULL						},	// 129
	{	NULL,								NULL						},	// 130
	{	NULL,								NULL						},	// 131
	{	qb_translate_incdec_pre,			increment_opcodes			},	// ZEND_PRE_INC_OBJ
	{	qb_translate_incdec_pre,			decrement_opcodes			},	// ZEND_PRE_DEC_OBJ
	{	qb_translate_incdec_post,			increment_opcodes			},	// ZEND_POST_INC_OBJ
	{	qb_translate_incdec_post,			decrement_opcodes			},	// ZEND_POST_DEC_OBJ
	{	qb_translate_assign,				NULL						},	// ZEND_ASSIGN_OBJ
	{	NULL,								NULL						},	// ZEND_OP_DATA
	{	NULL,								NULL						},	// ZEND_INSTANCEOF
	{	NULL,								NULL						},	// ZEND_DECLARE_CLASS
	{	NULL,								NULL						},	// ZEND_DECLARE_INHERITED_CLASS
	{	NULL,								NULL						},	// ZEND_DECLARE_FUNCTION
	{	NULL,								NULL						},	// ZEND_RAISE_ABSTRACT_ERROR
	{	NULL,								NULL						},	// ZEND_DECLARE_CONST
	{	NULL,								NULL						},	// ZEND_ADD_INTERFACE
	{	NULL,								NULL						},	// ZEND_DECLARE_INHERITED_CLASS_DELAYED
	{	NULL,								NULL						},	// ZEND_VERIFY_ABSTRACT_CLASS
	{	qb_translate_assign,				NULL						},	// ZEND_ASSIGN_DIM
	{	qb_translate_fetch_property,		NULL						},	// ZEND_ISSET_ISEMPTY_PROP_OBJ
	{	NULL,								NULL						},	// ZEND_HANDLE_EXCEPTION
	{	qb_translate_user_opcode,			NULL						},	// ZEND_USER_OPCODE
	{	NULL,								NULL						},	// 151
	{	qb_translate_jump_set,				NULL						},	// ZEND_JMP_SET
	{	NULL,								NULL						},	// TODO: ZEND_DECLARE_LAMBDA_FUNCTION
	{	NULL,								NULL						},	// ZEND_ADD_TRAIT
	{	NULL,								NULL						},	// ZEND_BIND_TRAITS
	{	NULL,								NULL						},	// ZEND_SEPARATE
	{	qb_translate_assign_temp,			NULL						},	// ZEND_QM_ASSIGN_VAR
	{	qb_translate_jump_set,				NULL						},	// ZEND_JMP_SET_VAR
};

static void ZEND_FASTCALL qb_translate_current_instruction(qb_compiler_context *cxt) {
	if(cxt->zend_op->opcode != ZEND_OP_DATA) {
		qb_operand operands[2], *operand1 = &operands[0], *operand2 = &operands[1];
		qb_translator *t;
		int result_type;
		uint32_t result_flags = 0;

		// retrieve operands (second one first)
		qb_retrieve_operand(cxt, Z_OPERAND_TYPE(cxt->zend_op->op2), &cxt->zend_op->op2, operand2);
		qb_retrieve_operand(cxt, Z_OPERAND_TYPE(cxt->zend_op->op1), &cxt->zend_op->op1, operand1);

		// see whether the op returns a value
		if(RETURN_VALUE_USED(cxt->zend_op)) {
			result_type = Z_OPERAND_TYPE(cxt->zend_op->result);
			if(result_type == Z_OPERAND_TMP_VAR) {
				result_flags |= QB_RESULT_EXPECT_TEMPORARY;
			} else if(result_type == Z_OPERAND_VAR) {
				result_flags |= QB_RESULT_EXPECT_VARIABLE;
			}
		}

		// look up the translator for this opcode
		t = &op_translators[cxt->zend_op->opcode];
		if(t->translate) {
			cxt->line_number = cxt->zend_op->lineno;
			t->translate(cxt, t, operand1, operand2, result_flags);
		}
	}
}

static uint32_t ZEND_FASTCALL qb_get_result_type(qb_compiler_context *cxt, uint32_t default_type) {
	uint32_t zend_op_index, stack_item_offset, stack_item_count, resolving_result_type;
	uint32_t jump_target_index1, jump_target_index2;
	uint32_t result_type = QB_TYPE_UNKNOWN;
	qb_operand *result;

#if ZEND_DEBUG
	if(default_type == QB_TYPE_UNKNOWN) {
		qb_abort("Default type cannot be unknown: use void instead");
	}
#endif

	// save state variables
	zend_op_index = cxt->zend_op_index;
	jump_target_index1 = cxt->jump_target_index1;
	jump_target_index2 = cxt->jump_target_index2;
	resolving_result_type = cxt->resolving_result_type;
	qb_preserve_stack(cxt, &stack_item_offset, &stack_item_count);

	// push a result operand onto stack
	result = qb_push_stack_item(cxt);
	result->type = QB_OPERAND_PREVIOUS_RESULT;
	result->result_type = &result_type;

	// process zend instructions until the type become known
	cxt->zend_op++;
	cxt->zend_op_index++;
	cxt->default_result_type = default_type;
	cxt->resolving_result_type = 1;
	for(;;) {
		qb_translate_current_instruction(cxt);

		if(result_type != QB_TYPE_UNKNOWN) {
			break;
		} 
		if(cxt->jump_target_index1) {
			if(cxt->jump_target_index1 != 0xFFFFFFFF && cxt->jump_target_index1 > cxt->zend_op_index) {
				cxt->zend_op = ZEND_OP(cxt->jump_target_index1);
				cxt->zend_op_index = cxt->jump_target_index1;
				cxt->jump_target_index1 = 0;
				cxt->jump_target_index2 = 0;
			} else {
				// the result is ignore if we encounter a return or backward jump
				result_type = QB_TYPE_VOID;
				break;
			}
		} else {
			cxt->zend_op++;
			cxt->zend_op_index++;
		}
	}

	// what to use when there is no place for the value to go
	if(result_type == QB_TYPE_VOID) {
		result_type = default_type;
	}

	// restore state
	cxt->zend_op = ZEND_OP(zend_op_index);
	cxt->zend_op_index = zend_op_index;
	cxt->jump_target_index1 = jump_target_index1;
	cxt->jump_target_index2 = jump_target_index2;
	cxt->resolving_result_type = resolving_result_type;
	qb_restore_stack(cxt, stack_item_offset, stack_item_count);
	return result_type;
}

static void ZEND_FASTCALL qb_translate_instruction_range(qb_compiler_context *cxt, uint32_t start_index, uint32_t end_index) {
	uint32_t zend_op_index, stack_item_offset, stack_item_count;

	// save states
	zend_op_index = cxt->zend_op_index;
	qb_preserve_stack(cxt, &stack_item_offset, &stack_item_count);

	if(cxt->pop_short_circuiting_bool) {
		qb_pop_stack_item(cxt);
		cxt->pop_short_circuiting_bool = FALSE;
	}

	// process zend instructions until we reach the end 
	// or if an instruction is already translated
	cxt->zend_op_index = start_index;
	cxt->zend_op = ZEND_OP(start_index);
	for(;;) {
		uint32_t current_op_count;

		if(cxt->op_translations[cxt->zend_op_index] != QB_OP_INDEX_NONE && cxt->op_translations[cxt->zend_op_index] != QB_OP_INDEX_JUMP_TARGET) {
			// instruction has already been translated--do a jump there and exit
			qb_append_op_with_jump_target(cxt, jump_opcode, cxt->zend_op_index);
			break;
		}

		// translate the current instruction, saving the op-count
		// so we know where the first resulting new op is
		current_op_count = cxt->op_count;
		qb_translate_current_instruction(cxt);

		// add a nop if new one wasn't generated
		if(current_op_count == cxt->op_count) {
			qb_append_simple_op(cxt, QB_NOP);
		}

		// flag new op as a jump target if there's a placeholder in the position
		if(cxt->op_translations[cxt->zend_op_index] == QB_OP_INDEX_JUMP_TARGET) {
			qb_op *first_op = cxt->ops[current_op_count];
			first_op->flags |= QB_OP_JUMP_TARGET;
		}
		cxt->op_translations[cxt->zend_op_index] = current_op_count;
		cxt->zend_op_processed_count++;

		// see if it was a branch or a jump
		if(cxt->jump_target_index1) {
			uint32_t target_index1 = cxt->jump_target_index1;
			uint32_t target_index2 = cxt->jump_target_index2;
			cxt->jump_target_index1 = 0;
			cxt->jump_target_index2 = 0;
			if(target_index1 == QB_INSTRUCTION_END) {
				break;
			} else if(target_index2) {
				qb_translate_instruction_range(cxt, target_index1, target_index2);
			} else {
				cxt->zend_op = ZEND_OP(target_index1);
				cxt->zend_op_index = target_index1;
			}
			if(target_index2) {
				cxt->zend_op = ZEND_OP(target_index2);
				cxt->zend_op_index = target_index2;
			}
		} else {
			cxt->zend_op++;
			cxt->zend_op_index++;
		}
		if(cxt->zend_op_index == end_index) {
			break;
		}
	}

	// restore the stack
	qb_restore_stack(cxt, stack_item_offset, stack_item_count);
	cxt->zend_op = ZEND_OP(zend_op_index);
	cxt->zend_op_index = zend_op_index;
}

static void ZEND_FASTCALL qb_translate_instructions(qb_compiler_context *cxt) {
	cxt->op_translations = qb_allocate_indices(cxt->pool, cxt->zend_op_array->last);
	memset(cxt->op_translations, 0xFF, cxt->zend_op_array->last * sizeof(uint32_t));
	if(cxt->op_count > 0) {
		// there are instructions for initializing static variables
		// the first translated instruction is going to be a jump target
		cxt->op_translations[0] = QB_OP_INDEX_JUMP_TARGET;
	}
	qb_translate_instruction_range(cxt, 0, cxt->zend_op_array->last);

	// make sure there's always a RET at the end
	if(cxt->op_count == 0 || cxt->ops[cxt->op_count - 1]->opcode != QB_RET) {
		qb_append_simple_op(cxt, return_opcode);
	}
}

extern const char compressed_table_zend_op_names[];

static zend_always_inline const char * qb_get_zend_op_name(qb_compiler_context *cxt, uint32_t opcode) {
	if(!cxt->pool->zend_op_names) {
		qb_uncompress_table(compressed_table_zend_op_names, (void ***) &cxt->pool->zend_op_names, NULL, 0);
		if(!cxt->pool->zend_op_names) {
			qb_abort("Unable to decompress table");
		}
	}
	return cxt->pool->zend_op_names[opcode];
}

static void ZEND_FASTCALL qb_print_zend_ops(qb_compiler_context *cxt) {
	uint32_t i = 0;
	for(i = 0; i < cxt->zend_op_array->last; i++) {
		zend_op *zop = &cxt->zend_op_array->opcodes[i];
		const char *opname = qb_get_zend_op_name(cxt, zop->opcode);
		php_printf("%s (line number: %d)\n", opname, zop->lineno);
	}
}
