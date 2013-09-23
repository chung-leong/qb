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

#ifndef QB_TRANSLATER_PHP_H_
#define QB_TRANSLATER_PHP_H_

typedef struct qb_php_translater_context	qb_php_translater_context;
typedef struct qb_php_op_translator			qb_php_op_translator;
typedef struct qb_php_function_translator	qb_php_function_translator;

typedef void (ZEND_FASTCALL *qb_php_op_translator_proc)(qb_php_translater_context *cxt, void *op_factory, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_result_prototype *result_prototype);
typedef void (ZEND_FASTCALL *qb_php_intrinsic_translator_proc)(qb_php_translater_context *cxt, qb_php_function_translator *f, qb_operand *arguments, uint32_t argument_count, qb_operand *result, qb_result_prototype *result_prototype);

struct qb_php_op_translator {
	qb_php_op_translator_proc translate;
	void *extra;
};

struct qb_php_function_translator {
	ulong hash_value;
	const char *name;
	uint32_t argument_count_min;
	uint32_t argument_count_max;
	void *extra;
};

struct qb_php_translater_context {
	qb_compiler_context *compiler_context;
	qb_compiler_data_pool *pool;

	qb_function function_prototype;
	qb_function_declaration *function_declaration;
	uint32_t function_flags;

	zend_function *zend_function_being_called;
	zval *zend_this_object;
	zval *zend_class_name;
	zend_class_entry *zend_class;
	zend_op_array *zend_op_array;
	zend_op *zend_op;
	uint32_t zend_op_index;
	uint32_t jump_target_index1;
	uint32_t jump_target_index2;

	int32_t silence;

	qb_storage *storage;

	zend_function *previous_function;
	zend_class_entry *previous_class;
	zval *previous_object;
	zval *previous_function_name;

	void ***tsrm_ls;
};

enum {
	Z_OPERAND_CONST 				= 1,
	Z_OPERAND_TMP_VAR				= 2,
	Z_OPERAND_VAR					= 4,
	Z_OPERAND_UNUSED				= 8,
	Z_OPERAND_CV					= 16,
};

#if !ZEND_ENGINE_2_3 && !ZEND_ENGINE_2_2 && !ZEND_ENGINE_2_1
	#define Z_CLASS_INFO(ce, prop)			ce->info.user.prop
	#define Z_OPERAND_INFO(op, prop)		(op).prop
	#define Z_OPERAND_TYPE(op)				op##_type
	#define Z_OPERAND_ZV(op)				Z_OPERAND_INFO(op, zv)
	#define RETURN_VALUE_USED(opline)		(!((opline)->result_type & EXT_TYPE_UNUSED))
	#define FETCH_TYPE(opline)				(opline->extended_value & ZEND_FETCH_TYPE_MASK)
#else
	#define Z_CLASS_INFO(ce, prop)			ce->prop
	#define Z_OPERAND_INFO(op, prop)		(op).u.prop
	#define Z_OPERAND_TYPE(op)				op.op_type
	#define Z_OPERAND_ZV(op)				&Z_OPERAND_INFO(op, constant)
	#define Z_HASH_P(zv)					zend_inline_hash_func(Z_STRVAL_P(zv), Z_STRLEN_P(zv) + 1)
	#define RETURN_VALUE_USED(opline)		(!((opline)->result.u.EA.type & EXT_TYPE_UNUSED))
	#define FETCH_TYPE(opline)				(opline->op2.u.EA.type)
#endif

#if !ZEND_ENGINE_2_4 && !ZEND_ENGINE_2_3 && !ZEND_ENGINE_2_2 && !ZEND_ENGINE_2_1
	#define Z_OPERAND_TMP_INDEX(op)			((- (int) Z_OPERAND_INFO(*op, var)) / sizeof(temp_variable) - 1)
#else
	#define Z_OPERAND_TMP_INDEX(op)			(Z_OPERAND_INFO(*op, var) / sizeof(temp_variable))
#endif

#define ZEND_OP_INDEX(zop)						(((uintptr_t) zop) - ((uintptr_t) cxt->zend_op_array->opcodes)) / sizeof(zend_op)
#define ZEND_OP(index)							&cxt->zend_op_array->opcodes[index]

#endif
