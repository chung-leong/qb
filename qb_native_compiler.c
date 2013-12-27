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

#if NATIVE_COMPILE_ENABLED

ZEND_ATTRIBUTE_FORMAT(printf, 2, 3)
static void qb_printf(qb_native_compiler_context *cxt, const char *format, ...) {
	USE_TSRM
	va_list args;
	va_start(args, format);
	vfprintf(cxt->write_stream, format, args);
	va_end(args);

	if(cxt->print_source) {
		char *buffer;
		int len;
		va_start(args, format);
		len = vspprintf(&buffer, 0, format, args);
		php_write(buffer, len TSRMLS_CC);
		efree(buffer);
	}
}

static void qb_print(qb_native_compiler_context *cxt, const char *s) {
	USE_TSRM
	uint32_t len = (uint32_t) strlen(s);
	if(cxt->print_source) {
		php_write((char *) s, len TSRMLS_CC);
	}
	fwrite(s, 1, len, cxt->write_stream);
}

static void qb_print_macros(qb_native_compiler_context *cxt) {
	qb_print(cxt, "#define NAN	"STRING(NAN)"\n");
	qb_print(cxt, "#define INF	"STRING(INFINITY)"\n");
	qb_print(cxt, "#define restrict	__restrict\n");
	qb_print(cxt, "#define NULL	"STRING(NULL)"\n");
	qb_print(cxt, "#define M_PI	"STRING(M_PI)"\n");
	qb_print(cxt, "#define MAX_DIMENSION	"STRING(MAX_DIMENSION)"\n");
#ifdef FAST_FLOAT_TO_INT
	qb_print(cxt, "#define FAST_FLOAT_TO_INT\n");
#endif
#ifdef QB_LITTLE_ENDIAN
	qb_print(cxt, "#define QB_LITTLE_ENDIAN\n");
#else
	qb_print(cxt, "#define QB_BIG_ENDIAN\n");
#endif
	qb_print(cxt, "#define TSRMLS_C		"STRING(TSRMLS_C)"\n");
	qb_print(cxt, "#define TSRMLS_CC	"STRING(TSRMLS_CC)"\n");
	qb_print(cxt, "#define TSRMLS_D		"STRING(TSRMLS_D)"\n");
	qb_print(cxt, "#define TSRMLS_DC	"STRING(TSRMLS_DC)"\n");
	qb_print(cxt, "#define PRId8	"STRING(PRId8)"\n");
	qb_print(cxt, "#define PRId16	"STRING(PRId16)"\n");
	qb_print(cxt, "#define PRId32	"STRING(PRId32)"\n");
	qb_print(cxt, "#define PRId64	"STRING(PRId64)"\n");
	qb_print(cxt, "#define PRIu8	"STRING(PRIu8)"\n");
	qb_print(cxt, "#define PRIu16	"STRING(PRIu16)"\n");
	qb_print(cxt, "#define PRIu32	"STRING(PRIu32)"\n");
	qb_print(cxt, "#define PRIu64	"STRING(PRIu64)"\n");
	qb_print(cxt, "#define NO_RETURN	"STRING(NO_RETURN)"\n");
	qb_print(cxt, "#define EXPECTED(c)	"STRING(EXPECTED(c))"\n");
	qb_print(cxt, "#define UNEXPECTED(c)	"STRING(UNEXPECTED(c))"\n");

	qb_print(cxt, "#define zend_always_inline	"STRING(zend_always_inline)"\n");
	qb_print(cxt, "#define SWAP_BE_I16(v)	"STRING(SWAP_BE_I16(v))"\n");
	qb_print(cxt, "#define SWAP_BE_I32(v)	"STRING(SWAP_BE_I32(v))"\n");
	qb_print(cxt, "#define SWAP_BE_I64(v)	"STRING(SWAP_BE_I64(v))"\n");
	qb_print(cxt, "#define SWAP_LE_I16(v)	"STRING(SWAP_LE_I16(v))"\n");
	qb_print(cxt, "#define SWAP_LE_I32(v)	"STRING(SWAP_LE_I32(v))"\n");
	qb_print(cxt, "#define SWAP_LE_I64(v)	"STRING(SWAP_LE_I64(v))"\n");
	qb_print(cxt, "#define TRUE	"STRING(TRUE)"\n");
	qb_print(cxt, "#define FALSE	"STRING(FALSE)"\n");
	qb_print(cxt, "#define zend_isinf(x)	"STRING(zend_isinf(x))"\n");
	qb_print(cxt, "#define zend_finite(x)	"STRING(zend_finite(x))"\n");
	qb_print(cxt, "#define zend_isnan(x)	"STRING(zend_isnan(x))"\n");

#ifdef __GNUC__
#ifndef __builtin_bswap16
	qb_print(cxt, "#define __builtin_bswap16(n)	((n >> 8) | (n << 8))\n");
#endif
#endif
}

static void qb_print_typedefs(qb_native_compiler_context *cxt) {
#ifdef __GNUC__
	qb_print(cxt, STRING(typedef __SIZE_TYPE__ size_t;)"\n");
#endif
#ifdef _MSC_VER
	#ifdef  _WIN64
	qb_print(cxt, "typedef unsigned __int64 size_t;\n");
	#else
	qb_print(cxt, "typedef unsigned int size_t;\n");
	#endif
#endif
	qb_print(cxt, "typedef char int8_t;\n");
	qb_print(cxt, "typedef unsigned char uint8_t;\n");
	qb_print(cxt, "typedef short int16_t;\n");
	qb_print(cxt, "typedef unsigned short uint16_t;\n");
	qb_print(cxt, "typedef int int32_t;\n");
	qb_print(cxt, "typedef unsigned int uint32_t;\n");
#if SIZEOF_LONG == 8
	qb_print(cxt, "typedef long int64_t;\n");
	qb_print(cxt, "typedef unsigned long uint64_t;\n");
#else
	qb_print(cxt, "typedef long long int64_t;\n");
	qb_print(cxt, "typedef unsigned long long uint64_t;\n");
#endif
#if SIZEOF_PTRDIFF_T == 8
	qb_print(cxt, "typedef int64_t intptr_t;\n");
	qb_print(cxt, "typedef uint64_t uintptr_t;\n");
#else
	qb_print(cxt, "typedef int32_t intptr_t;\n");
	qb_print(cxt, "typedef uint32_t uintptr_t;\n");
#endif

	qb_print(cxt, "typedef float float32_t;\n");
	qb_print(cxt, "typedef double float64_t;\n");
	qb_print(cxt, "\n");

	qb_print(cxt, "typedef struct qb_address\tqb_address;\n");
	qb_print(cxt, "typedef struct qb_variable\tqb_variable;\n");
	qb_print(cxt, "typedef struct zend_op_array\tzend_op_array;\n");
	qb_print(cxt, "typedef struct zval\tzval;\n");
	qb_print(cxt, "typedef struct php_stream\tphp_stream;\n");
	qb_print(cxt, "typedef struct qb_index_alias_scheme\tqb_index_alias_scheme;\n");
	qb_print(cxt, "typedef struct qb_memory_segment\tqb_memory_segment;\n");
	qb_print(cxt, "typedef struct qb_storage\tqb_storage;\n");
	qb_print(cxt, "typedef struct qb_function\tqb_function;\n");
	qb_print(cxt, "typedef struct qb_interpreter_context\tqb_interpreter_context;\n");
	qb_print(cxt, "typedef struct qb_external_symbol\tqb_external_symbol;\n");
	qb_print(cxt, "typedef struct qb_thread\tqb_thread;\n");
	
	qb_print(cxt, "typedef enum qb_vm_exit_type\tqb_vm_exit_type;\n");

	qb_print(cxt, "\
enum qb_vm_exit_type {\
	QB_VM_RETURN = 0,\
	QB_VM_ERROR,\
	QB_VM_WARNING,\
	QB_VM_TIMEOUT,\
	QB_VM_FORK,\
	QB_VM_SPOON,\
	QB_VM_EXCEPTION,\
	QB_VM_YIELD,\
};\
\n");

	qb_print(cxt, "\
struct qb_memory_segment {\
	int8_t *memory;\
	uint32_t flags;\
	uint32_t byte_count;\
	uint32_t current_allocation;\
	php_stream *stream;\
	qb_memory_segment *imported_segment;\
	qb_memory_segment *next_dependent;\
	uintptr_t **references;\
	uint32_t reference_count;\
};\
\n");

	qb_print(cxt, "\
struct qb_storage {\
	uint32_t size;\
	qb_memory_segment *segments;\
	uint32_t segment_count;\
	uint32_t flags;\
};\
\n");

	qb_print(cxt, "\
struct qb_external_symbol {\
	long hash_value;\
	const char *name;\
	void *address;\
};\
\n");

	qb_print(cxt, "\
struct qb_function {\
	int8_t *instructions;\
	int8_t *instruction_start;\
	uint64_t instruction_crc64;\
	uint32_t instruction_length;\
	uint16_t *instruction_opcodes;\
	uint32_t instruction_opcode_count;\
	uint32_t flags;\
	qb_variable *return_variable;\
	qb_variable *return_key_variable;\
	qb_variable *sent_variable;\
	qb_variable **variables;\
	uint32_t variable_count;\
	uint32_t argument_count;\
	qb_storage *local_storage;\
	const char *name;\
	uint32_t line_id;\
	void *native_proc;\
	uintptr_t instruction_base_address;\
	uintptr_t local_storage_base_address;\
	zend_op_array *zend_op_array;\
	qb_function *next_reentrance_copy;\
	qb_function *next_forked_copy;\
	volatile int32_t in_use;\
};\
\n");

	qb_print(cxt, "\
struct qb_interpreter_context {\
	qb_function *function;\
	int8_t *instruction_pointer;\
	qb_interpreter_context *caller_context;\
	qb_thread *thread;\
	uint32_t thread_count;\
	uint32_t fork_id;\
	uint32_t fork_count;\
	uint32_t *argument_indices;\
	uint32_t argument_count;\
	uint32_t result_index;\
	uint32_t line_id;\
	uint32_t call_depth;\
	void *send_target;\
	qb_vm_exit_type exit_type;\
	int32_t exit_status_code;\
	int32_t exception_encountered;\
	uint32_t debug_flags;\
	volatile unsigned char *windows_timed_out_pointer;\
	int floating_point_precision;\
	void ***tsrm_ls;\
};\
\n");

#if ZEND_DEBUG
	qb_print(cxt, "\
typedef struct qb_native_proc_record {\
	uint64_t crc64;\
	void *proc;\
} qb_native_proc_record;\
\n");
#endif

	qb_print(cxt, "\n");
}

#define PROTOTYPE_COUNT		2000

extern uint32_t illegal_vc11_intrinsic_indices[];
extern uint32_t illegal_vc11_intrinsic_count;

static void qb_print_prototypes(qb_native_compiler_context *cxt) {
	uint32_t i, j, k;
	int32_t *prototype_indices;
	uint32_t required[PROTOTYPE_COUNT];
	qb_op *qop;

	// see which functions are required
	memset(required, 0, sizeof(required));
	for(i = 0; i < cxt->compiler_context_count; i++) {
		qb_compiler_context *compiler_cxt = cxt->compiler_contexts[i];
		if(!compiler_cxt->compiled_function->native_proc && (compiler_cxt->function_flags & QB_FUNCTION_NATIVE_IF_POSSIBLE)) {
			// go through all ops and see what functions they use
			for(j = 0; j < compiler_cxt->op_count; j++) {
				qop = compiler_cxt->ops[j];
				prototype_indices = cxt->op_function_usages[qop->opcode];
				for(k = 0; prototype_indices[k] != 0xFFFFFFFF; k++) {
					uint32_t index = prototype_indices[k];
					qb_native_symbol *symbol = &global_native_symbols[index];
#if _MSV_VER >= 1600 
					if(symbol->flags & QB_NATIVE_SYMBOL_INTRINSIC_FUNCTION) {
						// symbol is an intrinsic in VC11--don't print prototype for it
						continue;
					}
#endif
					required[index] = TRUE;
				}
			}
		}
	}

	// print prototypes, maintaining correct order
	for(i = 0; i < PROTOTYPE_COUNT; i++) {
		if(required[i]) {
			const char *prototype = cxt->function_prototypes[i];
#ifdef _WIN64
			qb_native_symbol *symbol = &global_native_symbols[i];
			if(!symbol->flags) {
				// add __declspec so function looks like a something frm a DLL 
				qb_print(cxt, "__declspec(dllimport)\n");
			} else if(symbol->flags & QB_NATIVE_SYMBOL_INTRINSIC_FUNCTION) {
				continue;
			}
#endif
			qb_print(cxt, prototype);
			qb_print(cxt, "\n");
		}
	}
	qb_print(cxt, "\n");
}

static char * qb_get_buffer(qb_native_compiler_context *cxt) {
	if(cxt->string_buffer_index == 16) {
		cxt->string_buffer_index = 0;
	}
	return cxt->string_buffers[cxt->string_buffer_index++];
}

static qb_address * qb_get_root_address(qb_native_compiler_context *cxt, qb_address *address) {
	while(address->source_address) {
		address = address->source_address;
	}
	return address;
}

static int32_t qb_cast_required(qb_native_compiler_context *cxt, qb_address *address) {
	qb_address *base_address = qb_get_root_address(cxt, address);
	return (base_address->type != address->type);
}

static int32_t qb_offset_required(qb_native_compiler_context *cxt, qb_address *address) {
	if(address->array_index_address != cxt->zero_address) {
		return TRUE;
	} else {
		qb_address *base_address = qb_get_root_address(cxt, address);
		return (base_address->segment_offset != address->segment_offset);
	}
}

static qb_access_method qb_get_scalar_access_method(qb_native_compiler_context *cxt, qb_address *address) {
	switch(address->segment_selector) {
		case QB_SELECTOR_CONSTANT_ARRAY:
		case QB_SELECTOR_CONSTANT_SCALAR: return QB_SCALAR_LITERAL;
		case QB_SELECTOR_CLASS_SCALAR:
		case QB_SELECTOR_OBJECT_SCALAR:
		case QB_SELECTOR_GLOBAL_SCALAR:
		case QB_SELECTOR_STATIC_SCALAR:
		case QB_SELECTOR_SHARED_SCALAR: return QB_SCALAR_POINTER;
		case QB_SELECTOR_LOCAL_SCALAR:
		case QB_SELECTOR_TEMPORARY_SCALAR: {
			if(address->flags & QB_ADDRESS_DIMENSION) {
				return QB_SCALAR_POINTER;
			} else {
				return QB_SCALAR_LOCAL_VARIABLE;
			}
		}
		case INVALID_INDEX: {
			return QB_ARRAY_UNUSED;
		}
		default: return QB_SCALAR_ELEMENT;
	}
}

static qb_access_method qb_get_array_access_method(qb_native_compiler_context *cxt, qb_address *address) {
	switch(address->segment_selector) {
		case QB_SELECTOR_CONSTANT_SCALAR:
		case QB_SELECTOR_CLASS_SCALAR:
		case QB_SELECTOR_OBJECT_SCALAR:
		case QB_SELECTOR_GLOBAL_SCALAR:
		case QB_SELECTOR_STATIC_SCALAR:
		case QB_SELECTOR_SHARED_SCALAR: return QB_SCALAR_POINTER;
		case QB_SELECTOR_LOCAL_SCALAR:
		case QB_SELECTOR_TEMPORARY_SCALAR: {
			if(address->flags & QB_ADDRESS_DIMENSION) {
				return QB_SCALAR_POINTER;
			} else {
				return QB_SCALAR_LOCAL_VARIABLE;
			}
		}
		case QB_SELECTOR_CONSTANT_ARRAY:
		case QB_SELECTOR_CLASS_ARRAY:
		case QB_SELECTOR_OBJECT_ARRAY:
		case QB_SELECTOR_GLOBAL_ARRAY:
		case QB_SELECTOR_SHARED_ARRAY: 
		case QB_SELECTOR_LOCAL_ARRAY:
		case QB_SELECTOR_TEMPORARY_ARRAY: {
			if(qb_offset_required(cxt, address)) {
				return QB_ARRAY_SLICE;
			} else {
				return QB_ARRAY_POINTER;
			}
		}
		case INVALID_INDEX: {
			return QB_ARRAY_UNUSED;
		}
		default: {
			if(qb_offset_required(cxt, address)) {
				return QB_ARRAY_SLICE;
			} else {
				return QB_ARRAY_POINTER_POINTER;
			}
		}
	}
}

static const char * qb_get_pointer(qb_native_compiler_context *cxt, qb_address *address);

static const char * qb_get_scalar(qb_native_compiler_context *cxt, qb_address *address) {
	char *buffer = qb_get_buffer(cxt);
	const char *ctype = qb_cast_required(cxt, address) ? type_cnames[address->type] : NULL;
	qb_access_method method = qb_get_scalar_access_method(cxt, address);
	switch(method) {
		case QB_SCALAR_LITERAL: {
			// scalar is a constant
			switch(address->type) {
				case QB_TYPE_S08: snprintf(buffer, 128, "%" PRId8, VALUE(S08, address)); break;
				case QB_TYPE_U08: snprintf(buffer, 128, "%" PRIu8"U", VALUE(U08, address)); break;
				case QB_TYPE_S16: snprintf(buffer, 128, "%" PRId16, VALUE(S16, address)); break;
				case QB_TYPE_U16: snprintf(buffer, 128, "%" PRIu16"U", VALUE(U16, address)); break;
				case QB_TYPE_S32: snprintf(buffer, 128, "%" PRId32, VALUE(S32, address)); break;
				case QB_TYPE_U32: snprintf(buffer, 128, "%" PRIu32"U", VALUE(U32, address)); break;
				case QB_TYPE_S64: snprintf(buffer, 128, "%" PRId64"LL", VALUE(S64, address)); break;
				case QB_TYPE_U64: snprintf(buffer, 128, "%" PRIu64"ULL", VALUE(U64, address)); break;
				case QB_TYPE_F32: {
					float32_t number = VALUE(F32, address);
					if(isnan(number)) {
						snprintf(buffer, 128, "NAN");
					} else if(zend_finite(number)) {
						snprintf(buffer, 128, "%.11ff", number); 
					} else {
						if(number > 0) {
							snprintf(buffer, 128, "INF");
						} else {
							snprintf(buffer, 128, "-INF");
						}
					}
				}	break;
				case QB_TYPE_F64: {
					float64_t number = VALUE(F64, address);
					if(isnan(number)) {
						snprintf(buffer, 128, "NAN");
					} else if(zend_finite(number)) {
						snprintf(buffer, 128, "%.17f", number); 
					} else {
						if(number > 0) {
							snprintf(buffer, 128, "INF");
						} else {
							snprintf(buffer, 128, "-INF");
						}
					}
				}	break;
				default: break;
			}
		}	break;
		case QB_SCALAR_POINTER: {
			// scalar is referenced by pointer
			if(ctype) {
				snprintf(buffer, 128, "((%s) (*var_ptr_%u_%u))", ctype, address->segment_selector, address->segment_offset);
			} else {
				snprintf(buffer, 128, "(*var_ptr_%u_%u)", address->segment_selector, address->segment_offset);
			}
		}	break;
		case QB_SCALAR_LOCAL_VARIABLE: {
			// scalar is a local variable
			if(ctype) {
				snprintf(buffer, 128, "((%s) var_%u_%u)", ctype, address->segment_selector, address->segment_offset);
			} else {
				snprintf(buffer, 128, "var_%u_%u", address->segment_selector, address->segment_offset);
			}
		}	break;
		case QB_SCALAR_ELEMENT: {
			// array elements 
			qb_address *base_address = qb_get_root_address(cxt, address);
			const char *container = qb_get_pointer(cxt, base_address);
			if(address->array_index_address == cxt->zero_address) {
				// referenced by a fixed index
				uint32_t index = ELEMENT_COUNT(address->segment_offset - base_address->segment_offset, address->type);
				if(ctype) {
					snprintf(buffer, 128, "((%s) %s[%u])", ctype, container, index);
				} else {
					snprintf(buffer, 128, "(%s[%u])", container, index);
				}
			} else {
				// variable index
				const char *index = qb_get_scalar(cxt, address->array_index_address);
				if(ctype) {
					snprintf(buffer, 128, "((%s) %s[%s])", ctype, container, index);
				} else {
					snprintf(buffer, 128, "(%s[%s])", container, index);
				}
			}
		}	break;
		default: {
		}	break;
	}
	return buffer;
}

static const char * qb_get_pointer(qb_native_compiler_context *cxt, qb_address *address) {
	char *buffer = qb_get_buffer(cxt);
	const char *ctype = qb_cast_required(cxt, address) ? type_cnames[address->type] : NULL;
	qb_access_method method = qb_get_array_access_method(cxt, address);
	switch(method) {
		case QB_SCALAR_POINTER: {
			if(ctype) {
				snprintf(buffer, 128, "((%s *) var_ptr_%u_%u)", ctype, address->segment_selector, address->segment_offset);
			} else {
				snprintf(buffer, 128, "var_ptr_%u_%u", address->segment_selector, address->segment_offset);
			}
		}	break;
		case QB_SCALAR_LOCAL_VARIABLE: {
			if(ctype) {
				snprintf(buffer, 128, "((%s *) &var_%u_%u)", ctype, address->segment_selector, address->segment_offset);
			} else {
				snprintf(buffer, 128, "&var_%u_%u", address->segment_selector, address->segment_offset);
			}
		}	break;
		case QB_ARRAY_POINTER: {
			// fixed-length array
			if(ctype) {
				snprintf(buffer, 128, "((%s *) var_ptr_%u_%u)", ctype, address->segment_selector, address->segment_offset);
			} else {
				snprintf(buffer, 128, "var_ptr_%u_%u", address->segment_selector, address->segment_offset);
			}
		}	break;
		case QB_ARRAY_POINTER_POINTER: {
			// a variable-length array--dereference the pointer to pointer
			if(ctype) {
				snprintf(buffer, 128, "((%s *) (*var_ptr_ptr_%u))", ctype, address->segment_selector);
			} else {
				snprintf(buffer, 128, "(*var_ptr_ptr_%u)", address->segment_selector);
			}
		}	break;
		case QB_ARRAY_SLICE: {
			qb_address *base_address = qb_get_root_address(cxt, address);
			const char *container = qb_get_pointer(cxt, base_address);
			if(address->array_index_address == cxt->zero_address) {
				uint32_t offset = ELEMENT_COUNT(address->segment_offset - base_address->segment_offset, address->type);
				if(ctype) {
					snprintf(buffer, 128, "(((%s *) %s) + %u)", ctype, container, offset);
				} else {
					snprintf(buffer, 128, "(%s + %u)", container, offset);
				}
			} else {
				const char *offset = qb_get_scalar(cxt, address->array_index_address);
				if(ctype) {
					snprintf(buffer, 128, "(((%s *) %s) + %s)", ctype, container, offset);
				} else {
					snprintf(buffer, 128, "(%s + %s)", container, offset);
				}
			}
		}	break;
		default: {
		}	break;
	}
	return buffer;
}

static const char * qb_get_jump_label(qb_native_compiler_context *cxt, uint32_t jump_target_index) {
	char *buffer = qb_get_buffer(cxt);
	snprintf(buffer, 128, "L%04d", jump_target_index);
	return buffer;
}

static const char * qb_get_op_action(qb_native_compiler_context *cxt, uint32_t opcode) {
	return cxt->op_actions[opcode];
}

extern const char compressed_table_op_names[];

static const char * qb_get_op_name(qb_native_compiler_context *cxt, uint32_t opcode) {
	if(!cxt->pool->op_names) {
		// decompress the opname table
		qb_uncompress_table(compressed_table_op_names, (void ***) &cxt->pool->op_names, NULL, 0);
		if(!cxt->pool->op_names) {
			return "?";
		}
	}
	return cxt->pool->op_names[opcode];
}

static void qb_copy_local_scalar_to_storage(qb_native_compiler_context *cxt, qb_address *address) {
	qb_access_method method = qb_get_scalar_access_method(cxt, address);
	if(method == QB_SCALAR_LOCAL_VARIABLE) {
		const char *c_type = type_cnames[address->type];
		qb_printf(cxt, "*((%s *) (storage->segments[%u].memory + %u)) = var_%u_%u;\n", c_type, address->segment_selector, address->segment_offset, address->segment_selector, address->segment_offset);
	}
}

static void qb_copy_local_variables_to_storage(qb_native_compiler_context *cxt, qb_op *qop) {
	uint32_t i;
	for(i = 0; i < qop->operand_count; i++) {
		qb_operand *operand = &qop->operands[i];
		if(operand->type == QB_OPERAND_ADDRESS) {
			qb_address *address = operand->address;
			switch(address->mode) {
				case QB_ADDRESS_MODE_SCA: {
					qb_copy_local_scalar_to_storage(cxt, address);
				}	break;
				case QB_ADDRESS_MODE_ELE: {
					qb_copy_local_scalar_to_storage(cxt, address);
					qb_copy_local_scalar_to_storage(cxt, address->array_index_address);
				}	break;
				case QB_ADDRESS_MODE_ARR: {
					qb_copy_local_scalar_to_storage(cxt, address);
					qb_copy_local_scalar_to_storage(cxt, address->array_index_address);
				}	break;
				default: {
				}	break;
			}
		}
	}
}

static void qb_copy_local_arguments_to_storage(qb_native_compiler_context *cxt, qb_op *qop) {
	uint32_t *var_indices = ARRAY(U32, qop->operands[1].address);
	uint32_t arg_count = ARRAY_SIZE(qop->operands[1].address);
	uint32_t i;
	for(i = 0; i < arg_count; i++) {
		qb_variable *qvar = cxt->variables[var_indices[i]];
		qb_address *address = qvar->address;
		switch(address->mode) {
			case QB_ADDRESS_MODE_SCA: {
				qb_copy_local_scalar_to_storage(cxt, address);
			}	break;
			default: {
			}	break;
		}
	}
}

static void qb_copy_all_local_variables_to_storage(qb_native_compiler_context *cxt) {
	uint32_t i;
	for(i = 0; i < cxt->writable_scalar_count; i++) {
		qb_address *address = cxt->writable_scalars[i];
		qb_copy_local_scalar_to_storage(cxt, address);
	}
}

static void qb_copy_local_scalar_from_storage(qb_native_compiler_context *cxt, qb_address *address) {
	qb_access_method method = qb_get_scalar_access_method(cxt, address);
	if(method == QB_SCALAR_LOCAL_VARIABLE) {
		const char *c_type = type_cnames[address->type];
		qb_printf(cxt, "var_%u_%u = *((%s *) (storage->segments[%u].memory + %u));\n", address->segment_selector, address->segment_offset, c_type, address->segment_selector, address->segment_offset);
	}
}

static void qb_copy_local_variables_from_storage(qb_native_compiler_context *cxt, qb_op *qop) {
	uint32_t i;
	for(i = 0; i < qop->operand_count; i++) {
		qb_operand *operand = &qop->operands[i];
		if(operand->type == QB_OPERAND_ADDRESS) {
			if(qb_is_operand_write_target(qop->opcode, i)) {
				qb_address *address = operand->address;
				switch(address->mode) {
					case QB_ADDRESS_MODE_SCA: {
						qb_copy_local_scalar_from_storage(cxt, address);
					}	break;
					case QB_ADDRESS_MODE_ELE: {
						qb_copy_local_scalar_from_storage(cxt, address);
						qb_copy_local_scalar_from_storage(cxt, address->array_index_address);
					}	break;
					case QB_ADDRESS_MODE_ARR: {
						qb_copy_local_scalar_from_storage(cxt, address);
						qb_copy_local_scalar_from_storage(cxt, address->array_index_address);
					}	break;
					default: {
					}	break;
				}
			}
		}
	}
}

static void qb_copy_local_arguments_from_storage(qb_native_compiler_context *cxt, qb_op *qop) {
	USE_TSRM
	uint32_t symbol_index = VALUE(U32, qop->operands[0].address);
	qb_external_symbol *symbol = &QB_G(external_symbols)[symbol_index];
	zend_function *zfunc = symbol->pointer;
	qb_function *qfunc = qb_get_compiled_function(zfunc);
	uint32_t *var_indices = ARRAY(U32, qop->operands[1].address);
	uint32_t arg_count = ARRAY_SIZE(qop->operands[1].address);
	uint32_t retval_index = VALUE(U32, qop->operands[2].address);
	uint32_t i;
	for(i = 0; i < arg_count; i++) {
		int32_t by_ref;
		if(qfunc) {
			if(i < qfunc->argument_count ) {
				qb_variable *arg = qfunc->variables[i];
				by_ref = (arg->flags & QB_VARIABLE_BY_REF);
			} else {
				by_ref = FALSE;
			}
		} else {
			if(zfunc->common.arg_info && i < zfunc->common.num_args) {
				by_ref = zfunc->common.arg_info[i].pass_by_reference;
			} else {
				by_ref = TRUE;
			}
		}
		if(by_ref) {
			qb_variable *qvar = cxt->variables[var_indices[i]];
			qb_address *address = qvar->address;
			switch(address->mode) {
				case QB_ADDRESS_MODE_SCA: {
					qb_copy_local_scalar_from_storage(cxt, address);
				}	break;
				default: {
				}	break;
			}
		}
	}
	if(retval_index != INVALID_INDEX) {
		qb_variable *qvar = cxt->variables[retval_index];
		qb_address *address = qvar->address;
		switch(address->mode) {
			case QB_ADDRESS_MODE_SCA: {
				qb_copy_local_scalar_from_storage(cxt, address);
			}	break;
			default: {
			}	break;
		}
	}
}

static void qb_print_op(qb_native_compiler_context *cxt, qb_op *qop, uint32_t qop_index) {
	if(qop->flags & QB_OP_JUMP_TARGET) {
		const char *label = qb_get_jump_label(cxt, qop_index);
		qb_printf(cxt, "%s:\n", label);
	}
	if(qop->opcode != QB_NOP) {
		const char *name;
		const char *action;
		uint32_t i;

		if(cxt->print_source || TRUE) {
			name = qb_get_op_name(cxt, qop->opcode);
			qb_printf(cxt, "// %s (line #%u)\n", name, LINE_NUMBER(qop->line_id));
		}

		// define the operands
		if(qop->flags & QB_OP_NEED_LINE_IDENTIFIER) {
			qb_printf(cxt, "#define line_id	%u\n", qop->line_id);
		}
		for(i = 0; i < qop->operand_count; i++) {
			qb_operand *operand = &qop->operands[i];
			qb_address *address = operand->address;
			char name[8];
			if(i == qop->operand_count - 1 && qb_is_operand_write_target(qop->opcode, i)) {
				sprintf(name, "res");
			} else {
				sprintf(name, "op%u", i + 1);
			}
			if(operand->type == QB_OPERAND_ADDRESS) {
				switch(address->mode) {
					case QB_ADDRESS_MODE_SCA:
					case QB_ADDRESS_MODE_ELE: {
						const char *scalar = qb_get_scalar(cxt, address);
						qb_printf(cxt, "#define %s	%s\n", name, scalar);
					}	break;
					case QB_ADDRESS_MODE_ARR: {
						if(address->array_size_address != cxt->zero_address) {
							const char *count = qb_get_scalar(cxt, address->array_size_address);
							const char *pointer = qb_get_pointer(cxt, address);
							const char *count_pointer = qb_get_pointer(cxt, address->array_size_address);
							qb_printf(cxt, "#define %s_ptr	%s\n", name, pointer);
							qb_printf(cxt, "#define %s_count	%s\n", name, count);
							qb_printf(cxt, "#define %s_count_ptr	%s\n", name, count_pointer);
						} else {
							qb_printf(cxt, "#define %s_ptr	NULL\n", name);
							qb_printf(cxt, "#define %s_count	0U\n", name);
						}
					}	break;
					default: {
					}	break;
				}
			} else if(operand->type == QB_OPERAND_SEGMENT_SELECTOR) {
				qb_printf(cxt, "#define %s	%u\n", name, address->segment_selector);
			} else if(operand->type == QB_OPERAND_ELEMENT_SIZE) {
				qb_printf(cxt, "#define %s	%u\n", name, BYTE_COUNT(1, address->type));
			} else if(operand->type == QB_OPERAND_NUMBER) {
				qb_printf(cxt, "#define %s	%u\n", name, operand->number);
			}
		}
		qb_print(cxt, "\n");

		if(qop->flags & QB_OP_NEED_INSTRUCTION_STRUCT) {
			qb_copy_local_variables_to_storage(cxt, qop);
			qb_printf(cxt, "ip = cxt->function->instructions + %u;\n", qop->instruction_offset);
			qb_print(cxt, "\n");
		} else if(qop->opcode == QB_RET) {
			if(cxt->compiled_function->return_variable->address) {
				if(SCALAR(cxt->compiled_function->return_variable->address)) {
					qb_copy_local_scalar_to_storage(cxt, cxt->compiled_function->return_variable->address);
					qb_print(cxt, "\n");
				}
			}
		} else if(qop->opcode == QB_FCALL_U32_U32_U32 || qop->opcode == QB_FCALL_MT_U32_U32_U32) {
			qb_copy_local_arguments_to_storage(cxt, qop);
			qb_print(cxt, "\n");
		} else if(qop->opcode == QB_END_STATIC || qop->opcode == QB_RESUME || qop->opcode == QB_SPOON) {
			qb_printf(cxt, "ip = cxt->function->instructions + %u;\n", qop->instruction_offset);
		} else if(qop->opcode == QB_INTR || qop->opcode == QB_FORK_U32) {
			qb_printf(cxt, "ip = cxt->function->instructions + %u;\n", qop->instruction_offset);
			qb_copy_all_local_variables_to_storage(cxt);
		}

		// print code that actually performs the action
		action = qb_get_op_action(cxt, qop->opcode);
		if(action) {
			qb_print(cxt, action);
			qb_print(cxt, "\n");
		}

		if(qop->flags & QB_OP_NEED_INSTRUCTION_STRUCT) {
			qb_copy_local_variables_from_storage(cxt, qop);
			qb_print(cxt, "\n");
		} else if(qop->opcode == QB_FCALL_U32_U32_U32 || qop->opcode == QB_FCALL_MT_U32_U32_U32) {
			qb_copy_local_arguments_from_storage(cxt, qop);
			qb_print(cxt, "\n");
		} else if(qop->opcode == QB_END_STATIC || qop->opcode == QB_INTR || qop->opcode == QB_RESUME || qop->opcode == QB_SPOON) {
			if(!(cxt->ops[qop_index + 1]->flags & QB_OP_JUMP_TARGET)) {
				const char *jump_target = qb_get_jump_label(cxt, qop_index + 1);
				qb_printf(cxt, "%s:\n", jump_target);
			}
		}

#ifdef ZEND_WIN32				
		if(qop->flags & (QB_OP_BRANCH | QB_OP_JUMP)) {
			// check for timed-out condition on any backward jump
			uint32_t j;
			for(j = 0; j < qop->jump_target_count; j++) {
				if(qop->jump_target_indices[j] <= qop_index) {
					qb_print(cxt, "if(*cxt->windows_timed_out_pointer) {\n");
					qb_print(cxt,		"cxt->exit_type = QB_VM_TIMEOUT;\n");
					qb_print(cxt,		"return;\n");
					qb_print(cxt, "}\n");
					break;
				}
			}
		}
#endif

		if(qop->flags & QB_OP_BRANCH) {
			if(qop->jump_target_indices[1] == qop_index + 1) {
				const char *jump_target = qb_get_jump_label(cxt, qop->jump_target_indices[0]);
				qb_printf(cxt, "if(condition) goto %s;\n", jump_target);
			} else if(qop->jump_target_indices[0] == qop_index + 1) {
				const char *jump_target = qb_get_jump_label(cxt, qop->jump_target_indices[1]);
				qb_printf(cxt, "if(!condition) goto %s;\n", jump_target);
			} else {
				const char *jump_target1 = qb_get_jump_label(cxt, qop->jump_target_indices[0]);
				const char *jump_target2 = qb_get_jump_label(cxt, qop->jump_target_indices[1]);
				qb_printf(cxt, "if(condition) goto %s; else goto %s;\n", jump_target1, jump_target2);
			}
			qb_print(cxt, "\n");
		} else if(qop->flags & QB_OP_JUMP) {
			const char *jump_target = qb_get_jump_label(cxt, qop->jump_target_indices[0]);
			qb_printf(cxt, "goto %s;\n", jump_target);
			qb_print(cxt, "\n");
		} else if(qop->flags & QB_OP_BRANCH_TABLE) {
#ifdef __GNUC__
			// use computed goto
			qb_printf(cxt, "static void *branch_table_%u[%u] = {", qop_index, qop->jump_target_count);
			for(i = 0; i < qop->jump_target_count; i++) {
				uint32_t jump_target_index = qop->jump_target_indices[i];
				const char *jump_target = qb_get_jump_label(cxt, jump_target_index);
				qb_printf(cxt, "&&%s,\n", jump_target);
			}
			qb_print(cxt, "};\n");
			qb_printf(cxt, "goto *branch_table_%u[offset];\n", qop_index);
#else
			// create a switch statement for MSVC
			uint32_t case_count = qop->jump_target_count - 1;
			uint32_t default_jump_target_index = qop->jump_target_indices[case_count];
			const char *default_jump_target = qb_get_jump_label(cxt, default_jump_target_index);
			qb_print(cxt, "switch(offset) {\n");
			for(i = 0; i < case_count; i++) {
				uint32_t jump_target_index = qop->jump_target_indices[i];
				if(jump_target_index != default_jump_target_index) {
					uint32_t next_jump_target_index = qop->jump_target_indices[i + 1];
					if(jump_target_index == next_jump_target_index) {
						// fall through to the next case
						qb_printf(cxt, "case %u:\n", i);
					} else {
						const char *jump_target = qb_get_jump_label(cxt, jump_target_index);
						qb_printf(cxt, "case %u: goto %s;\n", i, jump_target);
					}
				}
			}
			qb_printf(cxt, "default: goto %s;\n", default_jump_target);
			qb_print(cxt, "}\n");
#endif
		}

#ifdef ZEND_DEBUG
		if(qop->flags & QB_OP_NEED_LINE_IDENTIFIER) {
			qb_printf(cxt, "#undef line_id\n");
		}
		for(i = 0; i < qop->operand_count; i++) {
			qb_operand *operand = &qop->operands[i];
			qb_address *address = operand->address;
			char name[8];
			if(i == qop->operand_count - 1 && qb_is_operand_write_target(qop->opcode, i)) {
				sprintf(name, "res");
			} else {
				sprintf(name, "op%u", i + 1);
			}
			if(operand->type == QB_OPERAND_ADDRESS) {
				switch(address->mode) {
					case QB_ADDRESS_MODE_SCA:
					case QB_ADDRESS_MODE_ELE: {
						qb_printf(cxt, "#undef %s\n", name);
					}	break;
					case QB_ADDRESS_MODE_ARR: {
						if(address->array_size_address != cxt->zero_address) {
							qb_printf(cxt, "#undef %s_ptr\n", name);
							qb_printf(cxt, "#undef %s_count\n", name);
							qb_printf(cxt, "#undef %s_count_ptr\n", name);
						} else {
							qb_printf(cxt, "#undef %s_ptr\n", name);
							qb_printf(cxt, "#undef %s_count\n", name);
						}
					}	break;
					default: {
					}	break;
				}
			} else if(operand->type == QB_OPERAND_SEGMENT_SELECTOR) {
				qb_printf(cxt, "#undef %s\n", name);
			} else if(operand->type == QB_OPERAND_ELEMENT_SIZE) {
				qb_printf(cxt, "#undef %s\n", name);
			} else if(operand->type == QB_OPERAND_NUMBER) {
				qb_printf(cxt, "#undef %s\n", name);
			}
		}
		qb_print(cxt, "\n");
#endif
	}
}

static void qb_print_local_variables(qb_native_compiler_context *cxt) {
	uint32_t i, j, k;
	int32_t *constant_as_pointer;
	ALLOCA_FLAG(use_heap);

	qb_print(cxt, "int8_t *ip;\n");
	qb_print(cxt, "int condition;\n");
	qb_print(cxt, "unsigned int offset;\n");
	qb_print(cxt, "qb_storage *storage = cxt->function->local_storage;\n");

	// create variables for writable scalars
	for(i = 0; i < cxt->writable_scalar_count; i++) {
		qb_address *address = cxt->writable_scalars[i];
		const char *c_type = type_cnames[address->type];
		qb_access_method method = qb_get_scalar_access_method(cxt, address);
		switch(method) {
			case QB_SCALAR_POINTER: {
				// create pointers to the value in the storage
				qb_printf(cxt, "%s *var_ptr_%u_%u = (%s *) (storage->segments[%u].memory + %u);\n", c_type, address->segment_selector, address->segment_offset, c_type, address->segment_selector, address->segment_offset);
			}	break;
			case QB_SCALAR_LOCAL_VARIABLE: {
				// create local variables
				qb_printf(cxt, "%s var_%u_%u = *((%s *) (storage->segments[%u].memory + %u));\n", c_type, address->segment_selector, address->segment_offset, c_type, address->segment_selector, address->segment_offset);
			}	break;
			default: {
			}	break;
		}
	}
	qb_print(cxt, "\n");

	// create variables for writable arrays
	for(i = 0; i < cxt->writable_array_count; i++) {
		qb_address *address = cxt->writable_arrays[i];
		const char *c_type = type_cnames[address->type];
		qb_access_method method = qb_get_array_access_method(cxt, address);
		switch(method) {
			case QB_ARRAY_POINTER: {
				// create pointers to the value in the storage
				qb_printf(cxt, "%s *var_ptr_%u_%u = (%s *) (storage->segments[%u].memory + %u);\n", c_type, address->segment_selector, address->segment_offset, c_type, address->segment_selector, address->segment_offset);
			}	break;
			case QB_ARRAY_POINTER_POINTER: {
				// create pointers to pointers in the storage, as the realocation can occur for variable-length arrays
				qb_printf(cxt, "%s **var_ptr_ptr_%u = (%s **) &storage->segments[%u].memory;\n", c_type, address->segment_selector, c_type, address->segment_selector);
			}	break;
			default: {
			}	break;
		}
	}
	qb_print(cxt, "\n");

	// see if any constant scalar is referenced as arrays
	constant_as_pointer = do_alloca(cxt->constant_scalar_count * sizeof(int32_t), use_heap);
	memset(constant_as_pointer, 0, cxt->constant_scalar_count * sizeof(int32_t));
	for(i = 0; i < cxt->op_count; i++) {
		qb_op *op = cxt->ops[i];
		for(j = 0; j < op->operand_count; j++) {
			qb_operand *operand = &op->operands[j];
			if(operand->type == QB_OPERAND_ADDRESS) {
				if(operand->address->mode == QB_ADDRESS_MODE_ARR && operand->address->segment_selector == QB_SELECTOR_CONSTANT_SCALAR) {
					for(k = 0; k < cxt->constant_scalar_count; k++) {
						if(operand->address->segment_offset == cxt->constant_scalars[k]->segment_offset) {
							constant_as_pointer[k] = TRUE;
							break;
						}
					}
				}
			}
		}
	}
	for(i = 0; i < cxt->constant_scalar_count; i++) {
		if(constant_as_pointer[i]) {
			qb_address *address = cxt->constant_scalars[i];
			const char *c_type = type_cnames[address->type];
			qb_printf(cxt, "%s *var_ptr_%u_%u = (%s *) (storage->segments[%u].memory + %u);\n", c_type, address->segment_selector, address->segment_offset, c_type, address->segment_selector, address->segment_offset);
		}
	}
	free_alloca(constant_as_pointer, use_heap);

	// create variables for constant arrays
	for(i = 0; i < cxt->constant_array_count; i++) {
		qb_address *address = cxt->constant_arrays[i];
		const char *c_type = type_cnames[address->type];
		qb_printf(cxt, "%s *var_ptr_%u_%u = (%s *) (storage->segments[%u].memory + %u);\n", c_type, address->segment_selector, address->segment_offset, c_type, address->segment_selector, address->segment_offset);
	}
	qb_print(cxt, "\n");

#ifdef ZTS
	qb_print(cxt, STRING(USE_TSRM)"\n");
#endif
	qb_print(cxt, "\n");
}

static void qb_print_reentry_switch(qb_native_compiler_context *cxt) {
	int32_t started = FALSE;
	uint32_t i;
	for(i = 0; i < cxt->op_count; i++) {
		qb_op *qop = cxt->ops[i];
		if(qop->opcode == QB_END_STATIC || qop->opcode == QB_INTR || qop->opcode == QB_RESUME || qop->opcode == QB_SPOON) {
			const char *jump_target = qb_get_jump_label(cxt, i + 1);
			if(!started) {
				qb_print(cxt, "switch(cxt->instruction_pointer - cxt->function->instructions) {\n");
				started = TRUE;
			}
			qb_printf(cxt,	"case %u: goto %s;\n", qop->instruction_offset, jump_target);
		}
	}
	if(started) {
		qb_print(cxt, "}\n");
		qb_print(cxt, "\n");
	}
}

static void qb_print_function(qb_native_compiler_context *cxt) {
	uint32_t i;
	if(cxt->print_source) {
		qb_printf(cxt, "\n// Handler for %s()\n", cxt->compiled_function->name);
	}
	qb_printf(cxt, STRING(QB_NATIVE_FUNCTION_RET QB_NATIVE_FUNCTION_ATTR) " QBN_%" PRIX64 "(" STRING(QB_NATIVE_FUNCTION_ARGS) ")\n{\n", cxt->compiled_function->instruction_crc64);
	qb_print_local_variables(cxt);
	qb_print_reentry_switch(cxt);
	for(i = 0; i < cxt->op_count; i++) {
		qb_print_op(cxt, cxt->ops[i], i);
	}
	qb_print(cxt, "}\n");
}

static void qb_print_version(qb_native_compiler_context *cxt) {
	qb_printf(cxt, "\nuint32_t QB_VERSION = 0x%08x;\n\n", QB_VERSION_SIGNATURE);
}

static void qb_print_functions(qb_native_compiler_context *cxt) {
	uint32_t i, j;
	for(i = 0; i < cxt->compiler_context_count; i++) {
		qb_compiler_context *compiler_cxt = cxt->compiler_contexts[i];
		if(!compiler_cxt->compiled_function->native_proc && (compiler_cxt->function_flags & QB_FUNCTION_NATIVE_IF_POSSIBLE)) {
			// check if an earlier function has the same crc64
			int32_t duplicate = FALSE;
			for(j = 0; j < i; j++) {
				if(cxt->compiler_contexts[j]->compiled_function->instruction_crc64 == compiler_cxt->compiled_function->instruction_crc64) {
					duplicate = TRUE;
					break;
				}
			}
			if(!duplicate) {
				// copy variables from the compiler context to make life easier
				cxt->ops = compiler_cxt->ops;
				cxt->op_count = compiler_cxt->op_count;
				cxt->variables = compiler_cxt->variables;
				cxt->variable_count = compiler_cxt->variable_count;
				cxt->constant_scalars = compiler_cxt->constant_scalars;
				cxt->constant_scalar_count = compiler_cxt->constant_scalar_count;
				cxt->writable_scalars = compiler_cxt->writable_scalars;
				cxt->writable_scalar_count = compiler_cxt->writable_scalar_count;
				cxt->constant_arrays = compiler_cxt->constant_arrays;
				cxt->constant_array_count = compiler_cxt->constant_array_count;
				cxt->writable_arrays = compiler_cxt->writable_arrays;
				cxt->writable_array_count = compiler_cxt->writable_array_count;
				cxt->zero_address = compiler_cxt->zero_address;
				cxt->one_address = compiler_cxt->one_address;
				cxt->storage = compiler_cxt->storage;
				cxt->compiled_function = compiler_cxt->compiled_function;

				// print the function
				qb_print_function(cxt);
			}
		}
	}
}

static void qb_print_function_records(qb_native_compiler_context *cxt) {
	uint32_t i;
	qb_print(cxt, "\n");
	qb_print(cxt, "#ifdef ZEND_DEBUG\n");
	qb_print(cxt, "#define HAVE_NATIVE_PROC_RECORDS\n");
	qb_print(cxt, "qb_native_proc_record native_proc_records[] = {\n");
	for(i = 0; i < cxt->compiler_context_count; i++) {
		qb_compiler_context *compiler_cxt = cxt->compiler_contexts[i];
		if(!compiler_cxt->compiled_function->native_proc && (compiler_cxt->function_flags & QB_FUNCTION_NATIVE_IF_POSSIBLE)) {
			qb_printf(cxt, "	{ 0x%" PRIX64 "ULL, QBN_%" PRIX64 " },\n", compiler_cxt->compiled_function->instruction_crc64, compiler_cxt->compiled_function->instruction_crc64);
		}
	}
	qb_print(cxt, "};\n");
	qb_print(cxt, "#endif\n");
}

static void * qb_get_intrinsic_function_address(const char *name) {
	void *address = NULL;
#if defined(_MSC_VER)
#ifndef _WIN64
	if(strcmp(name, "_ftol2") == 0) {
		address = _ftol2;
	} else if(strcmp(name, "_allshr") == 0) {
		address = _allshr;
	} else if(strcmp(name, "_allshl") == 0) {
		address = _allshl;
	}
#endif
	if(!address) {
		// intrinsics employed by MSVC are located in the runtime DLL
		// since the QB DLL might be used compiled using the same version of MSVC
		// we'll look for the function dynamically
#ifdef ZEND_DEBUG
		static const char *dll_names[] = { "msvcr110.dll", "msvcr110d.dll", "msvcr100.dll", "msvcr100d.dll", "msvcr90.dll", "msvcr90d.dll" };
#else
		static const char *dll_names[] = { "msvcr110.dll", "msvcr100.dll", "msvcr90.dll" };
#endif
		static HMODULE c_lib_handle = 0;
		static const char *c_lib_dll_name = NULL;

		if(!c_lib_handle) {
			// see which of these DLL's is loaded
			uint32_t i;
			for(i = 0; i < sizeof(dll_names) / sizeof(dll_names[0]); i++) {
				c_lib_handle = GetModuleHandle(dll_names[i]);
				if(c_lib_handle) {
					c_lib_dll_name = dll_names[i];
					break;
				}
			}
		}
		if(c_lib_handle) {
			address = GetProcAddress(c_lib_handle, name);
		}
		if(!address) {
			// try loading a more recent version of the runtime
			static HMODULE newer_c_lib_handle = 0;
			if(!newer_c_lib_handle) {
				uint32_t i;
				for(i = 0; i < sizeof(dll_names) / sizeof(dll_names[0]); i++) {
					if(dll_names[i] != c_lib_dll_name) {
						newer_c_lib_handle = LoadLibrary(dll_names[i]);
						if(newer_c_lib_handle) {
							break;
						}
					}
				}
			}
			if(newer_c_lib_handle) {
				address = GetProcAddress(newer_c_lib_handle, name);
			}
		}
	}
#else
	#ifdef HAVE_SINCOS
	if(!address) {
		if(strcmp(name, "sincos") == 0) {
			address = sincos;
		} else if(strcmp(name, "sincosf") == 0) {
			address = sincosf;
		}
	}
	#endif
#endif
	return address;
}

static void * qb_get_symbol_address(qb_native_compiler_context *cxt, qb_native_symbol *symbol) {
	if(!symbol->address) {
		if(symbol->flags & QB_NATIVE_SYMBOL_INTRINSIC_FUNCTION) {
			symbol->address = qb_get_intrinsic_function_address(symbol->name);
		}
	}
	return symbol->address;
}

#ifdef _WIN64
static void * qb_find_symbol_pointer(qb_native_compiler_context *cxt, const char *name) {
	static void **pointer_table = NULL;
	uint32_t i, name_len = (uint32_t) strlen(name);
	long hash_value = zend_get_hash_value(name, name_len + 1);
	if(!pointer_table) {
		pointer_table = VirtualAlloc(NULL, sizeof(void *) * global_native_symbol_count, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	}
	for(i = 0; i < global_native_symbol_count; i++) {
		qb_native_symbol *symbol = &global_native_symbols[i];
		if(symbol->hash_value == hash_value) {
			if(strcmp(symbol->name, name) == 0) {
				void *address = qb_get_symbol_address(cxt, symbol);
				if(address) {
					pointer_table[i] = address;
					return &pointer_table[i];
				} else {
					break;
				}
			}
		}
	}
	return NULL;
}
#endif

static int32_t qb_check_symbol(qb_native_compiler_context *cxt, const char *name) {
	uint32_t i, name_len = (uint32_t) strlen(name);
	long hash_value = zend_get_hash_value(name, name_len + 1);
	for(i = 0; i < global_native_symbol_count; i++) {
		qb_native_symbol *symbol = &global_native_symbols[i];
		if(symbol->hash_value == hash_value) {
			if(strcmp(symbol->name, name) == 0) {
				return TRUE;
			}
		}
	}
	return FALSE;
}

static void * qb_find_symbol(qb_native_compiler_context *cxt, const char *name) {
	long hash_value;
	uint32_t i, name_len = (uint32_t) strlen(name);
	const char *proc_name = name;
#ifdef _WIN64
	if(memcmp(name, "__imp_", 6) == 0) {
		return qb_find_symbol_pointer(cxt, name + 6);
	}
#endif
	hash_value = zend_get_hash_value(name, name_len + 1);
	for(i = 0; i < global_native_symbol_count; i++) {
		qb_native_symbol *symbol = &global_native_symbols[i];
		if(symbol->hash_value == hash_value) {
			if(strcmp(symbol->name, name) == 0) {
				return qb_get_symbol_address(cxt, symbol);
			}
		}
	}
	return NULL;
}

static uint32_t qb_attach_symbol(qb_native_compiler_context *cxt, const char *symbol_name, const char *address) {
	uint32_t count = 0;
	if(strncmp(symbol_name, "QBN_", 4) == 0) {
		qb_native_proc proc = (qb_native_proc) address;
		uint64_t crc64 = strtoull(symbol_name + 4, NULL, 16);
		uint32_t i;
		for(i = 0; i < cxt->compiler_context_count; i++) {
			qb_compiler_context *compiler_cxt = cxt->compiler_contexts[i];
			if(compiler_cxt->compiled_function->instruction_crc64 == crc64) {
				compiler_cxt->compiled_function->native_proc = proc;
				count++;
			}
		}
	} 
	return count;
}

static void qb_detach_symbols(qb_native_compiler_context *cxt) {
	uint32_t i;
	for(i = 0; i < cxt->compiler_context_count; i++) {
		qb_compiler_context *compiler_cxt = cxt->compiler_contexts[i];
		compiler_cxt->compiled_function->native_proc = NULL;
	}
	cxt->qb_version = 0;
}

extern const char compressed_table_native_actions[];
extern const char compressed_table_native_result_size_possibilities[];
extern const char compressed_table_native_result_size_calculations[];
extern const char compressed_table_native_prototypes[];
extern const char compressed_table_native_references[];

#if ZEND_DEBUG
static void qb_link_debuggable_functions(qb_native_compiler_context *cxt) {
	uint32_t i, j;
	for(i = 0; i < cxt->compiler_context_count; i++) {
		qb_compiler_context *compiler_cxt = cxt->compiler_contexts[i];
		for(j = 0; j < native_proc_table_size; j++) {
			qb_native_proc_record *rec = &native_proc_table[j];
			if(compiler_cxt->compiled_function->instruction_crc64 == rec->crc64) {
				compiler_cxt->compiled_function->native_proc = rec->proc;
			}
		}
	}
}
#endif

int32_t qb_decompress_code(qb_native_compiler_context *cxt) {
	// decompress string tables used to generate the C source code
	if(!cxt->pool->op_actions) {
		qb_uncompress_table(compressed_table_native_actions, (void ***) &cxt->pool->op_actions, NULL, 0);
	}
	if(!cxt->pool->op_function_usages) {
		qb_uncompress_table(compressed_table_native_references, (void ***) &cxt->pool->op_function_usages, NULL, 0);
	}
	if(!cxt->pool->function_prototypes) {
		uint32_t count;
		qb_uncompress_table(compressed_table_native_prototypes, (void ***) &cxt->pool->function_prototypes, &count, 0);
#if ZEND_DEBUG
		if(count > PROTOTYPE_COUNT) {
			qb_debug_abort("not enough space for the number of possible prototypes");
		}
#endif
	}

	cxt->op_names = cxt->pool->op_names;
	cxt->op_actions = cxt->pool->op_actions;
	cxt->op_function_usages = cxt->pool->op_function_usages;
	cxt->function_prototypes = cxt->pool->function_prototypes;

	return (cxt->op_actions && cxt->op_function_usages && cxt->function_prototypes);
}

#if defined(__MACH__)
#include "qb_native_compiler_osx.c"
#elif defined(__ELF__)
#include "qb_native_compiler_linux.c"
#elif defined(_MSC_VER)
#include "qb_native_compiler_win32.c"
#endif

void qb_compile_to_native_code(qb_native_compiler_context *cxt) {
	USE_TSRM
	uint32_t i, attempt;
	int32_t success = FALSE;

#if ZEND_DEBUG
	if(native_proc_table) {
		// link the functions to code in qb_native_proc_debug.c instead of compiling them live
		qb_link_debuggable_functions(cxt);
		return;
	}
#endif

	// make sure the cache folder exists
	qb_create_cache_folder(cxt);

	// XOR the crc64 of the functions together and use it as the file id
	cxt->file_id = 0;
	for(i = 0; i < cxt->compiler_context_count; i++) {
		qb_compiler_context *compiler_cxt = cxt->compiler_contexts[i];
		qb_function *qfunc = compiler_cxt->compiled_function;
		if(!qfunc->native_proc && (compiler_cxt->function_flags & QB_FUNCTION_NATIVE_IF_POSSIBLE)) {
			cxt->file_id ^= qfunc->instruction_crc64;
		}
	}
	spprintf(&cxt->obj_file_path, 0, "%s%cQB%" PRIX64 ".o", cxt->cache_folder_path, PHP_DIR_SEPARATOR, cxt->file_id);

#if ZEND_DEBUG
	for(attempt = 2; attempt <= 2 && !success; attempt++) {
#else
	for(attempt = 1; attempt <= 2 && !success; attempt++) {
#endif

		// first, try to load a previously created object file
		if(attempt == 2) {
			if(!qb_decompress_code(cxt)) {
				php_error_docref0(NULL TSRMLS_CC, E_WARNING, "Unable to decompress C source code");
				break;
			}

			// launch compiler
			if(!qb_launch_compiler(cxt)) {
				php_error_docref0(NULL TSRMLS_CC, E_WARNING, "Unable to launch compiler");
				break;
			}

#if ZEND_DEBUG
			// exclude macros, type declaration, and prototypes so they don't conflict with
			// what's defined in the header files if we include generated code to debug it
			qb_print(cxt, "#ifndef ZEND_DEBUG\n");
#endif
			// print macros and type definitions
			qb_print_macros(cxt);
			qb_print_typedefs(cxt);

			// print prototypes of function referenced
			qb_print_prototypes(cxt);
#if ZEND_DEBUG
	qb_print(cxt, "#endif\n");
#endif
			// print the current QB version
			qb_print_version(cxt);

			// print code of the qb functions themselves
			qb_print_functions(cxt);

#if ZEND_DEBUG
			// print a table of the functions
			qb_print_function_records(cxt);
#endif
			// wait for compiler to finish and see if it emits any error messages
			if(!qb_wait_for_compiler_response(cxt)) {
				break;
			}
		}

		// load the object file produced by the compiler into memory
		if(qb_load_object_file(cxt)) {
			// parse the object file and find pointers to the compiled functions
			if(qb_parse_object_file(cxt)) {
				if(cxt->qb_version == QB_VERSION_SIGNATURE) {
					qb_native_code_bundle *bundle;
					if(!QB_G(native_code_bundles)) {
						qb_create_array((void **) &QB_G(native_code_bundles), &QB_G(native_code_bundle_count), sizeof(qb_native_code_bundle), 8);
					}
					bundle = qb_enlarge_array((void **) &QB_G(native_code_bundles), 1);
					bundle->memory = cxt->binary;
					bundle->size = cxt->binary_size;
					qb_lock_object_file(cxt);
					cxt->binary = NULL;
					success = TRUE;
				} else {
					qb_detach_symbols(cxt);
				}
			}
			if(!success) {
				qb_remove_object_file(cxt);
			}
		}
	} 
}

void qb_initialize_native_compiler_context(qb_native_compiler_context *cxt, qb_build_context *build_cxt TSRMLS_DC) {
	static int hashes_initialized = FALSE;
	if(!hashes_initialized) {
		uint32_t i;
		// calculate hash for faster lookup
		for(i = 0; i < global_native_symbol_count; i++) {
			qb_native_symbol *symbol = &global_native_symbols[i];
			symbol->hash_value = zend_hash_func(symbol->name, (uint32_t) strlen(symbol->name) + 1);
		}
		hashes_initialized = TRUE;
	}

	memset(cxt, 0, sizeof(qb_native_compiler_context));

	cxt->pool = build_cxt->pool;
	cxt->print_errors = QB_G(show_compiler_errors);
	cxt->print_source = QB_G(show_native_source);
	cxt->compiler_contexts = build_cxt->compiler_contexts;
	cxt->compiler_context_count = build_cxt->compiler_context_count;
	SAVE_TSRMLS

	cxt->cache_folder_path = QB_G(native_code_cache_path);
}

void qb_free_native_compiler_context(qb_native_compiler_context *cxt) {
	USE_TSRM
	if(cxt->write_stream) {
		fclose(cxt->write_stream);
	}
	if(cxt->read_stream) {
		fclose(cxt->read_stream);
	}
	if(cxt->error_stream) {
		fclose(cxt->error_stream);
	}
	if(cxt->cache_folder_path && cxt->cache_folder_path != QB_G(native_code_cache_path)) {
		efree(cxt->cache_folder_path);
	}
	if(cxt->obj_file_path) {
		efree(cxt->obj_file_path);
	}
	if(cxt->c_file_path) {
		efree(cxt->c_file_path);
	}

#ifdef __GNUC__
	if(cxt->binary) {
		munmap(cxt->binary, cxt->binary_size);
	}
#endif
#ifdef _MSC_VER
	if(cxt->binary) {
		UnmapViewOfFile(cxt->binary);
	}
	if(cxt->msc_process) {
		CloseHandle(cxt->msc_process);
	}
	if(cxt->msc_thread) {
		CloseHandle(cxt->msc_thread);
	}
#endif
}

#endif	// NATIVE_COMPILE_ENABLED
