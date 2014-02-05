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


#pragma pack(push,1)

typedef struct qb_branch_table_entry {
	void *next_handler;
	int8_t *instruction_pointer;
} qb_branch_table_entry;

typedef struct qb_instruction {
	void *next_handler;
} qb_instruction;

typedef struct qb_instruction_ARR {
	void *next_handler;
	qb_pointer_ARR operand1;
} qb_instruction_ARR;

typedef struct qb_instruction_ARR_ARR {
	void *next_handler;
	qb_pointer_ARR operand1;
	qb_pointer_ARR operand2;
} qb_instruction_ARR_ARR;

typedef struct qb_instruction_ARR_ARR_ARR {
	void *next_handler;
	qb_pointer_ARR operand1;
	qb_pointer_ARR operand2;
	qb_pointer_ARR operand3;
} qb_instruction_ARR_ARR_ARR;

typedef struct qb_instruction_ARR_ARR_ARR_ARR {
	void *next_handler;
	qb_pointer_ARR operand1;
	qb_pointer_ARR operand2;
	qb_pointer_ARR operand3;
	qb_pointer_ARR operand4;
} qb_instruction_ARR_ARR_ARR_ARR;

typedef struct qb_instruction_ARR_ARR_ARR_line_id {
	void *next_handler;
	qb_pointer_ARR operand1;
	qb_pointer_ARR operand2;
	qb_pointer_ARR operand3;
	uint32_t line_id;
} qb_instruction_ARR_ARR_ARR_line_id;

typedef struct qb_instruction_ARR_ARR_ELE {
	void *next_handler;
	qb_pointer_ARR operand1;
	qb_pointer_ARR operand2;
	qb_pointer_ELE operand3;
} qb_instruction_ARR_ARR_ELE;

typedef struct qb_instruction_ARR_ARR_SCA {
	void *next_handler;
	qb_pointer_ARR operand1;
	qb_pointer_ARR operand2;
	qb_pointer_SCA operand3;
} qb_instruction_ARR_ARR_SCA;

typedef struct qb_instruction_ARR_ARR_SCA_ARR {
	void *next_handler;
	qb_pointer_ARR operand1;
	qb_pointer_ARR operand2;
	qb_pointer_SCA operand3;
	qb_pointer_ARR operand4;
} qb_instruction_ARR_ARR_SCA_ARR;

typedef struct qb_instruction_ARR_ARR_SCA_ELE {
	void *next_handler;
	qb_pointer_ARR operand1;
	qb_pointer_ARR operand2;
	qb_pointer_SCA operand3;
	qb_pointer_ELE operand4;
} qb_instruction_ARR_ARR_SCA_ELE;

typedef struct qb_instruction_ARR_ARR_SCA_SCA {
	void *next_handler;
	qb_pointer_ARR operand1;
	qb_pointer_ARR operand2;
	qb_pointer_SCA operand3;
	qb_pointer_SCA operand4;
} qb_instruction_ARR_ARR_SCA_SCA;

typedef struct qb_instruction_ARR_ARR_SCA_SCA_ARR {
	void *next_handler;
	qb_pointer_ARR operand1;
	qb_pointer_ARR operand2;
	qb_pointer_SCA operand3;
	qb_pointer_SCA operand4;
	qb_pointer_ARR operand5;
} qb_instruction_ARR_ARR_SCA_SCA_ARR;

typedef struct qb_instruction_ARR_ARR_SCA_SCA_SCA_ARR {
	void *next_handler;
	qb_pointer_ARR operand1;
	qb_pointer_ARR operand2;
	qb_pointer_SCA operand3;
	qb_pointer_SCA operand4;
	qb_pointer_SCA operand5;
	qb_pointer_ARR operand6;
} qb_instruction_ARR_ARR_SCA_SCA_SCA_ARR;

typedef struct qb_instruction_ARR_ARR_U32_ARR {
	void *next_handler;
	qb_pointer_ARR operand1;
	qb_pointer_ARR operand2;
	uint32_t operand3;
	qb_pointer_ARR operand4;
} qb_instruction_ARR_ARR_U32_ARR;

typedef struct qb_instruction_ARR_ELE {
	void *next_handler;
	qb_pointer_ARR operand1;
	qb_pointer_ELE operand2;
} qb_instruction_ARR_ELE;

typedef struct qb_instruction_ARR_ELE_SCA {
	void *next_handler;
	qb_pointer_ARR operand1;
	qb_pointer_ELE operand2;
	qb_pointer_SCA operand3;
} qb_instruction_ARR_ELE_SCA;

typedef struct qb_instruction_ARR_SCA {
	void *next_handler;
	qb_pointer_ARR operand1;
	qb_pointer_SCA operand2;
} qb_instruction_ARR_SCA;

typedef struct qb_instruction_ARR_SCA_ARR {
	void *next_handler;
	qb_pointer_ARR operand1;
	qb_pointer_SCA operand2;
	qb_pointer_ARR operand3;
} qb_instruction_ARR_SCA_ARR;

typedef struct qb_instruction_ARR_SCA_ARR_ARR {
	void *next_handler;
	qb_pointer_ARR operand1;
	qb_pointer_SCA operand2;
	qb_pointer_ARR operand3;
	qb_pointer_ARR operand4;
} qb_instruction_ARR_SCA_ARR_ARR;

typedef struct qb_instruction_ARR_SCA_ELE {
	void *next_handler;
	qb_pointer_ARR operand1;
	qb_pointer_SCA operand2;
	qb_pointer_ELE operand3;
} qb_instruction_ARR_SCA_ELE;

typedef struct qb_instruction_ARR_SCA_SCA {
	void *next_handler;
	qb_pointer_ARR operand1;
	qb_pointer_SCA operand2;
	qb_pointer_SCA operand3;
} qb_instruction_ARR_SCA_SCA;

typedef struct qb_instruction_ARR_SCA_SCA_ARR {
	void *next_handler;
	qb_pointer_ARR operand1;
	qb_pointer_SCA operand2;
	qb_pointer_SCA operand3;
	qb_pointer_ARR operand4;
} qb_instruction_ARR_SCA_SCA_ARR;

typedef struct qb_instruction_ARR_SCA_SCA_ARR_ARR_ARR {
	void *next_handler;
	qb_pointer_ARR operand1;
	qb_pointer_SCA operand2;
	qb_pointer_SCA operand3;
	qb_pointer_ARR operand4;
	qb_pointer_ARR operand5;
	qb_pointer_ARR operand6;
} qb_instruction_ARR_SCA_SCA_ARR_ARR_ARR;

typedef struct qb_instruction_ARR_SCA_SCA_ELE_ELE_ARR {
	void *next_handler;
	qb_pointer_ARR operand1;
	qb_pointer_SCA operand2;
	qb_pointer_SCA operand3;
	qb_pointer_ELE operand4;
	qb_pointer_ELE operand5;
	qb_pointer_ARR operand6;
} qb_instruction_ARR_SCA_SCA_ELE_ELE_ARR;

typedef struct qb_instruction_ARR_SCA_SCA_SCA_ARR_line_id {
	void *next_handler;
	qb_pointer_ARR operand1;
	qb_pointer_SCA operand2;
	qb_pointer_SCA operand3;
	qb_pointer_SCA operand4;
	qb_pointer_ARR operand5;
	uint32_t line_id;
} qb_instruction_ARR_SCA_SCA_SCA_ARR_line_id;

typedef struct qb_instruction_ARR_SCA_SCA_SCA_SCA_ARR {
	void *next_handler;
	qb_pointer_ARR operand1;
	qb_pointer_SCA operand2;
	qb_pointer_SCA operand3;
	qb_pointer_SCA operand4;
	qb_pointer_SCA operand5;
	qb_pointer_ARR operand6;
} qb_instruction_ARR_SCA_SCA_SCA_SCA_ARR;

typedef struct qb_instruction_ARR_SCA_SCA_SCA_SCA_U32_ARR {
	void *next_handler;
	qb_pointer_ARR operand1;
	qb_pointer_SCA operand2;
	qb_pointer_SCA operand3;
	qb_pointer_SCA operand4;
	qb_pointer_SCA operand5;
	uint32_t operand6;
	qb_pointer_ARR operand7;
} qb_instruction_ARR_SCA_SCA_SCA_SCA_U32_ARR;

typedef struct qb_instruction_ARR_U32_ARR {
	void *next_handler;
	qb_pointer_ARR operand1;
	uint32_t operand2;
	qb_pointer_ARR operand3;
} qb_instruction_ARR_U32_ARR;

typedef struct qb_instruction_ELE {
	void *next_handler;
	qb_pointer_ELE operand1;
} qb_instruction_ELE;

typedef struct qb_instruction_ELE_ARR {
	void *next_handler;
	qb_pointer_ELE operand1;
	qb_pointer_ARR operand2;
} qb_instruction_ELE_ARR;

typedef struct qb_instruction_ELE_ELE {
	void *next_handler;
	qb_pointer_ELE operand1;
	qb_pointer_ELE operand2;
} qb_instruction_ELE_ELE;

typedef struct qb_instruction_ELE_ELE_ELE {
	void *next_handler;
	qb_pointer_ELE operand1;
	qb_pointer_ELE operand2;
	qb_pointer_ELE operand3;
} qb_instruction_ELE_ELE_ELE;

typedef struct qb_instruction_ELE_ELE_ELE_ARR {
	void *next_handler;
	qb_pointer_ELE operand1;
	qb_pointer_ELE operand2;
	qb_pointer_ELE operand3;
	qb_pointer_ARR operand4;
} qb_instruction_ELE_ELE_ELE_ARR;

typedef struct qb_instruction_ELE_ELE_ELE_ELE {
	void *next_handler;
	qb_pointer_ELE operand1;
	qb_pointer_ELE operand2;
	qb_pointer_ELE operand3;
	qb_pointer_ELE operand4;
} qb_instruction_ELE_ELE_ELE_ELE;

typedef struct qb_instruction_ELE_ELE_ELE_SCA {
	void *next_handler;
	qb_pointer_ELE operand1;
	qb_pointer_ELE operand2;
	qb_pointer_ELE operand3;
	qb_pointer_SCA operand4;
} qb_instruction_ELE_ELE_ELE_SCA;

typedef struct qb_instruction_ELE_ELE_ELE_line_id {
	void *next_handler;
	qb_pointer_ELE operand1;
	qb_pointer_ELE operand2;
	qb_pointer_ELE operand3;
	uint32_t line_id;
} qb_instruction_ELE_ELE_ELE_line_id;

typedef struct qb_instruction_ELE_ELE_SCA {
	void *next_handler;
	qb_pointer_ELE operand1;
	qb_pointer_ELE operand2;
	qb_pointer_SCA operand3;
} qb_instruction_ELE_ELE_SCA;

typedef struct qb_instruction_ELE_ELE_SCA_SCA {
	void *next_handler;
	qb_pointer_ELE operand1;
	qb_pointer_ELE operand2;
	qb_pointer_SCA operand3;
	qb_pointer_SCA operand4;
} qb_instruction_ELE_ELE_SCA_SCA;

typedef struct qb_instruction_ELE_ELE_SCA_SCA_ARR_ARR {
	void *next_handler;
	qb_pointer_ELE operand1;
	qb_pointer_ELE operand2;
	qb_pointer_SCA operand3;
	qb_pointer_SCA operand4;
	qb_pointer_ARR operand5;
	qb_pointer_ARR operand6;
} qb_instruction_ELE_ELE_SCA_SCA_ARR_ARR;

typedef struct qb_instruction_ELE_SCA_SCA {
	void *next_handler;
	qb_pointer_ELE operand1;
	qb_pointer_SCA operand2;
	qb_pointer_SCA operand3;
} qb_instruction_ELE_SCA_SCA;

typedef struct qb_instruction_ELE_U32_ARR {
	void *next_handler;
	qb_pointer_ELE operand1;
	uint32_t operand2;
	qb_pointer_ARR operand3;
} qb_instruction_ELE_U32_ARR;

typedef struct qb_instruction_SCA {
	void *next_handler;
	qb_pointer_SCA operand1;
} qb_instruction_SCA;

typedef struct qb_instruction_SCA_ARR {
	void *next_handler;
	qb_pointer_SCA operand1;
	qb_pointer_ARR operand2;
} qb_instruction_SCA_ARR;

typedef struct qb_instruction_SCA_ARR_ARR {
	void *next_handler;
	qb_pointer_SCA operand1;
	qb_pointer_ARR operand2;
	qb_pointer_ARR operand3;
} qb_instruction_SCA_ARR_ARR;

typedef struct qb_instruction_SCA_ARR_SCA_line_id {
	void *next_handler;
	qb_pointer_SCA operand1;
	qb_pointer_ARR operand2;
	qb_pointer_SCA operand3;
	uint32_t line_id;
} qb_instruction_SCA_ARR_SCA_line_id;

typedef struct qb_instruction_SCA_ELE_ARR {
	void *next_handler;
	qb_pointer_SCA operand1;
	qb_pointer_ELE operand2;
	qb_pointer_ARR operand3;
} qb_instruction_SCA_ELE_ARR;

typedef struct qb_instruction_SCA_SCA {
	void *next_handler;
	qb_pointer_SCA operand1;
	qb_pointer_SCA operand2;
} qb_instruction_SCA_SCA;

typedef struct qb_instruction_SCA_SCA_ARR {
	void *next_handler;
	qb_pointer_SCA operand1;
	qb_pointer_SCA operand2;
	qb_pointer_ARR operand3;
} qb_instruction_SCA_SCA_ARR;

typedef struct qb_instruction_SCA_SCA_SCA {
	void *next_handler;
	qb_pointer_SCA operand1;
	qb_pointer_SCA operand2;
	qb_pointer_SCA operand3;
} qb_instruction_SCA_SCA_SCA;

typedef struct qb_instruction_SCA_SCA_SCA_ARR {
	void *next_handler;
	qb_pointer_SCA operand1;
	qb_pointer_SCA operand2;
	qb_pointer_SCA operand3;
	qb_pointer_ARR operand4;
} qb_instruction_SCA_SCA_SCA_ARR;

typedef struct qb_instruction_SCA_SCA_SCA_SCA {
	void *next_handler;
	qb_pointer_SCA operand1;
	qb_pointer_SCA operand2;
	qb_pointer_SCA operand3;
	qb_pointer_SCA operand4;
} qb_instruction_SCA_SCA_SCA_SCA;

typedef struct qb_instruction_SCA_SCA_SCA_SCA_ARR_ARR {
	void *next_handler;
	qb_pointer_SCA operand1;
	qb_pointer_SCA operand2;
	qb_pointer_SCA operand3;
	qb_pointer_SCA operand4;
	qb_pointer_ARR operand5;
	qb_pointer_ARR operand6;
} qb_instruction_SCA_SCA_SCA_SCA_ARR_ARR;

typedef struct qb_instruction_SCA_SCA_SCA_SCA_SCA {
	void *next_handler;
	qb_pointer_SCA operand1;
	qb_pointer_SCA operand2;
	qb_pointer_SCA operand3;
	qb_pointer_SCA operand4;
	qb_pointer_SCA operand5;
} qb_instruction_SCA_SCA_SCA_SCA_SCA;

typedef struct qb_instruction_SCA_SCA_SCA_SCA_SCA_SCA_SCA {
	void *next_handler;
	qb_pointer_SCA operand1;
	qb_pointer_SCA operand2;
	qb_pointer_SCA operand3;
	qb_pointer_SCA operand4;
	qb_pointer_SCA operand5;
	qb_pointer_SCA operand6;
	qb_pointer_SCA operand7;
} qb_instruction_SCA_SCA_SCA_SCA_SCA_SCA_SCA;

typedef struct qb_instruction_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_SCA_U32_U32 {
	void *next_handler;
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
	void *next_handler;
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
	void *next_handler;
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
	void *next_handler;
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
	void *next_handler;
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
	void *next_handler;
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
	void *next_handler;
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
	void *next_handler;
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
	void *next_handler;
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
	void *next_handler;
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
	void *next_handler;
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
	void *next_handler;
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
	void *next_handler;
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
	void *next_handler;
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
	void *next_handler;
	qb_pointer_SCA operand1;
	qb_pointer_SCA operand2;
	qb_pointer_SCA operand3;
	qb_pointer_SCA operand4;
	qb_pointer_SCA operand5;
	qb_pointer_SCA operand6;
	uint32_t line_id;
} qb_instruction_SCA_SCA_SCA_SCA_SCA_SCA_line_id;

typedef struct qb_instruction_SCA_SCA_SCA_SCA_SCA_line_id {
	void *next_handler;
	qb_pointer_SCA operand1;
	qb_pointer_SCA operand2;
	qb_pointer_SCA operand3;
	qb_pointer_SCA operand4;
	qb_pointer_SCA operand5;
	uint32_t line_id;
} qb_instruction_SCA_SCA_SCA_SCA_SCA_line_id;

typedef struct qb_instruction_SCA_SCA_SCA_SCA_U32_U32 {
	void *next_handler;
	qb_pointer_SCA operand1;
	qb_pointer_SCA operand2;
	qb_pointer_SCA operand3;
	qb_pointer_SCA operand4;
	uint32_t operand5;
	uint32_t operand6;
} qb_instruction_SCA_SCA_SCA_SCA_U32_U32;

typedef struct qb_instruction_SCA_SCA_SCA_SCA_U32_U32_SCA {
	void *next_handler;
	qb_pointer_SCA operand1;
	qb_pointer_SCA operand2;
	qb_pointer_SCA operand3;
	qb_pointer_SCA operand4;
	uint32_t operand5;
	uint32_t operand6;
	qb_pointer_SCA operand7;
} qb_instruction_SCA_SCA_SCA_SCA_U32_U32_SCA;

typedef struct qb_instruction_SCA_SCA_SCA_SCA_line_id {
	void *next_handler;
	qb_pointer_SCA operand1;
	qb_pointer_SCA operand2;
	qb_pointer_SCA operand3;
	qb_pointer_SCA operand4;
	uint32_t line_id;
} qb_instruction_SCA_SCA_SCA_SCA_line_id;

typedef struct qb_instruction_SCA_SCA_SCA_U32_ARR {
	void *next_handler;
	qb_pointer_SCA operand1;
	qb_pointer_SCA operand2;
	qb_pointer_SCA operand3;
	uint32_t operand4;
	qb_pointer_ARR operand5;
} qb_instruction_SCA_SCA_SCA_U32_ARR;

typedef struct qb_instruction_SCA_SCA_SCA_line_id {
	void *next_handler;
	qb_pointer_SCA operand1;
	qb_pointer_SCA operand2;
	qb_pointer_SCA operand3;
	uint32_t line_id;
} qb_instruction_SCA_SCA_SCA_line_id;

typedef struct qb_instruction_SCA_SCA_U32_ARR {
	void *next_handler;
	qb_pointer_SCA operand1;
	qb_pointer_SCA operand2;
	uint32_t operand3;
	qb_pointer_ARR operand4;
} qb_instruction_SCA_SCA_U32_ARR;

typedef struct qb_instruction_SCA_SCA_U32_U32 {
	void *next_handler;
	qb_pointer_SCA operand1;
	qb_pointer_SCA operand2;
	uint32_t operand3;
	uint32_t operand4;
} qb_instruction_SCA_SCA_U32_U32;

typedef struct qb_instruction_SCA_SCA_line_id {
	void *next_handler;
	qb_pointer_SCA operand1;
	qb_pointer_SCA operand2;
	uint32_t line_id;
} qb_instruction_SCA_SCA_line_id;

typedef struct qb_instruction_SCA_U32_ARR {
	void *next_handler;
	qb_pointer_SCA operand1;
	uint32_t operand2;
	qb_pointer_ARR operand3;
} qb_instruction_SCA_U32_ARR;

typedef struct qb_instruction_SCA_U32_U32_SCA {
	void *next_handler;
	qb_pointer_SCA operand1;
	uint32_t operand2;
	uint32_t operand3;
	qb_pointer_SCA operand4;
} qb_instruction_SCA_U32_U32_SCA;

typedef struct qb_instruction_SCA_line_id {
	void *next_handler;
	qb_pointer_SCA operand1;
	uint32_t line_id;
} qb_instruction_SCA_line_id;

typedef struct qb_instruction_U32_ARR {
	void *next_handler;
	uint32_t operand1;
	qb_pointer_ARR operand2;
} qb_instruction_U32_ARR;

typedef struct qb_instruction_U32_ARR_ARR {
	void *next_handler;
	uint32_t operand1;
	qb_pointer_ARR operand2;
	qb_pointer_ARR operand3;
} qb_instruction_U32_ARR_ARR;

typedef struct qb_instruction_branch_ELE {
	void *next_handler1;
	int8_t *instruction_pointer1;
	void *next_handler2;
	int8_t *instruction_pointer2;
	qb_pointer_ELE operand1;
} qb_instruction_branch_ELE;

typedef struct qb_instruction_branch_ELE_ELE {
	void *next_handler1;
	int8_t *instruction_pointer1;
	void *next_handler2;
	int8_t *instruction_pointer2;
	qb_pointer_ELE operand1;
	qb_pointer_ELE operand2;
} qb_instruction_branch_ELE_ELE;

typedef struct qb_instruction_branch_SCA {
	void *next_handler1;
	int8_t *instruction_pointer1;
	void *next_handler2;
	int8_t *instruction_pointer2;
	qb_pointer_SCA operand1;
} qb_instruction_branch_SCA;

typedef struct qb_instruction_branch_SCA_SCA {
	void *next_handler1;
	int8_t *instruction_pointer1;
	void *next_handler2;
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
	void *next_handler;
	int8_t *instruction_pointer;
} qb_instruction_jump;

#pragma pack(pop)

extern void *op_handlers[];

int qb_compare_ascending_F32(const void *p1, const void *p2);
int qb_compare_ascending_F32_array(const void *p1, const void *p2, const void *p3);
int qb_compare_ascending_F64(const void *p1, const void *p2);
int qb_compare_ascending_F64_array(const void *p1, const void *p2, const void *p3);
int qb_compare_ascending_S08(const void *p1, const void *p2);
int qb_compare_ascending_S08_array(const void *p1, const void *p2, const void *p3);
int qb_compare_ascending_S16(const void *p1, const void *p2);
int qb_compare_ascending_S16_array(const void *p1, const void *p2, const void *p3);
int qb_compare_ascending_S32(const void *p1, const void *p2);
int qb_compare_ascending_S32_array(const void *p1, const void *p2, const void *p3);
int qb_compare_ascending_S64(const void *p1, const void *p2);
int qb_compare_ascending_S64_array(const void *p1, const void *p2, const void *p3);
int qb_compare_ascending_U08(const void *p1, const void *p2);
int qb_compare_ascending_U08_array(const void *p1, const void *p2, const void *p3);
int qb_compare_ascending_U16(const void *p1, const void *p2);
int qb_compare_ascending_U16_array(const void *p1, const void *p2, const void *p3);
int qb_compare_ascending_U32(const void *p1, const void *p2);
int qb_compare_ascending_U32_array(const void *p1, const void *p2, const void *p3);
int qb_compare_ascending_U64(const void *p1, const void *p2);
int qb_compare_ascending_U64_array(const void *p1, const void *p2, const void *p3);
int qb_compare_descending_F32(const void *p1, const void *p2);
int qb_compare_descending_F32_array(const void *p1, const void *p2, const void *p3);
int qb_compare_descending_F64(const void *p1, const void *p2);
int qb_compare_descending_F64_array(const void *p1, const void *p2, const void *p3);
int qb_compare_descending_S08(const void *p1, const void *p2);
int qb_compare_descending_S08_array(const void *p1, const void *p2, const void *p3);
int qb_compare_descending_S16(const void *p1, const void *p2);
int qb_compare_descending_S16_array(const void *p1, const void *p2, const void *p3);
int qb_compare_descending_S32(const void *p1, const void *p2);
int qb_compare_descending_S32_array(const void *p1, const void *p2, const void *p3);
int qb_compare_descending_S64(const void *p1, const void *p2);
int qb_compare_descending_S64_array(const void *p1, const void *p2, const void *p3);
int qb_compare_descending_U08(const void *p1, const void *p2);
int qb_compare_descending_U08_array(const void *p1, const void *p2, const void *p3);
int qb_compare_descending_U16(const void *p1, const void *p2);
int qb_compare_descending_U16_array(const void *p1, const void *p2, const void *p3);
int qb_compare_descending_U32(const void *p1, const void *p2);
int qb_compare_descending_U32_array(const void *p1, const void *p2, const void *p3);
int qb_compare_descending_U64(const void *p1, const void *p2);
int qb_compare_descending_U64_array(const void *p1, const void *p2, const void *p3);
int32_t qb_compare_array_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count);
int32_t qb_compare_array_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count);
int32_t qb_compare_array_S08(int8_t *op1_ptr, uint32_t op1_count, int8_t *op2_ptr, uint32_t op2_count);
int32_t qb_compare_array_S16(int16_t *op1_ptr, uint32_t op1_count, int16_t *op2_ptr, uint32_t op2_count);
int32_t qb_compare_array_S32(int32_t *op1_ptr, uint32_t op1_count, int32_t *op2_ptr, uint32_t op2_count);
int32_t qb_compare_array_S64(int64_t *op1_ptr, uint32_t op1_count, int64_t *op2_ptr, uint32_t op2_count);
int32_t qb_compare_array_U08(uint8_t *op1_ptr, uint32_t op1_count, uint8_t *op2_ptr, uint32_t op2_count);
int32_t qb_compare_array_U16(uint16_t *op1_ptr, uint32_t op1_count, uint16_t *op2_ptr, uint32_t op2_count);
int32_t qb_compare_array_U32(uint32_t *op1_ptr, uint32_t op1_count, uint32_t *op2_ptr, uint32_t op2_count);
int32_t qb_compare_array_U64(uint64_t *op1_ptr, uint32_t op1_count, uint64_t *op2_ptr, uint32_t op2_count);
void qb_convert_hsl_to_rgb_F32(float32_t *op1_ptr, float32_t *res_ptr);
void qb_convert_hsl_to_rgb_F64(float64_t *op1_ptr, float64_t *res_ptr);
void qb_convert_hsv_to_rgb_F32(float32_t *op1_ptr, float32_t *res_ptr);
void qb_convert_hsv_to_rgb_F64(float64_t *op1_ptr, float64_t *res_ptr);
void qb_convert_rgb_to_hsl_F32(float32_t *op1_ptr, float32_t *res_ptr);
void qb_convert_rgb_to_hsl_F64(float64_t *op1_ptr, float64_t *res_ptr);
void qb_convert_rgb_to_hsv_F32(float32_t *op1_ptr, float32_t *res_ptr);
void qb_convert_rgb_to_hsv_F64(float64_t *op1_ptr, float64_t *res_ptr);
void qb_relocate_elements_F32(float32_t *elements, uint32_t *old_dims, uint32_t *new_dims, uint32_t dimension_count);
void qb_relocate_elements_F64(float64_t *elements, uint32_t *old_dims, uint32_t *new_dims, uint32_t dimension_count);
void qb_relocate_elements_I08(int8_t *elements, uint32_t *old_dims, uint32_t *new_dims, uint32_t dimension_count);
void qb_relocate_elements_I16(int16_t *elements, uint32_t *old_dims, uint32_t *new_dims, uint32_t dimension_count);
void qb_relocate_elements_I32(int32_t *elements, uint32_t *old_dims, uint32_t *new_dims, uint32_t dimension_count);
void qb_relocate_elements_I64(int64_t *elements, uint32_t *old_dims, uint32_t *new_dims, uint32_t dimension_count);
int32_t qb_do_array_column_F32(qb_interpreter_context *__restrict cxt, float32_t *op1_ptr, uint32_t op1_count, uint32_t op2, uint32_t op3, uint32_t op4, float32_t *res_ptr, uint32_t res_count, uint32_t line_id);
int32_t qb_do_array_column_F64(qb_interpreter_context *__restrict cxt, float64_t *op1_ptr, uint32_t op1_count, uint32_t op2, uint32_t op3, uint32_t op4, float64_t *res_ptr, uint32_t res_count, uint32_t line_id);
int32_t qb_do_array_column_I08(qb_interpreter_context *__restrict cxt, int8_t *op1_ptr, uint32_t op1_count, uint32_t op2, uint32_t op3, uint32_t op4, int8_t *res_ptr, uint32_t res_count, uint32_t line_id);
int32_t qb_do_array_column_I16(qb_interpreter_context *__restrict cxt, int16_t *op1_ptr, uint32_t op1_count, uint32_t op2, uint32_t op3, uint32_t op4, int16_t *res_ptr, uint32_t res_count, uint32_t line_id);
int32_t qb_do_array_column_I32(qb_interpreter_context *__restrict cxt, int32_t *op1_ptr, uint32_t op1_count, uint32_t op2, uint32_t op3, uint32_t op4, int32_t *res_ptr, uint32_t res_count, uint32_t line_id);
int32_t qb_do_array_column_I64(qb_interpreter_context *__restrict cxt, int64_t *op1_ptr, uint32_t op1_count, uint32_t op2, uint32_t op3, uint32_t op4, int64_t *res_ptr, uint32_t res_count, uint32_t line_id);
int32_t qb_do_divide_multiple_times_S08(qb_interpreter_context *__restrict cxt, int8_t *op1_ptr, uint32_t op1_count, int8_t *op2_ptr, uint32_t op2_count, int8_t *res_ptr, uint32_t res_count, uint32_t line_id);
int32_t qb_do_divide_multiple_times_S16(qb_interpreter_context *__restrict cxt, int16_t *op1_ptr, uint32_t op1_count, int16_t *op2_ptr, uint32_t op2_count, int16_t *res_ptr, uint32_t res_count, uint32_t line_id);
int32_t qb_do_divide_multiple_times_S32(qb_interpreter_context *__restrict cxt, int32_t *op1_ptr, uint32_t op1_count, int32_t *op2_ptr, uint32_t op2_count, int32_t *res_ptr, uint32_t res_count, uint32_t line_id);
int32_t qb_do_divide_multiple_times_S64(qb_interpreter_context *__restrict cxt, int64_t *op1_ptr, uint32_t op1_count, int64_t *op2_ptr, uint32_t op2_count, int64_t *res_ptr, uint32_t res_count, uint32_t line_id);
int32_t qb_do_divide_multiple_times_U08(qb_interpreter_context *__restrict cxt, uint8_t *op1_ptr, uint32_t op1_count, uint8_t *op2_ptr, uint32_t op2_count, uint8_t *res_ptr, uint32_t res_count, uint32_t line_id);
int32_t qb_do_divide_multiple_times_U16(qb_interpreter_context *__restrict cxt, uint16_t *op1_ptr, uint32_t op1_count, uint16_t *op2_ptr, uint32_t op2_count, uint16_t *res_ptr, uint32_t res_count, uint32_t line_id);
int32_t qb_do_divide_multiple_times_U32(qb_interpreter_context *__restrict cxt, uint32_t *op1_ptr, uint32_t op1_count, uint32_t *op2_ptr, uint32_t op2_count, uint32_t *res_ptr, uint32_t res_count, uint32_t line_id);
int32_t qb_do_divide_multiple_times_U64(qb_interpreter_context *__restrict cxt, uint64_t *op1_ptr, uint32_t op1_count, uint64_t *op2_ptr, uint32_t op2_count, uint64_t *res_ptr, uint32_t res_count, uint32_t line_id);
int32_t qb_do_modulo_multiple_times_S08(qb_interpreter_context *__restrict cxt, int8_t *op1_ptr, uint32_t op1_count, int8_t *op2_ptr, uint32_t op2_count, int8_t *res_ptr, uint32_t res_count, uint32_t line_id);
int32_t qb_do_modulo_multiple_times_S16(qb_interpreter_context *__restrict cxt, int16_t *op1_ptr, uint32_t op1_count, int16_t *op2_ptr, uint32_t op2_count, int16_t *res_ptr, uint32_t res_count, uint32_t line_id);
int32_t qb_do_modulo_multiple_times_S32(qb_interpreter_context *__restrict cxt, int32_t *op1_ptr, uint32_t op1_count, int32_t *op2_ptr, uint32_t op2_count, int32_t *res_ptr, uint32_t res_count, uint32_t line_id);
int32_t qb_do_modulo_multiple_times_S64(qb_interpreter_context *__restrict cxt, int64_t *op1_ptr, uint32_t op1_count, int64_t *op2_ptr, uint32_t op2_count, int64_t *res_ptr, uint32_t res_count, uint32_t line_id);
int32_t qb_do_modulo_multiple_times_U08(qb_interpreter_context *__restrict cxt, uint8_t *op1_ptr, uint32_t op1_count, uint8_t *op2_ptr, uint32_t op2_count, uint8_t *res_ptr, uint32_t res_count, uint32_t line_id);
int32_t qb_do_modulo_multiple_times_U16(qb_interpreter_context *__restrict cxt, uint16_t *op1_ptr, uint32_t op1_count, uint16_t *op2_ptr, uint32_t op2_count, uint16_t *res_ptr, uint32_t res_count, uint32_t line_id);
int32_t qb_do_modulo_multiple_times_U32(qb_interpreter_context *__restrict cxt, uint32_t *op1_ptr, uint32_t op1_count, uint32_t *op2_ptr, uint32_t op2_count, uint32_t *res_ptr, uint32_t res_count, uint32_t line_id);
int32_t qb_do_modulo_multiple_times_U64(qb_interpreter_context *__restrict cxt, uint64_t *op1_ptr, uint32_t op1_count, uint64_t *op2_ptr, uint32_t op2_count, uint64_t *res_ptr, uint32_t res_count, uint32_t line_id);
void qb_do_abs_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count);
void qb_do_abs_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void qb_do_abs_multiple_times_S08(int8_t *op1_ptr, uint32_t op1_count, int8_t *res_ptr, uint32_t res_count);
void qb_do_abs_multiple_times_S16(int16_t *op1_ptr, uint32_t op1_count, int16_t *res_ptr, uint32_t res_count);
void qb_do_abs_multiple_times_S32(int32_t *op1_ptr, uint32_t op1_count, int32_t *res_ptr, uint32_t res_count);
void qb_do_abs_multiple_times_S64(int64_t *op1_ptr, uint32_t op1_count, int64_t *res_ptr, uint32_t res_count);
void qb_do_accommodate_size_copy_dimension_4_U32(qb_interpreter_context *__restrict cxt, uint32_t op1, uint32_t op2, uint32_t *op3_ptr, uint32_t *op4_ptr, uint32_t op5, uint32_t op6, uint32_t *op7_ptr, uint32_t *op8_ptr, uint32_t op9, uint32_t op10, uint32_t *op11_ptr, uint32_t *op12_ptr, uint32_t op13, uint32_t op14, uint32_t *op15_ptr, uint32_t *op16_ptr, uint32_t op17, uint32_t op18);
void qb_do_accommodate_size_copy_dimension_5_U32(qb_interpreter_context *__restrict cxt, uint32_t op1, uint32_t op2, uint32_t *op3_ptr, uint32_t *op4_ptr, uint32_t op5, uint32_t op6, uint32_t *op7_ptr, uint32_t *op8_ptr, uint32_t op9, uint32_t op10, uint32_t *op11_ptr, uint32_t *op12_ptr, uint32_t op13, uint32_t op14, uint32_t *op15_ptr, uint32_t *op16_ptr, uint32_t op17, uint32_t op18, uint32_t *op19_ptr, uint32_t *op20_ptr, uint32_t op21, uint32_t op22);
void qb_do_accommodate_size_copy_dimension_6_U32(qb_interpreter_context *__restrict cxt, uint32_t op1, uint32_t op2, uint32_t *op3_ptr, uint32_t *op4_ptr, uint32_t op5, uint32_t op6, uint32_t *op7_ptr, uint32_t *op8_ptr, uint32_t op9, uint32_t op10, uint32_t *op11_ptr, uint32_t *op12_ptr, uint32_t op13, uint32_t op14, uint32_t *op15_ptr, uint32_t *op16_ptr, uint32_t op17, uint32_t op18, uint32_t *op19_ptr, uint32_t *op20_ptr, uint32_t op21, uint32_t op22, uint32_t *op23_ptr, uint32_t *op24_ptr, uint32_t op25, uint32_t op26);
void qb_do_accommodate_size_copy_dimension_7_U32(qb_interpreter_context *__restrict cxt, uint32_t op1, uint32_t op2, uint32_t *op3_ptr, uint32_t *op4_ptr, uint32_t op5, uint32_t op6, uint32_t *op7_ptr, uint32_t *op8_ptr, uint32_t op9, uint32_t op10, uint32_t *op11_ptr, uint32_t *op12_ptr, uint32_t op13, uint32_t op14, uint32_t *op15_ptr, uint32_t *op16_ptr, uint32_t op17, uint32_t op18, uint32_t *op19_ptr, uint32_t *op20_ptr, uint32_t op21, uint32_t op22, uint32_t *op23_ptr, uint32_t *op24_ptr, uint32_t op25, uint32_t op26, uint32_t *op27_ptr, uint32_t *op28_ptr, uint32_t op29, uint32_t op30);
void qb_do_accommodate_size_copy_dimension_8_U32(qb_interpreter_context *__restrict cxt, uint32_t op1, uint32_t op2, uint32_t *op3_ptr, uint32_t *op4_ptr, uint32_t op5, uint32_t op6, uint32_t *op7_ptr, uint32_t *op8_ptr, uint32_t op9, uint32_t op10, uint32_t *op11_ptr, uint32_t *op12_ptr, uint32_t op13, uint32_t op14, uint32_t *op15_ptr, uint32_t *op16_ptr, uint32_t op17, uint32_t op18, uint32_t *op19_ptr, uint32_t *op20_ptr, uint32_t op21, uint32_t op22, uint32_t *op23_ptr, uint32_t *op24_ptr, uint32_t op25, uint32_t op26, uint32_t *op27_ptr, uint32_t *op28_ptr, uint32_t op29, uint32_t op30, uint32_t *op31_ptr, uint32_t *op32_ptr, uint32_t op33, uint32_t op34);
void qb_do_accommodate_size_update_dimension_U32(qb_interpreter_context *__restrict cxt, uint32_t op1, uint32_t *op2_ptr, uint32_t *op3_ptr, uint32_t op4, uint32_t op5, uint32_t op6);
void qb_do_acos_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count);
void qb_do_acos_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void qb_do_acosh_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count);
void qb_do_acosh_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void qb_do_add_2x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, float32_t *res_ptr, uint32_t res_count);
void qb_do_add_2x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, float64_t *res_ptr, uint32_t res_count);
void qb_do_add_3x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, float32_t *res_ptr, uint32_t res_count);
void qb_do_add_3x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, float64_t *res_ptr, uint32_t res_count);
void qb_do_add_4x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, float32_t *res_ptr, uint32_t res_count);
void qb_do_add_4x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, float64_t *res_ptr, uint32_t res_count);
void qb_do_add_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, float32_t *res_ptr, uint32_t res_count);
void qb_do_add_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, float64_t *res_ptr, uint32_t res_count);
void qb_do_add_multiple_times_I08(int8_t *op1_ptr, uint32_t op1_count, int8_t *op2_ptr, uint32_t op2_count, int8_t *res_ptr, uint32_t res_count);
void qb_do_add_multiple_times_I16(int16_t *op1_ptr, uint32_t op1_count, int16_t *op2_ptr, uint32_t op2_count, int16_t *res_ptr, uint32_t res_count);
void qb_do_add_multiple_times_I32(int32_t *op1_ptr, uint32_t op1_count, int32_t *op2_ptr, uint32_t op2_count, int32_t *res_ptr, uint32_t res_count);
void qb_do_add_multiple_times_I64(int64_t *op1_ptr, uint32_t op1_count, int64_t *op2_ptr, uint32_t op2_count, int64_t *res_ptr, uint32_t res_count);
void qb_do_alpha_blend_2x_F32(float32_t *op1_ptr, float32_t *op2_ptr, float32_t *res_ptr);
void qb_do_alpha_blend_2x_F64(float64_t *op1_ptr, float64_t *op2_ptr, float64_t *res_ptr);
void qb_do_alpha_blend_2x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, float32_t *res_ptr, uint32_t res_count);
void qb_do_alpha_blend_2x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, float64_t *res_ptr, uint32_t res_count);
void qb_do_alpha_blend_4x_F32(float32_t *op1_ptr, float32_t *op2_ptr, float32_t *res_ptr);
void qb_do_alpha_blend_4x_F64(float64_t *op1_ptr, float64_t *op2_ptr, float64_t *res_ptr);
void qb_do_alpha_blend_4x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, float32_t *res_ptr, uint32_t res_count);
void qb_do_alpha_blend_4x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, float64_t *res_ptr, uint32_t res_count);
void qb_do_append_multidimensional_variable_F32(qb_interpreter_context *__restrict cxt, float32_t *op1_ptr, uint32_t op1_count, uint32_t *op2_ptr, uint32_t op2_count, uint32_t op3, uint8_t *res_ptr, uint32_t *res_count_ptr);
void qb_do_append_multidimensional_variable_F64(qb_interpreter_context *__restrict cxt, float64_t *op1_ptr, uint32_t op1_count, uint32_t *op2_ptr, uint32_t op2_count, uint32_t op3, uint8_t *res_ptr, uint32_t *res_count_ptr);
void qb_do_append_multidimensional_variable_S08(qb_interpreter_context *__restrict cxt, int8_t *op1_ptr, uint32_t op1_count, uint32_t *op2_ptr, uint32_t op2_count, uint32_t op3, uint8_t *res_ptr, uint32_t *res_count_ptr);
void qb_do_append_multidimensional_variable_S16(qb_interpreter_context *__restrict cxt, int16_t *op1_ptr, uint32_t op1_count, uint32_t *op2_ptr, uint32_t op2_count, uint32_t op3, uint8_t *res_ptr, uint32_t *res_count_ptr);
void qb_do_append_multidimensional_variable_S32(qb_interpreter_context *__restrict cxt, int32_t *op1_ptr, uint32_t op1_count, uint32_t *op2_ptr, uint32_t op2_count, uint32_t op3, uint8_t *res_ptr, uint32_t *res_count_ptr);
void qb_do_append_multidimensional_variable_S64(qb_interpreter_context *__restrict cxt, int64_t *op1_ptr, uint32_t op1_count, uint32_t *op2_ptr, uint32_t op2_count, uint32_t op3, uint8_t *res_ptr, uint32_t *res_count_ptr);
void qb_do_append_multidimensional_variable_U08(qb_interpreter_context *__restrict cxt, uint8_t *op1_ptr, uint32_t op1_count, uint32_t *op2_ptr, uint32_t op2_count, uint32_t op3, uint8_t *res_ptr, uint32_t *res_count_ptr);
void qb_do_append_multidimensional_variable_U16(qb_interpreter_context *__restrict cxt, uint16_t *op1_ptr, uint32_t op1_count, uint32_t *op2_ptr, uint32_t op2_count, uint32_t op3, uint8_t *res_ptr, uint32_t *res_count_ptr);
void qb_do_append_multidimensional_variable_U32(qb_interpreter_context *__restrict cxt, uint32_t *op1_ptr, uint32_t op1_count, uint32_t *op2_ptr, uint32_t op2_count, uint32_t op3, uint8_t *res_ptr, uint32_t *res_count_ptr);
void qb_do_append_multidimensional_variable_U64(qb_interpreter_context *__restrict cxt, uint64_t *op1_ptr, uint32_t op1_count, uint32_t *op2_ptr, uint32_t op2_count, uint32_t op3, uint8_t *res_ptr, uint32_t *res_count_ptr);
void qb_do_append_string_U08(qb_interpreter_context *__restrict cxt, uint8_t *op1_ptr, uint32_t op1_count, uint32_t op2, uint8_t *res_ptr, uint32_t *res_count_ptr);
void qb_do_append_variable_F32(qb_interpreter_context *__restrict cxt, float32_t op1, uint32_t op2, uint8_t *res_ptr, uint32_t *res_count_ptr);
void qb_do_append_variable_F64(qb_interpreter_context *__restrict cxt, float64_t op1, uint32_t op2, uint8_t *res_ptr, uint32_t *res_count_ptr);
void qb_do_append_variable_S08(qb_interpreter_context *__restrict cxt, int8_t op1, uint32_t op2, uint8_t *res_ptr, uint32_t *res_count_ptr);
void qb_do_append_variable_S16(qb_interpreter_context *__restrict cxt, int16_t op1, uint32_t op2, uint8_t *res_ptr, uint32_t *res_count_ptr);
void qb_do_append_variable_S32(qb_interpreter_context *__restrict cxt, int32_t op1, uint32_t op2, uint8_t *res_ptr, uint32_t *res_count_ptr);
void qb_do_append_variable_S64(qb_interpreter_context *__restrict cxt, int64_t op1, uint32_t op2, uint8_t *res_ptr, uint32_t *res_count_ptr);
void qb_do_append_variable_U08(qb_interpreter_context *__restrict cxt, uint8_t op1, uint32_t op2, uint8_t *res_ptr, uint32_t *res_count_ptr);
void qb_do_append_variable_U16(qb_interpreter_context *__restrict cxt, uint16_t op1, uint32_t op2, uint8_t *res_ptr, uint32_t *res_count_ptr);
void qb_do_append_variable_U32(qb_interpreter_context *__restrict cxt, uint32_t op1, uint32_t op2, uint8_t *res_ptr, uint32_t *res_count_ptr);
void qb_do_append_variable_U64(qb_interpreter_context *__restrict cxt, uint64_t op1, uint32_t op2, uint8_t *res_ptr, uint32_t *res_count_ptr);
void qb_do_append_variable_multiple_times_F32(qb_interpreter_context *__restrict cxt, float32_t *op1_ptr, uint32_t op1_count, uint32_t op2, uint8_t *res_ptr, uint32_t *res_count_ptr);
void qb_do_append_variable_multiple_times_F64(qb_interpreter_context *__restrict cxt, float64_t *op1_ptr, uint32_t op1_count, uint32_t op2, uint8_t *res_ptr, uint32_t *res_count_ptr);
void qb_do_append_variable_multiple_times_S08(qb_interpreter_context *__restrict cxt, int8_t *op1_ptr, uint32_t op1_count, uint32_t op2, uint8_t *res_ptr, uint32_t *res_count_ptr);
void qb_do_append_variable_multiple_times_S16(qb_interpreter_context *__restrict cxt, int16_t *op1_ptr, uint32_t op1_count, uint32_t op2, uint8_t *res_ptr, uint32_t *res_count_ptr);
void qb_do_append_variable_multiple_times_S32(qb_interpreter_context *__restrict cxt, int32_t *op1_ptr, uint32_t op1_count, uint32_t op2, uint8_t *res_ptr, uint32_t *res_count_ptr);
void qb_do_append_variable_multiple_times_S64(qb_interpreter_context *__restrict cxt, int64_t *op1_ptr, uint32_t op1_count, uint32_t op2, uint8_t *res_ptr, uint32_t *res_count_ptr);
void qb_do_append_variable_multiple_times_U08(qb_interpreter_context *__restrict cxt, uint8_t *op1_ptr, uint32_t op1_count, uint32_t op2, uint8_t *res_ptr, uint32_t *res_count_ptr);
void qb_do_append_variable_multiple_times_U16(qb_interpreter_context *__restrict cxt, uint16_t *op1_ptr, uint32_t op1_count, uint32_t op2, uint8_t *res_ptr, uint32_t *res_count_ptr);
void qb_do_append_variable_multiple_times_U32(qb_interpreter_context *__restrict cxt, uint32_t *op1_ptr, uint32_t op1_count, uint32_t op2, uint8_t *res_ptr, uint32_t *res_count_ptr);
void qb_do_append_variable_multiple_times_U64(qb_interpreter_context *__restrict cxt, uint64_t *op1_ptr, uint32_t op1_count, uint32_t op2, uint8_t *res_ptr, uint32_t *res_count_ptr);
void qb_do_apply_premultiplication_4x_F32(float32_t *op1_ptr, float32_t *res_ptr);
void qb_do_apply_premultiplication_4x_F64(float64_t *op1_ptr, float64_t *res_ptr);
void qb_do_apply_premultiplication_4x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count);
void qb_do_apply_premultiplication_4x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void qb_do_array_difference_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, uint32_t op3, float32_t *res_ptr, uint32_t res_count);
void qb_do_array_difference_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, uint32_t op3, float64_t *res_ptr, uint32_t res_count);
void qb_do_array_difference_I08(int8_t *op1_ptr, uint32_t op1_count, int8_t *op2_ptr, uint32_t op2_count, uint32_t op3, int8_t *res_ptr, uint32_t res_count);
void qb_do_array_difference_I16(int16_t *op1_ptr, uint32_t op1_count, int16_t *op2_ptr, uint32_t op2_count, uint32_t op3, int16_t *res_ptr, uint32_t res_count);
void qb_do_array_difference_I32(int32_t *op1_ptr, uint32_t op1_count, int32_t *op2_ptr, uint32_t op2_count, uint32_t op3, int32_t *res_ptr, uint32_t res_count);
void qb_do_array_difference_I64(int64_t *op1_ptr, uint32_t op1_count, int64_t *op2_ptr, uint32_t op2_count, uint32_t op3, int64_t *res_ptr, uint32_t res_count);
void qb_do_array_difference_count_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, uint32_t op3, uint32_t *res_ptr);
void qb_do_array_difference_count_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, uint32_t op3, uint32_t *res_ptr);
void qb_do_array_difference_count_I08(int8_t *op1_ptr, uint32_t op1_count, int8_t *op2_ptr, uint32_t op2_count, uint32_t op3, uint32_t *res_ptr);
void qb_do_array_difference_count_I16(int16_t *op1_ptr, uint32_t op1_count, int16_t *op2_ptr, uint32_t op2_count, uint32_t op3, uint32_t *res_ptr);
void qb_do_array_difference_count_I32(int32_t *op1_ptr, uint32_t op1_count, int32_t *op2_ptr, uint32_t op2_count, uint32_t op3, uint32_t *res_ptr);
void qb_do_array_difference_count_I64(int64_t *op1_ptr, uint32_t op1_count, int64_t *op2_ptr, uint32_t op2_count, uint32_t op3, uint32_t *res_ptr);
void qb_do_array_fill_F32(uint32_t op1, float32_t op2, float32_t *res_ptr, uint32_t res_count);
void qb_do_array_fill_F64(uint32_t op1, float64_t op2, float64_t *res_ptr, uint32_t res_count);
void qb_do_array_fill_I08(uint32_t op1, int8_t op2, int8_t *res_ptr, uint32_t res_count);
void qb_do_array_fill_I16(uint32_t op1, int16_t op2, int16_t *res_ptr, uint32_t res_count);
void qb_do_array_fill_I32(uint32_t op1, int32_t op2, int32_t *res_ptr, uint32_t res_count);
void qb_do_array_fill_I64(uint32_t op1, int64_t op2, int64_t *res_ptr, uint32_t res_count);
void qb_do_array_fill_multiple_times_F32(uint32_t op1, float32_t *op2_ptr, uint32_t op2_count, float32_t *res_ptr, uint32_t res_count);
void qb_do_array_fill_multiple_times_F64(uint32_t op1, float64_t *op2_ptr, uint32_t op2_count, float64_t *res_ptr, uint32_t res_count);
void qb_do_array_fill_multiple_times_I08(uint32_t op1, int8_t *op2_ptr, uint32_t op2_count, int8_t *res_ptr, uint32_t res_count);
void qb_do_array_fill_multiple_times_I16(uint32_t op1, int16_t *op2_ptr, uint32_t op2_count, int16_t *res_ptr, uint32_t res_count);
void qb_do_array_fill_multiple_times_I32(uint32_t op1, int32_t *op2_ptr, uint32_t op2_count, int32_t *res_ptr, uint32_t res_count);
void qb_do_array_fill_multiple_times_I64(uint32_t op1, int64_t *op2_ptr, uint32_t op2_count, int64_t *res_ptr, uint32_t res_count);
void qb_do_array_intersect_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, uint32_t op3, float32_t *res_ptr, uint32_t res_count);
void qb_do_array_intersect_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, uint32_t op3, float64_t *res_ptr, uint32_t res_count);
void qb_do_array_intersect_I08(int8_t *op1_ptr, uint32_t op1_count, int8_t *op2_ptr, uint32_t op2_count, uint32_t op3, int8_t *res_ptr, uint32_t res_count);
void qb_do_array_intersect_I16(int16_t *op1_ptr, uint32_t op1_count, int16_t *op2_ptr, uint32_t op2_count, uint32_t op3, int16_t *res_ptr, uint32_t res_count);
void qb_do_array_intersect_I32(int32_t *op1_ptr, uint32_t op1_count, int32_t *op2_ptr, uint32_t op2_count, uint32_t op3, int32_t *res_ptr, uint32_t res_count);
void qb_do_array_intersect_I64(int64_t *op1_ptr, uint32_t op1_count, int64_t *op2_ptr, uint32_t op2_count, uint32_t op3, int64_t *res_ptr, uint32_t res_count);
void qb_do_array_intersect_count_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, uint32_t op3, uint32_t *res_ptr);
void qb_do_array_intersect_count_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, uint32_t op3, uint32_t *res_ptr);
void qb_do_array_intersect_count_I08(int8_t *op1_ptr, uint32_t op1_count, int8_t *op2_ptr, uint32_t op2_count, uint32_t op3, uint32_t *res_ptr);
void qb_do_array_intersect_count_I16(int16_t *op1_ptr, uint32_t op1_count, int16_t *op2_ptr, uint32_t op2_count, uint32_t op3, uint32_t *res_ptr);
void qb_do_array_intersect_count_I32(int32_t *op1_ptr, uint32_t op1_count, int32_t *op2_ptr, uint32_t op2_count, uint32_t op3, uint32_t *res_ptr);
void qb_do_array_intersect_count_I64(int64_t *op1_ptr, uint32_t op1_count, int64_t *op2_ptr, uint32_t op2_count, uint32_t op3, uint32_t *res_ptr);
void qb_do_array_max_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr);
void qb_do_array_max_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr);
void qb_do_array_max_S08(int8_t *op1_ptr, uint32_t op1_count, int8_t *res_ptr);
void qb_do_array_max_S16(int16_t *op1_ptr, uint32_t op1_count, int16_t *res_ptr);
void qb_do_array_max_S32(int32_t *op1_ptr, uint32_t op1_count, int32_t *res_ptr);
void qb_do_array_max_S64(int64_t *op1_ptr, uint32_t op1_count, int64_t *res_ptr);
void qb_do_array_max_U08(uint8_t *op1_ptr, uint32_t op1_count, uint8_t *res_ptr);
void qb_do_array_max_U16(uint16_t *op1_ptr, uint32_t op1_count, uint16_t *res_ptr);
void qb_do_array_max_U32(uint32_t *op1_ptr, uint32_t op1_count, uint32_t *res_ptr);
void qb_do_array_max_U64(uint64_t *op1_ptr, uint32_t op1_count, uint64_t *res_ptr);
void qb_do_array_min_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr);
void qb_do_array_min_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr);
void qb_do_array_min_S08(int8_t *op1_ptr, uint32_t op1_count, int8_t *res_ptr);
void qb_do_array_min_S16(int16_t *op1_ptr, uint32_t op1_count, int16_t *res_ptr);
void qb_do_array_min_S32(int32_t *op1_ptr, uint32_t op1_count, int32_t *res_ptr);
void qb_do_array_min_S64(int64_t *op1_ptr, uint32_t op1_count, int64_t *res_ptr);
void qb_do_array_min_U08(uint8_t *op1_ptr, uint32_t op1_count, uint8_t *res_ptr);
void qb_do_array_min_U16(uint16_t *op1_ptr, uint32_t op1_count, uint16_t *res_ptr);
void qb_do_array_min_U32(uint32_t *op1_ptr, uint32_t op1_count, uint32_t *res_ptr);
void qb_do_array_min_U64(uint64_t *op1_ptr, uint32_t op1_count, uint64_t *res_ptr);
void qb_do_array_pad_F32(float32_t *op1_ptr, uint32_t op1_count, int32_t op2, float32_t *op3_ptr, uint32_t op3_count, float32_t *res_ptr, uint32_t res_count);
void qb_do_array_pad_F64(float64_t *op1_ptr, uint32_t op1_count, int32_t op2, float64_t *op3_ptr, uint32_t op3_count, float64_t *res_ptr, uint32_t res_count);
void qb_do_array_pad_I08(int8_t *op1_ptr, uint32_t op1_count, int32_t op2, int8_t *op3_ptr, uint32_t op3_count, int8_t *res_ptr, uint32_t res_count);
void qb_do_array_pad_I16(int16_t *op1_ptr, uint32_t op1_count, int32_t op2, int16_t *op3_ptr, uint32_t op3_count, int16_t *res_ptr, uint32_t res_count);
void qb_do_array_pad_I32(int32_t *op1_ptr, uint32_t op1_count, int32_t op2, int32_t *op3_ptr, uint32_t op3_count, int32_t *res_ptr, uint32_t res_count);
void qb_do_array_pad_I64(int64_t *op1_ptr, uint32_t op1_count, int32_t op2, int64_t *op3_ptr, uint32_t op3_count, int64_t *res_ptr, uint32_t res_count);
void qb_do_array_product_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr);
void qb_do_array_product_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr);
void qb_do_array_product_S08(int8_t *op1_ptr, uint32_t op1_count, int8_t *res_ptr);
void qb_do_array_product_S16(int16_t *op1_ptr, uint32_t op1_count, int16_t *res_ptr);
void qb_do_array_product_S32(int32_t *op1_ptr, uint32_t op1_count, int32_t *res_ptr);
void qb_do_array_product_S64(int64_t *op1_ptr, uint32_t op1_count, int64_t *res_ptr);
void qb_do_array_product_U08(uint8_t *op1_ptr, uint32_t op1_count, uint8_t *res_ptr);
void qb_do_array_product_U16(uint16_t *op1_ptr, uint32_t op1_count, uint16_t *res_ptr);
void qb_do_array_product_U32(uint32_t *op1_ptr, uint32_t op1_count, uint32_t *res_ptr);
void qb_do_array_product_U64(uint64_t *op1_ptr, uint32_t op1_count, uint64_t *res_ptr);
void qb_do_array_random_U32(qb_interpreter_context *__restrict cxt, uint32_t op1, uint32_t op2, uint32_t *res_ptr);
void qb_do_array_replace_F32(qb_interpreter_context *__restrict cxt, float32_t *op1_ptr, uint32_t op1_count, int32_t op2, int32_t op3, uint32_t *op4_ptr, uint32_t op5, uint32_t op6, float32_t *res_ptr, uint32_t *res_count_ptr);
void qb_do_array_replace_F64(qb_interpreter_context *__restrict cxt, float64_t *op1_ptr, uint32_t op1_count, int32_t op2, int32_t op3, uint32_t *op4_ptr, uint32_t op5, uint32_t op6, float64_t *res_ptr, uint32_t *res_count_ptr);
void qb_do_array_replace_I08(qb_interpreter_context *__restrict cxt, int8_t *op1_ptr, uint32_t op1_count, int32_t op2, int32_t op3, uint32_t *op4_ptr, uint32_t op5, uint32_t op6, int8_t *res_ptr, uint32_t *res_count_ptr);
void qb_do_array_replace_I16(qb_interpreter_context *__restrict cxt, int16_t *op1_ptr, uint32_t op1_count, int32_t op2, int32_t op3, uint32_t *op4_ptr, uint32_t op5, uint32_t op6, int16_t *res_ptr, uint32_t *res_count_ptr);
void qb_do_array_replace_I32(qb_interpreter_context *__restrict cxt, int32_t *op1_ptr, uint32_t op1_count, int32_t op2, int32_t op3, uint32_t *op4_ptr, uint32_t op5, uint32_t op6, int32_t *res_ptr, uint32_t *res_count_ptr);
void qb_do_array_replace_I64(qb_interpreter_context *__restrict cxt, int64_t *op1_ptr, uint32_t op1_count, int32_t op2, int32_t op3, uint32_t *op4_ptr, uint32_t op5, uint32_t op6, int64_t *res_ptr, uint32_t *res_count_ptr);
void qb_do_array_resize_1_F32(qb_interpreter_context *__restrict cxt, uint32_t op1, uint32_t *op2_ptr, uint32_t *op3_ptr, uint32_t op4, float32_t *res_ptr);
void qb_do_array_resize_1_F64(qb_interpreter_context *__restrict cxt, uint32_t op1, uint32_t *op2_ptr, uint32_t *op3_ptr, uint32_t op4, float64_t *res_ptr);
void qb_do_array_resize_1_I08(qb_interpreter_context *__restrict cxt, uint32_t op1, uint32_t *op2_ptr, uint32_t *op3_ptr, uint32_t op4, int8_t *res_ptr);
void qb_do_array_resize_1_I16(qb_interpreter_context *__restrict cxt, uint32_t op1, uint32_t *op2_ptr, uint32_t *op3_ptr, uint32_t op4, int16_t *res_ptr);
void qb_do_array_resize_1_I32(qb_interpreter_context *__restrict cxt, uint32_t op1, uint32_t *op2_ptr, uint32_t *op3_ptr, uint32_t op4, int32_t *res_ptr);
void qb_do_array_resize_1_I64(qb_interpreter_context *__restrict cxt, uint32_t op1, uint32_t *op2_ptr, uint32_t *op3_ptr, uint32_t op4, int64_t *res_ptr);
void qb_do_array_resize_2_F32(qb_interpreter_context *__restrict cxt, uint32_t op1, uint32_t *op2_ptr, uint32_t *op3_ptr, uint32_t op4, uint32_t *op5_ptr, uint32_t *op6_ptr, uint32_t op7, float32_t *res_ptr);
void qb_do_array_resize_2_F64(qb_interpreter_context *__restrict cxt, uint32_t op1, uint32_t *op2_ptr, uint32_t *op3_ptr, uint32_t op4, uint32_t *op5_ptr, uint32_t *op6_ptr, uint32_t op7, float64_t *res_ptr);
void qb_do_array_resize_2_I08(qb_interpreter_context *__restrict cxt, uint32_t op1, uint32_t *op2_ptr, uint32_t *op3_ptr, uint32_t op4, uint32_t *op5_ptr, uint32_t *op6_ptr, uint32_t op7, int8_t *res_ptr);
void qb_do_array_resize_2_I16(qb_interpreter_context *__restrict cxt, uint32_t op1, uint32_t *op2_ptr, uint32_t *op3_ptr, uint32_t op4, uint32_t *op5_ptr, uint32_t *op6_ptr, uint32_t op7, int16_t *res_ptr);
void qb_do_array_resize_2_I32(qb_interpreter_context *__restrict cxt, uint32_t op1, uint32_t *op2_ptr, uint32_t *op3_ptr, uint32_t op4, uint32_t *op5_ptr, uint32_t *op6_ptr, uint32_t op7, int32_t *res_ptr);
void qb_do_array_resize_2_I64(qb_interpreter_context *__restrict cxt, uint32_t op1, uint32_t *op2_ptr, uint32_t *op3_ptr, uint32_t op4, uint32_t *op5_ptr, uint32_t *op6_ptr, uint32_t op7, int64_t *res_ptr);
void qb_do_array_resize_3_F32(qb_interpreter_context *__restrict cxt, uint32_t op1, uint32_t *op2_ptr, uint32_t *op3_ptr, uint32_t op4, uint32_t *op5_ptr, uint32_t *op6_ptr, uint32_t op7, uint32_t *op8_ptr, uint32_t *op9_ptr, uint32_t op10, float32_t *res_ptr);
void qb_do_array_resize_3_F64(qb_interpreter_context *__restrict cxt, uint32_t op1, uint32_t *op2_ptr, uint32_t *op3_ptr, uint32_t op4, uint32_t *op5_ptr, uint32_t *op6_ptr, uint32_t op7, uint32_t *op8_ptr, uint32_t *op9_ptr, uint32_t op10, float64_t *res_ptr);
void qb_do_array_resize_3_I08(qb_interpreter_context *__restrict cxt, uint32_t op1, uint32_t *op2_ptr, uint32_t *op3_ptr, uint32_t op4, uint32_t *op5_ptr, uint32_t *op6_ptr, uint32_t op7, uint32_t *op8_ptr, uint32_t *op9_ptr, uint32_t op10, int8_t *res_ptr);
void qb_do_array_resize_3_I16(qb_interpreter_context *__restrict cxt, uint32_t op1, uint32_t *op2_ptr, uint32_t *op3_ptr, uint32_t op4, uint32_t *op5_ptr, uint32_t *op6_ptr, uint32_t op7, uint32_t *op8_ptr, uint32_t *op9_ptr, uint32_t op10, int16_t *res_ptr);
void qb_do_array_resize_3_I32(qb_interpreter_context *__restrict cxt, uint32_t op1, uint32_t *op2_ptr, uint32_t *op3_ptr, uint32_t op4, uint32_t *op5_ptr, uint32_t *op6_ptr, uint32_t op7, uint32_t *op8_ptr, uint32_t *op9_ptr, uint32_t op10, int32_t *res_ptr);
void qb_do_array_resize_3_I64(qb_interpreter_context *__restrict cxt, uint32_t op1, uint32_t *op2_ptr, uint32_t *op3_ptr, uint32_t op4, uint32_t *op5_ptr, uint32_t *op6_ptr, uint32_t op7, uint32_t *op8_ptr, uint32_t *op9_ptr, uint32_t op10, int64_t *res_ptr);
void qb_do_array_resize_4_F32(qb_interpreter_context *__restrict cxt, uint32_t op1, uint32_t *op2_ptr, uint32_t *op3_ptr, uint32_t op4, uint32_t *op5_ptr, uint32_t *op6_ptr, uint32_t op7, uint32_t *op8_ptr, uint32_t *op9_ptr, uint32_t op10, uint32_t *op11_ptr, uint32_t *op12_ptr, uint32_t op13, float32_t *res_ptr);
void qb_do_array_resize_4_F64(qb_interpreter_context *__restrict cxt, uint32_t op1, uint32_t *op2_ptr, uint32_t *op3_ptr, uint32_t op4, uint32_t *op5_ptr, uint32_t *op6_ptr, uint32_t op7, uint32_t *op8_ptr, uint32_t *op9_ptr, uint32_t op10, uint32_t *op11_ptr, uint32_t *op12_ptr, uint32_t op13, float64_t *res_ptr);
void qb_do_array_resize_4_I08(qb_interpreter_context *__restrict cxt, uint32_t op1, uint32_t *op2_ptr, uint32_t *op3_ptr, uint32_t op4, uint32_t *op5_ptr, uint32_t *op6_ptr, uint32_t op7, uint32_t *op8_ptr, uint32_t *op9_ptr, uint32_t op10, uint32_t *op11_ptr, uint32_t *op12_ptr, uint32_t op13, int8_t *res_ptr);
void qb_do_array_resize_4_I16(qb_interpreter_context *__restrict cxt, uint32_t op1, uint32_t *op2_ptr, uint32_t *op3_ptr, uint32_t op4, uint32_t *op5_ptr, uint32_t *op6_ptr, uint32_t op7, uint32_t *op8_ptr, uint32_t *op9_ptr, uint32_t op10, uint32_t *op11_ptr, uint32_t *op12_ptr, uint32_t op13, int16_t *res_ptr);
void qb_do_array_resize_4_I32(qb_interpreter_context *__restrict cxt, uint32_t op1, uint32_t *op2_ptr, uint32_t *op3_ptr, uint32_t op4, uint32_t *op5_ptr, uint32_t *op6_ptr, uint32_t op7, uint32_t *op8_ptr, uint32_t *op9_ptr, uint32_t op10, uint32_t *op11_ptr, uint32_t *op12_ptr, uint32_t op13, int32_t *res_ptr);
void qb_do_array_resize_4_I64(qb_interpreter_context *__restrict cxt, uint32_t op1, uint32_t *op2_ptr, uint32_t *op3_ptr, uint32_t op4, uint32_t *op5_ptr, uint32_t *op6_ptr, uint32_t op7, uint32_t *op8_ptr, uint32_t *op9_ptr, uint32_t op10, uint32_t *op11_ptr, uint32_t *op12_ptr, uint32_t op13, int64_t *res_ptr);
void qb_do_array_resize_5_F32(qb_interpreter_context *__restrict cxt, uint32_t op1, uint32_t *op2_ptr, uint32_t *op3_ptr, uint32_t op4, uint32_t *op5_ptr, uint32_t *op6_ptr, uint32_t op7, uint32_t *op8_ptr, uint32_t *op9_ptr, uint32_t op10, uint32_t *op11_ptr, uint32_t *op12_ptr, uint32_t op13, uint32_t *op14_ptr, uint32_t *op15_ptr, uint32_t op16, float32_t *res_ptr);
void qb_do_array_resize_5_F64(qb_interpreter_context *__restrict cxt, uint32_t op1, uint32_t *op2_ptr, uint32_t *op3_ptr, uint32_t op4, uint32_t *op5_ptr, uint32_t *op6_ptr, uint32_t op7, uint32_t *op8_ptr, uint32_t *op9_ptr, uint32_t op10, uint32_t *op11_ptr, uint32_t *op12_ptr, uint32_t op13, uint32_t *op14_ptr, uint32_t *op15_ptr, uint32_t op16, float64_t *res_ptr);
void qb_do_array_resize_5_I08(qb_interpreter_context *__restrict cxt, uint32_t op1, uint32_t *op2_ptr, uint32_t *op3_ptr, uint32_t op4, uint32_t *op5_ptr, uint32_t *op6_ptr, uint32_t op7, uint32_t *op8_ptr, uint32_t *op9_ptr, uint32_t op10, uint32_t *op11_ptr, uint32_t *op12_ptr, uint32_t op13, uint32_t *op14_ptr, uint32_t *op15_ptr, uint32_t op16, int8_t *res_ptr);
void qb_do_array_resize_5_I16(qb_interpreter_context *__restrict cxt, uint32_t op1, uint32_t *op2_ptr, uint32_t *op3_ptr, uint32_t op4, uint32_t *op5_ptr, uint32_t *op6_ptr, uint32_t op7, uint32_t *op8_ptr, uint32_t *op9_ptr, uint32_t op10, uint32_t *op11_ptr, uint32_t *op12_ptr, uint32_t op13, uint32_t *op14_ptr, uint32_t *op15_ptr, uint32_t op16, int16_t *res_ptr);
void qb_do_array_resize_5_I32(qb_interpreter_context *__restrict cxt, uint32_t op1, uint32_t *op2_ptr, uint32_t *op3_ptr, uint32_t op4, uint32_t *op5_ptr, uint32_t *op6_ptr, uint32_t op7, uint32_t *op8_ptr, uint32_t *op9_ptr, uint32_t op10, uint32_t *op11_ptr, uint32_t *op12_ptr, uint32_t op13, uint32_t *op14_ptr, uint32_t *op15_ptr, uint32_t op16, int32_t *res_ptr);
void qb_do_array_resize_5_I64(qb_interpreter_context *__restrict cxt, uint32_t op1, uint32_t *op2_ptr, uint32_t *op3_ptr, uint32_t op4, uint32_t *op5_ptr, uint32_t *op6_ptr, uint32_t op7, uint32_t *op8_ptr, uint32_t *op9_ptr, uint32_t op10, uint32_t *op11_ptr, uint32_t *op12_ptr, uint32_t op13, uint32_t *op14_ptr, uint32_t *op15_ptr, uint32_t op16, int64_t *res_ptr);
void qb_do_array_resize_6_F32(qb_interpreter_context *__restrict cxt, uint32_t op1, uint32_t *op2_ptr, uint32_t *op3_ptr, uint32_t op4, uint32_t *op5_ptr, uint32_t *op6_ptr, uint32_t op7, uint32_t *op8_ptr, uint32_t *op9_ptr, uint32_t op10, uint32_t *op11_ptr, uint32_t *op12_ptr, uint32_t op13, uint32_t *op14_ptr, uint32_t *op15_ptr, uint32_t op16, uint32_t *op17_ptr, uint32_t *op18_ptr, uint32_t op19, float32_t *res_ptr);
void qb_do_array_resize_6_F64(qb_interpreter_context *__restrict cxt, uint32_t op1, uint32_t *op2_ptr, uint32_t *op3_ptr, uint32_t op4, uint32_t *op5_ptr, uint32_t *op6_ptr, uint32_t op7, uint32_t *op8_ptr, uint32_t *op9_ptr, uint32_t op10, uint32_t *op11_ptr, uint32_t *op12_ptr, uint32_t op13, uint32_t *op14_ptr, uint32_t *op15_ptr, uint32_t op16, uint32_t *op17_ptr, uint32_t *op18_ptr, uint32_t op19, float64_t *res_ptr);
void qb_do_array_resize_6_I08(qb_interpreter_context *__restrict cxt, uint32_t op1, uint32_t *op2_ptr, uint32_t *op3_ptr, uint32_t op4, uint32_t *op5_ptr, uint32_t *op6_ptr, uint32_t op7, uint32_t *op8_ptr, uint32_t *op9_ptr, uint32_t op10, uint32_t *op11_ptr, uint32_t *op12_ptr, uint32_t op13, uint32_t *op14_ptr, uint32_t *op15_ptr, uint32_t op16, uint32_t *op17_ptr, uint32_t *op18_ptr, uint32_t op19, int8_t *res_ptr);
void qb_do_array_resize_6_I16(qb_interpreter_context *__restrict cxt, uint32_t op1, uint32_t *op2_ptr, uint32_t *op3_ptr, uint32_t op4, uint32_t *op5_ptr, uint32_t *op6_ptr, uint32_t op7, uint32_t *op8_ptr, uint32_t *op9_ptr, uint32_t op10, uint32_t *op11_ptr, uint32_t *op12_ptr, uint32_t op13, uint32_t *op14_ptr, uint32_t *op15_ptr, uint32_t op16, uint32_t *op17_ptr, uint32_t *op18_ptr, uint32_t op19, int16_t *res_ptr);
void qb_do_array_resize_6_I32(qb_interpreter_context *__restrict cxt, uint32_t op1, uint32_t *op2_ptr, uint32_t *op3_ptr, uint32_t op4, uint32_t *op5_ptr, uint32_t *op6_ptr, uint32_t op7, uint32_t *op8_ptr, uint32_t *op9_ptr, uint32_t op10, uint32_t *op11_ptr, uint32_t *op12_ptr, uint32_t op13, uint32_t *op14_ptr, uint32_t *op15_ptr, uint32_t op16, uint32_t *op17_ptr, uint32_t *op18_ptr, uint32_t op19, int32_t *res_ptr);
void qb_do_array_resize_6_I64(qb_interpreter_context *__restrict cxt, uint32_t op1, uint32_t *op2_ptr, uint32_t *op3_ptr, uint32_t op4, uint32_t *op5_ptr, uint32_t *op6_ptr, uint32_t op7, uint32_t *op8_ptr, uint32_t *op9_ptr, uint32_t op10, uint32_t *op11_ptr, uint32_t *op12_ptr, uint32_t op13, uint32_t *op14_ptr, uint32_t *op15_ptr, uint32_t op16, uint32_t *op17_ptr, uint32_t *op18_ptr, uint32_t op19, int64_t *res_ptr);
void qb_do_array_resize_7_F32(qb_interpreter_context *__restrict cxt, uint32_t op1, uint32_t *op2_ptr, uint32_t *op3_ptr, uint32_t op4, uint32_t *op5_ptr, uint32_t *op6_ptr, uint32_t op7, uint32_t *op8_ptr, uint32_t *op9_ptr, uint32_t op10, uint32_t *op11_ptr, uint32_t *op12_ptr, uint32_t op13, uint32_t *op14_ptr, uint32_t *op15_ptr, uint32_t op16, uint32_t *op17_ptr, uint32_t *op18_ptr, uint32_t op19, uint32_t *op20_ptr, uint32_t *op21_ptr, uint32_t op22, float32_t *res_ptr);
void qb_do_array_resize_7_F64(qb_interpreter_context *__restrict cxt, uint32_t op1, uint32_t *op2_ptr, uint32_t *op3_ptr, uint32_t op4, uint32_t *op5_ptr, uint32_t *op6_ptr, uint32_t op7, uint32_t *op8_ptr, uint32_t *op9_ptr, uint32_t op10, uint32_t *op11_ptr, uint32_t *op12_ptr, uint32_t op13, uint32_t *op14_ptr, uint32_t *op15_ptr, uint32_t op16, uint32_t *op17_ptr, uint32_t *op18_ptr, uint32_t op19, uint32_t *op20_ptr, uint32_t *op21_ptr, uint32_t op22, float64_t *res_ptr);
void qb_do_array_resize_7_I08(qb_interpreter_context *__restrict cxt, uint32_t op1, uint32_t *op2_ptr, uint32_t *op3_ptr, uint32_t op4, uint32_t *op5_ptr, uint32_t *op6_ptr, uint32_t op7, uint32_t *op8_ptr, uint32_t *op9_ptr, uint32_t op10, uint32_t *op11_ptr, uint32_t *op12_ptr, uint32_t op13, uint32_t *op14_ptr, uint32_t *op15_ptr, uint32_t op16, uint32_t *op17_ptr, uint32_t *op18_ptr, uint32_t op19, uint32_t *op20_ptr, uint32_t *op21_ptr, uint32_t op22, int8_t *res_ptr);
void qb_do_array_resize_7_I16(qb_interpreter_context *__restrict cxt, uint32_t op1, uint32_t *op2_ptr, uint32_t *op3_ptr, uint32_t op4, uint32_t *op5_ptr, uint32_t *op6_ptr, uint32_t op7, uint32_t *op8_ptr, uint32_t *op9_ptr, uint32_t op10, uint32_t *op11_ptr, uint32_t *op12_ptr, uint32_t op13, uint32_t *op14_ptr, uint32_t *op15_ptr, uint32_t op16, uint32_t *op17_ptr, uint32_t *op18_ptr, uint32_t op19, uint32_t *op20_ptr, uint32_t *op21_ptr, uint32_t op22, int16_t *res_ptr);
void qb_do_array_resize_7_I32(qb_interpreter_context *__restrict cxt, uint32_t op1, uint32_t *op2_ptr, uint32_t *op3_ptr, uint32_t op4, uint32_t *op5_ptr, uint32_t *op6_ptr, uint32_t op7, uint32_t *op8_ptr, uint32_t *op9_ptr, uint32_t op10, uint32_t *op11_ptr, uint32_t *op12_ptr, uint32_t op13, uint32_t *op14_ptr, uint32_t *op15_ptr, uint32_t op16, uint32_t *op17_ptr, uint32_t *op18_ptr, uint32_t op19, uint32_t *op20_ptr, uint32_t *op21_ptr, uint32_t op22, int32_t *res_ptr);
void qb_do_array_resize_7_I64(qb_interpreter_context *__restrict cxt, uint32_t op1, uint32_t *op2_ptr, uint32_t *op3_ptr, uint32_t op4, uint32_t *op5_ptr, uint32_t *op6_ptr, uint32_t op7, uint32_t *op8_ptr, uint32_t *op9_ptr, uint32_t op10, uint32_t *op11_ptr, uint32_t *op12_ptr, uint32_t op13, uint32_t *op14_ptr, uint32_t *op15_ptr, uint32_t op16, uint32_t *op17_ptr, uint32_t *op18_ptr, uint32_t op19, uint32_t *op20_ptr, uint32_t *op21_ptr, uint32_t op22, int64_t *res_ptr);
void qb_do_array_resize_8_F32(qb_interpreter_context *__restrict cxt, uint32_t op1, uint32_t *op2_ptr, uint32_t *op3_ptr, uint32_t op4, uint32_t *op5_ptr, uint32_t *op6_ptr, uint32_t op7, uint32_t *op8_ptr, uint32_t *op9_ptr, uint32_t op10, uint32_t *op11_ptr, uint32_t *op12_ptr, uint32_t op13, uint32_t *op14_ptr, uint32_t *op15_ptr, uint32_t op16, uint32_t *op17_ptr, uint32_t *op18_ptr, uint32_t op19, uint32_t *op20_ptr, uint32_t *op21_ptr, uint32_t op22, uint32_t *op23_ptr, uint32_t *op24_ptr, uint32_t op25, float32_t *res_ptr);
void qb_do_array_resize_8_F64(qb_interpreter_context *__restrict cxt, uint32_t op1, uint32_t *op2_ptr, uint32_t *op3_ptr, uint32_t op4, uint32_t *op5_ptr, uint32_t *op6_ptr, uint32_t op7, uint32_t *op8_ptr, uint32_t *op9_ptr, uint32_t op10, uint32_t *op11_ptr, uint32_t *op12_ptr, uint32_t op13, uint32_t *op14_ptr, uint32_t *op15_ptr, uint32_t op16, uint32_t *op17_ptr, uint32_t *op18_ptr, uint32_t op19, uint32_t *op20_ptr, uint32_t *op21_ptr, uint32_t op22, uint32_t *op23_ptr, uint32_t *op24_ptr, uint32_t op25, float64_t *res_ptr);
void qb_do_array_resize_8_I08(qb_interpreter_context *__restrict cxt, uint32_t op1, uint32_t *op2_ptr, uint32_t *op3_ptr, uint32_t op4, uint32_t *op5_ptr, uint32_t *op6_ptr, uint32_t op7, uint32_t *op8_ptr, uint32_t *op9_ptr, uint32_t op10, uint32_t *op11_ptr, uint32_t *op12_ptr, uint32_t op13, uint32_t *op14_ptr, uint32_t *op15_ptr, uint32_t op16, uint32_t *op17_ptr, uint32_t *op18_ptr, uint32_t op19, uint32_t *op20_ptr, uint32_t *op21_ptr, uint32_t op22, uint32_t *op23_ptr, uint32_t *op24_ptr, uint32_t op25, int8_t *res_ptr);
void qb_do_array_resize_8_I16(qb_interpreter_context *__restrict cxt, uint32_t op1, uint32_t *op2_ptr, uint32_t *op3_ptr, uint32_t op4, uint32_t *op5_ptr, uint32_t *op6_ptr, uint32_t op7, uint32_t *op8_ptr, uint32_t *op9_ptr, uint32_t op10, uint32_t *op11_ptr, uint32_t *op12_ptr, uint32_t op13, uint32_t *op14_ptr, uint32_t *op15_ptr, uint32_t op16, uint32_t *op17_ptr, uint32_t *op18_ptr, uint32_t op19, uint32_t *op20_ptr, uint32_t *op21_ptr, uint32_t op22, uint32_t *op23_ptr, uint32_t *op24_ptr, uint32_t op25, int16_t *res_ptr);
void qb_do_array_resize_8_I32(qb_interpreter_context *__restrict cxt, uint32_t op1, uint32_t *op2_ptr, uint32_t *op3_ptr, uint32_t op4, uint32_t *op5_ptr, uint32_t *op6_ptr, uint32_t op7, uint32_t *op8_ptr, uint32_t *op9_ptr, uint32_t op10, uint32_t *op11_ptr, uint32_t *op12_ptr, uint32_t op13, uint32_t *op14_ptr, uint32_t *op15_ptr, uint32_t op16, uint32_t *op17_ptr, uint32_t *op18_ptr, uint32_t op19, uint32_t *op20_ptr, uint32_t *op21_ptr, uint32_t op22, uint32_t *op23_ptr, uint32_t *op24_ptr, uint32_t op25, int32_t *res_ptr);
void qb_do_array_resize_8_I64(qb_interpreter_context *__restrict cxt, uint32_t op1, uint32_t *op2_ptr, uint32_t *op3_ptr, uint32_t op4, uint32_t *op5_ptr, uint32_t *op6_ptr, uint32_t op7, uint32_t *op8_ptr, uint32_t *op9_ptr, uint32_t op10, uint32_t *op11_ptr, uint32_t *op12_ptr, uint32_t op13, uint32_t *op14_ptr, uint32_t *op15_ptr, uint32_t op16, uint32_t *op17_ptr, uint32_t *op18_ptr, uint32_t op19, uint32_t *op20_ptr, uint32_t *op21_ptr, uint32_t op22, uint32_t *op23_ptr, uint32_t *op24_ptr, uint32_t op25, int64_t *res_ptr);
void qb_do_array_reverse_F32(float32_t *op1_ptr, uint32_t op1_count, uint32_t op2, float32_t *res_ptr, uint32_t res_count);
void qb_do_array_reverse_F64(float64_t *op1_ptr, uint32_t op1_count, uint32_t op2, float64_t *res_ptr, uint32_t res_count);
void qb_do_array_reverse_I08(int8_t *op1_ptr, uint32_t op1_count, uint32_t op2, int8_t *res_ptr, uint32_t res_count);
void qb_do_array_reverse_I16(int16_t *op1_ptr, uint32_t op1_count, uint32_t op2, int16_t *res_ptr, uint32_t res_count);
void qb_do_array_reverse_I32(int32_t *op1_ptr, uint32_t op1_count, uint32_t op2, int32_t *res_ptr, uint32_t res_count);
void qb_do_array_reverse_I64(int64_t *op1_ptr, uint32_t op1_count, uint32_t op2, int64_t *res_ptr, uint32_t res_count);
void qb_do_array_search_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t op2, int32_t *res_ptr);
void qb_do_array_search_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t op2, int32_t *res_ptr);
void qb_do_array_search_I08(int8_t *op1_ptr, uint32_t op1_count, int8_t op2, int32_t *res_ptr);
void qb_do_array_search_I16(int16_t *op1_ptr, uint32_t op1_count, int16_t op2, int32_t *res_ptr);
void qb_do_array_search_I32(int32_t *op1_ptr, uint32_t op1_count, int32_t op2, int32_t *res_ptr);
void qb_do_array_search_I64(int64_t *op1_ptr, uint32_t op1_count, int64_t op2, int32_t *res_ptr);
void qb_do_array_search_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, int32_t *res_ptr);
void qb_do_array_search_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, int32_t *res_ptr);
void qb_do_array_search_multiple_times_I08(int8_t *op1_ptr, uint32_t op1_count, int8_t *op2_ptr, uint32_t op2_count, int32_t *res_ptr);
void qb_do_array_search_multiple_times_I16(int16_t *op1_ptr, uint32_t op1_count, int16_t *op2_ptr, uint32_t op2_count, int32_t *res_ptr);
void qb_do_array_search_multiple_times_I32(int32_t *op1_ptr, uint32_t op1_count, int32_t *op2_ptr, uint32_t op2_count, int32_t *res_ptr);
void qb_do_array_search_multiple_times_I64(int64_t *op1_ptr, uint32_t op1_count, int64_t *op2_ptr, uint32_t op2_count, int32_t *res_ptr);
void qb_do_array_slice_F32(int32_t op1, int32_t op2, uint32_t op3, uint32_t op4, float32_t *op5_ptr, uint32_t op5_count, float32_t *res_ptr, uint32_t res_count);
void qb_do_array_slice_F64(int32_t op1, int32_t op2, uint32_t op3, uint32_t op4, float64_t *op5_ptr, uint32_t op5_count, float64_t *res_ptr, uint32_t res_count);
void qb_do_array_slice_I08(int32_t op1, int32_t op2, uint32_t op3, uint32_t op4, int8_t *op5_ptr, uint32_t op5_count, int8_t *res_ptr, uint32_t res_count);
void qb_do_array_slice_I16(int32_t op1, int32_t op2, uint32_t op3, uint32_t op4, int16_t *op5_ptr, uint32_t op5_count, int16_t *res_ptr, uint32_t res_count);
void qb_do_array_slice_I32(int32_t op1, int32_t op2, uint32_t op3, uint32_t op4, int32_t *op5_ptr, uint32_t op5_count, int32_t *res_ptr, uint32_t res_count);
void qb_do_array_slice_I64(int32_t op1, int32_t op2, uint32_t op3, uint32_t op4, int64_t *op5_ptr, uint32_t op5_count, int64_t *res_ptr, uint32_t res_count);
void qb_do_array_slice_count_I32(int32_t op1, int32_t op2, uint32_t op3, uint32_t *res_ptr);
void qb_do_array_sum_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr);
void qb_do_array_sum_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr);
void qb_do_array_sum_S08(int8_t *op1_ptr, uint32_t op1_count, int8_t *res_ptr);
void qb_do_array_sum_S16(int16_t *op1_ptr, uint32_t op1_count, int16_t *res_ptr);
void qb_do_array_sum_S32(int32_t *op1_ptr, uint32_t op1_count, int32_t *res_ptr);
void qb_do_array_sum_S64(int64_t *op1_ptr, uint32_t op1_count, int64_t *res_ptr);
void qb_do_array_sum_U08(uint8_t *op1_ptr, uint32_t op1_count, uint8_t *res_ptr);
void qb_do_array_sum_U16(uint16_t *op1_ptr, uint32_t op1_count, uint16_t *res_ptr);
void qb_do_array_sum_U32(uint32_t *op1_ptr, uint32_t op1_count, uint32_t *res_ptr);
void qb_do_array_sum_U64(uint64_t *op1_ptr, uint32_t op1_count, uint64_t *res_ptr);
void qb_do_array_unique_F32(float32_t *op1_ptr, uint32_t op1_count, uint32_t op2, float32_t *res_ptr, uint32_t res_count);
void qb_do_array_unique_F64(float64_t *op1_ptr, uint32_t op1_count, uint32_t op2, float64_t *res_ptr, uint32_t res_count);
void qb_do_array_unique_I08(int8_t *op1_ptr, uint32_t op1_count, uint32_t op2, int8_t *res_ptr, uint32_t res_count);
void qb_do_array_unique_I16(int16_t *op1_ptr, uint32_t op1_count, uint32_t op2, int16_t *res_ptr, uint32_t res_count);
void qb_do_array_unique_I32(int32_t *op1_ptr, uint32_t op1_count, uint32_t op2, int32_t *res_ptr, uint32_t res_count);
void qb_do_array_unique_I64(int64_t *op1_ptr, uint32_t op1_count, uint32_t op2, int64_t *res_ptr, uint32_t res_count);
void qb_do_array_unique_count_F32(float32_t *op1_ptr, uint32_t op1_count, uint32_t op2, uint32_t *res_ptr);
void qb_do_array_unique_count_F64(float64_t *op1_ptr, uint32_t op1_count, uint32_t op2, uint32_t *res_ptr);
void qb_do_array_unique_count_I08(int8_t *op1_ptr, uint32_t op1_count, uint32_t op2, uint32_t *res_ptr);
void qb_do_array_unique_count_I16(int16_t *op1_ptr, uint32_t op1_count, uint32_t op2, uint32_t *res_ptr);
void qb_do_array_unique_count_I32(int32_t *op1_ptr, uint32_t op1_count, uint32_t op2, uint32_t *res_ptr);
void qb_do_array_unique_count_I64(int64_t *op1_ptr, uint32_t op1_count, uint32_t op2, uint32_t *res_ptr);
void qb_do_asin_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count);
void qb_do_asin_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void qb_do_asinh_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count);
void qb_do_asinh_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void qb_do_atan2_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, float32_t *res_ptr, uint32_t res_count);
void qb_do_atan2_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, float64_t *res_ptr, uint32_t res_count);
void qb_do_atan_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count);
void qb_do_atan_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void qb_do_atanh_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count);
void qb_do_atanh_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void qb_do_bitwise_and_multiple_times_I08(int8_t *op1_ptr, uint32_t op1_count, int8_t *op2_ptr, uint32_t op2_count, int8_t *res_ptr, uint32_t res_count);
void qb_do_bitwise_and_multiple_times_I16(int16_t *op1_ptr, uint32_t op1_count, int16_t *op2_ptr, uint32_t op2_count, int16_t *res_ptr, uint32_t res_count);
void qb_do_bitwise_and_multiple_times_I32(int32_t *op1_ptr, uint32_t op1_count, int32_t *op2_ptr, uint32_t op2_count, int32_t *res_ptr, uint32_t res_count);
void qb_do_bitwise_and_multiple_times_I64(int64_t *op1_ptr, uint32_t op1_count, int64_t *op2_ptr, uint32_t op2_count, int64_t *res_ptr, uint32_t res_count);
void qb_do_bitwise_not_multiple_times_I08(int8_t *op1_ptr, uint32_t op1_count, int8_t *res_ptr, uint32_t res_count);
void qb_do_bitwise_not_multiple_times_I16(int16_t *op1_ptr, uint32_t op1_count, int16_t *res_ptr, uint32_t res_count);
void qb_do_bitwise_not_multiple_times_I32(int32_t *op1_ptr, uint32_t op1_count, int32_t *res_ptr, uint32_t res_count);
void qb_do_bitwise_not_multiple_times_I64(int64_t *op1_ptr, uint32_t op1_count, int64_t *res_ptr, uint32_t res_count);
void qb_do_bitwise_or_multiple_times_I08(int8_t *op1_ptr, uint32_t op1_count, int8_t *op2_ptr, uint32_t op2_count, int8_t *res_ptr, uint32_t res_count);
void qb_do_bitwise_or_multiple_times_I16(int16_t *op1_ptr, uint32_t op1_count, int16_t *op2_ptr, uint32_t op2_count, int16_t *res_ptr, uint32_t res_count);
void qb_do_bitwise_or_multiple_times_I32(int32_t *op1_ptr, uint32_t op1_count, int32_t *op2_ptr, uint32_t op2_count, int32_t *res_ptr, uint32_t res_count);
void qb_do_bitwise_or_multiple_times_I64(int64_t *op1_ptr, uint32_t op1_count, int64_t *op2_ptr, uint32_t op2_count, int64_t *res_ptr, uint32_t res_count);
void qb_do_bitwise_xor_multiple_times_I08(int8_t *op1_ptr, uint32_t op1_count, int8_t *op2_ptr, uint32_t op2_count, int8_t *res_ptr, uint32_t res_count);
void qb_do_bitwise_xor_multiple_times_I16(int16_t *op1_ptr, uint32_t op1_count, int16_t *op2_ptr, uint32_t op2_count, int16_t *res_ptr, uint32_t res_count);
void qb_do_bitwise_xor_multiple_times_I32(int32_t *op1_ptr, uint32_t op1_count, int32_t *op2_ptr, uint32_t op2_count, int32_t *res_ptr, uint32_t res_count);
void qb_do_bitwise_xor_multiple_times_I64(int64_t *op1_ptr, uint32_t op1_count, int64_t *op2_ptr, uint32_t op2_count, int64_t *res_ptr, uint32_t res_count);
void qb_do_cast_multiple_times_F32_F64(float32_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void qb_do_cast_multiple_times_F32_S08(float32_t *op1_ptr, uint32_t op1_count, int8_t *res_ptr, uint32_t res_count);
void qb_do_cast_multiple_times_F32_S16(float32_t *op1_ptr, uint32_t op1_count, int16_t *res_ptr, uint32_t res_count);
void qb_do_cast_multiple_times_F32_S32(float32_t *op1_ptr, uint32_t op1_count, int32_t *res_ptr, uint32_t res_count);
void qb_do_cast_multiple_times_F32_S64(float32_t *op1_ptr, uint32_t op1_count, int64_t *res_ptr, uint32_t res_count);
void qb_do_cast_multiple_times_F32_U08(float32_t *op1_ptr, uint32_t op1_count, uint8_t *res_ptr, uint32_t res_count);
void qb_do_cast_multiple_times_F32_U16(float32_t *op1_ptr, uint32_t op1_count, uint16_t *res_ptr, uint32_t res_count);
void qb_do_cast_multiple_times_F32_U32(float32_t *op1_ptr, uint32_t op1_count, uint32_t *res_ptr, uint32_t res_count);
void qb_do_cast_multiple_times_F32_U64(float32_t *op1_ptr, uint32_t op1_count, uint64_t *res_ptr, uint32_t res_count);
void qb_do_cast_multiple_times_F64_F32(float64_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count);
void qb_do_cast_multiple_times_F64_S08(float64_t *op1_ptr, uint32_t op1_count, int8_t *res_ptr, uint32_t res_count);
void qb_do_cast_multiple_times_F64_S16(float64_t *op1_ptr, uint32_t op1_count, int16_t *res_ptr, uint32_t res_count);
void qb_do_cast_multiple_times_F64_S32(float64_t *op1_ptr, uint32_t op1_count, int32_t *res_ptr, uint32_t res_count);
void qb_do_cast_multiple_times_F64_S64(float64_t *op1_ptr, uint32_t op1_count, int64_t *res_ptr, uint32_t res_count);
void qb_do_cast_multiple_times_F64_U08(float64_t *op1_ptr, uint32_t op1_count, uint8_t *res_ptr, uint32_t res_count);
void qb_do_cast_multiple_times_F64_U16(float64_t *op1_ptr, uint32_t op1_count, uint16_t *res_ptr, uint32_t res_count);
void qb_do_cast_multiple_times_F64_U32(float64_t *op1_ptr, uint32_t op1_count, uint32_t *res_ptr, uint32_t res_count);
void qb_do_cast_multiple_times_F64_U64(float64_t *op1_ptr, uint32_t op1_count, uint64_t *res_ptr, uint32_t res_count);
void qb_do_cast_multiple_times_I16_I08(int16_t *op1_ptr, uint32_t op1_count, int8_t *res_ptr, uint32_t res_count);
void qb_do_cast_multiple_times_I32_I08(int32_t *op1_ptr, uint32_t op1_count, int8_t *res_ptr, uint32_t res_count);
void qb_do_cast_multiple_times_I32_I16(int32_t *op1_ptr, uint32_t op1_count, int16_t *res_ptr, uint32_t res_count);
void qb_do_cast_multiple_times_I64_I08(int64_t *op1_ptr, uint32_t op1_count, int8_t *res_ptr, uint32_t res_count);
void qb_do_cast_multiple_times_I64_I16(int64_t *op1_ptr, uint32_t op1_count, int16_t *res_ptr, uint32_t res_count);
void qb_do_cast_multiple_times_I64_I32(int64_t *op1_ptr, uint32_t op1_count, int32_t *res_ptr, uint32_t res_count);
void qb_do_cast_multiple_times_S08_F32(int8_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count);
void qb_do_cast_multiple_times_S08_F64(int8_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void qb_do_cast_multiple_times_S08_I16(int8_t *op1_ptr, uint32_t op1_count, int16_t *res_ptr, uint32_t res_count);
void qb_do_cast_multiple_times_S08_I32(int8_t *op1_ptr, uint32_t op1_count, int32_t *res_ptr, uint32_t res_count);
void qb_do_cast_multiple_times_S08_I64(int8_t *op1_ptr, uint32_t op1_count, int64_t *res_ptr, uint32_t res_count);
void qb_do_cast_multiple_times_S16_F32(int16_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count);
void qb_do_cast_multiple_times_S16_F64(int16_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void qb_do_cast_multiple_times_S16_I32(int16_t *op1_ptr, uint32_t op1_count, int32_t *res_ptr, uint32_t res_count);
void qb_do_cast_multiple_times_S16_I64(int16_t *op1_ptr, uint32_t op1_count, int64_t *res_ptr, uint32_t res_count);
void qb_do_cast_multiple_times_S32_F32(int32_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count);
void qb_do_cast_multiple_times_S32_F64(int32_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void qb_do_cast_multiple_times_S32_I64(int32_t *op1_ptr, uint32_t op1_count, int64_t *res_ptr, uint32_t res_count);
void qb_do_cast_multiple_times_S64_F32(int64_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count);
void qb_do_cast_multiple_times_S64_F64(int64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void qb_do_cast_multiple_times_U08_F32(uint8_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count);
void qb_do_cast_multiple_times_U08_F64(uint8_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void qb_do_cast_multiple_times_U08_I16(uint8_t *op1_ptr, uint32_t op1_count, int16_t *res_ptr, uint32_t res_count);
void qb_do_cast_multiple_times_U08_I32(uint8_t *op1_ptr, uint32_t op1_count, int32_t *res_ptr, uint32_t res_count);
void qb_do_cast_multiple_times_U08_I64(uint8_t *op1_ptr, uint32_t op1_count, int64_t *res_ptr, uint32_t res_count);
void qb_do_cast_multiple_times_U16_F32(uint16_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count);
void qb_do_cast_multiple_times_U16_F64(uint16_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void qb_do_cast_multiple_times_U16_I32(uint16_t *op1_ptr, uint32_t op1_count, int32_t *res_ptr, uint32_t res_count);
void qb_do_cast_multiple_times_U16_I64(uint16_t *op1_ptr, uint32_t op1_count, int64_t *res_ptr, uint32_t res_count);
void qb_do_cast_multiple_times_U32_F32(uint32_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count);
void qb_do_cast_multiple_times_U32_F64(uint32_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void qb_do_cast_multiple_times_U32_I64(uint32_t *op1_ptr, uint32_t op1_count, int64_t *res_ptr, uint32_t res_count);
void qb_do_cast_multiple_times_U64_F32(uint64_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count);
void qb_do_cast_multiple_times_U64_F64(uint64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void qb_do_ceil_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count);
void qb_do_ceil_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void qb_do_choose_size_largest_of_three_U32(uint32_t op1, uint32_t op2, uint32_t op3, uint32_t op4, uint32_t op5, uint32_t op6, uint32_t *res_ptr);
void qb_do_choose_size_largest_of_three_top_level_U32(uint32_t op1, uint32_t op2, uint32_t op3, uint32_t *res_ptr);
void qb_do_clamp_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, float32_t *op3_ptr, uint32_t op3_count, float32_t *res_ptr, uint32_t res_count);
void qb_do_clamp_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, float64_t *op3_ptr, uint32_t op3_count, float64_t *res_ptr, uint32_t res_count);
void qb_do_clear_array_resize_F32(qb_interpreter_context *__restrict cxt, uint32_t op1, float32_t *res_ptr, uint32_t *res_count_ptr);
void qb_do_clear_array_resize_F64(qb_interpreter_context *__restrict cxt, uint32_t op1, float64_t *res_ptr, uint32_t *res_count_ptr);
void qb_do_clear_array_resize_I08(qb_interpreter_context *__restrict cxt, uint32_t op1, int8_t *res_ptr, uint32_t *res_count_ptr);
void qb_do_clear_array_resize_I16(qb_interpreter_context *__restrict cxt, uint32_t op1, int16_t *res_ptr, uint32_t *res_count_ptr);
void qb_do_clear_array_resize_I32(qb_interpreter_context *__restrict cxt, uint32_t op1, int32_t *res_ptr, uint32_t *res_count_ptr);
void qb_do_clear_array_resize_I64(qb_interpreter_context *__restrict cxt, uint32_t op1, int64_t *res_ptr, uint32_t *res_count_ptr);
void qb_do_clear_array_resize_update_dimension_F32(qb_interpreter_context *__restrict cxt, uint32_t *op1_ptr, uint32_t op2, float32_t *res_ptr, uint32_t *res_count_ptr);
void qb_do_clear_array_resize_update_dimension_F64(qb_interpreter_context *__restrict cxt, uint32_t *op1_ptr, uint32_t op2, float64_t *res_ptr, uint32_t *res_count_ptr);
void qb_do_clear_array_resize_update_dimension_I08(qb_interpreter_context *__restrict cxt, uint32_t *op1_ptr, uint32_t op2, int8_t *res_ptr, uint32_t *res_count_ptr);
void qb_do_clear_array_resize_update_dimension_I16(qb_interpreter_context *__restrict cxt, uint32_t *op1_ptr, uint32_t op2, int16_t *res_ptr, uint32_t *res_count_ptr);
void qb_do_clear_array_resize_update_dimension_I32(qb_interpreter_context *__restrict cxt, uint32_t *op1_ptr, uint32_t op2, int32_t *res_ptr, uint32_t *res_count_ptr);
void qb_do_clear_array_resize_update_dimension_I64(qb_interpreter_context *__restrict cxt, uint32_t *op1_ptr, uint32_t op2, int64_t *res_ptr, uint32_t *res_count_ptr);
void qb_do_clear_element_resize_F32(qb_interpreter_context *__restrict cxt, uint32_t op1, uint32_t op2, uint32_t op3, float32_t *res_ptr, uint32_t *res_count_ptr);
void qb_do_clear_element_resize_F64(qb_interpreter_context *__restrict cxt, uint32_t op1, uint32_t op2, uint32_t op3, float64_t *res_ptr, uint32_t *res_count_ptr);
void qb_do_clear_element_resize_I08(qb_interpreter_context *__restrict cxt, uint32_t op1, uint32_t op2, uint32_t op3, int8_t *res_ptr, uint32_t *res_count_ptr);
void qb_do_clear_element_resize_I16(qb_interpreter_context *__restrict cxt, uint32_t op1, uint32_t op2, uint32_t op3, int16_t *res_ptr, uint32_t *res_count_ptr);
void qb_do_clear_element_resize_I32(qb_interpreter_context *__restrict cxt, uint32_t op1, uint32_t op2, uint32_t op3, int32_t *res_ptr, uint32_t *res_count_ptr);
void qb_do_clear_element_resize_I64(qb_interpreter_context *__restrict cxt, uint32_t op1, uint32_t op2, uint32_t op3, int64_t *res_ptr, uint32_t *res_count_ptr);
void qb_do_clear_element_resize_update_dimension_F32(qb_interpreter_context *__restrict cxt, uint32_t op1, uint32_t op2, uint32_t *op3_ptr, uint32_t op4, float32_t *res_ptr, uint32_t *res_count_ptr);
void qb_do_clear_element_resize_update_dimension_F64(qb_interpreter_context *__restrict cxt, uint32_t op1, uint32_t op2, uint32_t *op3_ptr, uint32_t op4, float64_t *res_ptr, uint32_t *res_count_ptr);
void qb_do_clear_element_resize_update_dimension_I08(qb_interpreter_context *__restrict cxt, uint32_t op1, uint32_t op2, uint32_t *op3_ptr, uint32_t op4, int8_t *res_ptr, uint32_t *res_count_ptr);
void qb_do_clear_element_resize_update_dimension_I16(qb_interpreter_context *__restrict cxt, uint32_t op1, uint32_t op2, uint32_t *op3_ptr, uint32_t op4, int16_t *res_ptr, uint32_t *res_count_ptr);
void qb_do_clear_element_resize_update_dimension_I32(qb_interpreter_context *__restrict cxt, uint32_t op1, uint32_t op2, uint32_t *op3_ptr, uint32_t op4, int32_t *res_ptr, uint32_t *res_count_ptr);
void qb_do_clear_element_resize_update_dimension_I64(qb_interpreter_context *__restrict cxt, uint32_t op1, uint32_t op2, uint32_t *op3_ptr, uint32_t op4, int64_t *res_ptr, uint32_t *res_count_ptr);
void qb_do_complex_abs_2x_F32(float32_t *op1_ptr, float32_t *res_ptr);
void qb_do_complex_abs_2x_F64(float64_t *op1_ptr, float64_t *res_ptr);
void qb_do_complex_abs_2x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count);
void qb_do_complex_abs_2x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void qb_do_complex_argument_2x_F32(float32_t *op1_ptr, float32_t *res_ptr);
void qb_do_complex_argument_2x_F64(float64_t *op1_ptr, float64_t *res_ptr);
void qb_do_complex_argument_2x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count);
void qb_do_complex_argument_2x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void qb_do_complex_cos_2x_F32(float32_t *op1_ptr, float32_t *res_ptr);
void qb_do_complex_cos_2x_F64(float64_t *op1_ptr, float64_t *res_ptr);
void qb_do_complex_cos_2x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count);
void qb_do_complex_cos_2x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void qb_do_complex_cosh_2x_F32(float32_t *op1_ptr, float32_t *res_ptr);
void qb_do_complex_cosh_2x_F64(float64_t *op1_ptr, float64_t *res_ptr);
void qb_do_complex_cosh_2x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count);
void qb_do_complex_cosh_2x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void qb_do_complex_divide_2x_F32(float32_t *op1_ptr, float32_t *op2_ptr, float32_t *res_ptr);
void qb_do_complex_divide_2x_F64(float64_t *op1_ptr, float64_t *op2_ptr, float64_t *res_ptr);
void qb_do_complex_divide_2x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, float32_t *res_ptr, uint32_t res_count);
void qb_do_complex_divide_2x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, float64_t *res_ptr, uint32_t res_count);
void qb_do_complex_exp_2x_F32(float32_t *op1_ptr, float32_t *res_ptr);
void qb_do_complex_exp_2x_F64(float64_t *op1_ptr, float64_t *res_ptr);
void qb_do_complex_exp_2x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count);
void qb_do_complex_exp_2x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void qb_do_complex_log_2x_F32(float32_t *op1_ptr, float32_t *res_ptr);
void qb_do_complex_log_2x_F64(float64_t *op1_ptr, float64_t *res_ptr);
void qb_do_complex_log_2x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count);
void qb_do_complex_log_2x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void qb_do_complex_multiply_2x_F32(float32_t *op1_ptr, float32_t *op2_ptr, float32_t *res_ptr);
void qb_do_complex_multiply_2x_F64(float64_t *op1_ptr, float64_t *op2_ptr, float64_t *res_ptr);
void qb_do_complex_multiply_2x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, float32_t *res_ptr, uint32_t res_count);
void qb_do_complex_multiply_2x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, float64_t *res_ptr, uint32_t res_count);
void qb_do_complex_pow_2x_F32(float32_t *op1_ptr, float32_t *op2_ptr, float32_t *res_ptr);
void qb_do_complex_pow_2x_F64(float64_t *op1_ptr, float64_t *op2_ptr, float64_t *res_ptr);
void qb_do_complex_pow_2x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, float32_t *res_ptr, uint32_t res_count);
void qb_do_complex_pow_2x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, float64_t *res_ptr, uint32_t res_count);
void qb_do_complex_sin_2x_F32(float32_t *op1_ptr, float32_t *res_ptr);
void qb_do_complex_sin_2x_F64(float64_t *op1_ptr, float64_t *res_ptr);
void qb_do_complex_sin_2x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count);
void qb_do_complex_sin_2x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void qb_do_complex_sinh_2x_F32(float32_t *op1_ptr, float32_t *res_ptr);
void qb_do_complex_sinh_2x_F64(float64_t *op1_ptr, float64_t *res_ptr);
void qb_do_complex_sinh_2x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count);
void qb_do_complex_sinh_2x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void qb_do_complex_square_root_2x_F32(float32_t *op1_ptr, float32_t *res_ptr);
void qb_do_complex_square_root_2x_F64(float64_t *op1_ptr, float64_t *res_ptr);
void qb_do_complex_square_root_2x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count);
void qb_do_complex_square_root_2x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void qb_do_complex_tan_2x_F32(float32_t *op1_ptr, float32_t *res_ptr);
void qb_do_complex_tan_2x_F64(float64_t *op1_ptr, float64_t *res_ptr);
void qb_do_complex_tan_2x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count);
void qb_do_complex_tan_2x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void qb_do_complex_tanh_2x_F32(float32_t *op1_ptr, float32_t *res_ptr);
void qb_do_complex_tanh_2x_F64(float64_t *op1_ptr, float64_t *res_ptr);
void qb_do_complex_tanh_2x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count);
void qb_do_complex_tanh_2x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void qb_do_convert_from_string_F32(uint8_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr);
void qb_do_convert_from_string_F64(uint8_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr);
void qb_do_convert_from_string_S08(uint8_t *op1_ptr, uint32_t op1_count, int8_t *res_ptr);
void qb_do_convert_from_string_S16(uint8_t *op1_ptr, uint32_t op1_count, int16_t *res_ptr);
void qb_do_convert_from_string_S32(uint8_t *op1_ptr, uint32_t op1_count, int32_t *res_ptr);
void qb_do_convert_from_string_S64(uint8_t *op1_ptr, uint32_t op1_count, int64_t *res_ptr);
void qb_do_convert_from_string_U08(uint8_t *op1_ptr, uint32_t op1_count, uint8_t *res_ptr);
void qb_do_convert_from_string_U16(uint8_t *op1_ptr, uint32_t op1_count, uint16_t *res_ptr);
void qb_do_convert_from_string_U32(uint8_t *op1_ptr, uint32_t op1_count, uint32_t *res_ptr);
void qb_do_convert_from_string_U64(uint8_t *op1_ptr, uint32_t op1_count, uint64_t *res_ptr);
void qb_do_copy_2x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count);
void qb_do_copy_2x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void qb_do_copy_3x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count);
void qb_do_copy_3x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void qb_do_copy_4x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count);
void qb_do_copy_4x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void qb_do_copy_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count);
void qb_do_copy_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void qb_do_copy_multiple_times_I08(int8_t *op1_ptr, uint32_t op1_count, int8_t *res_ptr, uint32_t res_count);
void qb_do_copy_multiple_times_I16(int16_t *op1_ptr, uint32_t op1_count, int16_t *res_ptr, uint32_t res_count);
void qb_do_copy_multiple_times_I32(int32_t *op1_ptr, uint32_t op1_count, int32_t *res_ptr, uint32_t res_count);
void qb_do_copy_multiple_times_I64(int64_t *op1_ptr, uint32_t op1_count, int64_t *res_ptr, uint32_t res_count);
void qb_do_cos_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count);
void qb_do_cos_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void qb_do_cosh_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count);
void qb_do_cosh_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void qb_do_cross_product_2x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, float32_t *res_ptr, uint32_t res_count);
void qb_do_cross_product_2x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, float64_t *res_ptr, uint32_t res_count);
void qb_do_cross_product_3x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, float32_t *res_ptr, uint32_t res_count);
void qb_do_cross_product_3x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, float64_t *res_ptr, uint32_t res_count);
void qb_do_cross_product_4x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, float32_t *op3_ptr, uint32_t op3_count, float32_t *res_ptr, uint32_t res_count);
void qb_do_cross_product_4x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, float64_t *op3_ptr, uint32_t op3_count, float64_t *res_ptr, uint32_t res_count);
void qb_do_decrement_2x_multiple_times_F32(float32_t *res_ptr, uint32_t res_count);
void qb_do_decrement_2x_multiple_times_F64(float64_t *res_ptr, uint32_t res_count);
void qb_do_decrement_3x_multiple_times_F32(float32_t *res_ptr, uint32_t res_count);
void qb_do_decrement_3x_multiple_times_F64(float64_t *res_ptr, uint32_t res_count);
void qb_do_decrement_4x_multiple_times_F32(float32_t *res_ptr, uint32_t res_count);
void qb_do_decrement_4x_multiple_times_F64(float64_t *res_ptr, uint32_t res_count);
void qb_do_decrement_multiple_times_F32(float32_t *res_ptr, uint32_t res_count);
void qb_do_decrement_multiple_times_F64(float64_t *res_ptr, uint32_t res_count);
void qb_do_decrement_multiple_times_I08(int8_t *res_ptr, uint32_t res_count);
void qb_do_decrement_multiple_times_I16(int16_t *res_ptr, uint32_t res_count);
void qb_do_decrement_multiple_times_I32(int32_t *res_ptr, uint32_t res_count);
void qb_do_decrement_multiple_times_I64(int64_t *res_ptr, uint32_t res_count);
void qb_do_degree_to_radian_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count);
void qb_do_degree_to_radian_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void qb_do_determinant_2x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count);
void qb_do_determinant_2x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void qb_do_determinant_3x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count);
void qb_do_determinant_3x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void qb_do_determinant_4x_F32(float32_t *op1_ptr, float32_t *res_ptr);
void qb_do_determinant_4x_F64(float64_t *op1_ptr, float64_t *res_ptr);
void qb_do_determinant_4x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count);
void qb_do_determinant_4x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void qb_do_determinant_F32(float32_t *op1_ptr, uint32_t op1_count, uint32_t op2, float32_t *res_ptr);
void qb_do_determinant_F64(float64_t *op1_ptr, uint32_t op1_count, uint32_t op2, float64_t *res_ptr);
void qb_do_determinant_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, uint32_t op2, float32_t *res_ptr, uint32_t res_count);
void qb_do_determinant_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, uint32_t op2, float64_t *res_ptr, uint32_t res_count);
void qb_do_distance_2x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, float32_t *res_ptr, uint32_t res_count);
void qb_do_distance_2x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, float64_t *res_ptr, uint32_t res_count);
void qb_do_distance_3x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, float32_t *res_ptr, uint32_t res_count);
void qb_do_distance_3x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, float64_t *res_ptr, uint32_t res_count);
void qb_do_distance_4x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, float32_t *res_ptr, uint32_t res_count);
void qb_do_distance_4x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, float64_t *res_ptr, uint32_t res_count);
void qb_do_distance_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, uint32_t op3, float32_t *res_ptr);
void qb_do_distance_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, uint32_t op3, float64_t *res_ptr);
void qb_do_distance_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, uint32_t op3, float32_t *res_ptr, uint32_t res_count);
void qb_do_distance_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, uint32_t op3, float64_t *res_ptr, uint32_t res_count);
void qb_do_divide_2x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, float32_t *res_ptr, uint32_t res_count);
void qb_do_divide_2x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, float64_t *res_ptr, uint32_t res_count);
void qb_do_divide_3x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, float32_t *res_ptr, uint32_t res_count);
void qb_do_divide_3x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, float64_t *res_ptr, uint32_t res_count);
void qb_do_divide_4x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, float32_t *res_ptr, uint32_t res_count);
void qb_do_divide_4x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, float64_t *res_ptr, uint32_t res_count);
void qb_do_divide_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, float32_t *res_ptr, uint32_t res_count);
void qb_do_divide_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, float64_t *res_ptr, uint32_t res_count);
void qb_do_dot_product_2x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, float32_t *res_ptr, uint32_t res_count);
void qb_do_dot_product_2x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, float64_t *res_ptr, uint32_t res_count);
void qb_do_dot_product_3x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, float32_t *res_ptr, uint32_t res_count);
void qb_do_dot_product_3x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, float64_t *res_ptr, uint32_t res_count);
void qb_do_dot_product_4x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, float32_t *res_ptr, uint32_t res_count);
void qb_do_dot_product_4x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, float64_t *res_ptr, uint32_t res_count);
void qb_do_dot_product_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, uint32_t op3, float32_t *res_ptr);
void qb_do_dot_product_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, uint32_t op3, float64_t *res_ptr);
void qb_do_dot_product_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, uint32_t op3, float32_t *res_ptr, uint32_t res_count);
void qb_do_dot_product_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, uint32_t op3, float64_t *res_ptr, uint32_t res_count);
void qb_do_equal_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, int32_t *res_ptr);
void qb_do_equal_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, int32_t *res_ptr);
void qb_do_equal_multiple_times_I08(int8_t *op1_ptr, uint32_t op1_count, int8_t *op2_ptr, uint32_t op2_count, int32_t *res_ptr);
void qb_do_equal_multiple_times_I16(int16_t *op1_ptr, uint32_t op1_count, int16_t *op2_ptr, uint32_t op2_count, int32_t *res_ptr);
void qb_do_equal_multiple_times_I32(int32_t *op1_ptr, uint32_t op1_count, int32_t *op2_ptr, uint32_t op2_count, int32_t *res_ptr);
void qb_do_equal_multiple_times_I64(int64_t *op1_ptr, uint32_t op1_count, int64_t *op2_ptr, uint32_t op2_count, int32_t *res_ptr);
void qb_do_exp2_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count);
void qb_do_exp2_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void qb_do_exp_m1_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count);
void qb_do_exp_m1_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void qb_do_exp_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count);
void qb_do_exp_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void qb_do_extension_op_U32(qb_interpreter_context *__restrict cxt, uint32_t op1, uint32_t line_id);
void qb_do_face_forward_2x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, float32_t *res_ptr, uint32_t res_count);
void qb_do_face_forward_2x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, float64_t *res_ptr, uint32_t res_count);
void qb_do_face_forward_3x_F32(float32_t *op1_ptr, float32_t *op2_ptr, float32_t *res_ptr);
void qb_do_face_forward_3x_F64(float64_t *op1_ptr, float64_t *op2_ptr, float64_t *res_ptr);
void qb_do_face_forward_3x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, float32_t *res_ptr, uint32_t res_count);
void qb_do_face_forward_3x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, float64_t *res_ptr, uint32_t res_count);
void qb_do_face_forward_4x_F32(float32_t *op1_ptr, float32_t *op2_ptr, float32_t *res_ptr);
void qb_do_face_forward_4x_F64(float64_t *op1_ptr, float64_t *op2_ptr, float64_t *res_ptr);
void qb_do_face_forward_4x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, float32_t *res_ptr, uint32_t res_count);
void qb_do_face_forward_4x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, float64_t *res_ptr, uint32_t res_count);
void qb_do_face_forward_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, uint32_t op3, float32_t *res_ptr, uint32_t res_count);
void qb_do_face_forward_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, uint32_t op3, float64_t *res_ptr, uint32_t res_count);
void qb_do_face_forward_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, uint32_t op3, float32_t *res_ptr, uint32_t res_count);
void qb_do_face_forward_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, uint32_t op3, float64_t *res_ptr, uint32_t res_count);
void qb_do_floor_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count);
void qb_do_floor_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void qb_do_floored_division_modulo_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, float32_t *res_ptr, uint32_t res_count);
void qb_do_floored_division_modulo_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, float64_t *res_ptr, uint32_t res_count);
void qb_do_fract_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count);
void qb_do_fract_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void qb_do_gather_4x_F32(uint32_t op1, float32_t *op2_ptr, float32_t *res_ptr);
void qb_do_gather_4x_F64(uint32_t op1, float64_t *op2_ptr, float64_t *res_ptr);
void qb_do_gather_4x_I08(uint32_t op1, int8_t *op2_ptr, int8_t *res_ptr);
void qb_do_gather_4x_I16(uint32_t op1, int16_t *op2_ptr, int16_t *res_ptr);
void qb_do_gather_4x_I32(uint32_t op1, int32_t *op2_ptr, int32_t *res_ptr);
void qb_do_gather_4x_I64(uint32_t op1, int64_t *op2_ptr, int64_t *res_ptr);
void qb_do_gather_5x_F32(uint32_t op1, float32_t *op2_ptr, float32_t *res_ptr);
void qb_do_gather_5x_F64(uint32_t op1, float64_t *op2_ptr, float64_t *res_ptr);
void qb_do_gather_5x_I08(uint32_t op1, int8_t *op2_ptr, int8_t *res_ptr);
void qb_do_gather_5x_I16(uint32_t op1, int16_t *op2_ptr, int16_t *res_ptr);
void qb_do_gather_5x_I32(uint32_t op1, int32_t *op2_ptr, int32_t *res_ptr);
void qb_do_gather_5x_I64(uint32_t op1, int64_t *op2_ptr, int64_t *res_ptr);
void qb_do_gather_6x_F32(uint32_t op1, float32_t *op2_ptr, float32_t *res_ptr);
void qb_do_gather_6x_F64(uint32_t op1, float64_t *op2_ptr, float64_t *res_ptr);
void qb_do_gather_6x_I08(uint32_t op1, int8_t *op2_ptr, int8_t *res_ptr);
void qb_do_gather_6x_I16(uint32_t op1, int16_t *op2_ptr, int16_t *res_ptr);
void qb_do_gather_6x_I32(uint32_t op1, int32_t *op2_ptr, int32_t *res_ptr);
void qb_do_gather_6x_I64(uint32_t op1, int64_t *op2_ptr, int64_t *res_ptr);
void qb_do_gather_7x_F32(uint32_t op1, float32_t *op2_ptr, float32_t *res_ptr);
void qb_do_gather_7x_F64(uint32_t op1, float64_t *op2_ptr, float64_t *res_ptr);
void qb_do_gather_7x_I08(uint32_t op1, int8_t *op2_ptr, int8_t *res_ptr);
void qb_do_gather_7x_I16(uint32_t op1, int16_t *op2_ptr, int16_t *res_ptr);
void qb_do_gather_7x_I32(uint32_t op1, int32_t *op2_ptr, int32_t *res_ptr);
void qb_do_gather_7x_I64(uint32_t op1, int64_t *op2_ptr, int64_t *res_ptr);
void qb_do_gather_8x_F32(uint32_t op1, float32_t *op2_ptr, float32_t *res_ptr);
void qb_do_gather_8x_F64(uint32_t op1, float64_t *op2_ptr, float64_t *res_ptr);
void qb_do_gather_8x_I08(uint32_t op1, int8_t *op2_ptr, int8_t *res_ptr);
void qb_do_gather_8x_I16(uint32_t op1, int16_t *op2_ptr, int16_t *res_ptr);
void qb_do_gather_8x_I32(uint32_t op1, int32_t *op2_ptr, int32_t *res_ptr);
void qb_do_gather_8x_I64(uint32_t op1, int64_t *op2_ptr, int64_t *res_ptr);
void qb_do_get_time_F64(float64_t *res_ptr);
void qb_do_hsl2rgb_3x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count);
void qb_do_hsl2rgb_3x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void qb_do_hsl2rgb_4x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count);
void qb_do_hsl2rgb_4x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void qb_do_hsv2rgb_3x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count);
void qb_do_hsv2rgb_3x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void qb_do_hsv2rgb_4x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count);
void qb_do_hsv2rgb_4x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void qb_do_hypot_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, float32_t *res_ptr, uint32_t res_count);
void qb_do_hypot_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, float64_t *res_ptr, uint32_t res_count);
void qb_do_increment_2x_multiple_times_F32(float32_t *res_ptr, uint32_t res_count);
void qb_do_increment_2x_multiple_times_F64(float64_t *res_ptr, uint32_t res_count);
void qb_do_increment_3x_multiple_times_F32(float32_t *res_ptr, uint32_t res_count);
void qb_do_increment_3x_multiple_times_F64(float64_t *res_ptr, uint32_t res_count);
void qb_do_increment_4x_multiple_times_F32(float32_t *res_ptr, uint32_t res_count);
void qb_do_increment_4x_multiple_times_F64(float64_t *res_ptr, uint32_t res_count);
void qb_do_increment_multiple_times_F32(float32_t *res_ptr, uint32_t res_count);
void qb_do_increment_multiple_times_F64(float64_t *res_ptr, uint32_t res_count);
void qb_do_increment_multiple_times_I08(int8_t *res_ptr, uint32_t res_count);
void qb_do_increment_multiple_times_I16(int16_t *res_ptr, uint32_t res_count);
void qb_do_increment_multiple_times_I32(int32_t *res_ptr, uint32_t res_count);
void qb_do_increment_multiple_times_I64(int64_t *res_ptr, uint32_t res_count);
void qb_do_invert_matrix_2x_F32(float32_t *op1_ptr, float32_t *res_ptr);
void qb_do_invert_matrix_2x_F64(float64_t *op1_ptr, float64_t *res_ptr);
void qb_do_invert_matrix_2x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count);
void qb_do_invert_matrix_2x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void qb_do_invert_matrix_3x_F32(float32_t *op1_ptr, float32_t *res_ptr);
void qb_do_invert_matrix_3x_F64(float64_t *op1_ptr, float64_t *res_ptr);
void qb_do_invert_matrix_3x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count);
void qb_do_invert_matrix_3x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void qb_do_invert_matrix_4x_F32(float32_t *op1_ptr, float32_t *res_ptr);
void qb_do_invert_matrix_4x_F64(float64_t *op1_ptr, float64_t *res_ptr);
void qb_do_invert_matrix_4x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count);
void qb_do_invert_matrix_4x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void qb_do_invert_matrix_F32(float32_t *op1_ptr, uint32_t op1_count, uint32_t op2, float32_t *res_ptr, uint32_t res_count);
void qb_do_invert_matrix_F64(float64_t *op1_ptr, uint32_t op1_count, uint32_t op2, float64_t *res_ptr, uint32_t res_count);
void qb_do_invert_matrix_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, uint32_t op2, float32_t *res_ptr, uint32_t res_count);
void qb_do_invert_matrix_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, uint32_t op2, float64_t *res_ptr, uint32_t res_count);
void qb_do_is_finite_F32(float32_t op1, int32_t *res_ptr);
void qb_do_is_finite_F64(float64_t op1, int32_t *res_ptr);
void qb_do_is_finite_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, int32_t *res_ptr, uint32_t res_count);
void qb_do_is_finite_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, int32_t *res_ptr, uint32_t res_count);
void qb_do_is_infinite_F32(float32_t op1, int32_t *res_ptr);
void qb_do_is_infinite_F64(float64_t op1, int32_t *res_ptr);
void qb_do_is_infinite_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, int32_t *res_ptr, uint32_t res_count);
void qb_do_is_infinite_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, int32_t *res_ptr, uint32_t res_count);
void qb_do_is_na_n_F32(float32_t op1, int32_t *res_ptr);
void qb_do_is_na_n_F64(float64_t op1, int32_t *res_ptr);
void qb_do_is_na_n_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, int32_t *res_ptr, uint32_t res_count);
void qb_do_is_na_n_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, int32_t *res_ptr, uint32_t res_count);
void qb_do_lcg_F32(qb_interpreter_context *__restrict cxt, float32_t *res_ptr);
void qb_do_lcg_F64(qb_interpreter_context *__restrict cxt, float64_t *res_ptr);
void qb_do_lcg_multiple_times_F32(qb_interpreter_context *__restrict cxt, float32_t *res_ptr, uint32_t res_count);
void qb_do_lcg_multiple_times_F64(qb_interpreter_context *__restrict cxt, float64_t *res_ptr, uint32_t res_count);
void qb_do_length_2x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count);
void qb_do_length_2x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void qb_do_length_3x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count);
void qb_do_length_3x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void qb_do_length_4x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count);
void qb_do_length_4x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void qb_do_length_F32(float32_t *op1_ptr, uint32_t op1_count, uint32_t op2, float32_t *res_ptr);
void qb_do_length_F64(float64_t *op1_ptr, uint32_t op1_count, uint32_t op2, float64_t *res_ptr);
void qb_do_length_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, uint32_t op2, float32_t *res_ptr, uint32_t res_count);
void qb_do_length_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, uint32_t op2, float64_t *res_ptr, uint32_t res_count);
void qb_do_less_than_equal_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, int32_t *res_ptr);
void qb_do_less_than_equal_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, int32_t *res_ptr);
void qb_do_less_than_equal_multiple_times_S08(int8_t *op1_ptr, uint32_t op1_count, int8_t *op2_ptr, uint32_t op2_count, int32_t *res_ptr);
void qb_do_less_than_equal_multiple_times_S16(int16_t *op1_ptr, uint32_t op1_count, int16_t *op2_ptr, uint32_t op2_count, int32_t *res_ptr);
void qb_do_less_than_equal_multiple_times_S32(int32_t *op1_ptr, uint32_t op1_count, int32_t *op2_ptr, uint32_t op2_count, int32_t *res_ptr);
void qb_do_less_than_equal_multiple_times_S64(int64_t *op1_ptr, uint32_t op1_count, int64_t *op2_ptr, uint32_t op2_count, int32_t *res_ptr);
void qb_do_less_than_equal_multiple_times_U08(uint8_t *op1_ptr, uint32_t op1_count, uint8_t *op2_ptr, uint32_t op2_count, int32_t *res_ptr);
void qb_do_less_than_equal_multiple_times_U16(uint16_t *op1_ptr, uint32_t op1_count, uint16_t *op2_ptr, uint32_t op2_count, int32_t *res_ptr);
void qb_do_less_than_equal_multiple_times_U32(uint32_t *op1_ptr, uint32_t op1_count, uint32_t *op2_ptr, uint32_t op2_count, int32_t *res_ptr);
void qb_do_less_than_equal_multiple_times_U64(uint64_t *op1_ptr, uint32_t op1_count, uint64_t *op2_ptr, uint32_t op2_count, int32_t *res_ptr);
void qb_do_less_than_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, int32_t *res_ptr);
void qb_do_less_than_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, int32_t *res_ptr);
void qb_do_less_than_multiple_times_S08(int8_t *op1_ptr, uint32_t op1_count, int8_t *op2_ptr, uint32_t op2_count, int32_t *res_ptr);
void qb_do_less_than_multiple_times_S16(int16_t *op1_ptr, uint32_t op1_count, int16_t *op2_ptr, uint32_t op2_count, int32_t *res_ptr);
void qb_do_less_than_multiple_times_S32(int32_t *op1_ptr, uint32_t op1_count, int32_t *op2_ptr, uint32_t op2_count, int32_t *res_ptr);
void qb_do_less_than_multiple_times_S64(int64_t *op1_ptr, uint32_t op1_count, int64_t *op2_ptr, uint32_t op2_count, int32_t *res_ptr);
void qb_do_less_than_multiple_times_U08(uint8_t *op1_ptr, uint32_t op1_count, uint8_t *op2_ptr, uint32_t op2_count, int32_t *res_ptr);
void qb_do_less_than_multiple_times_U16(uint16_t *op1_ptr, uint32_t op1_count, uint16_t *op2_ptr, uint32_t op2_count, int32_t *res_ptr);
void qb_do_less_than_multiple_times_U32(uint32_t *op1_ptr, uint32_t op1_count, uint32_t *op2_ptr, uint32_t op2_count, int32_t *res_ptr);
void qb_do_less_than_multiple_times_U64(uint64_t *op1_ptr, uint32_t op1_count, uint64_t *op2_ptr, uint32_t op2_count, int32_t *res_ptr);
void qb_do_log10_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count);
void qb_do_log10_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void qb_do_log1p_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count);
void qb_do_log1p_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void qb_do_log2_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count);
void qb_do_log2_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void qb_do_log_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count);
void qb_do_log_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void qb_do_max_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, float32_t *res_ptr, uint32_t res_count);
void qb_do_max_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, float64_t *res_ptr, uint32_t res_count);
void qb_do_max_multiple_times_S08(int8_t *op1_ptr, uint32_t op1_count, int8_t *op2_ptr, uint32_t op2_count, int8_t *res_ptr, uint32_t res_count);
void qb_do_max_multiple_times_S16(int16_t *op1_ptr, uint32_t op1_count, int16_t *op2_ptr, uint32_t op2_count, int16_t *res_ptr, uint32_t res_count);
void qb_do_max_multiple_times_S32(int32_t *op1_ptr, uint32_t op1_count, int32_t *op2_ptr, uint32_t op2_count, int32_t *res_ptr, uint32_t res_count);
void qb_do_max_multiple_times_S64(int64_t *op1_ptr, uint32_t op1_count, int64_t *op2_ptr, uint32_t op2_count, int64_t *res_ptr, uint32_t res_count);
void qb_do_max_multiple_times_U08(uint8_t *op1_ptr, uint32_t op1_count, uint8_t *op2_ptr, uint32_t op2_count, uint8_t *res_ptr, uint32_t res_count);
void qb_do_max_multiple_times_U16(uint16_t *op1_ptr, uint32_t op1_count, uint16_t *op2_ptr, uint32_t op2_count, uint16_t *res_ptr, uint32_t res_count);
void qb_do_max_multiple_times_U32(uint32_t *op1_ptr, uint32_t op1_count, uint32_t *op2_ptr, uint32_t op2_count, uint32_t *res_ptr, uint32_t res_count);
void qb_do_max_multiple_times_U64(uint64_t *op1_ptr, uint32_t op1_count, uint64_t *op2_ptr, uint32_t op2_count, uint64_t *res_ptr, uint32_t res_count);
void qb_do_min_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, float32_t *res_ptr, uint32_t res_count);
void qb_do_min_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, float64_t *res_ptr, uint32_t res_count);
void qb_do_min_multiple_times_S08(int8_t *op1_ptr, uint32_t op1_count, int8_t *op2_ptr, uint32_t op2_count, int8_t *res_ptr, uint32_t res_count);
void qb_do_min_multiple_times_S16(int16_t *op1_ptr, uint32_t op1_count, int16_t *op2_ptr, uint32_t op2_count, int16_t *res_ptr, uint32_t res_count);
void qb_do_min_multiple_times_S32(int32_t *op1_ptr, uint32_t op1_count, int32_t *op2_ptr, uint32_t op2_count, int32_t *res_ptr, uint32_t res_count);
void qb_do_min_multiple_times_S64(int64_t *op1_ptr, uint32_t op1_count, int64_t *op2_ptr, uint32_t op2_count, int64_t *res_ptr, uint32_t res_count);
void qb_do_min_multiple_times_U08(uint8_t *op1_ptr, uint32_t op1_count, uint8_t *op2_ptr, uint32_t op2_count, uint8_t *res_ptr, uint32_t res_count);
void qb_do_min_multiple_times_U16(uint16_t *op1_ptr, uint32_t op1_count, uint16_t *op2_ptr, uint32_t op2_count, uint16_t *res_ptr, uint32_t res_count);
void qb_do_min_multiple_times_U32(uint32_t *op1_ptr, uint32_t op1_count, uint32_t *op2_ptr, uint32_t op2_count, uint32_t *res_ptr, uint32_t res_count);
void qb_do_min_multiple_times_U64(uint64_t *op1_ptr, uint32_t op1_count, uint64_t *op2_ptr, uint32_t op2_count, uint64_t *res_ptr, uint32_t res_count);
void qb_do_mix_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, float32_t *op3_ptr, uint32_t op3_count, float32_t *res_ptr, uint32_t res_count);
void qb_do_mix_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, float64_t *op3_ptr, uint32_t op3_count, float64_t *res_ptr, uint32_t res_count);
void qb_do_modulo_2x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, float32_t *res_ptr, uint32_t res_count);
void qb_do_modulo_2x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, float64_t *res_ptr, uint32_t res_count);
void qb_do_modulo_3x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, float32_t *res_ptr, uint32_t res_count);
void qb_do_modulo_3x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, float64_t *res_ptr, uint32_t res_count);
void qb_do_modulo_4x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, float32_t *res_ptr, uint32_t res_count);
void qb_do_modulo_4x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, float64_t *res_ptr, uint32_t res_count);
void qb_do_modulo_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, float32_t *res_ptr, uint32_t res_count);
void qb_do_modulo_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, float64_t *res_ptr, uint32_t res_count);
void qb_do_multiply_2x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, float32_t *res_ptr, uint32_t res_count);
void qb_do_multiply_2x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, float64_t *res_ptr, uint32_t res_count);
void qb_do_multiply_3x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, float32_t *res_ptr, uint32_t res_count);
void qb_do_multiply_3x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, float64_t *res_ptr, uint32_t res_count);
void qb_do_multiply_4x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, float32_t *res_ptr, uint32_t res_count);
void qb_do_multiply_4x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, float64_t *res_ptr, uint32_t res_count);
void qb_do_multiply_accumulate_2x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, float32_t *op3_ptr, uint32_t op3_count, float32_t *res_ptr, uint32_t res_count);
void qb_do_multiply_accumulate_2x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, float64_t *op3_ptr, uint32_t op3_count, float64_t *res_ptr, uint32_t res_count);
void qb_do_multiply_accumulate_3x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, float32_t *op3_ptr, uint32_t op3_count, float32_t *res_ptr, uint32_t res_count);
void qb_do_multiply_accumulate_3x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, float64_t *op3_ptr, uint32_t op3_count, float64_t *res_ptr, uint32_t res_count);
void qb_do_multiply_accumulate_4x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, float32_t *op3_ptr, uint32_t op3_count, float32_t *res_ptr, uint32_t res_count);
void qb_do_multiply_accumulate_4x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, float64_t *op3_ptr, uint32_t op3_count, float64_t *res_ptr, uint32_t res_count);
void qb_do_multiply_accumulate_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, float32_t *op3_ptr, uint32_t op3_count, float32_t *res_ptr, uint32_t res_count);
void qb_do_multiply_accumulate_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, float64_t *op3_ptr, uint32_t op3_count, float64_t *res_ptr, uint32_t res_count);
void qb_do_multiply_accumulate_multiple_times_S32(int32_t *op1_ptr, uint32_t op1_count, int32_t *op2_ptr, uint32_t op2_count, int32_t *op3_ptr, uint32_t op3_count, int32_t *res_ptr, uint32_t res_count);
void qb_do_multiply_accumulate_multiple_times_S64(int64_t *op1_ptr, uint32_t op1_count, int64_t *op2_ptr, uint32_t op2_count, int64_t *op3_ptr, uint32_t op3_count, int64_t *res_ptr, uint32_t res_count);
void qb_do_multiply_accumulate_multiple_times_U32(uint32_t *op1_ptr, uint32_t op1_count, uint32_t *op2_ptr, uint32_t op2_count, uint32_t *op3_ptr, uint32_t op3_count, uint32_t *res_ptr, uint32_t res_count);
void qb_do_multiply_accumulate_multiple_times_U64(uint64_t *op1_ptr, uint32_t op1_count, uint64_t *op2_ptr, uint32_t op2_count, uint64_t *op3_ptr, uint32_t op3_count, uint64_t *res_ptr, uint32_t res_count);
void qb_do_multiply_matrix_by_matrix_2x_multiple_times_column_major_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, float32_t *res_ptr, uint32_t res_count);
void qb_do_multiply_matrix_by_matrix_2x_multiple_times_column_major_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, float64_t *res_ptr, uint32_t res_count);
void qb_do_multiply_matrix_by_matrix_3x_column_major_F32(float32_t *op1_ptr, float32_t *op2_ptr, float32_t *res_ptr);
void qb_do_multiply_matrix_by_matrix_3x_column_major_F64(float64_t *op1_ptr, float64_t *op2_ptr, float64_t *res_ptr);
void qb_do_multiply_matrix_by_matrix_3x_multiple_times_column_major_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, float32_t *res_ptr, uint32_t res_count);
void qb_do_multiply_matrix_by_matrix_3x_multiple_times_column_major_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, float64_t *res_ptr, uint32_t res_count);
void qb_do_multiply_matrix_by_matrix_4x_column_major_F32(float32_t *op1_ptr, float32_t *op2_ptr, float32_t *res_ptr);
void qb_do_multiply_matrix_by_matrix_4x_column_major_F64(float64_t *op1_ptr, float64_t *op2_ptr, float64_t *res_ptr);
void qb_do_multiply_matrix_by_matrix_4x_multiple_times_column_major_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, float32_t *res_ptr, uint32_t res_count);
void qb_do_multiply_matrix_by_matrix_4x_multiple_times_column_major_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, float64_t *res_ptr, uint32_t res_count);
void qb_do_multiply_matrix_by_matrix_column_major_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, uint32_t op3, uint32_t op4, uint32_t op5, float32_t *res_ptr, uint32_t res_count);
void qb_do_multiply_matrix_by_matrix_column_major_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, uint32_t op3, uint32_t op4, uint32_t op5, float64_t *res_ptr, uint32_t res_count);
void qb_do_multiply_matrix_by_matrix_multiple_times_column_major_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, uint32_t op3, uint32_t op4, uint32_t op5, float32_t *res_ptr, uint32_t res_count);
void qb_do_multiply_matrix_by_matrix_multiple_times_column_major_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, uint32_t op3, uint32_t op4, uint32_t op5, float64_t *res_ptr, uint32_t res_count);
void qb_do_multiply_matrix_by_matrix_padded_3x_F32(float32_t *op1_ptr, float32_t *op2_ptr, float32_t *res_ptr);
void qb_do_multiply_matrix_by_vector_2x_multiple_times_column_major_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, float32_t *res_ptr, uint32_t res_count);
void qb_do_multiply_matrix_by_vector_2x_multiple_times_column_major_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, float64_t *res_ptr, uint32_t res_count);
void qb_do_multiply_matrix_by_vector_3x_multiple_times_column_major_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, float32_t *res_ptr, uint32_t res_count);
void qb_do_multiply_matrix_by_vector_3x_multiple_times_column_major_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, float64_t *res_ptr, uint32_t res_count);
void qb_do_multiply_matrix_by_vector_4x_multiple_times_column_major_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, float32_t *res_ptr, uint32_t res_count);
void qb_do_multiply_matrix_by_vector_4x_multiple_times_column_major_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, float64_t *res_ptr, uint32_t res_count);
void qb_do_multiply_matrix_by_vector_column_major_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, uint32_t op3, uint32_t op4, float32_t *res_ptr, uint32_t res_count);
void qb_do_multiply_matrix_by_vector_column_major_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, uint32_t op3, uint32_t op4, float64_t *res_ptr, uint32_t res_count);
void qb_do_multiply_matrix_by_vector_multiple_times_column_major_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, uint32_t op3, uint32_t op4, float32_t *res_ptr, uint32_t res_count);
void qb_do_multiply_matrix_by_vector_multiple_times_column_major_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, uint32_t op3, uint32_t op4, float64_t *res_ptr, uint32_t res_count);
void qb_do_multiply_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, float32_t *res_ptr, uint32_t res_count);
void qb_do_multiply_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, float64_t *res_ptr, uint32_t res_count);
void qb_do_multiply_multiple_times_S08(int8_t *op1_ptr, uint32_t op1_count, int8_t *op2_ptr, uint32_t op2_count, int8_t *res_ptr, uint32_t res_count);
void qb_do_multiply_multiple_times_S16(int16_t *op1_ptr, uint32_t op1_count, int16_t *op2_ptr, uint32_t op2_count, int16_t *res_ptr, uint32_t res_count);
void qb_do_multiply_multiple_times_S32(int32_t *op1_ptr, uint32_t op1_count, int32_t *op2_ptr, uint32_t op2_count, int32_t *res_ptr, uint32_t res_count);
void qb_do_multiply_multiple_times_S64(int64_t *op1_ptr, uint32_t op1_count, int64_t *op2_ptr, uint32_t op2_count, int64_t *res_ptr, uint32_t res_count);
void qb_do_multiply_multiple_times_U08(uint8_t *op1_ptr, uint32_t op1_count, uint8_t *op2_ptr, uint32_t op2_count, uint8_t *res_ptr, uint32_t res_count);
void qb_do_multiply_multiple_times_U16(uint16_t *op1_ptr, uint32_t op1_count, uint16_t *op2_ptr, uint32_t op2_count, uint16_t *res_ptr, uint32_t res_count);
void qb_do_multiply_multiple_times_U32(uint32_t *op1_ptr, uint32_t op1_count, uint32_t *op2_ptr, uint32_t op2_count, uint32_t *res_ptr, uint32_t res_count);
void qb_do_multiply_multiple_times_U64(uint64_t *op1_ptr, uint32_t op1_count, uint64_t *op2_ptr, uint32_t op2_count, uint64_t *res_ptr, uint32_t res_count);
void qb_do_multiply_vector_by_matrix_2x_multiple_times_column_major_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, float32_t *res_ptr, uint32_t res_count);
void qb_do_multiply_vector_by_matrix_2x_multiple_times_column_major_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, float64_t *res_ptr, uint32_t res_count);
void qb_do_multiply_vector_by_matrix_3x_multiple_times_column_major_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, float32_t *res_ptr, uint32_t res_count);
void qb_do_multiply_vector_by_matrix_3x_multiple_times_column_major_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, float64_t *res_ptr, uint32_t res_count);
void qb_do_multiply_vector_by_matrix_4x_multiple_times_column_major_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, float32_t *res_ptr, uint32_t res_count);
void qb_do_multiply_vector_by_matrix_4x_multiple_times_column_major_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, float64_t *res_ptr, uint32_t res_count);
void qb_do_multiply_vector_by_matrix_column_major_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, uint32_t op3, uint32_t op4, float32_t *res_ptr, uint32_t res_count);
void qb_do_multiply_vector_by_matrix_column_major_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, uint32_t op3, uint32_t op4, float64_t *res_ptr, uint32_t res_count);
void qb_do_multiply_vector_by_matrix_multiple_times_column_major_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, uint32_t op3, uint32_t op4, float32_t *res_ptr, uint32_t res_count);
void qb_do_multiply_vector_by_matrix_multiple_times_column_major_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, uint32_t op3, uint32_t op4, float64_t *res_ptr, uint32_t res_count);
void qb_do_negate_2x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count);
void qb_do_negate_2x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void qb_do_negate_3x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count);
void qb_do_negate_3x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void qb_do_negate_4x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count);
void qb_do_negate_4x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void qb_do_negate_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count);
void qb_do_negate_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void qb_do_negate_multiple_times_I08(int8_t *op1_ptr, uint32_t op1_count, int8_t *res_ptr, uint32_t res_count);
void qb_do_negate_multiple_times_I16(int16_t *op1_ptr, uint32_t op1_count, int16_t *res_ptr, uint32_t res_count);
void qb_do_negate_multiple_times_I32(int32_t *op1_ptr, uint32_t op1_count, int32_t *res_ptr, uint32_t res_count);
void qb_do_negate_multiple_times_I64(int64_t *op1_ptr, uint32_t op1_count, int64_t *res_ptr, uint32_t res_count);
void qb_do_normalize_2x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count);
void qb_do_normalize_2x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void qb_do_normalize_3x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count);
void qb_do_normalize_3x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void qb_do_normalize_4x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count);
void qb_do_normalize_4x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void qb_do_normalize_F32(float32_t *op1_ptr, uint32_t op1_count, uint32_t op2, float32_t *res_ptr, uint32_t res_count);
void qb_do_normalize_F64(float64_t *op1_ptr, uint32_t op1_count, uint32_t op2, float64_t *res_ptr, uint32_t res_count);
void qb_do_normalize_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, uint32_t op2, float32_t *res_ptr, uint32_t res_count);
void qb_do_normalize_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, uint32_t op2, float64_t *res_ptr, uint32_t res_count);
void qb_do_not_equal_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, int32_t *res_ptr);
void qb_do_not_equal_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, int32_t *res_ptr);
void qb_do_not_equal_multiple_times_I08(int8_t *op1_ptr, uint32_t op1_count, int8_t *op2_ptr, uint32_t op2_count, int32_t *res_ptr);
void qb_do_not_equal_multiple_times_I16(int16_t *op1_ptr, uint32_t op1_count, int16_t *op2_ptr, uint32_t op2_count, int32_t *res_ptr);
void qb_do_not_equal_multiple_times_I32(int32_t *op1_ptr, uint32_t op1_count, int32_t *op2_ptr, uint32_t op2_count, int32_t *res_ptr);
void qb_do_not_equal_multiple_times_I64(int64_t *op1_ptr, uint32_t op1_count, int64_t *op2_ptr, uint32_t op2_count, int32_t *res_ptr);
void qb_do_pow_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, float32_t *res_ptr, uint32_t res_count);
void qb_do_pow_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, float64_t *res_ptr, uint32_t res_count);
void qb_do_predicate_clear_array_F32(int32_t op1, float32_t *res_ptr, uint32_t res_count);
void qb_do_predicate_clear_array_F64(int32_t op1, float64_t *res_ptr, uint32_t res_count);
void qb_do_predicate_clear_array_I08(int32_t op1, int8_t *res_ptr, uint32_t res_count);
void qb_do_predicate_clear_array_I16(int32_t op1, int16_t *res_ptr, uint32_t res_count);
void qb_do_predicate_clear_array_I32(int32_t op1, int32_t *res_ptr, uint32_t res_count);
void qb_do_predicate_clear_array_I64(int32_t op1, int64_t *res_ptr, uint32_t res_count);
void qb_do_predicate_clear_element_F32(uint32_t op1, uint32_t op2, int32_t op3, float32_t *res_ptr, uint32_t res_count);
void qb_do_predicate_clear_element_F64(uint32_t op1, uint32_t op2, int32_t op3, float64_t *res_ptr, uint32_t res_count);
void qb_do_predicate_clear_element_I08(uint32_t op1, uint32_t op2, int32_t op3, int8_t *res_ptr, uint32_t res_count);
void qb_do_predicate_clear_element_I16(uint32_t op1, uint32_t op2, int32_t op3, int16_t *res_ptr, uint32_t res_count);
void qb_do_predicate_clear_element_I32(uint32_t op1, uint32_t op2, int32_t op3, int32_t *res_ptr, uint32_t res_count);
void qb_do_predicate_clear_element_I64(uint32_t op1, uint32_t op2, int32_t op3, int64_t *res_ptr, uint32_t res_count);
void qb_do_print_multidimensional_variable_F32(qb_interpreter_context *__restrict cxt, float32_t *op1_ptr, uint32_t op1_count, uint32_t *op2_ptr, uint32_t op2_count);
void qb_do_print_multidimensional_variable_F64(qb_interpreter_context *__restrict cxt, float64_t *op1_ptr, uint32_t op1_count, uint32_t *op2_ptr, uint32_t op2_count);
void qb_do_print_multidimensional_variable_S08(qb_interpreter_context *__restrict cxt, int8_t *op1_ptr, uint32_t op1_count, uint32_t *op2_ptr, uint32_t op2_count);
void qb_do_print_multidimensional_variable_S16(qb_interpreter_context *__restrict cxt, int16_t *op1_ptr, uint32_t op1_count, uint32_t *op2_ptr, uint32_t op2_count);
void qb_do_print_multidimensional_variable_S32(qb_interpreter_context *__restrict cxt, int32_t *op1_ptr, uint32_t op1_count, uint32_t *op2_ptr, uint32_t op2_count);
void qb_do_print_multidimensional_variable_S64(qb_interpreter_context *__restrict cxt, int64_t *op1_ptr, uint32_t op1_count, uint32_t *op2_ptr, uint32_t op2_count);
void qb_do_print_multidimensional_variable_U08(qb_interpreter_context *__restrict cxt, uint8_t *op1_ptr, uint32_t op1_count, uint32_t *op2_ptr, uint32_t op2_count);
void qb_do_print_multidimensional_variable_U16(qb_interpreter_context *__restrict cxt, uint16_t *op1_ptr, uint32_t op1_count, uint32_t *op2_ptr, uint32_t op2_count);
void qb_do_print_multidimensional_variable_U32(qb_interpreter_context *__restrict cxt, uint32_t *op1_ptr, uint32_t op1_count, uint32_t *op2_ptr, uint32_t op2_count);
void qb_do_print_multidimensional_variable_U64(qb_interpreter_context *__restrict cxt, uint64_t *op1_ptr, uint32_t op1_count, uint32_t *op2_ptr, uint32_t op2_count);
void qb_do_print_string_U08(qb_interpreter_context *__restrict cxt, uint8_t *op1_ptr, uint32_t op1_count);
void qb_do_print_variable_F32(qb_interpreter_context *__restrict cxt, float32_t op1);
void qb_do_print_variable_F64(qb_interpreter_context *__restrict cxt, float64_t op1);
void qb_do_print_variable_S08(qb_interpreter_context *__restrict cxt, int8_t op1);
void qb_do_print_variable_S16(qb_interpreter_context *__restrict cxt, int16_t op1);
void qb_do_print_variable_S32(qb_interpreter_context *__restrict cxt, int32_t op1);
void qb_do_print_variable_S64(qb_interpreter_context *__restrict cxt, int64_t op1);
void qb_do_print_variable_U08(qb_interpreter_context *__restrict cxt, uint8_t op1);
void qb_do_print_variable_U16(qb_interpreter_context *__restrict cxt, uint16_t op1);
void qb_do_print_variable_U32(qb_interpreter_context *__restrict cxt, uint32_t op1);
void qb_do_print_variable_U64(qb_interpreter_context *__restrict cxt, uint64_t op1);
void qb_do_print_variable_multiple_times_F32(qb_interpreter_context *__restrict cxt, float32_t *op1_ptr, uint32_t op1_count);
void qb_do_print_variable_multiple_times_F64(qb_interpreter_context *__restrict cxt, float64_t *op1_ptr, uint32_t op1_count);
void qb_do_print_variable_multiple_times_S08(qb_interpreter_context *__restrict cxt, int8_t *op1_ptr, uint32_t op1_count);
void qb_do_print_variable_multiple_times_S16(qb_interpreter_context *__restrict cxt, int16_t *op1_ptr, uint32_t op1_count);
void qb_do_print_variable_multiple_times_S32(qb_interpreter_context *__restrict cxt, int32_t *op1_ptr, uint32_t op1_count);
void qb_do_print_variable_multiple_times_S64(qb_interpreter_context *__restrict cxt, int64_t *op1_ptr, uint32_t op1_count);
void qb_do_print_variable_multiple_times_U08(qb_interpreter_context *__restrict cxt, uint8_t *op1_ptr, uint32_t op1_count);
void qb_do_print_variable_multiple_times_U16(qb_interpreter_context *__restrict cxt, uint16_t *op1_ptr, uint32_t op1_count);
void qb_do_print_variable_multiple_times_U32(qb_interpreter_context *__restrict cxt, uint32_t *op1_ptr, uint32_t op1_count);
void qb_do_print_variable_multiple_times_U64(qb_interpreter_context *__restrict cxt, uint64_t *op1_ptr, uint32_t op1_count);
void qb_do_radian_to_degree_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count);
void qb_do_radian_to_degree_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void qb_do_random_S08(qb_interpreter_context *__restrict cxt, int8_t op1, int8_t op2, int8_t *res_ptr);
void qb_do_random_S16(qb_interpreter_context *__restrict cxt, int16_t op1, int16_t op2, int16_t *res_ptr);
void qb_do_random_S32(qb_interpreter_context *__restrict cxt, int32_t op1, int32_t op2, int32_t *res_ptr);
void qb_do_random_S64(qb_interpreter_context *__restrict cxt, int64_t op1, int64_t op2, int64_t *res_ptr);
void qb_do_random_U08(qb_interpreter_context *__restrict cxt, uint8_t op1, uint8_t op2, uint8_t *res_ptr);
void qb_do_random_U16(qb_interpreter_context *__restrict cxt, uint16_t op1, uint16_t op2, uint16_t *res_ptr);
void qb_do_random_U32(qb_interpreter_context *__restrict cxt, uint32_t op1, uint32_t op2, uint32_t *res_ptr);
void qb_do_random_U64(qb_interpreter_context *__restrict cxt, uint64_t op1, uint64_t op2, uint64_t *res_ptr);
void qb_do_random_mt_S08(qb_interpreter_context *__restrict cxt, int8_t op1, int8_t op2, int8_t *res_ptr);
void qb_do_random_mt_S16(qb_interpreter_context *__restrict cxt, int16_t op1, int16_t op2, int16_t *res_ptr);
void qb_do_random_mt_S32(qb_interpreter_context *__restrict cxt, int32_t op1, int32_t op2, int32_t *res_ptr);
void qb_do_random_mt_S64(qb_interpreter_context *__restrict cxt, int64_t op1, int64_t op2, int64_t *res_ptr);
void qb_do_random_mt_U08(qb_interpreter_context *__restrict cxt, uint8_t op1, uint8_t op2, uint8_t *res_ptr);
void qb_do_random_mt_U16(qb_interpreter_context *__restrict cxt, uint16_t op1, uint16_t op2, uint16_t *res_ptr);
void qb_do_random_mt_U32(qb_interpreter_context *__restrict cxt, uint32_t op1, uint32_t op2, uint32_t *res_ptr);
void qb_do_random_mt_U64(qb_interpreter_context *__restrict cxt, uint64_t op1, uint64_t op2, uint64_t *res_ptr);
void qb_do_random_mt_multiple_times_S08(qb_interpreter_context *__restrict cxt, int8_t *op1_ptr, uint32_t op1_count, int8_t *op2_ptr, uint32_t op2_count, int8_t *res_ptr, uint32_t res_count);
void qb_do_random_mt_multiple_times_S16(qb_interpreter_context *__restrict cxt, int16_t *op1_ptr, uint32_t op1_count, int16_t *op2_ptr, uint32_t op2_count, int16_t *res_ptr, uint32_t res_count);
void qb_do_random_mt_multiple_times_S32(qb_interpreter_context *__restrict cxt, int32_t *op1_ptr, uint32_t op1_count, int32_t *op2_ptr, uint32_t op2_count, int32_t *res_ptr, uint32_t res_count);
void qb_do_random_mt_multiple_times_S64(qb_interpreter_context *__restrict cxt, int64_t *op1_ptr, uint32_t op1_count, int64_t *op2_ptr, uint32_t op2_count, int64_t *res_ptr, uint32_t res_count);
void qb_do_random_mt_multiple_times_U08(qb_interpreter_context *__restrict cxt, uint8_t *op1_ptr, uint32_t op1_count, uint8_t *op2_ptr, uint32_t op2_count, uint8_t *res_ptr, uint32_t res_count);
void qb_do_random_mt_multiple_times_U16(qb_interpreter_context *__restrict cxt, uint16_t *op1_ptr, uint32_t op1_count, uint16_t *op2_ptr, uint32_t op2_count, uint16_t *res_ptr, uint32_t res_count);
void qb_do_random_mt_multiple_times_U32(qb_interpreter_context *__restrict cxt, uint32_t *op1_ptr, uint32_t op1_count, uint32_t *op2_ptr, uint32_t op2_count, uint32_t *res_ptr, uint32_t res_count);
void qb_do_random_mt_multiple_times_U64(qb_interpreter_context *__restrict cxt, uint64_t *op1_ptr, uint32_t op1_count, uint64_t *op2_ptr, uint32_t op2_count, uint64_t *res_ptr, uint32_t res_count);
void qb_do_random_multiple_times_S08(qb_interpreter_context *__restrict cxt, int8_t *op1_ptr, uint32_t op1_count, int8_t *op2_ptr, uint32_t op2_count, int8_t *res_ptr, uint32_t res_count);
void qb_do_random_multiple_times_S16(qb_interpreter_context *__restrict cxt, int16_t *op1_ptr, uint32_t op1_count, int16_t *op2_ptr, uint32_t op2_count, int16_t *res_ptr, uint32_t res_count);
void qb_do_random_multiple_times_S32(qb_interpreter_context *__restrict cxt, int32_t *op1_ptr, uint32_t op1_count, int32_t *op2_ptr, uint32_t op2_count, int32_t *res_ptr, uint32_t res_count);
void qb_do_random_multiple_times_S64(qb_interpreter_context *__restrict cxt, int64_t *op1_ptr, uint32_t op1_count, int64_t *op2_ptr, uint32_t op2_count, int64_t *res_ptr, uint32_t res_count);
void qb_do_random_multiple_times_U08(qb_interpreter_context *__restrict cxt, uint8_t *op1_ptr, uint32_t op1_count, uint8_t *op2_ptr, uint32_t op2_count, uint8_t *res_ptr, uint32_t res_count);
void qb_do_random_multiple_times_U16(qb_interpreter_context *__restrict cxt, uint16_t *op1_ptr, uint32_t op1_count, uint16_t *op2_ptr, uint32_t op2_count, uint16_t *res_ptr, uint32_t res_count);
void qb_do_random_multiple_times_U32(qb_interpreter_context *__restrict cxt, uint32_t *op1_ptr, uint32_t op1_count, uint32_t *op2_ptr, uint32_t op2_count, uint32_t *res_ptr, uint32_t res_count);
void qb_do_random_multiple_times_U64(qb_interpreter_context *__restrict cxt, uint64_t *op1_ptr, uint32_t op1_count, uint64_t *op2_ptr, uint32_t op2_count, uint64_t *res_ptr, uint32_t res_count);
void qb_do_range_F32(float32_t op1, float32_t op2, float32_t op3, float32_t *res_ptr, uint32_t res_count);
void qb_do_range_F64(float64_t op1, float64_t op2, float64_t op3, float64_t *res_ptr, uint32_t res_count);
void qb_do_range_S08(int8_t op1, int8_t op2, int8_t op3, int8_t *res_ptr, uint32_t res_count);
void qb_do_range_S16(int16_t op1, int16_t op2, int16_t op3, int16_t *res_ptr, uint32_t res_count);
void qb_do_range_S32(int32_t op1, int32_t op2, int32_t op3, int32_t *res_ptr, uint32_t res_count);
void qb_do_range_S64(int64_t op1, int64_t op2, int64_t op3, int64_t *res_ptr, uint32_t res_count);
void qb_do_range_U08(uint8_t op1, uint8_t op2, int8_t op3, uint8_t *res_ptr, uint32_t res_count);
void qb_do_range_U16(uint16_t op1, uint16_t op2, int16_t op3, uint16_t *res_ptr, uint32_t res_count);
void qb_do_range_U32(uint32_t op1, uint32_t op2, int32_t op3, uint32_t *res_ptr, uint32_t res_count);
void qb_do_range_U64(uint64_t op1, uint64_t op2, int64_t op3, uint64_t *res_ptr, uint32_t res_count);
void qb_do_range_count_F32(float32_t op1, float32_t op2, float32_t op3, uint32_t *res_ptr);
void qb_do_range_count_F64(float64_t op1, float64_t op2, float64_t op3, uint32_t *res_ptr);
void qb_do_range_count_S08(int8_t op1, int8_t op2, int8_t op3, uint32_t *res_ptr);
void qb_do_range_count_S16(int16_t op1, int16_t op2, int16_t op3, uint32_t *res_ptr);
void qb_do_range_count_S32(int32_t op1, int32_t op2, int32_t op3, uint32_t *res_ptr);
void qb_do_range_count_S64(int64_t op1, int64_t op2, int64_t op3, uint32_t *res_ptr);
void qb_do_range_count_U08(uint8_t op1, uint8_t op2, int8_t op3, uint32_t *res_ptr);
void qb_do_range_count_U16(uint16_t op1, uint16_t op2, int16_t op3, uint32_t *res_ptr);
void qb_do_range_count_U32(uint32_t op1, uint32_t op2, int32_t op3, uint32_t *res_ptr);
void qb_do_range_count_U64(uint64_t op1, uint64_t op2, int64_t op3, uint32_t *res_ptr);
void qb_do_reflect_2x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, float32_t *res_ptr, uint32_t res_count);
void qb_do_reflect_2x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, float64_t *res_ptr, uint32_t res_count);
void qb_do_reflect_3x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, float32_t *res_ptr, uint32_t res_count);
void qb_do_reflect_3x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, float64_t *res_ptr, uint32_t res_count);
void qb_do_reflect_4x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, float32_t *res_ptr, uint32_t res_count);
void qb_do_reflect_4x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, float64_t *res_ptr, uint32_t res_count);
void qb_do_reflect_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, uint32_t op3, float32_t *res_ptr, uint32_t res_count);
void qb_do_reflect_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, uint32_t op3, float64_t *res_ptr, uint32_t res_count);
void qb_do_reflect_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, uint32_t op3, float32_t *res_ptr, uint32_t res_count);
void qb_do_reflect_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, uint32_t op3, float64_t *res_ptr, uint32_t res_count);
void qb_do_refract_2x_F32(float32_t *op1_ptr, float32_t *op2_ptr, float32_t op3, float32_t *res_ptr);
void qb_do_refract_2x_F64(float64_t *op1_ptr, float64_t *op2_ptr, float64_t op3, float64_t *res_ptr);
void qb_do_refract_2x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, float32_t op3, float32_t *res_ptr, uint32_t res_count);
void qb_do_refract_2x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, float64_t op3, float64_t *res_ptr, uint32_t res_count);
void qb_do_refract_3x_F32(float32_t *op1_ptr, float32_t *op2_ptr, float32_t op3, float32_t *res_ptr);
void qb_do_refract_3x_F64(float64_t *op1_ptr, float64_t *op2_ptr, float64_t op3, float64_t *res_ptr);
void qb_do_refract_3x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, float32_t op3, float32_t *res_ptr, uint32_t res_count);
void qb_do_refract_3x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, float64_t op3, float64_t *res_ptr, uint32_t res_count);
void qb_do_refract_4x_F32(float32_t *op1_ptr, float32_t *op2_ptr, float32_t op3, float32_t *res_ptr);
void qb_do_refract_4x_F64(float64_t *op1_ptr, float64_t *op2_ptr, float64_t op3, float64_t *res_ptr);
void qb_do_refract_4x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, float32_t op3, float32_t *res_ptr, uint32_t res_count);
void qb_do_refract_4x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, float64_t op3, float64_t *res_ptr, uint32_t res_count);
void qb_do_refract_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, uint32_t op3, uint32_t op4, float32_t *res_ptr, uint32_t res_count);
void qb_do_refract_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, uint32_t op3, uint32_t op4, float64_t *res_ptr, uint32_t res_count);
void qb_do_refract_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, uint32_t op3, uint32_t op4, float32_t *res_ptr, uint32_t res_count);
void qb_do_refract_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, uint32_t op3, uint32_t op4, float64_t *res_ptr, uint32_t res_count);
void qb_do_remove_premultiplication_4x_F32(float32_t *op1_ptr, float32_t *res_ptr);
void qb_do_remove_premultiplication_4x_F64(float64_t *op1_ptr, float64_t *res_ptr);
void qb_do_remove_premultiplication_4x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count);
void qb_do_remove_premultiplication_4x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void qb_do_reverse_sort_F32(uint32_t op1, float32_t *res_ptr, uint32_t res_count);
void qb_do_reverse_sort_F64(uint32_t op1, float64_t *res_ptr, uint32_t res_count);
void qb_do_reverse_sort_S08(uint32_t op1, int8_t *res_ptr, uint32_t res_count);
void qb_do_reverse_sort_S16(uint32_t op1, int16_t *res_ptr, uint32_t res_count);
void qb_do_reverse_sort_S32(uint32_t op1, int32_t *res_ptr, uint32_t res_count);
void qb_do_reverse_sort_S64(uint32_t op1, int64_t *res_ptr, uint32_t res_count);
void qb_do_reverse_sort_U08(uint32_t op1, uint8_t *res_ptr, uint32_t res_count);
void qb_do_reverse_sort_U16(uint32_t op1, uint16_t *res_ptr, uint32_t res_count);
void qb_do_reverse_sort_U32(uint32_t op1, uint32_t *res_ptr, uint32_t res_count);
void qb_do_reverse_sort_U64(uint32_t op1, uint64_t *res_ptr, uint32_t res_count);
void qb_do_rgb2hsl_3x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count);
void qb_do_rgb2hsl_3x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void qb_do_rgb2hsl_4x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count);
void qb_do_rgb2hsl_4x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void qb_do_rgb2hsv_3x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count);
void qb_do_rgb2hsv_3x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void qb_do_rgb2hsv_4x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count);
void qb_do_rgb2hsv_4x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void qb_do_rint_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count);
void qb_do_rint_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void qb_do_round_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count);
void qb_do_round_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void qb_do_round_to_precision_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, int32_t *op2_ptr, uint32_t op2_count, int32_t *op3_ptr, uint32_t op3_count, float32_t *res_ptr, uint32_t res_count);
void qb_do_round_to_precision_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, int32_t *op2_ptr, uint32_t op2_count, int32_t *op3_ptr, uint32_t op3_count, float64_t *res_ptr, uint32_t res_count);
void qb_do_rsqrt_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count);
void qb_do_rsqrt_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void qb_do_sample_bilinear_2x_F32(float32_t *op1_ptr, uint32_t op2, uint32_t op3, float32_t op4, float32_t op5, float32_t *res_ptr);
void qb_do_sample_bilinear_2x_F64(float64_t *op1_ptr, uint32_t op2, uint32_t op3, float64_t op4, float64_t op5, float64_t *res_ptr);
void qb_do_sample_bilinear_2x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, uint32_t op2, uint32_t op3, float32_t *op4_ptr, uint32_t op4_count, float32_t *op5_ptr, uint32_t op5_count, float32_t *res_ptr, uint32_t res_count);
void qb_do_sample_bilinear_2x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, uint32_t op2, uint32_t op3, float64_t *op4_ptr, uint32_t op4_count, float64_t *op5_ptr, uint32_t op5_count, float64_t *res_ptr, uint32_t res_count);
void qb_do_sample_bilinear_3x_F32(float32_t *op1_ptr, uint32_t op2, uint32_t op3, float32_t op4, float32_t op5, float32_t *res_ptr);
void qb_do_sample_bilinear_3x_F64(float64_t *op1_ptr, uint32_t op2, uint32_t op3, float64_t op4, float64_t op5, float64_t *res_ptr);
void qb_do_sample_bilinear_3x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, uint32_t op2, uint32_t op3, float32_t *op4_ptr, uint32_t op4_count, float32_t *op5_ptr, uint32_t op5_count, float32_t *res_ptr, uint32_t res_count);
void qb_do_sample_bilinear_3x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, uint32_t op2, uint32_t op3, float64_t *op4_ptr, uint32_t op4_count, float64_t *op5_ptr, uint32_t op5_count, float64_t *res_ptr, uint32_t res_count);
void qb_do_sample_bilinear_4x_F32(float32_t *op1_ptr, uint32_t op2, uint32_t op3, float32_t op4, float32_t op5, float32_t *res_ptr);
void qb_do_sample_bilinear_4x_F64(float64_t *op1_ptr, uint32_t op2, uint32_t op3, float64_t op4, float64_t op5, float64_t *res_ptr);
void qb_do_sample_bilinear_4x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, uint32_t op2, uint32_t op3, float32_t *op4_ptr, uint32_t op4_count, float32_t *op5_ptr, uint32_t op5_count, float32_t *res_ptr, uint32_t res_count);
void qb_do_sample_bilinear_4x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, uint32_t op2, uint32_t op3, float64_t *op4_ptr, uint32_t op4_count, float64_t *op5_ptr, uint32_t op5_count, float64_t *res_ptr, uint32_t res_count);
void qb_do_sample_bilinear_F32(float32_t *op1_ptr, uint32_t op2, uint32_t op3, float32_t op4, float32_t op5, float32_t *res_ptr);
void qb_do_sample_bilinear_F64(float64_t *op1_ptr, uint32_t op2, uint32_t op3, float64_t op4, float64_t op5, float64_t *res_ptr);
void qb_do_sample_bilinear_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, uint32_t op2, uint32_t op3, float32_t *op4_ptr, uint32_t op4_count, float32_t *op5_ptr, uint32_t op5_count, float32_t *res_ptr, uint32_t res_count);
void qb_do_sample_bilinear_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, uint32_t op2, uint32_t op3, float64_t *op4_ptr, uint32_t op4_count, float64_t *op5_ptr, uint32_t op5_count, float64_t *res_ptr, uint32_t res_count);
void qb_do_sample_nearest_2x_F32(float32_t *op1_ptr, uint32_t op2, uint32_t op3, float32_t op4, float32_t op5, float32_t *res_ptr);
void qb_do_sample_nearest_2x_F64(float64_t *op1_ptr, uint32_t op2, uint32_t op3, float64_t op4, float64_t op5, float64_t *res_ptr);
void qb_do_sample_nearest_2x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, uint32_t op2, uint32_t op3, float32_t *op4_ptr, uint32_t op4_count, float32_t *op5_ptr, uint32_t op5_count, float32_t *res_ptr, uint32_t res_count);
void qb_do_sample_nearest_2x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, uint32_t op2, uint32_t op3, float64_t *op4_ptr, uint32_t op4_count, float64_t *op5_ptr, uint32_t op5_count, float64_t *res_ptr, uint32_t res_count);
void qb_do_sample_nearest_3x_F32(float32_t *op1_ptr, uint32_t op2, uint32_t op3, float32_t op4, float32_t op5, float32_t *res_ptr);
void qb_do_sample_nearest_3x_F64(float64_t *op1_ptr, uint32_t op2, uint32_t op3, float64_t op4, float64_t op5, float64_t *res_ptr);
void qb_do_sample_nearest_3x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, uint32_t op2, uint32_t op3, float32_t *op4_ptr, uint32_t op4_count, float32_t *op5_ptr, uint32_t op5_count, float32_t *res_ptr, uint32_t res_count);
void qb_do_sample_nearest_3x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, uint32_t op2, uint32_t op3, float64_t *op4_ptr, uint32_t op4_count, float64_t *op5_ptr, uint32_t op5_count, float64_t *res_ptr, uint32_t res_count);
void qb_do_sample_nearest_4x_F32(float32_t *op1_ptr, uint32_t op2, uint32_t op3, float32_t op4, float32_t op5, float32_t *res_ptr);
void qb_do_sample_nearest_4x_F64(float64_t *op1_ptr, uint32_t op2, uint32_t op3, float64_t op4, float64_t op5, float64_t *res_ptr);
void qb_do_sample_nearest_4x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, uint32_t op2, uint32_t op3, float32_t *op4_ptr, uint32_t op4_count, float32_t *op5_ptr, uint32_t op5_count, float32_t *res_ptr, uint32_t res_count);
void qb_do_sample_nearest_4x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, uint32_t op2, uint32_t op3, float64_t *op4_ptr, uint32_t op4_count, float64_t *op5_ptr, uint32_t op5_count, float64_t *res_ptr, uint32_t res_count);
void qb_do_sample_nearest_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, uint32_t op2, uint32_t op3, float32_t *op4_ptr, uint32_t op4_count, float32_t *op5_ptr, uint32_t op5_count, float32_t *res_ptr, uint32_t res_count);
void qb_do_sample_nearest_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, uint32_t op2, uint32_t op3, float64_t *op4_ptr, uint32_t op4_count, float64_t *op5_ptr, uint32_t op5_count, float64_t *res_ptr, uint32_t res_count);
void qb_do_scatter_4x_F32(uint32_t op1, float32_t *op2_ptr, float32_t *res_ptr);
void qb_do_scatter_4x_F64(uint32_t op1, float64_t *op2_ptr, float64_t *res_ptr);
void qb_do_scatter_4x_I08(uint32_t op1, int8_t *op2_ptr, int8_t *res_ptr);
void qb_do_scatter_4x_I16(uint32_t op1, int16_t *op2_ptr, int16_t *res_ptr);
void qb_do_scatter_4x_I32(uint32_t op1, int32_t *op2_ptr, int32_t *res_ptr);
void qb_do_scatter_4x_I64(uint32_t op1, int64_t *op2_ptr, int64_t *res_ptr);
void qb_do_scatter_5x_F32(uint32_t op1, float32_t *op2_ptr, float32_t *res_ptr);
void qb_do_scatter_5x_F64(uint32_t op1, float64_t *op2_ptr, float64_t *res_ptr);
void qb_do_scatter_5x_I08(uint32_t op1, int8_t *op2_ptr, int8_t *res_ptr);
void qb_do_scatter_5x_I16(uint32_t op1, int16_t *op2_ptr, int16_t *res_ptr);
void qb_do_scatter_5x_I32(uint32_t op1, int32_t *op2_ptr, int32_t *res_ptr);
void qb_do_scatter_5x_I64(uint32_t op1, int64_t *op2_ptr, int64_t *res_ptr);
void qb_do_scatter_6x_F32(uint32_t op1, float32_t *op2_ptr, float32_t *res_ptr);
void qb_do_scatter_6x_F64(uint32_t op1, float64_t *op2_ptr, float64_t *res_ptr);
void qb_do_scatter_6x_I08(uint32_t op1, int8_t *op2_ptr, int8_t *res_ptr);
void qb_do_scatter_6x_I16(uint32_t op1, int16_t *op2_ptr, int16_t *res_ptr);
void qb_do_scatter_6x_I32(uint32_t op1, int32_t *op2_ptr, int32_t *res_ptr);
void qb_do_scatter_6x_I64(uint32_t op1, int64_t *op2_ptr, int64_t *res_ptr);
void qb_do_scatter_7x_F32(uint32_t op1, float32_t *op2_ptr, float32_t *res_ptr);
void qb_do_scatter_7x_F64(uint32_t op1, float64_t *op2_ptr, float64_t *res_ptr);
void qb_do_scatter_7x_I08(uint32_t op1, int8_t *op2_ptr, int8_t *res_ptr);
void qb_do_scatter_7x_I16(uint32_t op1, int16_t *op2_ptr, int16_t *res_ptr);
void qb_do_scatter_7x_I32(uint32_t op1, int32_t *op2_ptr, int32_t *res_ptr);
void qb_do_scatter_7x_I64(uint32_t op1, int64_t *op2_ptr, int64_t *res_ptr);
void qb_do_scatter_8x_F32(uint32_t op1, float32_t *op2_ptr, float32_t *res_ptr);
void qb_do_scatter_8x_F64(uint32_t op1, float64_t *op2_ptr, float64_t *res_ptr);
void qb_do_scatter_8x_I08(uint32_t op1, int8_t *op2_ptr, int8_t *res_ptr);
void qb_do_scatter_8x_I16(uint32_t op1, int16_t *op2_ptr, int16_t *res_ptr);
void qb_do_scatter_8x_I32(uint32_t op1, int32_t *op2_ptr, int32_t *res_ptr);
void qb_do_scatter_8x_I64(uint32_t op1, int64_t *op2_ptr, int64_t *res_ptr);
void qb_do_set_all_I32(int32_t *op1_ptr, uint32_t op1_count, int32_t *res_ptr);
void qb_do_set_any_I32(int32_t *op1_ptr, uint32_t op1_count, int32_t *res_ptr);
void qb_do_set_equal_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, int32_t *res_ptr, uint32_t res_count);
void qb_do_set_equal_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, int32_t *res_ptr, uint32_t res_count);
void qb_do_set_equal_I08(int8_t *op1_ptr, uint32_t op1_count, int8_t *op2_ptr, uint32_t op2_count, int32_t *res_ptr, uint32_t res_count);
void qb_do_set_equal_I16(int16_t *op1_ptr, uint32_t op1_count, int16_t *op2_ptr, uint32_t op2_count, int32_t *res_ptr, uint32_t res_count);
void qb_do_set_equal_I32(int32_t *op1_ptr, uint32_t op1_count, int32_t *op2_ptr, uint32_t op2_count, int32_t *res_ptr, uint32_t res_count);
void qb_do_set_equal_I64(int64_t *op1_ptr, uint32_t op1_count, int64_t *op2_ptr, uint32_t op2_count, int32_t *res_ptr, uint32_t res_count);
void qb_do_set_less_than_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, int32_t *res_ptr, uint32_t res_count);
void qb_do_set_less_than_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, int32_t *res_ptr, uint32_t res_count);
void qb_do_set_less_than_S08(int8_t *op1_ptr, uint32_t op1_count, int8_t *op2_ptr, uint32_t op2_count, int32_t *res_ptr, uint32_t res_count);
void qb_do_set_less_than_S16(int16_t *op1_ptr, uint32_t op1_count, int16_t *op2_ptr, uint32_t op2_count, int32_t *res_ptr, uint32_t res_count);
void qb_do_set_less_than_S32(int32_t *op1_ptr, uint32_t op1_count, int32_t *op2_ptr, uint32_t op2_count, int32_t *res_ptr, uint32_t res_count);
void qb_do_set_less_than_S64(int64_t *op1_ptr, uint32_t op1_count, int64_t *op2_ptr, uint32_t op2_count, int32_t *res_ptr, uint32_t res_count);
void qb_do_set_less_than_U08(uint8_t *op1_ptr, uint32_t op1_count, uint8_t *op2_ptr, uint32_t op2_count, int32_t *res_ptr, uint32_t res_count);
void qb_do_set_less_than_U16(uint16_t *op1_ptr, uint32_t op1_count, uint16_t *op2_ptr, uint32_t op2_count, int32_t *res_ptr, uint32_t res_count);
void qb_do_set_less_than_U32(uint32_t *op1_ptr, uint32_t op1_count, uint32_t *op2_ptr, uint32_t op2_count, int32_t *res_ptr, uint32_t res_count);
void qb_do_set_less_than_U64(uint64_t *op1_ptr, uint32_t op1_count, uint64_t *op2_ptr, uint32_t op2_count, int32_t *res_ptr, uint32_t res_count);
void qb_do_set_less_than_equal_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, int32_t *res_ptr, uint32_t res_count);
void qb_do_set_less_than_equal_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, int32_t *res_ptr, uint32_t res_count);
void qb_do_set_less_than_equal_S08(int8_t *op1_ptr, uint32_t op1_count, int8_t *op2_ptr, uint32_t op2_count, int32_t *res_ptr, uint32_t res_count);
void qb_do_set_less_than_equal_S16(int16_t *op1_ptr, uint32_t op1_count, int16_t *op2_ptr, uint32_t op2_count, int32_t *res_ptr, uint32_t res_count);
void qb_do_set_less_than_equal_S32(int32_t *op1_ptr, uint32_t op1_count, int32_t *op2_ptr, uint32_t op2_count, int32_t *res_ptr, uint32_t res_count);
void qb_do_set_less_than_equal_S64(int64_t *op1_ptr, uint32_t op1_count, int64_t *op2_ptr, uint32_t op2_count, int32_t *res_ptr, uint32_t res_count);
void qb_do_set_less_than_equal_U08(uint8_t *op1_ptr, uint32_t op1_count, uint8_t *op2_ptr, uint32_t op2_count, int32_t *res_ptr, uint32_t res_count);
void qb_do_set_less_than_equal_U16(uint16_t *op1_ptr, uint32_t op1_count, uint16_t *op2_ptr, uint32_t op2_count, int32_t *res_ptr, uint32_t res_count);
void qb_do_set_less_than_equal_U32(uint32_t *op1_ptr, uint32_t op1_count, uint32_t *op2_ptr, uint32_t op2_count, int32_t *res_ptr, uint32_t res_count);
void qb_do_set_less_than_equal_U64(uint64_t *op1_ptr, uint32_t op1_count, uint64_t *op2_ptr, uint32_t op2_count, int32_t *res_ptr, uint32_t res_count);
void qb_do_set_not_I32(int32_t *op1_ptr, uint32_t op1_count, int32_t *res_ptr, uint32_t res_count);
void qb_do_set_not_equal_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, int32_t *res_ptr, uint32_t res_count);
void qb_do_set_not_equal_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, int32_t *res_ptr, uint32_t res_count);
void qb_do_set_not_equal_I08(int8_t *op1_ptr, uint32_t op1_count, int8_t *op2_ptr, uint32_t op2_count, int32_t *res_ptr, uint32_t res_count);
void qb_do_set_not_equal_I16(int16_t *op1_ptr, uint32_t op1_count, int16_t *op2_ptr, uint32_t op2_count, int32_t *res_ptr, uint32_t res_count);
void qb_do_set_not_equal_I32(int32_t *op1_ptr, uint32_t op1_count, int32_t *op2_ptr, uint32_t op2_count, int32_t *res_ptr, uint32_t res_count);
void qb_do_set_not_equal_I64(int64_t *op1_ptr, uint32_t op1_count, int64_t *op2_ptr, uint32_t op2_count, int32_t *res_ptr, uint32_t res_count);
void qb_do_shift_left_multiple_times_S08(int8_t *op1_ptr, uint32_t op1_count, int8_t *op2_ptr, uint32_t op2_count, int8_t *res_ptr, uint32_t res_count);
void qb_do_shift_left_multiple_times_S16(int16_t *op1_ptr, uint32_t op1_count, int16_t *op2_ptr, uint32_t op2_count, int16_t *res_ptr, uint32_t res_count);
void qb_do_shift_left_multiple_times_S32(int32_t *op1_ptr, uint32_t op1_count, int32_t *op2_ptr, uint32_t op2_count, int32_t *res_ptr, uint32_t res_count);
void qb_do_shift_left_multiple_times_S64(int64_t *op1_ptr, uint32_t op1_count, int64_t *op2_ptr, uint32_t op2_count, int64_t *res_ptr, uint32_t res_count);
void qb_do_shift_left_multiple_times_U08(uint8_t *op1_ptr, uint32_t op1_count, uint8_t *op2_ptr, uint32_t op2_count, uint8_t *res_ptr, uint32_t res_count);
void qb_do_shift_left_multiple_times_U16(uint16_t *op1_ptr, uint32_t op1_count, uint16_t *op2_ptr, uint32_t op2_count, uint16_t *res_ptr, uint32_t res_count);
void qb_do_shift_left_multiple_times_U32(uint32_t *op1_ptr, uint32_t op1_count, uint32_t *op2_ptr, uint32_t op2_count, uint32_t *res_ptr, uint32_t res_count);
void qb_do_shift_left_multiple_times_U64(uint64_t *op1_ptr, uint32_t op1_count, uint64_t *op2_ptr, uint32_t op2_count, uint64_t *res_ptr, uint32_t res_count);
void qb_do_shift_right_multiple_times_S08(int8_t *op1_ptr, uint32_t op1_count, int8_t *op2_ptr, uint32_t op2_count, int8_t *res_ptr, uint32_t res_count);
void qb_do_shift_right_multiple_times_S16(int16_t *op1_ptr, uint32_t op1_count, int16_t *op2_ptr, uint32_t op2_count, int16_t *res_ptr, uint32_t res_count);
void qb_do_shift_right_multiple_times_S32(int32_t *op1_ptr, uint32_t op1_count, int32_t *op2_ptr, uint32_t op2_count, int32_t *res_ptr, uint32_t res_count);
void qb_do_shift_right_multiple_times_S64(int64_t *op1_ptr, uint32_t op1_count, int64_t *op2_ptr, uint32_t op2_count, int64_t *res_ptr, uint32_t res_count);
void qb_do_shift_right_multiple_times_U08(uint8_t *op1_ptr, uint32_t op1_count, uint8_t *op2_ptr, uint32_t op2_count, uint8_t *res_ptr, uint32_t res_count);
void qb_do_shift_right_multiple_times_U16(uint16_t *op1_ptr, uint32_t op1_count, uint16_t *op2_ptr, uint32_t op2_count, uint16_t *res_ptr, uint32_t res_count);
void qb_do_shift_right_multiple_times_U32(uint32_t *op1_ptr, uint32_t op1_count, uint32_t *op2_ptr, uint32_t op2_count, uint32_t *res_ptr, uint32_t res_count);
void qb_do_shift_right_multiple_times_U64(uint64_t *op1_ptr, uint32_t op1_count, uint64_t *op2_ptr, uint32_t op2_count, uint64_t *res_ptr, uint32_t res_count);
void qb_do_shuffle_F32(qb_interpreter_context *__restrict cxt, uint32_t op1, float32_t *res_ptr, uint32_t res_count);
void qb_do_shuffle_F64(qb_interpreter_context *__restrict cxt, uint32_t op1, float64_t *res_ptr, uint32_t res_count);
void qb_do_shuffle_I08(qb_interpreter_context *__restrict cxt, uint32_t op1, int8_t *res_ptr, uint32_t res_count);
void qb_do_shuffle_I16(qb_interpreter_context *__restrict cxt, uint32_t op1, int16_t *res_ptr, uint32_t res_count);
void qb_do_shuffle_I32(qb_interpreter_context *__restrict cxt, uint32_t op1, int32_t *res_ptr, uint32_t res_count);
void qb_do_shuffle_I64(qb_interpreter_context *__restrict cxt, uint32_t op1, int64_t *res_ptr, uint32_t res_count);
void qb_do_sign_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count);
void qb_do_sign_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void qb_do_sin_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count);
void qb_do_sin_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void qb_do_sinh_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count);
void qb_do_sinh_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void qb_do_smooth_step_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, float32_t *op3_ptr, uint32_t op3_count, float32_t *res_ptr, uint32_t res_count);
void qb_do_smooth_step_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, float64_t *op3_ptr, uint32_t op3_count, float64_t *res_ptr, uint32_t res_count);
void qb_do_sort_F32(uint32_t op1, float32_t *res_ptr, uint32_t res_count);
void qb_do_sort_F64(uint32_t op1, float64_t *res_ptr, uint32_t res_count);
void qb_do_sort_S08(uint32_t op1, int8_t *res_ptr, uint32_t res_count);
void qb_do_sort_S16(uint32_t op1, int16_t *res_ptr, uint32_t res_count);
void qb_do_sort_S32(uint32_t op1, int32_t *res_ptr, uint32_t res_count);
void qb_do_sort_S64(uint32_t op1, int64_t *res_ptr, uint32_t res_count);
void qb_do_sort_U08(uint32_t op1, uint8_t *res_ptr, uint32_t res_count);
void qb_do_sort_U16(uint32_t op1, uint16_t *res_ptr, uint32_t res_count);
void qb_do_sort_U32(uint32_t op1, uint32_t *res_ptr, uint32_t res_count);
void qb_do_sort_U64(uint32_t op1, uint64_t *res_ptr, uint32_t res_count);
void qb_do_sqrt_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count);
void qb_do_sqrt_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void qb_do_step_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, float32_t *res_ptr, uint32_t res_count);
void qb_do_step_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, float64_t *res_ptr, uint32_t res_count);
void qb_do_subarray_position_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, int32_t op3, int32_t *res_ptr);
void qb_do_subarray_position_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, int32_t op3, int32_t *res_ptr);
void qb_do_subarray_position_I08(int8_t *op1_ptr, uint32_t op1_count, int8_t *op2_ptr, uint32_t op2_count, int32_t op3, int32_t *res_ptr);
void qb_do_subarray_position_I16(int16_t *op1_ptr, uint32_t op1_count, int16_t *op2_ptr, uint32_t op2_count, int32_t op3, int32_t *res_ptr);
void qb_do_subarray_position_I32(int32_t *op1_ptr, uint32_t op1_count, int32_t *op2_ptr, uint32_t op2_count, int32_t op3, int32_t *res_ptr);
void qb_do_subarray_position_I64(int64_t *op1_ptr, uint32_t op1_count, int64_t *op2_ptr, uint32_t op2_count, int32_t op3, int32_t *res_ptr);
void qb_do_subarray_position_from_end_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, int32_t op3, int32_t *res_ptr);
void qb_do_subarray_position_from_end_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, int32_t op3, int32_t *res_ptr);
void qb_do_subarray_position_from_end_I08(int8_t *op1_ptr, uint32_t op1_count, int8_t *op2_ptr, uint32_t op2_count, int32_t op3, int32_t *res_ptr);
void qb_do_subarray_position_from_end_I16(int16_t *op1_ptr, uint32_t op1_count, int16_t *op2_ptr, uint32_t op2_count, int32_t op3, int32_t *res_ptr);
void qb_do_subarray_position_from_end_I32(int32_t *op1_ptr, uint32_t op1_count, int32_t *op2_ptr, uint32_t op2_count, int32_t op3, int32_t *res_ptr);
void qb_do_subarray_position_from_end_I64(int64_t *op1_ptr, uint32_t op1_count, int64_t *op2_ptr, uint32_t op2_count, int32_t op3, int32_t *res_ptr);
void qb_do_subtract_2x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, float32_t *res_ptr, uint32_t res_count);
void qb_do_subtract_2x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, float64_t *res_ptr, uint32_t res_count);
void qb_do_subtract_3x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, float32_t *res_ptr, uint32_t res_count);
void qb_do_subtract_3x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, float64_t *res_ptr, uint32_t res_count);
void qb_do_subtract_4x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, float32_t *res_ptr, uint32_t res_count);
void qb_do_subtract_4x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, float64_t *res_ptr, uint32_t res_count);
void qb_do_subtract_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, float32_t *res_ptr, uint32_t res_count);
void qb_do_subtract_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, float64_t *res_ptr, uint32_t res_count);
void qb_do_subtract_multiple_times_I08(int8_t *op1_ptr, uint32_t op1_count, int8_t *op2_ptr, uint32_t op2_count, int8_t *res_ptr, uint32_t res_count);
void qb_do_subtract_multiple_times_I16(int16_t *op1_ptr, uint32_t op1_count, int16_t *op2_ptr, uint32_t op2_count, int16_t *res_ptr, uint32_t res_count);
void qb_do_subtract_multiple_times_I32(int32_t *op1_ptr, uint32_t op1_count, int32_t *op2_ptr, uint32_t op2_count, int32_t *res_ptr, uint32_t res_count);
void qb_do_subtract_multiple_times_I64(int64_t *op1_ptr, uint32_t op1_count, int64_t *op2_ptr, uint32_t op2_count, int64_t *res_ptr, uint32_t res_count);
void qb_do_synchronize_shadow_variable_U32(qb_interpreter_context *__restrict cxt, uint32_t op1);
void qb_do_tan_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count);
void qb_do_tan_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void qb_do_tanh_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count);
void qb_do_tanh_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void qb_do_transform_vector_2x_multiple_times_column_major_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, float32_t *res_ptr, uint32_t res_count);
void qb_do_transform_vector_2x_multiple_times_column_major_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, float64_t *res_ptr, uint32_t res_count);
void qb_do_transform_vector_2x_multiple_times_row_major_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, float32_t *res_ptr, uint32_t res_count);
void qb_do_transform_vector_2x_multiple_times_row_major_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, float64_t *res_ptr, uint32_t res_count);
void qb_do_transform_vector_3x_multiple_times_column_major_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, float32_t *res_ptr, uint32_t res_count);
void qb_do_transform_vector_3x_multiple_times_column_major_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, float64_t *res_ptr, uint32_t res_count);
void qb_do_transform_vector_3x_multiple_times_row_major_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, float32_t *res_ptr, uint32_t res_count);
void qb_do_transform_vector_3x_multiple_times_row_major_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, float64_t *res_ptr, uint32_t res_count);
void qb_do_transform_vector_4x_multiple_times_column_major_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, float32_t *res_ptr, uint32_t res_count);
void qb_do_transform_vector_4x_multiple_times_column_major_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, float64_t *res_ptr, uint32_t res_count);
void qb_do_transform_vector_4x_multiple_times_row_major_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, float32_t *res_ptr, uint32_t res_count);
void qb_do_transform_vector_4x_multiple_times_row_major_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, float64_t *res_ptr, uint32_t res_count);
void qb_do_transpose_matrix_2x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count);
void qb_do_transpose_matrix_2x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void qb_do_transpose_matrix_3x_F32(float32_t *op1_ptr, float32_t *res_ptr);
void qb_do_transpose_matrix_3x_F64(float64_t *op1_ptr, float64_t *res_ptr);
void qb_do_transpose_matrix_3x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count);
void qb_do_transpose_matrix_3x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void qb_do_transpose_matrix_4x_F32(float32_t *op1_ptr, float32_t *res_ptr);
void qb_do_transpose_matrix_4x_F64(float64_t *op1_ptr, float64_t *res_ptr);
void qb_do_transpose_matrix_4x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count);
void qb_do_transpose_matrix_4x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void qb_do_transpose_matrix_F32(float32_t *op1_ptr, uint32_t op1_count, uint32_t op2, uint32_t op3, float32_t *res_ptr, uint32_t res_count);
void qb_do_transpose_matrix_F64(float64_t *op1_ptr, uint32_t op1_count, uint32_t op2, uint32_t op3, float64_t *res_ptr, uint32_t res_count);
void qb_do_transpose_matrix_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, uint32_t op2, uint32_t op3, float32_t *res_ptr, uint32_t res_count);
void qb_do_transpose_matrix_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, uint32_t op2, uint32_t op3, float64_t *res_ptr, uint32_t res_count);
void qb_do_utf8decode_U16(uint8_t *op1_ptr, uint32_t op1_count, uint16_t *res_ptr, uint32_t res_count);
void qb_do_utf8decode_U32(uint8_t *op1_ptr, uint32_t op1_count, uint32_t *res_ptr, uint32_t res_count);
void qb_do_utf8decode_count_U16(uint8_t *op1_ptr, uint32_t op1_count, uint32_t *res_ptr);
void qb_do_utf8encode_U16(uint16_t *op1_ptr, uint32_t op1_count, uint8_t *res_ptr, uint32_t res_count);
void qb_do_utf8encode_U32(uint32_t *op1_ptr, uint32_t op1_count, uint8_t *res_ptr, uint32_t res_count);
void qb_do_utf8encode_count_U16(uint16_t *op1_ptr, uint32_t op1_count, uint32_t *res_ptr);
void qb_do_utf8encode_count_U32(uint32_t *op1_ptr, uint32_t op1_count, uint32_t *res_ptr);
void qb_redirect_multiply_multiple_times_U32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_multiply_accumulate_multiple_times_U32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_shift_left_multiple_times_U32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_shift_right_multiple_times_U32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_increment_multiple_times_I32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_decrement_multiple_times_I32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_add_multiple_times_I32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_multiply_multiple_times_S32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_multiply_accumulate_multiple_times_S32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_subtract_multiple_times_I32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_negate_multiple_times_I32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_bitwise_and_multiple_times_I32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_bitwise_or_multiple_times_I32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_bitwise_xor_multiple_times_I32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_bitwise_not_multiple_times_I32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_shift_left_multiple_times_S32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_shift_right_multiple_times_S32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_abs_multiple_times_S32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_increment_multiple_times_F32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_decrement_multiple_times_F32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_add_multiple_times_F32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_multiply_multiple_times_F32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_multiply_accumulate_multiple_times_F32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_subtract_multiple_times_F32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_divide_multiple_times_F32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_modulo_multiple_times_F32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_floored_division_modulo_multiple_times_F32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_negate_multiple_times_F32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_abs_multiple_times_F32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_sin_multiple_times_F32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_asin_multiple_times_F32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_cos_multiple_times_F32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_acos_multiple_times_F32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_tan_multiple_times_F32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_atan_multiple_times_F32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_atan2_multiple_times_F32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_sinh_multiple_times_F32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_asinh_multiple_times_F32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_cosh_multiple_times_F32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_acosh_multiple_times_F32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_tanh_multiple_times_F32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_atanh_multiple_times_F32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_ceil_multiple_times_F32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_floor_multiple_times_F32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_rint_multiple_times_F32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_round_multiple_times_F32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_log_multiple_times_F32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_log1p_multiple_times_F32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_log2_multiple_times_F32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_log10_multiple_times_F32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_exp_multiple_times_F32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_exp_m1_multiple_times_F32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_exp2_multiple_times_F32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_pow_multiple_times_F32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_sqrt_multiple_times_F32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_hypot_multiple_times_F32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_is_finite_multiple_times_F32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_is_infinite_multiple_times_F32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_is_na_n_multiple_times_F32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_rsqrt_multiple_times_F32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_clamp_multiple_times_F32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_fract_multiple_times_F32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_mix_multiple_times_F32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_sign_multiple_times_F32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_step_multiple_times_F32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_smooth_step_multiple_times_F32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_radian_to_degree_multiple_times_F32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_degree_to_radian_multiple_times_F32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_sample_nearest_4x_multiple_times_F32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_sample_nearest_3x_multiple_times_F32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_sample_nearest_2x_multiple_times_F32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_sample_nearest_multiple_times_F32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_sample_bilinear_4x_multiple_times_F32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_sample_bilinear_3x_multiple_times_F32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_sample_bilinear_2x_multiple_times_F32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_sample_bilinear_multiple_times_F32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_alpha_blend_2x_multiple_times_F32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_alpha_blend_4x_multiple_times_F32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_apply_premultiplication_4x_multiple_times_F32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_remove_premultiplication_4x_multiple_times_F32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_rgb2hsv_3x_multiple_times_F32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_rgb2hsv_4x_multiple_times_F32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_hsv2rgb_3x_multiple_times_F32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_hsv2rgb_4x_multiple_times_F32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_rgb2hsl_3x_multiple_times_F32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_rgb2hsl_4x_multiple_times_F32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_hsl2rgb_3x_multiple_times_F32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_hsl2rgb_4x_multiple_times_F32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_multiply_matrix_by_matrix_4x_multiple_times_column_major_F32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_multiply_matrix_by_vector_4x_multiple_times_column_major_F32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_multiply_vector_by_matrix_4x_multiple_times_column_major_F32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_transpose_matrix_4x_multiple_times_F32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_invert_matrix_4x_multiple_times_F32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_determinant_4x_multiple_times_F32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_dot_product_4x_multiple_times_F32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_length_4x_multiple_times_F32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_distance_4x_multiple_times_F32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_normalize_4x_multiple_times_F32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_cross_product_4x_multiple_times_F32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_face_forward_4x_multiple_times_F32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_reflect_4x_multiple_times_F32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_refract_4x_multiple_times_F32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_transform_vector_4x_multiple_times_column_major_F32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_transform_vector_4x_multiple_times_row_major_F32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_add_4x_multiple_times_F32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_subtract_4x_multiple_times_F32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_multiply_4x_multiple_times_F32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_divide_4x_multiple_times_F32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_modulo_4x_multiple_times_F32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_negate_4x_multiple_times_F32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_multiply_accumulate_4x_multiple_times_F32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_multiply_matrix_by_matrix_3x_multiple_times_column_major_F32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_transpose_matrix_3x_multiple_times_F32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_invert_matrix_3x_multiple_times_F32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_determinant_3x_multiple_times_F32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_dot_product_3x_multiple_times_F32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_length_3x_multiple_times_F32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_distance_3x_multiple_times_F32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_normalize_3x_multiple_times_F32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_cross_product_3x_multiple_times_F32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_face_forward_3x_multiple_times_F32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_reflect_3x_multiple_times_F32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_refract_3x_multiple_times_F32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_transform_vector_3x_multiple_times_column_major_F32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_transform_vector_3x_multiple_times_row_major_F32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_add_3x_multiple_times_F32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_subtract_3x_multiple_times_F32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_multiply_3x_multiple_times_F32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_divide_3x_multiple_times_F32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_modulo_3x_multiple_times_F32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_negate_3x_multiple_times_F32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_multiply_accumulate_3x_multiple_times_F32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_multiply_matrix_by_matrix_2x_multiple_times_column_major_F32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_transpose_matrix_2x_multiple_times_F32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_invert_matrix_2x_multiple_times_F32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_determinant_2x_multiple_times_F32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_dot_product_2x_multiple_times_F32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_length_2x_multiple_times_F32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_distance_2x_multiple_times_F32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_normalize_2x_multiple_times_F32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_cross_product_2x_multiple_times_F32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_face_forward_2x_multiple_times_F32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_reflect_2x_multiple_times_F32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_refract_2x_multiple_times_F32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_transform_vector_2x_multiple_times_column_major_F32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_transform_vector_2x_multiple_times_row_major_F32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_add_2x_multiple_times_F32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_subtract_2x_multiple_times_F32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_multiply_2x_multiple_times_F32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_divide_2x_multiple_times_F32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_modulo_2x_multiple_times_F32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_negate_2x_multiple_times_F32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_multiply_accumulate_2x_multiple_times_F32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_complex_abs_2x_multiple_times_F32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_complex_argument_2x_multiple_times_F32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_complex_multiply_2x_multiple_times_F32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_complex_divide_2x_multiple_times_F32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_complex_exp_2x_multiple_times_F32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_complex_log_2x_multiple_times_F32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_complex_square_root_2x_multiple_times_F32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_complex_pow_2x_multiple_times_F32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_complex_sin_2x_multiple_times_F32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_complex_cos_2x_multiple_times_F32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_complex_tan_2x_multiple_times_F32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_complex_sinh_2x_multiple_times_F32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_complex_cosh_2x_multiple_times_F32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_complex_tanh_2x_multiple_times_F32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_increment_multiple_times_F64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_decrement_multiple_times_F64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_add_multiple_times_F64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_multiply_multiple_times_F64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_multiply_accumulate_multiple_times_F64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_subtract_multiple_times_F64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_divide_multiple_times_F64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_modulo_multiple_times_F64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_floored_division_modulo_multiple_times_F64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_negate_multiple_times_F64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_abs_multiple_times_F64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_sin_multiple_times_F64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_asin_multiple_times_F64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_cos_multiple_times_F64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_acos_multiple_times_F64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_tan_multiple_times_F64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_atan_multiple_times_F64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_atan2_multiple_times_F64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_sinh_multiple_times_F64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_asinh_multiple_times_F64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_cosh_multiple_times_F64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_acosh_multiple_times_F64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_tanh_multiple_times_F64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_atanh_multiple_times_F64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_ceil_multiple_times_F64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_floor_multiple_times_F64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_rint_multiple_times_F64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_round_multiple_times_F64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_log_multiple_times_F64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_log1p_multiple_times_F64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_log2_multiple_times_F64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_log10_multiple_times_F64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_exp_multiple_times_F64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_exp_m1_multiple_times_F64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_exp2_multiple_times_F64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_pow_multiple_times_F64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_sqrt_multiple_times_F64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_hypot_multiple_times_F64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_is_finite_multiple_times_F64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_is_infinite_multiple_times_F64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_is_na_n_multiple_times_F64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_rsqrt_multiple_times_F64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_clamp_multiple_times_F64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_fract_multiple_times_F64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_mix_multiple_times_F64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_sign_multiple_times_F64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_step_multiple_times_F64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_smooth_step_multiple_times_F64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_radian_to_degree_multiple_times_F64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_degree_to_radian_multiple_times_F64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_sample_nearest_4x_multiple_times_F64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_sample_nearest_3x_multiple_times_F64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_sample_nearest_2x_multiple_times_F64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_sample_nearest_multiple_times_F64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_sample_bilinear_4x_multiple_times_F64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_sample_bilinear_3x_multiple_times_F64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_sample_bilinear_2x_multiple_times_F64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_sample_bilinear_multiple_times_F64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_alpha_blend_2x_multiple_times_F64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_alpha_blend_4x_multiple_times_F64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_apply_premultiplication_4x_multiple_times_F64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_remove_premultiplication_4x_multiple_times_F64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_rgb2hsv_3x_multiple_times_F64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_rgb2hsv_4x_multiple_times_F64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_hsv2rgb_3x_multiple_times_F64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_hsv2rgb_4x_multiple_times_F64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_rgb2hsl_3x_multiple_times_F64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_rgb2hsl_4x_multiple_times_F64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_hsl2rgb_3x_multiple_times_F64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_hsl2rgb_4x_multiple_times_F64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_multiply_matrix_by_matrix_4x_multiple_times_column_major_F64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_multiply_matrix_by_vector_4x_multiple_times_column_major_F64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_multiply_vector_by_matrix_4x_multiple_times_column_major_F64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_transpose_matrix_4x_multiple_times_F64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_invert_matrix_4x_multiple_times_F64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_determinant_4x_multiple_times_F64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_dot_product_4x_multiple_times_F64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_length_4x_multiple_times_F64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_distance_4x_multiple_times_F64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_normalize_4x_multiple_times_F64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_cross_product_4x_multiple_times_F64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_face_forward_4x_multiple_times_F64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_reflect_4x_multiple_times_F64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_refract_4x_multiple_times_F64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_transform_vector_4x_multiple_times_column_major_F64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_transform_vector_4x_multiple_times_row_major_F64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_add_4x_multiple_times_F64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_subtract_4x_multiple_times_F64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_multiply_4x_multiple_times_F64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_divide_4x_multiple_times_F64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_modulo_4x_multiple_times_F64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_negate_4x_multiple_times_F64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_multiply_accumulate_4x_multiple_times_F64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_multiply_matrix_by_matrix_3x_multiple_times_column_major_F64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_multiply_matrix_by_vector_3x_multiple_times_column_major_F64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_multiply_vector_by_matrix_3x_multiple_times_column_major_F64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_transpose_matrix_3x_multiple_times_F64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_invert_matrix_3x_multiple_times_F64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_determinant_3x_multiple_times_F64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_dot_product_3x_multiple_times_F64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_length_3x_multiple_times_F64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_distance_3x_multiple_times_F64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_normalize_3x_multiple_times_F64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_cross_product_3x_multiple_times_F64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_face_forward_3x_multiple_times_F64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_reflect_3x_multiple_times_F64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_refract_3x_multiple_times_F64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_transform_vector_3x_multiple_times_column_major_F64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_transform_vector_3x_multiple_times_row_major_F64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_add_3x_multiple_times_F64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_subtract_3x_multiple_times_F64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_multiply_3x_multiple_times_F64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_divide_3x_multiple_times_F64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_modulo_3x_multiple_times_F64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_negate_3x_multiple_times_F64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_multiply_accumulate_3x_multiple_times_F64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_multiply_matrix_by_matrix_2x_multiple_times_column_major_F64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_multiply_matrix_by_vector_2x_multiple_times_column_major_F64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_multiply_vector_by_matrix_2x_multiple_times_column_major_F64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_transpose_matrix_2x_multiple_times_F64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_invert_matrix_2x_multiple_times_F64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_determinant_2x_multiple_times_F64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_dot_product_2x_multiple_times_F64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_length_2x_multiple_times_F64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_distance_2x_multiple_times_F64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_normalize_2x_multiple_times_F64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_cross_product_2x_multiple_times_F64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_face_forward_2x_multiple_times_F64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_reflect_2x_multiple_times_F64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_refract_2x_multiple_times_F64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_transform_vector_2x_multiple_times_column_major_F64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_transform_vector_2x_multiple_times_row_major_F64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_add_2x_multiple_times_F64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_subtract_2x_multiple_times_F64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_multiply_2x_multiple_times_F64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_divide_2x_multiple_times_F64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_modulo_2x_multiple_times_F64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_negate_2x_multiple_times_F64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_multiply_accumulate_2x_multiple_times_F64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_complex_abs_2x_multiple_times_F64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_complex_argument_2x_multiple_times_F64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_complex_multiply_2x_multiple_times_F64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_complex_divide_2x_multiple_times_F64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_complex_exp_2x_multiple_times_F64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_complex_log_2x_multiple_times_F64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_complex_square_root_2x_multiple_times_F64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_complex_pow_2x_multiple_times_F64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_complex_sin_2x_multiple_times_F64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_complex_cos_2x_multiple_times_F64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_complex_tan_2x_multiple_times_F64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_complex_sinh_2x_multiple_times_F64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_complex_cosh_2x_multiple_times_F64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_complex_tanh_2x_multiple_times_F64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_increment_multiple_times_I08(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_decrement_multiple_times_I08(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_add_multiple_times_I08(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_multiply_multiple_times_S08(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_subtract_multiple_times_I08(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_negate_multiple_times_I08(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_bitwise_and_multiple_times_I08(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_bitwise_or_multiple_times_I08(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_bitwise_xor_multiple_times_I08(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_bitwise_not_multiple_times_I08(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_shift_left_multiple_times_S08(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_shift_right_multiple_times_S08(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_abs_multiple_times_S08(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_multiply_multiple_times_U08(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_shift_left_multiple_times_U08(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_shift_right_multiple_times_U08(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_increment_multiple_times_I16(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_decrement_multiple_times_I16(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_add_multiple_times_I16(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_multiply_multiple_times_S16(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_subtract_multiple_times_I16(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_negate_multiple_times_I16(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_bitwise_and_multiple_times_I16(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_bitwise_or_multiple_times_I16(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_bitwise_xor_multiple_times_I16(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_bitwise_not_multiple_times_I16(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_shift_left_multiple_times_S16(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_shift_right_multiple_times_S16(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_abs_multiple_times_S16(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_multiply_multiple_times_U16(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_shift_left_multiple_times_U16(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_shift_right_multiple_times_U16(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_increment_multiple_times_I64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_decrement_multiple_times_I64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_add_multiple_times_I64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_multiply_multiple_times_S64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_multiply_accumulate_multiple_times_S64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_subtract_multiple_times_I64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_negate_multiple_times_I64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_bitwise_and_multiple_times_I64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_bitwise_or_multiple_times_I64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_bitwise_xor_multiple_times_I64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_bitwise_not_multiple_times_I64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_shift_left_multiple_times_S64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_shift_right_multiple_times_S64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_abs_multiple_times_S64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_multiply_multiple_times_U64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_multiply_accumulate_multiple_times_U64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_shift_left_multiple_times_U64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_shift_right_multiple_times_U64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_print_variable_U32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_print_variable_array_element_U32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_print_variable_multiple_times_U32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_print_multidimensional_variable_U32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_print_variable_S32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_print_variable_array_element_S32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_print_variable_multiple_times_S32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_print_multidimensional_variable_S32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_print_variable_F32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_print_variable_array_element_F32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_print_variable_multiple_times_F32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_print_multidimensional_variable_F32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_print_variable_F64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_print_variable_array_element_F64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_print_variable_multiple_times_F64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_print_multidimensional_variable_F64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_print_variable_S08(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_print_variable_array_element_S08(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_print_variable_multiple_times_S08(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_print_multidimensional_variable_S08(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_print_string_U08(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_print_variable_U08(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_print_variable_array_element_U08(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_print_variable_multiple_times_U08(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_print_multidimensional_variable_U08(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_print_variable_S16(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_print_variable_array_element_S16(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_print_variable_multiple_times_S16(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_print_multidimensional_variable_S16(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_print_variable_U16(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_print_variable_array_element_U16(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_print_variable_multiple_times_U16(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_print_multidimensional_variable_U16(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_print_variable_S64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_print_variable_array_element_S64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_print_variable_multiple_times_S64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_print_multidimensional_variable_S64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_print_variable_U64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_print_variable_array_element_U64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_print_variable_multiple_times_U64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_print_multidimensional_variable_U64(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_extension_op_U32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
void qb_redirect_synchronize_shadow_variable_U32(qb_interpreter_context *__restrict cxt, int8_t *__restrict ip, int unused);
int32_t qb_dispatch_instruction_ARR(qb_interpreter_context *__restrict cxt, void *control_func, qb_instruction_ARR *__restrict instr, uint32_t operand1_size, uint32_t threshold);
int32_t qb_dispatch_instruction_ARR_ARR(qb_interpreter_context *__restrict cxt, void *control_func, qb_instruction_ARR_ARR *__restrict instr, uint32_t operand1_size, uint32_t operand2_size, uint32_t threshold);
int32_t qb_dispatch_instruction_ARR_ARR_ARR(qb_interpreter_context *__restrict cxt, void *control_func, qb_instruction_ARR_ARR_ARR *__restrict instr, uint32_t operand1_size, uint32_t operand2_size, uint32_t operand3_size, uint32_t threshold);
int32_t qb_dispatch_instruction_ARR_ARR_ARR_ARR(qb_interpreter_context *__restrict cxt, void *control_func, qb_instruction_ARR_ARR_ARR_ARR *__restrict instr, uint32_t operand1_size, uint32_t operand2_size, uint32_t operand3_size, uint32_t operand4_size, uint32_t threshold);
int32_t qb_dispatch_instruction_ARR_ARR_SCA_ARR(qb_interpreter_context *__restrict cxt, void *control_func, qb_instruction_ARR_ARR_SCA_ARR *__restrict instr, uint32_t operand1_size, uint32_t operand2_size, uint32_t operand4_size, uint32_t threshold);
int32_t qb_dispatch_instruction_ARR_SCA_SCA_ARR_ARR_ARR(qb_interpreter_context *__restrict cxt, void *control_func, qb_instruction_ARR_SCA_SCA_ARR_ARR_ARR *__restrict instr, uint32_t operand1_size, uint32_t operand4_size, uint32_t operand5_size, uint32_t operand6_size, uint32_t threshold);

static zend_always_inline int32_t qb_do_check_index_add_U32(qb_interpreter_context *__restrict cxt, uint32_t op1, uint32_t op2, uint32_t op3, int32_t *op4_ptr, uint32_t *res_ptr, uint32_t line_id) {
	(*res_ptr) = op1 + op3;
	if(UNEXPECTED(!(op1 < op2))) {
		(*op4_ptr) = FALSE;
	}
	return TRUE;
}

static zend_always_inline int32_t qb_do_check_index_multiply_U32(qb_interpreter_context *__restrict cxt, uint32_t op1, uint32_t op2, uint32_t op3, int32_t *op4_ptr, uint32_t *res_ptr, uint32_t line_id) {
	(*res_ptr) = op1 * op3;
	if(UNEXPECTED(!(op1 < op2))) {
		(*op4_ptr) = FALSE;
	}
	return TRUE;
}

static zend_always_inline int32_t qb_do_check_index_multiply_add_U32(qb_interpreter_context *__restrict cxt, uint32_t op1, uint32_t op2, uint32_t op3, uint32_t op4, int32_t *op5_ptr, uint32_t *res_ptr, uint32_t line_id) {
	(*res_ptr) = op1 * op3 + op4;
	if(UNEXPECTED(!(op1 < op2))) {
		(*op5_ptr) = FALSE;
	}
	return TRUE;
}

static zend_always_inline int32_t qb_do_divide_S08(qb_interpreter_context *__restrict cxt, int8_t op1, int8_t op2, int8_t *res_ptr, uint32_t line_id) {
	if(UNEXPECTED(op2 == 0)) {
		qb_report_divide_by_zero_exception(line_id);
		cxt->exit_type = QB_VM_ERROR;
		return FALSE;
	}
	(*res_ptr) = op1 / op2;
	return TRUE;
}

static zend_always_inline int32_t qb_do_divide_S16(qb_interpreter_context *__restrict cxt, int16_t op1, int16_t op2, int16_t *res_ptr, uint32_t line_id) {
	if(UNEXPECTED(op2 == 0)) {
		qb_report_divide_by_zero_exception(line_id);
		cxt->exit_type = QB_VM_ERROR;
		return FALSE;
	}
	(*res_ptr) = op1 / op2;
	return TRUE;
}

static zend_always_inline int32_t qb_do_divide_S32(qb_interpreter_context *__restrict cxt, int32_t op1, int32_t op2, int32_t *res_ptr, uint32_t line_id) {
	if(UNEXPECTED(op2 == 0)) {
		qb_report_divide_by_zero_exception(line_id);
		cxt->exit_type = QB_VM_ERROR;
		return FALSE;
	}
	(*res_ptr) = op1 / op2;
	return TRUE;
}

static zend_always_inline int32_t qb_do_divide_S64(qb_interpreter_context *__restrict cxt, int64_t op1, int64_t op2, int64_t *res_ptr, uint32_t line_id) {
	if(UNEXPECTED(op2 == 0)) {
		qb_report_divide_by_zero_exception(line_id);
		cxt->exit_type = QB_VM_ERROR;
		return FALSE;
	}
	(*res_ptr) = op1 / op2;
	return TRUE;
}

static zend_always_inline int32_t qb_do_divide_U08(qb_interpreter_context *__restrict cxt, uint8_t op1, uint8_t op2, uint8_t *res_ptr, uint32_t line_id) {
	if(UNEXPECTED(op2 == 0)) {
		qb_report_divide_by_zero_exception(line_id);
		cxt->exit_type = QB_VM_ERROR;
		return FALSE;
	}
	(*res_ptr) = op1 / op2;
	return TRUE;
}

static zend_always_inline int32_t qb_do_divide_U16(qb_interpreter_context *__restrict cxt, uint16_t op1, uint16_t op2, uint16_t *res_ptr, uint32_t line_id) {
	if(UNEXPECTED(op2 == 0)) {
		qb_report_divide_by_zero_exception(line_id);
		cxt->exit_type = QB_VM_ERROR;
		return FALSE;
	}
	(*res_ptr) = op1 / op2;
	return TRUE;
}

static zend_always_inline int32_t qb_do_divide_U32(qb_interpreter_context *__restrict cxt, uint32_t op1, uint32_t op2, uint32_t *res_ptr, uint32_t line_id) {
	if(UNEXPECTED(op2 == 0)) {
		qb_report_divide_by_zero_exception(line_id);
		cxt->exit_type = QB_VM_ERROR;
		return FALSE;
	}
	(*res_ptr) = op1 / op2;
	return TRUE;
}

static zend_always_inline int32_t qb_do_divide_U64(qb_interpreter_context *__restrict cxt, uint64_t op1, uint64_t op2, uint64_t *res_ptr, uint32_t line_id) {
	if(UNEXPECTED(op2 == 0)) {
		qb_report_divide_by_zero_exception(line_id);
		cxt->exit_type = QB_VM_ERROR;
		return FALSE;
	}
	(*res_ptr) = op1 / op2;
	return TRUE;
}

static zend_always_inline int32_t qb_do_guard_extent_U32(qb_interpreter_context *__restrict cxt, uint32_t op1, uint32_t op2, uint32_t op3, uint32_t line_id) {
	if(UNEXPECTED(!(op1 + op2 <= op3))) {
	}
	return TRUE;
}

static zend_always_inline int32_t qb_do_guard_extent_multiply_U32(qb_interpreter_context *__restrict cxt, uint32_t op1, uint32_t op2, uint32_t op3, uint32_t op4, uint32_t *res_ptr, uint32_t line_id) {
	(*res_ptr) = op2 * op4;
	if(UNEXPECTED(!(op1 + op2 <= op3))) {
	}
	return TRUE;
}

static zend_always_inline int32_t qb_do_guard_extent_subtract_U32(qb_interpreter_context *__restrict cxt, uint32_t op1, uint32_t op2, uint32_t *res_ptr, uint32_t line_id) {
	(*res_ptr) = op1 - op2;
	if(UNEXPECTED(!(op2 <= op1))) {
	}
	return TRUE;
}

static zend_always_inline int32_t qb_do_guard_extent_subtract_multiply_U32(qb_interpreter_context *__restrict cxt, uint32_t op1, uint32_t op2, uint32_t op3, uint32_t *res_ptr, uint32_t line_id) {
	(*res_ptr) = (op1 - op2) * op3;
	if(UNEXPECTED(!(op2 <= op1))) {
	}
	return TRUE;
}

static zend_always_inline int32_t qb_do_guard_index_U32(qb_interpreter_context *__restrict cxt, uint32_t op1, uint32_t op2, uint32_t line_id) {
	if(UNEXPECTED(!(op1 < op2))) {
		qb_report_out_of_bound_exception(line_id, op1, op2, FALSE);
		cxt->exit_type = QB_VM_ERROR;
		return FALSE;
	}
	return TRUE;
}

static zend_always_inline int32_t qb_do_guard_index_add_U32(qb_interpreter_context *__restrict cxt, uint32_t op1, uint32_t op2, uint32_t op3, uint32_t *res_ptr, uint32_t line_id) {
	(*res_ptr) = op1 + op3;
	if(UNEXPECTED(!(op1 < op2))) {
		qb_report_out_of_bound_exception(line_id, op1, op2, FALSE);
		cxt->exit_type = QB_VM_ERROR;
		return FALSE;
	}
	return TRUE;
}

static zend_always_inline int32_t qb_do_guard_index_multiply_U32(qb_interpreter_context *__restrict cxt, uint32_t op1, uint32_t op2, uint32_t op3, uint32_t *res_ptr, uint32_t line_id) {
	(*res_ptr) = op1 * op3;
	if(UNEXPECTED(!(op1 < op2))) {
		qb_report_out_of_bound_exception(line_id, op1, op2, FALSE);
		cxt->exit_type = QB_VM_ERROR;
		return FALSE;
	}
	return TRUE;
}

static zend_always_inline int32_t qb_do_guard_index_multiply_add_U32(qb_interpreter_context *__restrict cxt, uint32_t op1, uint32_t op2, uint32_t op3, uint32_t op4, uint32_t *res_ptr, uint32_t line_id) {
	(*res_ptr) = op1 * op3 + op4;
	if(UNEXPECTED(!(op1 < op2))) {
		qb_report_out_of_bound_exception(line_id, op1, op2, FALSE);
		cxt->exit_type = QB_VM_ERROR;
		return FALSE;
	}
	return TRUE;
}

static zend_always_inline int32_t qb_do_guard_size_U32(qb_interpreter_context *__restrict cxt, uint32_t op1, uint32_t op2, uint32_t line_id) {
	if(UNEXPECTED(!(op1 <= op2))) {
		qb_report_out_of_bound_exception(line_id, op1, op2, TRUE);
		cxt->exit_type = QB_VM_ERROR;
		return FALSE;
	}
	return TRUE;
}

static zend_always_inline int32_t qb_do_guard_size_exact_U32(qb_interpreter_context *__restrict cxt, uint32_t op1, uint32_t op2, uint32_t line_id) {
	if(UNEXPECTED(!(op1 == op2))) {
		qb_report_element_size_mismatch_exception(line_id, op1, op2);
		cxt->exit_type = QB_VM_ERROR;
		return FALSE;
	}
	return TRUE;
}

static zend_always_inline int32_t qb_do_modulo_S08(qb_interpreter_context *__restrict cxt, int8_t op1, int8_t op2, int8_t *res_ptr, uint32_t line_id) {
	if(UNEXPECTED(op2 == 0)) {
		qb_report_divide_by_zero_exception(line_id);
		cxt->exit_type = QB_VM_ERROR;
		return FALSE;
	}
	(*res_ptr) = op1 % op2;
	return TRUE;
}

static zend_always_inline int32_t qb_do_modulo_S16(qb_interpreter_context *__restrict cxt, int16_t op1, int16_t op2, int16_t *res_ptr, uint32_t line_id) {
	if(UNEXPECTED(op2 == 0)) {
		qb_report_divide_by_zero_exception(line_id);
		cxt->exit_type = QB_VM_ERROR;
		return FALSE;
	}
	(*res_ptr) = op1 % op2;
	return TRUE;
}

static zend_always_inline int32_t qb_do_modulo_S32(qb_interpreter_context *__restrict cxt, int32_t op1, int32_t op2, int32_t *res_ptr, uint32_t line_id) {
	if(UNEXPECTED(op2 == 0)) {
		qb_report_divide_by_zero_exception(line_id);
		cxt->exit_type = QB_VM_ERROR;
		return FALSE;
	}
	(*res_ptr) = op1 % op2;
	return TRUE;
}

static zend_always_inline int32_t qb_do_modulo_S64(qb_interpreter_context *__restrict cxt, int64_t op1, int64_t op2, int64_t *res_ptr, uint32_t line_id) {
	if(UNEXPECTED(op2 == 0)) {
		qb_report_divide_by_zero_exception(line_id);
		cxt->exit_type = QB_VM_ERROR;
		return FALSE;
	}
	(*res_ptr) = op1 % op2;
	return TRUE;
}

static zend_always_inline int32_t qb_do_modulo_U08(qb_interpreter_context *__restrict cxt, uint8_t op1, uint8_t op2, uint8_t *res_ptr, uint32_t line_id) {
	if(UNEXPECTED(op2 == 0)) {
		qb_report_divide_by_zero_exception(line_id);
		cxt->exit_type = QB_VM_ERROR;
		return FALSE;
	}
	(*res_ptr) = op1 % op2;
	return TRUE;
}

static zend_always_inline int32_t qb_do_modulo_U16(qb_interpreter_context *__restrict cxt, uint16_t op1, uint16_t op2, uint16_t *res_ptr, uint32_t line_id) {
	if(UNEXPECTED(op2 == 0)) {
		qb_report_divide_by_zero_exception(line_id);
		cxt->exit_type = QB_VM_ERROR;
		return FALSE;
	}
	(*res_ptr) = op1 % op2;
	return TRUE;
}

static zend_always_inline int32_t qb_do_modulo_U32(qb_interpreter_context *__restrict cxt, uint32_t op1, uint32_t op2, uint32_t *res_ptr, uint32_t line_id) {
	if(UNEXPECTED(op2 == 0)) {
		qb_report_divide_by_zero_exception(line_id);
		cxt->exit_type = QB_VM_ERROR;
		return FALSE;
	}
	(*res_ptr) = op1 % op2;
	return TRUE;
}

static zend_always_inline int32_t qb_do_modulo_U64(qb_interpreter_context *__restrict cxt, uint64_t op1, uint64_t op2, uint64_t *res_ptr, uint32_t line_id) {
	if(UNEXPECTED(op2 == 0)) {
		qb_report_divide_by_zero_exception(line_id);
		cxt->exit_type = QB_VM_ERROR;
		return FALSE;
	}
	(*res_ptr) = op1 % op2;
	return TRUE;
}

static zend_always_inline int32_t qb_do_validate2dcross_product_U32(qb_interpreter_context *__restrict cxt, uint32_t op1, uint32_t op2, uint32_t line_id) {
	if(op1 != 2 || op2 != 2) {
		qb_report_invalid_cross_product_exception(line_id, op1, op2);
		cxt->exit_type = QB_VM_ERROR;
		return FALSE;
	}
	return TRUE;
}

static zend_always_inline int32_t qb_do_validate3dcross_product_U32(qb_interpreter_context *__restrict cxt, uint32_t op1, uint32_t op2, uint32_t line_id) {
	if(op1 != 3 || op2 != 3) {
		qb_report_invalid_cross_product_exception(line_id, op1, op2);
		cxt->exit_type = QB_VM_ERROR;
		return FALSE;
	}
	return TRUE;
}

static zend_always_inline int32_t qb_do_validate4dcross_product_U32(qb_interpreter_context *__restrict cxt, uint32_t op1, uint32_t op2, uint32_t op3, uint32_t line_id) {
	if(op1 != 4 || op2 != 4 || op3 != 4) {
		qb_report_invalid_4d_cross_product_exception(line_id, op1, op2, op3);
		cxt->exit_type = QB_VM_ERROR;
		return FALSE;
	}
	return TRUE;
}

static zend_always_inline int32_t qb_do_validate_matrix_by_matrix_U32(qb_interpreter_context *__restrict cxt, uint32_t op1, uint32_t op2, uint32_t line_id) {
	if(op1 != op2) {
		qb_report_invalid_matrix_multiplication_exception(line_id, op1, op2, 1 | 2);
		cxt->exit_type = QB_VM_ERROR;
		return FALSE;
	}
	return TRUE;
}

static zend_always_inline int32_t qb_do_validate_matrix_by_vector_U32(qb_interpreter_context *__restrict cxt, uint32_t op1, uint32_t op2, uint32_t line_id) {
	if(op1 != op2) {
		qb_report_invalid_matrix_multiplication_exception(line_id, op1, op2, 1);
		cxt->exit_type = QB_VM_ERROR;
		return FALSE;
	}
	return TRUE;
}

static zend_always_inline int32_t qb_do_validate_square_matrix_U32(qb_interpreter_context *__restrict cxt, uint32_t op1, uint32_t op2, uint32_t line_id) {
	if(op1 != op2) {
		qb_report_not_square_matrix_exception(cxt->line_id, op1, op2);
		cxt->exit_type = QB_VM_ERROR;
		return FALSE;
	}
	return TRUE;
}

static zend_always_inline int32_t qb_do_validate_transform_U32(qb_interpreter_context *__restrict cxt, uint32_t op1, uint32_t op2, uint32_t op3, uint32_t line_id) {
	if(!(op3 >= 2 && op3 <= 4 && op1 == op3 + 1 && op2 == op3)) {
		qb_report_invalid_transform_exception(line_id, op1, op2, op3);
		cxt->exit_type = QB_VM_ERROR;
		return FALSE;
	}
	return TRUE;
}

static zend_always_inline int32_t qb_do_validate_vector_by_matrix_U32(qb_interpreter_context *__restrict cxt, uint32_t op1, uint32_t op2, uint32_t line_id) {
	if(op1 != op2) {
		qb_report_invalid_matrix_multiplication_exception(line_id, op1, op2, 2);
		cxt->exit_type = QB_VM_ERROR;
		return FALSE;
	}
	return TRUE;
}

static zend_always_inline int32_t qb_do_validate_vector_widths_U32(qb_interpreter_context *__restrict cxt, uint32_t op1, uint32_t op2, uint32_t line_id) {
	if(op1 != op2) {
		qb_report_vector_width_mismatch_exception(cxt->line_id, op1, op2);
		cxt->exit_type = QB_VM_ERROR;
		return FALSE;
	}
	return TRUE;
}

static zend_always_inline void qb_do_abs_F32(float32_t op1, float32_t *res_ptr) {
	(*res_ptr) = (float32_t) fabsf(op1);
}

static zend_always_inline void qb_do_abs_F64(float64_t op1, float64_t *res_ptr) {
	(*res_ptr) = (float64_t) fabs(op1);
}

static zend_always_inline void qb_do_abs_S08(int8_t op1, int8_t *res_ptr) {
	(*res_ptr) = (int8_t) abs(op1);
}

static zend_always_inline void qb_do_abs_S16(int16_t op1, int16_t *res_ptr) {
	(*res_ptr) = (int16_t) abs(op1);
}

static zend_always_inline void qb_do_abs_S32(int32_t op1, int32_t *res_ptr) {
	(*res_ptr) = (int32_t) abs(op1);
}

static zend_always_inline void qb_do_abs_S64(int64_t op1, int64_t *res_ptr) {
	(*res_ptr) = (int64_t) llabs(op1);
}

static zend_always_inline void qb_do_accommodate_index_U32(qb_interpreter_context *__restrict cxt, uint32_t op1, uint32_t *op2_ptr, uint32_t op3, uint32_t op4) {
	if(UNEXPECTED(!(op1 < (*op2_ptr)))) {
		uint32_t new_size = op1 + 1;
		(*op2_ptr) = new_size;
		qb_resize_segment(&cxt->function->local_storage->segments[op3], new_size * op4);
	}
}

static zend_always_inline void qb_do_accommodate_index_multiply_U32(qb_interpreter_context *__restrict cxt, uint32_t op1, uint32_t *op2_ptr, uint32_t op3, uint32_t *op4_ptr, uint32_t op5, uint32_t op6, uint32_t *res_ptr) {
	(*res_ptr) = op1 * op3;
	if(UNEXPECTED(!(op1 < (*op2_ptr)))) {
		uint32_t new_dim = op1 + 1;
		uint32_t new_size = new_dim * op3;
		(*op4_ptr) = new_size;
		(*op2_ptr) = new_dim;
		qb_resize_segment(&cxt->function->local_storage->segments[op5], new_size * op6);
	}
}

static zend_always_inline void qb_do_accommodate_push_U32(qb_interpreter_context *__restrict cxt, uint32_t *op1_ptr, uint32_t op2, uint32_t op3, uint32_t *res_ptr) {
	uint32_t new_dim = (*op1_ptr) + 1;
	(*res_ptr) = (*op1_ptr);
	(*op1_ptr) = new_dim;
	qb_resize_segment(&cxt->function->local_storage->segments[op2], new_dim * op3);
}

static zend_always_inline void qb_do_accommodate_size_U32(qb_interpreter_context *__restrict cxt, uint32_t op1, uint32_t *op2_ptr, uint32_t op3, uint32_t op4) {
	if(UNEXPECTED(!(op1 == (*op2_ptr)))) {
		uint32_t new_dim = op1;
		(*op2_ptr) = new_dim;
		qb_resize_segment(&cxt->function->local_storage->segments[op3], new_dim * op4);
	}
}

static zend_always_inline void qb_do_accommodate_size_copy_dimension_2_U32(qb_interpreter_context *__restrict cxt, uint32_t op1, uint32_t op2, uint32_t *op3_ptr, uint32_t *op4_ptr, uint32_t op5, uint32_t op6, uint32_t *op7_ptr, uint32_t *op8_ptr, uint32_t op9, uint32_t op10) {
	(*op3_ptr) = op1;
	(*op4_ptr) = op2;
	(*op7_ptr) = op5;
	(*op8_ptr) = op6;
	qb_resize_segment(&cxt->function->local_storage->segments[op9], op1 * op10);
}

static zend_always_inline void qb_do_accommodate_size_copy_dimension_3_U32(qb_interpreter_context *__restrict cxt, uint32_t op1, uint32_t op2, uint32_t *op3_ptr, uint32_t *op4_ptr, uint32_t op5, uint32_t op6, uint32_t *op7_ptr, uint32_t *op8_ptr, uint32_t op9, uint32_t op10, uint32_t *op11_ptr, uint32_t *op12_ptr, uint32_t op13, uint32_t op14) {
	(*op3_ptr) = op1;
	(*op4_ptr) = op2;
	(*op7_ptr) = op5;
	(*op8_ptr) = op6;
	(*op11_ptr) = op9;
	(*op12_ptr) = op10;
	qb_resize_segment(&cxt->function->local_storage->segments[op13], op1 * op14);
}

static zend_always_inline void qb_do_acos_F32(float32_t op1, float32_t *res_ptr) {
	(*res_ptr) = (float32_t) acosf(op1);
}

static zend_always_inline void qb_do_acos_F64(float64_t op1, float64_t *res_ptr) {
	(*res_ptr) = (float64_t) acos(op1);
}

static zend_always_inline void qb_do_acosh_F32(float32_t op1, float32_t *res_ptr) {
	(*res_ptr) = acoshf(op1);
}

static zend_always_inline void qb_do_acosh_F64(float64_t op1, float64_t *res_ptr) {
	(*res_ptr) = acosh(op1);
}

static zend_always_inline void qb_do_add_2x_F32(float32_t *op1_ptr, float32_t *op2_ptr, float32_t *res_ptr) {
	res_ptr[0] = op1_ptr[0] + op2_ptr[0];
	res_ptr[1] = op1_ptr[1] + op2_ptr[1];
}

static zend_always_inline void qb_do_add_2x_F64(float64_t *op1_ptr, float64_t *op2_ptr, float64_t *res_ptr) {
	res_ptr[0] = op1_ptr[0] + op2_ptr[0];
	res_ptr[1] = op1_ptr[1] + op2_ptr[1];
}

static zend_always_inline void qb_do_add_3x_F32(float32_t *op1_ptr, float32_t *op2_ptr, float32_t *res_ptr) {
	res_ptr[0] = op1_ptr[0] + op2_ptr[0];
	res_ptr[1] = op1_ptr[1] + op2_ptr[1];
	res_ptr[2] = op1_ptr[2] + op2_ptr[2];
}

static zend_always_inline void qb_do_add_3x_F64(float64_t *op1_ptr, float64_t *op2_ptr, float64_t *res_ptr) {
	res_ptr[0] = op1_ptr[0] + op2_ptr[0];
	res_ptr[1] = op1_ptr[1] + op2_ptr[1];
	res_ptr[2] = op1_ptr[2] + op2_ptr[2];
}

static zend_always_inline void qb_do_add_4x_F32(float32_t *op1_ptr, float32_t *op2_ptr, float32_t *res_ptr) {
	res_ptr[0] = op1_ptr[0] + op2_ptr[0];
	res_ptr[1] = op1_ptr[1] + op2_ptr[1];
	res_ptr[2] = op1_ptr[2] + op2_ptr[2];
	res_ptr[3] = op1_ptr[3] + op2_ptr[3];
}

static zend_always_inline void qb_do_add_4x_F64(float64_t *op1_ptr, float64_t *op2_ptr, float64_t *res_ptr) {
	res_ptr[0] = op1_ptr[0] + op2_ptr[0];
	res_ptr[1] = op1_ptr[1] + op2_ptr[1];
	res_ptr[2] = op1_ptr[2] + op2_ptr[2];
	res_ptr[3] = op1_ptr[3] + op2_ptr[3];
}

static zend_always_inline void qb_do_add_F32(float32_t op1, float32_t op2, float32_t *res_ptr) {
	(*res_ptr) = op1 + op2;
}

static zend_always_inline void qb_do_add_F64(float64_t op1, float64_t op2, float64_t *res_ptr) {
	(*res_ptr) = op1 + op2;
}

static zend_always_inline void qb_do_add_I08(int8_t op1, int8_t op2, int8_t *res_ptr) {
	(*res_ptr) = op1 + op2;
}

static zend_always_inline void qb_do_add_I16(int16_t op1, int16_t op2, int16_t *res_ptr) {
	(*res_ptr) = op1 + op2;
}

static zend_always_inline void qb_do_add_I32(int32_t op1, int32_t op2, int32_t *res_ptr) {
	(*res_ptr) = op1 + op2;
}

static zend_always_inline void qb_do_add_I64(int64_t op1, int64_t op2, int64_t *res_ptr) {
	(*res_ptr) = op1 + op2;
}

static zend_always_inline void qb_do_array_pad_count_U32(uint32_t op1, int32_t op2, uint32_t *res_ptr) {
	if(op2 < 0) {
		(*res_ptr) = ((uint32_t) (-op2) > op1) ? (uint32_t) (-op2) : op1;
	} else {
		(*res_ptr) = ((uint32_t) op2 > op1) ? (uint32_t) op2 : op1;
	}
}

static zend_always_inline void qb_do_asin_F32(float32_t op1, float32_t *res_ptr) {
	(*res_ptr) = asinf(op1);
}

static zend_always_inline void qb_do_asin_F64(float64_t op1, float64_t *res_ptr) {
	(*res_ptr) = asin(op1);
}

static zend_always_inline void qb_do_asinh_F32(float32_t op1, float32_t *res_ptr) {
	(*res_ptr) = asinhf(op1);
}

static zend_always_inline void qb_do_asinh_F64(float64_t op1, float64_t *res_ptr) {
	(*res_ptr) = asinh(op1);
}

static zend_always_inline void qb_do_atan2_F32(float32_t op1, float32_t op2, float32_t *res_ptr) {
	(*res_ptr) = atan2f(op1, op2);
}

static zend_always_inline void qb_do_atan2_F64(float64_t op1, float64_t op2, float64_t *res_ptr) {
	(*res_ptr) = atan2(op1, op2);
}

static zend_always_inline void qb_do_atan_F32(float32_t op1, float32_t *res_ptr) {
	(*res_ptr) = atanf(op1);
}

static zend_always_inline void qb_do_atan_F64(float64_t op1, float64_t *res_ptr) {
	(*res_ptr) = atan(op1);
}

static zend_always_inline void qb_do_atanh_F32(float32_t op1, float32_t *res_ptr) {
	(*res_ptr) = atanhf(op1);
}

static zend_always_inline void qb_do_atanh_F64(float64_t op1, float64_t *res_ptr) {
	(*res_ptr) = atanh(op1);
}

static zend_always_inline void qb_do_bitwise_and_I08(int8_t op1, int8_t op2, int8_t *res_ptr) {
	(*res_ptr) = op1 & op2;
}

static zend_always_inline void qb_do_bitwise_and_I16(int16_t op1, int16_t op2, int16_t *res_ptr) {
	(*res_ptr) = op1 & op2;
}

static zend_always_inline void qb_do_bitwise_and_I32(int32_t op1, int32_t op2, int32_t *res_ptr) {
	(*res_ptr) = op1 & op2;
}

static zend_always_inline void qb_do_bitwise_and_I64(int64_t op1, int64_t op2, int64_t *res_ptr) {
	(*res_ptr) = op1 & op2;
}

static zend_always_inline void qb_do_bitwise_not_I08(int8_t op1, int8_t *res_ptr) {
	(*res_ptr) = ~op1;
}

static zend_always_inline void qb_do_bitwise_not_I16(int16_t op1, int16_t *res_ptr) {
	(*res_ptr) = ~op1;
}

static zend_always_inline void qb_do_bitwise_not_I32(int32_t op1, int32_t *res_ptr) {
	(*res_ptr) = ~op1;
}

static zend_always_inline void qb_do_bitwise_not_I64(int64_t op1, int64_t *res_ptr) {
	(*res_ptr) = ~op1;
}

static zend_always_inline void qb_do_bitwise_or_I08(int8_t op1, int8_t op2, int8_t *res_ptr) {
	(*res_ptr) = op1 | op2;
}

static zend_always_inline void qb_do_bitwise_or_I16(int16_t op1, int16_t op2, int16_t *res_ptr) {
	(*res_ptr) = op1 | op2;
}

static zend_always_inline void qb_do_bitwise_or_I32(int32_t op1, int32_t op2, int32_t *res_ptr) {
	(*res_ptr) = op1 | op2;
}

static zend_always_inline void qb_do_bitwise_or_I64(int64_t op1, int64_t op2, int64_t *res_ptr) {
	(*res_ptr) = op1 | op2;
}

static zend_always_inline void qb_do_bitwise_xor_I08(int8_t op1, int8_t op2, int8_t *res_ptr) {
	(*res_ptr) = op1 ^ op2;
}

static zend_always_inline void qb_do_bitwise_xor_I16(int16_t op1, int16_t op2, int16_t *res_ptr) {
	(*res_ptr) = op1 ^ op2;
}

static zend_always_inline void qb_do_bitwise_xor_I32(int32_t op1, int32_t op2, int32_t *res_ptr) {
	(*res_ptr) = op1 ^ op2;
}

static zend_always_inline void qb_do_bitwise_xor_I64(int64_t op1, int64_t op2, int64_t *res_ptr) {
	(*res_ptr) = op1 ^ op2;
}

static zend_always_inline void qb_do_boolean_cast_F32(float32_t op1, int32_t *res_ptr) {
	(*res_ptr) = (op1 != 0);
}

static zend_always_inline void qb_do_boolean_cast_F64(float64_t op1, int32_t *res_ptr) {
	(*res_ptr) = (op1 != 0);
}

static zend_always_inline void qb_do_boolean_cast_I08(int8_t op1, int32_t *res_ptr) {
	(*res_ptr) = (op1 != 0);
}

static zend_always_inline void qb_do_boolean_cast_I16(int16_t op1, int32_t *res_ptr) {
	(*res_ptr) = (op1 != 0);
}

static zend_always_inline void qb_do_boolean_cast_I32(int32_t op1, int32_t *res_ptr) {
	(*res_ptr) = (op1 != 0);
}

static zend_always_inline void qb_do_boolean_cast_I64(int64_t op1, int32_t *res_ptr) {
	(*res_ptr) = (op1 != 0);
}

static zend_always_inline void qb_do_branch_on_less_than_F32(int32_t *condition_ptr, float32_t op1, float32_t op2) {
	(*condition_ptr) = (op1 < op2);
}

static zend_always_inline void qb_do_branch_on_less_than_F64(int32_t *condition_ptr, float64_t op1, float64_t op2) {
	(*condition_ptr) = (op1 < op2);
}

static zend_always_inline void qb_do_branch_on_less_than_S08(int32_t *condition_ptr, int8_t op1, int8_t op2) {
	(*condition_ptr) = (op1 < op2);
}

static zend_always_inline void qb_do_branch_on_less_than_S16(int32_t *condition_ptr, int16_t op1, int16_t op2) {
	(*condition_ptr) = (op1 < op2);
}

static zend_always_inline void qb_do_branch_on_less_than_S32(int32_t *condition_ptr, int32_t op1, int32_t op2) {
	(*condition_ptr) = (op1 < op2);
}

static zend_always_inline void qb_do_branch_on_less_than_S64(int32_t *condition_ptr, int64_t op1, int64_t op2) {
	(*condition_ptr) = (op1 < op2);
}

static zend_always_inline void qb_do_branch_on_less_than_U08(int32_t *condition_ptr, uint8_t op1, uint8_t op2) {
	(*condition_ptr) = (op1 < op2);
}

static zend_always_inline void qb_do_branch_on_less_than_U16(int32_t *condition_ptr, uint16_t op1, uint16_t op2) {
	(*condition_ptr) = (op1 < op2);
}

static zend_always_inline void qb_do_branch_on_less_than_U32(int32_t *condition_ptr, uint32_t op1, uint32_t op2) {
	(*condition_ptr) = (op1 < op2);
}

static zend_always_inline void qb_do_branch_on_less_than_U64(int32_t *condition_ptr, uint64_t op1, uint64_t op2) {
	(*condition_ptr) = (op1 < op2);
}

static zend_always_inline void qb_do_branch_on_less_than_equal_F32(int32_t *condition_ptr, float32_t op1, float32_t op2) {
	(*condition_ptr) = (op1 <= op2);
}

static zend_always_inline void qb_do_branch_on_less_than_equal_F64(int32_t *condition_ptr, float64_t op1, float64_t op2) {
	(*condition_ptr) = (op1 <= op2);
}

static zend_always_inline void qb_do_branch_on_less_than_equal_S08(int32_t *condition_ptr, int8_t op1, int8_t op2) {
	(*condition_ptr) = (op1 <= op2);
}

static zend_always_inline void qb_do_branch_on_less_than_equal_S16(int32_t *condition_ptr, int16_t op1, int16_t op2) {
	(*condition_ptr) = (op1 <= op2);
}

static zend_always_inline void qb_do_branch_on_less_than_equal_S32(int32_t *condition_ptr, int32_t op1, int32_t op2) {
	(*condition_ptr) = (op1 <= op2);
}

static zend_always_inline void qb_do_branch_on_less_than_equal_S64(int32_t *condition_ptr, int64_t op1, int64_t op2) {
	(*condition_ptr) = (op1 <= op2);
}

static zend_always_inline void qb_do_branch_on_less_than_equal_U08(int32_t *condition_ptr, uint8_t op1, uint8_t op2) {
	(*condition_ptr) = (op1 <= op2);
}

static zend_always_inline void qb_do_branch_on_less_than_equal_U16(int32_t *condition_ptr, uint16_t op1, uint16_t op2) {
	(*condition_ptr) = (op1 <= op2);
}

static zend_always_inline void qb_do_branch_on_less_than_equal_U32(int32_t *condition_ptr, uint32_t op1, uint32_t op2) {
	(*condition_ptr) = (op1 <= op2);
}

static zend_always_inline void qb_do_branch_on_less_than_equal_U64(int32_t *condition_ptr, uint64_t op1, uint64_t op2) {
	(*condition_ptr) = (op1 <= op2);
}

static zend_always_inline void qb_do_branch_on_not_equal_F32(int32_t *condition_ptr, float32_t op1, float32_t op2) {
	(*condition_ptr) = (op1 != op2);
}

static zend_always_inline void qb_do_branch_on_not_equal_F64(int32_t *condition_ptr, float64_t op1, float64_t op2) {
	(*condition_ptr) = (op1 != op2);
}

static zend_always_inline void qb_do_branch_on_not_equal_I08(int32_t *condition_ptr, int8_t op1, int8_t op2) {
	(*condition_ptr) = (op1 != op2);
}

static zend_always_inline void qb_do_branch_on_not_equal_I16(int32_t *condition_ptr, int16_t op1, int16_t op2) {
	(*condition_ptr) = (op1 != op2);
}

static zend_always_inline void qb_do_branch_on_not_equal_I32(int32_t *condition_ptr, int32_t op1, int32_t op2) {
	(*condition_ptr) = (op1 != op2);
}

static zend_always_inline void qb_do_branch_on_not_equal_I64(int32_t *condition_ptr, int64_t op1, int64_t op2) {
	(*condition_ptr) = (op1 != op2);
}

static zend_always_inline void qb_do_cast_F32_F64(float32_t op1, float64_t *res_ptr) {
	(*res_ptr) = (float64_t) op1;
}

static zend_always_inline void qb_do_cast_F32_S08(float32_t op1, int8_t *res_ptr) {
	(*res_ptr) = (int8_t) op1;
}

static zend_always_inline void qb_do_cast_F32_S16(float32_t op1, int16_t *res_ptr) {
	(*res_ptr) = (int16_t) op1;
}

static zend_always_inline void qb_do_cast_F32_S32(float32_t op1, int32_t *res_ptr) {
	(*res_ptr) = (int32_t) op1;
}

static zend_always_inline void qb_do_cast_F32_S64(float32_t op1, int64_t *res_ptr) {
	(*res_ptr) = (int64_t) op1;
}

static zend_always_inline void qb_do_cast_F32_U08(float32_t op1, uint8_t *res_ptr) {
	(*res_ptr) = (uint8_t) op1;
}

static zend_always_inline void qb_do_cast_F32_U16(float32_t op1, uint16_t *res_ptr) {
	(*res_ptr) = (uint16_t) op1;
}

static zend_always_inline void qb_do_cast_F32_U32(float32_t op1, uint32_t *res_ptr) {
	(*res_ptr) = (uint32_t) op1;
}

static zend_always_inline void qb_do_cast_F32_U64(float32_t op1, uint64_t *res_ptr) {
	(*res_ptr) = (uint64_t) op1;
}

static zend_always_inline void qb_do_cast_F64_F32(float64_t op1, float32_t *res_ptr) {
	(*res_ptr) = (float32_t) op1;
}

static zend_always_inline void qb_do_cast_F64_S08(float64_t op1, int8_t *res_ptr) {
	(*res_ptr) = (int8_t) op1;
}

static zend_always_inline void qb_do_cast_F64_S16(float64_t op1, int16_t *res_ptr) {
	(*res_ptr) = (int16_t) op1;
}

static zend_always_inline void qb_do_cast_F64_S32(float64_t op1, int32_t *res_ptr) {
	(*res_ptr) = (int32_t) op1;
}

static zend_always_inline void qb_do_cast_F64_S64(float64_t op1, int64_t *res_ptr) {
	(*res_ptr) = (int64_t) op1;
}

static zend_always_inline void qb_do_cast_F64_U08(float64_t op1, uint8_t *res_ptr) {
	(*res_ptr) = (uint8_t) op1;
}

static zend_always_inline void qb_do_cast_F64_U16(float64_t op1, uint16_t *res_ptr) {
	(*res_ptr) = (uint16_t) op1;
}

static zend_always_inline void qb_do_cast_F64_U32(float64_t op1, uint32_t *res_ptr) {
	(*res_ptr) = (uint32_t) op1;
}

static zend_always_inline void qb_do_cast_F64_U64(float64_t op1, uint64_t *res_ptr) {
	(*res_ptr) = (uint64_t) op1;
}

static zend_always_inline void qb_do_cast_I16_I08(int16_t op1, int8_t *res_ptr) {
	(*res_ptr) = (int8_t) op1;
}

static zend_always_inline void qb_do_cast_I32_I08(int32_t op1, int8_t *res_ptr) {
	(*res_ptr) = (int8_t) op1;
}

static zend_always_inline void qb_do_cast_I32_I16(int32_t op1, int16_t *res_ptr) {
	(*res_ptr) = (int16_t) op1;
}

static zend_always_inline void qb_do_cast_I64_I08(int64_t op1, int8_t *res_ptr) {
	(*res_ptr) = (int8_t) op1;
}

static zend_always_inline void qb_do_cast_I64_I16(int64_t op1, int16_t *res_ptr) {
	(*res_ptr) = (int16_t) op1;
}

static zend_always_inline void qb_do_cast_I64_I32(int64_t op1, int32_t *res_ptr) {
	(*res_ptr) = (int32_t) op1;
}

static zend_always_inline void qb_do_cast_S08_F32(int8_t op1, float32_t *res_ptr) {
	(*res_ptr) = (float32_t) op1;
}

static zend_always_inline void qb_do_cast_S08_F64(int8_t op1, float64_t *res_ptr) {
	(*res_ptr) = (float64_t) op1;
}

static zend_always_inline void qb_do_cast_S08_I16(int8_t op1, int16_t *res_ptr) {
	(*res_ptr) = (int16_t) op1;
}

static zend_always_inline void qb_do_cast_S08_I32(int8_t op1, int32_t *res_ptr) {
	(*res_ptr) = (int32_t) op1;
}

static zend_always_inline void qb_do_cast_S08_I64(int8_t op1, int64_t *res_ptr) {
	(*res_ptr) = (int64_t) op1;
}

static zend_always_inline void qb_do_cast_S16_F32(int16_t op1, float32_t *res_ptr) {
	(*res_ptr) = (float32_t) op1;
}

static zend_always_inline void qb_do_cast_S16_F64(int16_t op1, float64_t *res_ptr) {
	(*res_ptr) = (float64_t) op1;
}

static zend_always_inline void qb_do_cast_S16_I32(int16_t op1, int32_t *res_ptr) {
	(*res_ptr) = (int32_t) op1;
}

static zend_always_inline void qb_do_cast_S16_I64(int16_t op1, int64_t *res_ptr) {
	(*res_ptr) = (int64_t) op1;
}

static zend_always_inline void qb_do_cast_S32_F32(int32_t op1, float32_t *res_ptr) {
	(*res_ptr) = (float32_t) op1;
}

static zend_always_inline void qb_do_cast_S32_F64(int32_t op1, float64_t *res_ptr) {
	(*res_ptr) = (float64_t) op1;
}

static zend_always_inline void qb_do_cast_S32_I64(int32_t op1, int64_t *res_ptr) {
	(*res_ptr) = (int64_t) op1;
}

static zend_always_inline void qb_do_cast_S64_F32(int64_t op1, float32_t *res_ptr) {
	(*res_ptr) = (float32_t) op1;
}

static zend_always_inline void qb_do_cast_S64_F64(int64_t op1, float64_t *res_ptr) {
	(*res_ptr) = (float64_t) op1;
}

static zend_always_inline void qb_do_cast_U08_F32(uint8_t op1, float32_t *res_ptr) {
	(*res_ptr) = (float32_t) op1;
}

static zend_always_inline void qb_do_cast_U08_F64(uint8_t op1, float64_t *res_ptr) {
	(*res_ptr) = (float64_t) op1;
}

static zend_always_inline void qb_do_cast_U08_I16(uint8_t op1, int16_t *res_ptr) {
	(*res_ptr) = (int16_t) op1;
}

static zend_always_inline void qb_do_cast_U08_I32(uint8_t op1, int32_t *res_ptr) {
	(*res_ptr) = (int32_t) op1;
}

static zend_always_inline void qb_do_cast_U08_I64(uint8_t op1, int64_t *res_ptr) {
	(*res_ptr) = (int64_t) op1;
}

static zend_always_inline void qb_do_cast_U16_F32(uint16_t op1, float32_t *res_ptr) {
	(*res_ptr) = (float32_t) op1;
}

static zend_always_inline void qb_do_cast_U16_F64(uint16_t op1, float64_t *res_ptr) {
	(*res_ptr) = (float64_t) op1;
}

static zend_always_inline void qb_do_cast_U16_I32(uint16_t op1, int32_t *res_ptr) {
	(*res_ptr) = (int32_t) op1;
}

static zend_always_inline void qb_do_cast_U16_I64(uint16_t op1, int64_t *res_ptr) {
	(*res_ptr) = (int64_t) op1;
}

static zend_always_inline void qb_do_cast_U32_F32(uint32_t op1, float32_t *res_ptr) {
	(*res_ptr) = (float32_t) op1;
}

static zend_always_inline void qb_do_cast_U32_F64(uint32_t op1, float64_t *res_ptr) {
	(*res_ptr) = (float64_t) op1;
}

static zend_always_inline void qb_do_cast_U32_I64(uint32_t op1, int64_t *res_ptr) {
	(*res_ptr) = (int64_t) op1;
}

static zend_always_inline void qb_do_cast_U64_F32(uint64_t op1, float32_t *res_ptr) {
	(*res_ptr) = (float32_t) op1;
}

static zend_always_inline void qb_do_cast_U64_F64(uint64_t op1, float64_t *res_ptr) {
	(*res_ptr) = (float64_t) op1;
}

static zend_always_inline void qb_do_ceil_F32(float32_t op1, float32_t *res_ptr) {
	(*res_ptr) = ceilf(op1);
}

static zend_always_inline void qb_do_ceil_F64(float64_t op1, float64_t *res_ptr) {
	(*res_ptr) = ceil(op1);
}

static zend_always_inline void qb_do_choose_size_larger_of_two_U32(uint32_t op1, uint32_t op2, uint32_t op3, uint32_t op4, uint32_t *res_ptr) {
	if(op1 >= op3) {
		(*res_ptr) = op2;
	} else {
		(*res_ptr) = op4;
	}
}

static zend_always_inline void qb_do_choose_size_larger_of_two_top_level_U32(uint32_t op1, uint32_t op2, uint32_t *res_ptr) {
	if(op1 >= op2) {
		(*res_ptr) = op1;
	} else {
		(*res_ptr) = op2;
	}
}

static zend_always_inline void qb_do_clamp_F32(float32_t op1, float32_t op2, float32_t op3, float32_t *res_ptr) {
	if(op1 < op2) {
		(*res_ptr) = op2;
	} else if(op1 > op3) {
		(*res_ptr) = op3;
	} else {
		(*res_ptr) = op1;
	}
}

static zend_always_inline void qb_do_clamp_F64(float64_t op1, float64_t op2, float64_t op3, float64_t *res_ptr) {
	if(op1 < op2) {
		(*res_ptr) = op2;
	} else if(op1 > op3) {
		(*res_ptr) = op3;
	} else {
		(*res_ptr) = op1;
	}
}

static zend_always_inline void qb_do_clear_scalar_F32(float32_t *res_ptr) {
	(*res_ptr) = 0;
}

static zend_always_inline void qb_do_clear_scalar_F64(float64_t *res_ptr) {
	(*res_ptr) = 0;
}

static zend_always_inline void qb_do_clear_scalar_I08(int8_t *res_ptr) {
	(*res_ptr) = 0;
}

static zend_always_inline void qb_do_clear_scalar_I16(int16_t *res_ptr) {
	(*res_ptr) = 0;
}

static zend_always_inline void qb_do_clear_scalar_I32(int32_t *res_ptr) {
	(*res_ptr) = 0;
}

static zend_always_inline void qb_do_clear_scalar_I64(int64_t *res_ptr) {
	(*res_ptr) = 0;
}

static zend_always_inline void qb_do_copy_2x_F32(float32_t *op1_ptr, float32_t *res_ptr) {
	res_ptr[0] = op1_ptr[0];
	res_ptr[1] = op1_ptr[1];
}

static zend_always_inline void qb_do_copy_2x_F64(float64_t *op1_ptr, float64_t *res_ptr) {
	res_ptr[0] = op1_ptr[0];
	res_ptr[1] = op1_ptr[1];
}

static zend_always_inline void qb_do_copy_3x_F32(float32_t *op1_ptr, float32_t *res_ptr) {
	res_ptr[0] = op1_ptr[0];
	res_ptr[1] = op1_ptr[1];
	res_ptr[2] = op1_ptr[2];
}

static zend_always_inline void qb_do_copy_3x_F64(float64_t *op1_ptr, float64_t *res_ptr) {
	res_ptr[0] = op1_ptr[0];
	res_ptr[1] = op1_ptr[1];
	res_ptr[2] = op1_ptr[2];
}

static zend_always_inline void qb_do_copy_4x_F32(float32_t *op1_ptr, float32_t *res_ptr) {
	res_ptr[0] = op1_ptr[0];
	res_ptr[1] = op1_ptr[1];
	res_ptr[2] = op1_ptr[2];
	res_ptr[3] = op1_ptr[3];
}

static zend_always_inline void qb_do_copy_4x_F64(float64_t *op1_ptr, float64_t *res_ptr) {
	res_ptr[0] = op1_ptr[0];
	res_ptr[1] = op1_ptr[1];
	res_ptr[2] = op1_ptr[2];
	res_ptr[3] = op1_ptr[3];
}

static zend_always_inline void qb_do_copy_F32(float32_t op1, float32_t *res_ptr) {
	(*res_ptr) = op1;
}

static zend_always_inline void qb_do_copy_F64(float64_t op1, float64_t *res_ptr) {
	(*res_ptr) = op1;
}

static zend_always_inline void qb_do_copy_I08(int8_t op1, int8_t *res_ptr) {
	(*res_ptr) = op1;
}

static zend_always_inline void qb_do_copy_I16(int16_t op1, int16_t *res_ptr) {
	(*res_ptr) = op1;
}

static zend_always_inline void qb_do_copy_I32(int32_t op1, int32_t *res_ptr) {
	(*res_ptr) = op1;
}

static zend_always_inline void qb_do_copy_I64(int64_t op1, int64_t *res_ptr) {
	(*res_ptr) = op1;
}

static zend_always_inline void qb_do_cos_F32(float32_t op1, float32_t *res_ptr) {
	(*res_ptr) = cosf(op1);
}

static zend_always_inline void qb_do_cos_F64(float64_t op1, float64_t *res_ptr) {
	(*res_ptr) = cos(op1);
}

static zend_always_inline void qb_do_cosh_F32(float32_t op1, float32_t *res_ptr) {
	(*res_ptr) = coshf(op1);
}

static zend_always_inline void qb_do_cosh_F64(float64_t op1, float64_t *res_ptr) {
	(*res_ptr) = cosh(op1);
}

static zend_always_inline void qb_do_cross_product_2x_F32(float32_t *op1_ptr, float32_t *op2_ptr, float32_t *res_ptr) {
	float32_t k = (op1_ptr[0] * op2_ptr[1]) - (op1_ptr[1] * op2_ptr[0]);
	res_ptr[0] = k;
	res_ptr[1] = k;
}

static zend_always_inline void qb_do_cross_product_2x_F64(float64_t *op1_ptr, float64_t *op2_ptr, float64_t *res_ptr) {
	float64_t k = (op1_ptr[0] * op2_ptr[1]) - (op1_ptr[1] * op2_ptr[0]);
	res_ptr[0] = k;
	res_ptr[1] = k;
}

static zend_always_inline void qb_do_cross_product_3x_F32(float32_t *op1_ptr, float32_t *op2_ptr, float32_t *res_ptr) {
	float32_t i = (op1_ptr[1] * op2_ptr[2]) - (op1_ptr[2] * op2_ptr[1]);
	float32_t j = (op1_ptr[2] * op2_ptr[0]) - (op1_ptr[0] * op2_ptr[2]);
	float32_t k = (op1_ptr[0] * op2_ptr[1]) - (op1_ptr[1] * op2_ptr[0]);
	res_ptr[0] = i;
	res_ptr[1] = j;
	res_ptr[2] = k;
}

static zend_always_inline void qb_do_cross_product_3x_F64(float64_t *op1_ptr, float64_t *op2_ptr, float64_t *res_ptr) {
	float64_t i = (op1_ptr[1] * op2_ptr[2]) - (op1_ptr[2] * op2_ptr[1]);
	float64_t j = (op1_ptr[2] * op2_ptr[0]) - (op1_ptr[0] * op2_ptr[2]);
	float64_t k = (op1_ptr[0] * op2_ptr[1]) - (op1_ptr[1] * op2_ptr[0]);
	res_ptr[0] = i;
	res_ptr[1] = j;
	res_ptr[2] = k;
}

static zend_always_inline void qb_do_cross_product_4x_F32(float32_t *op1_ptr, float32_t *op2_ptr, float32_t *op3_ptr, float32_t *res_ptr) {
	float32_t i = op1_ptr[1] * ((op2_ptr[2] * op3_ptr[3]) - (op3_ptr[2] * op2_ptr[3])) - op1_ptr[2] * ((op2_ptr[1] * op3_ptr[3]) - (op3_ptr[1] * op2_ptr[3])) + op1_ptr[3] * ((op2_ptr[1] * op3_ptr[2]) - (op3_ptr[1] * op2_ptr[2]));
	float32_t j = op1_ptr[0] * ((op3_ptr[2] * op2_ptr[3]) - (op2_ptr[2] * op3_ptr[3])) - op1_ptr[2] * ((op3_ptr[0] * op2_ptr[3]) - (op2_ptr[0] * op3_ptr[3])) + op1_ptr[3] * ((op3_ptr[0] * op2_ptr[2]) - (op2_ptr[0] * op3_ptr[2]));
	float32_t k = op1_ptr[0] * ((op2_ptr[1] * op3_ptr[3]) - (op3_ptr[1] * op2_ptr[3])) - op1_ptr[1] * ((op2_ptr[0] * op3_ptr[3]) - (op3_ptr[0] * op2_ptr[3])) + op1_ptr[3] * ((op2_ptr[0] * op3_ptr[1]) - (op3_ptr[0] * op2_ptr[1]));
	float32_t m = op1_ptr[0] * ((op3_ptr[1] * op2_ptr[2]) - (op2_ptr[1] * op3_ptr[2])) - op1_ptr[1] * ((op3_ptr[0] * op2_ptr[2]) - (op2_ptr[0] * op3_ptr[2])) + op1_ptr[2] * ((op3_ptr[0] * op2_ptr[1]) - (op2_ptr[0] * op3_ptr[1]));
	res_ptr[0] = i;
	res_ptr[1] = j;
	res_ptr[2] = k;
	res_ptr[3] = m;
}

static zend_always_inline void qb_do_cross_product_4x_F64(float64_t *op1_ptr, float64_t *op2_ptr, float64_t *op3_ptr, float64_t *res_ptr) {
	float64_t i = op1_ptr[1] * ((op2_ptr[2] * op3_ptr[3]) - (op3_ptr[2] * op2_ptr[3])) - op1_ptr[2] * ((op2_ptr[1] * op3_ptr[3]) - (op3_ptr[1] * op2_ptr[3])) + op1_ptr[3] * ((op2_ptr[1] * op3_ptr[2]) - (op3_ptr[1] * op2_ptr[2]));
	float64_t j = op1_ptr[0] * ((op3_ptr[2] * op2_ptr[3]) - (op2_ptr[2] * op3_ptr[3])) - op1_ptr[2] * ((op3_ptr[0] * op2_ptr[3]) - (op2_ptr[0] * op3_ptr[3])) + op1_ptr[3] * ((op3_ptr[0] * op2_ptr[2]) - (op2_ptr[0] * op3_ptr[2]));
	float64_t k = op1_ptr[0] * ((op2_ptr[1] * op3_ptr[3]) - (op3_ptr[1] * op2_ptr[3])) - op1_ptr[1] * ((op2_ptr[0] * op3_ptr[3]) - (op3_ptr[0] * op2_ptr[3])) + op1_ptr[3] * ((op2_ptr[0] * op3_ptr[1]) - (op3_ptr[0] * op2_ptr[1]));
	float64_t m = op1_ptr[0] * ((op3_ptr[1] * op2_ptr[2]) - (op2_ptr[1] * op3_ptr[2])) - op1_ptr[1] * ((op3_ptr[0] * op2_ptr[2]) - (op2_ptr[0] * op3_ptr[2])) + op1_ptr[2] * ((op3_ptr[0] * op2_ptr[1]) - (op2_ptr[0] * op3_ptr[1]));
	res_ptr[0] = i;
	res_ptr[1] = j;
	res_ptr[2] = k;
	res_ptr[3] = m;
}

static zend_always_inline void qb_do_decrement_2x_F32(float32_t *res_ptr) {
	--res_ptr[0];
	--res_ptr[1];
}

static zend_always_inline void qb_do_decrement_2x_F64(float64_t *res_ptr) {
	--res_ptr[0];
	--res_ptr[1];
}

static zend_always_inline void qb_do_decrement_3x_F32(float32_t *res_ptr) {
	--res_ptr[0];
	--res_ptr[1];
	--res_ptr[2];
}

static zend_always_inline void qb_do_decrement_3x_F64(float64_t *res_ptr) {
	--res_ptr[0];
	--res_ptr[1];
	--res_ptr[2];
}

static zend_always_inline void qb_do_decrement_4x_F32(float32_t *res_ptr) {
	--res_ptr[0];
	--res_ptr[1];
	--res_ptr[2];
	--res_ptr[3];
}

static zend_always_inline void qb_do_decrement_4x_F64(float64_t *res_ptr) {
	--res_ptr[0];
	--res_ptr[1];
	--res_ptr[2];
	--res_ptr[3];
}

static zend_always_inline void qb_do_decrement_F32(float32_t *res_ptr) {
	--(*res_ptr);
}

static zend_always_inline void qb_do_decrement_F64(float64_t *res_ptr) {
	--(*res_ptr);
}

static zend_always_inline void qb_do_decrement_I08(int8_t *res_ptr) {
	--(*res_ptr);
}

static zend_always_inline void qb_do_decrement_I16(int16_t *res_ptr) {
	--(*res_ptr);
}

static zend_always_inline void qb_do_decrement_I32(int32_t *res_ptr) {
	--(*res_ptr);
}

static zend_always_inline void qb_do_decrement_I64(int64_t *res_ptr) {
	--(*res_ptr);
}

static zend_always_inline void qb_do_degree_to_radian_F32(float32_t op1, float32_t *res_ptr) {
	(*res_ptr) = op1 * ((float32_t) (M_PI / 180.0));
}

static zend_always_inline void qb_do_degree_to_radian_F64(float64_t op1, float64_t *res_ptr) {
	(*res_ptr) = op1 * ((float64_t) (M_PI / 180.0));
}

static zend_always_inline void qb_do_determinant_2x_F32(float32_t *op1_ptr, float32_t *res_ptr) {
	(*res_ptr) = (op1_ptr[0 * 2 + 0] * op1_ptr[1 * 2 + 1]) - (op1_ptr[0 * 2 + 1] * op1_ptr[1 * 2 + 0]);
}

static zend_always_inline void qb_do_determinant_2x_F64(float64_t *op1_ptr, float64_t *res_ptr) {
	(*res_ptr) = (op1_ptr[0 * 2 + 0] * op1_ptr[1 * 2 + 1]) - (op1_ptr[0 * 2 + 1] * op1_ptr[1 * 2 + 0]);
}

static zend_always_inline void qb_do_determinant_3x_F32(float32_t *op1_ptr, float32_t *res_ptr) {
	(*res_ptr) =	 (op1_ptr[0 * 3 + 0] * op1_ptr[1 * 3 + 1] * op1_ptr[2 * 3 + 2]) -
	(op1_ptr[0 * 3 + 0] * op1_ptr[1 * 3 + 2] * op1_ptr[2 * 3 + 1]) +
	(op1_ptr[0 * 3 + 1] * op1_ptr[1 * 3 + 2] * op1_ptr[2 * 3 + 0]) -
	(op1_ptr[0 * 3 + 1] * op1_ptr[1 * 3 + 0] * op1_ptr[2 * 3 + 2]) +
	(op1_ptr[0 * 3 + 2] * op1_ptr[1 * 3 + 0] * op1_ptr[2 * 3 + 1]) -
	(op1_ptr[0 * 3 + 2] * op1_ptr[1 * 3 + 1] * op1_ptr[2 * 3 + 0]);
}

static zend_always_inline void qb_do_determinant_3x_F64(float64_t *op1_ptr, float64_t *res_ptr) {
	(*res_ptr) =	 (op1_ptr[0 * 3 + 0] * op1_ptr[1 * 3 + 1] * op1_ptr[2 * 3 + 2]) -
	(op1_ptr[0 * 3 + 0] * op1_ptr[1 * 3 + 2] * op1_ptr[2 * 3 + 1]) +
	(op1_ptr[0 * 3 + 1] * op1_ptr[1 * 3 + 2] * op1_ptr[2 * 3 + 0]) -
	(op1_ptr[0 * 3 + 1] * op1_ptr[1 * 3 + 0] * op1_ptr[2 * 3 + 2]) +
	(op1_ptr[0 * 3 + 2] * op1_ptr[1 * 3 + 0] * op1_ptr[2 * 3 + 1]) -
	(op1_ptr[0 * 3 + 2] * op1_ptr[1 * 3 + 1] * op1_ptr[2 * 3 + 0]);
}

static zend_always_inline void qb_do_distance_2x_F32(float32_t *op1_ptr, float32_t *op2_ptr, float32_t *res_ptr) {
	float32_t sum = (op1_ptr[0] - op2_ptr[0]) * (op1_ptr[0] - op2_ptr[0]) + (op1_ptr[1] - op2_ptr[1]) * (op1_ptr[1] - op2_ptr[1]);
	(*res_ptr) = qb_fast_sqrtf(sum);
}

static zend_always_inline void qb_do_distance_2x_F64(float64_t *op1_ptr, float64_t *op2_ptr, float64_t *res_ptr) {
	float64_t sum = (op1_ptr[0] - op2_ptr[0]) * (op1_ptr[0] - op2_ptr[0]) + (op1_ptr[1] - op2_ptr[1]) * (op1_ptr[1] - op2_ptr[1]);
	(*res_ptr) = qb_fast_sqrt(sum);
}

static zend_always_inline void qb_do_distance_3x_F32(float32_t *op1_ptr, float32_t *op2_ptr, float32_t *res_ptr) {
	float32_t sum = (op1_ptr[0] - op2_ptr[0]) * (op1_ptr[0] - op2_ptr[0]) + (op1_ptr[1] - op2_ptr[1]) * (op1_ptr[1] - op2_ptr[1]) + (op1_ptr[2] - op2_ptr[2]) * (op1_ptr[2] - op2_ptr[2]);
	(*res_ptr) = qb_fast_sqrtf(sum);
}

static zend_always_inline void qb_do_distance_3x_F64(float64_t *op1_ptr, float64_t *op2_ptr, float64_t *res_ptr) {
	float64_t sum = (op1_ptr[0] - op2_ptr[0]) * (op1_ptr[0] - op2_ptr[0]) + (op1_ptr[1] - op2_ptr[1]) * (op1_ptr[1] - op2_ptr[1]) + (op1_ptr[2] - op2_ptr[2]) * (op1_ptr[2] - op2_ptr[2]);
	(*res_ptr) = qb_fast_sqrt(sum);
}

static zend_always_inline void qb_do_distance_4x_F32(float32_t *op1_ptr, float32_t *op2_ptr, float32_t *res_ptr) {
	float32_t sum = (op1_ptr[0] - op2_ptr[0]) * (op1_ptr[0] - op2_ptr[0]) + (op1_ptr[1] - op2_ptr[1]) * (op1_ptr[1] - op2_ptr[1]) + (op1_ptr[2] - op2_ptr[2]) * (op1_ptr[2] - op2_ptr[2]) + (op1_ptr[3] - op2_ptr[3]) * (op1_ptr[3] - op2_ptr[3]);
	(*res_ptr) = qb_fast_sqrtf(sum);
}

static zend_always_inline void qb_do_distance_4x_F64(float64_t *op1_ptr, float64_t *op2_ptr, float64_t *res_ptr) {
	float64_t sum = (op1_ptr[0] - op2_ptr[0]) * (op1_ptr[0] - op2_ptr[0]) + (op1_ptr[1] - op2_ptr[1]) * (op1_ptr[1] - op2_ptr[1]) + (op1_ptr[2] - op2_ptr[2]) * (op1_ptr[2] - op2_ptr[2]) + (op1_ptr[3] - op2_ptr[3]) * (op1_ptr[3] - op2_ptr[3]);
	(*res_ptr) = qb_fast_sqrt(sum);
}

static zend_always_inline void qb_do_divide_2x_F32(float32_t *op1_ptr, float32_t *op2_ptr, float32_t *res_ptr) {
	res_ptr[0] = op1_ptr[0] / op2_ptr[0];
	res_ptr[1] = op1_ptr[1] / op2_ptr[1];
}

static zend_always_inline void qb_do_divide_2x_F64(float64_t *op1_ptr, float64_t *op2_ptr, float64_t *res_ptr) {
	res_ptr[0] = op1_ptr[0] / op2_ptr[0];
	res_ptr[1] = op1_ptr[1] / op2_ptr[1];
}

static zend_always_inline void qb_do_divide_3x_F32(float32_t *op1_ptr, float32_t *op2_ptr, float32_t *res_ptr) {
	res_ptr[0] = op1_ptr[0] / op2_ptr[0];
	res_ptr[1] = op1_ptr[1] / op2_ptr[1];
	res_ptr[2] = op1_ptr[2] / op2_ptr[2];
}

static zend_always_inline void qb_do_divide_3x_F64(float64_t *op1_ptr, float64_t *op2_ptr, float64_t *res_ptr) {
	res_ptr[0] = op1_ptr[0] / op2_ptr[0];
	res_ptr[1] = op1_ptr[1] / op2_ptr[1];
	res_ptr[2] = op1_ptr[2] / op2_ptr[2];
}

static zend_always_inline void qb_do_divide_4x_F32(float32_t *op1_ptr, float32_t *op2_ptr, float32_t *res_ptr) {
	res_ptr[0] = op1_ptr[0] / op2_ptr[0];
	res_ptr[1] = op1_ptr[1] / op2_ptr[1];
	res_ptr[2] = op1_ptr[2] / op2_ptr[2];
	res_ptr[3] = op1_ptr[3] / op2_ptr[3];
}

static zend_always_inline void qb_do_divide_4x_F64(float64_t *op1_ptr, float64_t *op2_ptr, float64_t *res_ptr) {
	res_ptr[0] = op1_ptr[0] / op2_ptr[0];
	res_ptr[1] = op1_ptr[1] / op2_ptr[1];
	res_ptr[2] = op1_ptr[2] / op2_ptr[2];
	res_ptr[3] = op1_ptr[3] / op2_ptr[3];
}

static zend_always_inline void qb_do_divide_F32(float32_t op1, float32_t op2, float32_t *res_ptr) {
	(*res_ptr) = op1 / op2;
}

static zend_always_inline void qb_do_divide_F64(float64_t op1, float64_t op2, float64_t *res_ptr) {
	(*res_ptr) = op1 / op2;
}

static zend_always_inline void qb_do_dot_product_2x_F32(float32_t *op1_ptr, float32_t *op2_ptr, float32_t *res_ptr) {
	(*res_ptr) = (op1_ptr[0] * op2_ptr[0]) + (op1_ptr[1] * op2_ptr[1]);
}

static zend_always_inline void qb_do_dot_product_2x_F64(float64_t *op1_ptr, float64_t *op2_ptr, float64_t *res_ptr) {
	(*res_ptr) = (op1_ptr[0] * op2_ptr[0]) + (op1_ptr[1] * op2_ptr[1]);
}

static zend_always_inline void qb_do_dot_product_3x_F32(float32_t *op1_ptr, float32_t *op2_ptr, float32_t *res_ptr) {
	(*res_ptr) = (op1_ptr[0] * op2_ptr[0]) + (op1_ptr[1] * op2_ptr[1]) + (op1_ptr[2] * op2_ptr[2]);
}

static zend_always_inline void qb_do_dot_product_3x_F64(float64_t *op1_ptr, float64_t *op2_ptr, float64_t *res_ptr) {
	(*res_ptr) = (op1_ptr[0] * op2_ptr[0]) + (op1_ptr[1] * op2_ptr[1]) + (op1_ptr[2] * op2_ptr[2]);
}

static zend_always_inline void qb_do_dot_product_4x_F32(float32_t *op1_ptr, float32_t *op2_ptr, float32_t *res_ptr) {
	(*res_ptr) = (op1_ptr[0] * op2_ptr[0]) + (op1_ptr[1] * op2_ptr[1]) + (op1_ptr[2] * op2_ptr[2]) + (op1_ptr[3] * op2_ptr[3]);
}

static zend_always_inline void qb_do_dot_product_4x_F64(float64_t *op1_ptr, float64_t *op2_ptr, float64_t *res_ptr) {
	(*res_ptr) = (op1_ptr[0] * op2_ptr[0]) + (op1_ptr[1] * op2_ptr[1]) + (op1_ptr[2] * op2_ptr[2]) + (op1_ptr[3] * op2_ptr[3]);
}

static zend_always_inline void qb_do_equal_F32(float32_t op1, float32_t op2, int32_t *res_ptr) {
	(*res_ptr) = (op1 == op2);
}

static zend_always_inline void qb_do_equal_F64(float64_t op1, float64_t op2, int32_t *res_ptr) {
	(*res_ptr) = (op1 == op2);
}

static zend_always_inline void qb_do_equal_I08(int8_t op1, int8_t op2, int32_t *res_ptr) {
	(*res_ptr) = (op1 == op2);
}

static zend_always_inline void qb_do_equal_I16(int16_t op1, int16_t op2, int32_t *res_ptr) {
	(*res_ptr) = (op1 == op2);
}

static zend_always_inline void qb_do_equal_I32(int32_t op1, int32_t op2, int32_t *res_ptr) {
	(*res_ptr) = (op1 == op2);
}

static zend_always_inline void qb_do_equal_I64(int64_t op1, int64_t op2, int32_t *res_ptr) {
	(*res_ptr) = (op1 == op2);
}

static zend_always_inline void qb_do_exp2_F32(float32_t op1, float32_t *res_ptr) {
	(*res_ptr) = exp2f(op1);
}

static zend_always_inline void qb_do_exp2_F64(float64_t op1, float64_t *res_ptr) {
	(*res_ptr) = exp2(op1);
}

static zend_always_inline void qb_do_exp_F32(float32_t op1, float32_t *res_ptr) {
	(*res_ptr) = expf(op1);
}

static zend_always_inline void qb_do_exp_F64(float64_t op1, float64_t *res_ptr) {
	(*res_ptr) = exp(op1);
}

static zend_always_inline void qb_do_exp_m1_F32(float32_t op1, float32_t *res_ptr) {
	(*res_ptr) = expm1f(op1);
}

static zend_always_inline void qb_do_exp_m1_F64(float64_t op1, float64_t *res_ptr) {
	(*res_ptr) = expm1(op1);
}

static zend_always_inline void qb_do_face_forward_2x_F32(float32_t *op1_ptr, float32_t *op2_ptr, float32_t *res_ptr) {
	float32_t dot_product = (op1_ptr[0] * op2_ptr[0]) + (op1_ptr[1] * op2_ptr[1]);
	if(dot_product > 0) {
		res_ptr[0] = op1_ptr[0];
		res_ptr[1] = op1_ptr[1];
	} else {
		res_ptr[0] = -op1_ptr[0];
		res_ptr[1] = -op1_ptr[1];
	}
}

static zend_always_inline void qb_do_face_forward_2x_F64(float64_t *op1_ptr, float64_t *op2_ptr, float64_t *res_ptr) {
	float64_t dot_product = (op1_ptr[0] * op2_ptr[0]) + (op1_ptr[1] * op2_ptr[1]);
	if(dot_product > 0) {
		res_ptr[0] = op1_ptr[0];
		res_ptr[1] = op1_ptr[1];
	} else {
		res_ptr[0] = -op1_ptr[0];
		res_ptr[1] = -op1_ptr[1];
	}
}

static zend_always_inline void qb_do_floor_F32(float32_t op1, float32_t *res_ptr) {
	(*res_ptr) = (float32_t) floorf(op1);
}

static zend_always_inline void qb_do_floor_F64(float64_t op1, float64_t *res_ptr) {
	(*res_ptr) = (float64_t) floor(op1);
}

static zend_always_inline void qb_do_floored_division_modulo_F32(float32_t op1, float32_t op2, float32_t *res_ptr) {
	(*res_ptr) = (float32_t) (op1 - op2 * floor(op1 / op2));
}

static zend_always_inline void qb_do_floored_division_modulo_F64(float64_t op1, float64_t op2, float64_t *res_ptr) {
	(*res_ptr) = (float64_t) (op1 - op2 * floor(op1 / op2));
}

static zend_always_inline void qb_do_fork_count_U32(qb_interpreter_context *__restrict cxt, uint32_t *res_ptr) {
	(*res_ptr) = cxt->fork_count;
}

static zend_always_inline void qb_do_fork_identifier_U32(qb_interpreter_context *__restrict cxt, uint32_t *res_ptr) {
	(*res_ptr) = cxt->fork_id;
}

static zend_always_inline void qb_do_fract_F32(float32_t op1, float32_t *res_ptr) {
	(*res_ptr) = op1 - floorf(op1);
}

static zend_always_inline void qb_do_fract_F64(float64_t op1, float64_t *res_ptr) {
	(*res_ptr) = op1 - floor(op1);
}

static zend_always_inline void qb_do_gather_2x_F32(uint32_t op1, float32_t *op2_ptr, float32_t *res_ptr) {
	float32_t v0, v1;
	v0 = op2_ptr[(op1 >> 0) & 0x0007];
	v1 = op2_ptr[(op1 >> 3) & 0x0007];
	res_ptr[0] = v0;
	res_ptr[1] = v1;
}

static zend_always_inline void qb_do_gather_2x_F64(uint32_t op1, float64_t *op2_ptr, float64_t *res_ptr) {
	float64_t v0, v1;
	v0 = op2_ptr[(op1 >> 0) & 0x0007];
	v1 = op2_ptr[(op1 >> 3) & 0x0007];
	res_ptr[0] = v0;
	res_ptr[1] = v1;
}

static zend_always_inline void qb_do_gather_2x_I08(uint32_t op1, int8_t *op2_ptr, int8_t *res_ptr) {
	int8_t v0, v1;
	v0 = op2_ptr[(op1 >> 0) & 0x0007];
	v1 = op2_ptr[(op1 >> 3) & 0x0007];
	res_ptr[0] = v0;
	res_ptr[1] = v1;
}

static zend_always_inline void qb_do_gather_2x_I16(uint32_t op1, int16_t *op2_ptr, int16_t *res_ptr) {
	int16_t v0, v1;
	v0 = op2_ptr[(op1 >> 0) & 0x0007];
	v1 = op2_ptr[(op1 >> 3) & 0x0007];
	res_ptr[0] = v0;
	res_ptr[1] = v1;
}

static zend_always_inline void qb_do_gather_2x_I32(uint32_t op1, int32_t *op2_ptr, int32_t *res_ptr) {
	int32_t v0, v1;
	v0 = op2_ptr[(op1 >> 0) & 0x0007];
	v1 = op2_ptr[(op1 >> 3) & 0x0007];
	res_ptr[0] = v0;
	res_ptr[1] = v1;
}

static zend_always_inline void qb_do_gather_2x_I64(uint32_t op1, int64_t *op2_ptr, int64_t *res_ptr) {
	int64_t v0, v1;
	v0 = op2_ptr[(op1 >> 0) & 0x0007];
	v1 = op2_ptr[(op1 >> 3) & 0x0007];
	res_ptr[0] = v0;
	res_ptr[1] = v1;
}

static zend_always_inline void qb_do_gather_3x_F32(uint32_t op1, float32_t *op2_ptr, float32_t *res_ptr) {
	float32_t v0, v1, v2;
	v0 = op2_ptr[(op1 >> 0) & 0x0007];
	v1 = op2_ptr[(op1 >> 3) & 0x0007];
	v2 = op2_ptr[(op1 >> 6) & 0x0007];
	res_ptr[0] = v0;
	res_ptr[1] = v1;
	res_ptr[2] = v2;
}

static zend_always_inline void qb_do_gather_3x_F64(uint32_t op1, float64_t *op2_ptr, float64_t *res_ptr) {
	float64_t v0, v1, v2;
	v0 = op2_ptr[(op1 >> 0) & 0x0007];
	v1 = op2_ptr[(op1 >> 3) & 0x0007];
	v2 = op2_ptr[(op1 >> 6) & 0x0007];
	res_ptr[0] = v0;
	res_ptr[1] = v1;
	res_ptr[2] = v2;
}

static zend_always_inline void qb_do_gather_3x_I08(uint32_t op1, int8_t *op2_ptr, int8_t *res_ptr) {
	int8_t v0, v1, v2;
	v0 = op2_ptr[(op1 >> 0) & 0x0007];
	v1 = op2_ptr[(op1 >> 3) & 0x0007];
	v2 = op2_ptr[(op1 >> 6) & 0x0007];
	res_ptr[0] = v0;
	res_ptr[1] = v1;
	res_ptr[2] = v2;
}

static zend_always_inline void qb_do_gather_3x_I16(uint32_t op1, int16_t *op2_ptr, int16_t *res_ptr) {
	int16_t v0, v1, v2;
	v0 = op2_ptr[(op1 >> 0) & 0x0007];
	v1 = op2_ptr[(op1 >> 3) & 0x0007];
	v2 = op2_ptr[(op1 >> 6) & 0x0007];
	res_ptr[0] = v0;
	res_ptr[1] = v1;
	res_ptr[2] = v2;
}

static zend_always_inline void qb_do_gather_3x_I32(uint32_t op1, int32_t *op2_ptr, int32_t *res_ptr) {
	int32_t v0, v1, v2;
	v0 = op2_ptr[(op1 >> 0) & 0x0007];
	v1 = op2_ptr[(op1 >> 3) & 0x0007];
	v2 = op2_ptr[(op1 >> 6) & 0x0007];
	res_ptr[0] = v0;
	res_ptr[1] = v1;
	res_ptr[2] = v2;
}

static zend_always_inline void qb_do_gather_3x_I64(uint32_t op1, int64_t *op2_ptr, int64_t *res_ptr) {
	int64_t v0, v1, v2;
	v0 = op2_ptr[(op1 >> 0) & 0x0007];
	v1 = op2_ptr[(op1 >> 3) & 0x0007];
	v2 = op2_ptr[(op1 >> 6) & 0x0007];
	res_ptr[0] = v0;
	res_ptr[1] = v1;
	res_ptr[2] = v2;
}

static zend_always_inline void qb_do_hsl2rgb_3x_F32(float32_t *op1_ptr, float32_t *res_ptr) {
	qb_convert_hsl_to_rgb_F32(op1_ptr, res_ptr);
}

static zend_always_inline void qb_do_hsl2rgb_3x_F64(float64_t *op1_ptr, float64_t *res_ptr) {
	qb_convert_hsl_to_rgb_F64(op1_ptr, res_ptr);
}

static zend_always_inline void qb_do_hsl2rgb_4x_F32(float32_t *op1_ptr, float32_t *res_ptr) {
	qb_convert_hsl_to_rgb_F32(op1_ptr, res_ptr);
	res_ptr[3] = op1_ptr[3];
}

static zend_always_inline void qb_do_hsl2rgb_4x_F64(float64_t *op1_ptr, float64_t *res_ptr) {
	qb_convert_hsl_to_rgb_F64(op1_ptr, res_ptr);
	res_ptr[3] = op1_ptr[3];
}

static zend_always_inline void qb_do_hsv2rgb_3x_F32(float32_t *op1_ptr, float32_t *res_ptr) {
	qb_convert_hsv_to_rgb_F32(op1_ptr, res_ptr);
}

static zend_always_inline void qb_do_hsv2rgb_3x_F64(float64_t *op1_ptr, float64_t *res_ptr) {
	qb_convert_hsv_to_rgb_F64(op1_ptr, res_ptr);
}

static zend_always_inline void qb_do_hsv2rgb_4x_F32(float32_t *op1_ptr, float32_t *res_ptr) {
	qb_convert_hsv_to_rgb_F32(op1_ptr, res_ptr);
	res_ptr[3] = op1_ptr[3];
}

static zend_always_inline void qb_do_hsv2rgb_4x_F64(float64_t *op1_ptr, float64_t *res_ptr) {
	qb_convert_hsv_to_rgb_F64(op1_ptr, res_ptr);
	res_ptr[3] = op1_ptr[3];
}

static zend_always_inline void qb_do_hypot_F32(float32_t op1, float32_t op2, float32_t *res_ptr) {
	(*res_ptr) = (float32_t) hypotf(op1, op2);
}

static zend_always_inline void qb_do_hypot_F64(float64_t op1, float64_t op2, float64_t *res_ptr) {
	(*res_ptr) = (float64_t) hypot(op1, op2);
}

static zend_always_inline void qb_do_increment_2x_F32(float32_t *res_ptr) {
	++res_ptr[0];
	++res_ptr[1];
}

static zend_always_inline void qb_do_increment_2x_F64(float64_t *res_ptr) {
	++res_ptr[0];
	++res_ptr[1];
}

static zend_always_inline void qb_do_increment_3x_F32(float32_t *res_ptr) {
	++res_ptr[0];
	++res_ptr[1];
	++res_ptr[2];
}

static zend_always_inline void qb_do_increment_3x_F64(float64_t *res_ptr) {
	++res_ptr[0];
	++res_ptr[1];
	++res_ptr[2];
}

static zend_always_inline void qb_do_increment_4x_F32(float32_t *res_ptr) {
	++res_ptr[0];
	++res_ptr[1];
	++res_ptr[2];
	++res_ptr[3];
}

static zend_always_inline void qb_do_increment_4x_F64(float64_t *res_ptr) {
	++res_ptr[0];
	++res_ptr[1];
	++res_ptr[2];
	++res_ptr[3];
}

static zend_always_inline void qb_do_increment_F32(float32_t *res_ptr) {
	++(*res_ptr);
}

static zend_always_inline void qb_do_increment_F64(float64_t *res_ptr) {
	++(*res_ptr);
}

static zend_always_inline void qb_do_increment_I08(int8_t *res_ptr) {
	++(*res_ptr);
}

static zend_always_inline void qb_do_increment_I16(int16_t *res_ptr) {
	++(*res_ptr);
}

static zend_always_inline void qb_do_increment_I32(int32_t *res_ptr) {
	++(*res_ptr);
}

static zend_always_inline void qb_do_increment_I64(int64_t *res_ptr) {
	++(*res_ptr);
}

static zend_always_inline void qb_do_increment_branch_on_greater_than_U32(int32_t *condition_ptr, uint32_t op1, uint32_t *res_ptr) {
	(*condition_ptr) = (op1 > ++(*res_ptr));
}

static zend_always_inline void qb_do_length_2x_F32(float32_t *op1_ptr, float32_t *res_ptr) {
	float32_t sum = (op1_ptr[0] * op1_ptr[0]) + (op1_ptr[1] * op1_ptr[1]);
	(*res_ptr) = qb_fast_sqrtf(sum);
}

static zend_always_inline void qb_do_length_2x_F64(float64_t *op1_ptr, float64_t *res_ptr) {
	float64_t sum = (op1_ptr[0] * op1_ptr[0]) + (op1_ptr[1] * op1_ptr[1]);
	(*res_ptr) = qb_fast_sqrt(sum);
}

static zend_always_inline void qb_do_length_3x_F32(float32_t *op1_ptr, float32_t *res_ptr) {
	float32_t sum = (op1_ptr[0] * op1_ptr[0]) + (op1_ptr[1] * op1_ptr[1]) + (op1_ptr[2] * op1_ptr[2]);
	(*res_ptr) = qb_fast_sqrtf(sum);
}

static zend_always_inline void qb_do_length_3x_F64(float64_t *op1_ptr, float64_t *res_ptr) {
	float64_t sum = (op1_ptr[0] * op1_ptr[0]) + (op1_ptr[1] * op1_ptr[1]) + (op1_ptr[2] * op1_ptr[2]);
	(*res_ptr) = qb_fast_sqrt(sum);
}

static zend_always_inline void qb_do_length_4x_F32(float32_t *op1_ptr, float32_t *res_ptr) {
	float32_t sum = (op1_ptr[0] * op1_ptr[0]) + (op1_ptr[1] * op1_ptr[1]) + (op1_ptr[2] * op1_ptr[2]) + (op1_ptr[3] * op1_ptr[3]);
	(*res_ptr) = qb_fast_sqrtf(sum);
}

static zend_always_inline void qb_do_length_4x_F64(float64_t *op1_ptr, float64_t *res_ptr) {
	float64_t sum = (op1_ptr[0] * op1_ptr[0]) + (op1_ptr[1] * op1_ptr[1]) + (op1_ptr[2] * op1_ptr[2]) + (op1_ptr[3] * op1_ptr[3]);
	(*res_ptr) = qb_fast_sqrt(sum);
}

static zend_always_inline void qb_do_less_than_F32(float32_t op1, float32_t op2, int32_t *res_ptr) {
	(*res_ptr) = (op1 < op2);
}

static zend_always_inline void qb_do_less_than_F64(float64_t op1, float64_t op2, int32_t *res_ptr) {
	(*res_ptr) = (op1 < op2);
}

static zend_always_inline void qb_do_less_than_S08(int8_t op1, int8_t op2, int32_t *res_ptr) {
	(*res_ptr) = (op1 < op2);
}

static zend_always_inline void qb_do_less_than_S16(int16_t op1, int16_t op2, int32_t *res_ptr) {
	(*res_ptr) = (op1 < op2);
}

static zend_always_inline void qb_do_less_than_S32(int32_t op1, int32_t op2, int32_t *res_ptr) {
	(*res_ptr) = (op1 < op2);
}

static zend_always_inline void qb_do_less_than_S64(int64_t op1, int64_t op2, int32_t *res_ptr) {
	(*res_ptr) = (op1 < op2);
}

static zend_always_inline void qb_do_less_than_U08(uint8_t op1, uint8_t op2, int32_t *res_ptr) {
	(*res_ptr) = (op1 < op2);
}

static zend_always_inline void qb_do_less_than_U16(uint16_t op1, uint16_t op2, int32_t *res_ptr) {
	(*res_ptr) = (op1 < op2);
}

static zend_always_inline void qb_do_less_than_U32(uint32_t op1, uint32_t op2, int32_t *res_ptr) {
	(*res_ptr) = (op1 < op2);
}

static zend_always_inline void qb_do_less_than_U64(uint64_t op1, uint64_t op2, int32_t *res_ptr) {
	(*res_ptr) = (op1 < op2);
}

static zend_always_inline void qb_do_less_than_equal_F32(float32_t op1, float32_t op2, int32_t *res_ptr) {
	(*res_ptr) = (op1 <= op2);
}

static zend_always_inline void qb_do_less_than_equal_F64(float64_t op1, float64_t op2, int32_t *res_ptr) {
	(*res_ptr) = (op1 <= op2);
}

static zend_always_inline void qb_do_less_than_equal_S08(int8_t op1, int8_t op2, int32_t *res_ptr) {
	(*res_ptr) = (op1 <= op2);
}

static zend_always_inline void qb_do_less_than_equal_S16(int16_t op1, int16_t op2, int32_t *res_ptr) {
	(*res_ptr) = (op1 <= op2);
}

static zend_always_inline void qb_do_less_than_equal_S32(int32_t op1, int32_t op2, int32_t *res_ptr) {
	(*res_ptr) = (op1 <= op2);
}

static zend_always_inline void qb_do_less_than_equal_S64(int64_t op1, int64_t op2, int32_t *res_ptr) {
	(*res_ptr) = (op1 <= op2);
}

static zend_always_inline void qb_do_less_than_equal_U08(uint8_t op1, uint8_t op2, int32_t *res_ptr) {
	(*res_ptr) = (op1 <= op2);
}

static zend_always_inline void qb_do_less_than_equal_U16(uint16_t op1, uint16_t op2, int32_t *res_ptr) {
	(*res_ptr) = (op1 <= op2);
}

static zend_always_inline void qb_do_less_than_equal_U32(uint32_t op1, uint32_t op2, int32_t *res_ptr) {
	(*res_ptr) = (op1 <= op2);
}

static zend_always_inline void qb_do_less_than_equal_U64(uint64_t op1, uint64_t op2, int32_t *res_ptr) {
	(*res_ptr) = (op1 <= op2);
}

static zend_always_inline void qb_do_log10_F32(float32_t op1, float32_t *res_ptr) {
	(*res_ptr) = log10f(op1);
}

static zend_always_inline void qb_do_log10_F64(float64_t op1, float64_t *res_ptr) {
	(*res_ptr) = log10(op1);
}

static zend_always_inline void qb_do_log1p_F32(float32_t op1, float32_t *res_ptr) {
	(*res_ptr) = log1pf(op1);
}

static zend_always_inline void qb_do_log1p_F64(float64_t op1, float64_t *res_ptr) {
	(*res_ptr) = log1p(op1);
}

static zend_always_inline void qb_do_log2_F32(float32_t op1, float32_t *res_ptr) {
	(*res_ptr) = log2f(op1);
}

static zend_always_inline void qb_do_log2_F64(float64_t op1, float64_t *res_ptr) {
	(*res_ptr) = log2(op1);
}

static zend_always_inline void qb_do_log_F32(float32_t op1, float32_t *res_ptr) {
	(*res_ptr) = logf(op1);
}

static zend_always_inline void qb_do_log_F64(float64_t op1, float64_t *res_ptr) {
	(*res_ptr) = log(op1);
}

static zend_always_inline void qb_do_logical_and_I32(int32_t op1, int32_t op2, int32_t *res_ptr) {
	(*res_ptr) = op1 && op2;
}

static zend_always_inline void qb_do_logical_not_I32(int32_t op1, int32_t *res_ptr) {
	(*res_ptr) = !op1;
}

static zend_always_inline void qb_do_logical_or_I32(int32_t op1, int32_t op2, int32_t *res_ptr) {
	(*res_ptr) = op1 || op2;
}

static zend_always_inline void qb_do_logical_xor_I32(int32_t op1, int32_t op2, int32_t *res_ptr) {
	(*res_ptr) = !op1 != !op2;
}

static zend_always_inline void qb_do_max_F32(float32_t op1, float32_t op2, float32_t *res_ptr) {
	(*res_ptr) = (op1 > op2) ? op1 : op2;
}

static zend_always_inline void qb_do_max_F64(float64_t op1, float64_t op2, float64_t *res_ptr) {
	(*res_ptr) = (op1 > op2) ? op1 : op2;
}

static zend_always_inline void qb_do_max_S08(int8_t op1, int8_t op2, int8_t *res_ptr) {
	(*res_ptr) = (op1 > op2) ? op1 : op2;
}

static zend_always_inline void qb_do_max_S16(int16_t op1, int16_t op2, int16_t *res_ptr) {
	(*res_ptr) = (op1 > op2) ? op1 : op2;
}

static zend_always_inline void qb_do_max_S32(int32_t op1, int32_t op2, int32_t *res_ptr) {
	(*res_ptr) = (op1 > op2) ? op1 : op2;
}

static zend_always_inline void qb_do_max_S64(int64_t op1, int64_t op2, int64_t *res_ptr) {
	(*res_ptr) = (op1 > op2) ? op1 : op2;
}

static zend_always_inline void qb_do_max_U08(uint8_t op1, uint8_t op2, uint8_t *res_ptr) {
	(*res_ptr) = (op1 > op2) ? op1 : op2;
}

static zend_always_inline void qb_do_max_U16(uint16_t op1, uint16_t op2, uint16_t *res_ptr) {
	(*res_ptr) = (op1 > op2) ? op1 : op2;
}

static zend_always_inline void qb_do_max_U32(uint32_t op1, uint32_t op2, uint32_t *res_ptr) {
	(*res_ptr) = (op1 > op2) ? op1 : op2;
}

static zend_always_inline void qb_do_max_U64(uint64_t op1, uint64_t op2, uint64_t *res_ptr) {
	(*res_ptr) = (op1 > op2) ? op1 : op2;
}

static zend_always_inline void qb_do_min_F32(float32_t op1, float32_t op2, float32_t *res_ptr) {
	(*res_ptr) = (op1 < op2) ? op1 : op2;
}

static zend_always_inline void qb_do_min_F64(float64_t op1, float64_t op2, float64_t *res_ptr) {
	(*res_ptr) = (op1 < op2) ? op1 : op2;
}

static zend_always_inline void qb_do_min_S08(int8_t op1, int8_t op2, int8_t *res_ptr) {
	(*res_ptr) = (op1 < op2) ? op1 : op2;
}

static zend_always_inline void qb_do_min_S16(int16_t op1, int16_t op2, int16_t *res_ptr) {
	(*res_ptr) = (op1 < op2) ? op1 : op2;
}

static zend_always_inline void qb_do_min_S32(int32_t op1, int32_t op2, int32_t *res_ptr) {
	(*res_ptr) = (op1 < op2) ? op1 : op2;
}

static zend_always_inline void qb_do_min_S64(int64_t op1, int64_t op2, int64_t *res_ptr) {
	(*res_ptr) = (op1 < op2) ? op1 : op2;
}

static zend_always_inline void qb_do_min_U08(uint8_t op1, uint8_t op2, uint8_t *res_ptr) {
	(*res_ptr) = (op1 < op2) ? op1 : op2;
}

static zend_always_inline void qb_do_min_U16(uint16_t op1, uint16_t op2, uint16_t *res_ptr) {
	(*res_ptr) = (op1 < op2) ? op1 : op2;
}

static zend_always_inline void qb_do_min_U32(uint32_t op1, uint32_t op2, uint32_t *res_ptr) {
	(*res_ptr) = (op1 < op2) ? op1 : op2;
}

static zend_always_inline void qb_do_min_U64(uint64_t op1, uint64_t op2, uint64_t *res_ptr) {
	(*res_ptr) = (op1 < op2) ? op1 : op2;
}

static zend_always_inline void qb_do_mix_F32(float32_t op1, float32_t op2, float32_t op3, float32_t *res_ptr) {
	(*res_ptr) = op1 * (1 - op3) + op2 * op3;
}

static zend_always_inline void qb_do_mix_F64(float64_t op1, float64_t op2, float64_t op3, float64_t *res_ptr) {
	(*res_ptr) = op1 * (1 - op3) + op2 * op3;
}

static zend_always_inline void qb_do_modulo_2x_F32(float32_t *op1_ptr, float32_t *op2_ptr, float32_t *res_ptr) {
	res_ptr[0] = fmodf(op1_ptr[0], op2_ptr[0]);
	res_ptr[1] = fmodf(op1_ptr[1], op2_ptr[1]);
}

static zend_always_inline void qb_do_modulo_2x_F64(float64_t *op1_ptr, float64_t *op2_ptr, float64_t *res_ptr) {
	res_ptr[0] = fmod(op1_ptr[0], op2_ptr[0]);
	res_ptr[1] = fmod(op1_ptr[1], op2_ptr[1]);
}

static zend_always_inline void qb_do_modulo_3x_F32(float32_t *op1_ptr, float32_t *op2_ptr, float32_t *res_ptr) {
	res_ptr[0] = fmodf(op1_ptr[0], op2_ptr[0]);
	res_ptr[1] = fmodf(op1_ptr[1], op2_ptr[1]);
	res_ptr[2] = fmodf(op1_ptr[2], op2_ptr[2]);
}

static zend_always_inline void qb_do_modulo_3x_F64(float64_t *op1_ptr, float64_t *op2_ptr, float64_t *res_ptr) {
	res_ptr[0] = fmod(op1_ptr[0], op2_ptr[0]);
	res_ptr[1] = fmod(op1_ptr[1], op2_ptr[1]);
	res_ptr[2] = fmod(op1_ptr[2], op2_ptr[2]);
}

static zend_always_inline void qb_do_modulo_4x_F32(float32_t *op1_ptr, float32_t *op2_ptr, float32_t *res_ptr) {
	res_ptr[0] = fmodf(op1_ptr[0], op2_ptr[0]);
	res_ptr[1] = fmodf(op1_ptr[1], op2_ptr[1]);
	res_ptr[2] = fmodf(op1_ptr[2], op2_ptr[2]);
	res_ptr[3] = fmodf(op1_ptr[3], op2_ptr[3]);
}

static zend_always_inline void qb_do_modulo_4x_F64(float64_t *op1_ptr, float64_t *op2_ptr, float64_t *res_ptr) {
	res_ptr[0] = fmod(op1_ptr[0], op2_ptr[0]);
	res_ptr[1] = fmod(op1_ptr[1], op2_ptr[1]);
	res_ptr[2] = fmod(op1_ptr[2], op2_ptr[2]);
	res_ptr[3] = fmod(op1_ptr[3], op2_ptr[3]);
}

static zend_always_inline void qb_do_modulo_F32(float32_t op1, float32_t op2, float32_t *res_ptr) {
	(*res_ptr) = fmodf(op1, op2);
}

static zend_always_inline void qb_do_modulo_F64(float64_t op1, float64_t op2, float64_t *res_ptr) {
	(*res_ptr) = fmod(op1, op2);
}

static zend_always_inline void qb_do_multiply_2x_F32(float32_t *op1_ptr, float32_t *op2_ptr, float32_t *res_ptr) {
	res_ptr[0] = op1_ptr[0] * op2_ptr[0];
	res_ptr[1] = op1_ptr[1] * op2_ptr[1];
}

static zend_always_inline void qb_do_multiply_2x_F64(float64_t *op1_ptr, float64_t *op2_ptr, float64_t *res_ptr) {
	res_ptr[0] = op1_ptr[0] * op2_ptr[0];
	res_ptr[1] = op1_ptr[1] * op2_ptr[1];
}

static zend_always_inline void qb_do_multiply_3x_F32(float32_t *op1_ptr, float32_t *op2_ptr, float32_t *res_ptr) {
	res_ptr[0] = op1_ptr[0] * op2_ptr[0];
	res_ptr[1] = op1_ptr[1] * op2_ptr[1];
	res_ptr[2] = op1_ptr[2] * op2_ptr[2];
}

static zend_always_inline void qb_do_multiply_3x_F64(float64_t *op1_ptr, float64_t *op2_ptr, float64_t *res_ptr) {
	res_ptr[0] = op1_ptr[0] * op2_ptr[0];
	res_ptr[1] = op1_ptr[1] * op2_ptr[1];
	res_ptr[2] = op1_ptr[2] * op2_ptr[2];
}

static zend_always_inline void qb_do_multiply_4x_F32(float32_t *op1_ptr, float32_t *op2_ptr, float32_t *res_ptr) {
	res_ptr[0] = op1_ptr[0] * op2_ptr[0];
	res_ptr[1] = op1_ptr[1] * op2_ptr[1];
	res_ptr[2] = op1_ptr[2] * op2_ptr[2];
	res_ptr[3] = op1_ptr[3] * op2_ptr[3];
}

static zend_always_inline void qb_do_multiply_4x_F64(float64_t *op1_ptr, float64_t *op2_ptr, float64_t *res_ptr) {
	res_ptr[0] = op1_ptr[0] * op2_ptr[0];
	res_ptr[1] = op1_ptr[1] * op2_ptr[1];
	res_ptr[2] = op1_ptr[2] * op2_ptr[2];
	res_ptr[3] = op1_ptr[3] * op2_ptr[3];
}

static zend_always_inline void qb_do_multiply_F32(float32_t op1, float32_t op2, float32_t *res_ptr) {
	(*res_ptr) = op1 * op2;
}

static zend_always_inline void qb_do_multiply_F64(float64_t op1, float64_t op2, float64_t *res_ptr) {
	(*res_ptr) = op1 * op2;
}

static zend_always_inline void qb_do_multiply_S08(int8_t op1, int8_t op2, int8_t *res_ptr) {
	(*res_ptr) = op1 * op2;
}

static zend_always_inline void qb_do_multiply_S16(int16_t op1, int16_t op2, int16_t *res_ptr) {
	(*res_ptr) = op1 * op2;
}

static zend_always_inline void qb_do_multiply_S32(int32_t op1, int32_t op2, int32_t *res_ptr) {
	(*res_ptr) = op1 * op2;
}

static zend_always_inline void qb_do_multiply_S64(int64_t op1, int64_t op2, int64_t *res_ptr) {
	(*res_ptr) = op1 * op2;
}

static zend_always_inline void qb_do_multiply_U08(uint8_t op1, uint8_t op2, uint8_t *res_ptr) {
	(*res_ptr) = op1 * op2;
}

static zend_always_inline void qb_do_multiply_U16(uint16_t op1, uint16_t op2, uint16_t *res_ptr) {
	(*res_ptr) = op1 * op2;
}

static zend_always_inline void qb_do_multiply_U32(uint32_t op1, uint32_t op2, uint32_t *res_ptr) {
	(*res_ptr) = op1 * op2;
}

static zend_always_inline void qb_do_multiply_U64(uint64_t op1, uint64_t op2, uint64_t *res_ptr) {
	(*res_ptr) = op1 * op2;
}

static zend_always_inline void qb_do_multiply_accumulate_2x_F32(float32_t *op1_ptr, float32_t *op2_ptr, float32_t *op3_ptr, float32_t *res_ptr) {
	res_ptr[0] = (op1_ptr[0] * op2_ptr[0]) + op3_ptr[0];
	res_ptr[1] = (op1_ptr[1] * op2_ptr[1]) + op3_ptr[1];
}

static zend_always_inline void qb_do_multiply_accumulate_2x_F64(float64_t *op1_ptr, float64_t *op2_ptr, float64_t *op3_ptr, float64_t *res_ptr) {
	res_ptr[0] = (op1_ptr[0] * op2_ptr[0]) + op3_ptr[0];
	res_ptr[1] = (op1_ptr[1] * op2_ptr[1]) + op3_ptr[1];
}

static zend_always_inline void qb_do_multiply_accumulate_3x_F32(float32_t *op1_ptr, float32_t *op2_ptr, float32_t *op3_ptr, float32_t *res_ptr) {
	res_ptr[0] = (op1_ptr[0] * op2_ptr[0]) + op3_ptr[0];
	res_ptr[1] = (op1_ptr[1] * op2_ptr[1]) + op3_ptr[1];
	res_ptr[2] = (op1_ptr[2] * op2_ptr[2]) + op3_ptr[2];
}

static zend_always_inline void qb_do_multiply_accumulate_3x_F64(float64_t *op1_ptr, float64_t *op2_ptr, float64_t *op3_ptr, float64_t *res_ptr) {
	res_ptr[0] = (op1_ptr[0] * op2_ptr[0]) + op3_ptr[0];
	res_ptr[1] = (op1_ptr[1] * op2_ptr[1]) + op3_ptr[1];
	res_ptr[2] = (op1_ptr[2] * op2_ptr[2]) + op3_ptr[2];
}

static zend_always_inline void qb_do_multiply_accumulate_4x_F32(float32_t *op1_ptr, float32_t *op2_ptr, float32_t *op3_ptr, float32_t *res_ptr) {
	res_ptr[0] = (op1_ptr[0] * op2_ptr[0]) + op3_ptr[0];
	res_ptr[1] = (op1_ptr[1] * op2_ptr[1]) + op3_ptr[1];
	res_ptr[2] = (op1_ptr[2] * op2_ptr[2]) + op3_ptr[2];
	res_ptr[3] = (op1_ptr[3] * op2_ptr[3]) + op3_ptr[3];
}

static zend_always_inline void qb_do_multiply_accumulate_4x_F64(float64_t *op1_ptr, float64_t *op2_ptr, float64_t *op3_ptr, float64_t *res_ptr) {
	res_ptr[0] = (op1_ptr[0] * op2_ptr[0]) + op3_ptr[0];
	res_ptr[1] = (op1_ptr[1] * op2_ptr[1]) + op3_ptr[1];
	res_ptr[2] = (op1_ptr[2] * op2_ptr[2]) + op3_ptr[2];
	res_ptr[3] = (op1_ptr[3] * op2_ptr[3]) + op3_ptr[3];
}

static zend_always_inline void qb_do_multiply_accumulate_F32(float32_t op1, float32_t op2, float32_t op3, float32_t *res_ptr) {
	(*res_ptr) = (op1 * op2) + op3;
}

static zend_always_inline void qb_do_multiply_accumulate_F64(float64_t op1, float64_t op2, float64_t op3, float64_t *res_ptr) {
	(*res_ptr) = (op1 * op2) + op3;
}

static zend_always_inline void qb_do_multiply_accumulate_S32(int32_t op1, int32_t op2, int32_t op3, int32_t *res_ptr) {
	(*res_ptr) = (op1 * op2) + op3;
}

static zend_always_inline void qb_do_multiply_accumulate_S64(int64_t op1, int64_t op2, int64_t op3, int64_t *res_ptr) {
	(*res_ptr) = (op1 * op2) + op3;
}

static zend_always_inline void qb_do_multiply_accumulate_U32(uint32_t op1, uint32_t op2, uint32_t op3, uint32_t *res_ptr) {
	(*res_ptr) = (op1 * op2) + op3;
}

static zend_always_inline void qb_do_multiply_accumulate_U64(uint64_t op1, uint64_t op2, uint64_t op3, uint64_t *res_ptr) {
	(*res_ptr) = (op1 * op2) + op3;
}

static zend_always_inline void qb_do_multiply_matrix_by_matrix_2x_column_major_F32(float32_t *op1_ptr, float32_t *op2_ptr, float32_t *res_ptr) {
	float32_t dot_product0 = (op1_ptr[0 * 2 + 0] * op2_ptr[0 * 2 + 0]) + (op1_ptr[0 * 2 + 1] * op2_ptr[1 * 2 + 0]);
	float32_t dot_product1 = (op1_ptr[0 * 2 + 0] * op2_ptr[0 * 2 + 1]) + (op1_ptr[0 * 2 + 1] * op2_ptr[1 * 2 + 1]);
	float32_t dot_product2 = (op1_ptr[1 * 2 + 0] * op2_ptr[0 * 2 + 0]) + (op1_ptr[1 * 2 + 1] * op2_ptr[1 * 2 + 0]);
	float32_t dot_product3 = (op1_ptr[1 * 2 + 0] * op2_ptr[0 * 2 + 1]) + (op1_ptr[1 * 2 + 1] * op2_ptr[1 * 2 + 1]);
	res_ptr[0 * 2 + 0] = dot_product0;
	res_ptr[0 * 2 + 1] = dot_product1;
	res_ptr[1 * 2 + 0] = dot_product2;
	res_ptr[1 * 2 + 1] = dot_product3;
}

static zend_always_inline void qb_do_multiply_matrix_by_matrix_2x_column_major_F64(float64_t *op1_ptr, float64_t *op2_ptr, float64_t *res_ptr) {
	float64_t dot_product0 = (op1_ptr[0 * 2 + 0] * op2_ptr[0 * 2 + 0]) + (op1_ptr[0 * 2 + 1] * op2_ptr[1 * 2 + 0]);
	float64_t dot_product1 = (op1_ptr[0 * 2 + 0] * op2_ptr[0 * 2 + 1]) + (op1_ptr[0 * 2 + 1] * op2_ptr[1 * 2 + 1]);
	float64_t dot_product2 = (op1_ptr[1 * 2 + 0] * op2_ptr[0 * 2 + 0]) + (op1_ptr[1 * 2 + 1] * op2_ptr[1 * 2 + 0]);
	float64_t dot_product3 = (op1_ptr[1 * 2 + 0] * op2_ptr[0 * 2 + 1]) + (op1_ptr[1 * 2 + 1] * op2_ptr[1 * 2 + 1]);
	res_ptr[0 * 2 + 0] = dot_product0;
	res_ptr[0 * 2 + 1] = dot_product1;
	res_ptr[1 * 2 + 0] = dot_product2;
	res_ptr[1 * 2 + 1] = dot_product3;
}

static zend_always_inline void qb_do_multiply_matrix_by_vector_2x_column_major_F32(float32_t *op1_ptr, float32_t *op2_ptr, float32_t *res_ptr) {
	float32_t dot_product0 = (op1_ptr[0 * 2 + 0] * op2_ptr[0]) + (op1_ptr[1 * 2 + 0] * op2_ptr[1]);
	float32_t dot_product1 = (op1_ptr[0 * 2 + 1] * op2_ptr[0]) + (op1_ptr[1 * 2 + 1] * op2_ptr[1]);
	res_ptr[0] = dot_product0;
	res_ptr[1] = dot_product1;
}

static zend_always_inline void qb_do_multiply_matrix_by_vector_2x_column_major_F64(float64_t *op1_ptr, float64_t *op2_ptr, float64_t *res_ptr) {
	float64_t dot_product0 = (op1_ptr[0 * 2 + 0] * op2_ptr[0]) + (op1_ptr[1 * 2 + 0] * op2_ptr[1]);
	float64_t dot_product1 = (op1_ptr[0 * 2 + 1] * op2_ptr[0]) + (op1_ptr[1 * 2 + 1] * op2_ptr[1]);
	res_ptr[0] = dot_product0;
	res_ptr[1] = dot_product1;
}

static zend_always_inline void qb_do_multiply_matrix_by_vector_3x_column_major_F32(float32_t *op1_ptr, float32_t *op2_ptr, float32_t *res_ptr) {
	float32_t dot_product0 = (op1_ptr[0 * 3 + 0] * op2_ptr[0]) + (op1_ptr[1 * 3 + 0] * op2_ptr[1]) + (op1_ptr[2 * 3 + 0] * op2_ptr[2]);
	float32_t dot_product1 = (op1_ptr[0 * 3 + 1] * op2_ptr[0]) + (op1_ptr[1 * 3 + 1] * op2_ptr[1]) + (op1_ptr[2 * 3 + 1] * op2_ptr[2]);
	float32_t dot_product2 = (op1_ptr[0 * 3 + 2] * op2_ptr[0]) + (op1_ptr[1 * 3 + 2] * op2_ptr[1]) + (op1_ptr[2 * 3 + 2] * op2_ptr[2]);
	res_ptr[0] = dot_product0;
	res_ptr[1] = dot_product1;
	res_ptr[2] = dot_product2;
}

static zend_always_inline void qb_do_multiply_matrix_by_vector_3x_column_major_F64(float64_t *op1_ptr, float64_t *op2_ptr, float64_t *res_ptr) {
	float64_t dot_product0 = (op1_ptr[0 * 3 + 0] * op2_ptr[0]) + (op1_ptr[1 * 3 + 0] * op2_ptr[1]) + (op1_ptr[2 * 3 + 0] * op2_ptr[2]);
	float64_t dot_product1 = (op1_ptr[0 * 3 + 1] * op2_ptr[0]) + (op1_ptr[1 * 3 + 1] * op2_ptr[1]) + (op1_ptr[2 * 3 + 1] * op2_ptr[2]);
	float64_t dot_product2 = (op1_ptr[0 * 3 + 2] * op2_ptr[0]) + (op1_ptr[1 * 3 + 2] * op2_ptr[1]) + (op1_ptr[2 * 3 + 2] * op2_ptr[2]);
	res_ptr[0] = dot_product0;
	res_ptr[1] = dot_product1;
	res_ptr[2] = dot_product2;
}

static zend_always_inline void qb_do_multiply_matrix_by_vector_4x_column_major_F32(float32_t *op1_ptr, float32_t *op2_ptr, float32_t *res_ptr) {
	float32_t dot_product0 = (op1_ptr[0 * 4 + 0] * op2_ptr[0]) + (op1_ptr[1 * 4 + 0] * op2_ptr[1]) + (op1_ptr[2 * 4 + 0] * op2_ptr[2]) + (op1_ptr[3 * 4 + 0] * op2_ptr[3]);
	float32_t dot_product1 = (op1_ptr[0 * 4 + 1] * op2_ptr[0]) + (op1_ptr[1 * 4 + 1] * op2_ptr[1]) + (op1_ptr[2 * 4 + 1] * op2_ptr[2]) + (op1_ptr[3 * 4 + 1] * op2_ptr[3]);
	float32_t dot_product2 = (op1_ptr[0 * 4 + 2] * op2_ptr[0]) + (op1_ptr[1 * 4 + 2] * op2_ptr[1]) + (op1_ptr[2 * 4 + 2] * op2_ptr[2]) + (op1_ptr[3 * 4 + 2] * op2_ptr[3]);
	float32_t dot_product3 = (op1_ptr[0 * 4 + 3] * op2_ptr[0]) + (op1_ptr[1 * 4 + 3] * op2_ptr[1]) + (op1_ptr[2 * 4 + 3] * op2_ptr[2]) + (op1_ptr[3 * 4 + 3] * op2_ptr[3]);
	res_ptr[0] = dot_product0;
	res_ptr[1] = dot_product1;
	res_ptr[2] = dot_product2;
	res_ptr[3] = dot_product3;
}

static zend_always_inline void qb_do_multiply_matrix_by_vector_4x_column_major_F64(float64_t *op1_ptr, float64_t *op2_ptr, float64_t *res_ptr) {
	float64_t dot_product0 = (op1_ptr[0 * 4 + 0] * op2_ptr[0]) + (op1_ptr[1 * 4 + 0] * op2_ptr[1]) + (op1_ptr[2 * 4 + 0] * op2_ptr[2]) + (op1_ptr[3 * 4 + 0] * op2_ptr[3]);
	float64_t dot_product1 = (op1_ptr[0 * 4 + 1] * op2_ptr[0]) + (op1_ptr[1 * 4 + 1] * op2_ptr[1]) + (op1_ptr[2 * 4 + 1] * op2_ptr[2]) + (op1_ptr[3 * 4 + 1] * op2_ptr[3]);
	float64_t dot_product2 = (op1_ptr[0 * 4 + 2] * op2_ptr[0]) + (op1_ptr[1 * 4 + 2] * op2_ptr[1]) + (op1_ptr[2 * 4 + 2] * op2_ptr[2]) + (op1_ptr[3 * 4 + 2] * op2_ptr[3]);
	float64_t dot_product3 = (op1_ptr[0 * 4 + 3] * op2_ptr[0]) + (op1_ptr[1 * 4 + 3] * op2_ptr[1]) + (op1_ptr[2 * 4 + 3] * op2_ptr[2]) + (op1_ptr[3 * 4 + 3] * op2_ptr[3]);
	res_ptr[0] = dot_product0;
	res_ptr[1] = dot_product1;
	res_ptr[2] = dot_product2;
	res_ptr[3] = dot_product3;
}

static zend_always_inline void qb_do_multiply_matrix_by_vector_padded_3x_F32(float32_t *op1_ptr, float32_t *op2_ptr, float32_t *res_ptr) {
	float32_t dot_product0 = (op1_ptr[0 * 4 + 0] * op2_ptr[0]) + (op1_ptr[1 * 4 + 0] * op2_ptr[1]) + (op1_ptr[2 * 4 + 0] * op2_ptr[2]);
	float32_t dot_product1 = (op1_ptr[0 * 4 + 1] * op2_ptr[0]) + (op1_ptr[1 * 4 + 1] * op2_ptr[1]) + (op1_ptr[2 * 4 + 1] * op2_ptr[2]);
	float32_t dot_product2 = (op1_ptr[0 * 4 + 2] * op2_ptr[0]) + (op1_ptr[1 * 4 + 2] * op2_ptr[1]) + (op1_ptr[2 * 4 + 2] * op2_ptr[2]);
	res_ptr[0] = dot_product0;
	res_ptr[1] = dot_product1;
	res_ptr[2] = dot_product2;
}

static zend_always_inline void qb_do_multiply_vector_by_matrix_2x_column_major_F32(float32_t *op1_ptr, float32_t *op2_ptr, float32_t *res_ptr) {
	float32_t dot_product0 = (op1_ptr[0] * op2_ptr[0 * 2 + 0]) + (op1_ptr[1] * op2_ptr[0 * 2 + 1]);
	float32_t dot_product1 = (op1_ptr[0] * op2_ptr[1 * 2 + 0]) + (op1_ptr[1] * op2_ptr[1 * 2 + 1]);
	res_ptr[0] = dot_product0;
	res_ptr[1] = dot_product1;
}

static zend_always_inline void qb_do_multiply_vector_by_matrix_2x_column_major_F64(float64_t *op1_ptr, float64_t *op2_ptr, float64_t *res_ptr) {
	float64_t dot_product0 = (op1_ptr[0] * op2_ptr[0 * 2 + 0]) + (op1_ptr[1] * op2_ptr[0 * 2 + 1]);
	float64_t dot_product1 = (op1_ptr[0] * op2_ptr[1 * 2 + 0]) + (op1_ptr[1] * op2_ptr[1 * 2 + 1]);
	res_ptr[0] = dot_product0;
	res_ptr[1] = dot_product1;
}

static zend_always_inline void qb_do_multiply_vector_by_matrix_3x_column_major_F32(float32_t *op1_ptr, float32_t *op2_ptr, float32_t *res_ptr) {
	float32_t dot_product0 = (op1_ptr[0] * op2_ptr[0 * 3 + 0]) + (op1_ptr[1] * op2_ptr[0 * 3 + 1]) + (op1_ptr[2] * op2_ptr[0 * 3 + 2]);
	float32_t dot_product1 = (op1_ptr[0] * op2_ptr[1 * 3 + 0]) + (op1_ptr[1] * op2_ptr[1 * 3 + 1]) + (op1_ptr[2] * op2_ptr[1 * 3 + 2]);
	float32_t dot_product2 = (op1_ptr[0] * op2_ptr[2 * 3 + 0]) + (op1_ptr[1] * op2_ptr[2 * 3 + 1]) + (op1_ptr[2] * op2_ptr[2 * 3 + 2]);
	res_ptr[0] = dot_product0;
	res_ptr[1] = dot_product1;
	res_ptr[2] = dot_product2;
}

static zend_always_inline void qb_do_multiply_vector_by_matrix_3x_column_major_F64(float64_t *op1_ptr, float64_t *op2_ptr, float64_t *res_ptr) {
	float64_t dot_product0 = (op1_ptr[0] * op2_ptr[0 * 3 + 0]) + (op1_ptr[1] * op2_ptr[0 * 3 + 1]) + (op1_ptr[2] * op2_ptr[0 * 3 + 2]);
	float64_t dot_product1 = (op1_ptr[0] * op2_ptr[1 * 3 + 0]) + (op1_ptr[1] * op2_ptr[1 * 3 + 1]) + (op1_ptr[2] * op2_ptr[1 * 3 + 2]);
	float64_t dot_product2 = (op1_ptr[0] * op2_ptr[2 * 3 + 0]) + (op1_ptr[1] * op2_ptr[2 * 3 + 1]) + (op1_ptr[2] * op2_ptr[2 * 3 + 2]);
	res_ptr[0] = dot_product0;
	res_ptr[1] = dot_product1;
	res_ptr[2] = dot_product2;
}

static zend_always_inline void qb_do_multiply_vector_by_matrix_4x_column_major_F32(float32_t *op1_ptr, float32_t *op2_ptr, float32_t *res_ptr) {
	float32_t dot_product0 = (op1_ptr[0] * op2_ptr[0 * 4 + 0]) + (op1_ptr[1] * op2_ptr[0 * 4 + 1]) + (op1_ptr[2] * op2_ptr[0 * 4 + 2]) + (op1_ptr[3] * op2_ptr[0 * 4 + 3]);
	float32_t dot_product1 = (op1_ptr[0] * op2_ptr[1 * 4 + 0]) + (op1_ptr[1] * op2_ptr[1 * 4 + 1]) + (op1_ptr[2] * op2_ptr[1 * 4 + 2]) + (op1_ptr[3] * op2_ptr[1 * 4 + 3]);
	float32_t dot_product2 = (op1_ptr[0] * op2_ptr[2 * 4 + 0]) + (op1_ptr[1] * op2_ptr[2 * 4 + 1]) + (op1_ptr[2] * op2_ptr[2 * 4 + 2]) + (op1_ptr[3] * op2_ptr[2 * 4 + 3]);
	float32_t dot_product3 = (op1_ptr[0] * op2_ptr[3 * 4 + 0]) + (op1_ptr[1] * op2_ptr[3 * 4 + 1]) + (op1_ptr[2] * op2_ptr[3 * 4 + 2]) + (op1_ptr[3] * op2_ptr[3 * 4 + 3]);
	res_ptr[0] = dot_product0;
	res_ptr[1] = dot_product1;
	res_ptr[2] = dot_product2;
	res_ptr[3] = dot_product3;
}

static zend_always_inline void qb_do_multiply_vector_by_matrix_4x_column_major_F64(float64_t *op1_ptr, float64_t *op2_ptr, float64_t *res_ptr) {
	float64_t dot_product0 = (op1_ptr[0] * op2_ptr[0 * 4 + 0]) + (op1_ptr[1] * op2_ptr[0 * 4 + 1]) + (op1_ptr[2] * op2_ptr[0 * 4 + 2]) + (op1_ptr[3] * op2_ptr[0 * 4 + 3]);
	float64_t dot_product1 = (op1_ptr[0] * op2_ptr[1 * 4 + 0]) + (op1_ptr[1] * op2_ptr[1 * 4 + 1]) + (op1_ptr[2] * op2_ptr[1 * 4 + 2]) + (op1_ptr[3] * op2_ptr[1 * 4 + 3]);
	float64_t dot_product2 = (op1_ptr[0] * op2_ptr[2 * 4 + 0]) + (op1_ptr[1] * op2_ptr[2 * 4 + 1]) + (op1_ptr[2] * op2_ptr[2 * 4 + 2]) + (op1_ptr[3] * op2_ptr[2 * 4 + 3]);
	float64_t dot_product3 = (op1_ptr[0] * op2_ptr[3 * 4 + 0]) + (op1_ptr[1] * op2_ptr[3 * 4 + 1]) + (op1_ptr[2] * op2_ptr[3 * 4 + 2]) + (op1_ptr[3] * op2_ptr[3 * 4 + 3]);
	res_ptr[0] = dot_product0;
	res_ptr[1] = dot_product1;
	res_ptr[2] = dot_product2;
	res_ptr[3] = dot_product3;
}

static zend_always_inline void qb_do_multiply_vector_by_matrix_padded_3x_F32(float32_t *op1_ptr, float32_t *op2_ptr, float32_t *res_ptr) {
	float32_t dot_product0 = (op1_ptr[0] * op2_ptr[0 * 4 + 0]) + (op1_ptr[1] * op2_ptr[0 * 4 + 1]) + (op1_ptr[2] * op2_ptr[0 * 4 + 2]);
	float32_t dot_product1 = (op1_ptr[0] * op2_ptr[1 * 4 + 0]) + (op1_ptr[1] * op2_ptr[1 * 4 + 1]) + (op1_ptr[2] * op2_ptr[1 * 4 + 2]);
	float32_t dot_product2 = (op1_ptr[0] * op2_ptr[2 * 4 + 0]) + (op1_ptr[1] * op2_ptr[2 * 4 + 1]) + (op1_ptr[2] * op2_ptr[2 * 4 + 2]);
	res_ptr[0] = dot_product0;
	res_ptr[1] = dot_product1;
	res_ptr[2] = dot_product2;
}

static zend_always_inline void qb_do_negate_2x_F32(float32_t *op1_ptr, float32_t *res_ptr) {
	res_ptr[0] = - op1_ptr[0];
	res_ptr[1] = - op1_ptr[1];
}

static zend_always_inline void qb_do_negate_2x_F64(float64_t *op1_ptr, float64_t *res_ptr) {
	res_ptr[0] = - op1_ptr[0];
	res_ptr[1] = - op1_ptr[1];
}

static zend_always_inline void qb_do_negate_3x_F32(float32_t *op1_ptr, float32_t *res_ptr) {
	res_ptr[0] = - op1_ptr[0];
	res_ptr[1] = - op1_ptr[1];
	res_ptr[2] = - op1_ptr[2];
}

static zend_always_inline void qb_do_negate_3x_F64(float64_t *op1_ptr, float64_t *res_ptr) {
	res_ptr[0] = - op1_ptr[0];
	res_ptr[1] = - op1_ptr[1];
	res_ptr[2] = - op1_ptr[2];
}

static zend_always_inline void qb_do_negate_4x_F32(float32_t *op1_ptr, float32_t *res_ptr) {
	res_ptr[0] = - op1_ptr[0];
	res_ptr[1] = - op1_ptr[1];
	res_ptr[2] = - op1_ptr[2];
	res_ptr[3] = - op1_ptr[3];
}

static zend_always_inline void qb_do_negate_4x_F64(float64_t *op1_ptr, float64_t *res_ptr) {
	res_ptr[0] = - op1_ptr[0];
	res_ptr[1] = - op1_ptr[1];
	res_ptr[2] = - op1_ptr[2];
	res_ptr[3] = - op1_ptr[3];
}

static zend_always_inline void qb_do_negate_F32(float32_t op1, float32_t *res_ptr) {
	(*res_ptr) = - op1;
}

static zend_always_inline void qb_do_negate_F64(float64_t op1, float64_t *res_ptr) {
	(*res_ptr) = - op1;
}

static zend_always_inline void qb_do_negate_I08(int8_t op1, int8_t *res_ptr) {
	(*res_ptr) = - op1;
}

static zend_always_inline void qb_do_negate_I16(int16_t op1, int16_t *res_ptr) {
	(*res_ptr) = - op1;
}

static zend_always_inline void qb_do_negate_I32(int32_t op1, int32_t *res_ptr) {
	(*res_ptr) = - op1;
}

static zend_always_inline void qb_do_negate_I64(int64_t op1, int64_t *res_ptr) {
	(*res_ptr) = - op1;
}

static zend_always_inline void qb_do_normalize_2x_F32(float32_t *op1_ptr, float32_t *res_ptr) {
	float32_t sum = (op1_ptr[0] * op1_ptr[0]) + (op1_ptr[1] * op1_ptr[1]);
	if(sum) {
		float32_t r = qb_fast_rsqrtf(sum);
		res_ptr[0] = op1_ptr[0] * r;
		res_ptr[1] = op1_ptr[1] * r;
	}
}

static zend_always_inline void qb_do_normalize_2x_F64(float64_t *op1_ptr, float64_t *res_ptr) {
	float64_t sum = (op1_ptr[0] * op1_ptr[0]) + (op1_ptr[1] * op1_ptr[1]);
	if(sum) {
		float64_t r = qb_fast_rsqrt(sum);
		res_ptr[0] = op1_ptr[0] * r;
		res_ptr[1] = op1_ptr[1] * r;
	}
}

static zend_always_inline void qb_do_normalize_3x_F32(float32_t *op1_ptr, float32_t *res_ptr) {
	float32_t sum = (op1_ptr[0] * op1_ptr[0]) + (op1_ptr[1] * op1_ptr[1]) + (op1_ptr[2] * op1_ptr[2]);
	if(sum) {
		float32_t r = qb_fast_rsqrtf(sum);
		res_ptr[0] = op1_ptr[0] * r;
		res_ptr[1] = op1_ptr[1] * r;
		res_ptr[2] = op1_ptr[2] * r;
	}
}

static zend_always_inline void qb_do_normalize_3x_F64(float64_t *op1_ptr, float64_t *res_ptr) {
	float64_t sum = (op1_ptr[0] * op1_ptr[0]) + (op1_ptr[1] * op1_ptr[1]) + (op1_ptr[2] * op1_ptr[2]);
	if(sum) {
		float64_t r = qb_fast_rsqrt(sum);
		res_ptr[0] = op1_ptr[0] * r;
		res_ptr[1] = op1_ptr[1] * r;
		res_ptr[2] = op1_ptr[2] * r;
	}
}

static zend_always_inline void qb_do_normalize_4x_F32(float32_t *op1_ptr, float32_t *res_ptr) {
	float32_t sum = (op1_ptr[0] * op1_ptr[0]) + (op1_ptr[1] * op1_ptr[1]) + (op1_ptr[2] * op1_ptr[2]) + (op1_ptr[3] * op1_ptr[3]);
	if(sum) {
		float32_t r = qb_fast_rsqrtf(sum);
		res_ptr[0] = op1_ptr[0] * r;
		res_ptr[1] = op1_ptr[1] * r;
		res_ptr[2] = op1_ptr[2] * r;
		res_ptr[3] = op1_ptr[3] * r;
	}
}

static zend_always_inline void qb_do_normalize_4x_F64(float64_t *op1_ptr, float64_t *res_ptr) {
	float64_t sum = (op1_ptr[0] * op1_ptr[0]) + (op1_ptr[1] * op1_ptr[1]) + (op1_ptr[2] * op1_ptr[2]) + (op1_ptr[3] * op1_ptr[3]);
	if(sum) {
		float64_t r = qb_fast_rsqrt(sum);
		res_ptr[0] = op1_ptr[0] * r;
		res_ptr[1] = op1_ptr[1] * r;
		res_ptr[2] = op1_ptr[2] * r;
		res_ptr[3] = op1_ptr[3] * r;
	}
}

static zend_always_inline void qb_do_not_equal_F32(float32_t op1, float32_t op2, int32_t *res_ptr) {
	(*res_ptr) = (op1 != op2);
}

static zend_always_inline void qb_do_not_equal_F64(float64_t op1, float64_t op2, int32_t *res_ptr) {
	(*res_ptr) = (op1 != op2);
}

static zend_always_inline void qb_do_not_equal_I08(int8_t op1, int8_t op2, int32_t *res_ptr) {
	(*res_ptr) = (op1 != op2);
}

static zend_always_inline void qb_do_not_equal_I16(int16_t op1, int16_t op2, int32_t *res_ptr) {
	(*res_ptr) = (op1 != op2);
}

static zend_always_inline void qb_do_not_equal_I32(int32_t op1, int32_t op2, int32_t *res_ptr) {
	(*res_ptr) = (op1 != op2);
}

static zend_always_inline void qb_do_not_equal_I64(int64_t op1, int64_t op2, int32_t *res_ptr) {
	(*res_ptr) = (op1 != op2);
}

static zend_always_inline void qb_do_pack_big_endian_F32(float32_t op1, uint8_t *res_ptr) {
	float32_t v = op1;
	*((uint32_t *) res_ptr) = SWAP_BE_I32(*((uint32_t *) &v));
}

static zend_always_inline void qb_do_pack_big_endian_F64(float64_t op1, uint8_t *res_ptr) {
	float64_t v = op1;
	*((uint64_t *) res_ptr) = SWAP_BE_I64(*((uint64_t *) &v));
}

static zend_always_inline void qb_do_pack_big_endian_I16(int16_t op1, uint8_t *res_ptr) {
	*((uint16_t *) res_ptr) = SWAP_BE_I16(op1);
}

static zend_always_inline void qb_do_pack_big_endian_I32(int32_t op1, uint8_t *res_ptr) {
	*((uint32_t *) res_ptr) = SWAP_BE_I32(op1);
}

static zend_always_inline void qb_do_pack_big_endian_I64(int64_t op1, uint8_t *res_ptr) {
	*((uint64_t *) res_ptr) = SWAP_BE_I64(op1);
}

static zend_always_inline void qb_do_pack_little_endian_F32(float32_t op1, uint8_t *res_ptr) {
	float32_t v = op1;
	*((uint32_t *) res_ptr) = SWAP_LE_I32(*((uint32_t *) &v));
}

static zend_always_inline void qb_do_pack_little_endian_F64(float64_t op1, uint8_t *res_ptr) {
	float64_t v = op1;
	*((uint64_t *) res_ptr) = SWAP_LE_I64(*((uint64_t *) &v));
}

static zend_always_inline void qb_do_pack_little_endian_I16(int16_t op1, uint8_t *res_ptr) {
	*((uint16_t *) res_ptr) = SWAP_LE_I16(op1);
}

static zend_always_inline void qb_do_pack_little_endian_I32(int32_t op1, uint8_t *res_ptr) {
	*((uint32_t *) res_ptr) = SWAP_LE_I32(op1);
}

static zend_always_inline void qb_do_pack_little_endian_I64(int64_t op1, uint8_t *res_ptr) {
	*((uint64_t *) res_ptr) = SWAP_LE_I64(op1);
}

static zend_always_inline void qb_do_pow_F32(float32_t op1, float32_t op2, float32_t *res_ptr) {
	(*res_ptr) = powf(op1, op2);
}

static zend_always_inline void qb_do_pow_F64(float64_t op1, float64_t op2, float64_t *res_ptr) {
	(*res_ptr) = pow(op1, op2);
}

static zend_always_inline void qb_do_radian_to_degree_F32(float32_t op1, float32_t *res_ptr) {
	(*res_ptr) = op1 * ((float32_t) (180 / M_PI));
}

static zend_always_inline void qb_do_radian_to_degree_F64(float64_t op1, float64_t *res_ptr) {
	(*res_ptr) = op1 * ((float64_t) (180 / M_PI));
}

static zend_always_inline void qb_do_reflect_2x_F32(float32_t *op1_ptr, float32_t *op2_ptr, float32_t *res_ptr) {
	float32_t dot_product = (op1_ptr[0] * op2_ptr[0]) + (op1_ptr[1] * op2_ptr[1]);
	res_ptr[0] = (float32_t) (op1_ptr[0] - 2.0 * dot_product * op2_ptr[0]);
	res_ptr[1] = (float32_t) (op1_ptr[1] - 2.0 * dot_product * op2_ptr[1]);
}

static zend_always_inline void qb_do_reflect_2x_F64(float64_t *op1_ptr, float64_t *op2_ptr, float64_t *res_ptr) {
	float64_t dot_product = (op1_ptr[0] * op2_ptr[0]) + (op1_ptr[1] * op2_ptr[1]);
	res_ptr[0] = (float64_t) (op1_ptr[0] - 2.0 * dot_product * op2_ptr[0]);
	res_ptr[1] = (float64_t) (op1_ptr[1] - 2.0 * dot_product * op2_ptr[1]);
}

static zend_always_inline void qb_do_reflect_3x_F32(float32_t *op1_ptr, float32_t *op2_ptr, float32_t *res_ptr) {
	float32_t dot_product = (op1_ptr[0] * op2_ptr[0]) + (op1_ptr[1] * op2_ptr[1]) + (op1_ptr[2] * op2_ptr[2]);
	res_ptr[0] = (float32_t) (op1_ptr[0] - 2.0 * dot_product * op2_ptr[0]);
	res_ptr[1] = (float32_t) (op1_ptr[1] - 2.0 * dot_product * op2_ptr[1]);
	res_ptr[2] = (float32_t) (op1_ptr[2] - 2.0 * dot_product * op2_ptr[2]);
}

static zend_always_inline void qb_do_reflect_3x_F64(float64_t *op1_ptr, float64_t *op2_ptr, float64_t *res_ptr) {
	float64_t dot_product = (op1_ptr[0] * op2_ptr[0]) + (op1_ptr[1] * op2_ptr[1]) + (op1_ptr[2] * op2_ptr[2]);
	res_ptr[0] = (float64_t) (op1_ptr[0] - 2.0 * dot_product * op2_ptr[0]);
	res_ptr[1] = (float64_t) (op1_ptr[1] - 2.0 * dot_product * op2_ptr[1]);
	res_ptr[2] = (float64_t) (op1_ptr[2] - 2.0 * dot_product * op2_ptr[2]);
}

static zend_always_inline void qb_do_reflect_4x_F32(float32_t *op1_ptr, float32_t *op2_ptr, float32_t *res_ptr) {
	float32_t dot_product = (op1_ptr[0] * op2_ptr[0]) + (op1_ptr[1] * op2_ptr[1]) + (op1_ptr[2] * op2_ptr[2]) + (op1_ptr[3] * op2_ptr[3]);
	res_ptr[0] = (float32_t) (op1_ptr[0] - 2.0 * dot_product * op2_ptr[0]);
	res_ptr[1] = (float32_t) (op1_ptr[1] - 2.0 * dot_product * op2_ptr[1]);
	res_ptr[2] = (float32_t) (op1_ptr[2] - 2.0 * dot_product * op2_ptr[2]);
	res_ptr[3] = (float32_t) (op1_ptr[3] - 2.0 * dot_product * op2_ptr[3]);
}

static zend_always_inline void qb_do_reflect_4x_F64(float64_t *op1_ptr, float64_t *op2_ptr, float64_t *res_ptr) {
	float64_t dot_product = (op1_ptr[0] * op2_ptr[0]) + (op1_ptr[1] * op2_ptr[1]) + (op1_ptr[2] * op2_ptr[2]) + (op1_ptr[3] * op2_ptr[3]);
	res_ptr[0] = (float64_t) (op1_ptr[0] - 2.0 * dot_product * op2_ptr[0]);
	res_ptr[1] = (float64_t) (op1_ptr[1] - 2.0 * dot_product * op2_ptr[1]);
	res_ptr[2] = (float64_t) (op1_ptr[2] - 2.0 * dot_product * op2_ptr[2]);
	res_ptr[3] = (float64_t) (op1_ptr[3] - 2.0 * dot_product * op2_ptr[3]);
}

static zend_always_inline void qb_do_rgb2hsl_3x_F32(float32_t *op1_ptr, float32_t *res_ptr) {
	qb_convert_rgb_to_hsl_F32(op1_ptr, res_ptr);
}

static zend_always_inline void qb_do_rgb2hsl_3x_F64(float64_t *op1_ptr, float64_t *res_ptr) {
	qb_convert_rgb_to_hsl_F64(op1_ptr, res_ptr);
}

static zend_always_inline void qb_do_rgb2hsl_4x_F32(float32_t *op1_ptr, float32_t *res_ptr) {
	qb_convert_rgb_to_hsl_F32(op1_ptr, res_ptr);
	res_ptr[3] = op1_ptr[3];
}

static zend_always_inline void qb_do_rgb2hsl_4x_F64(float64_t *op1_ptr, float64_t *res_ptr) {
	qb_convert_rgb_to_hsl_F64(op1_ptr, res_ptr);
	res_ptr[3] = op1_ptr[3];
}

static zend_always_inline void qb_do_rgb2hsv_3x_F32(float32_t *op1_ptr, float32_t *res_ptr) {
	qb_convert_rgb_to_hsv_F32(op1_ptr, res_ptr);
}

static zend_always_inline void qb_do_rgb2hsv_3x_F64(float64_t *op1_ptr, float64_t *res_ptr) {
	qb_convert_rgb_to_hsv_F64(op1_ptr, res_ptr);
}

static zend_always_inline void qb_do_rgb2hsv_4x_F32(float32_t *op1_ptr, float32_t *res_ptr) {
	qb_convert_rgb_to_hsv_F32(op1_ptr, res_ptr);
	res_ptr[3] = op1_ptr[3];
}

static zend_always_inline void qb_do_rgb2hsv_4x_F64(float64_t *op1_ptr, float64_t *res_ptr) {
	qb_convert_rgb_to_hsv_F64(op1_ptr, res_ptr);
	res_ptr[3] = op1_ptr[3];
}

static zend_always_inline void qb_do_rint_F32(float32_t op1, float32_t *res_ptr) {
	(*res_ptr) = rintf(op1);
}

static zend_always_inline void qb_do_rint_F64(float64_t op1, float64_t *res_ptr) {
	(*res_ptr) = rint(op1);
}

static zend_always_inline void qb_do_round_F32(float32_t op1, float32_t *res_ptr) {
	(*res_ptr) = roundf(op1);
}

static zend_always_inline void qb_do_round_F64(float64_t op1, float64_t *res_ptr) {
	(*res_ptr) = round(op1);
}

static zend_always_inline void qb_do_round_to_precision_F32(float32_t op1, int32_t op2, int32_t op3, float32_t *res_ptr) {
	(*res_ptr) = (float32_t) _php_math_round(op1, op2, op3);
}

static zend_always_inline void qb_do_round_to_precision_F64(float64_t op1, int32_t op2, int32_t op3, float64_t *res_ptr) {
	(*res_ptr) = (float64_t) _php_math_round(op1, op2, op3);
}

static zend_always_inline void qb_do_rsqrt_F32(float32_t op1, float32_t *res_ptr) {
	(*res_ptr) = qb_fast_rsqrtf(op1);
}

static zend_always_inline void qb_do_rsqrt_F64(float64_t op1, float64_t *res_ptr) {
	(*res_ptr) = qb_fast_rsqrt(op1);
}

static zend_always_inline void qb_do_sample_nearest_F32(float32_t *op1_ptr, uint32_t op2, uint32_t op3, float32_t op4, float32_t op5, float32_t *res_ptr) {
	int32_t ix = (int32_t) floorf(op4);
	int32_t iy = (int32_t) floorf(op5);
	if(((uint32_t) ix < op2) && ((uint32_t) iy < op3)) {
		uint32_t index = ((iy * op2) + ix) * 1;
		res_ptr[0] = op1_ptr[index + 0];
	} else {
		res_ptr[0] = 0;
	}
}

static zend_always_inline void qb_do_sample_nearest_F64(float64_t *op1_ptr, uint32_t op2, uint32_t op3, float64_t op4, float64_t op5, float64_t *res_ptr) {
	int32_t ix = (int32_t) floor(op4);
	int32_t iy = (int32_t) floor(op5);
	if(((uint32_t) ix < op2) && ((uint32_t) iy < op3)) {
		uint32_t index = ((iy * op2) + ix) * 1;
		res_ptr[0] = op1_ptr[index + 0];
	} else {
		res_ptr[0] = 0;
	}
}

static zend_always_inline void qb_do_scatter_2x_F32(uint32_t op1, float32_t *op2_ptr, float32_t *res_ptr) {
	float32_t v0, v1;
	v0 = op2_ptr[0];
	v1 = op2_ptr[1];
	res_ptr[(op1 >> 0) & 0x0007] = v0;
	res_ptr[(op1 >> 3) & 0x0007] = v1;
}

static zend_always_inline void qb_do_scatter_2x_F64(uint32_t op1, float64_t *op2_ptr, float64_t *res_ptr) {
	float64_t v0, v1;
	v0 = op2_ptr[0];
	v1 = op2_ptr[1];
	res_ptr[(op1 >> 0) & 0x0007] = v0;
	res_ptr[(op1 >> 3) & 0x0007] = v1;
}

static zend_always_inline void qb_do_scatter_2x_I08(uint32_t op1, int8_t *op2_ptr, int8_t *res_ptr) {
	int8_t v0, v1;
	v0 = op2_ptr[0];
	v1 = op2_ptr[1];
	res_ptr[(op1 >> 0) & 0x0007] = v0;
	res_ptr[(op1 >> 3) & 0x0007] = v1;
}

static zend_always_inline void qb_do_scatter_2x_I16(uint32_t op1, int16_t *op2_ptr, int16_t *res_ptr) {
	int16_t v0, v1;
	v0 = op2_ptr[0];
	v1 = op2_ptr[1];
	res_ptr[(op1 >> 0) & 0x0007] = v0;
	res_ptr[(op1 >> 3) & 0x0007] = v1;
}

static zend_always_inline void qb_do_scatter_2x_I32(uint32_t op1, int32_t *op2_ptr, int32_t *res_ptr) {
	int32_t v0, v1;
	v0 = op2_ptr[0];
	v1 = op2_ptr[1];
	res_ptr[(op1 >> 0) & 0x0007] = v0;
	res_ptr[(op1 >> 3) & 0x0007] = v1;
}

static zend_always_inline void qb_do_scatter_2x_I64(uint32_t op1, int64_t *op2_ptr, int64_t *res_ptr) {
	int64_t v0, v1;
	v0 = op2_ptr[0];
	v1 = op2_ptr[1];
	res_ptr[(op1 >> 0) & 0x0007] = v0;
	res_ptr[(op1 >> 3) & 0x0007] = v1;
}

static zend_always_inline void qb_do_scatter_3x_F32(uint32_t op1, float32_t *op2_ptr, float32_t *res_ptr) {
	float32_t v0, v1, v2;
	v0 = op2_ptr[0];
	v1 = op2_ptr[1];
	v2 = op2_ptr[2];
	res_ptr[(op1 >> 0) & 0x0007] = v0;
	res_ptr[(op1 >> 3) & 0x0007] = v1;
	res_ptr[(op1 >> 6) & 0x0007] = v2;
}

static zend_always_inline void qb_do_scatter_3x_F64(uint32_t op1, float64_t *op2_ptr, float64_t *res_ptr) {
	float64_t v0, v1, v2;
	v0 = op2_ptr[0];
	v1 = op2_ptr[1];
	v2 = op2_ptr[2];
	res_ptr[(op1 >> 0) & 0x0007] = v0;
	res_ptr[(op1 >> 3) & 0x0007] = v1;
	res_ptr[(op1 >> 6) & 0x0007] = v2;
}

static zend_always_inline void qb_do_scatter_3x_I08(uint32_t op1, int8_t *op2_ptr, int8_t *res_ptr) {
	int8_t v0, v1, v2;
	v0 = op2_ptr[0];
	v1 = op2_ptr[1];
	v2 = op2_ptr[2];
	res_ptr[(op1 >> 0) & 0x0007] = v0;
	res_ptr[(op1 >> 3) & 0x0007] = v1;
	res_ptr[(op1 >> 6) & 0x0007] = v2;
}

static zend_always_inline void qb_do_scatter_3x_I16(uint32_t op1, int16_t *op2_ptr, int16_t *res_ptr) {
	int16_t v0, v1, v2;
	v0 = op2_ptr[0];
	v1 = op2_ptr[1];
	v2 = op2_ptr[2];
	res_ptr[(op1 >> 0) & 0x0007] = v0;
	res_ptr[(op1 >> 3) & 0x0007] = v1;
	res_ptr[(op1 >> 6) & 0x0007] = v2;
}

static zend_always_inline void qb_do_scatter_3x_I32(uint32_t op1, int32_t *op2_ptr, int32_t *res_ptr) {
	int32_t v0, v1, v2;
	v0 = op2_ptr[0];
	v1 = op2_ptr[1];
	v2 = op2_ptr[2];
	res_ptr[(op1 >> 0) & 0x0007] = v0;
	res_ptr[(op1 >> 3) & 0x0007] = v1;
	res_ptr[(op1 >> 6) & 0x0007] = v2;
}

static zend_always_inline void qb_do_scatter_3x_I64(uint32_t op1, int64_t *op2_ptr, int64_t *res_ptr) {
	int64_t v0, v1, v2;
	v0 = op2_ptr[0];
	v1 = op2_ptr[1];
	v2 = op2_ptr[2];
	res_ptr[(op1 >> 0) & 0x0007] = v0;
	res_ptr[(op1 >> 3) & 0x0007] = v1;
	res_ptr[(op1 >> 6) & 0x0007] = v2;
}

static zend_always_inline void qb_do_shift_left_S08(int8_t op1, int8_t op2, int8_t *res_ptr) {
	(*res_ptr) = op1 << op2;
}

static zend_always_inline void qb_do_shift_left_S16(int16_t op1, int16_t op2, int16_t *res_ptr) {
	(*res_ptr) = op1 << op2;
}

static zend_always_inline void qb_do_shift_left_S32(int32_t op1, int32_t op2, int32_t *res_ptr) {
	(*res_ptr) = op1 << op2;
}

static zend_always_inline void qb_do_shift_left_S64(int64_t op1, int64_t op2, int64_t *res_ptr) {
	(*res_ptr) = op1 << op2;
}

static zend_always_inline void qb_do_shift_left_U08(uint8_t op1, uint8_t op2, uint8_t *res_ptr) {
	(*res_ptr) = op1 << op2;
}

static zend_always_inline void qb_do_shift_left_U16(uint16_t op1, uint16_t op2, uint16_t *res_ptr) {
	(*res_ptr) = op1 << op2;
}

static zend_always_inline void qb_do_shift_left_U32(uint32_t op1, uint32_t op2, uint32_t *res_ptr) {
	(*res_ptr) = op1 << op2;
}

static zend_always_inline void qb_do_shift_left_U64(uint64_t op1, uint64_t op2, uint64_t *res_ptr) {
	(*res_ptr) = op1 << op2;
}

static zend_always_inline void qb_do_shift_right_S08(int8_t op1, int8_t op2, int8_t *res_ptr) {
	(*res_ptr) = op1 >> op2;
}

static zend_always_inline void qb_do_shift_right_S16(int16_t op1, int16_t op2, int16_t *res_ptr) {
	(*res_ptr) = op1 >> op2;
}

static zend_always_inline void qb_do_shift_right_S32(int32_t op1, int32_t op2, int32_t *res_ptr) {
	(*res_ptr) = op1 >> op2;
}

static zend_always_inline void qb_do_shift_right_S64(int64_t op1, int64_t op2, int64_t *res_ptr) {
	(*res_ptr) = op1 >> op2;
}

static zend_always_inline void qb_do_shift_right_U08(uint8_t op1, uint8_t op2, uint8_t *res_ptr) {
	(*res_ptr) = op1 >> op2;
}

static zend_always_inline void qb_do_shift_right_U16(uint16_t op1, uint16_t op2, uint16_t *res_ptr) {
	(*res_ptr) = op1 >> op2;
}

static zend_always_inline void qb_do_shift_right_U32(uint32_t op1, uint32_t op2, uint32_t *res_ptr) {
	(*res_ptr) = op1 >> op2;
}

static zend_always_inline void qb_do_shift_right_U64(uint64_t op1, uint64_t op2, uint64_t *res_ptr) {
	(*res_ptr) = op1 >> op2;
}

static zend_always_inline void qb_do_sign_F32(float32_t op1, float32_t *res_ptr) {
	if(op1 < 0) {
		(*res_ptr) = -1;
	} else if(op1 > 0) {
		(*res_ptr) = 1;
	} else {
		(*res_ptr) = 0;
	}
}

static zend_always_inline void qb_do_sign_F64(float64_t op1, float64_t *res_ptr) {
	if(op1 < 0) {
		(*res_ptr) = -1;
	} else if(op1 > 0) {
		(*res_ptr) = 1;
	} else {
		(*res_ptr) = 0;
	}
}

static zend_always_inline void qb_do_sin_F32(float32_t op1, float32_t *res_ptr) {
	(*res_ptr) = (float32_t) sinf(op1);
}

static zend_always_inline void qb_do_sin_F64(float64_t op1, float64_t *res_ptr) {
	(*res_ptr) = (float64_t) sin(op1);
}

static zend_always_inline void qb_do_sinh_F32(float32_t op1, float32_t *res_ptr) {
	(*res_ptr) = sinhf(op1);
}

static zend_always_inline void qb_do_sinh_F64(float64_t op1, float64_t *res_ptr) {
	(*res_ptr) = sinh(op1);
}

static zend_always_inline void qb_do_smooth_step_F32(float32_t op1, float32_t op2, float32_t op3, float32_t *res_ptr) {
	if(op3 <= op1) {
		(*res_ptr) = 0;
	} else if(op3 >= op2) {
		(*res_ptr) = 1;
	} else {
		float32_t value = (op3 - op1) / (op2 - op1);
		(*res_ptr) = value * value * (3 - 2 * value);
	}
}

static zend_always_inline void qb_do_smooth_step_F64(float64_t op1, float64_t op2, float64_t op3, float64_t *res_ptr) {
	if(op3 <= op1) {
		(*res_ptr) = 0;
	} else if(op3 >= op2) {
		(*res_ptr) = 1;
	} else {
		float64_t value = (op3 - op1) / (op2 - op1);
		(*res_ptr) = value * value * (3 - 2 * value);
	}
}

static zend_always_inline void qb_do_sqrt_F32(float32_t op1, float32_t *res_ptr) {
	(*res_ptr) = sqrtf(op1);
}

static zend_always_inline void qb_do_sqrt_F64(float64_t op1, float64_t *res_ptr) {
	(*res_ptr) = sqrt(op1);
}

static zend_always_inline void qb_do_step_F32(float32_t op1, float32_t op2, float32_t *res_ptr) {
	(*res_ptr) = ((op2 < op1) ? 0.0f : 1.0f);
}

static zend_always_inline void qb_do_step_F64(float64_t op1, float64_t op2, float64_t *res_ptr) {
	(*res_ptr) = ((op2 < op1) ? 0.0 : 1.0);
}

static zend_always_inline void qb_do_subtract_2x_F32(float32_t *op1_ptr, float32_t *op2_ptr, float32_t *res_ptr) {
	res_ptr[0] = op1_ptr[0] - op2_ptr[0];
	res_ptr[1] = op1_ptr[1] - op2_ptr[1];
}

static zend_always_inline void qb_do_subtract_2x_F64(float64_t *op1_ptr, float64_t *op2_ptr, float64_t *res_ptr) {
	res_ptr[0] = op1_ptr[0] - op2_ptr[0];
	res_ptr[1] = op1_ptr[1] - op2_ptr[1];
}

static zend_always_inline void qb_do_subtract_3x_F32(float32_t *op1_ptr, float32_t *op2_ptr, float32_t *res_ptr) {
	res_ptr[0] = op1_ptr[0] - op2_ptr[0];
	res_ptr[1] = op1_ptr[1] - op2_ptr[1];
	res_ptr[2] = op1_ptr[2] - op2_ptr[2];
}

static zend_always_inline void qb_do_subtract_3x_F64(float64_t *op1_ptr, float64_t *op2_ptr, float64_t *res_ptr) {
	res_ptr[0] = op1_ptr[0] - op2_ptr[0];
	res_ptr[1] = op1_ptr[1] - op2_ptr[1];
	res_ptr[2] = op1_ptr[2] - op2_ptr[2];
}

static zend_always_inline void qb_do_subtract_4x_F32(float32_t *op1_ptr, float32_t *op2_ptr, float32_t *res_ptr) {
	res_ptr[0] = op1_ptr[0] - op2_ptr[0];
	res_ptr[1] = op1_ptr[1] - op2_ptr[1];
	res_ptr[2] = op1_ptr[2] - op2_ptr[2];
	res_ptr[3] = op1_ptr[3] - op2_ptr[3];
}

static zend_always_inline void qb_do_subtract_4x_F64(float64_t *op1_ptr, float64_t *op2_ptr, float64_t *res_ptr) {
	res_ptr[0] = op1_ptr[0] - op2_ptr[0];
	res_ptr[1] = op1_ptr[1] - op2_ptr[1];
	res_ptr[2] = op1_ptr[2] - op2_ptr[2];
	res_ptr[3] = op1_ptr[3] - op2_ptr[3];
}

static zend_always_inline void qb_do_subtract_F32(float32_t op1, float32_t op2, float32_t *res_ptr) {
	(*res_ptr) = op1 - op2;
}

static zend_always_inline void qb_do_subtract_F64(float64_t op1, float64_t op2, float64_t *res_ptr) {
	(*res_ptr) = op1 - op2;
}

static zend_always_inline void qb_do_subtract_I08(int8_t op1, int8_t op2, int8_t *res_ptr) {
	(*res_ptr) = op1 - op2;
}

static zend_always_inline void qb_do_subtract_I16(int16_t op1, int16_t op2, int16_t *res_ptr) {
	(*res_ptr) = op1 - op2;
}

static zend_always_inline void qb_do_subtract_I32(int32_t op1, int32_t op2, int32_t *res_ptr) {
	(*res_ptr) = op1 - op2;
}

static zend_always_inline void qb_do_subtract_I64(int64_t op1, int64_t op2, int64_t *res_ptr) {
	(*res_ptr) = op1 - op2;
}

static zend_always_inline void qb_do_tan_F32(float32_t op1, float32_t *res_ptr) {
	(*res_ptr) = tanf(op1);
}

static zend_always_inline void qb_do_tan_F64(float64_t op1, float64_t *res_ptr) {
	(*res_ptr) = tan(op1);
}

static zend_always_inline void qb_do_tanh_F32(float32_t op1, float32_t *res_ptr) {
	(*res_ptr) = tanhf(op1);
}

static zend_always_inline void qb_do_tanh_F64(float64_t op1, float64_t *res_ptr) {
	(*res_ptr) = tanh(op1);
}

static zend_always_inline void qb_do_transform_vector_2x_column_major_F32(float32_t *op1_ptr, float32_t *op2_ptr, float32_t *res_ptr) {
	float32_t dot_product0 = (op1_ptr[0 * 2 + 0] * op2_ptr[0]) + (op1_ptr[1 * 2 + 0] * op2_ptr[1]) + op1_ptr[2 * 2 + 0];
	float32_t dot_product1 = (op1_ptr[0 * 2 + 1] * op2_ptr[0]) + (op1_ptr[1 * 2 + 1] * op2_ptr[1]) + op1_ptr[2 * 2 + 1];
	res_ptr[0] = dot_product0;
	res_ptr[1] = dot_product1;
}

static zend_always_inline void qb_do_transform_vector_2x_column_major_F64(float64_t *op1_ptr, float64_t *op2_ptr, float64_t *res_ptr) {
	float64_t dot_product0 = (op1_ptr[0 * 2 + 0] * op2_ptr[0]) + (op1_ptr[1 * 2 + 0] * op2_ptr[1]) + op1_ptr[2 * 2 + 0];
	float64_t dot_product1 = (op1_ptr[0 * 2 + 1] * op2_ptr[0]) + (op1_ptr[1 * 2 + 1] * op2_ptr[1]) + op1_ptr[2 * 2 + 1];
	res_ptr[0] = dot_product0;
	res_ptr[1] = dot_product1;
}

static zend_always_inline void qb_do_transform_vector_2x_row_major_F32(float32_t *op1_ptr, float32_t *op2_ptr, float32_t *res_ptr) {
	float32_t dot_product0 = (op1_ptr[0 * 3 + 0] * op2_ptr[0]) + (op1_ptr[0 * 3 + 1] * op2_ptr[1]) + op1_ptr[0 * 3 + 2];
	float32_t dot_product1 = (op1_ptr[1 * 3 + 0] * op2_ptr[0]) + (op1_ptr[1 * 3 + 1] * op2_ptr[1]) + op1_ptr[1 * 3 + 2];
	res_ptr[0] = dot_product0;
	res_ptr[1] = dot_product1;
}

static zend_always_inline void qb_do_transform_vector_2x_row_major_F64(float64_t *op1_ptr, float64_t *op2_ptr, float64_t *res_ptr) {
	float64_t dot_product0 = (op1_ptr[0 * 3 + 0] * op2_ptr[0]) + (op1_ptr[0 * 3 + 1] * op2_ptr[1]) + op1_ptr[0 * 3 + 2];
	float64_t dot_product1 = (op1_ptr[1 * 3 + 0] * op2_ptr[0]) + (op1_ptr[1 * 3 + 1] * op2_ptr[1]) + op1_ptr[1 * 3 + 2];
	res_ptr[0] = dot_product0;
	res_ptr[1] = dot_product1;
}

static zend_always_inline void qb_do_transform_vector_3x_column_major_F32(float32_t *op1_ptr, float32_t *op2_ptr, float32_t *res_ptr) {
	float32_t dot_product0 = (op1_ptr[0 * 3 + 0] * op2_ptr[0]) + (op1_ptr[1 * 3 + 0] * op2_ptr[1]) + (op1_ptr[2 * 3 + 0] * op2_ptr[2]) + op1_ptr[3 * 3 + 0];
	float32_t dot_product1 = (op1_ptr[0 * 3 + 1] * op2_ptr[0]) + (op1_ptr[1 * 3 + 1] * op2_ptr[1]) + (op1_ptr[2 * 3 + 1] * op2_ptr[2]) + op1_ptr[3 * 3 + 1];
	float32_t dot_product2 = (op1_ptr[0 * 3 + 2] * op2_ptr[0]) + (op1_ptr[1 * 3 + 2] * op2_ptr[1]) + (op1_ptr[2 * 3 + 2] * op2_ptr[2]) + op1_ptr[3 * 3 + 2];
	res_ptr[0] = dot_product0;
	res_ptr[1] = dot_product1;
	res_ptr[2] = dot_product2;
}

static zend_always_inline void qb_do_transform_vector_3x_column_major_F64(float64_t *op1_ptr, float64_t *op2_ptr, float64_t *res_ptr) {
	float64_t dot_product0 = (op1_ptr[0 * 3 + 0] * op2_ptr[0]) + (op1_ptr[1 * 3 + 0] * op2_ptr[1]) + (op1_ptr[2 * 3 + 0] * op2_ptr[2]) + op1_ptr[3 * 3 + 0];
	float64_t dot_product1 = (op1_ptr[0 * 3 + 1] * op2_ptr[0]) + (op1_ptr[1 * 3 + 1] * op2_ptr[1]) + (op1_ptr[2 * 3 + 1] * op2_ptr[2]) + op1_ptr[3 * 3 + 1];
	float64_t dot_product2 = (op1_ptr[0 * 3 + 2] * op2_ptr[0]) + (op1_ptr[1 * 3 + 2] * op2_ptr[1]) + (op1_ptr[2 * 3 + 2] * op2_ptr[2]) + op1_ptr[3 * 3 + 2];
	res_ptr[0] = dot_product0;
	res_ptr[1] = dot_product1;
	res_ptr[2] = dot_product2;
}

static zend_always_inline void qb_do_transform_vector_3x_row_major_F32(float32_t *op1_ptr, float32_t *op2_ptr, float32_t *res_ptr) {
	float32_t dot_product0 = (op1_ptr[0 * 4 + 0] * op2_ptr[0]) + (op1_ptr[0 * 4 + 1] * op2_ptr[1]) + (op1_ptr[0 * 4 + 2] * op2_ptr[2]) + op1_ptr[0 * 4 + 3];
	float32_t dot_product1 = (op1_ptr[1 * 4 + 0] * op2_ptr[0]) + (op1_ptr[1 * 4 + 1] * op2_ptr[1]) + (op1_ptr[1 * 4 + 2] * op2_ptr[2]) + op1_ptr[1 * 4 + 3];
	float32_t dot_product2 = (op1_ptr[2 * 4 + 0] * op2_ptr[0]) + (op1_ptr[2 * 4 + 1] * op2_ptr[1]) + (op1_ptr[2 * 4 + 2] * op2_ptr[2]) + op1_ptr[2 * 4 + 3];
	res_ptr[0] = dot_product0;
	res_ptr[1] = dot_product1;
	res_ptr[2] = dot_product2;
}

static zend_always_inline void qb_do_transform_vector_3x_row_major_F64(float64_t *op1_ptr, float64_t *op2_ptr, float64_t *res_ptr) {
	float64_t dot_product0 = (op1_ptr[0 * 4 + 0] * op2_ptr[0]) + (op1_ptr[0 * 4 + 1] * op2_ptr[1]) + (op1_ptr[0 * 4 + 2] * op2_ptr[2]) + op1_ptr[0 * 4 + 3];
	float64_t dot_product1 = (op1_ptr[1 * 4 + 0] * op2_ptr[0]) + (op1_ptr[1 * 4 + 1] * op2_ptr[1]) + (op1_ptr[1 * 4 + 2] * op2_ptr[2]) + op1_ptr[1 * 4 + 3];
	float64_t dot_product2 = (op1_ptr[2 * 4 + 0] * op2_ptr[0]) + (op1_ptr[2 * 4 + 1] * op2_ptr[1]) + (op1_ptr[2 * 4 + 2] * op2_ptr[2]) + op1_ptr[2 * 4 + 3];
	res_ptr[0] = dot_product0;
	res_ptr[1] = dot_product1;
	res_ptr[2] = dot_product2;
}

static zend_always_inline void qb_do_transform_vector_4x_column_major_F32(float32_t *op1_ptr, float32_t *op2_ptr, float32_t *res_ptr) {
	float32_t dot_product0 = (op1_ptr[0 * 4 + 0] * op2_ptr[0]) + (op1_ptr[1 * 4 + 0] * op2_ptr[1]) + (op1_ptr[2 * 4 + 0] * op2_ptr[2]) + (op1_ptr[3 * 4 + 0] * op2_ptr[3]) + op1_ptr[4 * 4 + 0];
	float32_t dot_product1 = (op1_ptr[0 * 4 + 1] * op2_ptr[0]) + (op1_ptr[1 * 4 + 1] * op2_ptr[1]) + (op1_ptr[2 * 4 + 1] * op2_ptr[2]) + (op1_ptr[3 * 4 + 1] * op2_ptr[3]) + op1_ptr[4 * 4 + 1];
	float32_t dot_product2 = (op1_ptr[0 * 4 + 2] * op2_ptr[0]) + (op1_ptr[1 * 4 + 2] * op2_ptr[1]) + (op1_ptr[2 * 4 + 2] * op2_ptr[2]) + (op1_ptr[3 * 4 + 2] * op2_ptr[3]) + op1_ptr[4 * 4 + 2];
	float32_t dot_product3 = (op1_ptr[0 * 4 + 3] * op2_ptr[0]) + (op1_ptr[1 * 4 + 3] * op2_ptr[1]) + (op1_ptr[2 * 4 + 3] * op2_ptr[2]) + (op1_ptr[3 * 4 + 3] * op2_ptr[3]) + op1_ptr[4 * 4 + 3];
	res_ptr[0] = dot_product0;
	res_ptr[1] = dot_product1;
	res_ptr[2] = dot_product2;
	res_ptr[3] = dot_product3;
}

static zend_always_inline void qb_do_transform_vector_4x_column_major_F64(float64_t *op1_ptr, float64_t *op2_ptr, float64_t *res_ptr) {
	float64_t dot_product0 = (op1_ptr[0 * 4 + 0] * op2_ptr[0]) + (op1_ptr[1 * 4 + 0] * op2_ptr[1]) + (op1_ptr[2 * 4 + 0] * op2_ptr[2]) + (op1_ptr[3 * 4 + 0] * op2_ptr[3]) + op1_ptr[4 * 4 + 0];
	float64_t dot_product1 = (op1_ptr[0 * 4 + 1] * op2_ptr[0]) + (op1_ptr[1 * 4 + 1] * op2_ptr[1]) + (op1_ptr[2 * 4 + 1] * op2_ptr[2]) + (op1_ptr[3 * 4 + 1] * op2_ptr[3]) + op1_ptr[4 * 4 + 1];
	float64_t dot_product2 = (op1_ptr[0 * 4 + 2] * op2_ptr[0]) + (op1_ptr[1 * 4 + 2] * op2_ptr[1]) + (op1_ptr[2 * 4 + 2] * op2_ptr[2]) + (op1_ptr[3 * 4 + 2] * op2_ptr[3]) + op1_ptr[4 * 4 + 2];
	float64_t dot_product3 = (op1_ptr[0 * 4 + 3] * op2_ptr[0]) + (op1_ptr[1 * 4 + 3] * op2_ptr[1]) + (op1_ptr[2 * 4 + 3] * op2_ptr[2]) + (op1_ptr[3 * 4 + 3] * op2_ptr[3]) + op1_ptr[4 * 4 + 3];
	res_ptr[0] = dot_product0;
	res_ptr[1] = dot_product1;
	res_ptr[2] = dot_product2;
	res_ptr[3] = dot_product3;
}

static zend_always_inline void qb_do_transform_vector_4x_row_major_F32(float32_t *op1_ptr, float32_t *op2_ptr, float32_t *res_ptr) {
	float32_t dot_product0 = (op1_ptr[0 * 5 + 0] * op2_ptr[0]) + (op1_ptr[0 * 5 + 1] * op2_ptr[1]) + (op1_ptr[0 * 5 + 2] * op2_ptr[2]) + (op1_ptr[0 * 5 + 3] * op2_ptr[3]) + op1_ptr[0 * 5 + 4];
	float32_t dot_product1 = (op1_ptr[1 * 5 + 0] * op2_ptr[0]) + (op1_ptr[1 * 5 + 1] * op2_ptr[1]) + (op1_ptr[1 * 5 + 2] * op2_ptr[2]) + (op1_ptr[1 * 5 + 3] * op2_ptr[3]) + op1_ptr[1 * 5 + 4];
	float32_t dot_product2 = (op1_ptr[2 * 5 + 0] * op2_ptr[0]) + (op1_ptr[2 * 5 + 1] * op2_ptr[1]) + (op1_ptr[2 * 5 + 2] * op2_ptr[2]) + (op1_ptr[2 * 5 + 3] * op2_ptr[3]) + op1_ptr[2 * 5 + 4];
	float32_t dot_product3 = (op1_ptr[3 * 5 + 0] * op2_ptr[0]) + (op1_ptr[3 * 5 + 1] * op2_ptr[1]) + (op1_ptr[3 * 5 + 2] * op2_ptr[2]) + (op1_ptr[3 * 5 + 3] * op2_ptr[3]) + op1_ptr[3 * 5 + 4];
	res_ptr[0] = dot_product0;
	res_ptr[1] = dot_product1;
	res_ptr[2] = dot_product2;
	res_ptr[3] = dot_product3;
}

static zend_always_inline void qb_do_transform_vector_4x_row_major_F64(float64_t *op1_ptr, float64_t *op2_ptr, float64_t *res_ptr) {
	float64_t dot_product0 = (op1_ptr[0 * 5 + 0] * op2_ptr[0]) + (op1_ptr[0 * 5 + 1] * op2_ptr[1]) + (op1_ptr[0 * 5 + 2] * op2_ptr[2]) + (op1_ptr[0 * 5 + 3] * op2_ptr[3]) + op1_ptr[0 * 5 + 4];
	float64_t dot_product1 = (op1_ptr[1 * 5 + 0] * op2_ptr[0]) + (op1_ptr[1 * 5 + 1] * op2_ptr[1]) + (op1_ptr[1 * 5 + 2] * op2_ptr[2]) + (op1_ptr[1 * 5 + 3] * op2_ptr[3]) + op1_ptr[1 * 5 + 4];
	float64_t dot_product2 = (op1_ptr[2 * 5 + 0] * op2_ptr[0]) + (op1_ptr[2 * 5 + 1] * op2_ptr[1]) + (op1_ptr[2 * 5 + 2] * op2_ptr[2]) + (op1_ptr[2 * 5 + 3] * op2_ptr[3]) + op1_ptr[2 * 5 + 4];
	float64_t dot_product3 = (op1_ptr[3 * 5 + 0] * op2_ptr[0]) + (op1_ptr[3 * 5 + 1] * op2_ptr[1]) + (op1_ptr[3 * 5 + 2] * op2_ptr[2]) + (op1_ptr[3 * 5 + 3] * op2_ptr[3]) + op1_ptr[3 * 5 + 4];
	res_ptr[0] = dot_product0;
	res_ptr[1] = dot_product1;
	res_ptr[2] = dot_product2;
	res_ptr[3] = dot_product3;
}

static zend_always_inline void qb_do_transpose_matrix_2x_F32(float32_t *op1_ptr, float32_t *res_ptr) {
	float32_t m00 = op1_ptr[0 * 2 + 0];
	float32_t m10 = op1_ptr[0 * 2 + 1];
	float32_t m01 = op1_ptr[1 * 2 + 0];
	float32_t m11 = op1_ptr[1 * 2 + 1];
	res_ptr[0 * 2 + 0] = m00;
	res_ptr[0 * 2 + 1] = m01;
	res_ptr[1 * 2 + 0] = m10;
	res_ptr[1 * 2 + 1] = m11;
}

static zend_always_inline void qb_do_transpose_matrix_2x_F64(float64_t *op1_ptr, float64_t *res_ptr) {
	float64_t m00 = op1_ptr[0 * 2 + 0];
	float64_t m10 = op1_ptr[0 * 2 + 1];
	float64_t m01 = op1_ptr[1 * 2 + 0];
	float64_t m11 = op1_ptr[1 * 2 + 1];
	res_ptr[0 * 2 + 0] = m00;
	res_ptr[0 * 2 + 1] = m01;
	res_ptr[1 * 2 + 0] = m10;
	res_ptr[1 * 2 + 1] = m11;
}

static zend_always_inline void qb_do_unpack_big_endian_F32(uint8_t *op1_ptr, float32_t *res_ptr) {
	*((uint32_t *) &(*res_ptr)) = SWAP_BE_I32(*((uint32_t *) op1_ptr));
}

static zend_always_inline void qb_do_unpack_big_endian_F64(uint8_t *op1_ptr, float64_t *res_ptr) {
	*((uint64_t *) &(*res_ptr)) = SWAP_BE_I64(*((uint64_t *) op1_ptr));
}

static zend_always_inline void qb_do_unpack_big_endian_I16(uint8_t *op1_ptr, int16_t *res_ptr) {
	*((uint16_t *) &(*res_ptr)) = SWAP_BE_I16(*((uint16_t *) op1_ptr));
}

static zend_always_inline void qb_do_unpack_big_endian_I32(uint8_t *op1_ptr, int32_t *res_ptr) {
	*((uint32_t *) &(*res_ptr)) = SWAP_BE_I32(*((uint32_t *) op1_ptr));
}

static zend_always_inline void qb_do_unpack_big_endian_I64(uint8_t *op1_ptr, int64_t *res_ptr) {
	*((uint64_t *) &(*res_ptr)) = SWAP_BE_I64(*((uint64_t *) op1_ptr));
}

static zend_always_inline void qb_do_unpack_little_endian_F32(uint8_t *op1_ptr, float32_t *res_ptr) {
	*((uint32_t *) &(*res_ptr)) = SWAP_LE_I32(*((uint32_t *) op1_ptr));
}

static zend_always_inline void qb_do_unpack_little_endian_F64(uint8_t *op1_ptr, float64_t *res_ptr) {
	*((uint64_t *) &(*res_ptr)) = SWAP_LE_I64(*((uint64_t *) op1_ptr));
}

static zend_always_inline void qb_do_unpack_little_endian_I16(uint8_t *op1_ptr, int16_t *res_ptr) {
	*((uint16_t *) &(*res_ptr)) = SWAP_LE_I16(*((uint16_t *) op1_ptr));
}

static zend_always_inline void qb_do_unpack_little_endian_I32(uint8_t *op1_ptr, int32_t *res_ptr) {
	*((uint32_t *) &(*res_ptr)) = SWAP_LE_I32(*((uint32_t *) op1_ptr));
}

static zend_always_inline void qb_do_unpack_little_endian_I64(uint8_t *op1_ptr, int64_t *res_ptr) {
	*((uint64_t *) &(*res_ptr)) = SWAP_LE_I64(*((uint64_t *) op1_ptr));
}

