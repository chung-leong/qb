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
