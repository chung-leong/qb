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

typedef struct qb_op					qb_op;
typedef struct qb_address				qb_address;
typedef struct qb_on_demand_address		qb_on_demand_address;
typedef struct qb_variable				qb_variable;
typedef struct qb_function				qb_function;
typedef struct qb_memory_segment		qb_memory_segment;
typedef struct qb_storage				qb_storage;
typedef struct qb_index_alias_scheme	qb_index_alias_scheme;
typedef struct qb_external_symbol		qb_external_symbol;
typedef struct qb_native_code_bundle	qb_native_code_bundle;
typedef struct qb_block_allocator 		qb_block_allocator;
typedef struct qb_array_initializer		qb_array_initializer;
typedef struct qb_result_prototype		qb_result_prototype;
typedef struct qb_result_destination	qb_result_destination;

typedef struct qb_pointer_SCA			qb_pointer_SCA;
typedef struct qb_pointer_ELE			qb_pointer_ELE;
typedef struct qb_pointer_ARR			qb_pointer_ARR;
typedef struct qb_pointer_adjustment	qb_pointer_adjustment;

typedef struct qb_thread_parameters		qb_thread_parameters;

typedef struct qb_operand					qb_operand;

typedef enum qb_primitive_type			qb_primitive_type;
typedef enum qb_address_mode			qb_address_mode;
typedef enum qb_operand_type				qb_operand_type;
typedef enum qb_opcode						qb_opcode;
typedef enum qb_result_destination_type		qb_result_destination_type;

#define MAKE_STRING(x)						#x
#define STRING(x)							MAKE_STRING(x)

#define MAX_SEGMENT_COUNT					256
#define MAX_DIMENSION						64
#define MAX_THREAD_COUNT					32

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

#define CTYPE_INDEX							uint32_t

#define CTYPE(type)							CTYPE_##type

#define SCALAR(address)						(address->dimension_count == 0)
#define CONSTANT(address)					(address->flags & QB_ADDRESS_CONSTANT)
#define TEMPORARY(address)					(address->flags & QB_ADDRESS_TEMPORARY)
#define IN_USE(address)						(address->flags & QB_ADDRESS_IN_USE)
#define READ_ONLY(address)					(address->flags & QB_ADDRESS_READ_ONLY)

#define MULTIDIMENSIONAL_ARRAY(address)		(address->dimension_count > 1)
#define FIXED_LENGTH_ARRAY(address)			(address->dimension_count > 0 && CONSTANT(address->array_size_address))
#define VARIABLE_LENGTH_ARRAY(address)		(address->dimension_count > 0 && !CONSTANT(address->array_size_address))
#define EXPANDABLE_ARRAY(address)			(address->dimension_count > 0 && !READ_ONLY(address->array_size_address))

#define ARRAY_MEMBER(address)				(address->source_address && address->source_address->dimension_count > address->dimension_count)
#define CAST(address)						(address->source_address && address->source_address->dimension_count == address->dimension_count && address->type != address->source_address->type)

#define ARRAY_IN(storage, type, address)	((CTYPE(type) *) (storage->segments[address->segment_selector].memory + address->segment_offset))
#define VALUE_IN(storage, type, address)	*ARRAY_IN(storage, type, address)
#define ARRAY_SIZE_IN(storage, address)		VALUE_IN(storage, U32, address->array_size_address)

#define STORAGE_TYPE_MATCH(type1, type2)	((type1 == type2) || ((type1 & ~QB_TYPE_UNSIGNED) == (type2 & ~QB_TYPE_UNSIGNED) && (type1 < QB_TYPE_F32)))

#define ARRAY(type, address)				ARRAY_IN(cxt->storage, type, address)
#define VALUE(type, address)				VALUE_IN(cxt->storage, type, address)
#define ARRAY_SIZE(address)					VALUE(U32, address->array_size_address)

#define DIMENSION(address, i)				VALUE(U32, address->dimension_addresses[(i >= 0) ? i : address->dimension_count + i])

#define BYTE_COUNT(element_count, type)		((element_count) << type_size_shifts[type])
#define ELEMENT_COUNT(byte_count, type)		((byte_count) >> type_size_shifts[type])

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

	// the type used as array indices
	QB_TYPE_INDEX					= QB_TYPE_U32,

	// pseudo-types
	QB_TYPE_VOID					= 100,
	QB_TYPE_UNKNOWN,
	QB_TYPE_ANY,
};

enum qb_address_mode {
	QB_ADDRESS_MODE_SCA				= 0,
	QB_ADDRESS_MODE_ELE,
	QB_ADDRESS_MODE_ARR,

	QB_ADDRESS_MODE_UNKNOWN			= 100,
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
	QB_ADDRESS_ON_DEMAND_VALUE		= 0x02000000,

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
	uint32_t dimension_count;
	uint32_t segment_selector;
	uint32_t segment_offset;
	qb_address *array_index_address;
	qb_address *array_size_address;
	qb_address **dimension_addresses;
	qb_address **array_size_addresses;
	qb_address *source_address;
	qb_index_alias_scheme **index_alias_schemes;
};

struct qb_on_demand_address {
	qb_address_mode mode;
	qb_primitive_type type;
	uint32_t flags;
	uint32_t dimension_count;
	uint32_t segment_selector;
	uint32_t segment_offset;
	qb_address *operand_addresses[4];
	uint32_t operand_count;
	void *op_factory;
};

enum qb_operand_type {
	QB_OPERAND_NONE					= 0,
	QB_OPERAND_ADDRESS,
	QB_OPERAND_EXTERNAL_SYMBOL,
	QB_OPERAND_ARRAY_INITIALIZER,
	QB_OPERAND_ZEND_CLASS,
	QB_OPERAND_ZVAL,
	QB_OPERAND_GLOBAL_STATIC,
	QB_OPERAND_EMPTY,
	QB_OPERAND_RESULT_PROTOTYPE,
};

enum {
	QB_INSTRUCTION_OFFSET			= 0x40000000,
	QB_INSTRUCTION_NEXT 			= 1 | QB_INSTRUCTION_OFFSET,
	QB_INSTRUCTION_END				= 0xFFFFFFFF,
};

enum {
	QB_OP_INDEX_NONE				= 0xFFFFFFFF,
	QB_OP_INDEX_JUMP_TARGET			= 0xFFFFFFFE,
};

struct qb_operand {
	qb_operand_type type;
	union {
		qb_address *address;
		qb_function *function;
		uint32_t symbol_index;

		zval *constant;
		zend_class_entry *zend_class;
		qb_array_initializer *array_initializer;
		qb_result_prototype *result_prototype;
		void *generic_pointer;
	};
};

struct qb_array_initializer {
	qb_operand *elements;
	uint32_t element_count;
	qb_primitive_type desired_type;
};

struct qb_variable_dimensions {
	uint32_t dimension_count;
	uint32_t array_size;
	qb_address *dimension_addresses[64];
	qb_address *source_address;
};

enum qb_result_destination_type {
	QB_RESULT_DESTINATION_TEMPORARY	= 0,
	QB_RESULT_DESTINATION_VARIABLE,
	QB_RESULT_DESTINATION_ELEMENT,
	QB_RESULT_DESTINATION_PROPERTY,
	QB_RESULT_DESTINATION_ARGUMENT,
	QB_RESULT_DESTINATION_PRINT,
	QB_RESULT_DESTINATION_FREE,
};

struct qb_result_destination {
	qb_result_destination_type type;
	union {
		struct {
			qb_function *function;
			uint32_t index;
		} argument;
		struct {
			qb_operand container;
			qb_operand index;
		} element;
		struct {
			qb_operand container;
			qb_operand name;
		} property;
		qb_operand variable;
	};
	qb_result_prototype *prototype;
};

enum {
	// intrinsic properties of an op
	QB_OP_VARIABLE_LENGTH			= 0x8000,
	QB_OP_NEED_LINE_NUMBER			= 0x4000,
	QB_OP_BRANCH					= 0x3000,
	QB_OP_JUMP 						= 0x1000,
	QB_OP_SIDE_EFFECT				= 0x0800,
	QB_OP_PERFORM_WRAP_AROUND		= 0x0400,
	QB_OP_VERSION_AVAILABLE_ELE		= 0x0200,
	QB_OP_VERSION_AVAILABLE_MIO		= 0x0100,

	// compile time properties
	QB_OP_JUMP_TARGET 				= 0x80000000,
	QB_OP_CANNOT_REMOVE				= 0x40000000,
	QB_OP_COMPILE_TIME_FLAGS		= 0xFFFF0000,
};

struct qb_op {
	uint32_t flags;
	qb_opcode opcode;
	uint32_t operand_count;
	qb_operand *operands;
	uint32_t jump_target_count;
	uint32_t *jump_target_indices;
	uint32_t instruction_offset;
	uint32_t line_number;
};

struct qb_pointer_SCA {
	void *data_pointer;
};

struct qb_pointer_ELE {
	void *data_pointer;
	uint32_t *index_pointer;
};

struct qb_pointer_ARR {
	void *data_pointer;
	uint32_t *index_pointer;
	uint32_t *count_pointer;
};

struct qb_pointer_adjustment {
	uint32_t index;
	uint32_t count;
};

struct qb_thread_parameters {
	void *pointer1;
	void *pointer2;
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
	QB_VARIABLE_SHARED				= 0x00000080,
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
	QB_SEGMENT_SHARED				= 0x00000010,
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
};

struct qb_storage {
	qb_memory_segment *segments;
	uint32_t segment_count;
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
	uint32_t required_argument_count;
	qb_external_symbol **external_symbols;
	uint32_t external_symbol_count;
	qb_storage *local_storage;
	const char *name;
	const char *filename;
	void *native_proc;

	zend_function *zend_function;
	zend_arg_info *zend_arg_info;
	qb_function *next_copy;
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

float qb_fast_rsqrtf(float a);
double qb_fast_rsqrt(double a);
float qb_fast_sqrtf(float a);
double qb_fast_sqrt(double a);

uint64_t ZEND_FASTCALL qb_calculate_crc64(const uint8_t *buf, size_t size, uint64_t crc);
double ZEND_FASTCALL qb_get_high_res_timestamp(void);

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
