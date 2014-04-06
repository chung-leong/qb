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
#include "qb_interpreter_structures.h"

void qb_set_instruction_offsets(qb_encoder_context *cxt) {
	uint32_t instruction_offset, i;
	uint32_t count = 0;

	// determine the offsets of each instruction in the stream
	instruction_offset = sizeof(qb_op_handler);
	for(i = 0; i < cxt->op_count; i++) {
		qb_op *qop = cxt->ops[i];
		qop->instruction_offset = instruction_offset;
		if(qop->opcode != QB_NOP) {
			uint32_t instruction_length = qb_get_instruction_length(qop->opcode);
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
	// don't add relocation pointers for those pointing to preallocated segments 
	if(!(segment->flags & QB_SEGMENT_PREALLOCATED)) {
		uintptr_t **p_reference = &segment->references[segment->reference_count++];
		*p_reference = (uintptr_t *) ((void *) (cxt->instruction_base_address + ((uintptr_t) p_pointer - (uintptr_t) cxt->instructions)));
	}
}

static int32_t qb_get_pointer(qb_encoder_context *cxt, qb_address *address, void **p_pointer) {
	if(address->segment_selector < cxt->storage->segment_count) {
		qb_memory_segment *segment = &cxt->storage->segments[address->segment_selector];
		if(segment->flags & QB_SEGMENT_PREALLOCATED) {
			if(cxt->position_independent) {
				// add the base address so we know it's pointing to a preallocated segment
				*p_pointer = (void *) (cxt->storage_base_address + address->segment_offset + ((uintptr_t) segment->memory - (uintptr_t) cxt->storage));
			} else {
				*p_pointer = (void *) (segment->memory + address->segment_offset);
			}
		} else {
			*p_pointer = (void *) (uintptr_t) (0 + address->segment_offset);
		}
		return TRUE;
	} else {
		qb_report_internal_error(0, "Invalid segment");
		return FALSE;
	}
}

static int32_t qb_encode_address(qb_encoder_context *cxt, qb_address *address, int8_t **p_ip) {
	switch(address->mode) {
		case QB_ADDRESS_MODE_SCA: {
			qb_pointer_SCA *p = ((qb_pointer_SCA *) *p_ip);
			if(!qb_get_pointer(cxt, address, &p->data_pointer)) {
				return FALSE;
			}
			*p_ip += sizeof(qb_pointer_SCA);

			qb_add_segment_reference(cxt, address, &p->data_pointer);
		}	break;
		case QB_ADDRESS_MODE_ELE: {
			qb_pointer_ELE *p = ((qb_pointer_ELE *) *p_ip);
			if(!qb_get_pointer(cxt, address, &p->data_pointer)
			|| !qb_get_pointer(cxt, address->array_index_address, (void **) &p->index_pointer)) {
				return FALSE;
			}
			*p_ip += sizeof(qb_pointer_ELE);

			qb_add_segment_reference(cxt, address, &p->data_pointer);
			qb_add_segment_reference(cxt, address->array_index_address, (void **) &p->index_pointer);
		}	break;
		case QB_ADDRESS_MODE_ARR: {
			qb_pointer_ARR *p = ((qb_pointer_ARR *) *p_ip);
			if(!qb_get_pointer(cxt, address, &p->data_pointer)
			|| !qb_get_pointer(cxt, address->array_index_address, (void **) &p->index_pointer)
			|| !qb_get_pointer(cxt, address->array_size_address, (void **) &p->count_pointer)) {
				return FALSE;
			}
			*p_ip += sizeof(qb_pointer_ARR);

			qb_add_segment_reference(cxt, address, &p->data_pointer);
			qb_add_segment_reference(cxt, address->array_index_address, (void **) &p->index_pointer);
			qb_add_segment_reference(cxt, address->array_size_address, (void **) &p->count_pointer);
		}	break;
		default: {
			qb_report_internal_error(0, "Invalid address type");
			return FALSE;
		}
	}
	return TRUE;
}

#if defined(USE_TAIL_CALL_INTERPRETER_LOOP) || defined(USE_COMPUTED_GOTO_INTERPRETER_LOOP)
#	define USE_OP_HANDLER
#endif

static zend_always_inline qb_op_handler qb_get_handler(qb_encoder_context *cxt, qb_op *qop) {
#ifdef ZEND_DEBUG
	if(qop->opcode >= QB_OPCODE_COUNT) {
		qb_debug_abort("Illegal opcode");
	}
#endif
#ifdef USE_OP_HANDLER
	if(cxt->position_independent) {
		return (void *) ((uintptr_t) qop->opcode);
	} else {
		return op_handlers[qop->opcode];
	}
#else
	return (qb_op_handler) qop->opcode;
#endif
}

static int32_t qb_encode_handler(qb_encoder_context *cxt, uint32_t target_index, int8_t **p_ip) {
	qb_op *target_qop = cxt->ops[target_index];

	if(target_index < cxt->op_count) {
		while(target_qop->opcode == QB_NOP) {
			target_qop = cxt->ops[++target_index];
		}

		*((qb_op_handler *) *p_ip) = qb_get_handler(cxt, target_qop);
		*p_ip += sizeof(qb_op_handler);
		return TRUE;
	} else {
		qb_report_internal_error(0, "Invalid op index");
		return FALSE;
	}
}

static zend_always_inline int8_t *qb_get_instruction_pointer(qb_encoder_context *cxt, qb_op *qop) {
	int8_t *p;
	if(cxt->position_independent) {
		p = (int8_t *) (cxt->instruction_base_address + qop->instruction_offset);
	} else {
		p = cxt->instructions + qop->instruction_offset;
	}
	return p;
}

static int32_t qb_encode_jump_target(qb_encoder_context *cxt, uint32_t target_index, int8_t **p_ip) {
	qb_op *target_qop = cxt->ops[target_index];

	if(target_index < cxt->op_count) {
		while(target_qop->opcode == QB_NOP) {
			target_qop = cxt->ops[++target_index];
		}

		*((void **) *p_ip) = qb_get_handler(cxt, target_qop);
		*p_ip += sizeof(void *);

		*((int8_t **) *p_ip) = qb_get_instruction_pointer(cxt, target_qop); 
		*p_ip += sizeof(int8_t *);

		return TRUE;
	} else {
		qb_report_internal_error(0, "Invalid jump target");
		return FALSE;
	}
}

static int32_t qb_encode_line_id(qb_encoder_context *cxt, uint32_t line_id, int8_t **p_ip) {
	*((uint32_t *) *p_ip) = line_id; 
	*p_ip += sizeof(uint32_t);
	return TRUE;
}

static int32_t qb_encode_segment_selector(qb_encoder_context *cxt, qb_address *address, int8_t **p_ip) {
	*((uint32_t *) *p_ip) = address->segment_selector;
	*p_ip += sizeof(uint32_t);
	return TRUE;
}

static int32_t qb_encode_element_size(qb_encoder_context *cxt, qb_address *address, int8_t **p_ip) {
	*((uint32_t *) *p_ip) = BYTE_COUNT(1, address->type);
	*p_ip += sizeof(uint32_t);
	return TRUE;
}

static int32_t qb_encode_number(qb_encoder_context *cxt, int32_t number, int8_t **p_ip) {
	*((int32_t *) *p_ip) = number;
	*p_ip += sizeof(uint32_t);
	return TRUE;
}

int8_t * qb_encode_instruction_stream(qb_encoder_context *cxt, int8_t *memory) {
	uint32_t i, j;
	int8_t *ip = cxt->instructions = memory;

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

#ifdef ZEND_DEBUG
			if((int8_t *) memory + qop->instruction_offset != ip) {
				qb_op *prev_qop = cxt->ops[i - 1];
				do {
					prev_qop = cxt->ops[--i];
				} while(prev_qop->opcode == QB_NOP);
				qb_debug_abort("the previous op was not correctly encoded");
			}
#endif

			if(qop->flags & (QB_OP_JUMP | QB_OP_BRANCH | QB_OP_EXIT)) {
				// put in the jump targets
				for(j = 0; j < qop->jump_target_count; j++) {
					qb_encode_jump_target(cxt, qop->jump_target_indices[j], &ip);
				}
			} else if(!(qop->flags & QB_OP_BRANCH_TABLE)) {
				// need the handler to the next instruction if it's not a jump operation
				// as functions must always end with a return
				// there's no check here if there is an op at i + 1
				qb_encode_handler(cxt, i + 1, &ip);
			}

			for(j = 0; j < qop->operand_count; j++) {
				qb_operand *operand = &qop->operands[j];

				switch(operand->type) {
					case QB_OPERAND_ADDRESS: {
						if(!qb_encode_address(cxt, operand->address, &ip)) {
							return NULL;
						}
					}	break;
					case QB_OPERAND_SEGMENT_SELECTOR: {
						if(!qb_encode_segment_selector(cxt, operand->address, &ip)) {
							return NULL;
						}
					}	break;
					case QB_OPERAND_ELEMENT_SIZE: {
						if(!qb_encode_element_size(cxt, operand->address, &ip)) {
							return NULL;
						}
					}	break;
					case QB_OPERAND_NUMBER: {
						if(!qb_encode_number(cxt, operand->number, &ip)) {
							return NULL;
						}
					}	break;
					default: {
						qb_report_internal_error(qop->line_id, "Invalid operand type");
						return NULL;
					}	break;
				}
			}

			if(qop->flags & QB_OP_BRANCH_TABLE) {
				for(j = 0; j < qop->jump_target_count; j++) {
					if(!qb_encode_jump_target(cxt, qop->jump_target_indices[j], &ip)) {
						return NULL;
					}
				}
			}

			// put the line number at the end if it's needed
			if(qop->flags & QB_OP_NEED_LINE_IDENTIFIER) {
				if(!qb_encode_line_id(cxt, qop->line_id, &ip)) {
					return NULL;
				}
			}
		}
	}
#if ZEND_DEBUG
	if(memory + cxt->instruction_stream_length != ip) {
		qb_debug_abort("length mismatch");
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
	if(!IS_SCALAR(address)) {
		if(address->dimension_count > 1) {
			length += sizeof(qb_address *) * address->dimension_count * 2;
			length += sizeof(qb_address) * (address->dimension_count * 2 - 1);
		} else {
			length += sizeof(qb_address);
		}
		if(address->array_index_address) {
			if(address->array_index_address->segment_selector != QB_SELECTOR_CONSTANT_SCALAR || address->array_index_address->segment_offset != 0) {
				// it's not zero
				length += sizeof(qb_address);
			}
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

static int8_t * qb_copy_address(qb_address *address, int8_t *memory) {
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

	if(IS_SCALAR(src)) {
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
					dst->array_size_addresses[i] = (qb_address *) p; 
					p = qb_copy_address(src->array_size_addresses[i], p);
				}
			}
			dst->array_size_address = dst->array_size_addresses[0];
		} else {
			dst->dimension_addresses = &dst->array_size_address;
			dst->array_size_addresses = &dst->array_size_address;
			dst->array_size_address = (qb_address *) p;
			p = qb_copy_address(src->array_size_address, p);
		}
		if(src->array_index_address) {
			if(src->array_index_address->segment_selector != QB_SELECTOR_CONSTANT_SCALAR || src->array_index_address->segment_offset != 0) {
				dst->array_index_address = (qb_address *) p;
				p = qb_copy_address(src->array_index_address, p);
			}
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
				} else {
					dst->index_alias_schemes[i] = NULL;
				}
			}
		} else {
			dst->index_alias_schemes = NULL;
		}
	}
#if ZEND_DEBUG
	if(memory + length != p) {
		qb_debug_abort("length mismatch");
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

int8_t * qb_copy_variable(qb_variable *qvar, int8_t *memory) {
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
	dst->value = NULL;
	dst->value_pointer = NULL;

#if ZEND_DEBUG
	if(memory + length != p) {
		qb_debug_abort("length mismatch");
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
	return size;
}

static int8_t * qb_copy_function_structure(qb_encoder_context *cxt, int8_t *memory) {
	int8_t *p = memory;
	qb_function *qfunc;
	uint32_t i;
#if ZEND_DEBUG
	uint32_t length = qb_get_function_structure_size(cxt);
#endif

	qfunc = (qb_function *) p; p += sizeof(qb_function);

	// copy variables
	qfunc->return_variable = NULL;
	qfunc->return_key_variable = NULL;
	qfunc->sent_variable = NULL;
	qfunc->variables = (qb_variable **) p; p += sizeof(qb_variable *) * cxt->compiler_context->variable_count;
	qfunc->variable_count = cxt->compiler_context->variable_count;
	for(i = 0; i < cxt->compiler_context->variable_count; i++) {
		qfunc->variables[i] = (qb_variable *) p;
		p = qb_copy_variable(cxt->compiler_context->variables[i], p);

		if(qfunc->variables[i]->flags & QB_VARIABLE_RETURN_VALUE) {
			qfunc->return_variable = qfunc->variables[i];
		} else if(qfunc->variables[i]->flags & QB_VARIABLE_RETURN_KEY_VALUE) {
			qfunc->return_key_variable = qfunc->variables[i];
		} else if(qfunc->variables[i]->flags & QB_VARIABLE_SENT_VALUE) {
			qfunc->sent_variable = qfunc->variables[i];
		}
	}

	qfunc->argument_count = cxt->compiler_context->argument_count;
	qfunc->name = cxt->compiler_context->function_prototype.name;
	qfunc->line_id = cxt->compiler_context->function_prototype.line_id;
	qfunc->native_proc = NULL;
	qfunc->zend_op_array = cxt->compiler_context->zend_op_array;
	qfunc->flags = cxt->compiler_context->function_flags;
	qfunc->next_reentrance_copy = NULL;
	qfunc->next_forked_copy = NULL;
	qfunc->in_use = 0;

#if ZEND_DEBUG
	if(memory + length != p) {
		qb_debug_abort("length mismatch");
	}
#endif
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
		dst->imported_segment = NULL;
		dst->next_dependent = NULL;

		// increment the reference count as references are added
		dst->reference_count = 0;
		if(src->reference_count > 0) {
			p = (int8_t *) ALIGN_TO((uintptr_t) p, sizeof(uintptr_t));
			dst->references = (uintptr_t **) p;
			p += src->reference_count * sizeof(uintptr_t *);
		} else {
			dst->references = NULL;
		}
	}

#if ZEND_DEBUG
	if(memory + length != p) {
		qb_debug_abort("length mismatch");
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
	int8_t *p = (int8_t *) ALIGN_TO((uintptr_t) memory, 16);
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
			}
			if(dst->byte_count > 0 && !(dst->flags & QB_SEGMENT_CLEAR_ON_CALL)) {
				// the segment won't be cleared when the function is called
				// we need to zero out the bytes now
				if(src->current_allocation > dst->byte_count) {
					memset(dst->memory + dst->byte_count, 0, src->current_allocation - dst->byte_count);
				}
			}
			p += segment_length;
		}
	}
	return p;
}

int64_t qb_calculate_function_crc64(qb_encoder_context *cxt) {
	// calculate the crc64 of the instruction stream plus the constant segments
	qb_memory_segment *scalar_segment = &cxt->storage->segments[QB_SELECTOR_CONSTANT_SCALAR];
	qb_memory_segment *array_segment = &cxt->storage->segments[QB_SELECTOR_CONSTANT_ARRAY];
	uint32_t i;
	int64_t crc64 = 0;
	crc64 = qb_calculate_crc64((uint8_t *) cxt->instructions, cxt->instruction_stream_length, crc64);
	crc64 = qb_calculate_crc64((uint8_t *) scalar_segment->memory, scalar_segment->byte_count, crc64 ^ 0x1234567F);
	crc64 = qb_calculate_crc64((uint8_t *) array_segment->memory, array_segment->byte_count, crc64 ^ 0x123456F8);

	// make sure the layout of variables match
	for(i = 0; i < cxt->compiler_context->writable_scalar_count; i++) {
		qb_address *address = cxt->compiler_context->writable_scalars[i];
		crc64 = qb_calculate_crc64((uint8_t *) &address->segment_selector, sizeof(uint32_t), crc64 ^ 0x1234567F);
		crc64 = qb_calculate_crc64((uint8_t *) &address->segment_offset, sizeof(uint32_t), crc64);
	}
	for(i = 0; i < cxt->compiler_context->writable_array_count; i++) {
		qb_address *address = cxt->compiler_context->writable_arrays[i];
		crc64 = qb_calculate_crc64((uint8_t *) &address->segment_selector, sizeof(uint32_t), crc64 ^ 0x123456F8);
		crc64 = qb_calculate_crc64((uint8_t *) &address->segment_offset, sizeof(uint32_t), crc64);
	}
	for(i = 0; i < cxt->compiler_context->constant_array_count; i++) {
		qb_address *address = cxt->compiler_context->constant_arrays[i];
		crc64 = qb_calculate_crc64((uint8_t *) &address->segment_selector, sizeof(uint32_t), crc64 ^ 0x123456F8);
		crc64 = qb_calculate_crc64((uint8_t *) &address->segment_offset, sizeof(uint32_t), crc64);
	}
	return crc64;
}

qb_function * qb_encode_function(qb_encoder_context *cxt) {
	qb_function *qfunc;
	int8_t *p;
	uint32_t function_struct_size, storage_struct_size, preallocated_segment_size, instruction_length, opcode_length;

	// set the offset of the op
	qb_set_instruction_offsets(cxt);

	// calculate sizes of various part of the function
	function_struct_size = qb_get_function_structure_size(cxt);
	storage_struct_size = qb_get_storage_structure_size(cxt);
	preallocated_segment_size = qb_get_preallocated_segment_size(cxt);
	instruction_length = cxt->instruction_stream_length;
	opcode_length = sizeof(uint16_t) * cxt->instruction_op_count;

	// allocate memory for the function structure
	p = emalloc(function_struct_size + 16);

	// copy stuff into the function structure 
	qfunc = (qb_function *) p;
	p = qb_copy_function_structure(cxt, p);

	// allocate memory for the storage structure and preallocatd segments
	// add a bit of padding in case the pointer returned isn't aligned
	p = emalloc(storage_struct_size + preallocated_segment_size + 16);

	// copy stuff into the storage structure 
	qfunc->local_storage = (qb_storage *) p;
	qfunc->local_storage->size = storage_struct_size + preallocated_segment_size + 16;
	cxt->storage = qfunc->local_storage;
	p = qb_copy_storage_structure(cxt, p);
	
	// assign memory to preallocated segments
	p = qb_preallocate_segments(cxt, p, qfunc->local_storage);

	// allocate memory for the instruction stream and opcode array
	p = emalloc(instruction_length + opcode_length);

	// encode the instructions
	qfunc->instructions = cxt->instructions = p;
	p = qb_encode_instruction_stream(cxt, p);

	if(!p) {
		return NULL;
	}

	// store the opcodes for use during relocation
	qfunc->instruction_opcodes = (uint16_t *) p;
	qfunc->instruction_opcode_count = cxt->instruction_op_count;
	p = qb_copy_instruction_opcodes(cxt, p);

	// calculate the CRC64 signature
	qfunc->instruction_crc64 = qb_calculate_function_crc64(cxt);
	qfunc->instruction_length = cxt->instruction_stream_length;

	if(cxt->position_independent) {
		// save the placeholder base address used to encode the instructions
		qfunc->instruction_base_address = cxt->instruction_base_address;
		qfunc->instruction_start = (int8_t *) cxt->instruction_base_address;
		qfunc->local_storage_base_address = cxt->storage_base_address;
	} else {
		// everything is pointing to actual addresses already
		qfunc->flags |= QB_FUNCTION_INITIALIZED | QB_FUNCTION_RELOCATED;
		qfunc->instruction_start = cxt->instructions;
		qfunc->instruction_base_address = (uintptr_t) cxt->instructions;
		qfunc->local_storage_base_address = (uintptr_t) cxt->storage;
	}
	return qfunc;
}

static void qb_adjust_pointer(void **p, uintptr_t start, uintptr_t end, intptr_t shift) {
	uintptr_t address = *((uintptr_t *) p);
	if(start <= address && address < end) {
		SHIFT_POINTER(*p, shift);
	}
}

intptr_t qb_relocate_function(qb_function *qfunc, int32_t reentrance) {
	intptr_t instruction_shift = ((uintptr_t) qfunc->instructions) - qfunc->instruction_base_address;
	intptr_t storage_shift = ((uintptr_t) qfunc->local_storage) - qfunc->local_storage_base_address;
	if(instruction_shift || storage_shift) {
		int8_t *ip = qfunc->instructions;
		uint32_t i, j;
		int32_t initializing = !(qfunc->flags & QB_FUNCTION_INITIALIZED);
		uintptr_t range_start, range_end;
		qb_memory_segment *segment_start, *segment_end;

		if(initializing) {
			// all pointers to preallocated segments need to be relocated
			segment_start = &qfunc->local_storage->segments[QB_SELECTOR_FIRST_PREALLOCATED];
			segment_end = &qfunc->local_storage->segments[QB_SELECTOR_LAST_PREALLOCATED];
		} else {
			if(reentrance) {
				// the pointers to shared segments need to be relocated as well
				// since they're only shared between forked copies of the function
				segment_start = &qfunc->local_storage->segments[QB_SELECTOR_SHARED_SCALAR];
				segment_end = &qfunc->local_storage->segments[QB_SELECTOR_SHARED_ARRAY];
			} else {
				// only pointers to local segments need to be relocated
				segment_start = &qfunc->local_storage->segments[QB_SELECTOR_LOCAL_SCALAR];
				segment_end = &qfunc->local_storage->segments[QB_SELECTOR_LOCAL_ARRAY];
			}
		}
		range_start = qfunc->local_storage_base_address + ((uintptr_t) segment_start->memory - (uintptr_t) qfunc->local_storage);
		range_end = qfunc->local_storage_base_address + ((uintptr_t) segment_end->memory - (uintptr_t) qfunc->local_storage) + segment_end->byte_count;

#ifdef USE_OP_HANDLER
		if(initializing) {
			// update the first next handler
			void **p_handler = (void **) ip;
			qb_opcode next_opcode = (qb_opcode) *p_handler;
			*p_handler = op_handlers[next_opcode];
		}
#endif
		ip += sizeof(void *);

		// go through the instructions and fix up pointers to preallocated segments
		for(i = 0; i < qfunc->instruction_opcode_count; i++) {
			qb_opcode opcode = qfunc->instruction_opcodes[i];
			uint32_t op_flags = qb_get_op_flags(opcode);
			const char *format = qb_get_op_format(opcode), *s;

			if(op_flags & QB_OP_EXIT) {
				// nothing
			} else if(!(op_flags & QB_OP_BRANCH_TABLE)) {
#ifdef USE_OP_HANDLER
				if(initializing) {
					// update next handler
					void **p_handler = (void **) ip;
					qb_opcode next_opcode = (qb_opcode) *p_handler;
					*p_handler = op_handlers[next_opcode];
				}
#endif
				ip += sizeof(void *);

				if(op_flags & QB_OP_BRANCH) {
					// update instruction pointer
					int8_t **p_ip = (int8_t **) ip;
					SHIFT_POINTER(*p_ip, instruction_shift);
					ip += sizeof(int8_t *);

#ifdef USE_OP_HANDLER
					if(initializing) {
						// update second next handler
						void **p_handler = (void **) ip;
						qb_opcode next_opcode = (qb_opcode) *p_handler;
						*p_handler = op_handlers[next_opcode];
					}
#endif
					ip += sizeof(void *);

					// update second instruction pointer
					p_ip = (int8_t **) ip;
					SHIFT_POINTER(*p_ip, instruction_shift);
					ip += sizeof(int8_t *);
				} else if(op_flags & QB_OP_JUMP) {
					int8_t **p_ip = (int8_t **) ip;
					SHIFT_POINTER(*p_ip, instruction_shift);

					ip += sizeof(int8_t *);
				}
			}
			
			for(s = format; *s != '\0'; s++) {
				switch(*s) {
					case 'S':
					case 's': {
						qb_pointer_SCA *p = (qb_pointer_SCA *) ip;
						qb_adjust_pointer((void **) &p->data_pointer, range_start, range_end, storage_shift);
						ip += sizeof(qb_pointer_SCA);
					}	break;
					case 'E':
					case 'e': {
						qb_pointer_ELE *p = (qb_pointer_ELE *) ip;
						qb_adjust_pointer((void **) &p->data_pointer, range_start, range_end, storage_shift);
						qb_adjust_pointer((void **) &p->index_pointer, range_start, range_end, storage_shift);
						ip += sizeof(qb_pointer_ELE);
					}	break;	
					case 'A':
					case 'a': {
						qb_pointer_ARR *p = (qb_pointer_ARR *) ip;
						qb_adjust_pointer((void **) &p->data_pointer, range_start, range_end, storage_shift);
						qb_adjust_pointer((void **) &p->index_pointer, range_start, range_end, storage_shift);
						qb_adjust_pointer((void **) &p->count_pointer, range_start, range_end, storage_shift);
						ip += sizeof(qb_pointer_ARR);
					}	break;
					case 'c': {
						ip += sizeof(uint32_t);
					}	break;
				}
			}

			if(op_flags & QB_OP_BRANCH_TABLE) {
				uint32_t branch_count = qb_get_switch_table_size_from_opcode(opcode);
				uint32_t j;
				for(j = 0; j < branch_count; j++) {
					int8_t **p_ip;
#ifdef USE_OP_HANDLER
					if(initializing) {
						// update next handler
						void **p_handler = (void **) ip;
						qb_opcode next_opcode = (qb_opcode) *p_handler;
						*p_handler = op_handlers[next_opcode];
					}
#endif
					ip += sizeof(void *);

					p_ip = (int8_t **) ip;
					SHIFT_POINTER(*p_ip, instruction_shift);
					ip += sizeof(int8_t *);
				}
			}

			if(op_flags & QB_OP_NEED_LINE_IDENTIFIER) {
				ip += sizeof(uint32_t);
			}
		}

		// update the instruction start pointer
		SHIFT_POINTER(qfunc->instruction_start, instruction_shift);

		if(!(qfunc->flags & QB_FUNCTION_INITIALIZED)) {
			// update the reallocation pointers
			for(i = QB_SELECTOR_LAST_PREALLOCATED + 1; i < qfunc->local_storage->segment_count; i++) {
				qb_memory_segment *segment = &qfunc->local_storage->segments[i];
				for(j = 0; j < segment->reference_count; j++) {
					SHIFT_POINTER(segment->references[j], instruction_shift);
				}
			}
		}
	}
	qfunc->instruction_base_address = (uintptr_t) qfunc->instructions;
	qfunc->local_storage_base_address = (uintptr_t) qfunc->local_storage;
	qfunc->flags |= QB_FUNCTION_RELOCATED | QB_FUNCTION_INITIALIZED;
	return instruction_shift;
}

qb_function * qb_create_function_copy(qb_function *base, int32_t reentrance) {
	qb_function *qfunc;
	intptr_t instruction_shift = 0;

	qfunc = emalloc(sizeof(qb_function));
	memcpy(qfunc, base, sizeof(qb_function));
	if(base->instructions) {
		qfunc->instructions = emalloc(base->instruction_length);
		memcpy(qfunc->instructions, base->instructions, base->instruction_length);
		instruction_shift = (uintptr_t) qfunc->instructions - (uintptr_t) base->instructions;
	}
	qfunc->in_use = 1;
	qfunc->local_storage = qb_create_storage_copy(base->local_storage, instruction_shift, reentrance);
	qfunc->next_reentrance_copy = NULL;
	qfunc->next_forked_copy = NULL;
	qfunc->flags &= ~QB_FUNCTION_RELOCATED;
	return qfunc;
}

void qb_main(qb_interpreter_context *__restrict cxt);

void qb_initialize_encoder_context(qb_encoder_context *cxt, qb_compiler_context *compiler_cxt, int32_t position_independent TSRMLS_DC) {
#ifdef USE_COMPUTED_GOTO_INTERPRETER_LOOP
	// the handlers point to code position with in qb_main()
	// only the function have access to these pointers
	static int handlers_initialized = FALSE;
	if(!handlers_initialized) {
		qb_main(NULL);
		handlers_initialized = TRUE;
	}
#endif

	memset(cxt, 0, sizeof(qb_encoder_context));

	cxt->compiler_context = compiler_cxt;
	cxt->ops = compiler_cxt->ops;
	cxt->op_count = compiler_cxt->op_count;
	cxt->position_independent = position_independent;
	cxt->storage = NULL;

	cxt->instruction_stream_length = 0;
	cxt->instruction_op_count = 0;
	cxt->instruction_crc64 = 0;
	 NULL;

	if(position_independent) {
		// map stuff half and a quarter way up the entire address space initially
		cxt->instruction_base_address = ((uintptr_t) -1) / 4;
		cxt->storage_base_address = ((uintptr_t) -1) / 2;
	} else {
		cxt->storage = compiler_cxt->storage;
	}

	SAVE_TSRMLS
}

void qb_free_encoder_context(qb_encoder_context *cxt) {
}

void qb_free_function(qb_function *qfunc) {
	uint32_t i;

	if(qfunc->next_reentrance_copy) {
		qb_free_function(qfunc->next_reentrance_copy);
	}
	if(qfunc->next_forked_copy) {
		qb_free_function(qfunc->next_forked_copy);
	}
	// free memory segments
	for(i = QB_SELECTOR_ARRAY_START; i < qfunc->local_storage->segment_count; i++) {
		qb_memory_segment *segment = &qfunc->local_storage->segments[i];
		if(segment->current_allocation) {
			if(segment->flags & QB_SEGMENT_MAPPED) {
				// PHP should have clean it already
			} else if(!(segment->flags & QB_SEGMENT_BORROWED)) {
				efree(segment->memory);
			}
		}
	}
	efree(qfunc->instructions);
	efree(qfunc->local_storage);
	efree(qfunc);
}
