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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "qb.h"

#include "qb_crc64.c"

void ZEND_FASTCALL qb_copy_wrap_around(int8_t *memory, uint32_t filled_byte_count, uint32_t required_byte_count) {
	if(filled_byte_count) {
		while(filled_byte_count < required_byte_count) {
			uint32_t gap = (required_byte_count - filled_byte_count);
			uint32_t copy_count = (gap > filled_byte_count) ? filled_byte_count : gap;
			memcpy(memory + filled_byte_count, memory, copy_count);
			filled_byte_count += copy_count;
		}
	} else {
		memset(memory, 0, required_byte_count);
	}
}

void ZEND_FASTCALL qb_copy_elements(uint32_t source_type, int8_t *restrict source_memory, uint32_t source_count, uint32_t dest_type, int8_t *restrict dest_memory, uint32_t dest_count) {
	uint32_t i, count = min(source_count, dest_count);
	switch(dest_type) {
		case QB_TYPE_S08: {
			switch(source_type) {
				case QB_TYPE_S08:
				case QB_TYPE_U08: for(i = 0; i < count; i++) ((CTYPE(S08) *) dest_memory)[i] = (CTYPE(S08)) ((CTYPE(S08) *) source_memory)[i]; break;
				case QB_TYPE_S16:
				case QB_TYPE_U16: for(i = 0; i < count; i++) ((CTYPE(S08) *) dest_memory)[i] = (CTYPE(S08)) ((CTYPE(S16) *) source_memory)[i]; break;
				case QB_TYPE_S32:
				case QB_TYPE_U32: for(i = 0; i < count; i++) ((CTYPE(S08) *) dest_memory)[i] = (CTYPE(S08)) ((CTYPE(S32) *) source_memory)[i]; break;
				case QB_TYPE_S64:
				case QB_TYPE_U64: for(i = 0; i < count; i++) ((CTYPE(S08) *) dest_memory)[i] = (CTYPE(S08)) ((CTYPE(S64) *) source_memory)[i]; break;
				case QB_TYPE_F32: for(i = 0; i < count; i++) ((CTYPE(S08) *) dest_memory)[i] = (CTYPE(S08)) (CTYPE(S64)) ((CTYPE(F32) *) source_memory)[i]; break;
				case QB_TYPE_F64: for(i = 0; i < count; i++) ((CTYPE(S08) *) dest_memory)[i] = (CTYPE(S08)) (CTYPE(S64)) ((CTYPE(F64) *) source_memory)[i]; break;
			}
		}	break;
		case QB_TYPE_U08: {
			switch(source_type) {
				case QB_TYPE_S08:
				case QB_TYPE_U08: for(i = 0; i < count; i++) ((CTYPE(U08) *) dest_memory)[i] = (CTYPE(U08)) ((CTYPE(U08) *) source_memory)[i]; break;
				case QB_TYPE_S16:
				case QB_TYPE_U16: for(i = 0; i < count; i++) ((CTYPE(U08) *) dest_memory)[i] = (CTYPE(U08)) ((CTYPE(S16) *) source_memory)[i]; break;
				case QB_TYPE_S32:
				case QB_TYPE_U32: for(i = 0; i < count; i++) ((CTYPE(U08) *) dest_memory)[i] = (CTYPE(U08)) ((CTYPE(S32) *) source_memory)[i]; break;
				case QB_TYPE_S64:
				case QB_TYPE_U64: for(i = 0; i < count; i++) ((CTYPE(U08) *) dest_memory)[i] = (CTYPE(U08)) ((CTYPE(S64) *) source_memory)[i]; break;
				case QB_TYPE_F32: for(i = 0; i < count; i++) ((CTYPE(U08) *) dest_memory)[i] = (CTYPE(U08)) (CTYPE(S64)) ((CTYPE(F32) *) source_memory)[i]; break;
				case QB_TYPE_F64: for(i = 0; i < count; i++) ((CTYPE(U08) *) dest_memory)[i] = (CTYPE(U08)) (CTYPE(S64)) ((CTYPE(F64) *) source_memory)[i]; break;
			}
		}	break;
		case QB_TYPE_S16: {
			switch(source_type) {
				case QB_TYPE_S08: for(i = 0; i < count; i++) ((CTYPE(S16) *) dest_memory)[i] = (CTYPE(S16)) ((CTYPE(S08) *) source_memory)[i]; break;
				case QB_TYPE_U08: for(i = 0; i < count; i++) ((CTYPE(S16) *) dest_memory)[i] = (CTYPE(S16)) ((CTYPE(U08) *) source_memory)[i]; break;
				case QB_TYPE_S16:
				case QB_TYPE_U16: for(i = 0; i < count; i++) ((CTYPE(S16) *) dest_memory)[i] = (CTYPE(S16)) ((CTYPE(S16) *) source_memory)[i]; break;
				case QB_TYPE_S32:
				case QB_TYPE_U32: for(i = 0; i < count; i++) ((CTYPE(S16) *) dest_memory)[i] = (CTYPE(S16)) ((CTYPE(S32) *) source_memory)[i]; break;
				case QB_TYPE_S64:
				case QB_TYPE_U64: for(i = 0; i < count; i++) ((CTYPE(S16) *) dest_memory)[i] = (CTYPE(S16)) ((CTYPE(S64) *) source_memory)[i]; break;
				case QB_TYPE_F32: for(i = 0; i < count; i++) ((CTYPE(S16) *) dest_memory)[i] = (CTYPE(S16)) (CTYPE(S64)) ((CTYPE(F32) *) source_memory)[i]; break;
				case QB_TYPE_F64: for(i = 0; i < count; i++) ((CTYPE(S16) *) dest_memory)[i] = (CTYPE(S16)) (CTYPE(S64)) ((CTYPE(F64) *) source_memory)[i]; break;
			}
		}	break;
		case QB_TYPE_U16: {
			switch(source_type) {
				case QB_TYPE_S08: for(i = 0; i < count; i++) ((CTYPE(U16) *) dest_memory)[i] = (CTYPE(U16)) ((CTYPE(S08) *) source_memory)[i]; break;
				case QB_TYPE_U08: for(i = 0; i < count; i++) ((CTYPE(U16) *) dest_memory)[i] = (CTYPE(U16)) ((CTYPE(U08) *) source_memory)[i]; break;
				case QB_TYPE_S16:
				case QB_TYPE_U16: for(i = 0; i < count; i++) ((CTYPE(U16) *) dest_memory)[i] = (CTYPE(U16)) ((CTYPE(U16) *) source_memory)[i]; break;
				case QB_TYPE_S32:
				case QB_TYPE_U32: for(i = 0; i < count; i++) ((CTYPE(U16) *) dest_memory)[i] = (CTYPE(U16)) ((CTYPE(S32) *) source_memory)[i]; break;
				case QB_TYPE_S64:
				case QB_TYPE_U64: for(i = 0; i < count; i++) ((CTYPE(U16) *) dest_memory)[i] = (CTYPE(U16)) ((CTYPE(S64) *) source_memory)[i]; break;
				case QB_TYPE_F32: for(i = 0; i < count; i++) ((CTYPE(U16) *) dest_memory)[i] = (CTYPE(U16)) (CTYPE(S64)) ((CTYPE(F32) *) source_memory)[i]; break;
				case QB_TYPE_F64: for(i = 0; i < count; i++) ((CTYPE(U16) *) dest_memory)[i] = (CTYPE(U16)) (CTYPE(S64)) ((CTYPE(F64) *) source_memory)[i]; break;
			}
		}	break;
		case QB_TYPE_S32: {
			switch(source_type) {
				case QB_TYPE_S08: for(i = 0; i < count; i++) ((CTYPE(S32) *) dest_memory)[i] = (CTYPE(S32)) ((CTYPE(S08) *) source_memory)[i]; break;
				case QB_TYPE_U08: for(i = 0; i < count; i++) ((CTYPE(S32) *) dest_memory)[i] = (CTYPE(S32)) ((CTYPE(U08) *) source_memory)[i]; break;
				case QB_TYPE_S16: for(i = 0; i < count; i++) ((CTYPE(S32) *) dest_memory)[i] = (CTYPE(S32)) ((CTYPE(S16) *) source_memory)[i]; break;
				case QB_TYPE_U16: for(i = 0; i < count; i++) ((CTYPE(S32) *) dest_memory)[i] = (CTYPE(S32)) ((CTYPE(U16) *) source_memory)[i]; break;
				case QB_TYPE_S32:
				case QB_TYPE_U32: for(i = 0; i < count; i++) ((CTYPE(S32) *) dest_memory)[i] = (CTYPE(S32)) ((CTYPE(S32) *) source_memory)[i]; break;
				case QB_TYPE_S64:
				case QB_TYPE_U64: for(i = 0; i < count; i++) ((CTYPE(S32) *) dest_memory)[i] = (CTYPE(S32)) ((CTYPE(S64) *) source_memory)[i]; break;
				case QB_TYPE_F32: for(i = 0; i < count; i++) ((CTYPE(S32) *) dest_memory)[i] = (CTYPE(S32)) (CTYPE(S64)) ((CTYPE(F32) *) source_memory)[i]; break;
				case QB_TYPE_F64: for(i = 0; i < count; i++) ((CTYPE(S32) *) dest_memory)[i] = (CTYPE(S32)) (CTYPE(S64)) ((CTYPE(F64) *) source_memory)[i]; break;
			}
		}	break;
		case QB_TYPE_U32: {
			switch(source_type) {
				case QB_TYPE_S08: for(i = 0; i < count; i++) ((CTYPE(U32) *) dest_memory)[i] = (CTYPE(U32)) ((CTYPE(S08) *) source_memory)[i]; break;
				case QB_TYPE_U08: for(i = 0; i < count; i++) ((CTYPE(U32) *) dest_memory)[i] = (CTYPE(U32)) ((CTYPE(U08) *) source_memory)[i]; break;
				case QB_TYPE_S16: for(i = 0; i < count; i++) ((CTYPE(U32) *) dest_memory)[i] = (CTYPE(U32)) ((CTYPE(S16) *) source_memory)[i]; break;
				case QB_TYPE_U16: for(i = 0; i < count; i++) ((CTYPE(U32) *) dest_memory)[i] = (CTYPE(U32)) ((CTYPE(U16) *) source_memory)[i]; break;
				case QB_TYPE_S32:
				case QB_TYPE_U32: for(i = 0; i < count; i++) ((CTYPE(U32) *) dest_memory)[i] = (CTYPE(U32)) ((CTYPE(U32) *) source_memory)[i]; break;
				case QB_TYPE_S64:
				case QB_TYPE_U64: for(i = 0; i < count; i++) ((CTYPE(U32) *) dest_memory)[i] = (CTYPE(U32)) ((CTYPE(S64) *) source_memory)[i]; break;
				case QB_TYPE_F32: for(i = 0; i < count; i++) ((CTYPE(U32) *) dest_memory)[i] = (CTYPE(U32)) (CTYPE(S64)) ((CTYPE(F32) *) source_memory)[i]; break;
				case QB_TYPE_F64: for(i = 0; i < count; i++) ((CTYPE(U32) *) dest_memory)[i] = (CTYPE(U32)) (CTYPE(S64)) ((CTYPE(F64) *) source_memory)[i]; break;
			}
		}	break;
		case QB_TYPE_S64: {
			switch(source_type) {
				case QB_TYPE_S08: for(i = 0; i < count; i++) ((CTYPE(S64) *) dest_memory)[i] = (CTYPE(S64)) ((CTYPE(S08) *) source_memory)[i]; break;
				case QB_TYPE_U08: for(i = 0; i < count; i++) ((CTYPE(S64) *) dest_memory)[i] = (CTYPE(S64)) ((CTYPE(U08) *) source_memory)[i]; break;
				case QB_TYPE_S16: for(i = 0; i < count; i++) ((CTYPE(S64) *) dest_memory)[i] = (CTYPE(S64)) ((CTYPE(S16) *) source_memory)[i]; break;
				case QB_TYPE_U16: for(i = 0; i < count; i++) ((CTYPE(S64) *) dest_memory)[i] = (CTYPE(S64)) ((CTYPE(U16) *) source_memory)[i]; break;
				case QB_TYPE_S32: for(i = 0; i < count; i++) ((CTYPE(S64) *) dest_memory)[i] = (CTYPE(S64)) ((CTYPE(S32) *) source_memory)[i]; break;
				case QB_TYPE_U32: for(i = 0; i < count; i++) ((CTYPE(S64) *) dest_memory)[i] = (CTYPE(S64)) ((CTYPE(U32) *) source_memory)[i]; break;
				case QB_TYPE_S64:
				case QB_TYPE_U64: for(i = 0; i < count; i++) ((CTYPE(S64) *) dest_memory)[i] = (CTYPE(S64)) ((CTYPE(S64) *) source_memory)[i]; break;
				case QB_TYPE_F32: for(i = 0; i < count; i++) ((CTYPE(S64) *) dest_memory)[i] = (CTYPE(S64)) ((CTYPE(F32) *) source_memory)[i]; break;
				case QB_TYPE_F64: for(i = 0; i < count; i++) ((CTYPE(S64) *) dest_memory)[i] = (CTYPE(S64)) ((CTYPE(F64) *) source_memory)[i]; break;
			}
		}	break;
		case QB_TYPE_U64: {
			switch(source_type) {
				case QB_TYPE_S08: for(i = 0; i < count; i++) ((CTYPE(U64) *) dest_memory)[i] = (CTYPE(U64)) ((CTYPE(S08) *) source_memory)[i]; break;
				case QB_TYPE_U08: for(i = 0; i < count; i++) ((CTYPE(U64) *) dest_memory)[i] = (CTYPE(U64)) ((CTYPE(U08) *) source_memory)[i]; break;
				case QB_TYPE_S16: for(i = 0; i < count; i++) ((CTYPE(U64) *) dest_memory)[i] = (CTYPE(U64)) ((CTYPE(S16) *) source_memory)[i]; break;
				case QB_TYPE_U16: for(i = 0; i < count; i++) ((CTYPE(U64) *) dest_memory)[i] = (CTYPE(U64)) ((CTYPE(U16) *) source_memory)[i]; break;
				case QB_TYPE_S32: for(i = 0; i < count; i++) ((CTYPE(U64) *) dest_memory)[i] = (CTYPE(U64)) ((CTYPE(S32) *) source_memory)[i]; break;
				case QB_TYPE_U32: for(i = 0; i < count; i++) ((CTYPE(U64) *) dest_memory)[i] = (CTYPE(U64)) ((CTYPE(U32) *) source_memory)[i]; break;
				case QB_TYPE_S64:
				case QB_TYPE_U64: for(i = 0; i < count; i++) ((CTYPE(U64) *) dest_memory)[i] = (CTYPE(U64)) ((CTYPE(U64) *) source_memory)[i]; break;
				case QB_TYPE_F32: for(i = 0; i < count; i++) ((CTYPE(U64) *) dest_memory)[i] = (CTYPE(U64)) ((CTYPE(F32) *) source_memory)[i]; break;
				case QB_TYPE_F64: for(i = 0; i < count; i++) ((CTYPE(U64) *) dest_memory)[i] = (CTYPE(U64)) ((CTYPE(F64) *) source_memory)[i]; break;
			}
		}	break;
		case QB_TYPE_F32: {
			switch(source_type) {
				case QB_TYPE_S08: for(i = 0; i < count; i++) ((CTYPE(F32) *) dest_memory)[i] = (CTYPE(F32)) ((CTYPE(S08) *) source_memory)[i]; break;
				case QB_TYPE_U08: for(i = 0; i < count; i++) ((CTYPE(F32) *) dest_memory)[i] = (CTYPE(F32)) ((CTYPE(U08) *) source_memory)[i]; break;
				case QB_TYPE_S16: for(i = 0; i < count; i++) ((CTYPE(F32) *) dest_memory)[i] = (CTYPE(F32)) ((CTYPE(S16) *) source_memory)[i]; break;
				case QB_TYPE_U16: for(i = 0; i < count; i++) ((CTYPE(F32) *) dest_memory)[i] = (CTYPE(F32)) ((CTYPE(U16) *) source_memory)[i]; break;
				case QB_TYPE_S32: for(i = 0; i < count; i++) ((CTYPE(F32) *) dest_memory)[i] = (CTYPE(F32)) ((CTYPE(S32) *) source_memory)[i]; break;
				case QB_TYPE_U32: for(i = 0; i < count; i++) ((CTYPE(F32) *) dest_memory)[i] = (CTYPE(F32)) ((CTYPE(U32) *) source_memory)[i]; break;
				case QB_TYPE_S64: for(i = 0; i < count; i++) ((CTYPE(F32) *) dest_memory)[i] = (CTYPE(F32)) ((CTYPE(S64) *) source_memory)[i]; break;
				case QB_TYPE_U64: for(i = 0; i < count; i++) ((CTYPE(F32) *) dest_memory)[i] = (CTYPE(F32)) ((CTYPE(U64) *) source_memory)[i]; break;
				case QB_TYPE_F32: for(i = 0; i < count; i++) ((CTYPE(F32) *) dest_memory)[i] = (CTYPE(F32)) ((CTYPE(F32) *) source_memory)[i]; break;
				case QB_TYPE_F64: for(i = 0; i < count; i++) ((CTYPE(F32) *) dest_memory)[i] = (CTYPE(F32)) ((CTYPE(F64) *) source_memory)[i]; break;
			}
		}	break;
		case QB_TYPE_F64: {
			switch(source_type) {
				case QB_TYPE_S08: for(i = 0; i < count; i++) ((CTYPE(F64) *) dest_memory)[i] = (CTYPE(F64)) ((CTYPE(S08) *) source_memory)[i]; break;
				case QB_TYPE_U08: for(i = 0; i < count; i++) ((CTYPE(F64) *) dest_memory)[i] = (CTYPE(F64)) ((CTYPE(U08) *) source_memory)[i]; break;
				case QB_TYPE_S16: for(i = 0; i < count; i++) ((CTYPE(F64) *) dest_memory)[i] = (CTYPE(F64)) ((CTYPE(S16) *) source_memory)[i]; break;
				case QB_TYPE_U16: for(i = 0; i < count; i++) ((CTYPE(F64) *) dest_memory)[i] = (CTYPE(F64)) ((CTYPE(U16) *) source_memory)[i]; break;
				case QB_TYPE_S32: for(i = 0; i < count; i++) ((CTYPE(F64) *) dest_memory)[i] = (CTYPE(F64)) ((CTYPE(S32) *) source_memory)[i]; break;
				case QB_TYPE_U32: for(i = 0; i < count; i++) ((CTYPE(F64) *) dest_memory)[i] = (CTYPE(F64)) ((CTYPE(U32) *) source_memory)[i]; break;
				case QB_TYPE_S64: for(i = 0; i < count; i++) ((CTYPE(F64) *) dest_memory)[i] = (CTYPE(F64)) ((CTYPE(S64) *) source_memory)[i]; break;
				case QB_TYPE_U64: for(i = 0; i < count; i++) ((CTYPE(F64) *) dest_memory)[i] = (CTYPE(F64)) ((CTYPE(U64) *) source_memory)[i]; break;
				case QB_TYPE_F32: for(i = 0; i < count; i++) ((CTYPE(F64) *) dest_memory)[i] = (CTYPE(F64)) ((CTYPE(F32) *) source_memory)[i]; break;
				case QB_TYPE_F64: for(i = 0; i < count; i++) ((CTYPE(F64) *) dest_memory)[i] = (CTYPE(F64)) ((CTYPE(F64) *) source_memory)[i]; break;
			}
		}	break;
	}
	if(source_count < dest_count) {
		qb_copy_wrap_around(dest_memory, BYTE_COUNT(source_count, dest_type), BYTE_COUNT(dest_count, dest_type));
	}
}

int64_t ZEND_FASTCALL qb_zval_to_long(zval *zvalue) {
	if(Z_TYPE_P(zvalue) == IS_DOUBLE) {
		float64_t value = Z_DVAL_P(zvalue);
		if(value > 0) {
			// convert from floating point to unsigned int
			uint64_t integer = (uint64_t) value;
			return (int64_t) integer;
		} else {
			// convert from float to signed int
			int64_t integer = (int64_t) value;
			return integer;
		}
	} else {
		int64_t value = Z_LVAL_P(zvalue);
		return value;
	}
}

double ZEND_FASTCALL qb_zval_to_double(zval *zvalue) {
	if(Z_TYPE_P(zvalue) == IS_DOUBLE) {
		float64_t value = Z_DVAL_P(zvalue);
		return value;
	} else {
	#if SIZEOF_LONG == 8
		int64_t value = Z_LVAL_P(zvalue);
	#else
		int32_t value = Z_LVAL_P(zvalue);
	#endif
		return value;
	}
}

int64_t ZEND_FASTCALL qb_zval_array_to_int64(zval *zvalue) {
	int32_t hi_dword_present = FALSE, lo_dword_present = FALSE;
	int64_t hi_dword, lo_dword;
	HashTable *ht = Z_ARRVAL_P(zvalue);
	zval **p_hi_dword = NULL, **p_lo_dword = NULL;
	if(ht->nNumOfElements == 2) {
		if(zend_hash_index_find(ht, 0, (void **) &p_hi_dword) == SUCCESS) {
			if(Z_TYPE_PP(p_hi_dword) == IS_LONG)  {
				hi_dword = Z_LVAL_PP(p_hi_dword);
				hi_dword_present = TRUE;
			} else if(Z_TYPE_PP(p_hi_dword) == IS_DOUBLE) {
				hi_dword = (int64_t ) Z_DVAL_PP(p_hi_dword);
				hi_dword_present = TRUE;
			}
		}
		if(zend_hash_index_find(ht, 1, (void **) &p_lo_dword) == SUCCESS) {
			if(Z_TYPE_PP(p_lo_dword) == IS_LONG)  {
				lo_dword = Z_LVAL_PP(p_lo_dword);
				lo_dword_present = TRUE;
			} else if(Z_TYPE_PP(p_lo_dword) == IS_DOUBLE) {
				lo_dword = (int64_t ) Z_DVAL_PP(p_lo_dword);
				lo_dword_present = TRUE;
			}
		}
	}
	if(!hi_dword_present || !lo_dword_present) {
		qb_abort("unable to convert array to int64");
	}
	return (hi_dword & 0xFFFFFFFF) << 32 | (lo_dword & 0xFFFFFFFF);
}

zval * ZEND_FASTCALL qb_string_to_zval(const char *s, uint32_t len TSRMLS_DC) {
	zval *value = &QB_G(static_zvals)[ QB_G(static_zval_index)++ ];
	if(QB_G(static_zval_index) >= sizeof(QB_G(static_zvals)) / sizeof(zval)) {
		QB_G(static_zval_index) = 0;
	}
	value->value.str.val = (char *) s;
	value->value.str.len = len;
	return value;
}

zval * ZEND_FASTCALL qb_cstring_to_zval(const char *s TSRMLS_DC) {
	return qb_string_to_zval(s, strlen(s) TSRMLS_CC);
}

uint32_t ZEND_FASTCALL qb_element_to_string(char *buffer, uint32_t buffer_len, int8_t *bytes, uint32_t type) {
	switch(type) {
		case QB_TYPE_S08: return snprintf(buffer, buffer_len, "%" PRId8, ((CTYPE(S08) *) bytes)[0]); 
		case QB_TYPE_U08: return snprintf(buffer, buffer_len, "%" PRIu8, ((CTYPE(U08) *) bytes)[0]); 
		case QB_TYPE_S16: return snprintf(buffer, buffer_len, "%" PRId16, ((CTYPE(S16) *) bytes)[0]); 
		case QB_TYPE_U16: return snprintf(buffer, buffer_len, "%" PRIu16, ((CTYPE(S16) *) bytes)[0]); 
		case QB_TYPE_S32: return snprintf(buffer, buffer_len, "%" PRId32, ((CTYPE(S32) *) bytes)[0]); 
		case QB_TYPE_U32: return snprintf(buffer, buffer_len, "%" PRIu32, ((CTYPE(U32) *) bytes)[0]); 
		case QB_TYPE_S64: return snprintf(buffer, buffer_len, "%" PRId64, ((CTYPE(S64) *) bytes)[0]); 
		case QB_TYPE_U64: return snprintf(buffer, buffer_len, "%" PRIu64, ((CTYPE(U64) *) bytes)[0]); 
		case QB_TYPE_F32: return snprintf(buffer, buffer_len, "%.11G", ((CTYPE(F32) *) bytes)[0]); 
		case QB_TYPE_F64: return snprintf(buffer, buffer_len, "%.17G", ((CTYPE(F64) *) bytes)[0]); 
	}
	return 0;
}

int32_t ZEND_FASTCALL qb_uncompress_table(const char *data, void ***p_table, uint32_t *p_item_count, int32_t persistent) {
	static int32_t decompression_failed = FALSE;
	uint32_t compressed_length = SWAP_LE_I32(((uint32_t *) data)[0]);
	uint32_t uncompressed_length = SWAP_LE_I32(((uint32_t *) data)[1]);
	uint32_t total_data_length = SWAP_LE_I32(((uint32_t *) data)[2]);
	uint32_t item_count = SWAP_LE_I32(((uint32_t *) data)[3]);
	TSRMLS_FETCH();

	if(compressed_length < uncompressed_length) {
		// call gzinflate through PHP (since the zlib functions are not otherwise exposed)
		zval *function_name;
		zval *param, **p_param = &param;
		zval *retval = NULL;

		if(!decompression_failed) {
			function_name = qb_cstring_to_zval("gzinflate" TSRMLS_CC);
			param = qb_string_to_zval(data + sizeof(uint32_t) * 4, compressed_length TSRMLS_CC);

			if(call_user_function_ex(CG(function_table), NULL, function_name, &retval, 1, &p_param, TRUE, NULL TSRMLS_CC) != SUCCESS) {
				decompression_failed = TRUE;
			}
		}
		if(retval && Z_TYPE_P(retval) == IS_STRING) {
			int8_t *p = (int8_t *) Z_STRVAL_P(retval), *q; 
			int8_t *data_end = p + Z_STRLEN_P(retval);

			// allocate a memory block large enough to hold both the pointer array and the data
			void **table = pemalloc(sizeof(void *) * item_count + total_data_length, persistent);
			memset(table, 0, sizeof(void *) * item_count);
			q = ((int8_t *) table) + sizeof(void *) * item_count;

			while(p < data_end) {
				uint32_t i, index, index_count = SWAP_LE_I32(((uint32_t *) p)[0]);
				uint32_t *indices = ((uint32_t *) p) + 1;
				uint32_t data_length = SWAP_LE_I32(indices[index_count]);
				uint32_t header_length = sizeof(uint32_t) * (1 + index_count + 1);
				p += header_length;
				memcpy(q, p, data_length);
				for(i = 0; i < index_count; i++) {
					index = SWAP_LE_I32(indices[i]);
					table[index] = q;
				}
				p += data_length;
				q += data_length;
			}

			*p_table = table;
			if(p_item_count) {
				*p_item_count = item_count;
			}
			zval_ptr_dtor(&retval);
			return TRUE;
		} else {
			*p_table = NULL;
			if(p_item_count) {
				*p_item_count = 0;
			}
			return FALSE;
		}
	} else {
		int8_t *p = (int8_t *) data + sizeof(uint32_t) * 4;
		int8_t *data_end = (int8_t *) data + uncompressed_length;
		void **table = pemalloc(sizeof(void *) * item_count, persistent);
		memset(table, 0, sizeof(void *) * item_count);

		while(p < data_end) {
			uint32_t i, index, index_count = SWAP_LE_I32(((uint32_t *) p)[0]);
			uint32_t *indices = ((uint32_t *) p) + 1;
			uint32_t data_length = SWAP_LE_I32(indices[index_count]);
			uint32_t header_length = sizeof(uint32_t) * (1 + index_count + 1);
			p += header_length;
			for(i = 0; i < index_count; i++) {
				index = SWAP_LE_I32(indices[i]);
				table[index] = p;
			}
			p += data_length;
		}
		*p_table = table;
		if(p_item_count) {
			*p_item_count = item_count;
		}
		return TRUE;
	}
}

void ZEND_FASTCALL qb_create_block_allocator(qb_block_allocator **p_allocator, uint32_t item_size, uint32_t capacity) {
	uint32_t total_size = offsetof(qb_block_allocator, data) + (item_size * capacity);
	qb_block_allocator *al = emalloc(total_size);
	al->count = 0;
	al->previous_block = NULL;
	al->capacity = capacity;
	al->item_size = item_size;
	al->top = al->data;
	memset(al->data, 0, capacity * item_size);
	*p_allocator = al;
}

void * ZEND_FASTCALL qb_allocate_items(qb_block_allocator **p_allocator, uint32_t count) {
	qb_block_allocator *al = *p_allocator;
	void *pointer;
	if(al->count + count > al->capacity) {
		qb_block_allocator *new_block;
		if(count > al->capacity) {
			qb_create_block_allocator(&new_block, al->item_size, count);
			new_block->previous_block = al->previous_block;
			al->previous_block = new_block;
			al = new_block;
		} else {
			qb_create_block_allocator(&new_block, al->item_size, al->capacity);
			new_block->previous_block = al;
			*p_allocator = al = new_block;
		}
	}
	pointer = al->top;
	al->count += count;
	al->top += count * al->item_size;
	return pointer;
}

void ZEND_FASTCALL qb_reset_block_allocator(qb_block_allocator **p_allocator) {
	qb_block_allocator *al = *p_allocator, *bl;
	while(al->previous_block) {
		bl = al;
		al = al->previous_block;
		efree(bl);
	}
	al->count = 0;
	memset(al->data, 0, al->capacity * al->item_size);
	al->top = al->data;
	*p_allocator = al;
}

void ZEND_FASTCALL qb_destroy_block_allocator(qb_block_allocator **p_allocator) {
	qb_block_allocator *al = *p_allocator, *bl;
	while(al) {
		bl = al;
		al = al->previous_block;
		efree(bl);
	}
}

typedef struct qb_array_attributes {
	uint32_t *p_count;
	uint32_t capacity;
	uint32_t item_size;
	uint32_t increment;
	char data[4];
} qb_array_attributes;

#define GET_ARRAY_ATTRIBUTES(p)		((qb_array_attributes *) ((char *) (p) - offsetof(qb_array_attributes, data)))

void ZEND_FASTCALL qb_create_array(void **p_array, uint32_t *p_count, uint32_t item_size, uint32_t initial_capacity) {
	uint32_t total_size = offsetof(qb_array_attributes, data) + (item_size * initial_capacity);
	qb_array_attributes *a = emalloc(total_size);
	a->item_size = item_size;
	a->capacity = initial_capacity;
	a->increment = (initial_capacity > 16) ? initial_capacity / 4 : 4;
	a->p_count = p_count;
	memset(a->data, 0, item_size * initial_capacity);
	*p_count = 0;
	*p_array = a->data;
}

void * ZEND_FASTCALL qb_enlarge_array(void **p_array, uint32_t addition) {
	qb_array_attributes *a = GET_ARRAY_ATTRIBUTES(*p_array);
	void *pointer;
	uint32_t current_count = *a->p_count;
	uint32_t new_count = *a->p_count + addition;
	if(new_count > a->capacity) {
		uint32_t total_size;
		a->capacity += a->increment;
		if(new_count > a->capacity) {
			a->capacity = new_count;
		}
		total_size = offsetof(qb_array_attributes, data) + (a->item_size * a->capacity);
		a = erealloc(a, total_size);
		a->increment = a->capacity / 4;
		memset(a->data + (a->item_size * current_count), 0, a->item_size * (a->capacity - current_count));
		*p_array = a->data;
	}
	pointer = a->data + (a->item_size * current_count);
	*a->p_count += addition;
	return pointer;
}

void ZEND_FASTCALL qb_destroy_array(void **p_array) {
	if(*p_array) {
		qb_array_attributes *a = GET_ARRAY_ATTRIBUTES(*p_array);
		efree(a);
	}
}

#ifdef PHP_WIN32
double ZEND_FASTCALL qb_get_high_res_timestamp(void) {
	double seconds;
	LARGE_INTEGER count, frequency;
	QueryPerformanceCounter(&count);
	QueryPerformanceFrequency(&frequency);
	seconds = (double) count.QuadPart / (double) frequency.QuadPart;
	return seconds;
}
#elif defined(__MACH__)
#include <mach/mach_time.h>
double ZEND_FASTCALL qb_get_high_res_timestamp(void) {
	double seconds;
	uint64_t abs_time = mach_absolute_time();
	mach_timebase_info_data_t info;
	mach_timebase_info(&info);
	seconds = abs_time * ((double) info.numer / info.denom) / 1000000000.0;
	return seconds;
}
#else
#include <time.h>
double ZEND_FASTCALL qb_get_high_res_timestamp(void) {
	double seconds;
	struct timespec t;
	clock_gettime(CLOCK_THREAD_CPUTIME_ID, &t);
	seconds = t.tv_sec + ((double) t.tv_nsec) / 1000000000;
	return seconds;
}
#endif

#if __SSE2__
#include <emmintrin.h>
#endif

float qb_fast_rsqrtf(float a) {
	float r;
#if __SSE2__
	float half = 0.5f;
	float three = 3.0f;
	__m128 arg, nr, muls, beta, gamma, final;
	arg = _mm_load_ss(&a);
	nr = _mm_rsqrt_ss(arg);
	muls = _mm_mul_ss(_mm_mul_ss(arg, nr), nr);
	beta = _mm_mul_ss(_mm_load_ss(&half), nr);
	gamma = _mm_sub_ss(_mm_load_ss(&three), muls);
	final = _mm_mul_ss(beta, gamma);
	_mm_store_ss(&r, final);
#else
	r = 1.0f / sqrtf(a);
#endif
	return r;
}

double qb_fast_rsqrt(double a) {
	return 1.0 / sqrt(a);
}

float qb_fast_sqrtf(float a) {
	if(a != 0) {
		return a * qb_fast_rsqrtf(a);
	} else {
		return 0;
	}
}

double qb_fast_sqrt(double a) {
	return sqrt(a);
}

uint32_t type_sizes[QB_TYPE_COUNT] = {
	sizeof(int8_t),
	sizeof(uint8_t),
	sizeof(int16_t),
	sizeof(uint16_t),
	sizeof(int32_t),
	sizeof(uint32_t),
	sizeof(int64_t),
	sizeof(uint64_t),
	sizeof(float32_t),
	sizeof(float64_t)
};

#define SIZE_SHIFT(type)				sizeof(type) == 8 ? 3 : sizeof(type) == 4 ? 2 : sizeof(type) == 2 ? 1 : 0

uint32_t type_size_shifts[] = {
	SIZE_SHIFT(int8_t),
	SIZE_SHIFT(uint8_t),
	SIZE_SHIFT(int16_t),
	SIZE_SHIFT(uint16_t),
	SIZE_SHIFT(int32_t),
	SIZE_SHIFT(uint32_t),
	SIZE_SHIFT(int64_t),
	SIZE_SHIFT(uint64_t),
	SIZE_SHIFT(float32_t),
	SIZE_SHIFT(float64_t)
};

int64_t integer_lower_bounds[] = {
	INT8_MIN,
	0,
	INT16_MIN,
	0,
	INT32_MIN,
	0,
	INT64_MIN,
	0
};

uint64_t integer_upper_bounds[] = {
	INT8_MAX,
	UINT8_MAX,
	INT16_MAX,
	UINT16_MAX,
	INT32_MAX,
	UINT32_MAX,
	INT64_MAX,
	UINT64_MAX
};

const char *type_names[] = {
	"int8",
	"uint8",
	"int16",
	"uint16",
	"int32",
	"uint32",
	"int64",
	"uint64",
	"float32",
	"float64"
};

const char *type_cnames[] = {
	"int8_t",
	"uint8_t",
	"int16_t",
	"uint16_t",
	"int32_t",
	"uint32_t",
	"int64_t",
	"uint64_t",
	"float32_t",
	"float64_t"
};

const uint8_t utf8d[] = {
  // The first part of the table maps bytes to character classes that
  // to reduce the size of the transition table and create bitmasks.
   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
   1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,  9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,
   7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,  7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
   8,8,2,2,2,2,2,2,2,2,2,2,2,2,2,2,  2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
  10,3,3,3,3,3,3,3,3,3,3,3,3,4,3,3, 11,6,6,6,5,8,8,8,8,8,8,8,8,8,8,8,

  // The second part is a transition table that maps a combination
  // of a state of the automaton and a character class to a state.
   0,12,24,36,60,96,84,12,12,12,48,72, 12,12,12,12,12,12,12,12,12,12,12,12,
  12, 0,12,12,12,12,12, 0,12, 0,12,12, 12,24,12,12,12,12,12,24,12,24,12,12,
  12,12,12,12,12,12,12,24,12,12,12,12, 12,24,12,12,12,12,12,12,12,24,12,12,
  12,12,12,12,12,12,12,36,12,36,12,12, 12,36,12,12,12,12,12,36,12,36,12,12,
  12,36,12,12,12,12,12,12,12,12,12,12, 
};
