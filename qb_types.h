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

typedef struct qb_block_allocator 			qb_block_allocator;
typedef struct qb_intrinsic_function		qb_intrinsic_function;
typedef struct qb_data_pool					qb_data_pool;

typedef struct qb_pointer_SCA				qb_pointer_SCA;
typedef struct qb_pointer_ELE				qb_pointer_ELE;
typedef struct qb_pointer_ARR				qb_pointer_ARR;
typedef struct qb_pointer_adjustment		qb_pointer_adjustment;

typedef struct qb_thread_parameters			qb_thread_parameters;

typedef enum qb_primitive_type				qb_primitive_type;

#define MAKE_STRING(x)						#x
#define STRING(x)							MAKE_STRING(x)

#define MAX_DIMENSION						8
#define MAX_THREAD_COUNT					256

#define CTYPE_I08							int8_t
#define CTYPE_I16							int16_t
#define CTYPE_I32							int32_t
#define CTYPE_I64							int64_t
#define CTYPE_F32							float32_t
#define CTYPE_F64							float64_t
#define CTYPE_S08							int8_t
#define CTYPE_S16							int16_t
#define CTYPE_S32							int32_t
#define CTYPE_S64							int64_t
#define CTYPE_U08							uint8_t
#define CTYPE_U16							uint16_t
#define CTYPE_U32							uint32_t
#define CTYPE_U64							uint64_t

#define CTYPE(type)							CTYPE_##type

#define STORAGE_TYPE_MATCH(type1, type2)	((type1 == type2) || ((type1 & ~QB_TYPE_UNSIGNED) == (type2 & ~QB_TYPE_UNSIGNED) && (type1 < QB_TYPE_F32)))

#define BYTE_COUNT(element_count, type)		((element_count) << type_size_shifts[type])
#define ELEMENT_COUNT(byte_count, type)		((byte_count) >> type_size_shifts[type])

#define SHIFT_POINTER(p, shift)				*((uintptr_t *) &(p)) += shift

#define INVALID_INDEX						((uint32_t) -1)

enum qb_primitive_type {
	QB_TYPE_I08						= 0,
	QB_TYPE_I16						= 2,
	QB_TYPE_I32						= 4,
	QB_TYPE_I64						= 6,

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

	QB_TYPE_UNSIGNED				= 0x0001,

	QB_TYPE_COUNT					= 10,

	// pseudo-types
	QB_TYPE_VOID					= 100,
	QB_TYPE_UNKNOWN,
	QB_TYPE_ANY,
};

struct qb_intrinsic_function {
	ulong hash_value;
	const char *name;
	uint32_t argument_count_min;
	uint32_t argument_count_max;
	void *extra;
};

struct qb_pointer_adjustment {
	uint32_t index;
	uint32_t count;
};

struct qb_thread_parameters {
	void *pointer1;
	void *pointer2;
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

int64_t qb_zval_to_long(zval *zvalue);
double qb_zval_to_double(zval *zvalue);
int64_t qb_zval_array_to_int64(zval *zvalue);

zval * qb_cstring_to_zval(const char *s TSRMLS_DC);
zval * qb_string_to_zval(const char *s, uint32_t len TSRMLS_DC);
zval * qb_cstring_to_zend_literal(const char *s TSRMLS_DC);
zval * qb_string_to_zend_literal(const char *s, uint32_t len TSRMLS_DC);

uint32_t qb_element_to_string(char *buffer, uint32_t buffer_len, int8_t *bytes, uint32_t type);

#define __qb_clamp_float32(f, max)				qb_clamp_float32_0_##max(f)
#define qb_clamp_float32(f, max)				__qb_clamp_float32(f, max)
#define __qb_clamp_float64(f, max)				qb_clamp_float64_0_##max(f)
#define qb_clamp_float64(f, max)				__qb_clamp_float64(f, max)

#if defined(__i386__)
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
	n = ((int32_t *) &f)[1];
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
	n = ((int32_t *) &f)[1];
	sig = n >> 13;
	mask1 = -(n >= 0x40000000);
	mask2 = -(n >= 0x3FF00000) & 0x7F;
	return (sig | mask1) & mask2;
#endif
}

int32_t qb_uncompress_table(const char *data, void ***p_table, uint32_t *p_item_count, int32_t persistent);

struct qb_block_allocator {
	uint32_t count;
	uint32_t capacity;
	uint32_t item_size;
	qb_block_allocator *previous_block;
	char *top;
	char data[4];
};

struct qb_data_pool {
	void ***arrays;
	uint32_t array_count;

	qb_block_allocator *op_allocator;
	qb_block_allocator *address_allocator;
	qb_block_allocator *expression_allocator;
	qb_block_allocator *pointer_allocator;
	qb_block_allocator *operand_allocator;
	qb_block_allocator *index_alias_scheme_allocator;
	qb_block_allocator *string_allocator;
	qb_block_allocator *uint32_allocator;
	qb_block_allocator *type_declaration_allocator;
	qb_block_allocator *variable_allocator;
	qb_block_allocator *function_declaration_allocator;
	qb_block_allocator *class_declaration_allocator;
	qb_block_allocator *array_initializer_allocator;
	qb_block_allocator *result_destination_allocator;

	char * const *op_names;
	char * const *op_actions;
	int32_t * const *op_function_usages;
	char * const *function_prototypes;
	char * const *zend_op_names;
	char * const *pbj_op_names;
};

void qb_create_block_allocator(qb_block_allocator **p_allocator, uint32_t item_size, uint32_t capacity);
void * qb_allocate_items(qb_block_allocator **p_allocator, uint32_t count);
void qb_destroy_block_allocator(qb_block_allocator **p_allocator);

void qb_create_array(void **p_array, uint32_t *p_count, uint32_t item_size, uint32_t increment);
void * qb_enlarge_array(void **p_array, uint32_t count);
void qb_destroy_array(void **p_array);

float qb_fast_rsqrtf(float a);
double qb_fast_rsqrt(double a);
float qb_fast_sqrtf(float a);
double qb_fast_sqrt(double a);

uint64_t qb_calculate_crc64(const uint8_t *buf, size_t size, uint64_t crc);
double qb_get_high_res_timestamp(void);

void qb_initialize_data_pool(qb_data_pool *pool);
void qb_free_data_pool(qb_data_pool *pool);

// Copyright (c) 2008-2010 Bjoern Hoehrmann <bjoern@hoehrmann.de>
// See http://bjoern.hoehrmann.de/utf-8/decoder/dfa/ for details.

#define UTF8_ACCEPT 0
#define UTF8_REJECT 12

extern const uint8_t utf8d[];

static uint32_t inline
decode(uint32_t* state, uint32_t* codep, uint32_t byte) {
  uint32_t type = utf8d[byte];

  *codep = (*state != UTF8_ACCEPT) ?
    (byte & 0x3fu) | (*codep << 6) :
    (0xff >> type) & (byte);

  *state = utf8d[256 + *state + type];
  return *state;
}

static uint32_t inline
encode(uint32_t c, uint8_t *bytes) {
	if (c < 0x80) {
		*bytes = c;
		return 1;
	} else {
		int32_t i;
		uint32_t len = c < 0x800 ? 1 : c < 0x10000 ? 2 : 3;
		*bytes++ = (-1 << (7 - len)) | (c >> len * 6);
		for (i = len; i > 0; --i, c >>= 6) {
			*bytes++ = ((c & 0x3f) | 0x80);
		}
		return len + 1;
	}
}

#endif
