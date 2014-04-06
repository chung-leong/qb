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


#ifndef QB_INTERPRETER_STRUCTURES_H_
#define QB_INTERPRETER_STRUCTURES_H_

#if defined(USE_TAIL_CALL_INTERPRETER_LOOP) || defined(USE_COMPUTED_GOTO_INTERPRETER_LOOP)
typedef void * qb_op_handler;
#else
typedef int32_t qb_op_handler;
#endif

#if defined(__sun) || defined(__sun__)
#pragma pack(1)
#else
#pragma pack(push, 1)
#endif

typedef struct qb_branch_table_entry {
	qb_op_handler next_handler;
	int8_t *instruction_pointer;
} qb_branch_table_entry;

typedef struct qb_instruction {
	qb_op_handler next_handler;
} qb_instruction;

typedef struct qb_instruction_ARR {
	qb_op_handler next_handler;
	qb_pointer_ARR operand1;
} qb_instruction_ARR;

typedef struct qb_instruction_ARR_ARR {
	qb_op_handler next_handler;
	qb_pointer_ARR operand1;
	qb_pointer_ARR operand2;
} qb_instruction_ARR_ARR;

typedef struct qb_instruction_ARR_ARR_ARR {
	qb_op_handler next_handler;
	qb_pointer_ARR operand1;
	qb_pointer_ARR operand2;
	qb_pointer_ARR operand3;
} qb_instruction_ARR_ARR_ARR;

typedef struct qb_instruction_ARR_ARR_ARR_ARR {
	qb_op_handler next_handler;
	qb_pointer_ARR operand1;
	qb_pointer_ARR operand2;
	qb_pointer_ARR operand3;
	qb_pointer_ARR operand4;
} qb_instruction_ARR_ARR_ARR_ARR;

typedef struct qb_instruction_ARR_ARR_ARR_line_id {
	qb_op_handler next_handler;
	qb_pointer_ARR operand1;
	qb_pointer_ARR operand2;
	qb_pointer_ARR operand3;
	uint32_t line_id;
} qb_instruction_ARR_ARR_ARR_line_id;

typedef struct qb_instruction_ARR_ARR_ELE {
	qb_op_handler next_handler;
	qb_pointer_ARR operand1;
	qb_pointer_ARR operand2;
	qb_pointer_ELE operand3;
} qb_instruction_ARR_ARR_ELE;

typedef struct qb_instruction_ARR_ARR_SCA {
	qb_op_handler next_handler;
	qb_pointer_ARR operand1;
	qb_pointer_ARR operand2;
	qb_pointer_SCA operand3;
} qb_instruction_ARR_ARR_SCA;

typedef struct qb_instruction_ARR_ARR_SCA_ARR {
	qb_op_handler next_handler;
	qb_pointer_ARR operand1;
	qb_pointer_ARR operand2;
	qb_pointer_SCA operand3;
	qb_pointer_ARR operand4;
} qb_instruction_ARR_ARR_SCA_ARR;

typedef struct qb_instruction_ARR_ARR_SCA_ELE {
	qb_op_handler next_handler;
	qb_pointer_ARR operand1;
	qb_pointer_ARR operand2;
	qb_pointer_SCA operand3;
	qb_pointer_ELE operand4;
} qb_instruction_ARR_ARR_SCA_ELE;

typedef struct qb_instruction_ARR_ARR_SCA_SCA {
	qb_op_handler next_handler;
	qb_pointer_ARR operand1;
	qb_pointer_ARR operand2;
	qb_pointer_SCA operand3;
	qb_pointer_SCA operand4;
} qb_instruction_ARR_ARR_SCA_SCA;

typedef struct qb_instruction_ARR_ARR_SCA_SCA_ARR {
	qb_op_handler next_handler;
	qb_pointer_ARR operand1;
	qb_pointer_ARR operand2;
	qb_pointer_SCA operand3;
	qb_pointer_SCA operand4;
	qb_pointer_ARR operand5;
} qb_instruction_ARR_ARR_SCA_SCA_ARR;

typedef struct qb_instruction_ARR_ARR_SCA_SCA_SCA_ARR {
	qb_op_handler next_handler;
	qb_pointer_ARR operand1;
	qb_pointer_ARR operand2;
	qb_pointer_SCA operand3;
	qb_pointer_SCA operand4;
	qb_pointer_SCA operand5;
	qb_pointer_ARR operand6;
} qb_instruction_ARR_ARR_SCA_SCA_SCA_ARR;

typedef struct qb_instruction_ARR_ARR_U32_ARR {
	qb_op_handler next_handler;
	qb_pointer_ARR operand1;
	qb_pointer_ARR operand2;
	uint32_t operand3;
	qb_pointer_ARR operand4;
} qb_instruction_ARR_ARR_U32_ARR;

typedef struct qb_instruction_ARR_ELE {
	qb_op_handler next_handler;
	qb_pointer_ARR operand1;
	qb_pointer_ELE operand2;
} qb_instruction_ARR_ELE;

typedef struct qb_instruction_ARR_ELE_SCA {
	qb_op_handler next_handler;
	qb_pointer_ARR operand1;
	qb_pointer_ELE operand2;
	qb_pointer_SCA operand3;
} qb_instruction_ARR_ELE_SCA;

typedef struct qb_instruction_ARR_SCA {
	qb_op_handler next_handler;
	qb_pointer_ARR operand1;
	qb_pointer_SCA operand2;
} qb_instruction_ARR_SCA;

typedef struct qb_instruction_ARR_SCA_ARR {
	qb_op_handler next_handler;
	qb_pointer_ARR operand1;
	qb_pointer_SCA operand2;
	qb_pointer_ARR operand3;
} qb_instruction_ARR_SCA_ARR;

typedef struct qb_instruction_ARR_SCA_ARR_ARR {
	qb_op_handler next_handler;
	qb_pointer_ARR operand1;
	qb_pointer_SCA operand2;
	qb_pointer_ARR operand3;
	qb_pointer_ARR operand4;
} qb_instruction_ARR_SCA_ARR_ARR;

typedef struct qb_instruction_ARR_SCA_ELE {
	qb_op_handler next_handler;
	qb_pointer_ARR operand1;
	qb_pointer_SCA operand2;
	qb_pointer_ELE operand3;
} qb_instruction_ARR_SCA_ELE;

typedef struct qb_instruction_ARR_SCA_SCA {
	qb_op_handler next_handler;
	qb_pointer_ARR operand1;
	qb_pointer_SCA operand2;
	qb_pointer_SCA operand3;
} qb_instruction_ARR_SCA_SCA;

typedef struct qb_instruction_ARR_SCA_SCA_ARR {
	qb_op_handler next_handler;
	qb_pointer_ARR operand1;
	qb_pointer_SCA operand2;
	qb_pointer_SCA operand3;
	qb_pointer_ARR operand4;
} qb_instruction_ARR_SCA_SCA_ARR;

typedef struct qb_instruction_ARR_SCA_SCA_ARR_ARR_ARR {
	qb_op_handler next_handler;
	qb_pointer_ARR operand1;
	qb_pointer_SCA operand2;
	qb_pointer_SCA operand3;
	qb_pointer_ARR operand4;
	qb_pointer_ARR operand5;
	qb_pointer_ARR operand6;
} qb_instruction_ARR_SCA_SCA_ARR_ARR_ARR;

typedef struct qb_instruction_ARR_SCA_SCA_ELE_ELE_ARR {
	qb_op_handler next_handler;
	qb_pointer_ARR operand1;
	qb_pointer_SCA operand2;
	qb_pointer_SCA operand3;
	qb_pointer_ELE operand4;
	qb_pointer_ELE operand5;
	qb_pointer_ARR operand6;
} qb_instruction_ARR_SCA_SCA_ELE_ELE_ARR;

typedef struct qb_instruction_ARR_SCA_SCA_SCA_ARR_line_id {
	qb_op_handler next_handler;
	qb_pointer_ARR operand1;
	qb_pointer_SCA operand2;
	qb_pointer_SCA operand3;
	qb_pointer_SCA operand4;
	qb_pointer_ARR operand5;
	uint32_t line_id;
} qb_instruction_ARR_SCA_SCA_SCA_ARR_line_id;

typedef struct qb_instruction_ARR_SCA_SCA_SCA_SCA_ARR {
	qb_op_handler next_handler;
	qb_pointer_ARR operand1;
	qb_pointer_SCA operand2;
	qb_pointer_SCA operand3;
	qb_pointer_SCA operand4;
	qb_pointer_SCA operand5;
	qb_pointer_ARR operand6;
} qb_instruction_ARR_SCA_SCA_SCA_SCA_ARR;

typedef struct qb_instruction_ARR_SCA_SCA_SCA_SCA_U32_ARR {
	qb_op_handler next_handler;
	qb_pointer_ARR operand1;
	qb_pointer_SCA operand2;
	qb_pointer_SCA operand3;
	qb_pointer_SCA operand4;
	qb_pointer_SCA operand5;
	uint32_t operand6;
	qb_pointer_ARR operand7;
} qb_instruction_ARR_SCA_SCA_SCA_SCA_U32_ARR;

typedef struct qb_instruction_ARR_U32_ARR {
	qb_op_handler next_handler;
	qb_pointer_ARR operand1;
	uint32_t operand2;
	qb_pointer_ARR operand3;
} qb_instruction_ARR_U32_ARR;

typedef struct qb_instruction_ELE {
	qb_op_handler next_handler;
	qb_pointer_ELE operand1;
} qb_instruction_ELE;

typedef struct qb_instruction_ELE_ARR {
	qb_op_handler next_handler;
	qb_pointer_ELE operand1;
	qb_pointer_ARR operand2;
} qb_instruction_ELE_ARR;

typedef struct qb_instruction_ELE_ELE {
	qb_op_handler next_handler;
	qb_pointer_ELE operand1;
	qb_pointer_ELE operand2;
} qb_instruction_ELE_ELE;

typedef struct qb_instruction_ELE_ELE_ELE {
	qb_op_handler next_handler;
	qb_pointer_ELE operand1;
	qb_pointer_ELE operand2;
	qb_pointer_ELE operand3;
} qb_instruction_ELE_ELE_ELE;

typedef struct qb_instruction_ELE_ELE_ELE_ARR {
	qb_op_handler next_handler;
	qb_pointer_ELE operand1;
	qb_pointer_ELE operand2;
	qb_pointer_ELE operand3;
	qb_pointer_ARR operand4;
} qb_instruction_ELE_ELE_ELE_ARR;

typedef struct qb_instruction_ELE_ELE_ELE_ELE {
	qb_op_handler next_handler;
	qb_pointer_ELE operand1;
	qb_pointer_ELE operand2;
	qb_pointer_ELE operand3;
	qb_pointer_ELE operand4;
} qb_instruction_ELE_ELE_ELE_ELE;

typedef struct qb_instruction_ELE_ELE_ELE_SCA {
	qb_op_handler next_handler;
	qb_pointer_ELE operand1;
	qb_pointer_ELE operand2;
	qb_pointer_ELE operand3;
	qb_pointer_SCA operand4;
} qb_instruction_ELE_ELE_ELE_SCA;

typedef struct qb_instruction_ELE_ELE_ELE_line_id {
	qb_op_handler next_handler;
	qb_pointer_ELE operand1;
	qb_pointer_ELE operand2;
	qb_pointer_ELE operand3;
	uint32_t line_id;
} qb_instruction_ELE_ELE_ELE_line_id;

typedef struct qb_instruction_ELE_ELE_SCA {
	qb_op_handler next_handler;
	qb_pointer_ELE operand1;
	qb_pointer_ELE operand2;
	qb_pointer_SCA operand3;
} qb_instruction_ELE_ELE_SCA;

typedef struct qb_instruction_ELE_ELE_SCA_SCA {
	qb_op_handler next_handler;
	qb_pointer_ELE operand1;
	qb_pointer_ELE operand2;
	qb_pointer_SCA operand3;
	qb_pointer_SCA operand4;
} qb_instruction_ELE_ELE_SCA_SCA;

typedef struct qb_instruction_ELE_ELE_SCA_SCA_ARR_ARR {
	qb_op_handler next_handler;
	qb_pointer_ELE operand1;
	qb_pointer_ELE operand2;
	qb_pointer_SCA operand3;
	qb_pointer_SCA operand4;
	qb_pointer_ARR operand5;
	qb_pointer_ARR operand6;
} qb_instruction_ELE_ELE_SCA_SCA_ARR_ARR;

typedef struct qb_instruction_ELE_SCA_SCA {
	qb_op_handler next_handler;
	qb_pointer_ELE operand1;
	qb_pointer_SCA operand2;
	qb_pointer_SCA operand3;
} qb_instruction_ELE_SCA_SCA;

typedef struct qb_instruction_ELE_U32_ARR {
	qb_op_handler next_handler;
	qb_pointer_ELE operand1;
	uint32_t operand2;
	qb_pointer_ARR operand3;
} qb_instruction_ELE_U32_ARR;

typedef struct qb_instruction_SCA {
	qb_op_handler next_handler;
	qb_pointer_SCA operand1;
} qb_instruction_SCA;

typedef struct qb_instruction_SCA_ARR {
	qb_op_handler next_handler;
	qb_pointer_SCA operand1;
	qb_pointer_ARR operand2;
} qb_instruction_SCA_ARR;

typedef struct qb_instruction_SCA_ARR_ARR {
	qb_op_handler next_handler;
	qb_pointer_SCA operand1;
	qb_pointer_ARR operand2;
	qb_pointer_ARR operand3;
} qb_instruction_SCA_ARR_ARR;

typedef struct qb_instruction_SCA_ARR_SCA_line_id {
	qb_op_handler next_handler;
	qb_pointer_SCA operand1;
	qb_pointer_ARR operand2;
	qb_pointer_SCA operand3;
	uint32_t line_id;
} qb_instruction_SCA_ARR_SCA_line_id;

typedef struct qb_instruction_SCA_ELE_ARR {
	qb_op_handler next_handler;
	qb_pointer_SCA operand1;
	qb_pointer_ELE operand2;
	qb_pointer_ARR operand3;
} qb_instruction_SCA_ELE_ARR;

typedef struct qb_instruction_SCA_SCA {
	qb_op_handler next_handler;
	qb_pointer_SCA operand1;
	qb_pointer_SCA operand2;
} qb_instruction_SCA_SCA;

typedef struct qb_instruction_SCA_SCA_ARR {
	qb_op_handler next_handler;
	qb_pointer_SCA operand1;
	qb_pointer_SCA operand2;
	qb_pointer_ARR operand3;
} qb_instruction_SCA_SCA_ARR;

typedef struct qb_instruction_SCA_SCA_SCA {
	qb_op_handler next_handler;
	qb_pointer_SCA operand1;
	qb_pointer_SCA operand2;
	qb_pointer_SCA operand3;
} qb_instruction_SCA_SCA_SCA;

typedef struct qb_instruction_SCA_SCA_SCA_ARR {
	qb_op_handler next_handler;
	qb_pointer_SCA operand1;
	qb_pointer_SCA operand2;
	qb_pointer_SCA operand3;
	qb_pointer_ARR operand4;
} qb_instruction_SCA_SCA_SCA_ARR;

typedef struct qb_instruction_SCA_SCA_SCA_SCA {
	qb_op_handler next_handler;
	qb_pointer_SCA operand1;
	qb_pointer_SCA operand2;
	qb_pointer_SCA operand3;
	qb_pointer_SCA operand4;
} qb_instruction_SCA_SCA_SCA_SCA;

typedef struct qb_instruction_SCA_SCA_SCA_SCA_ARR_ARR {
	qb_op_handler next_handler;
	qb_pointer_SCA operand1;
	qb_pointer_SCA operand2;
	qb_pointer_SCA operand3;
	qb_pointer_SCA operand4;
	qb_pointer_ARR operand5;
	qb_pointer_ARR operand6;
} qb_instruction_SCA_SCA_SCA_SCA_ARR_ARR;

typedef struct qb_instruction_SCA_SCA_SCA_SCA_SCA {
	qb_op_handler next_handler;
	qb_pointer_SCA operand1;
	qb_pointer_SCA operand2;
	qb_pointer_SCA operand3;
	qb_pointer_SCA operand4;
	qb_pointer_SCA operand5;
} qb_instruction_SCA_SCA_SCA_SCA_SCA;

typedef struct qb_instruction_SCA_SCA_SCA_SCA_SCA_SCA_SCA {
	qb_op_handler next_handler;
	qb_pointer_SCA operand1;
	qb_pointer_SCA operand2;
	qb_pointer_SCA operand3;
	qb_pointer_SCA operand4;
	qb_pointer_SCA operand5;
	qb_pointer_SCA operand6;
	qb_pointer_SCA operand7;
} qb_instruction_SCA_SCA_SCA_SCA_SCA_SCA_SCA;

typedef struct qb_instruction_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_U32_U32 {
	qb_op_handler next_handler;
	qb_pointer_SCA operand1;
	qb_pointer_SCA operand2;
	qb_pointer_SCA operand3;
	qb_pointer_SCA operand4;
	qb_pointer_SCA operand5;
	qb_pointer_SCA operand6;
	qb_pointer_SCA operand7;
	qb_pointer_SCA operand8;
	qb_pointer_SCA operand9;
	qb_pointer_SCA operand10;
	qb_pointer_SCA operand11;
	qb_pointer_SCA operand12;
	qb_pointer_SCA operand13;
	qb_pointer_SCA operand14;
	qb_pointer_SCA operand15;
	qb_pointer_SCA operand16;
	qb_pointer_SCA operand17;
	qb_pointer_SCA operand18;
	qb_pointer_SCA operand19;
	qb_pointer_SCA operand20;
	qb_pointer_SCA operand21;
	qb_pointer_SCA operand22;
	qb_pointer_SCA operand23;
	qb_pointer_SCA operand24;
	qb_pointer_SCA operand25;
	qb_pointer_SCA operand26;
	qb_pointer_SCA operand27;
	qb_pointer_SCA operand28;
	qb_pointer_SCA operand29;
	qb_pointer_SCA operand30;
	qb_pointer_SCA operand31;
	qb_pointer_SCA operand32;
	uint32_t operand33;
	uint32_t operand34;
} qb_instruction_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_U32_U32;

typedef struct qb_instruction_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_U32_U32 {
	qb_op_handler next_handler;
	qb_pointer_SCA operand1;
	qb_pointer_SCA operand2;
	qb_pointer_SCA operand3;
	qb_pointer_SCA operand4;
	qb_pointer_SCA operand5;
	qb_pointer_SCA operand6;
	qb_pointer_SCA operand7;
	qb_pointer_SCA operand8;
	qb_pointer_SCA operand9;
	qb_pointer_SCA operand10;
	qb_pointer_SCA operand11;
	qb_pointer_SCA operand12;
	qb_pointer_SCA operand13;
	qb_pointer_SCA operand14;
	qb_pointer_SCA operand15;
	qb_pointer_SCA operand16;
	qb_pointer_SCA operand17;
	qb_pointer_SCA operand18;
	qb_pointer_SCA operand19;
	qb_pointer_SCA operand20;
	qb_pointer_SCA operand21;
	qb_pointer_SCA operand22;
	qb_pointer_SCA operand23;
	qb_pointer_SCA operand24;
	qb_pointer_SCA operand25;
	qb_pointer_SCA operand26;
	qb_pointer_SCA operand27;
	qb_pointer_SCA operand28;
	uint32_t operand29;
	uint32_t operand30;
} qb_instruction_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_U32_U32;

typedef struct qb_instruction_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_U32_ARR {
	qb_op_handler next_handler;
	qb_pointer_SCA operand1;
	qb_pointer_SCA operand2;
	qb_pointer_SCA operand3;
	qb_pointer_SCA operand4;
	qb_pointer_SCA operand5;
	qb_pointer_SCA operand6;
	qb_pointer_SCA operand7;
	qb_pointer_SCA operand8;
	qb_pointer_SCA operand9;
	qb_pointer_SCA operand10;
	qb_pointer_SCA operand11;
	qb_pointer_SCA operand12;
	qb_pointer_SCA operand13;
	qb_pointer_SCA operand14;
	qb_pointer_SCA operand15;
	qb_pointer_SCA operand16;
	qb_pointer_SCA operand17;
	qb_pointer_SCA operand18;
	qb_pointer_SCA operand19;
	qb_pointer_SCA operand20;
	qb_pointer_SCA operand21;
	qb_pointer_SCA operand22;
	qb_pointer_SCA operand23;
	qb_pointer_SCA operand24;
	uint32_t operand25;
	qb_pointer_ARR operand26;
} qb_instruction_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_U32_ARR;

typedef struct qb_instruction_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_U32_U32 {
	qb_op_handler next_handler;
	qb_pointer_SCA operand1;
	qb_pointer_SCA operand2;
	qb_pointer_SCA operand3;
	qb_pointer_SCA operand4;
	qb_pointer_SCA operand5;
	qb_pointer_SCA operand6;
	qb_pointer_SCA operand7;
	qb_pointer_SCA operand8;
	qb_pointer_SCA operand9;
	qb_pointer_SCA operand10;
	qb_pointer_SCA operand11;
	qb_pointer_SCA operand12;
	qb_pointer_SCA operand13;
	qb_pointer_SCA operand14;
	qb_pointer_SCA operand15;
	qb_pointer_SCA operand16;
	qb_pointer_SCA operand17;
	qb_pointer_SCA operand18;
	qb_pointer_SCA operand19;
	qb_pointer_SCA operand20;
	qb_pointer_SCA operand21;
	qb_pointer_SCA operand22;
	qb_pointer_SCA operand23;
	qb_pointer_SCA operand24;
	uint32_t operand25;
	uint32_t operand26;
} qb_instruction_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_U32_U32;

typedef struct qb_instruction_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_U32_ARR {
	qb_op_handler next_handler;
	qb_pointer_SCA operand1;
	qb_pointer_SCA operand2;
	qb_pointer_SCA operand3;
	qb_pointer_SCA operand4;
	qb_pointer_SCA operand5;
	qb_pointer_SCA operand6;
	qb_pointer_SCA operand7;
	qb_pointer_SCA operand8;
	qb_pointer_SCA operand9;
	qb_pointer_SCA operand10;
	qb_pointer_SCA operand11;
	qb_pointer_SCA operand12;
	qb_pointer_SCA operand13;
	qb_pointer_SCA operand14;
	qb_pointer_SCA operand15;
	qb_pointer_SCA operand16;
	qb_pointer_SCA operand17;
	qb_pointer_SCA operand18;
	qb_pointer_SCA operand19;
	qb_pointer_SCA operand20;
	qb_pointer_SCA operand21;
	uint32_t operand22;
	qb_pointer_ARR operand23;
} qb_instruction_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_U32_ARR;

typedef struct qb_instruction_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_U32_U32 {
	qb_op_handler next_handler;
	qb_pointer_SCA operand1;
	qb_pointer_SCA operand2;
	qb_pointer_SCA operand3;
	qb_pointer_SCA operand4;
	qb_pointer_SCA operand5;
	qb_pointer_SCA operand6;
	qb_pointer_SCA operand7;
	qb_pointer_SCA operand8;
	qb_pointer_SCA operand9;
	qb_pointer_SCA operand10;
	qb_pointer_SCA operand11;
	qb_pointer_SCA operand12;
	qb_pointer_SCA operand13;
	qb_pointer_SCA operand14;
	qb_pointer_SCA operand15;
	qb_pointer_SCA operand16;
	qb_pointer_SCA operand17;
	qb_pointer_SCA operand18;
	qb_pointer_SCA operand19;
	qb_pointer_SCA operand20;
	uint32_t operand21;
	uint32_t operand22;
} qb_instruction_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_U32_U32;

typedef struct qb_instruction_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_U32_ARR {
	qb_op_handler next_handler;
	qb_pointer_SCA operand1;
	qb_pointer_SCA operand2;
	qb_pointer_SCA operand3;
	qb_pointer_SCA operand4;
	qb_pointer_SCA operand5;
	qb_pointer_SCA operand6;
	qb_pointer_SCA operand7;
	qb_pointer_SCA operand8;
	qb_pointer_SCA operand9;
	qb_pointer_SCA operand10;
	qb_pointer_SCA operand11;
	qb_pointer_SCA operand12;
	qb_pointer_SCA operand13;
	qb_pointer_SCA operand14;
	qb_pointer_SCA operand15;
	qb_pointer_SCA operand16;
	qb_pointer_SCA operand17;
	qb_pointer_SCA operand18;
	uint32_t operand19;
	qb_pointer_ARR operand20;
} qb_instruction_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_U32_ARR;

typedef struct qb_instruction_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_U32_U32 {
	qb_op_handler next_handler;
	qb_pointer_SCA operand1;
	qb_pointer_SCA operand2;
	qb_pointer_SCA operand3;
	qb_pointer_SCA operand4;
	qb_pointer_SCA operand5;
	qb_pointer_SCA operand6;
	qb_pointer_SCA operand7;
	qb_pointer_SCA operand8;
	qb_pointer_SCA operand9;
	qb_pointer_SCA operand10;
	qb_pointer_SCA operand11;
	qb_pointer_SCA operand12;
	qb_pointer_SCA operand13;
	qb_pointer_SCA operand14;
	qb_pointer_SCA operand15;
	qb_pointer_SCA operand16;
	uint32_t operand17;
	uint32_t operand18;
} qb_instruction_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_U32_U32;

typedef struct qb_instruction_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_U32_ARR {
	qb_op_handler next_handler;
	qb_pointer_SCA operand1;
	qb_pointer_SCA operand2;
	qb_pointer_SCA operand3;
	qb_pointer_SCA operand4;
	qb_pointer_SCA operand5;
	qb_pointer_SCA operand6;
	qb_pointer_SCA operand7;
	qb_pointer_SCA operand8;
	qb_pointer_SCA operand9;
	qb_pointer_SCA operand10;
	qb_pointer_SCA operand11;
	qb_pointer_SCA operand12;
	qb_pointer_SCA operand13;
	qb_pointer_SCA operand14;
	qb_pointer_SCA operand15;
	uint32_t operand16;
	qb_pointer_ARR operand17;
} qb_instruction_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_U32_ARR;

typedef struct qb_instruction_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_U32_ARR {
	qb_op_handler next_handler;
	qb_pointer_SCA operand1;
	qb_pointer_SCA operand2;
	qb_pointer_SCA operand3;
	qb_pointer_SCA operand4;
	qb_pointer_SCA operand5;
	qb_pointer_SCA operand6;
	qb_pointer_SCA operand7;
	qb_pointer_SCA operand8;
	qb_pointer_SCA operand9;
	qb_pointer_SCA operand10;
	qb_pointer_SCA operand11;
	qb_pointer_SCA operand12;
	uint32_t operand13;
	qb_pointer_ARR operand14;
} qb_instruction_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_U32_ARR;

typedef struct qb_instruction_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_U32_U32 {
	qb_op_handler next_handler;
	qb_pointer_SCA operand1;
	qb_pointer_SCA operand2;
	qb_pointer_SCA operand3;
	qb_pointer_SCA operand4;
	qb_pointer_SCA operand5;
	qb_pointer_SCA operand6;
	qb_pointer_SCA operand7;
	qb_pointer_SCA operand8;
	qb_pointer_SCA operand9;
	qb_pointer_SCA operand10;
	qb_pointer_SCA operand11;
	qb_pointer_SCA operand12;
	uint32_t operand13;
	uint32_t operand14;
} qb_instruction_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_U32_U32;

typedef struct qb_instruction_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_U32_ARR {
	qb_op_handler next_handler;
	qb_pointer_SCA operand1;
	qb_pointer_SCA operand2;
	qb_pointer_SCA operand3;
	qb_pointer_SCA operand4;
	qb_pointer_SCA operand5;
	qb_pointer_SCA operand6;
	qb_pointer_SCA operand7;
	qb_pointer_SCA operand8;
	qb_pointer_SCA operand9;
	uint32_t operand10;
	qb_pointer_ARR operand11;
} qb_instruction_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_U32_ARR;

typedef struct qb_instruction_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_U32_U32 {
	qb_op_handler next_handler;
	qb_pointer_SCA operand1;
	qb_pointer_SCA operand2;
	qb_pointer_SCA operand3;
	qb_pointer_SCA operand4;
	qb_pointer_SCA operand5;
	qb_pointer_SCA operand6;
	qb_pointer_SCA operand7;
	qb_pointer_SCA operand8;
	uint32_t operand9;
	uint32_t operand10;
} qb_instruction_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_U32_U32;

typedef struct qb_instruction_SCA_SCA_SCA_SCA_SCA_SCA_U32_ARR {
	qb_op_handler next_handler;
	qb_pointer_SCA operand1;
	qb_pointer_SCA operand2;
	qb_pointer_SCA operand3;
	qb_pointer_SCA operand4;
	qb_pointer_SCA operand5;
	qb_pointer_SCA operand6;
	uint32_t operand7;
	qb_pointer_ARR operand8;
} qb_instruction_SCA_SCA_SCA_SCA_SCA_SCA_U32_ARR;

typedef struct qb_instruction_SCA_SCA_SCA_SCA_SCA_SCA_line_id {
	qb_op_handler next_handler;
	qb_pointer_SCA operand1;
	qb_pointer_SCA operand2;
	qb_pointer_SCA operand3;
	qb_pointer_SCA operand4;
	qb_pointer_SCA operand5;
	qb_pointer_SCA operand6;
	uint32_t line_id;
} qb_instruction_SCA_SCA_SCA_SCA_SCA_SCA_line_id;

typedef struct qb_instruction_SCA_SCA_SCA_SCA_SCA_line_id {
	qb_op_handler next_handler;
	qb_pointer_SCA operand1;
	qb_pointer_SCA operand2;
	qb_pointer_SCA operand3;
	qb_pointer_SCA operand4;
	qb_pointer_SCA operand5;
	uint32_t line_id;
} qb_instruction_SCA_SCA_SCA_SCA_SCA_line_id;

typedef struct qb_instruction_SCA_SCA_SCA_SCA_U32_U32 {
	qb_op_handler next_handler;
	qb_pointer_SCA operand1;
	qb_pointer_SCA operand2;
	qb_pointer_SCA operand3;
	qb_pointer_SCA operand4;
	uint32_t operand5;
	uint32_t operand6;
} qb_instruction_SCA_SCA_SCA_SCA_U32_U32;

typedef struct qb_instruction_SCA_SCA_SCA_SCA_U32_U32_SCA {
	qb_op_handler next_handler;
	qb_pointer_SCA operand1;
	qb_pointer_SCA operand2;
	qb_pointer_SCA operand3;
	qb_pointer_SCA operand4;
	uint32_t operand5;
	uint32_t operand6;
	qb_pointer_SCA operand7;
} qb_instruction_SCA_SCA_SCA_SCA_U32_U32_SCA;

typedef struct qb_instruction_SCA_SCA_SCA_SCA_line_id {
	qb_op_handler next_handler;
	qb_pointer_SCA operand1;
	qb_pointer_SCA operand2;
	qb_pointer_SCA operand3;
	qb_pointer_SCA operand4;
	uint32_t line_id;
} qb_instruction_SCA_SCA_SCA_SCA_line_id;

typedef struct qb_instruction_SCA_SCA_SCA_U32_ARR {
	qb_op_handler next_handler;
	qb_pointer_SCA operand1;
	qb_pointer_SCA operand2;
	qb_pointer_SCA operand3;
	uint32_t operand4;
	qb_pointer_ARR operand5;
} qb_instruction_SCA_SCA_SCA_U32_ARR;

typedef struct qb_instruction_SCA_SCA_SCA_line_id {
	qb_op_handler next_handler;
	qb_pointer_SCA operand1;
	qb_pointer_SCA operand2;
	qb_pointer_SCA operand3;
	uint32_t line_id;
} qb_instruction_SCA_SCA_SCA_line_id;

typedef struct qb_instruction_SCA_SCA_U32_ARR {
	qb_op_handler next_handler;
	qb_pointer_SCA operand1;
	qb_pointer_SCA operand2;
	uint32_t operand3;
	qb_pointer_ARR operand4;
} qb_instruction_SCA_SCA_U32_ARR;

typedef struct qb_instruction_SCA_SCA_U32_U32 {
	qb_op_handler next_handler;
	qb_pointer_SCA operand1;
	qb_pointer_SCA operand2;
	uint32_t operand3;
	uint32_t operand4;
} qb_instruction_SCA_SCA_U32_U32;

typedef struct qb_instruction_SCA_SCA_line_id {
	qb_op_handler next_handler;
	qb_pointer_SCA operand1;
	qb_pointer_SCA operand2;
	uint32_t line_id;
} qb_instruction_SCA_SCA_line_id;

typedef struct qb_instruction_SCA_U32_ARR {
	qb_op_handler next_handler;
	qb_pointer_SCA operand1;
	uint32_t operand2;
	qb_pointer_ARR operand3;
} qb_instruction_SCA_U32_ARR;

typedef struct qb_instruction_SCA_U32_U32_SCA {
	qb_op_handler next_handler;
	qb_pointer_SCA operand1;
	uint32_t operand2;
	uint32_t operand3;
	qb_pointer_SCA operand4;
} qb_instruction_SCA_U32_U32_SCA;

typedef struct qb_instruction_SCA_line_id {
	qb_op_handler next_handler;
	qb_pointer_SCA operand1;
	uint32_t line_id;
} qb_instruction_SCA_line_id;

typedef struct qb_instruction_U32_ARR {
	qb_op_handler next_handler;
	uint32_t operand1;
	qb_pointer_ARR operand2;
} qb_instruction_U32_ARR;

typedef struct qb_instruction_U32_ARR_ARR {
	qb_op_handler next_handler;
	uint32_t operand1;
	qb_pointer_ARR operand2;
	qb_pointer_ARR operand3;
} qb_instruction_U32_ARR_ARR;

typedef struct qb_instruction_branch_ELE {
	qb_op_handler next_handler1;
	int8_t *instruction_pointer1;
	qb_op_handler next_handler2;
	int8_t *instruction_pointer2;
	qb_pointer_ELE operand1;
} qb_instruction_branch_ELE;

typedef struct qb_instruction_branch_ELE_ELE {
	qb_op_handler next_handler1;
	int8_t *instruction_pointer1;
	qb_op_handler next_handler2;
	int8_t *instruction_pointer2;
	qb_pointer_ELE operand1;
	qb_pointer_ELE operand2;
} qb_instruction_branch_ELE_ELE;

typedef struct qb_instruction_branch_SCA {
	qb_op_handler next_handler1;
	int8_t *instruction_pointer1;
	qb_op_handler next_handler2;
	int8_t *instruction_pointer2;
	qb_pointer_SCA operand1;
} qb_instruction_branch_SCA;

typedef struct qb_instruction_branch_SCA_SCA {
	qb_op_handler next_handler1;
	int8_t *instruction_pointer1;
	qb_op_handler next_handler2;
	int8_t *instruction_pointer2;
	qb_pointer_SCA operand1;
	qb_pointer_SCA operand2;
} qb_instruction_branch_SCA_SCA;

typedef struct qb_instruction_branch_table_1025_ELE_ELE {
	qb_pointer_ELE operand1;
	qb_pointer_ELE operand2;
	qb_branch_table_entry branch_table[1025];
} qb_instruction_branch_table_1025_ELE_ELE;

typedef struct qb_instruction_branch_table_1025_SCA_SCA {
	qb_pointer_SCA operand1;
	qb_pointer_SCA operand2;
	qb_branch_table_entry branch_table[1025];
} qb_instruction_branch_table_1025_SCA_SCA;

typedef struct qb_instruction_branch_table_129_ELE_ELE {
	qb_pointer_ELE operand1;
	qb_pointer_ELE operand2;
	qb_branch_table_entry branch_table[129];
} qb_instruction_branch_table_129_ELE_ELE;

typedef struct qb_instruction_branch_table_129_SCA_SCA {
	qb_pointer_SCA operand1;
	qb_pointer_SCA operand2;
	qb_branch_table_entry branch_table[129];
} qb_instruction_branch_table_129_SCA_SCA;

typedef struct qb_instruction_branch_table_17_ELE_ELE {
	qb_pointer_ELE operand1;
	qb_pointer_ELE operand2;
	qb_branch_table_entry branch_table[17];
} qb_instruction_branch_table_17_ELE_ELE;

typedef struct qb_instruction_branch_table_17_SCA_SCA {
	qb_pointer_SCA operand1;
	qb_pointer_SCA operand2;
	qb_branch_table_entry branch_table[17];
} qb_instruction_branch_table_17_SCA_SCA;

typedef struct qb_instruction_branch_table_257_ELE_ELE {
	qb_pointer_ELE operand1;
	qb_pointer_ELE operand2;
	qb_branch_table_entry branch_table[257];
} qb_instruction_branch_table_257_ELE_ELE;

typedef struct qb_instruction_branch_table_257_SCA_SCA {
	qb_pointer_SCA operand1;
	qb_pointer_SCA operand2;
	qb_branch_table_entry branch_table[257];
} qb_instruction_branch_table_257_SCA_SCA;

typedef struct qb_instruction_branch_table_33_ELE_ELE {
	qb_pointer_ELE operand1;
	qb_pointer_ELE operand2;
	qb_branch_table_entry branch_table[33];
} qb_instruction_branch_table_33_ELE_ELE;

typedef struct qb_instruction_branch_table_33_SCA_SCA {
	qb_pointer_SCA operand1;
	qb_pointer_SCA operand2;
	qb_branch_table_entry branch_table[33];
} qb_instruction_branch_table_33_SCA_SCA;

typedef struct qb_instruction_branch_table_513_ELE_ELE {
	qb_pointer_ELE operand1;
	qb_pointer_ELE operand2;
	qb_branch_table_entry branch_table[513];
} qb_instruction_branch_table_513_ELE_ELE;

typedef struct qb_instruction_branch_table_513_SCA_SCA {
	qb_pointer_SCA operand1;
	qb_pointer_SCA operand2;
	qb_branch_table_entry branch_table[513];
} qb_instruction_branch_table_513_SCA_SCA;

typedef struct qb_instruction_branch_table_65_ELE_ELE {
	qb_pointer_ELE operand1;
	qb_pointer_ELE operand2;
	qb_branch_table_entry branch_table[65];
} qb_instruction_branch_table_65_ELE_ELE;

typedef struct qb_instruction_branch_table_65_SCA_SCA {
	qb_pointer_SCA operand1;
	qb_pointer_SCA operand2;
	qb_branch_table_entry branch_table[65];
} qb_instruction_branch_table_65_SCA_SCA;

typedef struct qb_instruction_branch_table_9_ELE_ELE {
	qb_pointer_ELE operand1;
	qb_pointer_ELE operand2;
	qb_branch_table_entry branch_table[9];
} qb_instruction_branch_table_9_ELE_ELE;

typedef struct qb_instruction_branch_table_9_SCA_SCA {
	qb_pointer_SCA operand1;
	qb_pointer_SCA operand2;
	qb_branch_table_entry branch_table[9];
} qb_instruction_branch_table_9_SCA_SCA;

typedef struct qb_instruction_exit_SCA {
	qb_pointer_SCA operand1;
} qb_instruction_exit_SCA;

typedef struct qb_instruction_jump {
	qb_op_handler next_handler;
	int8_t *instruction_pointer;
} qb_instruction_jump;

#if defined(__sun) || defined(__sun__)
#	pragma pack()
#else
#	pragma pack(pop)
#endif
#if defined(USE_TAIL_CALL_INTERPRETER_LOOP) || defined(USE_COMPUTED_GOTO_INTERPRETER_LOOP)
extern qb_op_handler op_handlers[];
#endif

#endif
