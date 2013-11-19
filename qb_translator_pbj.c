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
#include "qb_translator_pbj.h"

static uint8_t qb_read_pbj_I08(qb_pbj_translator_context *cxt) {
	uint8_t value = 0;
	if(cxt->pbj_data_end - cxt->pbj_data >= 1) { 
		value = *((uint8_t *) cxt->pbj_data);
		cxt->pbj_data += 1;
	}
	return value;
}

static uint16_t qb_read_pbj_I16(qb_pbj_translator_context *cxt) {
	uint16_t value = 0;
	if(cxt->pbj_data_end - cxt->pbj_data >= 2) { 
		value = SWAP_LE_I16(*((uint16_t *) cxt->pbj_data));
		cxt->pbj_data += 2;
	}
	return value;
}

static uint32_t qb_read_pbj_I32(qb_pbj_translator_context *cxt) {
	uint32_t value = 0;
	if(cxt->pbj_data_end - cxt->pbj_data >= 4) { 
		value = SWAP_LE_I32(*((uint32_t *) cxt->pbj_data));
		cxt->pbj_data += 4;
	}
	return value;
}

static float32_t qb_read_pbj_F32(qb_pbj_translator_context *cxt) {
	float32_t value = 0.0f;
	if(cxt->pbj_data_end - cxt->pbj_data >= 4) { 
		uint32_t int_value = SWAP_BE_I32(*((uint32_t *) cxt->pbj_data));
		value = *((float32_t *) &int_value);
		cxt->pbj_data += 4;
	}
	return value;
}

static const char * qb_read_pbj_string(qb_pbj_translator_context *cxt, uint32_t *p_length) {
	const char *value = "";
	if(p_length) {
		uint16_t len = qb_read_pbj_I16(cxt);
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

static qb_pbj_texture * qb_find_pbj_texture_by_id(qb_pbj_translator_context *cxt, uint32_t image_id) {
	uint32_t i;
	for(i = 0; i < cxt->texture_count; i++) {
		qb_pbj_texture *texture = &cxt->textures[i];
		if(texture->image_id == image_id) {
			return texture;
		}
	}
	return NULL;
}

static void qb_read_pbj_value(qb_pbj_translator_context *cxt, uint32_t type, qb_pbj_value *value) {
	uint32_t i;
	value->type = type;
	switch(type) {
		case PBJ_TYPE_FLOAT: {
			value->float1 = qb_read_pbj_F32(cxt);
		}	break;
		case PBJ_TYPE_FLOAT2: {
			for(i = 0; i < 2; i++) {
				value->float2[i] = qb_read_pbj_F32(cxt);
			}
		}	break;
		case PBJ_TYPE_FLOAT3: {
			for(i = 0; i < 3; i++) {
				value->float3[i] = qb_read_pbj_F32(cxt);
			}
		}	break;
		case PBJ_TYPE_FLOAT4: {
			for(i = 0; i < 4; i++) {
				value->float4[i] = qb_read_pbj_F32(cxt);
			}
		}	break;
		case PBJ_TYPE_FLOAT2X2: {
			for(i = 0; i < 4; i++) {
				value->float2x2[i] = qb_read_pbj_F32(cxt);
			}
		}	break;
		case PBJ_TYPE_FLOAT3X3: {
			for(i = 0; i < 9; i++) {
				value->float3x3[i] = qb_read_pbj_F32(cxt);
			}
		}	break;
		case PBJ_TYPE_FLOAT4X4: {
			for(i = 0; i < 16; i++) {
				value->float4x4[i] = qb_read_pbj_F32(cxt);
			}
		}	break;
		case PBJ_TYPE_BOOL:
		case PBJ_TYPE_INT: {
			value->int1 = qb_read_pbj_I16(cxt);
		}	break;
		case PBJ_TYPE_BOOL2:
		case PBJ_TYPE_INT2: {
			for(i = 0; i < 2; i++) {
				value->int2[i] = qb_read_pbj_I16(cxt);
			}
		}	break;
		case PBJ_TYPE_BOOL3:
		case PBJ_TYPE_INT3: {
			for(i = 0; i < 3; i++) {
				value->int3[i] = qb_read_pbj_I16(cxt);
			}
		}	break;
		case PBJ_TYPE_BOOL4:
		case PBJ_TYPE_INT4: {
			for(i = 0; i < 4; i++) {
				value->int4[i] = qb_read_pbj_I16(cxt);
			}
		}	break;
		case PBJ_TYPE_STRING: {
			value->string = qb_read_pbj_string(cxt, NULL);
		}	break;
	}
}

static qb_pbj_texture * qb_find_pbj_texture(qb_pbj_translator_context *cxt, const char *name) {
	uint32_t i;
	for(i = 0; i < cxt->texture_count; i++) {
		qb_pbj_texture *texture = &cxt->textures[i];
		if(strcmp(texture->name, name) == 0) {
			return texture;
		}
	}
	return NULL;
}

static qb_pbj_channel_id qb_get_pbj_channel_id(qb_pbj_translator_context *cxt, qb_pbj_channel_id *channels, uint32_t channel_count) {
	switch(channel_count) {
		case 1: return channels[0];
		case 2: {
			if(channels[0] == PBJ_CHANNEL_R && channels[1] == PBJ_CHANNEL_G) {
				return PBJ_CHANNEL_RG;
			} else if(channels[0] == PBJ_CHANNEL_G && channels[1] == PBJ_CHANNEL_B) {
				return PBJ_CHANNEL_GB;
			} else if(channels[0] == PBJ_CHANNEL_B && channels[1] == PBJ_CHANNEL_A) {
				return PBJ_CHANNEL_BA;
			}
		}	break;
		case 3: {
			if(channels[0] == PBJ_CHANNEL_R && channels[1] == PBJ_CHANNEL_G && channels[2] == PBJ_CHANNEL_B) {
				return PBJ_CHANNEL_RGB;
			} else if(channels[0] == PBJ_CHANNEL_G && channels[1] == PBJ_CHANNEL_B && channels[2] == PBJ_CHANNEL_A) {
				return PBJ_CHANNEL_GBA;
			}
		}	break;
		case 4: {
			if(channels[0] == PBJ_CHANNEL_R && channels[1] == PBJ_CHANNEL_G && channels[2] == PBJ_CHANNEL_B && channels[3] == PBJ_CHANNEL_A) {
				return PBJ_CHANNEL_RGBA;
			}
		}	break;
	}
	return PBJ_CHANNEL_NOT_CONTINUOUS;
}

static uint32_t qb_get_pbj_channel_count(qb_pbj_translator_context *cxt, qb_pbj_channel_id channel_id) {
	switch(channel_id) {
		case PBJ_CHANNEL_RG:
		case PBJ_CHANNEL_GB:
		case PBJ_CHANNEL_BA: return 2;
		case PBJ_CHANNEL_RGB:
		case PBJ_CHANNEL_GBA: return 3;
		case PBJ_CHANNEL_RGBA: return 4;
		default: break;
	}
	return 1;
}

static qb_pbj_channel_id qb_get_first_pbj_channel(qb_pbj_translator_context *cxt, qb_pbj_channel_id channel_id) {
	switch(channel_id) {
		case PBJ_CHANNEL_RG:
		case PBJ_CHANNEL_RGB:
		case PBJ_CHANNEL_RGBA: return PBJ_CHANNEL_R;
		case PBJ_CHANNEL_GB:
		case PBJ_CHANNEL_GBA: return PBJ_CHANNEL_G;
		case PBJ_CHANNEL_BA: return PBJ_CHANNEL_B;
		default: break;
	}
	return channel_id;
}

static uint32_t qb_get_pbj_channel_offset(qb_pbj_translator_context *cxt, qb_pbj_channel_id parent_channel_id, qb_pbj_channel_id channel_id) {
	switch(parent_channel_id) {
		case PBJ_CHANNEL_RGBA: {
			switch(channel_id) {
				case PBJ_CHANNEL_R:
				case PBJ_CHANNEL_RG:
				case PBJ_CHANNEL_RGB: return 0;
				case PBJ_CHANNEL_G:
				case PBJ_CHANNEL_GB:
				case PBJ_CHANNEL_GBA: return 1;
				case PBJ_CHANNEL_B:
				case PBJ_CHANNEL_BA: return 2;
				case PBJ_CHANNEL_A: return 3;
				default: break;
			}
		}	break;
		case PBJ_CHANNEL_GBA: {
			switch(channel_id) {
				case PBJ_CHANNEL_G:
				case PBJ_CHANNEL_GB:
				case PBJ_CHANNEL_GBA: return 0;
				case PBJ_CHANNEL_B:
				case PBJ_CHANNEL_BA: return 1;
				case PBJ_CHANNEL_A: return 2;
				default: break;
			}
		}	break;
		case PBJ_CHANNEL_RGB: {
			switch(channel_id) {
				case PBJ_CHANNEL_R:
				case PBJ_CHANNEL_RG: return 0;
				case PBJ_CHANNEL_G:
				case PBJ_CHANNEL_GB: return 1;
				case PBJ_CHANNEL_B: return 2;
				default: break;
			}
		}	break;
		case PBJ_CHANNEL_BA: {
			switch(channel_id) {
				case PBJ_CHANNEL_B: return 0;
				case PBJ_CHANNEL_A: return 1;
				default: break;
			}
		}	break;
		case PBJ_CHANNEL_GB: {
			switch(channel_id) {
				case PBJ_CHANNEL_G: return 0;
				case PBJ_CHANNEL_B: return 1;
				default: break;
			}
		}	break;
		case PBJ_CHANNEL_RG: {
			switch(channel_id) {
				case PBJ_CHANNEL_R: return 0;
				case PBJ_CHANNEL_G: return 1;
				default: break;
			}
		}	break;
		default: {
		}	break;
	}
	return -1;
}

static uint32_t qb_get_pbj_channel_mask(qb_pbj_translator_context *cxt, qb_pbj_channel_id *channels, uint32_t channel_count) {
	qb_pbj_channel_id first_channel = PBJ_CHANNEL_A;
	uint32_t mask = 0;
	uint32_t i;
	for(i = 0; i < channel_count; i++) {
		qb_pbj_channel_id channel = channels[i];
		if(channel < first_channel) {
			first_channel = channel;
		}
	}
	for(i = 0; i < channel_count; i++) {
		qb_pbj_channel_id channel = channels[i];
		mask |= (channel - first_channel) << (i * 3);
	}
	return mask;
}

static qb_pbj_channel_id qb_get_pbj_channel_extent(qb_pbj_translator_context *cxt, qb_pbj_channel_id *channels, uint32_t channel_count) {
	qb_pbj_channel_id start = PBJ_CHANNEL_A;
	qb_pbj_channel_id end = PBJ_CHANNEL_R;
	uint32_t i, width;
	for(i = 0; i < channel_count; i++) {
		qb_pbj_channel_id channel = channels[i];
		if(channel < start) {
			start = channel;
		} else if(channel > end) {
			end = channel;
		}
	}
	width = start - end + 1;
	if(width == 1) {
		return start;
	} else if(width == 2) {
		switch(start) {
			case PBJ_CHANNEL_R: return PBJ_CHANNEL_RG;
			case PBJ_CHANNEL_G: return PBJ_CHANNEL_GB;
			case PBJ_CHANNEL_B: return PBJ_CHANNEL_BA;
			default: break;
		}
	} else if(width == 3) {
		switch(start) {
			case PBJ_CHANNEL_R: return PBJ_CHANNEL_RGB;
			case PBJ_CHANNEL_G: return PBJ_CHANNEL_GBA;
			default: break;
		}
	}
	return PBJ_CHANNEL_RGBA;
}

static void qb_set_pbj_destination_channels(qb_pbj_translator_context *cxt, qb_pbj_address *address, uint32_t channel_mask, uint32_t dimension) {
	// dimension is used only if no destination channels are selected 
	// otherwise dimension is pertinent only to the source
	if(channel_mask) {
		qb_pbj_channel_id channels[4];
		qb_pbj_channel_id channel;
		uint32_t bit, channel_count = 0;
		for(bit = 0x08, channel = 0; channel < 4; channel++, bit >>= 1) {
			if(channel_mask & bit) {
				channels[channel_count++] = channel;
			}
		}
		address->dimension = 1;
		address->channel_id = qb_get_pbj_channel_id(cxt, channels, channel_count);
		address->channel_count = channel_count;
		if(address->channel_id == PBJ_CHANNEL_NOT_CONTINUOUS) {
			address->channel_id = qb_get_pbj_channel_extent(cxt, channels, channel_count);
			address->channel_mask = qb_get_pbj_channel_mask(cxt, channels, channel_count);
		} else {
			address->channel_mask = INVALID_INDEX;
		}
	} else {
		address->dimension = dimension;
		address->channel_mask = INVALID_INDEX;
		address->channel_id = PBJ_CHANNEL_MATRIX;
	}
}

static void qb_set_pbj_source_channels(qb_pbj_translator_context *cxt, qb_pbj_address *address, uint32_t channel_swizzle, uint32_t channel_count, uint32_t dimension) {
	if(dimension == 1) {
		qb_pbj_channel_id channels[4];
		qb_pbj_channel_id channel;
		uint32_t i;
		for(i = 0; i < channel_count; i++) {
			channel = (channel_swizzle >> ((3 - i) * 2)) & 0x03;
			channels[i] = channel;
		}
		address->dimension = 1;
		address->channel_id = qb_get_pbj_channel_id(cxt, channels, channel_count);
		address->channel_count = channel_count;
		if(address->channel_id == PBJ_CHANNEL_NOT_CONTINUOUS) {
			address->channel_id = qb_get_pbj_channel_extent(cxt, channels, channel_count);
			address->channel_mask = qb_get_pbj_channel_mask(cxt, channels, channel_count);
		} else {
			address->channel_mask = INVALID_INDEX;
		}
	} else {
		address->dimension = dimension;
		address->channel_mask = INVALID_INDEX;
		address->channel_id = PBJ_CHANNEL_MATRIX;
	}
}

static int32_t qb_match_pbj_addresses(qb_pbj_translator_context *cxt, qb_pbj_address *reg1_address, qb_pbj_address *reg2_address) {
	return (reg1_address->register_id == reg2_address->register_id && reg1_address->dimension == reg2_address->dimension 
 		 && reg1_address->channel_id == reg2_address->channel_id && reg1_address->channel_mask == reg2_address->channel_mask);
}

typedef struct qb_pbj_op_info {
	qb_pbj_opcode opcode;
	qb_pbj_address **src_ptr;
	qb_pbj_address **dst_ptr;
} qb_pbj_op_info;

static uint32_t qb_find_op_sequence(qb_pbj_translator_context *cxt, qb_pbj_op_info *seq, uint32_t seq_len) {
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
								if(!qb_match_pbj_addresses(cxt, &pop->source, *seq[i].src_ptr)) {
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
							if(!qb_match_pbj_addresses(cxt, &pop->destination, *seq[i].dst_ptr)) {
								correct = FALSE;
							}
						}
					}
				}
				return (correct) ? j : INVALID_INDEX;
			}
		}
		skip = bad_opcode_skip[cxt->pbj_ops[j + last].opcode];
    }
    return INVALID_INDEX;
}

static void qb_substitute_ops(qb_pbj_translator_context *cxt) {
	// look for smoothStep sequence
	qb_pbj_address *x = NULL, *edge0 = NULL, *edge1 = NULL, *result = NULL;
	qb_pbj_address *zero = NULL, *one = NULL, *var1 = NULL, *var2 = NULL, *var3 = NULL; 
	float32_t constants[4] = { 0.0, 1.0, 2.0, 3.0 }; 
	qb_pbj_op_info smooth_step_sequence[] = { 
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
		{	PBJ_MULTIPLY,		&var3,								&result	},
	};
	uint32_t start_index, seq_len = sizeof(smooth_step_sequence) / sizeof(qb_pbj_op_info);
	start_index = qb_find_op_sequence(cxt, smooth_step_sequence, seq_len);
	if(start_index != INVALID_INDEX) {
		qb_pbj_op *ss_pop = &cxt->pbj_ops[start_index];
		qb_pbj_op *ss_data_pop = &cxt->pbj_ops[start_index + 1];
		uint32_t i;

		// change the first and second ops to smoothStep
		ss_pop->opcode = PBJ_SMOOTH_STEP;
		memcpy(&ss_pop->source, edge0, sizeof(qb_pbj_address));
		memcpy(&ss_pop->destination, result, sizeof(qb_pbj_address));
		ss_data_pop->opcode = PBJ_OP_DATA;
		memcpy(&ss_data_pop->source2, edge1, sizeof(qb_pbj_address));
		memcpy(&ss_data_pop->source3, x, sizeof(qb_pbj_address));

		// make the other NOP
		for(i = 2; i < seq_len; i++) {
			qb_pbj_op *nop = &cxt->pbj_ops[start_index + i];
			nop->opcode = PBJ_NOP;
		}
	}
}

void qb_decode_pbj_binary(qb_pbj_translator_context *cxt) {
	uint32_t opcode, prev_opcode, i;
	cxt->pbj_data = (uint8_t *) cxt->compiler_context->external_code;
	cxt->pbj_data_end = cxt->pbj_data + cxt->compiler_context->external_code_length;

	while(cxt->pbj_data < cxt->pbj_data_end) {
		opcode = qb_read_pbj_I08(cxt);
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

			// read the destination address
			pop->destination.register_id = qb_read_pbj_I16(cxt);
			bit_fields = qb_read_pbj_I08(cxt);
			channel_mask = bit_fields >> 4;
			channel_count = (bit_fields & 0x03) + 1;
			dimension = ((bit_fields >> 2) & 0x03) + 1;
			qb_set_pbj_destination_channels(cxt, &pop->destination, channel_mask, dimension);

			if(opcode == PBJ_LOAD_CONSTANT) {
				pop->source.channel_id = pop->source.channel_mask = pop->source.register_id = pop->source.dimension = 0;
				pop->image_id = 0;
				if(pop->destination.register_id & PBJ_REGISTER_INT) {
					pop->constant.int_value = qb_read_pbj_I32(cxt);
					pop->constant.type = PBJ_TYPE_INT;
				} else {			
					pop->constant.float_value = qb_read_pbj_F32(cxt);
					pop->constant.type = PBJ_TYPE_FLOAT;
				}
				pop->flags = PBJ_DESTINATION_IN_USE;
			} else {
				// read the source
				pop->source.register_id = qb_read_pbj_I16(cxt);
				channel_swizzle = qb_read_pbj_I08(cxt);
				qb_set_pbj_source_channels(cxt, &pop->source, channel_swizzle, channel_count, dimension);

				// read the image id (only used for sampling functions)
				pop->image_id = qb_read_pbj_I08(cxt);

				if(opcode == PBJ_IF) {
					// push the instruction onto the conditional stack
					qb_pbj_op **p_conditional_pop = qb_enlarge_array((void **) &cxt->conditionals, 1);
					*p_conditional_pop = pop;
					pop->destination.dimension = 0;
					pop->branch_target_index = 0;
					pop->flags = PBJ_SOURCE_IN_USE;
				} else if(opcode == PBJ_ELSE || opcode == PBJ_END_IF) {
					// update the branch target index of the op at the top of the stack
					qb_pbj_op *related_pop;
					if(cxt->conditional_count == 0) {
						qb_abort("unexpected opcode: %02x", opcode);
					}
					related_pop = cxt->conditionals[cxt->conditional_count - 1];
					related_pop->branch_target_index = pop_index;
					if(opcode == PBJ_ELSE) {
						// replace the if with the else op
						cxt->conditionals[cxt->conditional_count - 1] = pop;
					} else {
						// pop the item off
						cxt->conditional_count--;
					}
					pop->source.dimension = 0;
					pop->destination.dimension = 0;
					pop->branch_target_index = 0;
					pop->flags = 0;
				} else if(opcode == PBJ_SELECT) {
					// put the on-true and on-false sources in the next op
					qb_pbj_op *data_pop = qb_enlarge_array((void **) &cxt->pbj_ops, 1);
					data_pop->opcode = PBJ_OP_DATA;
					data_pop->source2.register_id = qb_read_pbj_I16(cxt);
					channel_swizzle = qb_read_pbj_I08(cxt);
					bit_fields = qb_read_pbj_I08(cxt);
					channel_count = (bit_fields >> 6) + 1;
					qb_set_pbj_source_channels(cxt, &data_pop->source2, channel_swizzle, channel_count, dimension);

					data_pop->source3.register_id = qb_read_pbj_I16(cxt);
					channel_swizzle = qb_read_pbj_I08(cxt);
					bit_fields = qb_read_pbj_I08(cxt);
					channel_count = (bit_fields >> 6) + 1;
					qb_set_pbj_source_channels(cxt, &data_pop->source3, channel_swizzle, channel_count, dimension);
					data_pop->flags = PBJ_SOURCE_IN_USE | PBJ_DESTINATION_IN_USE;
					pop->flags = PBJ_SOURCE_IN_USE | PBJ_DESTINATION_IN_USE;
				} else if(opcode == PBJ_SAMPLE_BILINEAR || opcode == PBJ_SAMPLE_NEAREST) {
					pop->flags = PBJ_IMAGE_ID_IN_USE | PBJ_SOURCE_IN_USE | PBJ_DESTINATION_IN_USE; 
				} else {
					pop->flags = PBJ_SOURCE_IN_USE | PBJ_DESTINATION_IN_USE;
				}
			}
		} else if(opcode == PBJ_KERNEL_METADATA || opcode == PBJ_PARAMETER_METADATA) {
			uint8_t value_type;
			const char *value_name;
			qb_pbj_parameter *last_parameter;
			qb_pbj_value *value = NULL, tmp;
			value_type = qb_read_pbj_I08(cxt);
			value_name = qb_read_pbj_string(cxt, NULL);
			if(opcode == PBJ_PARAMETER_METADATA && cxt->parameter_count > 0) {
				last_parameter = &cxt->parameters[cxt->parameter_count - 1];
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
			qb_read_pbj_value(cxt, value_type, value);
			if(opcode == PBJ_PARAMETER_METADATA && value_type == PBJ_TYPE_STRING) {
				if(strcmp(value_name, "parameterType") == 0) {
					last_parameter->parameter_type = value->string;
				} else if(strcmp(value_name, "inputSizeName") == 0) {
					last_parameter->input_size_name = value->string;
				} else if(strcmp(value_name, "description") == 0) {
					last_parameter->description = value->string;
				} else if(strcmp(value_name, "displayName") == 0) {
					last_parameter->display_name = value->string;
				} else if(strcmp(value_name, "parameterType") == 0) {
					last_parameter->parameter_type = value->string;
				} 
			} else if(opcode == PBJ_KERNEL_METADATA) {
				if(strcmp(value_name, "description") == 0) {
					cxt->description = value->string;
				} else if(strcmp(value_name, "vendor") == 0) {
					cxt->vendor = value->string;
				} else if(strcmp(value_name, "displayName") == 0) {
					cxt->display_name = value->string;
				} else if(strcmp(value_name, "version") == 0) {
					cxt->version = value->int1;
				}
			}
		} else if(opcode == PBJ_PARAMETER_DATA) {
			qb_pbj_parameter *parameter = qb_enlarge_array((void **) &cxt->parameters, 1);
			uint32_t bit_fields, dimension, channel_mask;
			parameter->qualifier = qb_read_pbj_I08(cxt);
			parameter->type = qb_read_pbj_I08(cxt);
			parameter->destination.register_id = qb_read_pbj_I16(cxt);
			bit_fields = qb_read_pbj_I08(cxt);
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
			qb_set_pbj_destination_channels(cxt, &parameter->destination, channel_mask, dimension);
			parameter->name = qb_read_pbj_string(cxt, NULL);
			parameter->address = NULL;
			parameter->parameter_type = NULL;
			parameter->description = NULL;
			parameter->input_size_name = NULL;
			memset(&parameter->default_value, 0, sizeof(qb_pbj_value));
			memset(&parameter->min_value, 0, sizeof(qb_pbj_value));
			memset(&parameter->max_value, 0, sizeof(qb_pbj_value));
		} else if(opcode == PBJ_TEXTURE_DATA) {
			qb_pbj_texture *texture = qb_enlarge_array((void **) &cxt->textures, 1);
			texture->image_id = qb_read_pbj_I08(cxt);
			texture->channel_count = qb_read_pbj_I08(cxt);
			texture->name = qb_read_pbj_string(cxt, NULL);
			texture->address = NULL;
			texture->input_size = NULL;
		} else if(opcode == PBJ_KERNEL_NAME) {
			cxt->name = qb_read_pbj_string(cxt, &cxt->name_length);
		} else if(opcode == PBJ_VERSION_DATA) {
			qb_read_pbj_I32(cxt);
		} else {
			qb_abort("unknown opcode: %02x (previous opcode = %02x)", opcode, prev_opcode);
			break;
		}
		prev_opcode = opcode;
	}

	// look for for sequences of ops representing smoothstep()
	qb_substitute_ops(cxt);

	for(i = 0; i < cxt->parameter_count; i++) {
		qb_pbj_parameter *parameter = &cxt->parameters[i];
		if(strcmp(parameter->name, "_OutCoord") == 0) {
			cxt->out_coord = parameter;
		} else if(parameter->qualifier == PBJ_PARAMETER_OUT) {
			cxt->out_pixel = parameter;
		} else if(parameter->input_size_name) {
			qb_pbj_texture *texture = qb_find_pbj_texture(cxt, parameter->input_size_name);
			if(texture) {
				texture->input_size = parameter;
			}
		}
	}
	if(!cxt->out_pixel) {
		qb_abort("missing output pixel");
	}
	if(!cxt->out_coord) {
		qb_abort("missing output pixel coordinates");
	}
	if(cxt->conditional_count > 0) {
		qb_abort("missing end if");
	}
	if(cxt->out_pixel->type != PBJ_TYPE_FLOAT3 && cxt->out_pixel->type != PBJ_TYPE_FLOAT4) {
		qb_abort("output pixel is not float3 or float4");
	}
}

static qb_pbj_register * qb_get_pbj_register(qb_pbj_translator_context *cxt, qb_pbj_address *reg_address) {
	if(reg_address->register_id & PBJ_REGISTER_INT) {
		return &cxt->int_registers[reg_address->register_id & ~PBJ_REGISTER_INT];
	} else {
		return &cxt->float_registers[reg_address->register_id];
	}
}

static int32_t qb_is_image(qb_pbj_translator_context *cxt, qb_address *address, uint32_t channel_count) {
	if(address && address->type == QB_TYPE_F32 && address->dimension_count == 3) {
		if(CONSTANT_DIMENSION(address, -1) && DIMENSION(address, -1) == channel_count) {
			return TRUE;
		}
	}
	return FALSE;
}

static qb_variable * qb_find_argument(qb_pbj_translator_context *cxt, const char *name) {
	uint32_t i;
	for(i = 0; i < cxt->compiler_context->argument_count; i++) {
		qb_variable *qvar = cxt->compiler_context->variables[i];
		if(strcmp(qvar->name, name) == 0) {
			return qvar;
		}
	}
	return NULL;
}

static qb_variable * qb_find_input(qb_pbj_translator_context *cxt) {
	qb_variable *input_var = NULL;
	uint32_t i;
	for(i = 0; i < cxt->compiler_context->argument_count; i++) {
		qb_variable *qvar = cxt->compiler_context->variables[i];
		if(!(qvar->flags & QB_VARIABLE_BY_REF)) {
			if(qb_is_image(cxt, qvar->address, 4) || qb_is_image(cxt, qvar->address, 3)) {
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

static qb_variable * qb_find_output(qb_pbj_translator_context *cxt) {
	uint32_t i;
	for(i = 0; i < cxt->compiler_context->argument_count; i++) {
		qb_variable *qvar = cxt->compiler_context->variables[i];
		if(qvar->flags & QB_VARIABLE_BY_REF) {
			if(qb_is_image(cxt, qvar->address, 4) || qb_is_image(cxt, qvar->address, 3)) {
				return qvar;
			}
		}
	}
	return NULL;
}

static qb_pbj_register * qb_create_pbj_register(qb_pbj_translator_context *cxt, qb_pbj_address *reg_address) {
	qb_pbj_register **p_regs, *reg;
	uint32_t id, reg_required, end, *p_count;

	if(reg_address->register_id & PBJ_REGISTER_INT) {
		id = reg_address->register_id & ~PBJ_REGISTER_INT;
		p_regs = &cxt->int_registers;
		p_count = &cxt->int_register_count;
	} else {
		id = reg_address->register_id;
		p_regs = &cxt->float_registers;
		p_count = &cxt->float_register_count;
	}

	// matrices could span more than one register
	if(reg_address->dimension > 1) {
		switch(reg_address->dimension) {
			case 2: reg_required = 1; break;
			case 3: reg_required = 3; break;
			case 4: reg_required = 4; break;
		}
	} else {
		reg_required = 1;
	}
	end = id + reg_required - 1;

	if(end >= *p_count) {
		uint32_t addition = end - *p_count + 1;
		qb_enlarge_array((void **) p_regs, addition);
	}
	reg = &(*p_regs)[id];
	reg->span = reg_required;
	return reg;
}

static void qb_propagate_pbj_matrix_span(qb_pbj_translator_context *cxt, qb_pbj_register *reg) {
	if(reg->span == 1) {
		qb_address *rgba_address = qb_create_address_alias(cxt->compiler_context, reg->matrix_address);
		rgba_address->dimension_count = 1;
		rgba_address->array_size_addresses = rgba_address->dimension_addresses = &rgba_address->array_size_address;
		reg->channel_addresses[PBJ_CHANNEL_RGBA] = rgba_address;
	} else if(reg->span == 3) {
		// copy vector addresses into this and subsequent registers
		uint32_t i;
		for(i = 0; i < reg->span; i++) {
			qb_address *index_address = qb_obtain_constant_U32(cxt->compiler_context, i);
			reg[i].channel_addresses[PBJ_CHANNEL_RGBA] = qb_obtain_array_element(cxt->compiler_context, reg->matrix_address, index_address, QB_ARRAY_BOUND_CHECK_NONE);
		}
	} else {
		uint32_t i;
		for(i = 0; i < reg->span; i++) {
			qb_address *index_address = qb_obtain_constant_U32(cxt->compiler_context, i);
			reg[i].channel_addresses[PBJ_CHANNEL_RGBA] = qb_obtain_array_element(cxt->compiler_context, reg->matrix_address, index_address, QB_ARRAY_BOUND_CHECK_NONE);
		}
	}
}

static qb_address * qb_create_pbj_constant(qb_compiler_context *cxt, qb_pbj_value *value) {
	qb_address *address = NULL;
	int32_t *int_values = NULL;
	float32_t *float_values;
	uint32_t dimensions[2];
	uint32_t dimension_count;
	uint32_t i, j;

	switch(value->type) {
		case PBJ_TYPE_BOOL:
		case PBJ_TYPE_INT: return qb_obtain_constant_S32(cxt, value->int1);
		case PBJ_TYPE_FLOAT: return qb_obtain_constant_F32(cxt, value->float1);
		case PBJ_TYPE_INT2: {
			dimensions[0] = 2;
			dimension_count = 1;
			int_values = value->int2;
		}	break;
		case PBJ_TYPE_INT3: {
			dimensions[0] = 3;
			dimension_count = 1;
			int_values = value->int3;
		}	break;
		case PBJ_TYPE_INT4: {
			dimensions[0] = 4;
			dimension_count = 1;
			int_values = value->int4;
		}	break;
		case PBJ_TYPE_FLOAT2: {
			dimensions[0] = 2;
			dimension_count = 1;
			float_values = value->float2;
		}	break;
		case PBJ_TYPE_FLOAT3: {
			dimensions[0] = 3;
			dimension_count = 1;
			float_values = value->float3;
		}	break;
		case PBJ_TYPE_FLOAT4: {
			dimensions[0] = 4;
			dimension_count = 1;
			float_values = value->float4;
		}	break;
		case PBJ_TYPE_FLOAT2X2: {
			dimensions[0] = 2;
			dimensions[1] = 2;
			dimension_count = 2;
			float_values = value->float2x2;
		}	break;
		case PBJ_TYPE_FLOAT3X3: {
			dimensions[0] = 4;
			dimensions[1] = 3;
			dimension_count = 2;
			float_values = value->float3x3;
		}	break;
		case PBJ_TYPE_FLOAT4X4: {
			dimensions[0] = 4;
			dimensions[1] = 4;
			dimension_count = 2;
			float_values = value->float4x4;
		}	break;
		default:
			qb_abort("invalid type id: %d", value->type);
	}

	if(int_values) {
		address = qb_create_constant_array(cxt, QB_TYPE_S32, dimensions, dimension_count);
		for(i = 0; i < dimensions[0]; i++) {
			ARRAY(S32, address)[i] = int_values[i];
		}
	} else if(float_values) {
		uint32_t value_count = (dimension_count == 1) ? dimensions[0] : dimensions[1] * dimensions[0];
		address = qb_create_constant_array(cxt, QB_TYPE_F32, dimensions, dimension_count);
		if(value_count == 9) {
			// 3x3 matrices are padded
			for(i = 0, j = 0; i < value_count; i++, j++) {
				ARRAY(F32, address)[j] = float_values[i];
				if(i == 2 || i == 5 || i == 8) {
					ARRAY(F32, address)[++j] = 0;
				}
			}
		} else {
			for(i = 0; i < value_count; i++) {
				ARRAY(F32, address)[i] = float_values[i];
			}
		}
	}
	return address;
}

static void qb_map_pbj_variables(qb_pbj_translator_context *cxt) {
	uint32_t i, dimension;
	qb_variable *qvar;

	// find the output image
	qvar = qb_find_output(cxt);
	if(!qvar) {
		qb_abort("a parameter must be passed by reference to the function to receive the result");
	} 
	cxt->output_image_address = qvar->address;
	cxt->output_image_width_address = cxt->output_image_address->dimension_addresses[1];
	cxt->output_image_height_address = cxt->output_image_address->dimension_addresses[0];
	cxt->output_image_channel_count_address = cxt->output_image_address->dimension_addresses[2];

	// variables for looping 
	cxt->x_address = qb_create_writable_scalar(cxt->compiler_context, QB_TYPE_U32);
	cxt->y_address = qb_create_writable_scalar(cxt->compiler_context, QB_TYPE_U32);

	// the current coordinate--basically x and y in float 
	dimension = 2;
	cxt->out_coord_address = qb_create_writable_array(cxt->compiler_context, QB_TYPE_F32, &dimension, 1);
	cxt->out_coord_x_address = qb_obtain_array_element(cxt->compiler_context, cxt->out_coord_address, cxt->compiler_context->zero_address, QB_ARRAY_BOUND_CHECK_NONE);
	cxt->out_coord_y_address = qb_obtain_array_element(cxt->compiler_context, cxt->out_coord_address, cxt->compiler_context->one_address, QB_ARRAY_BOUND_CHECK_NONE);

	// sub-array representing the pixel
	cxt->output_image_scanline_address = qb_obtain_array_element(cxt->compiler_context, cxt->output_image_address, cxt->y_address, QB_ARRAY_BOUND_CHECK_NONE);
	cxt->output_image_pixel_address = qb_obtain_array_element(cxt->compiler_context, cxt->output_image_scanline_address, cxt->x_address, QB_ARRAY_BOUND_CHECK_NONE);

	// a temporary array holding the pixel being worked on
	dimension = DIMENSION(cxt->output_image_address, -1);
	cxt->active_pixel_address = qb_create_writable_array(cxt->compiler_context, QB_TYPE_F32, &dimension, 1);

	// hook input images to the texture parameters
	for(i = 0; i < cxt->texture_count; i++) {
		qb_pbj_texture *texture = &cxt->textures[i];
		qvar = qb_find_argument(cxt, texture->name);
		if(qvar) {
			if(qb_is_image(cxt, qvar->address, texture->channel_count)) {
				texture->address = qvar->address;
			} else {
				qb_abort("parameter '%s' is not of the correct type", qvar->name);
			} 
		} else {
			// if there's only one input image, then use that one
			qvar = qb_find_input(cxt);
			if(qvar) {
				texture->address = qvar->address;
			} else {
				qb_abort("input image '%s' must be passed to the function", texture->name);
			}
		}
		// premultiplication is applied to input images with alpha channel
		if(DIMENSION(texture->address, -1) == 4) {
			qb_mark_as_writable(cxt->compiler_context, texture->address);
		}
	}

	for(i = 0; i < cxt->parameter_count; i++) {
		qb_pbj_parameter *parameter = &cxt->parameters[i];
		qb_pbj_register *reg = qb_create_pbj_register(cxt, &parameter->destination);

		if(parameter == cxt->out_pixel) {
			parameter->address = cxt->active_pixel_address;
		} else if(parameter == cxt->out_coord) {
			parameter->address = cxt->out_coord_address;
		} else {
			qvar = qb_find_argument(cxt, parameter->name);
			if(qvar) {
				if(!parameter->input_size_name) {
					uint32_t parameter_type = (parameter->destination.register_id & PBJ_REGISTER_INT) ? QB_TYPE_I32 : QB_TYPE_F32;
					if(qvar->address->type == parameter_type) {
						uint32_t parameter_size, element_count;
						if(parameter->destination.dimension > 1) {
							parameter_size = reg->span * 4;
						} else {
							parameter_size = parameter->destination.channel_count;
						}
						if(SCALAR(qvar->address)) {
							element_count = 1;
						} else {
							if(FIXED_LENGTH(qvar->address)) {
								element_count = ARRAY_SIZE(qvar->address);
							} else {
								element_count = 0;
							}
						}
						if(element_count == parameter_size) {
							parameter->address = qvar->address;
						} else {
							qb_abort("parameter '%s' is not of the correct size", qvar->name);
						}
					} else {
						qb_abort("parameter '%s' is not of the correct type", qvar->name);
					}
				} else {
					qb_abort("parameter '%s' is obtained from the input image and cannot be passed separately", parameter->name);
				}
			} else {
				// see if it has a default value
				if(parameter->default_value.type) {
					parameter->address = qb_create_pbj_constant(cxt->compiler_context, &parameter->default_value);
				} else {
					qb_abort("parameter '%s' does not have a default value and must be passed to the function", parameter->name);
				}
			}
		}

		// hook the address up to the register
		if(parameter->destination.dimension > 1) {
			reg->matrix_address = parameter->address;
			qb_propagate_pbj_matrix_span(cxt, reg);
		} else {
			reg->channel_addresses[parameter->destination.channel_id] = parameter->address;
		}
	}
}

static void qb_translate_pbj_basic_op(qb_pbj_translator_context *cxt, qb_pbj_translator *t, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_result_prototype *result_prototype) {
	qb_produce_op(cxt->compiler_context, t->extra, operands, operand_count, result, NULL, 0, result_prototype);
}

static void qb_translate_pbj_copy(qb_pbj_translator_context *cxt, qb_pbj_translator *t, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_result_prototype *result_prototype) {
	operands[1] = operands[0];
	operands[0] = *result;
	qb_produce_op(cxt->compiler_context, t->extra, operands, 2, result, NULL, 0, result_prototype);
}

static void qb_fetch_pbj_register(qb_pbj_translator_context *cxt, qb_pbj_address *reg_address, qb_operand *operand);
static void qb_fetch_pbj_write_target(qb_pbj_translator_context *cxt, qb_pbj_address *reg_address, qb_operand *result, qb_result_prototype *result_prototype);
static void qb_retire_pbj_write_target(qb_pbj_translator_context *cxt, qb_pbj_address *reg_address, qb_operand *operand);

static void qb_translate_pbj_load_constant(qb_pbj_translator_context *cxt, qb_pbj_translator *t, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_result_prototype *result_prototype) {
	qb_operand *value = &operands[0];
	qb_pbj_constant *constant = &cxt->pbj_op->constant;
	qb_pbj_register *reg = qb_get_pbj_register(cxt, &cxt->pbj_op->destination);
	qb_pbj_address new_reg_address = cxt->pbj_op->destination;
	qb_primitive_type expr_type;
	qb_result_prototype value_prototype;

	// see if other load-constant ops follow that write into consecutive locations
	uint32_t next_reg_id = new_reg_address.register_id;
	qb_pbj_channel_id next_channel = new_reg_address.channel_id;
	qb_pbj_channel_id first_channel = new_reg_address.channel_id;
	uint32_t constant_count = 1;
	uint32_t pop_index = cxt->pbj_op_index;
	qb_pbj_op *pop = cxt->pbj_op;
	while(pop_index + 1 < cxt->pbj_op_count && constant_count < 16) {
		pop++;
		pop_index++;
		if(next_channel < PBJ_CHANNEL_A) {
			next_channel++;
		} else {
			next_channel = PBJ_CHANNEL_R;
			next_reg_id++;
		}
		if(pop->opcode != PBJ_LOAD_CONSTANT) {
			break;
		} else if(pop->destination.register_id != next_reg_id || pop->destination.channel_id != next_channel) {
			if(constant_count != 3 || constant_count != 6) {
				break;
			}
		} else {
			constant_count++;
		}
	}

	if(constant_count > 1) {
		// see when the constant may fit
		switch(first_channel) {
			case 0: {
				if(reg->matrix_address && ((constant_count >= ARRAY_SIZE(reg->matrix_address)) || (constant_count >= 9 && ARRAY_SIZE(reg->matrix_address) == 12))) {
					constant_count = ARRAY_SIZE(reg->matrix_address);
					if(constant_count == 12) {
						constant_count = 9;
					}
					new_reg_address.dimension = DIMENSION(reg->matrix_address, -1);
					new_reg_address.channel_id = PBJ_CHANNEL_MATRIX;
				} else if(reg->channel_addresses[PBJ_CHANNEL_RGBA] && constant_count >= 4) {
					constant_count = 4;
					new_reg_address.channel_count = 4;
					new_reg_address.channel_id = PBJ_CHANNEL_RGBA;
				} else if(reg->channel_addresses[PBJ_CHANNEL_RGB] && constant_count >= 3) {
					constant_count = 3;
					new_reg_address.channel_count = 3;
					new_reg_address.channel_id = PBJ_CHANNEL_RGB;
				} else if(reg->channel_addresses[PBJ_CHANNEL_RG] && constant_count >= 2) {
					constant_count = 2;
					new_reg_address.channel_count = 2;
					new_reg_address.channel_id = PBJ_CHANNEL_RG;
				}
			}	break;
			case 1: {
				if(reg->channel_addresses[PBJ_CHANNEL_GBA] && constant_count >= 3) {
					constant_count = 3;
					new_reg_address.channel_count = 3;
					new_reg_address.channel_id = PBJ_CHANNEL_GBA;
				} else if(reg->channel_addresses[PBJ_CHANNEL_GB] && constant_count >= 2) {
					constant_count = 2;
					new_reg_address.channel_count = 2;
					new_reg_address.channel_id = PBJ_CHANNEL_GB;
				}
			}	break;
			case 2: {
				 if(reg->channel_addresses[PBJ_CHANNEL_BA] && constant_count >= 2) {
					constant_count = 2;
					new_reg_address.channel_count = 2;
					new_reg_address.channel_id = PBJ_CHANNEL_BA;
				}
			}	break;
			default: {
			}	break;
		}
	}
	if(new_reg_address.channel_id <= PBJ_CHANNEL_A) {
		constant_count = 1;
	}

	switch(constant->type) {
		case PBJ_TYPE_INT: expr_type = QB_TYPE_S32; break;
		case PBJ_TYPE_FLOAT: expr_type = QB_TYPE_F32; break;
		default: break;
	}

	if(cxt->compiler_context->stage == QB_STAGE_RESULT_TYPE_RESOLUTION) {
		value_prototype.preliminary_type = value_prototype.final_type = expr_type;
		value_prototype.address_flags = QB_ADDRESS_CONSTANT;
		value_prototype.coercion_flags = 0;
		value_prototype.destination = NULL;
		value->result_prototype = &value_prototype;
		value->type = QB_OPERAND_RESULT_PROTOTYPE;
	} else if(cxt->compiler_context->stage == QB_STAGE_OPCODE_TRANSLATION) {
		if(constant_count == 1) {
			switch(expr_type) {
				case QB_TYPE_S32: value->address = qb_obtain_constant_S32(cxt->compiler_context, constant->int_value); break;
				case QB_TYPE_F32: value->address = qb_obtain_constant_F32(cxt->compiler_context, constant->float_value); break;
				default: break;
			}
		} else {
			qb_pbj_op *pop = cxt->pbj_op;
			uint32_t i, j;
			uint32_t dimensions[2], dimension_count;
			if(new_reg_address.dimension > 1) {
				if(new_reg_address.dimension == 3) {
					dimensions[0] = 4;
					dimensions[1] = 3;
				} else {
					dimensions[0] = dimensions[1] = new_reg_address.dimension;
				}
				dimension_count = 2;
			} else {
				dimensions[0] = new_reg_address.channel_count;
				dimension_count = 1;
			}

			value->address = qb_create_constant_array(cxt->compiler_context, expr_type, dimensions, dimension_count);
			for(i = 0, j = 0; i < constant_count; i++, j++) {
				if(constant_count == 9 && (i == 3 || i == 6)) {
					// padding for 3x3 matrices
					j++;
				}
				switch(expr_type) {
					case QB_TYPE_S32: ARRAY(S32, value->address)[j] = pop->constant.int_value; break;
					case QB_TYPE_F32: ARRAY(F32, value->address)[j] = pop->constant.float_value; break;
					default: break;
				}
				pop++;
			}
		}
		value->type = QB_OPERAND_ADDRESS;
	}
	// jump over additional ops that are processed
	cxt->pbj_op_index += (constant_count - 1);

	qb_fetch_pbj_write_target(cxt, &new_reg_address, result, result_prototype);
	qb_translate_pbj_copy(cxt, t, value, 1, result, result_prototype);
	qb_retire_pbj_write_target(cxt, &new_reg_address, result);
}

static void qb_translate_pbj_if(qb_pbj_translator_context *cxt, qb_pbj_translator *t, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_result_prototype *result_prototype) {
	// jump to the instruction immediately following the else or end-if if the condition is false
	uint32_t target_indices[2] = { JUMP_TARGET_INDEX(cxt->loop_op_index + cxt->pbj_op_index + 1, 0), JUMP_TARGET_INDEX(cxt->loop_op_index + cxt->pbj_op->branch_target_index + 1, 0) };
	qb_produce_op(cxt->compiler_context, t->extra, operands, operand_count, result, target_indices, 2, result_prototype);
}

static void qb_translate_pbj_else(qb_pbj_translator_context *cxt, qb_pbj_translator *t, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_result_prototype *result_prototype) {
	// jump over the else block
	uint32_t target_indices[1] = { JUMP_TARGET_INDEX(cxt->loop_op_index + cxt->pbj_op->branch_target_index + 1, 0) };
	qb_produce_op(cxt->compiler_context, t->extra, operands, operand_count, result, target_indices, 1, result_prototype);
}

static void qb_translate_pbj_end_if(qb_pbj_translator_context *cxt, qb_pbj_translator *t, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_result_prototype *result_prototype) {
	// do nothing
}

#define PBJ_RS				PBJ_READ_SOURCE
#define PBJ_WD				PBJ_WRITE_DESTINATION
#define PBJ_RI_RS_WD		(PBJ_READ_IMAGE | PBJ_READ_SOURCE | PBJ_WRITE_DESTINATION)
#define PBJ_RS_WD			(PBJ_READ_SOURCE | PBJ_WRITE_DESTINATION)
#define PBJ_RS_RD1_WD		(PBJ_READ_SOURCE | PBJ_READ_DESTINATION_FIRST | PBJ_WRITE_DESTINATION)
#define PBJ_RS_RD1_WDS		(PBJ_READ_SOURCE | PBJ_READ_DESTINATION_FIRST | PBJ_WRITE_DESTINATION | PBJ_WRITE_SCALAR)
#define PBJ_RS_RD2_WD		(PBJ_READ_SOURCE | PBJ_READ_DESTINATION | PBJ_WRITE_DESTINATION)
#define PBJ_RS_RD1_WB		(PBJ_READ_SOURCE | PBJ_READ_DESTINATION_FIRST | PBJ_WRITE_BOOLEAN)
#define PBJ_RS_RS2_RS3_WD	(PBJ_READ_SOURCE | PBJ_READ_SOURCE2 | PBJ_READ_SOURCE3 | PBJ_WRITE_DESTINATION)

static qb_pbj_translator pbj_op_translators[] = {
	{	NULL,										0,					NULL								},	// PBJ_NOP
	{	qb_translate_pbj_basic_op,					PBJ_RS_RD1_WD,		&factory_add						},	// PBJ_ADD
	{	qb_translate_pbj_basic_op,					PBJ_RS_RD1_WD,		&factory_subtract					},	// PBJ_SUBTRACT
	{	qb_translate_pbj_basic_op,					PBJ_RS_RD1_WD,		&factory_multiply					},	// PBJ_MULTIPLY
	{	qb_translate_pbj_basic_op,					PBJ_RS_WD,			&factory_reciprocal					},	// PBJ_RECIPROCAL
	{	qb_translate_pbj_basic_op,					PBJ_RS_RD1_WD,		&factory_divide						},	// PBJ_DIVIDE
	{	qb_translate_pbj_basic_op,					PBJ_RS_RD1_WD,		&factory_atan2						},	// PBJ_ATAN2
	{	qb_translate_pbj_basic_op,					PBJ_RS_RD1_WD,		&factory_pow						},	// PBJ_POW
	{	qb_translate_pbj_basic_op,					PBJ_RS_RD1_WD,		&factory_floor_modulo				},	// PBJ_MOD
	{	qb_translate_pbj_basic_op,					PBJ_RS_RD1_WD,		&factory_min_two					},	// PBJ_MIN
	{	qb_translate_pbj_basic_op,					PBJ_RS_RD1_WD,		&factory_max_two					},	// PBJ_MAX
	{	qb_translate_pbj_basic_op,					PBJ_RS_RD1_WD,		&factory_step						},	// PBJ_STEP
	{	qb_translate_pbj_basic_op,					PBJ_RS_WD,			&factory_sin						},	// PBJ_SIN
	{	qb_translate_pbj_basic_op,					PBJ_RS_WD,			&factory_cos						},	// PBJ_COS
	{	qb_translate_pbj_basic_op,					PBJ_RS_WD,			&factory_tan						},	// PBJ_TAN
	{	qb_translate_pbj_basic_op,					PBJ_RS_WD,			&factory_asin						},	// PBJ_ASIN
	{	qb_translate_pbj_basic_op,					PBJ_RS_WD,			&factory_acos						},	// PBJ_ACOS
	{	qb_translate_pbj_basic_op,					PBJ_RS_WD,			&factory_atan						},	// PBJ_ATAN
	{	qb_translate_pbj_basic_op,					PBJ_RS_WD,			&factory_exp						},	// PBJ_EXP
	{	qb_translate_pbj_basic_op,					PBJ_RS_WD,			&factory_exp2						},	// PBJ_EXP2
	{	qb_translate_pbj_basic_op,					PBJ_RS_WD,			&factory_log						},	// PBJ_LOG
	{	qb_translate_pbj_basic_op,					PBJ_RS_WD,			&factory_log2						},	// PBJ_LOG2
	{	qb_translate_pbj_basic_op,					PBJ_RS_WD,			&factory_sqrt						},	// PBJ_SQRT
	{	qb_translate_pbj_basic_op,					PBJ_RS_WD,			&factory_rsqrt						},	// PBJ_RSQRT
	{	qb_translate_pbj_basic_op,					PBJ_RS_WD,			&factory_abs						},	// PBJ_ABS
	{	qb_translate_pbj_basic_op,					PBJ_RS_WD,			&factory_sign						},	// PBJ_SIGN
	{	qb_translate_pbj_basic_op,					PBJ_RS_WD,			&factory_floor						},	// PBJ_FLOOR
	{	qb_translate_pbj_basic_op,					PBJ_RS_WD,			&factory_ceil						},	// PBJ_CEIL
	{	qb_translate_pbj_basic_op,					PBJ_RS_WD,			&factory_fract						},	// PBJ_FRACT
	{	qb_translate_pbj_copy,						PBJ_RS_WD,			&factory_assign						},	// PBJ_COPY
	{	qb_translate_pbj_copy,						PBJ_RS_WD,			&factory_assign						},	// PBJ_FLOAT_TO_INT
	{	qb_translate_pbj_copy,						PBJ_RS_WD,			&factory_assign						},	// PBJ_INT_TO_FLOAT
	{	qb_translate_pbj_basic_op,					PBJ_RS_RD2_WD,		&factory_mm_mult_cm					},	// PBJ_MATRIX_MATRIX_MULTIPLY
	{	qb_translate_pbj_basic_op,					PBJ_RS_RD1_WD,		&factory_vm_mult_cm					},	// PBJ_VECTOR_MATRIX_MULTIPLY
	{	qb_translate_pbj_basic_op,					PBJ_RS_RD2_WD,		&factory_mv_mult_cm					},	// PBJ_MATRIX_VECTOR_MULTIPLY
	{	qb_translate_pbj_basic_op,					PBJ_RS_WD,			&factory_normalize					},	// PBJ_NORMALIZE
	{	qb_translate_pbj_basic_op,					PBJ_RS_WD,			&factory_length						},	// PBJ_LENGTH
	{	qb_translate_pbj_basic_op,					PBJ_RS_RD1_WDS,		&factory_distance					},	// PBJ_DISTANCE
	{	qb_translate_pbj_basic_op,					PBJ_RS_RD1_WDS,		&factory_dot_product				},	// PBJ_DOT_PRODUCT
	{	qb_translate_pbj_basic_op,					PBJ_RS_RD1_WD,		&factory_cross_product				},	// PBJ_CROSS_PRODUCT
	{	qb_translate_pbj_basic_op,					PBJ_RS_RD1_WB,		&factory_equal						},	// PBJ_EQUAL
	{	qb_translate_pbj_basic_op,					PBJ_RS_RD1_WB,		&factory_not_equal					},	// PBJ_NOT_EQUAL
	{	qb_translate_pbj_basic_op,					PBJ_RS_RD1_WB,		&factory_set_or_scalar_less_than	},	// PBJ_LESS_THAN
	{	qb_translate_pbj_basic_op,					PBJ_RS_RD1_WB,		&factory_set_or_scalar_less_equal	},	// PBJ_LESS_THAN_EQUAL
	{	qb_translate_pbj_basic_op,					PBJ_RS_WD,			&factory_logical_not				},	// PBJ_LOGICAL_NOT
	{	qb_translate_pbj_basic_op,					PBJ_RS_RD1_WD,		&factory_logical_and				},	// PBJ_LOGICAL_AND
	{	qb_translate_pbj_basic_op,					PBJ_RS_RD1_WD,		&factory_logical_or					},	// PBJ_LOGICAL_OR
	{	qb_translate_pbj_basic_op,					PBJ_RS_RD1_WD,		&factory_logical_xor				},	// PBJ_LOGICAL_XOR
	{	qb_translate_pbj_basic_op,					PBJ_RI_RS_WD,		&factory_sample_nearest_vector		},	// PBJ_SAMPLE_NEAREST
	{	qb_translate_pbj_basic_op,					PBJ_RI_RS_WD,		&factory_sample_bilinear_vector		},	// PBJ_SAMPLE_BILINEAR
	{	qb_translate_pbj_load_constant,				0,					&factory_assign,					},	// PBJ_LOAD_CONSTANT
	{	qb_translate_pbj_basic_op,					PBJ_RS_RS2_RS3_WD,	&factory_select,					},	// PBJ_SELECT
	{	qb_translate_pbj_if,						PBJ_RS,				&factory_branch_on_true,			},	// PBJ_IF
	{	qb_translate_pbj_else,						0,					&factory_jump,						},	// PBJ_ELSE
	{	qb_translate_pbj_end_if,					0,					NULL								},	// PBJ_END_IF
	{	qb_translate_pbj_basic_op,					0,					&factory_not_equal					},	// PBJ_FLOAT_TO_BOOL
	{	qb_translate_pbj_copy,						PBJ_RS_WD,			&factory_assign						},	// PBJ_BOOL_TO_FLOAT
	{	qb_translate_pbj_basic_op,					0,					&factory_not_equal					},	// PBJ_INT_TO_BOOL
	{	NULL,										0,					NULL								},	// PBJ_BOOL_TO_INT
	{	qb_translate_pbj_basic_op,					PBJ_RS_RD1_WB,		&factory_set_equal					},	// PBJ_VECTOR_EQUAL
	{	qb_translate_pbj_basic_op,					PBJ_RS_RD1_WB,		&factory_set_not_equal				},	// PBJ_VECTOR_NOT_EQUAL
	{	qb_translate_pbj_basic_op,					PBJ_RS_WD,			&factory_any						},	// PBJ_ANY
	{	qb_translate_pbj_basic_op,					PBJ_RS_WD,			&factory_all						},	// PBJ_ALL
	{	qb_translate_pbj_basic_op,					PBJ_RS_RS2_RS3_WD,	&factory_smooth_step				},	// PBJ_SMOOTH_STEP
};

static void qb_perform_gather(qb_pbj_translator_context *cxt, qb_address *src_address, qb_address *dst_address, uint32_t mask);
static void qb_perform_scatter(qb_pbj_translator_context *cxt, qb_address *src_address, qb_address *dst_address, uint32_t mask);

static void qb_fetch_pbj_register(qb_pbj_translator_context *cxt, qb_pbj_address *reg_address, qb_operand *operand) {
	qb_pbj_register *reg = qb_get_pbj_register(cxt, reg_address);
	if(reg_address->dimension > 1) {
		operand->address = reg->matrix_address;
		operand->type = QB_OPERAND_ADDRESS;
	} else {
		qb_address *channel_address = reg->channel_addresses[reg_address->channel_id];
		if(reg_address->channel_mask != INVALID_INDEX) {
			if(cxt->compiler_context->stage == QB_STAGE_OPCODE_TRANSLATION) {
				qb_variable_dimensions dim = { 1, qb_obtain_constant_U32(cxt->compiler_context, reg_address->channel_count) };
				qb_address *gather_address = qb_obtain_temporary_variable(cxt->compiler_context, channel_address->type, &dim);
				qb_perform_gather(cxt, channel_address, gather_address, reg_address->channel_mask);
				qb_lock_address(cxt->compiler_context, gather_address);
				operand->address = gather_address;
			} else {
				operand->address = channel_address;
			}
		} else {
			operand->address = channel_address;
		}
		operand->type = QB_OPERAND_ADDRESS;
	}
}

static void qb_fetch_pbj_write_target(qb_pbj_translator_context *cxt, qb_pbj_address *reg_address, qb_operand *result, qb_result_prototype *result_prototype) {
	qb_pbj_register *reg = qb_get_pbj_register(cxt, reg_address);
	if(reg_address->dimension > 1) {
		result->address = reg->matrix_address;
	} else {
		result->address = reg->channel_addresses[reg_address->channel_id];
	}
	result->type = QB_OPERAND_ADDRESS;
}

static void qb_retire_pbj_write_target(qb_pbj_translator_context *cxt, qb_pbj_address *reg_address, qb_operand *operand) {
	if(reg_address->channel_mask != INVALID_INDEX) {
		qb_pbj_register *reg = qb_get_pbj_register(cxt, reg_address);
		qb_address *channel_address = reg->channel_addresses[reg_address->channel_id];
		if(cxt->compiler_context->stage == QB_STAGE_OPCODE_TRANSLATION) {				
			qb_perform_scatter(cxt, operand->address, channel_address, reg_address->channel_mask);
		}
	}
}

static void qb_translate_current_pbj_instruction(qb_pbj_translator_context *cxt) {
	qb_pbj_op *pop = cxt->pbj_op;
	if(pop->opcode != PBJ_OP_DATA && pop->opcode != PBJ_NOP) {
		qb_pbj_translator *t = &pbj_op_translators[pop->opcode];
		if(t->translate) {
			qb_result_prototype *result_prototype = &cxt->result_prototypes[cxt->loop_op_index + cxt->pbj_op_index];
			qb_operand operands[4]; 
			qb_operand result;
			qb_pbj_address scalar_destination, comparison_result, *result_pbj_address = NULL;
			uint32_t operand_count = 0;

			if(t->flags & PBJ_READ_IMAGE) {
				qb_pbj_texture *texture = qb_find_pbj_texture_by_id(cxt, pop->image_id);
				qb_operand *image = &operands[operand_count++];
				image->address = texture->address;
				image->type = QB_OPERAND_ADDRESS;
			}
			if(t->flags & PBJ_READ_DESTINATION_FIRST) {
				qb_fetch_pbj_register(cxt, (pop->destination_origin) ? pop->destination_origin : &pop->destination, &operands[operand_count++]);
			}
			if(t->flags & PBJ_READ_SOURCE) {
				qb_fetch_pbj_register(cxt, &pop->source, &operands[operand_count++]);
			} 
			if(t->flags & PBJ_READ_SOURCE2) {
				qb_pbj_op *data_pop = pop + 1;
				qb_fetch_pbj_register(cxt, &data_pop->source2, &operands[operand_count++]);
				if(t->flags & PBJ_READ_SOURCE3) {
					qb_fetch_pbj_register(cxt, &data_pop->source3, &operands[operand_count++]);
				}
			}
			if(t->flags & PBJ_READ_DESTINATION) {
				qb_fetch_pbj_register(cxt, (pop->destination_origin) ? pop->destination_origin : &pop->destination, &operands[operand_count++]);
			}
			if(t->flags & (PBJ_WRITE_DESTINATION | PBJ_WRITE_BOOLEAN)) {
				if(t->flags & PBJ_WRITE_SCALAR) {
					// the op writes to the first channel of the destination only (e.g. dot product)
					if(pop->eventual_destination) {
						result_pbj_address = pop->eventual_destination;
					} else {
						scalar_destination = pop->destination;
						scalar_destination.channel_id = qb_get_first_pbj_channel(cxt, pop->destination.channel_id);
						result_pbj_address = &scalar_destination;
					}
				} else if(t->flags & PBJ_WRITE_BOOLEAN) {
					if(pop->eventual_destination) {
						result_pbj_address = pop->eventual_destination;
					} else {
						// the op writes to integer register #1
						comparison_result.dimension = 1;
						comparison_result.register_id = 0 | PBJ_REGISTER_INT;
						comparison_result.channel_id = PBJ_CHANNEL_R;
						comparison_result.channel_mask = INVALID_INDEX;
						switch(pop->source.channel_count) {
							case 1: comparison_result.channel_id = PBJ_CHANNEL_R; break;
							case 2: comparison_result.channel_id = PBJ_CHANNEL_RG; break;
							case 3: comparison_result.channel_id = PBJ_CHANNEL_RGB; break;
							case 4: comparison_result.channel_id = PBJ_CHANNEL_RGBA; break;
						}
						result_pbj_address = &comparison_result;
					}
				} else {
					result_pbj_address = (pop->eventual_destination) ? pop->eventual_destination : &pop->destination;
				}
				qb_fetch_pbj_write_target(cxt, result_pbj_address, &result, result_prototype);
			}

			t->translate(cxt, t, operands, operand_count, &result, result_prototype);

			if(t->flags & (PBJ_WRITE_DESTINATION | PBJ_WRITE_BOOLEAN)) {
				qb_retire_pbj_write_target(cxt, result_pbj_address, &result);
			}
		}
	}
}

static void qb_allocate_pbj_register(qb_pbj_translator_context *cxt, qb_pbj_address *reg_address) {
	qb_pbj_register *reg = qb_create_pbj_register(cxt, reg_address);
	qb_primitive_type element_type = (reg_address->register_id & PBJ_REGISTER_INT) ? QB_TYPE_I32 : QB_TYPE_F32;
	if(reg_address->dimension > 1) {
		if(!reg->matrix_address) {
			uint32_t size = reg->span * 4;
			uint32_t dimensions[2] = { reg_address->dimension, size / reg_address->dimension };
			reg->matrix_address = qb_create_writable_array(cxt->compiler_context, element_type, dimensions, 2);
			qb_propagate_pbj_matrix_span(cxt, reg);
		}
	} else {
		if(!reg->channel_addresses[reg_address->channel_id]) {
			qb_address *channel_address, *src_address = NULL;
			qb_pbj_channel_id first_channel = qb_get_first_pbj_channel(cxt, reg_address->channel_id);
			qb_pbj_channel_id parent_channel;
			uint32_t offset;
			uint32_t channel_count;

			// find a wider channel that contains this one
			for(parent_channel = PBJ_CHANNEL_RGBA; parent_channel > reg_address->channel_id; parent_channel--) {
				if(reg->channel_addresses[parent_channel]) {
					offset = qb_get_pbj_channel_offset(cxt, parent_channel, reg_address->channel_id);
					if(offset != INVALID_INDEX) {
						src_address = reg->channel_addresses[parent_channel];
						break;
					}
				}
			}

			if(reg_address->channel_mask == INVALID_INDEX) {
				channel_count = reg_address->channel_count;
			} else {
				channel_count = qb_get_pbj_channel_count(cxt, reg_address->channel_id);
			}

			if(src_address) {
				qb_address *index_address = qb_obtain_constant_U32(cxt->compiler_context, offset);
				if(channel_count == 1) {
					channel_address = qb_obtain_array_element(cxt->compiler_context, src_address, index_address, QB_ARRAY_BOUND_CHECK_NONE);
				} else {
					qb_address *length_address = qb_obtain_constant_U32(cxt->compiler_context, reg_address->channel_count);
					channel_address = qb_obtain_array_slice(cxt->compiler_context, src_address, index_address, length_address, QB_ARRAY_BOUND_CHECK_NONE);
				}
			} else {
				qb_primitive_type type = (reg_address->register_id & PBJ_REGISTER_INT) ? QB_TYPE_S32 : QB_TYPE_F32;
				if(channel_count == 1) {
					channel_address = qb_create_writable_scalar(cxt->compiler_context, type);
				} else {
					channel_address = qb_create_writable_array(cxt->compiler_context, type, &channel_count, 1);
				}
			}
			reg->channel_addresses[reg_address->channel_id] = channel_address;
#ifdef ZEND_DEBUG
			{
				uint32_t expected_length = qb_get_pbj_channel_count(cxt, reg_address->channel_id);
				uint32_t actual_length = ARRAY_SIZE(channel_address);
				if(expected_length != actual_length) {
					qb_abort("Error");
				}
			}
#endif
		}
	}
}

static void qb_allocate_pbj_registers_in_range(qb_pbj_translator_context *cxt, qb_pbj_channel_id first, qb_pbj_channel_id last) {
	uint32_t i;
	for(i = 0; i < cxt->pbj_op_count; i++) {
		qb_pbj_op *pop = &cxt->pbj_ops[i];
		if(pop->flags & PBJ_SOURCE_IN_USE) {
			if(first <= pop->source.channel_id && pop->source.channel_id <= last) {
				qb_allocate_pbj_register(cxt, &pop->source);
			}
		}
		if(pop->flags & PBJ_DESTINATION_IN_USE) {
			if(first <= pop->destination.channel_id && pop->destination.channel_id <= last) {
				qb_allocate_pbj_register(cxt, &pop->destination);
			}
		}
	}
}

static void qb_allocate_pbj_registers(qb_pbj_translator_context *cxt) {
	// allocate matrices first, since they can spill into multiple registers
	qb_allocate_pbj_registers_in_range(cxt, PBJ_CHANNEL_MATRIX, PBJ_CHANNEL_MATRIX);

	// allocate vectors, starting from the largest
	qb_allocate_pbj_registers_in_range(cxt, PBJ_CHANNEL_RGBA, PBJ_CHANNEL_RGBA);
	qb_allocate_pbj_registers_in_range(cxt, PBJ_CHANNEL_RGB, PBJ_CHANNEL_GBA);
	qb_allocate_pbj_registers_in_range(cxt, PBJ_CHANNEL_RG, PBJ_CHANNEL_BA);

	// allocate scalars
	qb_allocate_pbj_registers_in_range(cxt, PBJ_CHANNEL_R, PBJ_CHANNEL_A);
}

static void qb_perform_assignment(qb_pbj_translator_context *cxt, qb_address *dst_address, qb_address *src_address) {
	qb_operand operands[2] = { { QB_OPERAND_ADDRESS, { dst_address } }, { QB_OPERAND_ADDRESS, { src_address } } };
	qb_operand result = { QB_OPERAND_EMPTY, { NULL } };
	qb_set_source_op_index(cxt->compiler_context, cxt->loop_op_index, 0);
	qb_produce_op(cxt->compiler_context, &factory_assign, operands, 2, &result, NULL, 0, &cxt->result_prototypes[cxt->loop_op_index++]);
}

static void qb_perform_subtraction(qb_pbj_translator_context *cxt, qb_address *dst_address, qb_address *src_address) {
	qb_operand operands[2] = { { QB_OPERAND_ADDRESS, { dst_address } }, { QB_OPERAND_ADDRESS, { src_address } } };
	qb_operand result = { QB_OPERAND_EMPTY, { NULL } };
	qb_set_source_op_index(cxt->compiler_context, cxt->loop_op_index, 0);
	qb_produce_op(cxt->compiler_context, factories_subtract_assign[0], operands, 2, &result, NULL, 0, &cxt->result_prototypes[cxt->loop_op_index++]);
}

static void qb_perform_increment(qb_pbj_translator_context *cxt, qb_address *dst_address) {
	qb_operand operand = { QB_OPERAND_ADDRESS, { dst_address } };
	qb_operand result = { QB_OPERAND_EMPTY, { NULL } };
	qb_set_source_op_index(cxt->compiler_context, cxt->loop_op_index, 0);
	qb_produce_op(cxt->compiler_context, &factory_increment_pre, &operand, 1, &result, NULL, 0, &cxt->result_prototypes[cxt->loop_op_index++]);
}

static void qb_perform_alpha_premultication(qb_pbj_translator_context *cxt, qb_address *dst_address) {
	qb_operand operand = { QB_OPERAND_ADDRESS, { dst_address } };
	qb_operand result = { QB_OPERAND_ADDRESS, { dst_address } };
	qb_set_source_op_index(cxt->compiler_context, cxt->loop_op_index, 0);
	qb_produce_op(cxt->compiler_context, &factory_apply_premult, &operand, 1, &result, NULL, 0, &cxt->result_prototypes[cxt->loop_op_index++]);
}

static void qb_perform_alpha_premultiplication_removal(qb_pbj_translator_context *cxt, qb_address *dst_address) {
	qb_operand operand = { QB_OPERAND_ADDRESS, { dst_address } };
	qb_operand result = { QB_OPERAND_ADDRESS, { dst_address } };
	qb_set_source_op_index(cxt->compiler_context, cxt->loop_op_index, 0);
	qb_produce_op(cxt->compiler_context, &factory_remove_premult, &operand, 1, &result, NULL, 0, &cxt->result_prototypes[cxt->loop_op_index++]);
}

static void qb_perform_return(qb_pbj_translator_context *cxt) {
	qb_operand operand = { QB_OPERAND_NONE, { NULL } };
	qb_operand result = { QB_OPERAND_EMPTY, { NULL } };
	qb_set_source_op_index(cxt->compiler_context, cxt->loop_op_index, 0);
	qb_produce_op(cxt->compiler_context, &factory_return, &operand, 1, &result, NULL, 0, &cxt->result_prototypes[cxt->loop_op_index++]);
}

static void qb_perform_loop(qb_pbj_translator_context *cxt, qb_address *index_address, qb_address *limit_address, uint32_t target_op_index) {
	qb_operand operands[2] = { { QB_OPERAND_ADDRESS, { index_address } }, { QB_OPERAND_ADDRESS, { limit_address } } };
	qb_operand result = { QB_OPERAND_EMPTY, { NULL } };
	uint32_t target_indices[2] = { JUMP_TARGET_INDEX(target_op_index, 0), JUMP_TARGET_INDEX(cxt->loop_op_index + 1, 0) };
	qb_set_source_op_index(cxt->compiler_context, cxt->loop_op_index, 0);
	qb_produce_op(cxt->compiler_context, &factory_loop, operands, 2, &result, target_indices, 2, &cxt->result_prototypes[cxt->loop_op_index++]);
}

static void qb_perform_branch(qb_pbj_translator_context *cxt, qb_address *condition_address, uint32_t target_op_index) {
	qb_operand operand = { QB_OPERAND_ADDRESS, { condition_address } };
	qb_operand result = { QB_OPERAND_NONE, { NULL } };
	uint32_t target_indices[2] = { JUMP_TARGET_INDEX(target_op_index, 0), JUMP_TARGET_INDEX(cxt->loop_op_index + cxt->pbj_op_index + 1, 0) };
	qb_set_source_op_index(cxt->compiler_context, cxt->loop_op_index, 0);
	qb_produce_op(cxt->compiler_context, &factory_branch_on_true, &operand, 1, &result, target_indices, 2, &cxt->result_prototypes[cxt->loop_op_index++]);
}

static void qb_perform_jump(qb_pbj_translator_context *cxt, uint32_t target_op_index) {
	qb_operand result = { QB_OPERAND_NONE, { NULL } };
	uint32_t target_indices[1] = { JUMP_TARGET_INDEX(target_op_index, 0) }; 
	qb_set_source_op_index(cxt->compiler_context, cxt->loop_op_index, 0);
	qb_produce_op(cxt->compiler_context, &factory_jump, NULL, 0, &result, target_indices, 1, &cxt->result_prototypes[cxt->loop_op_index++]);
}

static void qb_perform_nop(qb_pbj_translator_context *cxt) {
	qb_set_source_op_index(cxt->compiler_context, cxt->loop_op_index, 0);
	qb_create_op(cxt->compiler_context, &factory_nop, QB_TYPE_VOID, NULL, 0, NULL, NULL, 0, FALSE);
}

static void qb_perform_gather(qb_pbj_translator_context *cxt, qb_address *src_address, qb_address *dst_address, uint32_t mask) {
	qb_operand operands[3] = { { QB_OPERAND_ADDRESS, { dst_address } }, { QB_OPERAND_ADDRESS, { src_address } }, { QB_OPERAND_NUMBER, { NULL } } };
	qb_operand result = { QB_OPERAND_EMPTY, { NULL } };
	operands[2].number = mask;
	qb_set_source_op_index(cxt->compiler_context, cxt->loop_op_index, 0);
	qb_produce_op(cxt->compiler_context, &factory_gather, operands, 3, &result, NULL, 0, NULL);
}

static void qb_perform_scatter(qb_pbj_translator_context *cxt, qb_address *src_address, qb_address *dst_address, uint32_t mask) {
	qb_operand operands[3] = { { QB_OPERAND_ADDRESS, { dst_address } }, { QB_OPERAND_ADDRESS, { src_address } }, { QB_OPERAND_NUMBER, { NULL } } };
	qb_operand result = { QB_OPERAND_EMPTY, { NULL } };
	operands[2].number = mask;
	qb_set_source_op_index(cxt->compiler_context, cxt->loop_op_index, 0);
	qb_produce_op(cxt->compiler_context, &factory_scatter, operands, 3, &result, NULL, 0, NULL);
}

static void qb_start_pbj_filter_loop(qb_pbj_translator_context *cxt) {
	qb_address *start_coord_address;
	uint32_t i;

	cxt->loop_op_index = 0;

	// apply premultiplication to input images
	for(i = 0; i < cxt->texture_count; i++) {
		qb_pbj_texture *texture = &cxt->textures[i];
		if(DIMENSION(texture->address, -1) == 4) {
			qb_perform_alpha_premultication(cxt, texture->address);
		}
	}

	// set y to zero, but _OutCoord.y to 0.5, as that's the center of the grid
	start_coord_address = qb_obtain_constant_F32(cxt->compiler_context, 0.5);
	qb_perform_assignment(cxt, cxt->y_address, cxt->compiler_context->zero_address);
	qb_perform_assignment(cxt, cxt->out_coord_y_address, start_coord_address);

	// set x to zero, but _OutCoord.x to 0.5
	// the outer loop starts here
	cxt->outer_loop_start_index = cxt->loop_op_index;
	qb_perform_assignment(cxt, cxt->x_address, cxt->compiler_context->zero_address);
	qb_perform_assignment(cxt, cxt->out_coord_x_address, start_coord_address);

	// initialize the active pixel to zero--the inner loop starts here
	cxt->inner_loop_start_index = cxt->loop_op_index;
	qb_perform_subtraction(cxt, cxt->active_pixel_address, cxt->active_pixel_address);
}

static void qb_end_pbj_filter_loop(qb_pbj_translator_context *cxt) {
	qb_operand operand;

	cxt->loop_op_index += cxt->pbj_op_count;

	// copy the output pixel into the image
	qb_fetch_pbj_register(cxt, &cxt->out_pixel->destination, &operand);
	qb_perform_assignment(cxt, cxt->output_image_pixel_address, operand.address);

	// increment _OutCoord.x
	qb_perform_increment(cxt, cxt->out_coord_x_address);

	// jump to beginning of inner loop if x is less than width
	qb_perform_loop(cxt, cxt->x_address, cxt->output_image_width_address, cxt->inner_loop_start_index);

	// increment _OutCoord.y
	qb_perform_increment(cxt, cxt->out_coord_y_address);

	// jump to beginning of outer loop if y is less than height
	qb_perform_loop(cxt, cxt->y_address, cxt->output_image_height_address, cxt->outer_loop_start_index);

	// remove premultiplication from output image if it has an alpha channel
	if(DIMENSION(cxt->output_image_address, -1) == 4) {
		qb_perform_alpha_premultiplication_removal(cxt, cxt->output_image_address);
	}

	qb_perform_return(cxt);
}

static void qb_remove_redundant_pbj_ops(qb_pbj_translator_context *cxt);

void qb_survey_pbj_instructions(qb_pbj_translator_context *cxt) {
	uint32_t i;

	// map function arguments to PB kernel parameters
	qb_map_pbj_variables(cxt);
	
	// allocate registers
	qb_allocate_pbj_registers(cxt);

	// eliminate redundant ops
	qb_remove_redundant_pbj_ops(cxt);

	// initialize result prototypes
	qb_enlarge_array((void **) &cxt->result_prototypes, cxt->pbj_op_count + 16 + cxt->texture_count);
	for(i = 0; i < cxt->result_prototype_count; i++) {
		qb_result_prototype *prototype = &cxt->result_prototypes[i];
		prototype->preliminary_type = prototype->final_type = QB_TYPE_UNKNOWN;
	}

	cxt->compiler_context->stage = QB_STAGE_RESULT_TYPE_RESOLUTION;

	qb_start_pbj_filter_loop(cxt);
	for(cxt->pbj_op_index = 0; cxt->pbj_op_index < cxt->pbj_op_count; cxt->pbj_op_index++) {
		cxt->pbj_op = &cxt->pbj_ops[cxt->pbj_op_index];
		qb_set_source_op_index(cxt->compiler_context, cxt->loop_op_index + cxt->pbj_op_index, cxt->pbj_op_index);
		qb_translate_current_pbj_instruction(cxt);
	}
	qb_end_pbj_filter_loop(cxt);
}

void qb_translate_pbj_instructions(qb_pbj_translator_context *cxt) {
	// interpret 3x4 matrix as 3x3 with padded element
	cxt->compiler_context->matrix_padding = TRUE;
	cxt->compiler_context->stage = QB_STAGE_OPCODE_TRANSLATION;

	// translate the instructions
	qb_start_pbj_filter_loop(cxt);
	for(cxt->pbj_op_index = 0; cxt->pbj_op_index < cxt->pbj_op_count; cxt->pbj_op_index++) {
		cxt->pbj_op = &cxt->pbj_ops[cxt->pbj_op_index];
		qb_set_source_op_index(cxt->compiler_context, cxt->loop_op_index + cxt->pbj_op_index, cxt->pbj_op_index);
		qb_translate_current_pbj_instruction(cxt);
	}
	qb_end_pbj_filter_loop(cxt);
}

void qb_initialize_pbj_translator_context(qb_pbj_translator_context *cxt, qb_compiler_context *compiler_cxt TSRMLS_DC) {
	memset(cxt, 0, sizeof(qb_pbj_translator_context));
	cxt->compiler_context = compiler_cxt;
	cxt->pool = compiler_cxt->pool;
	cxt->storage = compiler_cxt->storage;

	qb_attach_new_array(cxt->pool, (void **) &cxt->conditionals, &cxt->conditional_count, sizeof(qb_pbj_op *), 8);
	qb_attach_new_array(cxt->pool, (void **) &cxt->parameters, &cxt->parameter_count, sizeof(qb_pbj_parameter), 8);
	qb_attach_new_array(cxt->pool, (void **) &cxt->textures, &cxt->texture_count, sizeof(qb_pbj_texture), 4);

	qb_attach_new_array(cxt->pool, (void **) &cxt->int_registers, &cxt->int_register_count, sizeof(qb_pbj_register), 16);
	qb_attach_new_array(cxt->pool, (void **) &cxt->float_registers, &cxt->float_register_count, sizeof(qb_pbj_register), 16);

	qb_attach_new_array(cxt->pool, (void **) &cxt->result_prototypes, &cxt->result_prototype_count, sizeof(qb_result_prototype), 16);

	SAVE_TSRMLS
}

void qb_free_pbj_translator_context(qb_pbj_translator_context *cxt) {
}

static uint32_t qb_pbj_get_channel_mask(qb_pbj_translator_context *cxt, qb_pbj_address *reg_address) {
	uint32_t mask;
	if(reg_address->dimension == 1) {
		switch(reg_address->channel_id) {
			case PBJ_CHANNEL_R: mask = 0x0001; break;
			case PBJ_CHANNEL_G: mask = 0x0002; break;
			case PBJ_CHANNEL_B: mask = 0x0004; break;
			case PBJ_CHANNEL_A: mask = 0x0008; break;
			case PBJ_CHANNEL_RG: mask = 0x0001 | 0x0002; break;
			case PBJ_CHANNEL_GB: mask = 0x0002 | 0x0004; break;
			case PBJ_CHANNEL_BA: mask = 0x0004 | 0x0008; break;
			case PBJ_CHANNEL_RGB: mask = 0x0001 | 0x0002 | 0x0004; break;
			case PBJ_CHANNEL_GBA: mask = 0x0002 | 0x0004 | 0x0008; break;
			case PBJ_CHANNEL_RGBA: mask = 0x0001 | 0x0002 | 0x0004 | 0x0008; break;
			default: break;
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

static int32_t qb_pbj_find_channel_overlap(qb_pbj_translator_context *cxt, qb_pbj_address *reg1_address, qb_pbj_address *reg2_address) {
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

static int32_t qb_find_pbj_address_overlap(qb_pbj_translator_context *cxt, qb_pbj_address *reg1_address, qb_pbj_address *reg2_address) {
	// see if the they're of the same type
	if((reg1_address->register_id & PBJ_REGISTER_INT) == (reg2_address->register_id & PBJ_REGISTER_INT)) {
		// see if the dimension matches
		if(reg1_address->dimension == reg2_address->dimension) {
			if(reg1_address->dimension == 1) {
				// vector
				if(reg1_address->register_id == reg2_address->register_id) {
					if(reg1_address->channel_id == reg2_address->channel_id && reg1_address->channel_mask == reg2_address->channel_mask && reg1_address->channel_count == reg2_address->channel_count) {
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
			static uint32_t matrix_sizes[] = { 0, 1, 4, 12, 16 };
			uint32_t reg1_start = reg1_address->register_id;
			uint32_t reg1_end = reg1_start + matrix_sizes[reg1_address->dimension];
			uint32_t reg2_start = reg2_address->register_id;
			uint32_t reg2_end = reg2_start + matrix_sizes[reg2_address->dimension];

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

static uint32_t qb_get_pbj_op_effect(qb_pbj_translator_context *cxt, qb_pbj_op *pop, qb_pbj_address *address, uint32_t effect_mask) {
	qb_pbj_address *dst[1], *input[4];
	uint32_t i, dst_count = 0, dst_types[1], input_count = 0, input_types[4], flags = 0;
	qb_pbj_translator *t = &pbj_op_translators[pop->opcode];
	qb_pbj_address comparison_register;

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
		} else if(t->flags & PBJ_WRITE_BOOLEAN) {
			comparison_register.channel_count = pop->source.channel_count;
			comparison_register.dimension = 1;
			switch(pop->source.channel_count) {
				case 1: comparison_register.channel_id = PBJ_CHANNEL_R; break;
				case 2: comparison_register.channel_id = PBJ_CHANNEL_RG; break;
				case 3: comparison_register.channel_id = PBJ_CHANNEL_RGB; break;
				case 4: comparison_register.channel_id = PBJ_CHANNEL_RGBA; break;
			}
			comparison_register.register_id = 0 | PBJ_REGISTER_INT;
			comparison_register.channel_mask = INVALID_INDEX;
			dst[dst_count] = &comparison_register;
			dst_types[dst_count] = PBJ_AS_BOOL;
			dst_count++;
		}
	}
	for(i = 0; i < input_count; i++) {
		uint32_t overlap = qb_find_pbj_address_overlap(cxt, input[i], address);
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
		uint32_t overlap = qb_find_pbj_address_overlap(cxt, dst[0], address);
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

static int32_t qb_check_value_reuse(qb_pbj_translator_context *cxt, uint32_t start_index, qb_pbj_address *address) {
	uint32_t i, effect, mask = qb_pbj_get_channel_mask(cxt, address);
	for(i = start_index; i < cxt->pbj_op_count; i++) {
		qb_pbj_op *pop = &cxt->pbj_ops[i];
		effect = qb_get_pbj_op_effect(cxt, pop, address, PBJ_OP_READ | PBJ_OP_WRITE);
		if(effect & PBJ_OP_READ) {
			// address is read
			return TRUE;
		} else if(effect & PBJ_OP_WRITE) {
			if((effect & PBJ_OP_OVERWRITE) == PBJ_OP_OVERWRITE) {
				// address is overwritten before it's read again
				break;
			} else {
				// address is only partial modified--clear the bits of the mask where the change occurs
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

static void qb_remove_redundant_pbj_ops(qb_pbj_translator_context *cxt) {
	uint32_t i;
	for(i = 0; i < cxt->pbj_op_count; i++) {
		qb_pbj_op *pop = &cxt->pbj_ops[i];
		uint32_t effect;
		if(pop->opcode == PBJ_COPY || pop->opcode == PBJ_LOAD_CONSTANT) {
			if(i + 1 < cxt->pbj_op_count) {
				qb_pbj_op *next_pop = &cxt->pbj_ops[i + 1];
				effect = qb_get_pbj_op_effect(cxt, next_pop, &pop->destination, PBJ_OP_READ | PBJ_OP_WRITE);
				// see if the destination is read by the next op
				if((effect & PBJ_OP_READ_EXACT) == PBJ_OP_READ_EXACT) {
					if(effect & PBJ_AS_DESTINATION) {
						// it's used as the destination (serving simultaneously as input value)
						// unless the destination is overwritten, check whether the value is used by ops beyond the next
						// if not, then we can safely remove the op
						if(((effect & PBJ_OP_OVERWRITE) == PBJ_OP_OVERWRITE) || !qb_check_value_reuse(cxt, i + 2, &pop->destination)) {
							next_pop->destination_origin = &pop->source;
							pop->opcode = PBJ_NOP;
						}
					}
				} 
			}
		}
	}
	for(i = 0; i < cxt->pbj_op_count; i++) {
		qb_pbj_op *pop = &cxt->pbj_ops[i];
		uint32_t effect;
		if(pop->opcode == PBJ_COPY && i > 0) {
			qb_pbj_op *prev_pop = &cxt->pbj_ops[i - 1];
			// see if the previous op write into the source of this one
			effect = qb_get_pbj_op_effect(cxt, prev_pop, &pop->source, PBJ_OP_WRITE);
			if((effect & PBJ_OP_OVERWRITE_EXACT) == PBJ_OP_OVERWRITE_EXACT) {
				if(!qb_check_value_reuse(cxt, i + 1, &pop->source)) {
					qb_pbj_address *input = (prev_pop->destination_origin) ? prev_pop->destination_origin : &prev_pop->destination;
					if(qb_match_pbj_addresses(cxt, input, &pop->destination)) {
						continue;
					}
					prev_pop->eventual_destination = &pop->destination;
					pop->opcode = PBJ_NOP;
				}
			}
		}
	}
}
