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

#ifndef QB_STORAGE_H_
#define QB_STORAGE_H_

typedef struct qb_memory_segment			qb_memory_segment;
typedef struct qb_storage					qb_storage;

enum {
	QB_SEGMENT_PREALLOCATED			= 0x00000001,
	QB_SEGMENT_SEPARATE_ON_FORK		= 0x00000002,
	QB_SEGMENT_CLEAR_ON_CALL		= 0x00000004,
	QB_SEGMENT_FREE_ON_RETURN		= 0x00000008,
	QB_SEGMENT_EMPTY_ON_RETURN		= 0x00000010,

	QB_SEGMENT_BORROWED				= 0x00000100,
	QB_SEGMENT_MAPPED				= 0x00000200,
};

struct qb_memory_segment {
	int8_t *memory;
	uint32_t flags;
	uint32_t byte_count;						// number of bytes in this segment
	uint32_t current_allocation;				// number of bytes allocated
	php_stream *stream;							// memory-mapped file
	uintptr_t **references;
	uint32_t reference_count;
};

struct qb_storage {
	qb_memory_segment *segments;
	uint32_t segment_count;
	uint32_t flags;
};

enum {
	QB_TRANSFER_CAN_BORROW_MEMORY	= 0x0001,
	QB_TRANSFER_CAN_SEIZE_MEMORY	= 0x0002,
	QB_TRANSFER_CAN_ENLARGE_SEGMENT	= 0x0004,
};

#define ARRAY_IN(storage, type, address)	((CTYPE(type) *) (storage->segments[address->segment_selector].memory + address->segment_offset))
#define VALUE_IN(storage, type, address)	*ARRAY_IN(storage, type, address)
#define ARRAY_SIZE_IN(storage, address)		VALUE_IN(storage, U32, address->array_size_address)

#define ARRAY(type, address)				ARRAY_IN(cxt->storage, type, address)
#define VALUE(type, address)				VALUE_IN(cxt->storage, type, address)
#define ARRAY_SIZE(address)					VALUE(U32, address->array_size_address)

#endif