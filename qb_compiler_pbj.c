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

static uint8_t ZEND_FASTCALL qb_pbj_read_I08(qb_compiler_context *cxt) {
	uint8_t value = 0;
	if(cxt->pbj_data_end - cxt->pbj_data >= 1) { 
		value = *((uint8_t *) cxt->pbj_data);
		cxt->pbj_data += 1;
	}
	return value;
}

static uint16_t ZEND_FASTCALL qb_pbj_read_I16(qb_compiler_context *cxt) {
	uint16_t value = 0;
	if(cxt->pbj_data_end - cxt->pbj_data >= 2) { 
		value = SWAP_LE_I16(*((uint16_t *) cxt->pbj_data));
		cxt->pbj_data += 2;
	}
	return value;
}

static uint32_t ZEND_FASTCALL qb_pbj_read_I32(qb_compiler_context *cxt) {
	uint32_t value = 0;
	if(cxt->pbj_data_end - cxt->pbj_data >= 4) { 
		value = SWAP_LE_I32(*((uint32_t *) cxt->pbj_data));
		cxt->pbj_data += 4;
	}
	return value;
}

static float32_t ZEND_FASTCALL qb_pbj_read_F32(qb_compiler_context *cxt) {
	float32_t value = 0.0f;
	if(cxt->pbj_data_end - cxt->pbj_data >= 4) { 
		uint32_t int_value = SWAP_BE_I32(*((uint32_t *) cxt->pbj_data));
		value = *((float32_t *) &int_value);
		cxt->pbj_data += 4;
	}
	return value;
}

static const char * ZEND_FASTCALL qb_pbj_read_string(qb_compiler_context *cxt, uint32_t *p_length) {
	const char *value = "";
	if(p_length) {
		uint16_t len = qb_pbj_read_I16(cxt);
		if(cxt->pbj_data_end - cxt->pbj_data >= len) { 
			value = (const char *) cxt->pbj_data;
			*p_length = len;
		} else {
			*p_length = 0;
		}
		cxt->pbj_data += len;
	} else {
		const char *start = (const char *) cxt->pbj_data;
		while(cxt->pbj_data < cxt->pbj_data_end) {
			if(*cxt->pbj_data == '\0') {
				value = start;
				cxt->pbj_data++;
				break;
			}
			cxt->pbj_data++; 
		}
	}
	return value;
}

static qb_pbj_parameter * ZEND_FASTCALL qb_pbj_find_parameter_by_address(qb_compiler_context *cxt, qb_pbj_address *address) {
	uint32_t i;
	for(i = 0; i < cxt->pbj_parameter_count; i++) {
		qb_pbj_parameter *parameter = &cxt->pbj_parameters[i];
		qb_pbj_address *destination = &parameter->destination;
		if(destination->register_id == address->register_id) {
			if(destination->dimension > 1) {
				return parameter;
			} else {
				// see if the address is pointing to a channel used by the parameter
				uint32_t j, k;
				for(j = 0; j < address->channel_count; j++) {
					uint32_t channel = address->channels[j];
					for(k = 0; k < destination->channel_count; k++) {
						if(channel == destination->channels[k]) {
							return parameter;
						}
					}
				}
			}
		}
	}
	return NULL;
}

static qb_pbj_texture * ZEND_FASTCALL qb_pbj_find_texture_by_id(qb_compiler_context *cxt, uint32_t image_id) {
	uint32_t i;
	for(i = 0; i < cxt->pbj_texture_count; i++) {
		qb_pbj_texture *texture = &cxt->pbj_textures[i];
		if(texture->image_id == image_id) {
			return texture;
		}
	}
	return NULL;
}

static void ZEND_FASTCALL qb_pbj_print_address(qb_compiler_context *cxt, qb_pbj_address *address) {
	if(address->dimension) {
		qb_pbj_parameter *parameter = qb_pbj_find_parameter_by_address(cxt, address);
		if(parameter) {
			if(parameter->destination.channel_count == 1) {
				// don't show channels when it's a scalar
				php_printf("%s ", parameter->name);
				return;
			} else {
				php_printf("%s.", parameter->name);
			}
		} else {
			if(address->register_id & PBJ_REGISTER_INT) {
				php_printf("i%d.", address->register_id & ~PBJ_REGISTER_INT);
			} else {
				php_printf("f%d.", address->register_id);
			}
		}
		if(address->dimension == 1) {
			static const char *rgba[] = { "r", "g", "b", "a" };
			uint32_t i;
			for(i = 0; i < address->channel_count; i++) {
				php_printf("%s", rgba[address->channels[i]]);
			}
		} else {
			static const char *matrix[] = { "m2x2", "m3x3", "m4x4" };
			php_printf("%s", matrix[address->dimension - 2]);
		}
		php_printf(" ");
	}
}

extern const char compressed_table_pbj_op_names[];

static zend_always_inline const char * qb_pbj_get_op_name(qb_compiler_context *cxt, uint32_t opcode) {
	if(!cxt->pool->pbj_op_names) {
		qb_uncompress_table(compressed_table_pbj_op_names, (void ***) &cxt->pool->pbj_op_names, NULL, 0);
		if(!cxt->pool->pbj_op_names) {
			qb_abort("Unable to decompress table");
		}
	}
	return cxt->pool->pbj_op_names[opcode];
}

static void ZEND_FASTCALL qb_pbj_print_op(qb_compiler_context *cxt, qb_pbj_op *pop, uint32_t pop_index) {
	const char *op_name = qb_pbj_get_op_name(cxt, pop->opcode);
	php_printf("%04d: %s ", pop_index, op_name);

	if(pop->opcode == PBJ_SAMPLE_NEAREST || pop->opcode == PBJ_SAMPLE_BILINEAR) {
		qb_pbj_texture *texture = qb_pbj_find_texture_by_id(cxt, pop->image_id);
		if(texture) {
			php_printf("%s ", texture->name);
		} else {
			php_printf("t%d ", pop->image_id);
		}
	}
	if(pop->opcode == PBJ_LOAD_CONSTANT) {
		if(pop->constant.type == PBJ_TYPE_INT) {
			php_printf("%d ", pop->constant.int_value);
		} else {
			php_printf("%G ", pop->constant.float_value);
		}
	} else {
		if(pop->destination_origin) {
			qb_pbj_print_address(cxt, pop->destination_origin);
		}
		if(pop->source_origin) {
			qb_pbj_print_address(cxt, pop->source_origin);
		} else {
			qb_pbj_print_address(cxt, &pop->source);
		}
	}
	if(pop->opcode == PBJ_SELECT || pop->opcode == PBJ_SMOOTH_STEP) {
		qb_pbj_op *data_pop = pop + 1;
		qb_pbj_print_address(cxt, &data_pop->source2);
		qb_pbj_print_address(cxt, &data_pop->source3);
	}
	if(!(pop->opcode >= PBJ_IF && pop->opcode <= PBJ_END_IF)) {
		if(pop->eventual_destination) {
			qb_pbj_print_address(cxt, pop->eventual_destination);
		} else {
			qb_pbj_print_address(cxt, &pop->destination);
		}
	}
	php_printf("\n");
}

static void ZEND_FASTCALL qb_pbj_print_ops(qb_compiler_context *cxt) {
	uint32_t i;
	for(i = 0; i < cxt->pbj_op_count; i++) {
		qb_pbj_op *pop = &cxt->pbj_ops[i];
		qb_pbj_print_op(cxt, pop, i);
	}
}

static void ZEND_FASTCALL qb_pbj_read_value(qb_compiler_context *cxt, uint32_t type, qb_pbj_value *value) {
	uint32_t i;
	value->type = type;
	switch(type) {
		case PBJ_TYPE_FLOAT: {
			value->float1 = qb_pbj_read_F32(cxt);
		}	break;
		case PBJ_TYPE_FLOAT2: {
			for(i = 0; i < 2; i++) {
				value->float2[i] = qb_pbj_read_F32(cxt);
			}
		}	break;
		case PBJ_TYPE_FLOAT3: {
			for(i = 0; i < 3; i++) {
				value->float3[i] = qb_pbj_read_F32(cxt);
			}
		}	break;
		case PBJ_TYPE_FLOAT4: {
			for(i = 0; i < 4; i++) {
				value->float4[i] = qb_pbj_read_F32(cxt);
			}
		}	break;
		case PBJ_TYPE_FLOAT2X2: {
			for(i = 0; i < 4; i++) {
				value->float2x2[i] = qb_pbj_read_F32(cxt);
			}
		}	break;
		case PBJ_TYPE_FLOAT3X3: {
			for(i = 0; i < 9; i++) {
				value->float3x3[i] = qb_pbj_read_F32(cxt);
			}
		}	break;
		case PBJ_TYPE_FLOAT4X4: {
			for(i = 0; i < 16; i++) {
				value->float4x4[i] = qb_pbj_read_F32(cxt);
			}
		}	break;
		case PBJ_TYPE_BOOL:
		case PBJ_TYPE_INT: {
			value->int1 = qb_pbj_read_I16(cxt);
		}	break;
		case PBJ_TYPE_BOOL2:
		case PBJ_TYPE_INT2: {
			for(i = 0; i < 2; i++) {
				value->int2[i] = qb_pbj_read_I16(cxt);
			}
		}	break;
		case PBJ_TYPE_BOOL3:
		case PBJ_TYPE_INT3: {
			for(i = 0; i < 3; i++) {
				value->int3[i] = qb_pbj_read_I16(cxt);
			}
		}	break;
		case PBJ_TYPE_BOOL4:
		case PBJ_TYPE_INT4: {
			for(i = 0; i < 4; i++) {
				value->int4[i] = qb_pbj_read_I16(cxt);
			}
		}	break;
		case PBJ_TYPE_STRING: {
			value->string = qb_pbj_read_string(cxt, NULL);
		}	break;
	}
}

static void ZEND_FASTCALL qb_pbj_set_destination_channels(qb_compiler_context *cxt, qb_pbj_address *address, uint32_t channel_mask, uint32_t dimension) {
	// dimension is used only if no destination channels are selected 
	// otherwise dimension is pertinent only to the source
	if(channel_mask) {
		uint32_t channel, bit;
		address->dimension = 1;
		address->channel_count = 0;
		address->all_channels = 0;
		for(bit = 0x08, channel = 0; channel < 4; channel++, bit >>= 1) {
			if(channel_mask & bit) {
				address->channels[address->channel_count++] = channel;
			}
		}
	} else {
		address->dimension = dimension;
		address->channel_count = 0;
	}
}

static void ZEND_FASTCALL qb_pbj_set_source_channels(qb_compiler_context *cxt, qb_pbj_address *address, uint32_t channel_swizzle, uint32_t channel_count, uint32_t dimension) {
	if(dimension == 1) {
		uint32_t channel, i;
		address->dimension = 1;
		address->channel_count = channel_count;
		address->all_channels = 0;
		for(i = 0; i < channel_count; i++) {
			channel = (channel_swizzle >> ((3 - i) * 2)) & 0x03;
			address->channels[i] = channel;
		}
	} else {
		address->dimension = dimension;
		address->channel_count = 0;
	}
}

static qb_pbj_parameter * ZEND_FASTCALL qb_pbj_find_parameter(qb_compiler_context *cxt, const char *name) {
	uint32_t i;
	for(i = 0; i < cxt->pbj_parameter_count; i++) {
		qb_pbj_parameter *parameter = &cxt->pbj_parameters[i];
		if(strcmp(parameter->name, name) == 0) {
			return parameter;
		}
	}
	return NULL;
}

static qb_pbj_texture * ZEND_FASTCALL qb_pbj_find_texture(qb_compiler_context *cxt, const char *name) {
	uint32_t i;
	for(i = 0; i < cxt->pbj_texture_count; i++) {
		qb_pbj_texture *texture = &cxt->pbj_textures[i];
		if(strcmp(texture->name, name) == 0) {
			return texture;
		}
	}
	return NULL;
}

static uint32_t zend_always_inline qb_pbj_match_addresses(qb_compiler_context *cxt, qb_pbj_address *reg1_address, qb_pbj_address *reg2_address) {
	return (reg1_address->register_id == reg2_address->register_id && reg1_address->dimension == reg2_address->dimension 
		 && reg1_address->channel_count == reg2_address->channel_count && reg1_address->all_channels == reg2_address->all_channels);
}

#define PBJ_ADDRESS_EQUAL						1
#define PBJ_ADDRESS_EXACT_OVERLAP				2
#define PBJ_ADDRESS_SECOND_CONTAINS_FIRST		3
#define PBJ_ADDRESS_FIRST_CONTAINS_SECOND		4
#define PBJ_ADDRESS_OVERLAP						5

static uint32_t ZEND_FASTCALL qb_pbj_get_channel_mask(qb_compiler_context *cxt, qb_pbj_address *reg_address) {
	uint32_t mask;
	if(reg_address->dimension == 1) {
		uint32_t i, channel;
		for(i = 0, mask = 0x00; i < reg_address->channel_count; i++) {
			channel = reg_address->channels[i];
			mask |= 1 << channel;
		}
	} else if(reg_address->dimension == 2) {
		mask = 0x0000000F;	// 4 bits set
	} else if(reg_address->dimension == 3) {
		mask = 0x00000FFF;	// 12 bits set
	} else if(reg_address->dimension == 4) {
		mask = 0x0000FFFF;	// 16 bits set
	}
	return mask;
}

static int32_t ZEND_FASTCALL qb_pbj_find_channel_overlap(qb_compiler_context *cxt, qb_pbj_address *reg1_address, qb_pbj_address *reg2_address) {
	uint32_t channel_mask1 = qb_pbj_get_channel_mask(cxt, reg1_address);
	uint32_t channel_mask2 = qb_pbj_get_channel_mask(cxt, reg2_address);
	if(channel_mask1 & channel_mask2) {
		if(channel_mask1 == channel_mask2) {
			return PBJ_ADDRESS_EXACT_OVERLAP;
		} else if((channel_mask2 & ~channel_mask1) == 0) {
			// reg1_address has all the channels of reg2_address
			return PBJ_ADDRESS_FIRST_CONTAINS_SECOND;
		} else if((channel_mask2 & ~channel_mask1) == 0) {
			// reg2_address has all the channels of reg1_address
			return PBJ_ADDRESS_SECOND_CONTAINS_FIRST;
		}
		return PBJ_ADDRESS_OVERLAP;
	}
	return 0;
}

static int32_t ZEND_FASTCALL qb_pbj_find_address_overlap(qb_compiler_context *cxt, qb_pbj_address *reg1_address, qb_pbj_address *reg2_address) {
	// see if the they're of the same type
	if((reg1_address->register_id & PBJ_REGISTER_INT) == (reg2_address->register_id & PBJ_REGISTER_INT)) {
		// see if the dimension matches
		if(reg1_address->dimension == reg2_address->dimension) {
			if(reg1_address->dimension == 1) {
				// vector
				if(reg1_address->register_id == reg2_address->register_id) {
					if(reg1_address->all_channels == reg2_address->all_channels && reg1_address->channel_count == reg2_address->channel_count) {
						return PBJ_ADDRESS_EQUAL;
					} else {
						return qb_pbj_find_channel_overlap(cxt, reg1_address, reg2_address);
					}
				}
			} else {
				// matrices don't overlap--it's exact match or none
				if(reg1_address->register_id == reg2_address->register_id) {
					return PBJ_ADDRESS_EQUAL;
				}
			}
		} else {
			uint32_t reg1_start = reg1_address->register_id;
			uint32_t reg1_end = reg1_start + pbj_matrix_sizes[reg1_address->dimension];
			uint32_t reg2_start = reg2_address->register_id;
			uint32_t reg2_end = reg2_start + pbj_matrix_sizes[reg2_address->dimension];

			if(!(reg2_end <= reg1_start || reg1_end <= reg2_start)) {
				if(reg1_start < reg2_start && reg2_end < reg1_end) {
					return PBJ_ADDRESS_FIRST_CONTAINS_SECOND;
				} else if(reg2_start < reg1_start && reg1_end < reg2_end) {
					return PBJ_ADDRESS_SECOND_CONTAINS_FIRST;
				} else if(reg1_start == reg2_start && reg1_end == reg2_end) {
					// only possible if one is a float4 and the other a float2x2
					return qb_pbj_find_channel_overlap(cxt, reg1_address, reg2_address);
				} else {
					return PBJ_ADDRESS_OVERLAP;
				}
			}
		}
	}
	return 0;
}

static qb_pbj_register * ZEND_FASTCALL qb_pbj_obtain_registers(qb_compiler_context *cxt, uint32_t id, uint32_t count) {
	qb_pbj_register *reg;
	if(id & PBJ_REGISTER_INT) {
		uint32_t index = id & ~PBJ_REGISTER_INT;
		if(index + count > cxt->pbj_int_register_count) {
			qb_enlarge_array((void **) &cxt->pbj_int_registers, index + count - cxt->pbj_int_register_count);
		} 
		reg = &cxt->pbj_int_registers[index];
	} else {
		uint32_t index = id;
		if(index >= cxt->pbj_float_register_count) {
			qb_enlarge_array((void **) &cxt->pbj_float_registers, index + count - cxt->pbj_float_register_count);
		}
		reg = &cxt->pbj_float_registers[index];
	}
	return reg;
}

static zend_always_inline qb_pbj_register * qb_pbj_obtain_register(qb_compiler_context *cxt, uint32_t id) {
	return qb_pbj_obtain_registers(cxt, id, 1);
}

static qb_address * ZEND_FASTCALL qb_pbj_obtain_channel_address(qb_compiler_context *cxt, uint32_t id, uint32_t channel, uint32_t channel_count) {
	qb_pbj_register *reg = qb_pbj_obtain_register(cxt, id);
	uint32_t access_type = PBJ_CHANNEL_ACCESS_TYPE(channel, channel_count);
#if ZEND_DEBUG
	if(channel + channel_count > 4) {
		qb_abort("Invalid channel");
	}
#endif
	if(!reg->channel_addresses[access_type]) {
		qb_address *channel_address = qb_allocate_address(cxt->pool);
		channel_address->type = (id & PBJ_REGISTER_INT) ? QB_TYPE_I32 : QB_TYPE_F32;
		//channel_address->segment_selector = 0xFFFFFFFF;
		channel_address->segment_offset = ((id & ~PBJ_REGISTER_INT) * 4 + channel) * sizeof(float32_t);
		channel_address->flags = QB_ADDRESS_TEMPORARY;
		if(channel_count > 1) {
			channel_address->mode = QB_ADDRESS_MODE_ARR;
			channel_address->dimension_count = 1;
			channel_address->array_size_address = cxt->pbj_int_numerals[channel_count];
			channel_address->array_size_addresses =
			channel_address->dimension_addresses = &channel_address->array_size_address;
		} else {
			channel_address->mode = QB_ADDRESS_MODE_VAR;
		}
		// indicate that the register is accessed in this manner
		reg->channel_usage |= (1 << access_type);
		reg->channel_addresses[access_type] = channel_address;
	}
	return reg->channel_addresses[access_type];
}

static qb_address * ZEND_FASTCALL qb_pbj_obtain_matrix_address(qb_compiler_context *cxt, uint32_t id, uint32_t dimension) {
	uint32_t row_count, col_count, reg_count, i;
	qb_pbj_register *reg;

	if(dimension == 2) {
		// a 2x2 matrix fits into a single register
		row_count = 2;
		col_count = 2;
		reg_count = 1;
	} else if(dimension == 3) {
		// 3x3 matrices have an extra padding element in each row
		row_count = 3;
		col_count = 4;
		reg_count = 3;
	} else if(dimension == 4) {
		row_count = 4;
		col_count = 4;
		reg_count = 4;
	}
	reg = qb_pbj_obtain_registers(cxt, id, reg_count);
	if(!reg->matrix_address) {
		qb_address *matrix_address = qb_allocate_address(cxt->pool);
		matrix_address->type = (id & PBJ_REGISTER_INT) ? QB_TYPE_I32 : QB_TYPE_F32;
		matrix_address->flags = QB_ADDRESS_TEMPORARY;
		matrix_address->mode = QB_ADDRESS_MODE_ARR;
		//matrix_address->segment_selector = 0xFFFFFFFF;
		matrix_address->segment_offset = (id & ~PBJ_REGISTER_INT) * 4 * sizeof(float32_t);
		matrix_address->dimension_count = 2;
		matrix_address->array_size_addresses = qb_allocate_pointers(cxt->pool, 2);
		matrix_address->dimension_addresses = qb_allocate_pointers(cxt->pool, 2);
		matrix_address->array_size_address =
		matrix_address->array_size_addresses[0] = cxt->pbj_int_numerals[row_count * col_count];
		matrix_address->dimension_addresses[0] = cxt->pbj_int_numerals[row_count];
		matrix_address->array_size_addresses[1] =
		matrix_address->dimension_addresses[1] = cxt->pbj_int_numerals[col_count];
		reg->matrix_address = matrix_address;

		for(i = 0; i < reg_count; i++) {
			// indicate that the registers are used as a matrix
			reg->matrix_usage = PBJ_MATRIX_ACCESS_TYPE(dimension, i);
			reg++;
		}
	}
	return reg->matrix_address;
}

static void ZEND_FASTCALL qb_pbj_allocate_registers(qb_compiler_context *cxt, qb_pbj_address *reg_address) {
	if(reg_address->dimension == 1) {
		uint32_t i;
		uint32_t channel_count = 1;
		uint32_t channel_start = reg_address->channels[0];
		for(i = 1; i < reg_address->channel_count; i++) {
			if(reg_address->channels[i - 1] + 1 != reg_address->channels[i]) {
				qb_pbj_obtain_channel_address(cxt, reg_address->register_id, channel_start, channel_count);
				channel_start = reg_address->channels[i];
				channel_count = 1;
			}
			qb_pbj_obtain_channel_address(cxt, reg_address->register_id, channel_start, channel_count);
		}
		qb_pbj_obtain_channel_address(cxt, reg_address->register_id, channel_start, channel_count);
	} else if(reg_address->dimension > 1) {
		qb_pbj_obtain_matrix_address(cxt, reg_address->register_id, reg_address->dimension);
	}
}

static qb_address * ZEND_FASTCALL qb_pbj_map_registers_to_memory(qb_compiler_context *cxt, qb_pbj_register *regs, uint32_t reg_count, uint32_t type) {
	qb_address *base_address = qb_create_fixed_length_array(cxt, type, reg_count * 4, QB_CREATE_IN_VARIABLE_SEGMENT);
	qb_address *matrix_address = NULL, *channel_address;
	uint32_t i, j, k;
	for(i = 0; i < reg_count; i++) {
		qb_pbj_register *reg = &regs[i];
		if(reg->matrix_usage) {
			if(reg->matrix_address) {
				matrix_address = reg->matrix_address;
				matrix_address->segment_selector = base_address->segment_selector;
				matrix_address->segment_offset += base_address->segment_offset;
				qb_add_array(cxt, matrix_address);
			} else {
				// the register is used by a matrix that began at a previous register
			}
		} else {
			matrix_address = NULL;
		}

		if(reg->channel_usage) {
			if(reg->channel_usage == PBJ_USE_RGBA) {
				// the register is only accessed as an RGBA only
				channel_address = reg->channel_addresses[PBJ_CHANNEL_RGBA];
				channel_address->segment_selector = base_address->segment_selector;
				channel_address->segment_offset += base_address->segment_offset;

				// attach the matrix address as the source address so we know
				// that this address isn't an independent variable
				if(matrix_address) {
					channel_address->source_address = matrix_address;
				} else {
					qb_add_array(cxt, channel_address);
				}
			} else {
				// things get more complicated when the register might be accessible through multiple addresses
				if(!(reg->channel_usage & PBJ_USE_RGBA)) {
					// ensure partially overlapping addresses can have a common parent
					// create the full RGBA address if necessary
					int32_t create_rgba = FALSE;
					if(reg->channel_usage & PBJ_USE_RGB) {
						create_rgba = (reg->channel_usage & (PBJ_USE_GBA | PBJ_USE_BA));
					} else if(reg->channel_usage & PBJ_USE_GBA) {
						create_rgba = (reg->channel_usage & PBJ_USE_RG);
					} else if(reg->channel_usage & PBJ_USE_RG) {
						create_rgba = (reg->channel_usage & PBJ_USE_GB);
					} else if(reg->channel_usage & PBJ_USE_GB) {
						create_rgba = (reg->channel_usage & PBJ_USE_BA);
					}
					if(create_rgba) {
						qb_pbj_obtain_channel_address(cxt, i, 0, 4);
					}
				}

				// update the position of all the channel addresses
				for(j = PBJ_CHANNEL_RGBA; (int32_t) j >= 0; j--) {
					if(reg->channel_usage & (1 << j)) {
						channel_address = reg->channel_addresses[j];
						channel_address->segment_selector = base_address->segment_selector;
						channel_address->segment_offset += base_address->segment_offset;

						if(matrix_address) {
							channel_address->source_address = matrix_address;
						} else {
							// see if the address is contained by another
							for(k = PBJ_CHANNEL_RGBA; k > j; k--) {
								if(reg->channel_usage & (1 << k)) {
									uint32_t channel_start1 = PBJ_CHANNEL_ACCESS_INDEX(j);
									uint32_t channel_count1 = PBJ_CHANNEL_ACCESS_COUNT(j);
									uint32_t channel_start2 = PBJ_CHANNEL_ACCESS_INDEX(k);
									uint32_t channel_count2 = PBJ_CHANNEL_ACCESS_COUNT(k);
									if(channel_start2 <= channel_start1 && (channel_start1 + channel_count1) <= (channel_start2 + channel_count2)) {
										channel_address->source_address = reg->channel_addresses[k];
										break;
									}
								}
							}
							if(!channel_address->source_address) {
								if(IS_SCALAR(channel_address)) {
									qb_add_scalar(cxt, channel_address);
								} else {
									qb_add_array(cxt, channel_address);
								}
							}
						}
					}
				}
			}
		}
	}
	return base_address;
}

static qb_address * ZEND_FASTCALL qb_pbj_obtain_value_address(qb_compiler_context *cxt, qb_pbj_value *value) {
	qb_address *address = NULL;
	int32_t *int_values = NULL;
	float32_t *float_values;
	uint32_t i, j, value_count;

	switch(value->type) {
		case PBJ_TYPE_BOOL:
		case PBJ_TYPE_INT: return qb_obtain_constant_S32(cxt, value->int1);
		case PBJ_TYPE_FLOAT: return qb_obtain_constant_F32(cxt, value->float1);
		case PBJ_TYPE_INT2: {
			value_count = 2;
			int_values = value->int2;
		}	break;
		case PBJ_TYPE_INT3: {
			value_count = 3;
			int_values = value->int3;
		}	break;
		case PBJ_TYPE_INT4: {
			value_count = 4;
			int_values = value->int4;
		}	break;
		case PBJ_TYPE_FLOAT2: {
			value_count = 2;
			float_values = value->float2;
		}	break;
		case PBJ_TYPE_FLOAT3: {
			value_count = 3;
			float_values = value->float3;
		}	break;
		case PBJ_TYPE_FLOAT4: {
			value_count = 4;
			float_values = value->float4;
		}	break;
		case PBJ_TYPE_FLOAT2X2: {
			value_count = 4;
			float_values = value->float2x2;
		}	break;
		case PBJ_TYPE_FLOAT3X3: {
			value_count = 9;
			float_values = value->float3x3;
		}	break;
		case PBJ_TYPE_FLOAT4X4: {
			value_count = 16;
			float_values = value->float4x4;
		}	break;
		default:
			qb_abort("Invalid type id: %d", value->type);
	}

	if(int_values) {
		address = qb_create_fixed_length_array(cxt, QB_TYPE_S32, value_count, QB_CREATE_IN_VARIABLE_SEGMENT);
		address->flags |= QB_ADDRESS_CONSTANT;
		for(i = 0; i < value_count; i++) {
			ARRAY(S32, address)[i] = int_values[i];
		}
	} else if(float_values) {
		if(value_count == 9) {
			address = qb_create_fixed_length_array(cxt, QB_TYPE_F32, 12, QB_CREATE_IN_VARIABLE_SEGMENT);
			address->flags |= QB_ADDRESS_CONSTANT;
			for(i = 0, j = 0; i < value_count; i++) {
				ARRAY(F32, address)[j] = float_values[j];
				j++;
				if(j == 3) {
					ARRAY(F32, address)[j] = 0;
					j++;
				}
			}
		} else {
			address = qb_create_fixed_length_array(cxt, QB_TYPE_F32, value_count, QB_CREATE_IN_VARIABLE_SEGMENT);
			address->flags |= QB_ADDRESS_CONSTANT;
			for(i = 0; i < value_count; i++) {
				ARRAY(F32, address)[i] = float_values[i];
			}
		}
	}
	return address;
}

static qb_address * ZEND_FASTCALL qb_pbj_obtain_address(qb_compiler_context *cxt, qb_pbj_address *reg_address) {
	if(reg_address->dimension > 1) {
		return qb_pbj_obtain_matrix_address(cxt, reg_address->register_id, reg_address->dimension);
	} else if(reg_address->channel_count == 0) {
		qb_pbj_constant *constant = (qb_pbj_constant *) reg_address;
		if(constant->type == PBJ_TYPE_INT) {
			return qb_obtain_constant_U32(cxt, constant->int_value);
		} else {
			return qb_obtain_constant_F32(cxt, constant->float_value);
		}
	} else {
		return qb_pbj_obtain_channel_address(cxt, reg_address->register_id, reg_address->channels[0], reg_address->channel_count);
	}
}

static qb_address * ZEND_FASTCALL qb_pbj_obtain_temporary_vector(qb_compiler_context *cxt, qb_pbj_address_bundle *bundle) {
	qb_address *address;
	if(bundle->address_count == 1 && (!IS_SCALAR(bundle->addresses[0]) || bundle->channel_counts[0] == 1)) {
		// there's one address and it's a vector (or is supposed to be a scalar)
		address = bundle->addresses[0];
		bundle->temporary_address = NULL;
	} else {
		// the channels are not in order then we need to use a temporary variable
		uint32_t i, channel, type = bundle->addresses[0]->type;
		address = qb_obtain_temporary_fixed_length_array(cxt, type, bundle->total_channel_count);
		qb_add_reference(cxt, address);
		if(bundle->access == PBJ_READ_SOURCE) {
			for(i = 0, channel = 0; i < bundle->address_count; i++, channel += bundle->channel_counts[i]) {
				qb_address *offset_address = cxt->pbj_int_numerals[channel];
				qb_address *length_address = cxt->pbj_int_numerals[bundle->channel_counts[i]];
				qb_address *source_address = bundle->addresses[i];
				qb_address *channel_address = qb_get_array_slice(cxt, address, offset_address, length_address);
				qb_create_unary_op(cxt, &factory_copy, source_address, channel_address);
			}
		}
		bundle->temporary_address = address;
	}
	return address;
}

static void ZEND_FASTCALL qb_pbj_release_temporary_vector(qb_compiler_context *cxt, qb_pbj_address_bundle *bundle) {
	if(bundle->temporary_address) {
		if(bundle->access == PBJ_WRITE_DESTINATION) {
			// copy the result to the proper location
			uint32_t i, channel;
			for(i = 0, channel = 0; i < bundle->address_count; i++, channel += bundle->channel_counts[i]) {
				qb_address *offset_address = cxt->pbj_int_numerals[channel];
				qb_address *length_address = cxt->pbj_int_numerals[bundle->channel_counts[i]];
				qb_address *destination_address = bundle->addresses[i];
				qb_address *channel_address = qb_get_array_slice(cxt, bundle->temporary_address, offset_address, length_address);
				qb_create_unary_op(cxt, &factory_copy, channel_address, destination_address);
			}
		}
		qb_remove_reference(cxt, bundle->temporary_address);
	} 
}

static void ZEND_FASTCALL qb_pbj_create_address_bundles(qb_compiler_context *cxt, qb_pbj_address **inputs, uint32_t input_count, qb_pbj_address *output, qb_pbj_address_bundle *input_bundles, qb_pbj_address_bundle *output_bundle) {
	uint32_t current_channel, previous_channel;
	uint32_t group_count = 0, group_channel_index, group_channel_count, group_channel_indices[4], group_channel_counts[4];
	uint32_t input_use_scalar[3][3];
	uint32_t i, j, channel_count, vector_count = 0;
	qb_pbj_address *input;
	qb_pbj_address_bundle *input_bundle;

	// handle constant, scalar, and matrix inputs first
	for(j = 0; j < input_count; j++) {
		input = inputs[j];
		input_bundle = &input_bundles[j];
		if(input->dimension > 1 || input->channel_count <= 1) {
			input_bundle->addresses[0] = qb_pbj_obtain_address(cxt, input);
			input_bundle->address_count = input_bundle->distinct_address_count = 1;
			input_bundle->channel_counts[0] = 1;
		} else {
			// vector
			vector_count++;
			input_use_scalar[j][0] = 0;
			input_use_scalar[j][1] = 0;
			input_use_scalar[j][2] = 0;
			input_bundle->distinct_address_count = 0;
			channel_count = input->channel_count;
		}
		input_bundle->access = PBJ_READ_SOURCE;
	}
	if(output) {
		if(output->dimension > 1 || output->channel_count <= 1) {
			output_bundle->addresses[0] = qb_pbj_obtain_address(cxt, output);
			output_bundle->address_count = output_bundle->distinct_address_count = 1;
			output_bundle->channel_counts[0] = 1;
		} else {
			output_bundle->distinct_address_count = 0;
			vector_count++;
			channel_count = output->channel_count;
		}
		output_bundle->access = PBJ_WRITE_DESTINATION;
	}

	if(vector_count > 0) {
		// vectors can spawn multiple addresses, as the channels can be out of order 
		// due to masking (output) or swizzling (input)
		// a swizzled input can have one channel appearing multiple times
		// try to group the channels so that adjacent ones are handled in one operation

		group_channel_count = 1;
		group_channel_indices[0] = 0;
		for(i = 1; i < channel_count; i++) {
			int32_t new_group = FALSE;
			if(output) {
				if(output_bundle->distinct_address_count == 0) {
					current_channel = output->channels[i];
					previous_channel = output->channels[i - 1];
					if(current_channel != previous_channel + 1) {
						// channels are not sequential and hence cannot be merged into a single array address
						new_group = TRUE;
					}
				}
			}
			for(j = 0; j < input_count; j++) {
				input = inputs[j];
				input_bundle = &input_bundles[j];
				if(input_bundle->distinct_address_count == 0) {
					current_channel = input->channels[i];
					previous_channel = input->channels[i - 1];
					if(input_use_scalar[j][group_count]) {
						// channel is no longer the same as previous one
						if(current_channel != previous_channel) {
							new_group = TRUE;
						}
					} else {
						if(current_channel != previous_channel + 1) {
							// if it's the same channel as the previous one, combine the two into scalar
							if(input->channels[i] == input->channels[i - 1]) {
								input_use_scalar[j][group_count] = TRUE;
							} else {
								new_group = TRUE;
							}
						}
					}
				}
			}
			if(new_group) {
				group_channel_counts[group_count++] = group_channel_count;
				group_channel_indices[group_count] = i;
				group_channel_count = 1;
			} else {
				group_channel_count++;
			}
		}
		group_channel_counts[group_count++] = group_channel_count;

		for(j = 0; j < input_count; j++) {
			input = inputs[j];
			input_bundle = &input_bundles[j];
			if(input_bundle->distinct_address_count == 0) {
				// get the addresses
				for(i = 0; i < group_count; i++) {
					group_channel_count = group_channel_counts[i];
					group_channel_index = group_channel_indices[i];
					input_bundle->addresses[i] = qb_pbj_obtain_channel_address(cxt, input->register_id, input->channels[group_channel_index], (input_use_scalar[j][i]) ? 1 : group_channel_count);
					input_bundle->channel_counts[i] = group_channel_count;
				}
				input_bundle->address_count = input_bundle->distinct_address_count = group_count;
			} else {
				// replicate the same address
				for(i = 1; i < group_count; i++) {
					input_bundle->addresses[i] = input_bundle->addresses[0];
					input_bundle->channel_counts[i] = group_channel_counts[i];
				}
				input_bundle->address_count = group_count;
			}
			input_bundle->total_channel_count = channel_count;
		}
		if(output) {
			if(output_bundle->distinct_address_count == 0) {
				for(i = 0; i < group_count; i++) {
					group_channel_count = group_channel_counts[i];
					group_channel_index = group_channel_indices[i];
					output_bundle->addresses[i] = qb_pbj_obtain_channel_address(cxt, output->register_id, output->channels[group_channel_index], group_channel_count);
					output_bundle->channel_counts[i] = group_channel_count;
				}
				output_bundle->address_count = output_bundle->distinct_address_count = group_count;
			} else {
				for(i = 1; i < group_count; i++) {
					output_bundle->addresses[i] = output_bundle->addresses[0];
					output_bundle->channel_counts[i] = group_channel_counts[i];
				}
				output_bundle->address_count = group_count;
			}
			output_bundle->total_channel_count = channel_count;
		}
	} 
}

static void zend_always_inline qb_pbj_create_input_bundle(qb_compiler_context *cxt, qb_pbj_address *input, qb_pbj_address_bundle *input_bundle) {
	qb_pbj_create_address_bundles(cxt, &input, 1, NULL, input_bundle, NULL);
}

static void zend_always_inline qb_pbj_create_output_bundle(qb_compiler_context *cxt, qb_pbj_address *output, qb_pbj_address_bundle *output_bundle) {
	qb_pbj_create_address_bundles(cxt, NULL, 0, output, NULL, output_bundle);
}

static void ZEND_FASTCALL qb_pbj_translate_copy(qb_compiler_context *cxt, qb_pbj_translator *t, qb_pbj_address **inputs, uint32_t input_count, qb_pbj_address *output) {
	qb_pbj_address_bundle input_bundle, output_bundle;
	qb_address *op_address, *result_address;
	uint32_t i;

	qb_pbj_create_address_bundles(cxt, inputs, 1, output, &input_bundle, &output_bundle);
	for(i = 0; i < output_bundle.address_count; i++) {
		op_address = input_bundle.addresses[i];
		result_address = output_bundle.addresses[i];
		qb_create_unary_op(cxt, &factory_copy, op_address, result_address);
	}
}

static void ZEND_FASTCALL qb_pbj_translate_bool(qb_compiler_context *cxt, qb_pbj_translator *t, qb_pbj_address **inputs, uint32_t input_count, qb_pbj_address *output) {
	qb_pbj_address_bundle input_bundle, output_bundle;
	qb_address *zero_address = (inputs[0]->register_id & PBJ_REGISTER_INT) ? cxt->pbj_int_numerals[0] : cxt->pbj_float_numerals[0];
	uint32_t i;

	qb_pbj_create_address_bundles(cxt, inputs, 1, output, &input_bundle, &output_bundle);
	for(i = 0; i < output_bundle.address_count; i++) {
		qb_create_binary_op(cxt, t->extra, input_bundle.addresses[i], zero_address, output_bundle.addresses[i]);
	}
}

static void ZEND_FASTCALL qb_pbj_translate_reciprocal(qb_compiler_context *cxt, qb_pbj_translator *t, qb_pbj_address **inputs, uint32_t input_count, qb_pbj_address *output) {
	qb_pbj_address_bundle input_bundle, output_bundle;
	uint32_t i;

	qb_pbj_create_address_bundles(cxt, inputs, 1, output, &input_bundle, &output_bundle);
	for(i = 0; i < output_bundle.address_count; i++) {
		// pbj_float_numerals[2] is 1, cxt->pbj_float_numerals[1] is 0.5
		qb_create_binary_op(cxt, t->extra, cxt->pbj_float_numerals[2], input_bundle.addresses[i], output_bundle.addresses[i]);
	}
}

static void ZEND_FASTCALL qb_pbj_translate_basic_op(qb_compiler_context *cxt, qb_pbj_translator *t, qb_pbj_address **inputs, uint32_t input_count, qb_pbj_address *output) {
	qb_pbj_address_bundle input_bundles[2], output_bundle;
	qb_operand operands[2], result;
	uint32_t i, j;

	qb_pbj_create_address_bundles(cxt, inputs, input_count, output, input_bundles, &output_bundle);
	for(i = 0; i < output_bundle.address_count; i++) {
		for(j = 0; j < input_count; j++) {
			operands[j].type = QB_OPERAND_ADDRESS;
			operands[j].address = input_bundles[j].addresses[i];
		}
		result.type = QB_OPERAND_ADDRESS;
		result.address = output_bundle.addresses[i];
		qb_create_op(cxt, t->extra, operands, input_count, &result);
	}
}

static void ZEND_FASTCALL qb_pbj_translate_basic_vector_op(qb_compiler_context *cxt, qb_pbj_translator *t, qb_pbj_address **inputs, uint32_t input_count, qb_pbj_address *output) {
	qb_pbj_address_bundle input_bundles[2], output_bundle;
	qb_operand operands[2], result;
	uint32_t j;

	for(j = 0; j < input_count; j++) {
		qb_pbj_create_input_bundle(cxt, inputs[j], &input_bundles[j]);
		operands[j].type = QB_OPERAND_ADDRESS;
		operands[j].address = qb_pbj_obtain_temporary_vector(cxt, &input_bundles[j]);
	}

	qb_pbj_create_output_bundle(cxt, output, &output_bundle);
	result.type = QB_OPERAND_ADDRESS;
	result.address = qb_pbj_obtain_temporary_vector(cxt, &output_bundle);

	qb_create_op(cxt, t->extra, operands, input_count, &result);

	for(j = 0; j < input_count; j++) {
		qb_pbj_release_temporary_vector(cxt, &input_bundles[j]);
	}
	qb_pbj_release_temporary_vector(cxt, &output_bundle);
}

static void ZEND_FASTCALL qb_pbj_translate_reverse_vector_op(qb_compiler_context *cxt, qb_pbj_translator *t, qb_pbj_address **inputs, uint32_t input_count, qb_pbj_address *output) {
	qb_pbj_address *reverse_inputs[2];
	reverse_inputs[1] = inputs[0];
	reverse_inputs[0] = inputs[1];
	qb_pbj_translate_basic_vector_op(cxt, t, reverse_inputs, 2, output);
}

static void ZEND_FASTCALL qb_pbj_translate_load_constant(qb_compiler_context *cxt, qb_pbj_translator *t, qb_pbj_address **inputs, uint32_t input_count, qb_pbj_address *output) {
	qb_address *constant_address, *result_address, *expected_result_address = NULL;
	qb_pbj_register *reg;
	uint32_t current_index, previous_index, current_int_flag, int_flag;
	uint32_t i, j, type, constant_count;
	uint32_t expected_constant_count = 1;

	reg = qb_pbj_obtain_register(cxt, output->register_id);
	if(reg->matrix_usage) {
		if(output->channels[0] == 0) {
			uint32_t dimension = PBJ_MATRIX_ACCESS_DIMENSION(reg->matrix_usage);
			uint32_t offset = PBJ_MATRIX_ACCESS_OFFSET(reg->matrix_usage);
			if(offset == 0) {
				if(dimension == 2) {
					expected_constant_count = 4;
				} else if(dimension == 3) {
					expected_constant_count = 9;
				} else if(dimension == 4) {
					expected_constant_count = 16;
				}
				expected_result_address = reg->matrix_address;
			}
		}
	} else if(reg->channel_usage) {
		for(i = PBJ_CHANNEL_RGBA; i > PBJ_CHANNEL_A; i--) {
			if(reg->channel_usage & (1 << i)) {
				uint32_t channel_start = PBJ_CHANNEL_ACCESS_INDEX(i);
				if(channel_start == output->channels[0]) {
					expected_constant_count = PBJ_CHANNEL_ACCESS_COUNT(i);
					expected_result_address = reg->channel_addresses[i];
					break;
				}
			}
		}
	}
	// see if there are other load-constant ops
	constant_count = 1;
	previous_index = (output->register_id & ~PBJ_REGISTER_INT) * 4 + output->channels[0];
	int_flag = output->register_id & PBJ_REGISTER_INT;
	type = (int_flag) ? QB_TYPE_I32 : QB_TYPE_F32;
	for(i = cxt->pbj_op_index + 1; i < cxt->pbj_op_count && constant_count < expected_constant_count; i++) {
		qb_pbj_op *pop = &cxt->pbj_ops[i];
		if(pop->opcode != PBJ_LOAD_CONSTANT) {
			break;
		}
		current_index = (pop->destination.register_id & ~PBJ_REGISTER_INT) * 4 + pop->destination.channels[0];
		current_int_flag = pop->destination.register_id & PBJ_REGISTER_INT;
		if(current_index != previous_index + 1 || current_int_flag != int_flag) {
			break;
		}
		constant_count++;
		previous_index = current_index;

		if(expected_constant_count == 9) {
			// a 3x3 matrix--skip over the padding element
			if(constant_count == 3 || constant_count == 6) {
				previous_index++;
			}
		}
	}
	
	if(constant_count == expected_constant_count && expected_result_address) {
		result_address = expected_result_address;
	} else {
		constant_count = 1;
		result_address = qb_pbj_obtain_channel_address(cxt, output->register_id, output->channels[0], 1);
	}

	if(constant_count > 1) {
		constant_address = qb_create_fixed_length_array(cxt, type, (expected_constant_count == 9) ? 12 : constant_count, QB_CREATE_IN_VARIABLE_SEGMENT);
	} else {
		constant_address = qb_create_scalar(cxt, type);
	}
	constant_address->flags |= QB_ADDRESS_CONSTANT;

	if(int_flag) {
		int32_t *constants = ARRAY(I32, constant_address);
		for(i = cxt->pbj_op_index, j = 0; j < constant_count; i++, j++) {
			constants[j] = cxt->pbj_ops[i].constant.int_value;
		}
	} else {
		float32_t *constants = ARRAY(F32, constant_address);
		for(i = cxt->pbj_op_index, j = 0; j < constant_count; i++, j++) {
			constants[j] = cxt->pbj_ops[i].constant.float_value;
			if(expected_constant_count == 9) {
				// apply padding
				if(i == 2 || i == 5 || i == 8) {
					j++;
					constants[j] = 0;
				}
			}
		}
	}
	qb_create_unary_op(cxt, &factory_copy, constant_address, result_address);

	// jump over ops which are processed already
	cxt->jump_target_index1 = cxt->pbj_op_index + constant_count;
}

static void ZEND_FASTCALL qb_pbj_translate_sample(qb_compiler_context *cxt, qb_pbj_translator *t, qb_pbj_address **inputs, uint32_t input_count, qb_pbj_address *output) {
	qb_pbj_address_bundle output_bundle;
	qb_pbj_texture *texture = qb_pbj_find_texture_by_id(cxt, cxt->pbj_op->image_id);
	qb_pbj_address *coord = inputs[0];
	qb_address *x_address, *y_address, *result_address;

	qb_pbj_create_address_bundles(cxt, NULL, 0, output, NULL, &output_bundle);
	x_address = qb_pbj_obtain_channel_address(cxt, coord->register_id, coord->channels[0], 1);
	y_address = qb_pbj_obtain_channel_address(cxt, coord->register_id, coord->channels[1], 1);
	result_address = qb_pbj_obtain_temporary_vector(cxt, &output_bundle);
	qb_create_ternary_op(cxt, t->extra, texture->address, x_address, y_address, result_address);
	qb_pbj_release_temporary_vector(cxt, &output_bundle);
}

static void ZEND_FASTCALL qb_pbj_translate_select(qb_compiler_context *cxt, qb_pbj_translator *t, qb_pbj_address **inputs, uint32_t input_count, qb_pbj_address *output) {
	qb_pbj_address *condition = inputs[0];
	qb_address *condition_address;
	qb_op *if_qop, *jmp_qop;
	uint32_t if_index, jmp_index, target_t_index, target_f_index, end_index;

	// fall into true block if non-zero, else branch to false block
	condition_address = qb_pbj_obtain_channel_address(cxt, condition->register_id, condition->channels[0], 1);
	if_index = cxt->op_count;
	if_qop = qb_create_branch_op(cxt,  &factory_branch_on_true, QB_INSTRUCTION_NEXT, QB_OP_INDEX_NONE, condition_address);

	target_t_index = cxt->op_count;
	qb_pbj_translate_copy(cxt, t, &inputs[1], 1, output);

	// jump over the false block
	jmp_index = cxt->op_count;
	jmp_qop = qb_create_jump_op(cxt, &factory_jump, QB_OP_INDEX_NONE);

	target_f_index = cxt->op_count;
	qb_pbj_translate_copy(cxt, t, &inputs[2], 1, output);
	end_index = cxt->op_count;

	// set the jump target indices now that we know what they are
	if_qop->operands[1].jump_target_index = (target_f_index - if_index) | QB_INSTRUCTION_OFFSET;
	jmp_qop->operands[0].jump_target_index = (end_index - jmp_index) | QB_INSTRUCTION_OFFSET;

	// flags the ops as jump targets
	cxt->ops[target_t_index]->flags |= QB_OP_JUMP_TARGET;
	cxt->ops[target_f_index]->flags |= QB_OP_JUMP_TARGET;
	cxt->jump_target_index1 = cxt->pbj_op_index + 2;
	cxt->op_translations[cxt->pbj_op_offset + cxt->jump_target_index1] = QB_OP_INDEX_JUMP_TARGET;
}

static void ZEND_FASTCALL qb_pbj_translate_if(qb_compiler_context *cxt, qb_pbj_translator *t, qb_pbj_address **inputs, uint32_t input_count, qb_pbj_address *output) {
	qb_pbj_address *condition = inputs[0];
	qb_address *condition_address;

	// jump to the instruction immediately following the else or end-if if the condition is false
	condition_address = qb_pbj_obtain_channel_address(cxt, condition->register_id, condition->channels[0], 1);
	qb_create_branch_op(cxt, &factory_branch_on_false, cxt->pbj_op_offset + cxt->pbj_op->branch_target_index + 1, QB_INSTRUCTION_NEXT, condition_address);
}

static void ZEND_FASTCALL qb_pbj_translate_else(qb_compiler_context *cxt, qb_pbj_translator *t, qb_pbj_address **inputs, uint32_t input_count, qb_pbj_address *output) {
	// jump over the else block
	qb_create_jump_op(cxt, &factory_jump, cxt->pbj_op_offset + cxt->pbj_op->branch_target_index + 1);
}

static void ZEND_FASTCALL qb_pbj_translate_end_if(qb_compiler_context *cxt, qb_pbj_translator *t, qb_pbj_address **inputs, uint32_t input_count, qb_pbj_address *output) {
	// do nothing
}

#define PBJ_RS				PBJ_READ_SOURCE
#define PBJ_WD				PBJ_WRITE_DESTINATION
#define PBJ_RS_WD			(PBJ_READ_SOURCE | PBJ_WRITE_DESTINATION)
#define PBJ_RS_RD_WD		(PBJ_READ_SOURCE | PBJ_READ_DESTINATION | PBJ_WRITE_DESTINATION)
#define PBJ_RS_RD_WD1		(PBJ_READ_SOURCE | PBJ_READ_DESTINATION | PBJ_WRITE_DESTINATION | PBJ_WRITE_SCALAR)
#define PBJ_RS_RD_WB		(PBJ_READ_SOURCE | PBJ_READ_DESTINATION | PBJ_WRITE_BOOL)
#define PBJ_RS_RS2_RS3_WD	(PBJ_READ_SOURCE | PBJ_READ_SOURCE2 | PBJ_READ_SOURCE3 | PBJ_WRITE_DESTINATION)

static qb_pbj_translator pbj_op_translators[] = {
	{	NULL,										0,					NULL						},	// PBJ_NOP
	{	qb_pbj_translate_basic_op,					PBJ_RS_RD_WD,		&factory_add				},	// PBJ_ADD
	{	qb_pbj_translate_basic_op,					PBJ_RS_RD_WD,		&factory_subtract			},	// PBJ_SUBTRACT
	{	qb_pbj_translate_basic_op,					PBJ_RS_RD_WD,		&factory_multiply			},	// PBJ_MULTIPLY
	{	qb_pbj_translate_reciprocal,				PBJ_RS_WD,			&factory_divide				},	// PBJ_RECIPROCAL
	{	qb_pbj_translate_basic_op,					PBJ_RS_RD_WD,		&factory_divide				},	// PBJ_DIVIDE
	{	qb_pbj_translate_basic_op,					PBJ_RS_RD_WD,		&factory_atan2				},	// PBJ_ATAN2
	{	qb_pbj_translate_basic_op,					PBJ_RS_RD_WD,		&factory_pow				},	// PBJ_POW
	{	qb_pbj_translate_basic_op,					PBJ_RS_RD_WD,		&factory_floor_modulo		},	// PBJ_MOD
	{	qb_pbj_translate_basic_op,					PBJ_RS_RD_WD,		&factory_min				},	// PBJ_MIN
	{	qb_pbj_translate_basic_op,					PBJ_RS_RD_WD,		&factory_max				},	// PBJ_MAX
	{	qb_pbj_translate_basic_op,					PBJ_RS_RD_WD,		&factory_step				},	// PBJ_STEP
	{	qb_pbj_translate_basic_op,					PBJ_RS_WD,			&factory_sin				},	// PBJ_SIN
	{	qb_pbj_translate_basic_op,					PBJ_RS_WD,			&factory_cos				},	// PBJ_COS
	{	qb_pbj_translate_basic_op,					PBJ_RS_WD,			&factory_tan				},	// PBJ_TAN
	{	qb_pbj_translate_basic_op,					PBJ_RS_WD,			&factory_asin				},	// PBJ_ASIN
	{	qb_pbj_translate_basic_op,					PBJ_RS_WD,			&factory_acos				},	// PBJ_ACOS
	{	qb_pbj_translate_basic_op,					PBJ_RS_WD,			&factory_atan				},	// PBJ_ATAN
	{	qb_pbj_translate_basic_op,					PBJ_RS_WD,			&factory_exp				},	// PBJ_EXP
	{	qb_pbj_translate_basic_op,					PBJ_RS_WD,			&factory_exp2				},	// PBJ_EXP2
	{	qb_pbj_translate_basic_op,					PBJ_RS_WD,			&factory_log				},	// PBJ_LOG
	{	qb_pbj_translate_basic_op,					PBJ_RS_WD,			&factory_log2				},	// PBJ_LOG2
	{	qb_pbj_translate_basic_op,					PBJ_RS_WD,			&factory_sqrt				},	// PBJ_SQRT
	{	qb_pbj_translate_basic_op,					PBJ_RS_WD,			&factory_inv_sqrt			},	// PBJ_RSQRT
	{	qb_pbj_translate_basic_op,					PBJ_RS_WD,			&factory_abs				},	// PBJ_ABS
	{	qb_pbj_translate_basic_op,					PBJ_RS_WD,			&factory_sign				},	// PBJ_SIGN
	{	qb_pbj_translate_basic_op,					PBJ_RS_WD,			&factory_floor				},	// PBJ_FLOOR
	{	qb_pbj_translate_basic_op,					PBJ_RS_WD,			&factory_ceil				},	// PBJ_CEIL
	{	qb_pbj_translate_basic_op,					PBJ_RS_WD,			&factory_fract				},	// PBJ_FRACT
	{	qb_pbj_translate_copy,						PBJ_RS_WD,			&factory_copy				},	// PBJ_COPY
	{	qb_pbj_translate_copy,						PBJ_RS_WD,			&factory_copy				},	// PBJ_FLOAT_TO_INT
	{	qb_pbj_translate_copy,						PBJ_RS_WD,			&factory_copy				},	// PBJ_INT_TO_FLOAT
	{	qb_pbj_translate_reverse_vector_op,			PBJ_RS_RD_WD,		&factory_mm_multiply		},	// PBJ_MATRIX_MATRIX_MULTIPLY
	{	qb_pbj_translate_basic_vector_op,			PBJ_RS_RD_WD,		&factory_vm_multiply		},	// PBJ_VECTOR_MATRIX_MULTIPLY
	{	qb_pbj_translate_reverse_vector_op,			PBJ_RS_RD_WD,		&factory_mv_multiply		},	// PBJ_MATRIX_VECTOR_MULTIPLY
	{	qb_pbj_translate_basic_vector_op,			PBJ_RS_WD,			&factory_normalize			},	// PBJ_NORMALIZE
	{	qb_pbj_translate_basic_vector_op,			PBJ_RS_WD,			&factory_length				},	// PBJ_LENGTH
	{	qb_pbj_translate_basic_vector_op,			PBJ_RS_RD_WD1,		&factory_distance			},	// PBJ_DISTANCE
	{	qb_pbj_translate_basic_vector_op,			PBJ_RS_RD_WD1,		&factory_dot_product		},	// PBJ_DOT_PRODUCT
	{	qb_pbj_translate_basic_vector_op,			PBJ_RS_RD_WD,		&factory_cross_product		},	// PBJ_CROSS_PRODUCT
	{	qb_pbj_translate_basic_op,					PBJ_RS_RD_WB,		&factory_equal				},	// PBJ_EQUAL
	{	qb_pbj_translate_basic_op,					PBJ_RS_RD_WB,		&factory_not_equal			},	// PBJ_NOT_EQUAL
	{	qb_pbj_translate_basic_op,					PBJ_RS_RD_WB,		&factory_less_than			},	// PBJ_LESS_THAN
	{	qb_pbj_translate_basic_op,					PBJ_RS_RD_WB,		&factory_less_equal			},	// PBJ_LESS_THAN_EQUAL
	{	qb_pbj_translate_basic_op,					PBJ_RS_WD,			&factory_logical_not		},	// PBJ_LOGICAL_NOT
	{	qb_pbj_translate_basic_op,					PBJ_RS_RD_WD,		&factory_logical_and		},	// PBJ_LOGICAL_AND
	{	qb_pbj_translate_basic_op,					PBJ_RS_RD_WD,		&factory_logical_or			},	// PBJ_LOGICAL_OR
	{	qb_pbj_translate_basic_op,					PBJ_RS_RD_WD,		&factory_logical_xor		},	// PBJ_LOGICAL_XOR
	{	qb_pbj_translate_sample,					PBJ_RS_WD,			&factory_sample_nearest		},	// PBJ_SAMPLE_NEAREST
	{	qb_pbj_translate_sample,					PBJ_RS_WD,			&factory_sample_bilinear	},	// PBJ_SAMPLE_BILINEAR
	{	qb_pbj_translate_load_constant,				PBJ_WD,				NULL,						},	// PBJ_LOAD_CONSTANT
	{	qb_pbj_translate_select,					PBJ_RS_RS2_RS3_WD,	NULL,						},	// PBJ_SELECT
	{	qb_pbj_translate_if,						PBJ_RS,				NULL,						},	// PBJ_IF
	{	qb_pbj_translate_else,						0,					NULL,						},	// PBJ_ELSE
	{	qb_pbj_translate_end_if,					0,					NULL						},	// PBJ_END_IF
	{	qb_pbj_translate_bool,						0,					&factory_not_equal			},	// PBJ_FLOAT_TO_BOOL
	{	qb_pbj_translate_copy,						PBJ_RS_WD,			&factory_copy				},	// PBJ_BOOL_TO_FLOAT
	{	qb_pbj_translate_bool,						0,					&factory_not_equal			},	// PBJ_INT_TO_BOOL
	{	qb_pbj_translate_copy,						PBJ_RS_WD,			&factory_copy				},	// PBJ_BOOL_TO_INT
	{	qb_pbj_translate_basic_vector_op,			PBJ_RS_RD_WB,		&factory_set_equal			},	// PBJ_VECTOR_EQUAL
	{	qb_pbj_translate_basic_vector_op,			PBJ_RS_RD_WB,		&factory_set_not_equal		},	// PBJ_VECTOR_NOT_EQUAL
	{	qb_pbj_translate_basic_vector_op,			PBJ_RS_WD,			&factory_any				},	// PBJ_ANY
	{	qb_pbj_translate_basic_vector_op,			PBJ_RS_WD,			&factory_all				},	// PBJ_ALL
	{	qb_pbj_translate_basic_op,					PBJ_RS_RS2_RS3_WD,	&factory_smooth_step		},	// PBJ_SMOOTH_STEP
};

static void ZEND_FASTCALL qb_pbj_translate_current_instruction(qb_compiler_context *cxt) {
	qb_pbj_translator *t;
	if(cxt->pbj_op->opcode != PBJ_OP_DATA && cxt->pbj_op->opcode != PBJ_NOP) {
		t = &pbj_op_translators[cxt->pbj_op->opcode];
		if(t->translate) {
			qb_pbj_op *pop = cxt->pbj_op;
			uint32_t input_count = 0;
			qb_pbj_address *inputs[3], *output, scalar_destination;

			// create list of input addresses based on what the op wants
			if(t->flags & PBJ_READ_DESTINATION) {
				inputs[input_count++] = (pop->destination_origin) ? pop->destination_origin : &pop->destination;
			}
			if(t->flags & PBJ_READ_SOURCE) {
				inputs[input_count++] = (pop->source_origin) ? pop->source_origin : &pop->source;
				if(t->flags & PBJ_READ_SOURCE2) {
					qb_pbj_op *data_pop = pop + 1;
					inputs[input_count++] = &data_pop->source2;
					if(t->flags & PBJ_READ_SOURCE3) {
						inputs[input_count++] = &data_pop->source3;
					}
				}
			}
			// see where the output goes
			if(t->flags & PBJ_WRITE_DESTINATION) {
				if(pop->eventual_destination) {
					output = pop->eventual_destination;
				} else if(t->flags & PBJ_WRITE_SCALAR) {
					// the op writes to the first channel of the destination only (e.g. dot product)
					scalar_destination.dimension = 1;
					scalar_destination.all_channels = 0;
					scalar_destination.channel_count = 1;
					scalar_destination.channels[0] = pop->destination.channels[0];
					scalar_destination.register_id = pop->destination.register_id;
					output = &scalar_destination;
				} else {
					output = &pop->destination;
				}
			} else if(t->flags & PBJ_WRITE_BOOL) {
				if(pop->eventual_destination) {
					output = pop->eventual_destination;
				} else {
					uint32_t i;
					cxt->pbj_comparison_register.channel_count = pop->source.channel_count;
					cxt->pbj_comparison_register.dimension = 1;
					for(i = 0; i < pop->source.channel_count; i++) {
						cxt->pbj_comparison_register.channels[i] = i;
					}
					cxt->pbj_comparison_register.register_id = 0 | PBJ_REGISTER_INT;
					output = &cxt->pbj_comparison_register;
				}
			}

			cxt->line_number = cxt->pbj_op_index + 1;
			t->translate(cxt, t, inputs, input_count, output);
		}
	}
}

static void ZEND_FASTCALL qb_pbj_decode(qb_compiler_context *cxt) {
	uint32_t opcode, prev_opcode, i;
	float32_t f;
	cxt->pbj_data = (uint8_t *) cxt->external_code;
	cxt->pbj_data_end = cxt->pbj_data + cxt->external_code_length;

	qb_attach_new_array(cxt->pool, (void **) &cxt->pbj_conditionals, &cxt->pbj_conditional_count, sizeof(qb_pbj_op *), 8);
	qb_attach_new_array(cxt->pool, (void **) &cxt->pbj_parameters, &cxt->pbj_parameter_count, sizeof(qb_pbj_parameter), 8);
	qb_attach_new_array(cxt->pool, (void **) &cxt->pbj_textures, &cxt->pbj_texture_count, sizeof(qb_pbj_texture), 8);
	qb_attach_new_array(cxt->pool, (void **) &cxt->pbj_float_registers, &cxt->pbj_float_register_count, sizeof(qb_pbj_register), 256);
	qb_attach_new_array(cxt->pool, (void **) &cxt->pbj_int_registers, &cxt->pbj_int_register_count, sizeof(qb_pbj_register), 256);

	// obtain addresses to integer and float constants
	cxt->pbj_int_numerals = qb_allocate_address_pointers(cxt->pool, 17);
	for(i = 0; i < 17; i++) {
		if(i <= 4 || i == 12 || i == 16) {
			cxt->pbj_int_numerals[i] = qb_obtain_constant_U32(cxt, i);
		}
	}
	cxt->pbj_float_numerals = qb_allocate_address_pointers(cxt->pool, 3);
	for(i = 0, f = 0; i < 3; i++, f += 0.5) {
		cxt->pbj_float_numerals[i] = qb_obtain_constant_F32(cxt, f);
	}

	while(cxt->pbj_data < cxt->pbj_data_end) {
		opcode = qb_pbj_read_I08(cxt);
		if(opcode <= PBJ_ALL) {
			uint32_t bit_fields, channel_mask, channel_swizzle, channel_count, dimension;
			qb_pbj_op *pop;
			uint32_t pop_index;

			if(!cxt->pbj_ops) {				
				// each op is 8 bytes long 
				uint32_t bytes_remaining = (cxt->pbj_data_end - cxt->pbj_data) + 1;
				uint32_t op_count = bytes_remaining / 8;
				if(op_count == 0) {
					return;
				}
				qb_attach_new_array(cxt->pool, (void **) &cxt->pbj_ops, &cxt->pbj_op_count, sizeof(qb_pbj_op), op_count);
			}
			pop_index = cxt->pbj_op_count;
			pop = qb_enlarge_array((void **) &cxt->pbj_ops, 1);
			pop->opcode = opcode;
			pop->source_origin = pop->destination_origin = pop->eventual_destination = NULL;

			// read the destination address
			pop->destination.register_id = qb_pbj_read_I16(cxt);
			bit_fields = qb_pbj_read_I08(cxt);
			channel_mask = bit_fields >> 4;
			channel_count = (bit_fields & 0x03) + 1;
			dimension = ((bit_fields >> 2) & 0x03) + 1;
			qb_pbj_set_destination_channels(cxt, &pop->destination, channel_mask, dimension);

			if(opcode == PBJ_LOAD_CONSTANT) {
				pop->source.channel_count = pop->source.dimension = 0;
				pop->image_id = 0;
				if(pop->destination.register_id & PBJ_REGISTER_INT) {
					pop->constant.int_value = qb_pbj_read_I32(cxt);
					pop->constant.type = PBJ_TYPE_INT;
				} else {			
					pop->constant.float_value = qb_pbj_read_F32(cxt);
					pop->constant.type = PBJ_TYPE_FLOAT;
				}
				// omit allocating registers for constant load operation
				// any constant loaded will be used eventually elsewhere
			} else {
				// read the source
				pop->source.register_id = qb_pbj_read_I16(cxt);
				channel_swizzle = qb_pbj_read_I08(cxt);
				qb_pbj_set_source_channels(cxt, &pop->source, channel_swizzle, channel_count, dimension);

				// read the image id (only used for sampling functions)
				pop->image_id = qb_pbj_read_I08(cxt);

				if(opcode == PBJ_IF) {
					// push the instruction onto the conditional stack
					qb_pbj_op **p_conditional_pop = qb_enlarge_array((void **) &cxt->pbj_conditionals, 1);
					*p_conditional_pop = pop;
					pop->destination.dimension = 0;
					pop->branch_target_index = 0;
					qb_pbj_allocate_registers(cxt, &pop->source);
				} else if(opcode == PBJ_ELSE || opcode == PBJ_END_IF) {
					// update the branch target index of the op at the top of the stack
					qb_pbj_op *related_pop;
					if(cxt->pbj_conditional_count == 0) {
						qb_abort("Unexpected opcode: %02x", opcode);
					}
					related_pop = cxt->pbj_conditionals[cxt->pbj_conditional_count - 1];
					related_pop->branch_target_index = pop_index;
					if(opcode == PBJ_ELSE) {
						// replace the if with the else op
						cxt->pbj_conditionals[cxt->pbj_conditional_count - 1] = pop;
					} else {
						// pop the item off
						cxt->pbj_conditional_count--;
					}
					pop->source.dimension = 0;
					pop->destination.dimension = 0;
					pop->branch_target_index = 0;
				} else if(opcode == PBJ_SELECT) {
					// put the on-true and on-false sources in the next op
					qb_pbj_op *data_pop = qb_enlarge_array((void **) &cxt->pbj_ops, 1);
					data_pop->opcode = PBJ_OP_DATA;
					data_pop->source2.register_id = qb_pbj_read_I16(cxt);
					channel_swizzle = qb_pbj_read_I08(cxt);
					bit_fields = qb_pbj_read_I08(cxt);
					channel_count = (bit_fields >> 6) + 1;
					qb_pbj_set_source_channels(cxt, &data_pop->source2, channel_swizzle, channel_count, dimension);

					data_pop->source3.register_id = qb_pbj_read_I16(cxt);
					channel_swizzle = qb_pbj_read_I08(cxt);
					bit_fields = qb_pbj_read_I08(cxt);
					channel_count = (bit_fields >> 6) + 1;
					qb_pbj_set_source_channels(cxt, &data_pop->source3, channel_swizzle, channel_count, dimension);
					qb_pbj_allocate_registers(cxt, &pop->source);
					qb_pbj_allocate_registers(cxt, &data_pop->source2);
					qb_pbj_allocate_registers(cxt, &data_pop->source3);
					qb_pbj_allocate_registers(cxt, &pop->destination);
				} else {
					qb_pbj_allocate_registers(cxt, &pop->source);
					qb_pbj_allocate_registers(cxt, &pop->destination);
				}
			}
		} else if(opcode == PBJ_KERNEL_METADATA || opcode == PBJ_PARAMETER_METADATA) {
			uint8_t value_type;
			const char *value_name;
			qb_pbj_parameter *last_parameter;
			qb_pbj_value *value = NULL, tmp;
			value_type = qb_pbj_read_I08(cxt);
			value_name = qb_pbj_read_string(cxt, NULL);
			if(opcode == PBJ_PARAMETER_METADATA && cxt->pbj_parameter_count > 0) {
				last_parameter = &cxt->pbj_parameters[cxt->pbj_parameter_count - 1];
				if(strcmp(value_name, "minValue") == 0) {
					value = &last_parameter->min_value;
				} else if(strcmp(value_name, "maxValue") == 0) {
					value = &last_parameter->max_value;
				} else if(strcmp(value_name, "defaultValue") == 0) {
					value = &last_parameter->default_value;
				}
			}
			if(!value) {
				value = &tmp;
			}
			qb_pbj_read_value(cxt, value_type, value);
			if(opcode == PBJ_PARAMETER_METADATA && value_type == PBJ_TYPE_STRING) {
				if(strcmp(value_name, "parameterType") == 0) {
					last_parameter->parameter_type = value->string;
				} else if(strcmp(value_name, "inputSizeName") == 0) {
					last_parameter->input_size_name = value->string;
				} else if(strcmp(value_name, "description") == 0) {
					last_parameter->description = value->string;
				}
			}
		} else if(opcode == PBJ_PARAMETER_DATA) {
			qb_pbj_parameter *parameter = qb_enlarge_array((void **) &cxt->pbj_parameters, 1);
			uint32_t bit_fields, dimension, channel_mask;
			parameter->qualifier = qb_pbj_read_I08(cxt);
			parameter->type = qb_pbj_read_I08(cxt);
			parameter->destination.register_id = qb_pbj_read_I16(cxt);
			bit_fields = qb_pbj_read_I08(cxt);
			switch(parameter->type) {
				case PBJ_TYPE_FLOAT2X2: 
				case PBJ_TYPE_FLOAT3X3: 
				case PBJ_TYPE_FLOAT4X4: 
					dimension = (parameter->type - PBJ_TYPE_FLOAT2X2) + 2;
					channel_mask = 0;
					break;
				default:
					dimension = 1;
					channel_mask = bit_fields & 0x0F;
					break;
			}
			qb_pbj_set_destination_channels(cxt, &parameter->destination, channel_mask, dimension);
			parameter->name = qb_pbj_read_string(cxt, NULL);
			parameter->address = NULL;
			parameter->parameter_type = NULL;
			parameter->description = NULL;
			parameter->input_size_name = NULL;
			memset(&parameter->default_value, 0, sizeof(qb_pbj_value));
			memset(&parameter->min_value, 0, sizeof(qb_pbj_value));
			memset(&parameter->max_value, 0, sizeof(qb_pbj_value));
			qb_pbj_allocate_registers(cxt, &parameter->destination);
		} else if(opcode == PBJ_TEXTURE_DATA) {
			qb_pbj_texture *texture = qb_enlarge_array((void **) &cxt->pbj_textures, 1);
			texture->image_id = qb_pbj_read_I08(cxt);
			texture->channel_count = qb_pbj_read_I08(cxt);
			texture->name = qb_pbj_read_string(cxt, NULL);
			texture->address = NULL;
			texture->input_size = NULL;
		} else if(opcode == PBJ_KERNEL_NAME) {
			uint32_t len;
			const char *name = qb_pbj_read_string(cxt, &len);
		} else if(opcode == PBJ_VERSION_DATA) {
			uint32_t version;
			version = qb_pbj_read_I32(cxt);
		} else {
			qb_abort("Unknown opcode: %02x (previous opcode = %02x)", opcode, prev_opcode);
			break;
		}
		prev_opcode = opcode;
	}
	for(i = 0; i < cxt->pbj_parameter_count; i++) {
		qb_pbj_parameter *parameter = &cxt->pbj_parameters[i];
		if(strcmp(parameter->name, "_OutCoord") == 0) {
			cxt->pbj_out_coord = parameter;
		} else if(parameter->qualifier == PBJ_PARAMETER_OUT) {
			cxt->pbj_out_pixel = parameter;
		} else if(parameter->input_size_name) {
			qb_pbj_texture *texture = qb_pbj_find_texture(cxt, parameter->input_size_name);
			if(texture) {
				texture->input_size = parameter;
			}
		}
	}

	if(!cxt->pbj_out_pixel) {
		qb_abort("Missing output pixel");
	}
	if(!cxt->pbj_out_coord) {
		qb_abort("Missing output pixel coordinates");
	}
	if(cxt->pbj_conditional_count > 0) {
		qb_abort("Missing end if");
	}
	if(cxt->pbj_out_pixel->type != PBJ_TYPE_FLOAT4) {
		qb_abort("Output pixel is not float4");
	}
}

static uint32_t zend_always_inline qb_pbj_get_op_effect(qb_compiler_context *cxt, qb_pbj_op *pop, qb_pbj_address *address, uint32_t effect_mask) {
	qb_pbj_address *dst[1], *input[4];
	uint32_t i, dst_count = 0, dst_types[1], input_count = 0, input_types[4], flags = 0;
	qb_pbj_translator *t = &pbj_op_translators[pop->opcode];

	if(effect_mask & PBJ_OP_READ) {
		if(t->flags & PBJ_READ_SOURCE) {
			input[input_count] = &pop->source;
			input_types[input_count] = PBJ_AS_SOURCE;
			input_count++;

			if(t->flags & PBJ_READ_SOURCE2) {
				input[input_count] = &pop->source2;
				input_types[input_count] = PBJ_AS_SOURCE2;
				input_count++;

				if(t->flags & PBJ_READ_SOURCE3) {
					input[input_count] = &pop->source3;
					input_types[input_count] = PBJ_AS_SOURCE3;
					input_count++;
				}
			}
		}
		if(t->flags & PBJ_READ_DESTINATION) {
			input[input_count] = &pop->destination;
			input_types[input_count] = PBJ_AS_DESTINATION;
			input_count++;
		}
	}
	if(effect_mask & PBJ_OP_WRITE) {
		if(t->flags & PBJ_WRITE_DESTINATION) {
			dst[dst_count] = &pop->destination;
			dst_types[dst_count] = PBJ_AS_DESTINATION;
			dst_count++;
		} else if(t->flags & PBJ_WRITE_BOOL) {
			cxt->pbj_comparison_register.channel_count = pop->source.channel_count;
			cxt->pbj_comparison_register.dimension = 1;
			for(i = 0; i < pop->source.channel_count; i++) {
				cxt->pbj_comparison_register.channels[i] = i;
			}
			cxt->pbj_comparison_register.register_id = 0 | PBJ_REGISTER_INT;
			dst[dst_count] = &cxt->pbj_comparison_register;
			dst_types[dst_count] = PBJ_AS_BOOL;
			dst_count++;
		}
	}
	for(i = 0; i < input_count; i++) {
		uint32_t overlap = qb_pbj_find_address_overlap(cxt, input[i], address);
		if(overlap) {
			if(overlap == PBJ_ADDRESS_EQUAL) {
				flags |= PBJ_OP_READ_EXACT;
			} else {
				flags |= PBJ_OP_READ;
			}
			flags |= input_types[i];
		}
	}
	if(dst_count) {
		uint32_t overlap = qb_pbj_find_address_overlap(cxt, dst[0], address);
		if(overlap) {
			if(overlap == PBJ_ADDRESS_EQUAL) {
				flags |= PBJ_OP_OVERWRITE_EXACT;
			} else if(overlap == PBJ_ADDRESS_FIRST_CONTAINS_SECOND || overlap == PBJ_ADDRESS_EXACT_OVERLAP) {
				flags |= PBJ_OP_OVERWRITE;
			} else {
				flags |= PBJ_OP_WRITE;
			}
			flags |= dst_types[0];
		}
	}
	return flags;
}

static int32_t qb_pbj_check_value_reuse(qb_compiler_context *cxt, uint32_t start_index, qb_pbj_address *address) {
	uint32_t i, effect, mask = qb_pbj_get_channel_mask(cxt, address);
	for(i = start_index; i < cxt->pbj_op_count; i++) {
		qb_pbj_op *pop = &cxt->pbj_ops[i];
		effect = qb_pbj_get_op_effect(cxt, pop, address, PBJ_OP_READ | PBJ_OP_WRITE);
		if(effect & PBJ_OP_READ) {
			// address is read
			return TRUE;
		} else if(effect & PBJ_OP_WRITE) {
			if((effect & PBJ_OP_OVERWRITE) == PBJ_OP_OVERWRITE) {
				// address is overwritten before it's read again
				break;
			} else {
				// address is only partial modified--clear the bits of the mask where the change 
				uint32_t modified_channel_mask;
				if(effect & PBJ_AS_DESTINATION) {
					modified_channel_mask = qb_pbj_get_channel_mask(cxt, &pop->destination);
					if(address->dimension > 2) {
						// a 3x3 or 4x4 matrix can span multiple registers
						uint32_t offset = pop->destination.register_id - address->register_id;
						modified_channel_mask <<= offset * 4;
					}
				} else if(effect & PBJ_AS_BOOL) {
					modified_channel_mask = 0x000000001;
				}
				mask &= ~modified_channel_mask;
				if(!mask) {
					// all the channels have been modified, so the value isn't reused
					break;
				}
			}
		}
	}
	return FALSE;
}

typedef struct qb_pbj_op_info {
	uint32_t opcode;
	qb_pbj_address **src_ptr;
	qb_pbj_address **dst_ptr;
} qb_pbj_op_info;

static uint32_t ZEND_FASTCALL ob_pbj_find_op_sequence(qb_compiler_context *cxt, qb_pbj_op_info *seq, uint32_t seq_len) {
	// use Boyer-Moore-Horspool to locate the opcode sequence
	uint32_t i, j, last = seq_len - 1, skip;
    uint32_t bad_opcode_skip[PBJ_MAX_OPCODE + 1];
	for(i = 0; i <= PBJ_MAX_OPCODE; i++) {
        bad_opcode_skip[i] = seq_len;
	}
	for(i = 0; i < last; i++) {
		bad_opcode_skip[seq[i].opcode] = last - i;
	}
	for(j = 0; j + last < cxt->pbj_op_count; j += skip) {
		for(i = last; cxt->pbj_ops[j + i].opcode == seq[i].opcode; i--) {
			if(i == 0) {
				// found the opcode sequence--now verify that the addresses are correct
				int32_t correct = TRUE;
				for(i = 0; i < seq_len && correct; i++) {
					qb_pbj_op *pop = &cxt->pbj_ops[j + i];
					if(pop->opcode == PBJ_LOAD_CONSTANT) {
						// check the constant
						if(pop->constant.type == PBJ_TYPE_INT) {
							if(pop->constant.int_value != *((int32_t *) seq[i].src_ptr)) {
								correct = FALSE;
							}
						} else {
							if(pop->constant.float_value != *((float32_t *) seq[i].src_ptr)) {
								correct = FALSE;
							}
						}
					} else {
						if(seq[i].src_ptr) {
							// check the source:
							// if the pointer is null, set it
							// if not, check that the address match
							if(!*seq[i].src_ptr) {
								*seq[i].src_ptr = &pop->source;
							} else {
								if(!qb_pbj_match_addresses(cxt, &pop->source, *seq[i].src_ptr)) {
									correct = FALSE;
								}
							}
						}
					}
					if(seq[i].dst_ptr) {
						// check the destination--same logic
						if(!*seq[i].dst_ptr) {
							*seq[i].dst_ptr = &pop->destination;
						} else {
							if(!qb_pbj_match_addresses(cxt, &pop->destination, *seq[i].dst_ptr)) {
								correct = FALSE;
							}
						}
					}
				}
				return (correct) ? j : (uint32_t) -1;
			}
		}
		skip = bad_opcode_skip[cxt->pbj_ops[j + last].opcode];
    }
    return (uint32_t) -1;
}

static void ZEND_FASTCALL qb_pbj_substitute_ops(qb_compiler_context *cxt) {
	// look for smoothStep sequence
	qb_pbj_address *x = NULL, *edge0 = NULL, *edge1 = NULL, *result = NULL;
	qb_pbj_address *zero = NULL, *one = NULL, *var1 = NULL, *var2 = NULL, *var3 = NULL; 
	float32_t constants[4] = { 0.0, 1.0, 2.0, 3.0 }; 
	qb_pbj_op_info smooth_step_sequence[18] = { 
		{	PBJ_LOAD_CONSTANT,	(qb_pbj_address **)	&constants[0],	&zero	},
		{	PBJ_LOAD_CONSTANT,	(qb_pbj_address **)	&constants[1],	&one	},
		{	PBJ_COPY,			&x,									&var1	},
		{	PBJ_SUBTRACT,		&edge0,								&var1	},
		{	PBJ_COPY,			&edge1,								&var2	},
		{	PBJ_SUBTRACT,		&edge0,								&var2	},
		{	PBJ_RECIPROCAL,		&var2,								&var3	},
		{	PBJ_MULTIPLY,		&var3,								&var1	},
		{	PBJ_MAX,			&zero,								&var1	},
		{	PBJ_MIN,			&one,								&var1	},
		{	PBJ_LOAD_CONSTANT,	(qb_pbj_address **)	&constants[2],	&var2	},
		{	PBJ_MULTIPLY,		&var1,								&var2	},
		{	PBJ_LOAD_CONSTANT,	(qb_pbj_address **)	&constants[3],	&var3	},
		{	PBJ_SUBTRACT,		&var2,								&var3	},
		{	PBJ_COPY,			&var1,								&var2	},
		{	PBJ_MULTIPLY,		&var1,								&var2	},
		{	PBJ_MULTIPLY,		&var3,								&var2	},
		{	PBJ_COPY,			&var2,								&result	},
	};
	uint32_t start_index, seq_len = sizeof(smooth_step_sequence) / sizeof(qb_pbj_op_info);
	start_index = ob_pbj_find_op_sequence(cxt, smooth_step_sequence, seq_len);
	if(start_index != (uint32_t) -1) {
		qb_pbj_op *ss_pop = &cxt->pbj_ops[start_index];
		qb_pbj_op *ss_data_pop = &cxt->pbj_ops[start_index + 1];
		uint32_t i;

		// change the first and second ops to smoothStep
		ss_pop->opcode = PBJ_SMOOTH_STEP;
		memcpy(&ss_pop->source, x, sizeof(qb_pbj_address));
		memcpy(&ss_pop->destination, result, sizeof(qb_pbj_address));
		ss_data_pop->opcode = PBJ_OP_DATA;
		memcpy(&ss_data_pop->source2, edge0, sizeof(qb_pbj_address));
		memcpy(&ss_data_pop->source3, edge1, sizeof(qb_pbj_address));

		// make the other NOP
		for(i = 2; i < seq_len; i++) {
			qb_pbj_op *nop = &cxt->pbj_ops[start_index + i];
			nop->opcode = PBJ_NOP;
		}
	}
}

static void ZEND_FASTCALL qb_pbj_remove_redundant_ops(qb_compiler_context *cxt) {
	uint32_t i;
	for(i = 0; i < cxt->pbj_op_count; i++) {
		qb_pbj_op *pop = &cxt->pbj_ops[i];
		uint32_t effect;
		if(pop->opcode == PBJ_COPY || pop->opcode == PBJ_LOAD_CONSTANT) {
			if(i + 1 < cxt->pbj_op_count) {
				qb_pbj_op *next_pop = &cxt->pbj_ops[i + 1];
				effect = qb_pbj_get_op_effect(cxt, next_pop, &pop->destination, PBJ_OP_READ | PBJ_OP_WRITE);
				// see if the destination is read by the next op
				if((effect & PBJ_OP_READ_EXACT) == PBJ_OP_READ_EXACT) {
					if(effect & PBJ_AS_DESTINATION) {
						// it's used as the destination (serving simultaneously as input value)
						// unless the destination is overwritten, check whether the value is used by ops beyond the next
						// if not, then we can safely remove the op
						if(((effect & PBJ_OP_OVERWRITE) == PBJ_OP_OVERWRITE) || !qb_pbj_check_value_reuse(cxt, i + 2, &pop->destination)) {
							next_pop->destination_origin = &pop->source;
							pop->opcode = PBJ_NOP;
						}
					} else if(effect & PBJ_AS_SOURCE) {
						// it's used as the source						
						/*if(!qb_pbj_check_value_reuse(cxt, i + 2, &pop->destination)) {
							next_pop->source_origin = &pop->source;
							pop->opcode = PBJ_NOP;
						}*/
					}
				} 
			}
			/*if(pop->opcode == PBJ_COPY && i > 0) {
				qb_pbj_op *prev_pop = &cxt->pbj_ops[i - 1];
				// see if the previous op write into the source of this one
				effect = qb_pbj_get_op_effect(cxt, prev_pop, &pop->source, PBJ_OP_WRITE);
				if((effect & PBJ_OP_OVERWRITE_EXACT) == PBJ_OP_OVERWRITE_EXACT) {
					if(!qb_pbj_check_value_reuse(cxt, i + 1, &pop->source)) {
						if(PBJ_MATRIX_MATRIX_MULTIPLY <= prev_pop->opcode && prev_pop->opcode <= PBJ_MATRIX_MATRIX_MULTIPLY) {
							// matrix operators cannot read and write into the same location
							qb_pbj_address *input = (prev_pop->destination_origin) ? prev_pop->destination_origin : &prev_pop->destination;
							if(qb_pbj_match_addresses(cxt, input, &pop->destination)) {
								continue;
							}
						}
						prev_pop->eventual_destination = &pop->destination;
						pop->opcode = PBJ_NOP;
					}
				}
			}*/
		}
	}
}

static int32_t ZEND_FASTCALL qb_pbj_is_image(qb_compiler_context *cxt, qb_address *address) {
	if(address && address->type == QB_TYPE_F32 && address->dimension_count == 3) {
		if(VALUE(U32, address->dimension_addresses[2]) == 4) {
			return TRUE;
		}
	}
	return FALSE;
}

static qb_variable * ZEND_FASTCALL qb_pbj_find_argument(qb_compiler_context *cxt, const char *name) {
	uint32_t i;
	for(i = 0; i < cxt->argument_count; i++) {
		qb_variable *qvar = cxt->variables[i];
		if(strcmp(qvar->name, name) == 0) {
			return qvar;
		}
	}
	return NULL;
}

static qb_variable * ZEND_FASTCALL qb_pbj_find_input(qb_compiler_context *cxt) {
	qb_variable *input_var = NULL;
	uint32_t i;
	for(i = 0; i < cxt->argument_count; i++) {
		qb_variable *qvar = cxt->variables[i];
		if(!(qvar->flags & QB_VARIABLE_PASSED_BY_REF)) {
			if(qb_pbj_is_image(cxt, qvar->address)) {
				if(!input_var) {
					input_var = qvar;
				} else {
					// if there's more than one then the name ought to match
				}
			}
		}
	}
	return input_var;
}

static qb_variable * ZEND_FASTCALL qb_pbj_find_output(qb_compiler_context *cxt) {
	uint32_t i;
	for(i = 0; i < cxt->argument_count; i++) {
		qb_variable *qvar = cxt->variables[i];
		if(qvar->flags & QB_VARIABLE_PASSED_BY_REF) {
			if(qb_pbj_is_image(cxt, qvar->address)) {
				return qvar;
			}
		}
	}
	return NULL;
}

static void ZEND_FASTCALL qb_pbj_map_arguments(qb_compiler_context *cxt) {
	uint32_t i;
	qb_variable *qvar;

	for(i = 0; i < cxt->pbj_texture_count; i++) {
		qb_pbj_texture *texture = &cxt->pbj_textures[i];
		qvar = qb_pbj_find_argument(cxt, texture->name);
		if(qvar) {
			if(qb_pbj_is_image(cxt, qvar->address)) {
				texture->address = qvar->address;
			} else {
				qb_abort("Parameter '%s' is not of the correct type", qvar->name);
			} 
		} else {
			// if there's only one input image, then use that one
			qvar = qb_pbj_find_input(cxt);
			if(qvar) {
				texture->address = qvar->address;
			} else {
				qb_abort("Input image '%s' must be passed to the function", texture->name); 
			}
		}
	}

	for(i = 0; i < cxt->pbj_parameter_count; i++) {
		qb_pbj_parameter *parameter = &cxt->pbj_parameters[i];
		if(parameter == cxt->pbj_out_pixel) {
			qvar = qb_pbj_find_output(cxt);
			if(qvar) {
				parameter->address = qvar->address;
			} else {
				qb_abort("A parameter must be passed by reference to the function to receive the result");
			} 
		} else if(parameter == cxt->pbj_out_coord) {
			// this is actually set within the function 
		} else {
			qvar = qb_pbj_find_argument(cxt, parameter->name);
			if(qvar) {
				if(!parameter->input_size_name) {
					uint32_t parameter_type = (parameter->destination.register_id & PBJ_REGISTER_INT) ? QB_TYPE_I32 : QB_TYPE_F32;
					if(qvar->address->type == parameter_type) {
						uint32_t parameter_size, element_count;
						if(parameter->destination.dimension > 1) {
							parameter_size = pbj_matrix_sizes[parameter->destination.dimension];
						} else {
							parameter_size = parameter->destination.channel_count;
						}
						if(IS_SCALAR(qvar->address)) {
							element_count = 1;
						} else {
							element_count = ARRAY_SIZE(qvar->address);
						}
						if(element_count == parameter_size) {
							parameter->address = qvar->address;
						} else {
							qb_abort("Parameter '%s' is not of the correct size", qvar->name);
						}
					} else {
						qb_abort("Parameter '%s' is not of the correct type", qvar->name);
					}
				} else {
					qb_abort("Parameter '%s' is obtained from the input image and cannot be passed separately", parameter->name);
				}

			} else {
				// see if it has a default value
				if(!parameter->default_value.type) {
					qb_abort("Parameter '%s' does not have a default value and must be passed to the function", parameter->name);
				}
			}
		}
	}
}

static void ZEND_FASTCALL qb_pbj_translate_instructions(qb_compiler_context *cxt) {
	qb_address *image_address, *pixel_address, *output_address;
	qb_address *width_address, *height_address, *x_address, *y_address, *pixel_index_address, *channel_count_address;
	qb_address *out_coord_x_address, *out_coord_y_address;
	qb_op *qop;
	uint32_t i, end_index;

	// interpret 3x4 matrix as 3x3 with padded element
	cxt->matrix_padding = TRUE;

	// coordinates for looping 
	x_address = qb_obtain_temporary_scalar(cxt, QB_TYPE_U32);
	qb_add_reference(cxt, x_address);
	y_address = qb_obtain_temporary_scalar(cxt, QB_TYPE_U32);
	qb_add_reference(cxt, y_address);
	image_address = cxt->pbj_out_pixel->address;
	width_address = image_address->dimension_addresses[1];
	height_address = image_address->dimension_addresses[0];
	channel_count_address = image_address->dimension_addresses[2];

	// index into output image array
	pixel_index_address = qb_obtain_temporary_scalar(cxt, QB_TYPE_U32);
	qb_add_reference(cxt, pixel_index_address);

	// sub-array representing the pixel
	pixel_address = qb_allocate_address(cxt->pool);
	pixel_address->mode = QB_ADDRESS_MODE_ARR;
	pixel_address->type = image_address->type;
	pixel_address->segment_selector = image_address->segment_selector;
	pixel_address->segment_offset = QB_OFFSET_INVALID;
	pixel_address->array_index_address = pixel_index_address;
	pixel_address->dimension_count = 1;
	pixel_address->flags = QB_ADDRESS_INITIALIZED;
	pixel_address->array_size_address = channel_count_address;
	pixel_address->array_size_addresses = pixel_address->dimension_addresses = &pixel_address->array_size_address;
	pixel_address->ref_count = 1;
	pixel_address->source_address = image_address;

	// the pixel produced by the PB routine
	output_address = qb_pbj_obtain_channel_address(cxt, cxt->pbj_out_pixel->destination.register_id, 0, 4);

	// _OutCoord--or x and y as float
	out_coord_x_address = qb_pbj_obtain_channel_address(cxt, cxt->pbj_out_coord->destination.register_id, cxt->pbj_out_coord->destination.channels[0], 1);
	out_coord_y_address = qb_pbj_obtain_channel_address(cxt, cxt->pbj_out_coord->destination.register_id, cxt->pbj_out_coord->destination.channels[1], 1);

#define OUTER_LOOK_JUMP_TARGET_INDEX		0
#define INNER_LOOP_JUMP_TARGET_INDEX		1

	// shift the translation table by two so there's a place to put the inner and outer loop jump targets
	cxt->pbj_op_offset = 2;
	cxt->op_translations = qb_allocate_indices(cxt->pool, cxt->pbj_op_offset + cxt->pbj_op_count + 1); 
	memset(cxt->op_translations, 0xFF, (cxt->pbj_op_offset + cxt->pbj_op_count + 1) * sizeof(uint32_t));

	// copy parameters and input sizes
	for(i = 0; i < cxt->pbj_parameter_count; i++) {
		qb_pbj_parameter *parameter = &cxt->pbj_parameters[i];
		if(parameter != cxt->pbj_out_coord && parameter != cxt->pbj_out_pixel) {
			if(parameter->input_size_name) {
				qb_address *input_image_address = parameter->address;
				qb_address *input_width_address = input_image_address->dimension_addresses[1];
				qb_address *input_height_address = input_image_address->dimension_addresses[0];
				qb_address *input_size_x_address = qb_pbj_obtain_channel_address(cxt, parameter->destination.register_id, parameter->destination.channels[0], 1);
				qb_address *input_size_y_address = qb_pbj_obtain_channel_address(cxt, parameter->destination.register_id, parameter->destination.channels[1], 1);
				qb_create_unary_op(cxt, &factory_copy, input_width_address, input_size_x_address);
				qb_create_unary_op(cxt, &factory_copy, input_height_address, input_size_y_address);
			} else if(parameter->address) {
				qb_address *dst_address;
				qb_address *src_address = parameter->address;
				qb_pbj_address *dst = &parameter->destination;
				dst_address = qb_pbj_obtain_address(cxt, dst);
				qb_create_unary_op(cxt, &factory_copy, src_address, dst_address);
			} else if(parameter->default_value.type) {
				// use default value if parameter isn't mapped to an argument
				qb_address *value_address = qb_pbj_obtain_value_address(cxt, &parameter->default_value);
				qb_address *param_address = qb_pbj_obtain_address(cxt, &parameter->destination);
				if(value_address) {
					qb_create_unary_op(cxt, &factory_copy, value_address, param_address);
				}
			}
		}
	}

	// set pixel index to zero
	qb_create_unary_op(cxt, &factory_copy, cxt->pbj_int_numerals[0], pixel_index_address);

	// set y to zero, but _OutCoord.y to 0.5, as that's the center of the grid
	qb_create_unary_op(cxt, &factory_copy, cxt->pbj_int_numerals[0], y_address);
	qb_create_unary_op(cxt, &factory_copy, cxt->pbj_float_numerals[1], out_coord_y_address);

	// set x to zero, but _OutCoord.x to 0.5
	// the outer loop starts here
	cxt->op_translations[OUTER_LOOK_JUMP_TARGET_INDEX] = cxt->op_count;
	qb_create_unary_op(cxt, &factory_copy, cxt->pbj_int_numerals[0], x_address);
	qop = cxt->ops[cxt->op_count - 1];
	qop->flags |= QB_OP_JUMP_TARGET;
	qb_create_unary_op(cxt, &factory_copy, cxt->pbj_float_numerals[1], out_coord_x_address);

	// initialize the output pixel to zero--the inner loop starts here
	cxt->op_translations[INNER_LOOP_JUMP_TARGET_INDEX] = cxt->op_count;
	qb_create_unary_op(cxt, &factory_copy, cxt->pbj_float_numerals[0], output_address);
	qop = cxt->ops[cxt->op_count - 1];
	qop->flags |= QB_OP_JUMP_TARGET;

	// replace specific sequences of ops with inline functions (only smoothStep for now)
	qb_pbj_substitute_ops(cxt);

	// remove redunant copy ops and find second operand of binary operations
	qb_pbj_remove_redundant_ops(cxt);

	// translate the PB instructions
	cxt->pbj_op = &cxt->pbj_ops[0];
	while(cxt->pbj_op_index < cxt->pbj_op_count) {
		uint32_t current_op_count = cxt->op_count;
		qb_pbj_translate_current_instruction(cxt);

		// add a nop if new one wasn't generated
		if(current_op_count == cxt->op_count) {
			qb_create_nop(cxt);
		}

		// flag new op as a jump target if there's a placeholder in the position
		if(cxt->op_translations[cxt->pbj_op_offset + cxt->pbj_op_index] == QB_OP_INDEX_JUMP_TARGET) {
			qb_op *first_op = cxt->ops[current_op_count];
			first_op->flags |= QB_OP_JUMP_TARGET;
		}
		cxt->op_translations[cxt->pbj_op_offset + cxt->pbj_op_index] = current_op_count;

		if(cxt->jump_target_index1) {
			cxt->pbj_op_index = cxt->jump_target_index1;
			cxt->pbj_op = &cxt->pbj_ops[cxt->pbj_op_index];
			cxt->jump_target_index1 = 0;
		} else {
			cxt->pbj_op++;
			cxt->pbj_op_index++;
		}
	}
	end_index = cxt->op_count;

	// copy the output pixel into the image
	qb_do_assignment(cxt, output_address, pixel_address);

	// move the pixel index forward
	qb_create_binary_op(cxt, &factory_add, pixel_index_address, channel_count_address, pixel_index_address);

	// increment x and _OutCoord.x
	qb_create_nullary_op(cxt, &factory_increment, x_address);
	qb_create_nullary_op(cxt, &factory_increment, out_coord_x_address);

	// if there's an jump to the end of the PB program, mark the instruction as a jump target
	if(cxt->op_translations[cxt->pbj_op_offset + cxt->pbj_op_count] == QB_OP_INDEX_JUMP_TARGET) {
		qop = cxt->ops[end_index];
		qop->flags |= QB_OP_JUMP_TARGET;
		cxt->op_translations[cxt->pbj_op_offset + cxt->pbj_op_count] = end_index;
	}

	// jump to beginning of inner loop if x is less than width
	qb_create_comparison_branch_op(cxt, &factory_branch_on_less_than, INNER_LOOP_JUMP_TARGET_INDEX, QB_INSTRUCTION_NEXT, x_address, width_address);

	// increment y and _OutCoord.y
	qb_create_nullary_op(cxt, &factory_increment, y_address);
	qb_create_nullary_op(cxt, &factory_increment, out_coord_y_address);

	// jump to beginning of outer loop if y is less than height
	qb_create_comparison_branch_op(cxt, &factory_branch_on_less_than, OUTER_LOOK_JUMP_TARGET_INDEX, QB_INSTRUCTION_NEXT, y_address, height_address);
	qb_create_op(cxt, &factory_return, NULL, 0, NULL);

	// allocate memory space for the registers
	if(cxt->pbj_float_register_count > 0) {
		qb_pbj_map_registers_to_memory(cxt, cxt->pbj_float_registers, cxt->pbj_float_register_count, QB_TYPE_F32);
	}
	if(cxt->pbj_int_register_count > 0) {
		qb_pbj_map_registers_to_memory(cxt, cxt->pbj_int_registers, cxt->pbj_int_register_count, QB_TYPE_I32);
	}
}

uint32_t pbj_matrix_sizes[] = { 0, 1, 4, 12, 16 };
