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
	QB_SEGMENT_GLOBAL				= 0x00000020,
	QB_SEGMENT_OBJECT_INSTANCE		= 0x00000040,

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

enum {
	// constant scalar (no separation on fork, no clearing on call)
	QB_SELECTOR_CONSTANT_SCALAR		= 0,
	// static scalars (no separation on fork, no clearing on call)
	QB_SELECTOR_STATIC_SCALAR		= 1,
	// global/class scalars (no separation on fork, no clearing on call, requires relocation)
	QB_SELECTOR_GLOBAL_SCALAR		= 2,
	// object scalars (no separation on fork, no clearing on call, requires relocation)
	QB_SELECTOR_OBJECT_SCALAR		= 3,
	// shared scalars (no separation on fork, clearing on call) 
	QB_SELECTOR_SHARED_SCALAR		= 4,
	// local scalars (separation on fork, clearing on call)
	QB_SELECTOR_LOCAL_SCALAR		= 5,
	// temporary scalars (seperation on fork, no clearing on call)
	QB_SELECTOR_TEMPORARY_SCALAR	= 6,

	QB_SELECTOR_SCALAR_LAST				= QB_SELECTOR_TEMPORARY_SCALAR,

	// constant arrays (no separation on fork, no clearing on call)
	QB_SELECTOR_CONSTANT_ARRAY		= 13,
	// static arrays (no separation on fork, no clearing on call)
	QB_SELECTOR_STATIC_ARRAY		= 12,
	// global/class arrays (no separation on fork, no clearing on call, requires relocation)
	QB_SELECTOR_GLOBAL_ARRAY		= 11,
	// object arrays (no separation on fork, no clearing on call, requires relocation)
	QB_SELECTOR_OBJECT_ARRAY		= 10,
	// shared fixed-length arrays (no separation on fork, clearing on call) 
	QB_SELECTOR_SHARED_ARRAY		= 9,
	// local fixed-length arrays (separation on fork, clearing on call)
	QB_SELECTOR_LOCAL_ARRAY			= 8,
	// temporary fixed-length arrays (seperation on fork, no clearing on call)
	QB_SELECTOR_TEMPORARY_ARRAY		= 7,

	QB_SELECTOR_FIXED_LENGTH_ARRAY_LAST	= QB_SELECTOR_CONSTANT_ARRAY,

	// note how the order is reverse for the array segments
	// this is done so that the segments requiring separation are back-to-back,
	// maing it easier to see if a given pointer requires relocation or not
	//
	// the arrangement also brings variables likely to be active closer together
	//
	// segments that need to be cleared when the function is called are also placed 
	// back-to-back, so we only need two memset() to wipe all variables clean
	//
	// parameters are found in the shared segments

	// variable length arrays are stored in segment 14 and above
	QB_SELECTOR_ARRAY_START			= 14,

	QB_SELECTOR_INVALID 			= 0xFFFFFFFF,
};

enum {
	QB_OFFSET_INVALID 				= 0xFFFFFFFF,
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