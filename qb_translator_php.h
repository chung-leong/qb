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

#ifndef QB_TRANSLATOR_PHP_H_
#define QB_TRANSLATOR_PHP_H_

typedef struct qb_php_translator_context	qb_php_translator_context;
typedef struct qb_php_op_translator			qb_php_op_translator;
typedef struct qb_temporary_variable		qb_temporary_variable;

typedef enum zend_operand_type				zend_operand_type;

typedef int32_t (*qb_php_op_translator_proc)(qb_php_translator_context *cxt, void *op_factory, qb_operand *operands, uint32_t operand_count, qb_operand *result, qb_result_prototype *result_prototype);

struct qb_php_op_translator {
	qb_php_op_translator_proc translate;
	void *factory;
};

struct qb_temporary_variable {
	qb_operand operand;
	uint32_t last_access_op_index;
#ifdef ZEND_DEBUG
	int32_t freed;
#endif
};

struct qb_php_translator_context {
	qb_compiler_context *compiler_context;
	qb_data_pool *pool;

	int8_t *op_processed;

	zend_op_array *zend_op_array;
	zend_op *zend_op;
	uint32_t zend_op_index;
	uint32_t next_op_index1;
	uint32_t next_op_index2;

	qb_operand **stack_items;
	uint32_t stack_item_buffer_size;
	uint32_t stack_item_count;
	uint32_t stack_item_offset;

	qb_result_prototype *result_prototypes;
	uint32_t result_prototype_count;

	qb_temporary_variable *temp_variables;
	uint32_t temp_variable_count;

	qb_address *foreach_index_address;

	int32_t silence;
	void ***tsrm_ls;
};

enum zend_operand_type {
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
	#define IGNORE_RETURN_VALUE(opline)		(opline)->result_type |= EXT_TYPE_UNUSED
	#define FETCH_TYPE(opline)				(opline->extended_value & ZEND_FETCH_TYPE_MASK)
#else
	#define Z_CLASS_INFO(ce, prop)			ce->prop
	#define Z_OPERAND_INFO(op, prop)		(op).u.prop
	#define Z_OPERAND_TYPE(op)				op.op_type
	#define Z_OPERAND_ZV(op)				&Z_OPERAND_INFO(op, constant)
	#define Z_HASH_P(zv)					zend_hash_func(Z_STRVAL_P(zv), Z_STRLEN_P(zv) + 1)
	#define RETURN_VALUE_USED(opline)		(!((opline)->result.u.EA.type & EXT_TYPE_UNUSED))
	#define IGNORE_RETURN_VALUE(opline)		(opline)->result.u.EA.type |= EXT_TYPE_UNUSED
	#define FETCH_TYPE(opline)				(opline->op2.u.EA.type)
#endif

#if !ZEND_ENGINE_2_4 && !ZEND_ENGINE_2_3 && !ZEND_ENGINE_2_2 && !ZEND_ENGINE_2_1
	#define Z_OPERAND_TMP_INDEX(op)			((- (int) Z_OPERAND_INFO(*op, var)) / sizeof(temp_variable) - 1)
#elif !ZEND_ENGINE_2_2 && !ZEND_ENGINE_2_1
	#define Z_OPERAND_TMP_INDEX(op)			(Z_OPERAND_INFO(*op, var) / ZEND_MM_ALIGNED_SIZE(sizeof(temp_variable)))
#else
	#define Z_OPERAND_TMP_INDEX(op)			(Z_OPERAND_INFO(*op, var) / sizeof(temp_variable))
#endif

#define ZEND_OP_INDEX(zop)						((uint32_t) (((uintptr_t) zop) - ((uintptr_t) cxt->zend_op_array->opcodes)) / sizeof(zend_op))
#define ZEND_OP(index)							&cxt->zend_op_array->opcodes[index]

void qb_initialize_php_translator_context(qb_php_translator_context *cxt, qb_compiler_context *compiler_cxt TSRMLS_DC);
void qb_free_php_translator_context(qb_php_translator_context *cxt);
int32_t qb_survey_instructions(qb_php_translator_context *cxt);
int32_t qb_translate_instructions(qb_php_translator_context *cxt);

qb_intrinsic_function * qb_find_intrinsic_function(qb_php_translator_context *cxt, zval *callable);

#endif
