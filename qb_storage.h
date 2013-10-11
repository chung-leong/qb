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
	// shared scalars (no separation on fork, clearing on call) 
	QB_SELECTOR_SHARED_SCALAR		= 2,
	// local scalars (separation on fork, clearing on call)
	QB_SELECTOR_LOCAL_SCALAR		= 3,
	// temporary scalars (seperation on fork, no clearing on call)
	QB_SELECTOR_TEMPORARY_SCALAR	= 4,

	// constant arrays (no separation on fork, no clearing on call)
	QB_SELECTOR_CONSTANT_ARRAY		= 9,
	// static arrays (no separation on fork, no clearing on call)
	QB_SELECTOR_STATIC_ARRAY		= 8,
	// shared fixed-length arrays (no separation on fork, clearing on call) 
	QB_SELECTOR_SHARED_ARRAY		= 7,
	// local fixed-length arrays (separation on fork, clearing on call)
	QB_SELECTOR_LOCAL_ARRAY			= 6,
	// temporary fixed-length arrays (seperation on fork, no clearing on call)
	QB_SELECTOR_TEMPORARY_ARRAY		= 5,

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

	// global variables
	QB_SELECTOR_GLOBAL_SCALAR		= 10,
	QB_SELECTOR_GLOBAL_ARRAY		= 11,

	// class (static) variables
	QB_SELECTOR_CLASS_SCALAR		= 12,
	QB_SELECTOR_CLASS_ARRAY			= 13,

	// object instance variables 
	QB_SELECTOR_OBJECT_SCALAR		= 14,
	QB_SELECTOR_OBJECT_ARRAY		= 15,

	// variable length arrays are stored in segment 16 and above
	QB_SELECTOR_ARRAY_START			= 16,

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

#define gdTrueColorAlpha(r, g, b, a) (((a) << 24) + \
	((r) << 16) + \
	((g) << 8) + \
	(b))

#define gdMaxColors 256

#define gdAlphaMax 127
#define gdAlphaOpaque 0
#define gdAlphaTransparent 127
#define gdRedMax 255
#define gdGreenMax 255
#define gdBlueMax 255
#define gdTrueColorGetAlpha(c) (((c) & 0x7F000000) >> 24)
#define gdTrueColorGetRed(c) (((c) & 0xFF0000) >> 16)
#define gdTrueColorGetGreen(c) (((c) & 0x00FF00) >> 8)
#define gdTrueColorGetBlue(c) ((c) & 0x0000FF)

#define gdImagePalettePixel(im, x, y) (im)->pixels[(y)][(x)]
#define gdImageTrueColorPixel(im, x, y) (im)->tpixels[(y)][(x)]

typedef struct gdImageStruct {
	/* Palette-based image pixels */
	unsigned char ** pixels;
	int sx;
	int sy;
	/* These are valid in palette images only. See also
		'alpha', which appears later in the structure to
		preserve binary backwards compatibility */
	int colorsTotal;
	int red[gdMaxColors];
	int green[gdMaxColors];
	int blue[gdMaxColors];
	int open[gdMaxColors];
	/* For backwards compatibility, this is set to the
		first palette entry with 100% transparency,
		and is also set and reset by the
		gdImageColorTransparent function. Newer
		applications can allocate palette entries
		with any desired level of transparency; however,
		bear in mind that many viewers, notably
		many web browsers, fail to implement
		full alpha channel for PNG and provide
		support for full opacity or transparency only. */
	int transparent;
	int *polyInts;
	int polyAllocated;
	struct gdImageStruct *brush;
	struct gdImageStruct *tile;
	int brushColorMap[gdMaxColors];
	int tileColorMap[gdMaxColors];
	int styleLength;
	int stylePos;
	int *style;
	int interlace;
	/* New in 2.0: thickness of line. Initialized to 1. */
	int thick;
	/* New in 2.0: alpha channel for palettes. Note that only
		Macintosh Internet Explorer and (possibly) Netscape 6
		really support multiple levels of transparency in
		palettes, to my knowledge, as of 2/15/01. Most
		common browsers will display 100% opaque and
		100% transparent correctly, and do something
		unpredictable and/or undesirable for levels
		in between. TBB */
	int alpha[gdMaxColors];
	/* Truecolor flag and pixels. New 2.0 fields appear here at the
		end to minimize breakage of existing object code. */
	int trueColor;
	int ** tpixels;
	/* Should alpha channel be copied, or applied, each time a
		pixel is drawn? This applies to truecolor images only.
		No attempt is made to alpha-blend in palette images,
		even if semitransparent palette entries exist.
		To do that, build your image as a truecolor image,
		then quantize down to 8 bits. */
	int alphaBlendingFlag;
	/* Should antialias functions be used */
	int antialias;
	/* Should the alpha channel of the image be saved? This affects
		PNG at the moment; other future formats may also
		have that capability. JPEG doesn't. */
	int saveAlphaFlag;


	/* 2.0.12: anti-aliased globals */
	int AA;
	int AA_color;
	int AA_dont_blend;
	unsigned char **AA_opacity;
	int AA_polygon;
	/* Stored and pre-computed variables for determining the perpendicular
	 * distance from a point to the anti-aliased line being drawn:
	 */
	int AAL_x1;
	int AAL_y1;
	int AAL_x2;
	int AAL_y2;
	int AAL_Bx_Ax;
	int AAL_By_Ay;
	int AAL_LAB_2;
	float AAL_LAB;

	/* 2.0.12: simple clipping rectangle. These values must be checked for safety when set; please use gdImageSetClip */
	int cx1;
	int cy1;
	int cx2;
	int cy2;
} gdImage;

typedef gdImage * gdImagePtr;

#define ARRAY_IN(storage, type, address)	((CTYPE(type) *) (storage->segments[address->segment_selector].memory + address->segment_offset))
#define VALUE_IN(storage, type, address)	*ARRAY_IN(storage, type, address)
#define ARRAY_SIZE_IN(storage, address)		VALUE_IN(storage, U32, address->array_size_address)

#define ARRAY(type, address)				ARRAY_IN(cxt->storage, type, address)
#define VALUE(type, address)				VALUE_IN(cxt->storage, type, address)
#define ARRAY_SIZE(address)					VALUE(U32, address->array_size_address)

void qb_transfer_value_from_zval(qb_storage *storage, qb_address *address, zval *zvalue, int32_t transfer_flags);
void qb_transfer_value_from_storage_location(qb_storage *storage, qb_address *address, qb_storage *src_storage, qb_address *src_address, uint32_t transfer_flags);
void qb_transfer_value_to_zval(qb_storage *storage, qb_address *address, zval *zvalue);
void qb_transfer_value_to_storage_location(qb_storage *storage, qb_address *address, qb_storage *dst_storage, qb_address *src_address);

void qb_allocate_segment_memory(qb_storage *storage, qb_memory_segment *segment, uint32_t byte_count);
void qb_release_segment(qb_storage *storage, qb_memory_segment *segment);
intptr_t qb_resize_segment(qb_storage *storage, qb_memory_segment *segment, uint32_t new_size);

void qb_import_segments(qb_storage *src_storage, qb_address *src_address, qb_storage *dst_storage, qb_address *dst_address);

#endif