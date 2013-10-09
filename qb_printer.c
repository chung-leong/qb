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

extern const char compressed_table_zend_op_names[];

const char * qb_get_zend_op_name(qb_printer_context *cxt, uint32_t opcode) {
	if(!cxt->pool->zend_op_names) {
		qb_uncompress_table(compressed_table_zend_op_names, (void ***) &cxt->pool->zend_op_names, NULL, 0);
		if(!cxt->pool->zend_op_names) {
			return "?";
		}
	}
	return cxt->pool->zend_op_names[opcode];
}

static void qb_print_value(qb_printer_context *cxt, int8_t *bytes, uint32_t type) {
	USE_TSRM
	char buffer[64];
	uint32_t len;
	len = qb_element_to_string(buffer, sizeof(buffer), bytes, type);
	php_write(buffer, len TSRMLS_CC);
}

static qb_variable * qb_find_variable_with_address(qb_printer_context *cxt, qb_address *address) {
	uint32_t i;
	for(i = 0; i < cxt->compiler_context->variable_count; i++) {
		qb_variable *qvar = cxt->compiler_context->variables[i];
		if(qvar->address == address) {
			return qvar;
		}
	}
	return NULL;
}

static qb_variable * qb_find_variable_with_size_address(qb_printer_context *cxt, qb_address *address, uint32_t *p_depth, int32_t *p_recursive) {
	uint32_t i, j;
	for(i = 0; i < cxt->compiler_context->variable_count; i++) {
		qb_variable *qvar = cxt->compiler_context->variables[i];
		if(qvar->address) {
			for(j = 0; j < qvar->address->dimension_count; j++) {
				if(qvar->address->dimension_addresses[j] == address) {
					*p_depth = j + 1;
					*p_recursive = FALSE;
					return qvar;
				} else if(qvar->address->array_size_addresses[j] == address) {
					*p_depth = j + 1;
					*p_recursive = TRUE;
					return qvar;
				}
			}
		}
	}
	return NULL;
}

static void qb_print_address(qb_printer_context *cxt, qb_address *address, int32_t ignore_index) {
	uint32_t i;
	if(CONSTANT(address)) {
		if(!SCALAR(address)) {
			if(address->flags & QB_ADDRESS_STRING) {
				uint32_t len = VALUE(U32, address->array_size_address);
				char *string = (char *) ARRAY(U08, address);
				php_printf("\"%.*s\"", len, string);
			} else {
				uint32_t count = ARRAY_SIZE(address);
				int8_t *bytes = ARRAY(I08, address);
				php_printf("[");
				for(i = 0; i < count; i++) {
					qb_print_value(cxt, bytes, address->type);
					if(i < count - 1) {
						php_printf(", ");
					}
					if(i == 3 && count > 5) {
						php_printf("...(%d more)", count - 4);
						break;
					}
					bytes += BYTE_COUNT(1, address->type);
				}
				php_printf("]");
			}
		} else {
			if(address->flags & QB_ADDRESS_STRING) {
				char *string = (char *) ARRAY(U08, address);
				php_printf("\"%.*s\"", 1, string);
			} else{
				qb_print_value(cxt, ARRAY(I08, address), address->type);
			}
		}
	} else if(address->source_address) {
		if(address->source_address->dimension_count == address->dimension_count + 1) {
			// array element
			qb_print_address(cxt, address->source_address, TRUE);
			if(!ignore_index) {
				php_printf("[");
				qb_print_address(cxt, address->array_index_address, FALSE);
				php_printf("]");
			}
		} else {
			qb_print_address(cxt, address->source_address, FALSE); 
		}
	} else {
		qb_variable *qvar;
		uint32_t depth;
		int32_t recursive;
		if((qvar = qb_find_variable_with_address(cxt, address))) {
			if(qvar->name) {
				if(qvar->flags & QB_VARIABLE_CLASS) {
					if(qvar->zend_class) {
						php_printf("self::$%s", qvar->name);
					} else {
						php_printf("static::$%s", qvar->name);
					}
				} else if(qvar->flags & QB_VARIABLE_CLASS_INSTANCE) {
					php_printf("$this->%s", qvar->name);
				} else if(qvar->flags & QB_VARIABLE_CLASS_CONSTANT) {
						php_printf("static::%s", qvar->name);
				} else {
					php_printf("$%s", qvar->name);
				}
			} else {
				php_printf("(retval)");
			}
		} else if((qvar = qb_find_variable_with_size_address(cxt, address, &depth, &recursive))) {
			php_printf("count($%s", qvar->name);
			while(--depth > 0) {
				php_printf("[]");
			}
			php_printf((recursive) ? ", true)" : ")");
		} else {
			if(SCALAR(address)) {
				if(address->flags & QB_ADDRESS_FOREACH_INDEX) {
					php_printf("(fe:%u:%u)", address->segment_selector, address->segment_offset);
				} else {
					php_printf("(%u:%u)", address->segment_selector, address->segment_offset);
				}
			} else {
				if(VARIABLE_LENGTH(address)) {
					php_printf("(%u:%u...)", address->segment_selector, address->segment_offset);
				} else {
					php_printf("(%u:%u..%u:%u)", address->segment_selector, address->segment_offset, address->segment_selector, address->segment_offset + (ARRAY_SIZE(address) - 1));
				}
			}
		}
	}
}

extern const char compressed_table_op_names[];

static const char * qb_get_op_name(qb_printer_context *cxt, uint32_t opcode) {
	if(!cxt->pool->op_names) {
		// decompress the opname table
		qb_uncompress_table(compressed_table_op_names, (void ***) &cxt->pool->op_names, NULL, 0);
		if(!cxt->pool->op_names) {
			return "?";
		}
	}
	return cxt->pool->op_names[opcode];
}

static void qb_print_op(qb_printer_context *cxt, qb_op *qop, uint32_t index) {
	uint32_t i;
	const char *name = qb_get_op_name(cxt, qop->opcode);
	php_printf("%04d: %s ", index, name);
	for(i = 0; i < qop->operand_count; i++) {
		qb_operand *operand = &qop->operands[i];
		switch(operand->type) {
			case QB_OPERAND_ADDRESS: {
				qb_print_address(cxt, operand->address, FALSE);
			}	break;
			case QB_OPERAND_EXTERNAL_SYMBOL: {
				qb_external_symbol *symbol = &cxt->compiler_context->external_symbols[operand->symbol_index];
				php_printf("[%s]", symbol->name);
			}	break;
			case QB_OPERAND_SEGMENT_SELECTOR: {
				php_printf("@%d", operand->address->segment_selector);
			}	break;
			case QB_OPERAND_NUMBER: {
				php_printf("'%d'", operand->number);
			}	break;
			default: {
				php_printf("(ERROR)");
				break;
			}
		}
		php_printf(" ");
	}
	for(i = 0; i < qop->jump_target_count; i++) {
		uint32_t jump_target_index = qop->jump_target_indices[i];
		php_printf("<%04d> ", jump_target_index);
	}
	php_printf("\n");
}

void qb_print_ops(qb_printer_context *cxt) {
	uint32_t i;
	for(i = 0; i < cxt->compiler_context->op_count; i++) {
		qb_op *qop = cxt->compiler_context->ops[i];
		if(qop->opcode != QB_NOP) {
			qb_print_op(cxt, qop, i);
		}
	}
}

void qb_print_zend_ops(qb_printer_context *cxt) {
	uint32_t i = 0;
	for(i = 0; i < cxt->compiler_context->zend_op_array->last; i++) {
		zend_op *zop = &cxt->compiler_context->zend_op_array->opcodes[i];
		if(zop->opcode != qb_user_opcode) {
			const char *opname = qb_get_zend_op_name(cxt, zop->opcode);
			php_printf("[%04d] %s (line number: %d)\n", i, opname, zop->lineno);
		}
	}
}

void qb_initialize_printer_context(qb_printer_context *cxt, qb_compiler_context *compiler_cxt TSRMLS_DC) {
	cxt->compiler_context = compiler_cxt;
	cxt->pool = compiler_cxt->pool;
	cxt->storage = compiler_cxt->storage;

	SAVE_TSRMLS
}