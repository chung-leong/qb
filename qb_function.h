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

#ifndef QB_FUNCTION_H_
#define QB_FUNCTION_H_

typedef struct qb_variable					qb_variable;
typedef struct qb_function					qb_function;
typedef struct qb_native_code_bundle		qb_native_code_bundle;

typedef enum qb_external_symbol_type		qb_external_symbol_type;

enum {
	QB_TYPE_DECL_STRING				= 0x00010000,
	QB_TYPE_DECL_EXPANDABLE			= 0x00020000,
	QB_TYPE_DECL_HAS_ALIAS_SCHEMES	= 0x00040000,
};

enum {
	QB_VARIABLE_LOCAL				= 0x00000001,
	QB_VARIABLE_ARGUMENT			= 0x00000002,
	QB_VARIABLE_STATIC				= 0x00000004,
	QB_VARIABLE_GLOBAL				= 0x00000008,
	QB_VARIABLE_CLASS				= 0x00000010,
	QB_VARIABLE_CLASS_INSTANCE		= 0x00000020,
	QB_VARIABLE_CLASS_CONSTANT		= 0x00000100,
	QB_VARIABLE_RETURN_VALUE		= 0x00000040,
	QB_VARIABLE_SHARED				= 0x00000080,
	QB_VARIABLE_TYPES				= 0x0000FFFF,

	QB_VARIABLE_BY_REF				= 0x00010000,
	QB_VARIABLE_IMPORTED			= 0x00020000,
};

struct qb_variable {
	uint32_t flags;
	qb_address *address;
	const char *name;
	uint32_t name_length;
	ulong hash_value;
	zend_class_entry *zend_class;
	union {
		zval *default_value;
		zval **value_pointer;
	};
};

enum qb_external_symbol_type {
	QB_EXT_SYM_ZEND_FUNCTION			= 1,
	QB_EXT_SYM_ZEND_CLASS				= 2,
	QB_EXT_SYM_PCRE						= 3,
};

struct qb_external_symbol {
	uint32_t type;
	const char *name;
	uint32_t name_length;
	void *pointer;
};

enum {
	QB_ENGINE_COMPILE_IF_POSSIBLE	= 0x00000001,
	QB_ENGINE_NEVER_COMPILE			= 0x00000002,
	QB_ENGINE_GO_THRU_ZEND			= 0x00000004,
};

struct qb_function {
	int8_t *instructions;
	int8_t *instruction_start;
	int8_t *instruction_non_static_start;
	uint64_t instruction_crc64;
	uint16_t *instruction_opcodes;
	uint32_t instruction_opcode_count;
	uint32_t size;
	uint32_t flags;
	qb_variable *return_variable;
	qb_variable **variables;
	uint32_t variable_count;
	uint32_t argument_count;
	qb_external_symbol **external_symbols;
	uint32_t external_symbol_count;
	qb_storage *local_storage;
	const char *name;
	const char *filename;
	void *native_proc;

	zend_op_array *zend_op_array;
	qb_function *next_copy;
};

struct qb_native_code_bundle {
	void *memory;
	uint32_t size;
};

#endif QB_FUNCTION_H_