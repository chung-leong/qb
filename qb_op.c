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
extern const char *global_operand_codes[];

uint32_t qb_get_op_flags(qb_opcode opcode) {
	const qb_op_info *op = &global_op_info[opcode];
	return op->flags;
}

uint32_t qb_get_operand_count(qb_opcode opcode) {
	const qb_op_info *op = &global_op_info[opcode];
	const char *codes = op->instruction_format;
	return (uint32_t) strlen(codes);
}

uint32_t qb_get_instruction_length(qb_opcode opcode) {
	const qb_op_info *op = &global_op_info[opcode];
	uint32_t length = op->instruction_length;
	return length;
}

qb_address_mode qb_get_operand_address_mode(uint32_t opcode, uint32_t operand_index) {
	const qb_op_info *op = &global_op_info[opcode];
	const char *codes = op->instruction_format;
	char code = codes[operand_index];
	switch(code) {
		case 'S':
		case 's':
			return QB_ADDRESS_MODE_SCA;
		case 'E':
		case 'e':
			return QB_ADDRESS_MODE_ELE;
		case 'A':
		case 'a':
			return QB_ADDRESS_MODE_ARR;
		case 'c':
			return QB_ADDRESS_MODE_SCA;
	}
	return -1;
}

int32_t qb_is_operand_write_target(qb_opcode opcode, uint32_t operand_index) {
	const qb_op_info *op = &global_op_info[opcode];
	const char *codes = op->instruction_format;
	char code = codes[operand_index];
	switch(code) {
		case 'S':
		case 'E':
		case 'A':
			return TRUE;
	}
	return FALSE;
}

const char *qb_get_op_format(uint32_t opcode) {
	const qb_op_info *op = &global_op_info[opcode];
	return op->instruction_format;
}

static qb_opcode switch_opcodes[8][8] = 
{
	{	QB_SWITCH8_I64_I64,		QB_SWITCH8_I64_I64,		QB_SWITCH8_I32_I32,		QB_SWITCH8_I32_I32,		QB_SWITCH8_I16_I16,		QB_SWITCH8_I16_I16,		QB_SWITCH8_I08_I08,		QB_SWITCH8_I08_I08		},
	{	QB_SWITCH16_I64_I64,	QB_SWITCH16_I64_I64,	QB_SWITCH16_I32_I32,	QB_SWITCH16_I32_I32,	QB_SWITCH16_I16_I16,	QB_SWITCH16_I16_I16,	QB_SWITCH16_I08_I08,	QB_SWITCH16_I08_I08		},
	{	QB_SWITCH32_I64_I64,	QB_SWITCH32_I64_I64,	QB_SWITCH32_I32_I32,	QB_SWITCH32_I32_I32,	QB_SWITCH32_I16_I16,	QB_SWITCH32_I16_I16,	QB_SWITCH32_I08_I08,	QB_SWITCH32_I08_I08		},
	{	QB_SWITCH64_I64_I64,	QB_SWITCH64_I64_I64,	QB_SWITCH64_I32_I32,	QB_SWITCH64_I32_I32,	QB_SWITCH64_I16_I16,	QB_SWITCH64_I16_I16,	QB_SWITCH64_I08_I08,	QB_SWITCH64_I08_I08		},
	{	QB_SWITCH128_I64_I64,	QB_SWITCH128_I64_I64,	QB_SWITCH128_I32_I32,	QB_SWITCH128_I32_I32,	QB_SWITCH128_I16_I16,	QB_SWITCH128_I16_I16,	QB_SWITCH128_I08_I08,	QB_SWITCH128_I08_I08	},
	{	QB_SWITCH256_I64_I64,	QB_SWITCH256_I64_I64,	QB_SWITCH256_I32_I32,	QB_SWITCH256_I32_I32,	QB_SWITCH256_I16_I16,	QB_SWITCH256_I16_I16,	QB_SWITCH256_I08_I08,	QB_SWITCH256_I08_I08	},
	{	QB_SWITCH512_I64_I64,	QB_SWITCH512_I64_I64,	QB_SWITCH512_I32_I32,	QB_SWITCH512_I32_I32,	QB_SWITCH512_I16_I16,	QB_SWITCH512_I16_I16,	QB_SWITCH512_I08_I08,	QB_SWITCH512_I08_I08	},
	{	QB_SWITCH1024_I64_I64,	QB_SWITCH1024_I64_I64,	QB_SWITCH1024_I32_I32,	QB_SWITCH1024_I32_I32,	QB_SWITCH1024_I16_I16,	QB_SWITCH1024_I16_I16,	QB_SWITCH1024_I08_I08,	QB_SWITCH1024_I08_I08	},
};

static uint32_t switch_table_sizes[8] = { 8, 16, 32, 64, 128, 256, 512, 1024 };

static uint32_t qb_get_switch_table_index(uint32_t case_count) {
	uint32_t i;
	for(i = 0; i < sizeof(switch_table_sizes) / sizeof(switch_table_sizes[0]); i++) {
		uint32_t size = switch_table_sizes[i];
		if(case_count < size) {
			return i;
		}
	}
	return (uint32_t) -1;
}

uint32_t qb_get_switch_table_size(uint32_t case_count) {
	uint32_t index = qb_get_switch_table_index(case_count);
	return switch_table_sizes[index] + 1;
}

qb_opcode qb_get_switch_opcode(uint32_t case_count, qb_primitive_type type) {
	uint32_t index = qb_get_switch_table_index(case_count);
	qb_opcode *opcodes = switch_opcodes[index];
	return opcodes[type];
}

uint32_t qb_get_switch_table_size_from_opcode(qb_opcode opcode) {
	uint32_t i, j;
	for(i = 0; sizeof(switch_table_sizes) / sizeof(switch_table_sizes[0]); i++) {
		qb_opcode *opcodes = switch_opcodes[i];
		for(j = 0; j < 8; j++) {
			if(opcodes[j] == opcode) {
				return switch_table_sizes[i] + 1;
			}
		}
	}
	return 0;
}
