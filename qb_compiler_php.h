#ifndef QB_COMPILER_PHP_H_
#define QB_COMPILER_PHP_H_

typedef struct qb_translator			qb_translator;
typedef struct qb_intrinsic_function	qb_intrinsic_function;

typedef void (ZEND_FASTCALL *qb_translator_proc)(qb_compiler_context *cxt, qb_translator *t, qb_operand *operand1, qb_operand *operand2, uint32_t result_flags);
typedef void (ZEND_FASTCALL *qb_intrinsic_coercion_proc)(qb_compiler_context *cxt, qb_intrinsic_function *f, qb_operand *argument, uint32_t argument_index);
typedef void (ZEND_FASTCALL *qb_intrinsic_translate_proc)(qb_compiler_context *cxt, qb_intrinsic_function *f, qb_operand *arguments, uint32_t argument_count, uint32_t result_flags);

struct qb_translator {
	qb_translator_proc translate;
	void *extra;
};

struct qb_intrinsic_function {
	ulong hash_value;
	const char *name;
	qb_intrinsic_coercion_proc do_type_coercion;
	qb_intrinsic_translate_proc translate;
	uint32_t argument_count_min;
	uint32_t argument_count_max;
	void *extra;
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
#else
	#define Z_CLASS_INFO(ce, prop)			ce->prop
	#define Z_OPERAND_INFO(op, prop)		(op).u.prop
	#define Z_OPERAND_TYPE(op)				op.op_type
	#define Z_OPERAND_ZV(op)				&Z_OPERAND_INFO(op, constant)
	#define Z_HASH_P(zv)					zend_inline_hash_func(Z_STRVAL_P(zv), Z_STRLEN_P(zv) + 1)
	#define RETURN_VALUE_USED(opline)		(!((opline)->result.u.EA.type & EXT_TYPE_UNUSED))
#endif

#define ZEND_OP_INDEX(zop)						(((uintptr_t) zop) - ((uintptr_t) cxt->zend_op_array->opcodes)) / sizeof(zend_op)
#define ZEND_OP(index)							&cxt->zend_op_array->opcodes[index]

#endif
