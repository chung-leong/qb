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

#ifndef QB_TYPES_H_
#define QB_TYPES_H_

typedef float  float32_t;
typedef double float64_t;

typedef struct qb_complex_F64			qb_complex_F64;
typedef struct qb_complex_F32			qb_complex_F32;

typedef struct qb_address				qb_address;
typedef struct qb_variable				qb_variable;
typedef struct qb_function				qb_function;
typedef struct qb_memory_segment		qb_memory_segment;
typedef struct qb_storage				qb_storage;
typedef struct qb_index_alias_scheme	qb_index_alias_scheme;
typedef struct qb_external_symbol		qb_external_symbol;
typedef struct qb_native_code_bundle	qb_native_code_bundle;
typedef struct qb_block_allocator 		qb_block_allocator;

typedef enum qb_primitive_type			qb_primitive_type;
typedef enum qb_address_mode			qb_address_mode;

#define MAKE_STRING(x)					#x
#define STRING(x)						MAKE_STRING(x)

#define MAX_SEGMENT_COUNT			256
#define MAX_DIMENSION				64

#define CTYPE_I08					int8_t
#define CTYPE_I16					int16_t
#define CTYPE_I32					int32_t
#define CTYPE_I64					int64_t
#define CTYPE_F32					float32_t
#define CTYPE_F64					float64_t
#define CTYPE_S08					int8_t
#define CTYPE_S16					int16_t
#define CTYPE_S32					int32_t
#define CTYPE_S64					int64_t
#define CTYPE_U08					uint8_t
#define CTYPE_U16					uint16_t
#define CTYPE_U32					uint32_t
#define CTYPE_U64					uint64_t

#define CTYPE(type)					CTYPE_##type

enum qb_primitive_type {
	QB_TYPE_I08						= 0,
	QB_TYPE_I16						= 2,
	QB_TYPE_I32						= 4,
	QB_TYPE_I64						= 6,
	QB_TYPE_UNSIGNED				= 0x0001,

	QB_TYPE_S08						= 0,
	QB_TYPE_U08						= 1,
	QB_TYPE_S16						= 2,
	QB_TYPE_U16						= 3,
	QB_TYPE_S32						= 4,
	QB_TYPE_U32						= 5,
	QB_TYPE_S64						= 6,
	QB_TYPE_U64						= 7,

	QB_TYPE_F32						= 8,
	QB_TYPE_F64						= 9,

	QB_TYPE_COUNT					= 10,

	// pseudo-types
	QB_TYPE_VOID					= 100,
	QB_TYPE_UNKNOWN,
	QB_TYPE_ANY,
	QB_TYPE_OPERAND,
};

enum qb_address_mode {
	QB_ADDRESS_MODE_VAR				= 0,
	QB_ADDRESS_MODE_ELC,
	QB_ADDRESS_MODE_ELV,
	QB_ADDRESS_MODE_ARR,
};

enum {
	QB_ADDRESS_READ_ONLY			= 0x00000001,
	QB_ADDRESS_CONSTANT				= 0x00000002,
	QB_ADDRESS_STRING				= 0x00000004,
	QB_ADDRESS_BOOLEAN				= 0x00000008,
	QB_ADDRESS_SEGMENT				= 0x00000010,
	QB_ADDRESS_TEMPORARY			= 0x00010000,
	QB_ADDRESS_REUSED				= 0x00040000,
	QB_ADDRESS_INITIALIZED			= 0x00080000,
	QB_ADDRESS_ALWAYS_IN_BOUND		= 0x00100000,
	QB_ADDRESS_AUTO_INCREMENT		= 0x00200000,
	QB_ADDRESS_CAST					= 0x00400000,
	QB_ADDRESS_NON_LOCAL			= 0x00800000,
	QB_ADDRESS_FOREACH_INDEX		= 0x01000000,
	QB_ADDRESS_IN_USE				= 0x80000000,


	QB_ADDRESS_RUNTIME_FLAGS		= 0x0000FFFF,
	QB_ADDRESS_COMPILE_TIME_FLAGS	= 0xFFFF0000,
};

enum {
	// scalar constant and scalar variables are stored in segment 0
	QB_SELECTOR_VARIABLE 			= 0,
	// constant arrays are stored in segment 1 (to not impact negatively locality of variables)
	QB_SELECTOR_CONSTANT_ARRAY 		= 1,
	// larger array variables are stored in individual segments (so an offset doesn't need to be added to element indices)
	QB_SELECTOR_DYNAMIC_ARRAY_START	= 2,

	QB_SELECTOR_INVALID 			= 0xFFFF,
};

enum {
	QB_OFFSET_INVALID 				= 0xFFFFFFFF,
};

struct qb_index_alias_scheme {
	uint32_t dimension;
	char **aliases;
	uint32_t *alias_lengths;
	const char *class_name;
	uint32_t class_name_length;
	zend_class_entry *zend_class;
};

struct qb_address {
	qb_address_mode mode;
	qb_primitive_type type;
	uint32_t flags;
	uint32_t segment_selector;
	uint32_t segment_offset;
	qb_address *array_index_address;
	qb_address *array_size_address;
	uint32_t dimension_count;
	qb_address **dimension_addresses;
	qb_address **array_size_addresses;
	qb_address *source_address;
	qb_index_alias_scheme **index_alias_schemes;
};

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
	QB_VARIABLE_RETURN_VALUE		= 0x00000040,
	QB_VARIABLE_TYPES				= 0x0000FFFF,

	QB_VARIABLE_PASSED_BY_REF		= 0x00010000,
};

struct qb_variable {
	uint32_t flags;
	qb_address *address;
	qb_address *default_value_address;
	const char *name;
	uint32_t name_length;
	ulong hash_value;
	zend_class_entry *zend_class;
};

enum {
	QB_STORAGE_IN_USE				= 0x0001,
	QB_STORAGE_STATIC_INITIALIZED	= 0x0002,
};

enum {
	QB_SEGMENT_STATIC				= 0x00000001,
	QB_SEGMENT_EXPANDABLE			= 0x00000002,
	QB_SEGMENT_PREALLOCATED			= 0x00000004,
	QB_SEGMENT_TEMPORARY			= 0x00000008,
	QB_SEGMENT_BORROWED				= 0x00000100,
	QB_SEGMENT_MAPPED				= 0x00000200,
};

struct qb_memory_segment {
	int8_t *memory;
	uint32_t flags;
	uint32_t type;								// type of elements contained in this segment
	uint32_t element_count;						// number of elements in this segment
	uint32_t current_allocation;				// number of elements allocated

	uint32_t *array_size_pointer;				// pointer to array size variable accessible to user code (i.e. it points to a place in the first member of this struct)
	uint32_t *dimension_pointer;				// pointer to the first array dimension, which would be different from array size if the array is multidimensional
	uint32_t *increment_pointer;				// pointer to the size of each sub-array, populated only if array is multidimensional

	php_stream *stream;							// memory-mapped file

	int8_t *__restrict *stack_ref_memory;			// pointers to local variables in qb_run()
	uint32_t *stack_ref_element_count;
};

struct qb_storage {
	qb_memory_segment *segments;
	uint32_t segment_count;
	uint32_t size;
	uint32_t flags;
};

enum {
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
	uint32_t instruction_length;
	uint64_t instruction_crc64;
	uint32_t flags;
	qb_variable *return_variable;
	qb_variable **variables;
	uint32_t variable_count;
	uint32_t argument_count;
	uint32_t required_argument_count;
	qb_external_symbol **external_symbols;
	uint32_t external_symbol_count;
	qb_storage *local_storage;
	const char *name;
	const char *filename;
	void *native_proc;

	zend_function *zend_function;
};

#ifdef _MSC_VER
	#define SWAP_BE_I16(v)		_byteswap_ushort(v)
	#define SWAP_BE_I32(v)		_byteswap_ulong(v)
	#define SWAP_BE_I64(v)		_byteswap_uint64(v)
	#define SWAP_LE_I16(v)		v
	#define SWAP_LE_I32(v)		v
	#define SWAP_LE_I64(v)		v
#endif

#ifdef __GNUC__
	#ifdef QB_LITTLE_ENDIAN
		#define SWAP_BE_I16(v)	__builtin_bswap16(v)
		#define SWAP_BE_I32(v)	__builtin_bswap32(v)
		#define SWAP_BE_I64(v)	__builtin_bswap64(v)
		#define SWAP_LE_I16(v)	v
		#define SWAP_LE_I32(v)	v
		#define SWAP_LE_I64(v)	v
	#else
		#define SWAP_BE_I16(v)	v
		#define SWAP_BE_I32(v)	v
		#define SWAP_BE_I64(v)	v
		#define SWAP_LE_I16(v)	__builtin_bswap16(v)
		#define SWAP_LE_I32(v)	__builtin_bswap32(v)
		#define SWAP_LE_I64(v)	__builtin_bswap64(v)
	#endif
#endif

#define ALIGN_TO(v, count)		(((v) + (count) - 1) & ~((count) - 1))

#define ZERO_IF_FALSE(v, cond)		(v & ~(-(cond)))

extern uint32_t type_sizes[];
extern uint32_t type_size_shifts[];
extern const char *type_names[];
extern const char *type_cnames[];
extern int64_t integer_lower_bounds[];
extern uint64_t integer_upper_bounds[];

void ZEND_FASTCALL qb_copy_wrap_around(int8_t *memory, uint32_t filled_byte_count, uint32_t required_byte_count);
void ZEND_FASTCALL qb_copy_elements(uint32_t source_type, int8_t *restrict source_memory, uint32_t source_count, uint32_t dest_type, int8_t *restrict dest_memory, uint32_t dest_count);

static zend_always_inline void qb_copy_element(uint32_t source_type, int8_t *restrict source_memory, uint32_t dest_type, int8_t *restrict dest_memory) {
	qb_copy_elements(source_type, source_memory, 1, dest_type, dest_memory, 1);
}

PHP_FUNCTION(qb_execute);

static zend_always_inline int32_t qb_is_compiled_function(zend_function *zfunc) {
	return (zfunc->type == ZEND_INTERNAL_FUNCTION && zfunc->internal_function.handler == PHP_FN(qb_execute));
}

int64_t ZEND_FASTCALL qb_zval_to_long(zval *zvalue);
double ZEND_FASTCALL qb_zval_to_double(zval *zvalue);
int64_t ZEND_FASTCALL qb_zval_array_to_int64(zval *zvalue);

zval * ZEND_FASTCALL qb_cstring_to_zval(const char *s TSRMLS_DC);
zval * ZEND_FASTCALL qb_string_to_zval(const char *s, uint32_t len TSRMLS_DC);

uint32_t ZEND_FASTCALL qb_element_to_string(char *buffer, uint32_t buffer_len, int8_t *bytes, uint32_t type);

#define __qb_clamp_float32(f, max)				qb_clamp_float32_0_##max(f)
#define qb_clamp_float32(f, max)				__qb_clamp_float32(f, max)
#define __qb_clamp_float64(f, max)				qb_clamp_float64_0_##max(f)
#define qb_clamp_float64(f, max)				__qb_clamp_float64(f, max)

#ifdef QB_LITTLE_ENDIAN
	#define FP_EXPONENT_INDEX	1
	#define FP_MANTISSA_INDEX	0
#else
	#define FP_EXPONENT_INDEX	0
	#define FP_MANTISSA_INDEX	1
#endif

#if defined(__i386__) || defined(_MSC_VER)
	#ifdef __SSE2__
		#define FAST_FLOAT_TO_INT
	#endif
#else
	#define FAST_FLOAT_TO_INT
#endif

static zend_always_inline int32_t qb_clamp_float32_0_255(float32_t f) {
#ifdef FAST_FLOAT_TO_INT
	int32_t n = (int32_t) (f * 255);
	if(UNEXPECTED((uint32_t) n > 255)) {
		if(n < 0) {
			n = 0;
		} else {
			n = 255;
		}
	}
	return n;
#else
	int32_t n, sig, mask1, mask2;
	f++;
	n = ((int32_t *) &f)[0];
	sig = n >> 15;
	mask1 = -(n >= 0x40000000);
	mask2 = -(n >= 0x3F800000) & 0xFF;
	return (sig | mask1) & mask2;
#endif
}

static zend_always_inline int32_t qb_clamp_float64_0_255(float64_t f) {
#ifdef FAST_FLOAT_TO_INT
	int32_t n = (int32_t) (f * 255);
	if(UNEXPECTED((uint32_t) n > 255)) {
		if(n < 0) {
			n = 0;
		} else {
			n = 255;
		}
	}
	return n;
#else
	int32_t n, sig, mask1, mask2;
	f++;
	n = ((int32_t *) &f)[FP_EXPONENT_INDEX];
	sig = n >> 12;
	mask1 = -(n >= 0x40000000);
	mask2 = -(n >= 0x3FF00000) & 0xFF;
	return (sig | mask1) & mask2;
#endif
}

static zend_always_inline int32_t qb_clamp_float32_0_127(float32_t f) {
#ifdef FAST_FLOAT_TO_INT
	int32_t n = (int32_t) (f * 127);
	if(UNEXPECTED((uint32_t) n > 127)) {
		if(n < 0) {
			n = 0;
		} else {
			n = 127;
		}
	}
	return n;
#else
	int32_t n, sig, mask1, mask2;
	f++;
	n = ((int32_t *) &f)[0];
	sig = n >> 16;
	mask1 = -(n >= 0x40000000);
	mask2 = -(n >= 0x3F800000) & 0x7F;
	return (sig | mask1) & mask2;
#endif
}

static zend_always_inline int32_t qb_clamp_float64_0_127(float64_t f) {
#ifdef FAST_FLOAT_TO_INT
	int32_t n = (int32_t) (f * 127);
	if(UNEXPECTED((uint32_t) n > 127)) {
		if(n < 0) {
			n = 0;
		} else {
			n = 127;
		}
	}
	return n;
#else
	int32_t n, sig, mask1, mask2;
	f++;
	n = ((int32_t *) &f)[FP_EXPONENT_INDEX];
	sig = n >> 13;
	mask1 = -(n >= 0x40000000);
	mask2 = -(n >= 0x3FF00000) & 0x7F;
	return (sig | mask1) & mask2;
#endif
}

static zend_always_inline int32_t qb_quick_floor(double f) {
#ifdef FAST_FLOAT_TO_INT
	return (int32_t) f;
#else
	int32_t n;
	if(f < INT16_MIN) {
		return INT16_MIN;
	} else if(f > INT16_MAX) {
		return INT16_MAX;
	}
	f += 103079215104.0;
	n = ((long *) &f)[FP_MANTISSA_INDEX];
	return n >> 16;
#endif
}

static zend_always_inline int32_t qb_quick_round(double f) {
#ifdef FAST_FLOAT_TO_INT
	return (int32_t) round(f);
#else
	int32_t n;
	if(f < INT16_MIN) {
		return INT16_MIN;
	} else if(f > INT16_MAX) {
		return INT16_MAX;
	}
	f += 103079215104.5;
	n = ((long *) &f)[FP_MANTISSA_INDEX];
	return n >> 16;
#endif
}

static zend_always_inline int32_t qb_quick_floorf(float f) {
#ifdef FAST_FLOAT_TO_INT
	return (int32_t) f;
#else
	return qb_quick_floor(f);
#endif
}

static zend_always_inline int32_t qb_quick_roundf(float f) {
#ifdef FAST_FLOAT_TO_INT
	return (int32_t) roundf(f);
#else
	return qb_quick_round(f);
#endif
}

#pragma pack(push,1)

struct qb_complex_F32 {
	float32_t r;
	float32_t i;
};

struct qb_complex_F64 {
	float64_t r;
	float64_t i;
};

#pragma pack(pop)

int32_t ZEND_FASTCALL qb_uncompress_table(const char *data, void ***p_table, uint32_t *p_item_count, int32_t persistent);

struct qb_block_allocator {
	uint32_t count;
	uint32_t capacity;
	uint32_t item_size;
	qb_block_allocator *previous_block;
	char *top;
	char data[4];
};

struct qb_native_code_bundle {
	void *memory;
	uint32_t size;
};

void ZEND_FASTCALL qb_create_block_allocator(qb_block_allocator **p_allocator, uint32_t item_size, uint32_t capacity);
void * ZEND_FASTCALL qb_allocate_items(qb_block_allocator **p_allocator, uint32_t count);
void ZEND_FASTCALL qb_destroy_block_allocator(qb_block_allocator **p_allocator);

void ZEND_FASTCALL qb_create_array(void **p_array, uint32_t *p_count, uint32_t item_size, uint32_t increment);
void * ZEND_FASTCALL qb_enlarge_array(void **p_array, uint32_t count);
void ZEND_FASTCALL qb_destroy_array(void **p_array);

uint64_t ZEND_FASTCALL qb_calculate_crc64(const uint8_t *buf, size_t size, uint64_t crc);
double ZEND_FASTCALL qb_get_high_res_timestamp(void);

#endif
