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

#ifndef QB_DEBUG_INTERFACE_H_
#define QB_DEBUG_INTERFACE_H_

#define QB_INTERFACE_SIGNATURE				0xF0013761
#define QB_MAXIMUM_DIMENSION_COUNT			8
#define QB_USER_OPCODE						254

#define QB_VAR_IS_INTEGER					0x00000001
#define QB_VAR_IS_UNSIGNED					0x00000002
#define QB_VAR_IS_FLOAT						0x00000004

#define QB_VAR_IS_BOOL						0x00000010
#define QB_VAR_IS_STRING					0x00000020
#define QB_VAR_IS_IMAGE						0x00000040

#define QB_VAR_IS_READ_ONLY					0x00010000
#define QB_VAR_IS_VARIABLE_LENGTH			0x00020000

#define QB_IS_COMPILED(op_array)			((op_array)->opcodes[0].opcode == QB_USER_OPCODE)

#if PHP_MAJOR_VERSION == 5 && (PHP_MINOR_VERSION == 1 || PHP_MINOR_VERSION == 2 || PHP_MINOR_VERSION == 3)
	#define QB_GET_INTERFACE(op_array)		((void *) (op_array)->opcodes[0].op1.u.jmp_addr)
	#define QB_SET_INTERFACE(op_array, p)	(op_array)->opcodes[0].op1.u.jmp_addr = (void *) p
	#define QB_GET_FUNCTION(op_array)		((void *) (op_array)->opcodes[0].op2.u.jmp_addr)
	#define QB_SET_FUNCTION(op_array, p)	(op_array)->opcodes[0].op2.u.jmp_addr = (void *) p
#elif PHP_MAJOR_VERSION == 5 && (PHP_MINOR_VERSION == 4 || PHP_MINOR_VERSION == 5)
	#define QB_GET_INTERFACE(op_array)		((op_array)->opcodes[0].op1.ptr)
	#define QB_SET_INTERFACE(op_array, p)	(op_array)->opcodes[0].op1.ptr = (void *) p
	#define QB_GET_FUNCTION(op_array)		((op_array)->opcodes[0].op2.ptr)
	#define QB_SET_FUNCTION(op_array, p)	(op_array)->opcodes[0].op2.ptr = (void *) p
#else
	#error Incompatible version of PHP
#endif

typedef struct qb_debug_interface			qb_debug_interface;
typedef struct qb_debug_variable_details	qb_debug_variable_details;
typedef struct qb_debug_bitmap				qb_debug_bitmap;

typedef int (*qb_set_compatibility_mode_proc)(int enable);
typedef int (*qb_get_local_variable_details_proc)(void *func_ptr, const char *var_name, qb_debug_variable_details **p_details);
typedef int (*qb_get_instance_variable_details_proc)(zval *obj, const char *var_name, qb_debug_variable_details **p_details);
typedef int (*qb_get_class_variable_details_proc)(zend_class_entry *ce, const char *var_name, qb_debug_variable_details **p_details);
typedef int (*qb_get_global_variable_details_proc)(const char *var_name, qb_debug_variable_details **p_details);
typedef void (*qb_free_variable_details_proc)(qb_debug_variable_details *details);
typedef int (*qb_get_bitmap_proc)(qb_debug_variable_details *details, qb_debug_bitmap **p_bitmap);
typedef void (*qb_free_bitmap_proc)(qb_debug_bitmap *bitmap);

struct qb_debug_interface {
	int signature;
	int struct_size;
	qb_set_compatibility_mode_proc set_compatibility_mode;
	qb_free_bitmap_proc free_bitmap;
	qb_get_bitmap_proc get_bitmap;
	qb_free_variable_details_proc free_variable_details;
	qb_get_local_variable_details_proc get_local_variable_details;
	qb_get_instance_variable_details_proc get_instance_variable_details;
	qb_get_class_variable_details_proc get_class_variable_details;
	qb_get_global_variable_details_proc get_global_variable_details;

};

struct qb_debug_variable_details {
	int flags;
	int bitness;
	void *data;
	zval *zend_variable;
	unsigned int byte_count;
	unsigned int dimension_count;
	unsigned int dimensions[QB_MAXIMUM_DIMENSION_COUNT];
};

struct qb_debug_bitmap {
	unsigned int width;
	unsigned int height;
	int **scanlines;
};

#endif
