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

extern const qb_op_info global_op_info[];
extern const uint32_t global_instruction_lengths[];

static uint32_t qb_get_instruction_length(qb_encoder_context *cxt, uint32_t opcode) {
	const qb_op_info *op = &global_op_info[opcode];
	uint32_t length = op->instruction_length;
	return length;
}

static uint32_t qb_get_op_encoded_length(qb_encoder_context *cxt, qb_op *qop) {
	uint32_t length = qb_get_instruction_length(cxt, qop->opcode);
	if(!length) {
		// variable-length instruction
	}
	return length;
}

static void qb_set_instruction_offsets(qb_encoder_context *cxt) {
	uint32_t instruction_offset, i;
	uint32_t count = 0;

	// determine the offsets of each instruction in the stream
	instruction_offset = sizeof(void *);
	for(i = 0; i < cxt->op_count; i++) {
		qb_op *qop = cxt->ops[i];
		qop->instruction_offset = instruction_offset;
		if(qop->opcode != QB_NOP) {
			uint32_t instruction_length = qb_get_op_encoded_length(cxt, qop);
			instruction_offset += instruction_length;
			count++;
		}
	}

	// the final offset also happens to be the total length
	cxt->instruction_stream_length = instruction_offset;
	cxt->instruction_op_count = count;
}

static void qb_add_segment_reference(qb_encoder_context *cxt, qb_address *address, void **p_pointer) {
	qb_memory_segment *segment = &cxt->storage->segments[address->segment_selector];
	if(!(segment->flags & QB_SEGMENT_PREALLOCATED)) {
		uintptr_t **p_reference = &segment->references[segment->reference_count++];
		*p_reference = (uintptr_t *) p_pointer;
	}
}

static void * qb_get_pointer(qb_encoder_context *cxt, qb_address *address) {
	qb_memory_segment *segment = &cxt->storage->segments[address->segment_selector];
	return (void *) (segment->memory + address->segment_offset);
}

static void qb_encode_address(qb_encoder_context *cxt, qb_address *address, int8_t **p_ip) {
	switch(address->mode) {
		case QB_ADDRESS_MODE_SCA: {
			qb_pointer_SCA *p = ((qb_pointer_SCA *) *p_ip);
			p->data_pointer = qb_get_pointer(cxt, address);
			*p_ip += sizeof(qb_pointer_SCA);

			qb_add_segment_reference(cxt, address, &p->data_pointer);
		}	break;
		case QB_ADDRESS_MODE_ELE: {
			qb_pointer_ELE *p = ((qb_pointer_ELE *) *p_ip);
			p->data_pointer = qb_get_pointer(cxt, address);
			p->index_pointer = qb_get_pointer(cxt, address->array_index_address);
			*p_ip += sizeof(qb_pointer_ELE);

			qb_add_segment_reference(cxt, address, &p->data_pointer);
			qb_add_segment_reference(cxt, address->array_index_address, &p->index_pointer);
		}	break;
		case QB_ADDRESS_MODE_ARR: {
			qb_pointer_ARR *p = ((qb_pointer_ARR *) *p_ip);
			p->data_pointer = qb_get_pointer(cxt, address);
			p->index_pointer = qb_get_pointer(cxt, address->array_index_address);
			p->count_pointer = qb_get_pointer(cxt, address->array_size_address);
			*p_ip += sizeof(qb_pointer_ARR);

			qb_add_segment_reference(cxt, address, &p->data_pointer);
			qb_add_segment_reference(cxt, address->array_index_address, &p->index_pointer);
			qb_add_segment_reference(cxt, address->array_size_address, &p->count_pointer);
		}	break;
		default:
			qb_abort("invalid address type");
	}
}

static zend_always_inline void *qb_get_handler(qb_encoder_context *cxt, qb_op *qop) {
	void *handler_address;
#ifdef ZEND_DEBUG
	if(qop->opcode >= QB_OPCODE_COUNT) {
		qb_abort("illegal opcode");
	}
#endif
#ifdef __GNUC__
		handler_address = op_handlers[qop->opcode];
#else
		handler_address = (void *) ((uintptr_t) qop->opcode);
#endif
	return handler_address;
}

static void qb_encode_handler(qb_encoder_context *cxt, uint32_t target_index, int8_t **p_ip) {
	qb_op *target_qop = cxt->ops[target_index];

#ifdef ZEND_DEBUG
	if(target_index >= cxt->op_count) {
		qb_abort("invalid op index");
	}
#endif

	while(target_qop->opcode == QB_NOP) {
		target_qop = cxt->ops[++target_index];
	}

	*((void **) *p_ip) = qb_get_handler(cxt, target_qop); 
	*p_ip += sizeof(void *);
}

static zend_always_inline int8_t *qb_get_instruction_pointer(qb_encoder_context *cxt, qb_op *qop) {
	int8_t *p = cxt->instructions + qop->instruction_offset;
	return p;
}

static void qb_encode_jump_target(qb_encoder_context *cxt, uint32_t target_index, int8_t **p_ip) {
	qb_op *target_qop = cxt->ops[target_index];

#ifdef ZEND_DEBUG
	if(target_index >= cxt->op_count) {
		qb_abort("invalid jump target");
	}
#endif
	while(target_qop->opcode == QB_NOP) {
		target_qop = cxt->ops[++target_index];
	}

	*((void **) *p_ip) = qb_get_handler(cxt, target_qop);
	*p_ip += sizeof(void *);

	*((int8_t **) *p_ip) = qb_get_instruction_pointer(cxt, target_qop); 
	*p_ip += sizeof(int8_t *);
}

static void qb_encode_line_number(qb_encoder_context *cxt, uint32_t line_number, int8_t **p_ip) {
	*((uint32_t *) *p_ip) = line_number; 
	*p_ip += sizeof(uint32_t);
}

static void qb_encode_segment_selector(qb_encoder_context *cxt, qb_address *address, int8_t **p_ip) {
	*((uint32_t *) *p_ip) = address->segment_selector;
	*p_ip += sizeof(uint32_t);
}

static void qb_encode_number(qb_encoder_context *cxt, int32_t number, int8_t **p_ip) {
	*((int32_t *) *p_ip) = number;
	*p_ip += sizeof(uint32_t);
}

int8_t * qb_encode_instruction_stream(qb_encoder_context *cxt, int8_t *memory) {
	uint32_t i, j;
	int8_t *ip = memory;

	//  encode the instruction stream in the following manner:
	//  [op1 handler][op2 handler][op1 operands][op3 handler][op2 operands][op3 operands]...
	//  each instruction thus contains the handler of the *next* instruction
	for(i = 0; i < cxt->op_count; i++) {
		qb_op *qop = cxt->ops[i];

		if(qop->opcode != QB_NOP) {
			if(ip == cxt->instructions) {
				// set the next handler for the "zeroth" instruction
				qb_encode_handler(cxt, i, &ip);
			}

			if((int8_t *) memory + qop->instruction_offset != ip) {
				qb_op *prev_qop = cxt->ops[i - 1];
				do {
					prev_qop = cxt->ops[--i];
				} while(prev_qop->opcode == QB_NOP);
				qb_abort("the previous op was not correctly encoded");
			}

			if(qop->flags & QB_OP_JUMP) {
				// put in the jump targets
				for(j = 0; j < qop->jump_target_count; j++) {
					qb_encode_jump_target(cxt, qop->jump_target_indices[j], &ip);
				}
			} else {
				// need the handler to the next instruction if it's not a jump operation
				// as functions must always end with a return
				// there's no check here if there is an op at i + 1
				qb_encode_handler(cxt, i + 1, &ip);
			}

			for(j = 0; j < qop->operand_count; j++) {
				qb_operand *operand = &qop->operands[j];

				switch(operand->type) {
					case QB_OPERAND_ADDRESS: {
						qb_encode_address(cxt, operand->address, &ip);
					}	break;
					case QB_OPERAND_SEGMENT_SELECTOR: {
						qb_encode_segment_selector(cxt, operand->address, &ip);
					}	break;
					case QB_OPERAND_NUMBER: {
						qb_encode_number(cxt, operand->number, &ip);
					}	break;
					default: {
						qb_abort("unknown operand type: %d", operand->type);
					}	break;
				}
			}

			// put the line number at the end if it's needed
			if(qop->flags & QB_OP_NEED_LINE_NUMBER) {
				qb_encode_line_number(cxt, qop->line_number, &ip);
			}
		}
	}
#if ZEND_DEBUG
	if(memory + cxt->instruction_stream_length != ip) {
		qb_abort("length mismatch");
	}
#endif
	return ip;
}

int8_t * qb_copy_instruction_opcodes(qb_encoder_context *cxt, int8_t *memory) {
	int16_t *cp = (int16_t *) memory;
	uint32_t i;
	for(i = 0; i < cxt->op_count; i++) {
		qb_op *qop = cxt->ops[i];
		if(qop->opcode != QB_NOP) {
			*cp = qop->opcode;
			cp++;
		}
	}
	return (int8_t *) cp;
}

static uint32_t qb_get_address_length(qb_address *address) {
	uint32_t i, j;
	uint32_t length = sizeof(qb_address);	// the address itself
	if(!SCALAR(address)) {
		if(address->dimension_count > 1) {
			length += sizeof(qb_address *) * address->dimension_count * 2;
			length += sizeof(qb_address) * (address->dimension_count * 2 - 1);
		} else {
			length += sizeof(qb_address);
		}
		if(address->index_alias_schemes) {
			// need a pointer per dimension
			length += sizeof(qb_index_alias_scheme *) * address->dimension_count;	
			for(i = 0; i < address->dimension_count; i++) {
				qb_index_alias_scheme *scheme = address->index_alias_schemes[i];
				if(scheme) {
					length += sizeof(qb_index_alias_scheme);
					// a string pointer and an uint32 per alias 
					length += (sizeof(const char *) + sizeof(uint32_t)) * scheme->dimension;
					for(j = 0; j < scheme->dimension; j++) {
						length += scheme->alias_lengths[j] + 1;
					}
					if(scheme->class_name) {
						length += scheme->class_name_length + 1;
					}
				}
			}
		}
	}
	return length;
}

static uint8_t * qb_copy_address(qb_address *address, int8_t *memory) {
#if ZEND_DEBUG
	uint32_t length = qb_get_address_length(address);
#endif
	int8_t *p = memory;
	qb_address *src = address;
	qb_address *dst = (qb_address *) p; p += sizeof(qb_address);
	dst->type = src->type;
	dst->flags = src->flags;
	dst->segment_selector = src->segment_selector;
	dst->segment_offset = src->segment_offset;
	dst->dimension_count = src->dimension_count;
	dst->array_index_address = NULL;
	dst->source_address = NULL;
	dst->expression = NULL;

	if(SCALAR(src)) {
		dst->mode = QB_ADDRESS_MODE_SCA;
		dst->array_size_address = NULL;
		dst->array_size_addresses = NULL;
		dst->dimension_addresses = NULL;
		dst->index_alias_schemes = NULL;		
	} else {
		uint32_t i, j;
		dst->mode = QB_ADDRESS_MODE_ARR;
		if(src->dimension_count > 1) {
			dst->dimension_addresses = (qb_address **) p; p += sizeof(qb_address *) * src->dimension_count;
			dst->array_size_addresses = (qb_address **) p; p += sizeof(qb_address *) * src->dimension_count;
			for(i = 0; i < src->dimension_count; i++) {
				dst->dimension_addresses[i] = (qb_address *) p; p += sizeof(qb_address);
				*dst->dimension_addresses[i] = *src->dimension_addresses[i];
				if(i == src->dimension_count - 1) {
					dst->array_size_addresses[i] = dst->dimension_addresses[i];
				} else {
					dst->array_size_addresses[i] = (qb_address *) p; p += sizeof(qb_address);
					*dst->array_size_addresses[i] = *src->array_size_addresses[i];
				}
			}
			dst->array_size_address = dst->array_size_addresses[0];
		} else {
			dst->dimension_addresses = &dst->array_size_address;
			dst->array_size_addresses = &dst->array_size_address;
			dst->array_size_address = (qb_address *) p; p += sizeof(qb_address);
			*dst->array_size_address = *src->array_size_address;
		}
		if(src->index_alias_schemes) {
			dst->index_alias_schemes = (qb_index_alias_scheme **) p; p += sizeof(qb_index_alias_scheme *) * src->dimension_count;
			for(i = 0; i < src->dimension_count; i++) {
				qb_index_alias_scheme *dst_scheme, *src_scheme = src->index_alias_schemes[i];
				if(src_scheme) {
					dst_scheme = (qb_index_alias_scheme *) p; p += sizeof(qb_index_alias_scheme);
					dst_scheme->dimension = src_scheme->dimension;
					dst_scheme->zend_class = src_scheme->zend_class;
					dst_scheme->aliases = (char **) p; p += sizeof(char *) * src_scheme->dimension;
					dst_scheme->alias_lengths = (uint32_t *) p; p += sizeof(uint32_t) * src_scheme->dimension;
					for(j = 0; j < src_scheme->dimension; j++) {
						dst_scheme->aliases[j] = (char *) p; p += src_scheme->alias_lengths[j] + 1;
						memcpy(dst_scheme->aliases[j], src_scheme->aliases[j], src_scheme->alias_lengths[j] + 1);
						dst_scheme->alias_lengths[j] = src_scheme->alias_lengths[j];
					}
					if(src_scheme->class_name) {
						dst_scheme->class_name = (char *) p; p+= src_scheme->class_name_length + 1;
						memcpy((char *) dst_scheme->class_name, src_scheme->class_name, src_scheme->class_name_length + 1);
						dst_scheme->class_name_length = src_scheme->class_name_length;
					} else {
						dst_scheme->class_name = NULL;
						dst_scheme->class_name_length = 0;
					}
					dst->index_alias_schemes[i] = dst_scheme;
				}
			}
		} else {
			dst->index_alias_schemes = NULL;
		}
	}
#if ZEND_DEBUG
	if(memory + length != p) {
		qb_abort("length mismatch");
	}
#endif
	return p;
}

uint32_t qb_get_variable_length(qb_variable *qvar) {
	uint32_t length = sizeof(qb_variable);
	if(qvar->name) {
		length += qvar->name_length + 1;
	}
	if(qvar->address) {
		length += qb_get_address_length(qvar->address);
	}
	return length;
}

uint8_t * qb_copy_variable(qb_variable *qvar, int8_t *memory) {
#if ZEND_DEBUG
	uint32_t length = qb_get_variable_length(qvar);
#endif
	int8_t *p = memory;
	qb_variable *src = qvar;
	qb_variable *dst = (qb_variable *) p; p += sizeof(qb_variable);
	dst->flags = src->flags;
	dst->default_value = src->default_value;
	dst->zend_class = src->zend_class;
	if(src->name) {
		dst->name = (char *) p; p += src->name_length + 1;
		memcpy((char *) dst->name, src->name, src->name_length + 1);
		dst->hash_value = src->hash_value;
		dst->name_length = src->name_length;

		// since we're going to free the op array later, update the pointer so
		// cxt->variables continue to point to a valid string (to avoid seeing garbage in the debugger)
		src->name = dst->name;
	} else {
		dst->hash_value = 0;
		dst->name_length = 0;
		dst->name = NULL;
	}
	if(src->address) {
		dst->address = (qb_address *) p;
		p = qb_copy_address(src->address, p);
	} else {
		dst->address = NULL;
	}

#if ZEND_DEBUG
	if(memory + length != p) {
		qb_abort("length mismatch");
	}
#endif
	return p;
}

static uint32_t qb_get_external_symbol_length(qb_external_symbol *symbol) {
	uint32_t length = sizeof(qb_external_symbol);
	length += symbol->name_length + 1;
	return length;
}

static uint8_t * qb_copy_external_symbol(qb_external_symbol *symbol, uint8_t *memory) {
#if ZEND_DEBUG
	uint32_t length = qb_get_external_symbol_length(symbol);
#endif
	uint8_t *p = memory;
	qb_external_symbol *src = symbol;
	qb_external_symbol *dst = (qb_external_symbol *) p; p += sizeof(qb_external_symbol);
	dst->type = src->type;
	dst->name = (char *) p; p += src->name_length + 1;
	memcpy((char *) dst->name, src->name, src->name_length + 1);
	dst->name_length = src->name_length;
	dst->pointer = src->pointer;

	// in case the function name disappears when the op_array is freed
	src->name = dst->name;
#if ZEND_DEBUG
	if(memory + length != p) {
		qb_abort("length mismatch");
	}
#endif
	return p;
}

static uint32_t qb_get_function_structure_size(qb_encoder_context *cxt) {
	uint32_t size = sizeof(qb_function);
	uint32_t i;

	size += sizeof(qb_variable *) * cxt->compiler_context->variable_count;
	for(i = 0; i < cxt->compiler_context->variable_count; i++) {
		size += qb_get_variable_length(cxt->compiler_context->variables[i]);
	}
	size += sizeof(qb_external_symbol *) * cxt->compiler_context->external_symbol_count;
	for(i = 0; i < cxt->compiler_context->external_symbol_count; i++) {
		size += qb_get_external_symbol_length(&cxt->compiler_context->external_symbols[i]);
	}
	size = ALIGN_TO(size, 16);
	return size;
}

static int8_t * qb_copy_function_structure(qb_encoder_context *cxt, int8_t *memory) {
	int8_t *p = memory;
	qb_function *qfunc;
	char *func_name = NULL, *filename = NULL;
	uint32_t i;

	qfunc = (qb_function *) p; p += sizeof(qb_function);

	// copy variables
	qfunc->variables = (qb_variable **) p; p += sizeof(qb_variable *) * cxt->compiler_context->variable_count;
	qfunc->variable_count = cxt->compiler_context->variable_count;
	for(i = 0; i < cxt->compiler_context->variable_count; i++) {
		qfunc->variables[i] = (qb_variable *) p;
		p = qb_copy_variable(cxt->compiler_context->variables[i], p);

		if(qfunc->variables[i]->flags & QB_VARIABLE_RETURN_VALUE) {
			qfunc->return_variable = qfunc->variables[i];
		}
	}

	// copy external symbols
	qfunc->external_symbols = (qb_external_symbol **) p; p += sizeof(qb_external_symbol *) * cxt->compiler_context->external_symbol_count;
	qfunc->external_symbol_count = cxt->compiler_context->external_symbol_count;
	for(i = 0; i < cxt->compiler_context->external_symbol_count; i++) {
		qfunc->external_symbols[i] = (qb_external_symbol *) p;
		p = qb_copy_external_symbol(&cxt->compiler_context->external_symbols[i], p);
	}

	qfunc->argument_count = cxt->compiler_context->argument_count;
	qfunc->name = (cxt->compiler_context->zend_op_array) ? cxt->compiler_context->zend_op_array->function_name : NULL;
	qfunc->filename = (cxt->compiler_context->zend_op_array) ? cxt->compiler_context->zend_op_array->filename : NULL;
	qfunc->native_proc = NULL;
	qfunc->zend_op_array = cxt->compiler_context->zend_op_array;
	qfunc->next_copy = NULL;
	qfunc->flags = cxt->compiler_context->function_flags;

	p = (int8_t *) ALIGN_TO((uintptr_t) p, 16);
	return p;
}

static uint32_t qb_get_storage_structure_size(qb_encoder_context *cxt) {
	uint32_t size = sizeof(qb_storage), i;
	size += sizeof(qb_memory_segment) * cxt->compiler_context->storage->segment_count;
	size = ALIGN_TO(size, sizeof(uintptr_t));
	for(i = 0; i < cxt->compiler_context->storage->segment_count; i++) {
		qb_memory_segment *src = &cxt->compiler_context->storage->segments[i];
		size += src->reference_count * sizeof(uintptr_t *);
	}
	size = ALIGN_TO(size, 16);
	return size;
}

static int8_t * qb_copy_storage_structure(qb_encoder_context *cxt, int8_t * memory) {
	int8_t *p = memory;
	qb_storage *storage;
	uint32_t i;
#if ZEND_DEBUG
	uint32_t length = qb_get_storage_structure_size(cxt);
#endif

	storage = (qb_storage *) p;	p += sizeof(qb_storage);
	storage->flags = 0;
	storage->segment_count = cxt->compiler_context->storage->segment_count;
	storage->segments = (qb_memory_segment *) p; p += sizeof(qb_memory_segment) * cxt->compiler_context->storage->segment_count;

	for(i = 0; i < cxt->compiler_context->storage->segment_count; i++) {
		qb_memory_segment *src = &cxt->compiler_context->storage->segments[i];
		qb_memory_segment *dst = &storage->segments[i];
		dst->flags = src->flags;
		dst->byte_count = src->byte_count;
		dst->current_allocation = 0;
		dst->stream = NULL;
		dst->memory = NULL;
		dst->reference_count = 0;

		if(src->reference_count > 0) {
			p = (int8_t *) ALIGN_TO((uintptr_t) p, sizeof(uintptr_t));
			dst->references = (uintptr_t **) p;
			p += src->reference_count * sizeof(uintptr_t *);
		} else {
			dst->references = NULL;
		}
	}

	p = (int8_t *) ALIGN_TO((uintptr_t) p, 16);
#if ZEND_DEBUG
	if(memory + length != p) {
		qb_abort("length mismatch");
	}
#endif
	return p;
}

static uint32_t qb_get_preallocated_segment_size(qb_encoder_context *cxt) {
	uint32_t size = 0;
	uint32_t i;
	for(i = 0; i < cxt->compiler_context->storage->segment_count; i++) {
		qb_memory_segment *src = &cxt->compiler_context->storage->segments[i];
		if(src->flags & QB_SEGMENT_PREALLOCATED) {
			uint32_t segment_length = ALIGN_TO(src->byte_count, 16); 
			size += segment_length;
		}
	}
	return size;
}

static int8_t * qb_preallocate_segments(qb_encoder_context *cxt, int8_t *memory, qb_storage *storage) {
#if ZEND_DEBUG
	uint32_t length = qb_get_preallocated_segment_size(cxt);
#endif
	int8_t *p = memory;
	uint32_t i;

	// set up memory segments 
	for(i = 0; i < cxt->compiler_context->storage->segment_count; i++) {
		qb_memory_segment *dst = &storage->segments[i];

		// memory is preallocated
		if(dst->flags & QB_SEGMENT_PREALLOCATED) {
			qb_memory_segment *src = &cxt->compiler_context->storage->segments[i];
			qb_memory_segment *dst = &storage->segments[i];
			uint32_t segment_length = ALIGN_TO(src->byte_count, 16); 

			dst->memory = p;
			dst->current_allocation = segment_length;

			if(src->memory) {
				memcpy(dst->memory, src->memory, src->current_allocation);
			} else {
#if ZEND_DEBUG
				memset(dst->memory, 'K', segment_length);
#endif
			}
			if(dst->byte_count > 0 && !(dst->flags & QB_SEGMENT_CLEAR_ON_CALL)) {
				// the segment won't be cleared when the function is called
				// we need to zero out the bytes now
				if(src->current_allocation != dst->byte_count) {
					memset(dst->memory + src->current_allocation, 0, dst->byte_count - src->current_allocation);
				}
			}
			p += segment_length;
		}
	}

#if ZEND_DEBUG
	if(memory + length != p) {
		qb_abort("length mismatch");
	}
#endif
	return p;
}

qb_function * qb_encode_function(qb_encoder_context *cxt) {
	qb_function *qfunc;
	int8_t *memory, *p;
	uint32_t function_struct_size, storage_struct_size, preallocated_segment_size, instruction_length, opcode_length;
	uint32_t total_size;

	// set the offset of the op
	qb_set_instruction_offsets(cxt);

	// calculate sizes of various part of the function
	function_struct_size = qb_get_function_structure_size(cxt);
	storage_struct_size = qb_get_storage_structure_size(cxt);
	preallocated_segment_size = qb_get_preallocated_segment_size(cxt);
	instruction_length = cxt->instruction_stream_length;
	opcode_length = sizeof(uint16_t) * cxt->instruction_op_count;

	total_size = function_struct_size + storage_struct_size + preallocated_segment_size + instruction_length + opcode_length;

	// allocate memory and copy everything into a continuous block of memory
	// add a bit of padding in case the memory pointer returned isn't aligned
	p = memory = emalloc(total_size + 16);

	// copy stuff into the function structure 
	qfunc = (qb_function *) p;
	p = qb_copy_function_structure(cxt, p);

	// copy stuff into the storage structure 
	qfunc->local_storage = (qb_storage *) p;
	cxt->storage = qfunc->local_storage;
	p = qb_copy_storage_structure(cxt, p);
	
	// assign memory to preallocated segments
	p = qb_preallocate_segments(cxt, p, qfunc->local_storage);

	// encode the instructions
	qfunc->instructions = p;
	qfunc->instruction_start = qfunc->instructions;
	qfunc->instruction_non_static_start = qfunc->instructions + cxt->ops[cxt->initialization_op_count]->instruction_offset;
	cxt->instructions = qfunc->instructions;
	p = qb_encode_instruction_stream(cxt, p);

	// store the opcodes for use during relocation
	qfunc->instruction_opcodes = (uint16_t *) p;
	qfunc->instruction_opcode_count = cxt->instruction_op_count;
	p = qb_copy_instruction_opcodes(cxt, p);

	// calculate the CRC64 signature
	qfunc->instruction_crc64 = qb_calculate_crc64((uint8_t *) qfunc->instructions, cxt->instruction_stream_length, 0);
	qfunc->size = total_size;
	return qfunc;
}

void qb_initialize_encoder_context(qb_encoder_context *cxt, qb_compiler_context *compiler_cxt TSRMLS_DC) {
#ifndef _MSC_VER
	static int handlers_initialized = FALSE;
	if(!handlers_initialized) {
		qb_main(NULL, NULL);
		handlers_initialized = TRUE;
	}
#endif

	memset(cxt, 0, sizeof(qb_encoder_context));

	cxt->compiler_context = compiler_cxt;
	cxt->ops = compiler_cxt->ops;
	cxt->op_count = compiler_cxt->op_count;
	cxt->initialization_op_count = compiler_cxt->initialization_op_count;

	SAVE_TSRMLS
}

void qb_free_function(qb_function *qfunc) {
	uint32_t i;

	// free memory segments
	for(i = QB_SELECTOR_ARRAY_START; i < qfunc->local_storage->segment_count; i++) {
		qb_memory_segment *segment = &qfunc->local_storage->segments[i];
		if(segment->memory) {
			if(segment->flags & QB_SEGMENT_MAPPED) {
				// PHP should have clean it already
			} else if(!(segment->flags & QB_SEGMENT_BORROWED)) {
				efree(segment->memory);
			}
		}
	}

	efree(qfunc);
}
