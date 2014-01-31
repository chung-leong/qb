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

#include "qb.h"
#include "zend_variables.h"

static void qb_element_to_zval(zval *zvalue, int8_t *bytes, uint32_t type) {
	switch(type) {
		case QB_TYPE_S08: {
			Z_TYPE_P(zvalue) = IS_LONG;
			Z_LVAL_P(zvalue) = ((CTYPE(S08) *) bytes)[0];
		}	break;
		case QB_TYPE_U08: {
			Z_TYPE_P(zvalue) = IS_LONG;
			Z_LVAL_P(zvalue) = ((CTYPE(U08) *) bytes)[0];
		}	break;
		case QB_TYPE_S16: {
			Z_TYPE_P(zvalue) = IS_LONG;
			Z_LVAL_P(zvalue) = ((CTYPE(S16) *) bytes)[0];
		}	break;
		case QB_TYPE_U16: {
			Z_TYPE_P(zvalue) = IS_LONG;
			Z_LVAL_P(zvalue) = ((CTYPE(U16) *) bytes)[0];
		}	break;
		case QB_TYPE_S32: {
			Z_TYPE_P(zvalue) = IS_LONG;
			Z_LVAL_P(zvalue) = ((CTYPE(S32) *) bytes)[0];
		}	break;
		case QB_TYPE_U32: {
			Z_TYPE_P(zvalue) = IS_LONG;
			Z_LVAL_P(zvalue) = (long) ((CTYPE(U32) *) bytes)[0];
		}	break;
		case QB_TYPE_S64: {
			Z_TYPE_P(zvalue) = IS_LONG;
			Z_LVAL_P(zvalue) = (long) ((CTYPE(S64) *) bytes)[0];
		}	break;
		case QB_TYPE_U64: {
			Z_TYPE_P(zvalue) = IS_LONG;
			Z_LVAL_P(zvalue) = (long) ((CTYPE(U64) *) bytes)[0];
		}	break;
		case QB_TYPE_F32: {
			Z_TYPE_P(zvalue) = IS_DOUBLE;
			Z_DVAL_P(zvalue) = ((CTYPE(F32) *) bytes)[0];
		}	break;
		case QB_TYPE_F64: {
			Z_TYPE_P(zvalue) = IS_DOUBLE;
			Z_DVAL_P(zvalue) = ((CTYPE(F64) *) bytes)[0];
		}	break;
	}
}

static void qb_transfer_value_to_debug_zval(qb_interpreter_context *cxt, qb_address *address, zval *zvalue) {
	USE_TSRM
	int32_t store_as_string;
	qb_storage *storage = cxt->function->local_storage;
	if(SCALAR(address)) {
		if(address->flags & QB_ADDRESS_STRING) {
			store_as_string = TRUE;
		} else {
			switch(address->type) {
#if SIZEOF_LONG == 8
				case QB_TYPE_U64: {
					uint64_t value = VALUE_IN(storage, U32, address);
					store_as_string = (value > INT64_MAX);
				}	break;
#else
				case QB_TYPE_U32: {
					uint32_t value = VALUE_IN(storage, U32, address);
					store_as_string = (value > INT32_MAX);
				}	break;
				case QB_TYPE_S64: {
					int64_t value = VALUE_IN(storage, S64, address);
					store_as_string = (value > INT32_MAX) || (value < INT32_MIN);
				}	break;
				case QB_TYPE_U64: {
					uint64_t value = VALUE_IN(storage, U64, address);
					store_as_string = (value > INT32_MAX);
				}	break;
#endif
				default: {
					store_as_string = FALSE;
				}	break;
			}
		}

		if(store_as_string) {
			if(address->flags & QB_ADDRESS_STRING) {
				zval_dtor(zvalue);
				ZVAL_STRINGL(zvalue, (char *) ARRAY_IN(storage, I08, address), 1, TRUE);
			} else {
				uint32_t length;
				if(Z_TYPE_P(zvalue) != IS_STRING) {
					// create a string with a buffer large enough for any number
					zval_dtor(zvalue);
					Z_TYPE_P(zvalue) = IS_STRING;
					Z_STRVAL_P(zvalue) = emalloc(MAX_LENGTH_OF_DOUBLE + 1);
				}
				length = qb_element_to_string(Z_STRVAL_P(zvalue), MAX_LENGTH_OF_DOUBLE, ARRAY_IN(storage, I08, address), address->type);
				Z_STRLEN_P(zvalue) = length;
			}
		} else {
			zval_dtor(zvalue);
			qb_element_to_zval(zvalue, ARRAY_IN(storage, I08, address), address->type);
		}
	} else {
		if(address->flags & QB_ADDRESS_STRING) {
			store_as_string = TRUE;
		} else if(address->flags & QB_ADDRESS_IMAGE) {
			store_as_string = TRUE;
		} else {
			uint32_t element_count = ARRAY_SIZE_IN(storage, address);
			store_as_string = (element_count > 1024);
		}

		if(store_as_string) {
			if(address->flags & QB_ADDRESS_STRING) {
				zval_dtor(zvalue);
				ZVAL_STRINGL(zvalue, (char *) ARRAY_IN(storage, I08, address), ARRAY_SIZE_IN(storage, address), TRUE);
			} else if(address->flags & QB_ADDRESS_IMAGE) {
				if(Z_TYPE_P(zvalue) != IS_STRING) {
					zval_dtor(zvalue);
					ZVAL_STRING(zvalue, "image", TRUE);
				}
			} else {
				if(Z_TYPE_P(zvalue) != IS_STRING || VARIABLE_LENGTH(address)) {
					char buffer[64];
					uint32_t dimension = VALUE_IN(storage, U32, address->dimension_addresses[0]);
					uint32_t len = snprintf(buffer, sizeof(buffer), "(%d elements)", dimension);
					zval_dtor(zvalue);
					ZVAL_STRINGL(zvalue, buffer, len, TRUE);
				}
			}
		} else {
			uint32_t item_element_count, item_byte_count, i;
			uint32_t dimension = VALUE_IN(storage, U32, address->dimension_addresses[0]);
			qb_address _item_address, *item_address = &_item_address;

			item_address->flags = address->flags;
			item_address->type = address->type;
			item_address->segment_selector = address->segment_selector;
			item_address->segment_offset = address->segment_offset;
			item_address->dimension_count = address->dimension_count - 1;
			item_address->array_index_address = NULL;
			if(item_address->dimension_count > 0) {
				item_address->mode = QB_ADDRESS_MODE_ARR;
				item_address->dimension_addresses = address->dimension_addresses + 1;
				item_address->array_size_addresses = address->array_size_addresses + 1;
				item_address->array_size_address = item_address->array_size_addresses[0];
				if(address->index_alias_schemes) {
					item_address->index_alias_schemes = address->index_alias_schemes + 1;
				} else {
					item_address->index_alias_schemes = NULL;
				}
				item_element_count = VALUE_IN(storage, U32, item_address->array_size_address);
			} else {
				item_address->mode = QB_ADDRESS_MODE_SCA;
				item_address->dimension_addresses = NULL;
				item_address->array_size_addresses = NULL;
				item_address->array_size_address = NULL;
				item_address->array_index_address = NULL;
				item_address->index_alias_schemes = NULL;
				item_element_count = 1;
			}
			item_byte_count = BYTE_COUNT(item_element_count, address->type);

			if(Z_TYPE_P(zvalue) != IS_ARRAY) {
				zval_dtor(zvalue);
				array_init_size(zvalue, dimension);
			}
			for(i = 0; i < dimension; i++) {
				zval *item, **p_item;
				if(zend_hash_index_find(Z_ARRVAL_P(zvalue), i, (void **) &p_item) == SUCCESS) {
					item = *p_item;
				} else {
					ALLOC_INIT_ZVAL(item);
					zend_hash_index_update(Z_ARRVAL_P(zvalue), i, (void **) &item, sizeof(zval *), NULL);
				}
				qb_transfer_value_to_debug_zval(cxt, item_address, item);
				item_address->segment_offset += item_byte_count;
			}
		}
	}
}

void qb_create_shadow_variables(qb_interpreter_context *cxt) {
	USE_TSRM
	uint32_t i, j;
	zend_execute_data *ex = EG(current_execute_data);

	if(!EG(active_symbol_table)) {
		zend_rebuild_symbol_table(TSRMLS_C);
	}

	cxt->shadow_variables = ecalloc(cxt->function->variable_count, sizeof(zval *));
	for(i = 0, j = 0; i < cxt->function->variable_count; i++) {
		qb_variable *qvar = cxt->function->variables[i];
		if(!(qvar->flags & (QB_VARIABLE_CLASS | QB_VARIABLE_CLASS_INSTANCE | QB_VARIABLE_RETURN_VALUE))) {
			zval **var, *value;
			ALLOC_INIT_ZVAL(value);
			qb_transfer_value_to_debug_zval(cxt, qvar->address, value);
			zend_hash_quick_update(ex->symbol_table, qvar->name, qvar->name_length + 1, qvar->hash_value, &value, sizeof(zval *), (void **) &var);
#if !ZEND_ENGINE_2_4 && !ZEND_ENGINE_2_3 && !ZEND_ENGINE_2_2 && !ZEND_ENGINE_2_1
			*EX_CV_NUM(ex, j) = var;
#else
			ex->CVs[j] = var;
#endif
			j++;

			if(qvar->flags & QB_VARIABLE_ARGUMENT) {
				// push argument onto Zend stack
				Z_ADDREF_P(value);
#if !ZEND_ENGINE_2_2 && !ZEND_ENGINE_2_1
				zend_vm_stack_push(value TSRMLS_CC);
#else
				zend_ptr_stack_push(&EG(argument_stack), value);
#endif
			}
			cxt->shadow_variables[i] = value;
		}
	}
	// push the argument count
#if !ZEND_ENGINE_2_4 && !ZEND_ENGINE_2_3 && !ZEND_ENGINE_2_2 && !ZEND_ENGINE_2_1
	ex->function_state.arguments = zend_vm_stack_top(TSRMLS_C);
	zend_vm_stack_push((void *) (zend_uintptr_t) cxt->function->argument_count TSRMLS_CC);
#elif !ZEND_ENGINE_2_2 && !ZEND_ENGINE_2_1
	ex->function_state.arguments = zend_vm_stack_push_args(cxt->function->argument_count TSRMLS_CC);
#else
	zend_ptr_stack_2_push(&EG(argument_stack), (void *) (zend_uintptr_t) cxt->function->argument_count, NULL);
#endif
}

void qb_sync_shadow_variable(qb_interpreter_context *cxt, uint32_t index) {
	if(debug_compatibility_mode) {
		USE_TSRM
		qb_variable *qvar = cxt->function->variables[index];
		if(cxt->shadow_variables) {
			if(!(qvar->flags & (QB_VARIABLE_CLASS | QB_VARIABLE_CLASS_INSTANCE | QB_VARIABLE_RETURN_VALUE))) {
				zval *shadow_var = cxt->shadow_variables[index];
				qb_transfer_value_to_debug_zval(cxt, qvar->address, shadow_var);
			}
			if(qvar->flags & (QB_VARIABLE_CLASS_INSTANCE | QB_VARIABLE_CLASS | QB_VARIABLE_GLOBAL)) {
				qb_sync_imported_variable(cxt, qvar);
			}
		}
	}
}

void qb_destroy_shadow_variables(qb_interpreter_context *cxt) {
	// Zend will actually perform the clean-up of the CVs
	// the only thing we need to do here is to put arguments that were passed by reference
	// back into the symbol table and pop them off the stack
	USE_TSRM
	uint32_t i, arg_count;
	zend_execute_data *ex = EG(current_execute_data);

	// pop the argument count
#if !ZEND_ENGINE_2_2 && !ZEND_ENGINE_2_1
	arg_count = (uint32_t) (zend_uintptr_t) zend_vm_stack_pop(TSRMLS_C);
#else
	zend_ptr_stack_pop(&EG(argument_stack));
	arg_count = (uint32_t) (zend_uintptr_t) zend_ptr_stack_pop(&EG(argument_stack));
#endif

	for(i = 0; i < arg_count; i++) {
		qb_variable *qvar = cxt->function->variables[i];

		// pop argument off Zend stack
#if !ZEND_ENGINE_2_2 && !ZEND_ENGINE_2_1
		zval *argument = zend_vm_stack_pop(TSRMLS_C);
#else
		zval *argument = zend_ptr_stack_pop(&EG(argument_stack));
#endif
		if(qvar->flags & QB_VARIABLE_BY_REF) {
			zval **var;
			Z_ADDREF_P(argument);
			zend_hash_quick_update(ex->symbol_table, qvar->name, qvar->name_length + 1, qvar->hash_value, &argument, sizeof(zval *), (void **) &var);

			// updating the CVs for consistency sake
#if !ZEND_ENGINE_2_4 && !ZEND_ENGINE_2_3 && !ZEND_ENGINE_2_2 && !ZEND_ENGINE_2_1
			*EX_CV_NUM(ex, i) = var;
#else
			ex->CVs[i] = var;
#endif
		}
		zval_ptr_dtor(&argument);
	}
	efree(cxt->shadow_variables);
}

#include "zend_extensions.h"
#include "zend_vm.h"

void qb_run_zend_extension_op(qb_interpreter_context *cxt, uint32_t zend_opcode, uint32_t line_id) {
	USE_TSRM
	if(!EG(no_extensions)) {
		USE_TSRM
		zend_llist_element *element;
		zend_op_array *op_array = EG(current_execute_data)->op_array;
		zend_op *opline = EG(current_execute_data)->opline;
		opline->lineno = LINE_NUMBER(line_id);

		if(cxt->function->flags & QB_FUNCTION_NEED_SHADOWS) {
			if(debug_compatibility_mode) {
				if(!cxt->shadow_variables) {
					qb_create_shadow_variables(cxt);
				}
			}
		}

		for(element = zend_extensions.head; element; element = element->next) {
			zend_extension *extension = (zend_extension *) element->data;
			switch(zend_opcode) {
				case ZEND_EXT_FCALL_BEGIN: {
					if(extension->fcall_begin_handler != NULL) {
						extension->fcall_begin_handler(op_array);
					}
				}	break;
				case ZEND_EXT_FCALL_END: {
					if(extension->fcall_end_handler != NULL) {
						extension->fcall_end_handler(op_array);
					}
				}	break;
				case ZEND_EXT_STMT: {
					if(extension->statement_handler != NULL) {
						extension->statement_handler(op_array);
					}
				}	break;
			}
		}
	}
}

#ifdef ZEND_DEBUG
void qb_test_debug_interface(qb_interpreter_context *cxt) {
	static qb_debug_interface *qb = NULL;
	USE_TSRM
	if(!qb) {
		zend_extension *qb_ext = zend_get_extension("QB");
		if(qb_ext) {
			qb = QB_GET_DEBUG_INTERFACE(qb_ext);
		}
	}

	if(qb) {
		uint32_t i;
		for(i = 0; i < cxt->function->variable_count; i++) {
			qb_variable *qvar = cxt->function->variables[i];
			qb_debug_variable_details *details = NULL;
			if(qvar->flags & QB_VARIABLE_CLASS_INSTANCE) {
				qb->get_instance_variable_details(EG(This), qvar->name, &details);
			} else if(qvar->flags & QB_VARIABLE_CLASS) {
				qb->get_class_variable_details(EG(called_scope), qvar->name, &details);
			} else if(qvar->flags & QB_VARIABLE_GLOBAL) {
				qb->get_global_variable_details(qvar->name, &details);
			} else if(qvar->flags & QB_VARIABLE_CLASS_CONSTANT) {
				continue;
			} else if(!qvar->name) {
				continue;
			} else {
				void *f;
				if(qb->get_function_pointer(EG(active_op_array), &f)) {
					qb->get_local_variable_details(f, qvar->name, &details);
				}
			}
			printf("%s:\n", qvar->name);
			if(details) {
				if(details->flags & QB_VAR_IS_STRING) {
					char *s = (char *) details->data;
					int len = details->byte_count;
					printf("%.*s\n\n", len, s);
				} else if(details->flags & QB_VAR_IS_IMAGE) {
					qb_debug_bitmap *bitmap;
					if(qb->get_bitmap(details, &bitmap)) {
						printf("[%dx%d bitmap]\n\n", bitmap->width, bitmap->height);
						qb->free_bitmap(bitmap);
					} else {
						printf("failed\n\n");
					}
				} else {
					uint32_t count = details->byte_count / (details->bitness / 8), i;
					if(details->flags & QB_VAR_IS_FLOAT) {
						switch(details->bitness) {
							case 32: {
								float32_t *f = (float32_t *) details->data;
								for(i = 0; i < count; i++) {
									printf("%g ", f[i]);
								}
							}	break;
							case 64: {
								float64_t *f = (float64_t *) details->data;
								for(i = 0; i < count; i++) {
									printf("%g ", f[i]);
								}
							}	break;
						}
					} else if(details->flags & QB_VAR_IS_UNSIGNED) {
						switch(details->bitness) {
							case 8: {
								uint8_t *u = (uint8_t *) details->data;
								for(i = 0; i < count; i++) {
									printf("%" PRIu8 " ", u[i]);
								}
							}	break;
							case 16: {
								uint16_t *u = (uint16_t *) details->data;
								for(i = 0; i < count; i++) {
									printf("%" PRIu16 " ", u[i]);
								}
							}	break;
							case 32: {
								uint32_t *u = (uint32_t *) details->data;
								for(i = 0; i < count; i++) {
									printf("%" PRIu32 " ", u[i]);
								}
							}	break;
							case 64: {
								uint64_t *u = (uint64_t *) details->data;
								for(i = 0; i < count; i++) {
									printf("%" PRIu64 " ", u[i]);
								}
							}	break;
						}
					} else {
						switch(details->bitness) {
							case 8: {
								int8_t *d = (int8_t *) details->data;
								for(i = 0; i < count; i++) {
									printf("%" PRId8 " ", d[i]);
								}
							}	break;
							case 16: {
								int16_t *d = (int16_t *) details->data;
								for(i = 0; i < count; i++) {
									printf("%" PRId16 " ", d[i]);
								}
							}	break;
							case 32: {
								int32_t *d = (int32_t *) details->data;
								for(i = 0; i < count; i++) {
									printf("%" PRId32 " ", d[i]);
								}
							}	break;
							case 64: {
								int64_t *d = (int64_t *) details->data;
								for(i = 0; i < count; i++) {
									printf("%" PRId64 " ", d[i]);
								}
							}	break;
						}
					}
					printf("\n\n");
				}
				qb->free_variable_details(details);
			} else {
				printf("failed\n\n");
			}
		}
	}
}
#endif
