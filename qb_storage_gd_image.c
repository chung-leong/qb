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

typedef enum qb_pixel_format qb_pixel_format;

enum qb_pixel_format {
	QB_PIXEL_ARRANGEMENT_1D = 0x40000000,
	QB_PIXEL_ARRANGEMENT_2D = 0x80000000,
	QB_PIXEL_ARRANGEMENT_FLAGS = 0xF0000000,

	QB_PIXEL_INVALID = 0,
	QB_PIXEL_I08_1,
	QB_PIXEL_I08_4,
	QB_PIXEL_I32_1,
	QB_PIXEL_F32_1,
	QB_PIXEL_F32_3,
	QB_PIXEL_F32_4,
	QB_PIXEL_F64_1,
	QB_PIXEL_F64_3,
	QB_PIXEL_F64_4,

	QB_PIXEL_1D_I08_1 = QB_PIXEL_I08_1 | QB_PIXEL_ARRANGEMENT_1D,
	QB_PIXEL_1D_I08_4 = QB_PIXEL_I08_4 | QB_PIXEL_ARRANGEMENT_1D,
	QB_PIXEL_1D_I32_1 = QB_PIXEL_I32_1 | QB_PIXEL_ARRANGEMENT_1D,
	QB_PIXEL_1D_F32_1 = QB_PIXEL_F32_1 | QB_PIXEL_ARRANGEMENT_1D,
	QB_PIXEL_1D_F32_3 = QB_PIXEL_F32_3 | QB_PIXEL_ARRANGEMENT_1D,
	QB_PIXEL_1D_F32_4 = QB_PIXEL_F32_4 | QB_PIXEL_ARRANGEMENT_1D,
	QB_PIXEL_1D_F64_1 = QB_PIXEL_F64_1 | QB_PIXEL_ARRANGEMENT_1D,
	QB_PIXEL_1D_F64_3 = QB_PIXEL_F64_3 | QB_PIXEL_ARRANGEMENT_1D,
	QB_PIXEL_1D_F64_4 = QB_PIXEL_F64_4 | QB_PIXEL_ARRANGEMENT_1D,

	QB_PIXEL_2D_I08_1 = QB_PIXEL_I08_1 | QB_PIXEL_ARRANGEMENT_2D,
	QB_PIXEL_2D_I08_4 = QB_PIXEL_I08_4 | QB_PIXEL_ARRANGEMENT_2D,
	QB_PIXEL_2D_I32_1 = QB_PIXEL_I32_1 | QB_PIXEL_ARRANGEMENT_2D,
	QB_PIXEL_2D_F32_1 = QB_PIXEL_F32_1 | QB_PIXEL_ARRANGEMENT_2D,
	QB_PIXEL_2D_F32_3 = QB_PIXEL_F32_3 | QB_PIXEL_ARRANGEMENT_2D,
	QB_PIXEL_2D_F32_4 = QB_PIXEL_F32_4 | QB_PIXEL_ARRANGEMENT_2D,
	QB_PIXEL_2D_F64_1 = QB_PIXEL_F64_1 | QB_PIXEL_ARRANGEMENT_2D,
	QB_PIXEL_2D_F64_3 = QB_PIXEL_F64_3 | QB_PIXEL_ARRANGEMENT_2D,
	QB_PIXEL_2D_F64_4 = QB_PIXEL_F64_4 | QB_PIXEL_ARRANGEMENT_2D,
};

static gdImagePtr qb_get_gd_image(zval *resource) {
	if(Z_TYPE_P(resource) == IS_RESOURCE) {
		static int le_gd = -1;
		TSRMLS_FETCH();
		if(le_gd == -1) {
			le_gd = zend_fetch_list_dtor_id("gd");
		}
		if(le_gd) {
			gdImagePtr image = (gdImagePtr) zend_fetch_resource(&resource TSRMLS_CC, -1, NULL, NULL, 1, le_gd);
			return image;
		}
	}
	return NULL;
}

static void qb_set_image_dimensions(qb_storage *storage, qb_address *address, gdImagePtr image) {
	qb_address *width_address = address->dimension_addresses[1];
	qb_address *height_address = address->dimension_addresses[0];
	uint32_t width_expected = VALUE_IN(storage, U32, width_address);
	uint32_t height_expected = VALUE_IN(storage, U32, height_address);
	if(width_expected != image->sx) {
		if(CONSTANT(width_address)) {
			qb_abort("Declared array size (%d) does not match the width of image (%d)", width_expected, image->sy);
		} else if(width_expected != 0) {
			qb_abort("Current array size (%d) does not match the width of image (%d)", width_expected, image->sx);
		}
		VALUE_IN(storage, U32, width_address) = image->sx;
	}

	if(height_expected != image->sy) {
		if(CONSTANT(height_address)) {
			qb_abort("Declared array size (%d) does not match the height of image (%d)", height_expected, image->sy);
		} else if(height_expected != 0) {
			qb_abort("Current array size (%d) does not match the height of image (%d)", height_expected, image->sy);
		}
		VALUE_IN(storage, U32, height_address) = image->sy;
	}
}

static void qb_set_image_linear_size(qb_storage *storage, qb_address *address, gdImagePtr image) {
	qb_address *length_address = address->dimension_addresses[0];
	uint32_t pixel_count = image->sx * image->sy;
	uint32_t length_expected = VALUE_IN(storage, U32, length_address);
	if(length_expected != pixel_count) {
		if(CONSTANT(length_address)) {
			qb_abort("Declared array size (%d) does not match the size of image (%d)", length_expected, pixel_count);
		} else if(length_expected != 0) {
			qb_abort("Current array size (%d) does not match the size of image (%d)", length_expected, pixel_count);
		}
	}
	VALUE_IN(storage, U32, length_address) = pixel_count;
}

static qb_pixel_format qb_get_compatible_pixel_format(qb_storage *storage, qb_address *address, int32_t true_color) {
	qb_pixel_format pixel_format = QB_PIXEL_INVALID;
	qb_primitive_type element_type = address->type;
	qb_address *last_dimension_address = DIMENSION_ADDRESS(address, -1);
	uint32_t last_dimension = VALUE_IN(storage, U32, last_dimension_address);

	if(address->dimension_count == 3) {
		// allow int8, float32, and float64 and the last dimension must be 1, 3, or 4
		// the GD image must be true color
		if(true_color) {
			if(STORAGE_TYPE_MATCH(element_type, QB_TYPE_I08)) {
				if(last_dimension == 4) {
					pixel_format = QB_PIXEL_2D_I08_4;
				}
			} else if(element_type == QB_TYPE_F32) {
				if(last_dimension == 1) {
					pixel_format = QB_PIXEL_2D_F32_1;
				} else if(last_dimension == 3) {
					pixel_format = QB_PIXEL_2D_F32_3;
				} else if(last_dimension == 4) {
					pixel_format = QB_PIXEL_2D_F32_4;
				}
			} else if(element_type == QB_TYPE_F64) {
				if(last_dimension == 1) {
					pixel_format = QB_PIXEL_2D_F64_1;
				} else if(last_dimension == 3) {
					pixel_format = QB_PIXEL_2D_F64_3;
				} else if(last_dimension == 4) {
					pixel_format = QB_PIXEL_2D_F64_4;
				}
			}
		}
	} else if(address->dimension_count == 2) {
		// allow either int32, representing each pixel (if the image is true color)
		// or int8, representing representing each pixel (if the image uses a palette )
		// or int8, float32, and float64, with the last dimension being one, three, or four (if the image is true color)
		if(true_color) {
			if(STORAGE_TYPE_MATCH(element_type, QB_TYPE_I32)) {
				pixel_format = QB_PIXEL_2D_I32_1;
			} else if(element_type == QB_TYPE_F32) {
				if(last_dimension == 1) {
					pixel_format = QB_PIXEL_1D_F32_1;
				} else if(last_dimension == 3) {
					pixel_format = QB_PIXEL_1D_F32_3;
				} else if(last_dimension == 4) {
					pixel_format = QB_PIXEL_1D_F32_4;
				}
			} else if(element_type == QB_TYPE_F64) {
				if(last_dimension == 1) {
					pixel_format = QB_PIXEL_1D_F64_1;
				} else if(last_dimension == 3) {
					pixel_format = QB_PIXEL_1D_F64_3;
				} else if(last_dimension == 4) {
					pixel_format = QB_PIXEL_1D_F64_4;
				}
			}
		} else {
			if(STORAGE_TYPE_MATCH(element_type, QB_TYPE_I08)) {
				pixel_format = QB_PIXEL_2D_I08_1;
			}
		}
	} else if(address->dimension_count == 1) {
		// allow either int32, representing each pixel (if the image is true color)
		// or int8, representing representing each pixel (if the image uses a palette)
		if(true_color) {
			if(STORAGE_TYPE_MATCH(element_type, QB_TYPE_I32)) {
				pixel_format = QB_PIXEL_1D_I32_1;
			}
		} else {
			if(STORAGE_TYPE_MATCH(element_type, QB_TYPE_I08)) {
				pixel_format = QB_PIXEL_1D_I08_1;
			}
		}
	}
	return pixel_format;
}

static uint32_t qb_set_array_dimensions_from_image(qb_storage *storage, qb_address *address, gdImagePtr image) {
	uint32_t array_size, i;
	qb_pixel_format pixel_format = qb_get_compatible_pixel_format(storage, address, image->trueColor);

	if(pixel_format == QB_PIXEL_INVALID) {
		// see what went wrong
		if(address->dimension_count > 3) {
			qb_abort("Variable has too many dimensions for an image (%d)", address->dimension_count);
		} else {
			if(image->trueColor) {
				qb_abort("Variable is not invalid for an true-color image");
			} else {
				qb_abort("Variable is not invalid for an paletted image");
			}
		}
	}

	// set the dimension(s)
	if(pixel_format & QB_PIXEL_ARRANGEMENT_1D) {
		qb_set_image_linear_size(storage, address, image);
	} else {
		qb_set_image_dimensions(storage, address, image);
	}

	// calculate the array sizes
	array_size = 1;
	for(i = address->dimension_count - 1; (int32_t) i >= 0; i--) {
		qb_address *dimension_address = address->dimension_addresses[i];
		qb_address *array_size_address = address->array_size_addresses[i];
		array_size *= VALUE_IN(storage, U32, dimension_address);
		VALUE_IN(storage, U32, array_size_address) = array_size;
	}
	return array_size;
}

static void qb_reallocate_gd_image(gdImagePtr image, int width, int height) {
	int i, scanline_size, pixel_size;
	unsigned char ***p_scanlines;

	if(width <= 0 || height <= 0 || width > INT_MAX / sizeof(int) || height > INT_MAX / sizeof(void *)) {
		qb_abort("Illegal image size");
	}

	if(image->trueColor) {
		p_scanlines = (unsigned char ***) &image->tpixels;
		scanline_size = sizeof(int) * width;
		pixel_size = sizeof(int);
	} else {
		p_scanlines = &image->pixels;
		scanline_size = sizeof(unsigned char) * width;
		pixel_size = sizeof(unsigned char);
	}

	// free scanlines that aren't needed
	for(i = height; i < image->sy; i++) {
		efree((*p_scanlines)[i]);
		efree(image->AA_opacity[i]);
	}

	// reallocate scanline pointer array
	*p_scanlines = erealloc(*p_scanlines, sizeof(unsigned char *) * height);
	image->AA_opacity = erealloc(image->AA_opacity, sizeof(unsigned char *) * height);
	for(i = image->sy; i < height; i++) {
		(*p_scanlines)[i] = NULL;
	}

	// reallocate the scalines themselves
	for(i = 0; i < height; i++) {
		if((*p_scanlines)[i]) {
			(*p_scanlines)[i] = erealloc((*p_scanlines)[i], scanline_size);
			if(width > image->sx) {
				memset((*p_scanlines)[i] + image->sx * pixel_size, 0, pixel_size * (width - image->sx));
			}
		} else {
			(*p_scanlines)[i] = emalloc(scanline_size);
			memset((*p_scanlines)[i], 0, scanline_size);
		}
		if(image->AA_opacity[i]) {
			image->AA_opacity[i] = erealloc(image->AA_opacity[i], sizeof(unsigned char) * width);
			if(width > image->sx) {
				memset(image->AA_opacity[i] + image->sx, 0, sizeof(unsigned char) * (width - image->sx));
			}
		} else {
			image->AA_opacity[i] = emalloc(sizeof(unsigned char) * width);
			memset(image->AA_opacity[i], 0, sizeof(unsigned char) * width);
		}
	}
	image->sx = width;
	image->sy = height;
}

static void qb_copy_elements_from_gd_image(qb_storage *storage, qb_address *address, gdImagePtr image) {
	uint32_t i, j;
	qb_pixel_format pixel_format = qb_get_compatible_pixel_format(storage, address, image->trueColor);
	qb_pixel_format pixel_type = pixel_format & ~QB_PIXEL_ARRANGEMENT_FLAGS;

	if(image->trueColor) {
		int tpixel;

		switch(pixel_type) {
			case QB_PIXEL_I08_4:
			case QB_PIXEL_I32_1: {
				int32_t *p = ARRAY_IN(storage, I32, address);

				for(i = 0; i < (uint32_t) image->sy; i++) {
					for(j = 0; j < (uint32_t) image->sx; j++) {
						tpixel = gdImageTrueColorPixel(image, j, i);
						p[0] = tpixel;
						p += 1;
					}
				}
			}	break;
			case QB_PIXEL_F32_4: {
				float32_t *p = ARRAY_IN(storage, F32, address);
				for(i = 0; i < (uint32_t) image->sy; i++) {
					for(j = 0; j < (uint32_t) image->sx; j++) {
						tpixel = gdImageTrueColorPixel(image, j, i);
						p[0] = ((float32_t) gdTrueColorGetRed(tpixel) * (1.0f / gdRedMax));
						p[1] = ((float32_t) gdTrueColorGetGreen(tpixel) * (1.0f / gdGreenMax));
						p[2] = ((float32_t) gdTrueColorGetBlue(tpixel) * (1.0f / gdBlueMax));
						p[3] = ((float32_t) (gdAlphaTransparent - gdTrueColorGetAlpha(tpixel)) * (1.0f / gdAlphaMax));
						p += 4;
					}
				}
			}	break;
			case QB_PIXEL_F32_3: {
				float32_t *p = ARRAY_IN(storage, F32, address);
				for(i = 0; i < (uint32_t) image->sy; i++) {
					for(j = 0; j < (uint32_t) image->sx; j++) {
						tpixel = gdImageTrueColorPixel(image, j, i);
						p[0] = ((float32_t) gdTrueColorGetRed(tpixel) * (1.0f / gdRedMax));
						p[1] = ((float32_t) gdTrueColorGetGreen(tpixel) * (1.0f / gdGreenMax));
						p[2] = ((float32_t) gdTrueColorGetBlue(tpixel) * (1.0f / gdBlueMax));
						p += 3;
					}
				}
			}	break;
			case QB_PIXEL_F32_1: {
				float32_t *p = ARRAY_IN(storage, F32, address);
				for(i = 0; i < (uint32_t) image->sy; i++) {
					for(j = 0; j < (uint32_t) image->sx; j++) {
						tpixel = gdImageTrueColorPixel(image, j, i);
						p[0] = ((float32_t) gdTrueColorGetRed(tpixel) * (1.0f / gdRedMax)) * 0.299f
							 + ((float32_t) gdTrueColorGetGreen(tpixel) * (1.0f / gdGreenMax)) * 0.587f
							 + ((float32_t) gdTrueColorGetBlue(tpixel) * (1.0f / gdBlueMax)) * 0.114f;
						p += 1;
					}
				}
			}	break;
			case QB_PIXEL_F64_4: {
				float64_t *p = ARRAY_IN(storage, F64, address);
				for(i = 0; i < (uint32_t) image->sy; i++) {
					for(j = 0; j < (uint32_t) image->sx; j++) {
						tpixel = gdImageTrueColorPixel(image, j, i);
						p[0] = ((float32_t) gdTrueColorGetRed(tpixel) * (1.0 / gdRedMax));
						p[1] = ((float32_t) gdTrueColorGetGreen(tpixel) * (1.0 / gdGreenMax));
						p[2] = ((float32_t) gdTrueColorGetBlue(tpixel) * (1.0 / gdBlueMax));
						p[3] = ((float32_t) (gdAlphaTransparent - gdTrueColorGetAlpha(tpixel)) * (1.0 / gdAlphaMax));
						p += 4;
					}
				}
			}	break;
			case QB_PIXEL_F64_3: {
				float64_t *p = ARRAY_IN(storage, F64, address);
				for(i = 0; i < (uint32_t) image->sy; i++) {
					for(j = 0; j < (uint32_t) image->sx; j++) {
						tpixel = gdImageTrueColorPixel(image, j, i);
						p[0] = ((float64_t) gdTrueColorGetRed(tpixel) * (1.0 / gdRedMax));
						p[1] = ((float64_t) gdTrueColorGetGreen(tpixel) * (1.0 / gdGreenMax));
						p[2] = ((float64_t) gdTrueColorGetBlue(tpixel) * (1.0 / gdBlueMax));
						p += 3;
					}
				}
			}	break;
			case QB_PIXEL_F64_1: {
				float64_t *p = ARRAY_IN(storage, F64, address);
				for(i = 0; i < (uint32_t) image->sy; i++) {
					for(j = 0; j < (uint32_t) image->sx; j++) {
						tpixel = gdImageTrueColorPixel(image, j, i);
						p[0] = ((float64_t) gdTrueColorGetRed(tpixel) * (1.0 / gdRedMax)) * 0.299
							 + ((float64_t) gdTrueColorGetGreen(tpixel) * (1.0 / gdGreenMax)) * 0.587
							 + ((float64_t) gdTrueColorGetBlue(tpixel) * (1.0 / gdBlueMax)) * 0.114;
						p += 1;
					}
				}
			}	break;
			default: {
			}	break;
		}
	} else {
		unsigned char pixel;

		switch(pixel_type) {
			case QB_PIXEL_I08_1: {
				int8_t *p = ARRAY_IN(storage, I08, address);

				for(i = 0; i < (uint32_t) image->sy; i++) {
					for(j = 0; j < (uint32_t) image->sx; j++) {
						pixel = gdImagePalettePixel(image, j, i);
						p[0] = pixel;
						p += 1;
					}
				}

			}	break;
			default: {
			}	break;
		}
	}
}

static void qb_copy_elements_to_gd_image(qb_storage *storage, qb_address *address, gdImagePtr image) {
	int32_t width, height;
	uint32_t i, j;
	qb_pixel_format pixel_format = qb_get_compatible_pixel_format(storage, address, image->trueColor);
	qb_pixel_format pixel_type = pixel_format & ~QB_PIXEL_ARRANGEMENT_FLAGS;

	if(pixel_format & QB_PIXEL_ARRANGEMENT_1D) {
		qb_address *length_address = address->dimension_addresses[0];
		uint32_t length = VALUE_IN(storage, U32, length_address);
		if(length >= (uint32_t) image->sx) {
			width = image->sx;
			height = length / width;
		} else {
			width = 0;
			height = 0;
		}
	} else {
		qb_address *height_address = address->dimension_addresses[0];
		qb_address *width_address = address->dimension_addresses[1];
		height = VALUE_IN(storage, S32, height_address);
		width = VALUE_IN(storage, S32, width_address);
	}
	if(image->sy != height || image->sx != width) {
		qb_reallocate_gd_image(image, width, height);
	}

	if(image->trueColor) {
		int tpixel, r, g, b, a;

		switch(pixel_type) {
			case QB_PIXEL_I08_4:
			case QB_PIXEL_I32_1: {
				int32_t *p = ARRAY_IN(storage, I32, address);

				for(i = 0; i < (uint32_t) image->sy; i++) {
					for(j = 0; j < (uint32_t) image->sx; j++) {
						tpixel = *p++;
						gdImageTrueColorPixel(image, j, i) = tpixel;
					}
				}
			}	break;
			case QB_PIXEL_F32_4: {
				float32_t *p = ARRAY_IN(storage, F32, address);
				for(i = 0; i < (uint32_t) image->sy; i++) {
					for(j = 0; j < (uint32_t) image->sx; j++) {
						r = qb_clamp_float32(p[0], gdRedMax);
						g = qb_clamp_float32(p[1], gdGreenMax);
						b = qb_clamp_float32(p[2], gdBlueMax);
						a = gdAlphaTransparent - qb_clamp_float32(p[3], gdAlphaMax);
						tpixel = gdTrueColorAlpha(r, g, b, a);
						gdImageTrueColorPixel(image, j, i) = tpixel;
						p += 4;
					}
				}
			}	break;
			case QB_PIXEL_F32_3: {
				float32_t *p = ARRAY_IN(storage, F32, address);
				for(i = 0; i < (uint32_t) image->sy; i++) {
					for(j = 0; j < (uint32_t) image->sx; j++) {
						r = qb_clamp_float32(p[0], gdRedMax);
						g = qb_clamp_float32(p[1], gdGreenMax);
						b = qb_clamp_float32(p[2], gdBlueMax);
						tpixel = gdTrueColorAlpha(r, g, b, gdAlphaOpaque);
						gdImageTrueColorPixel(image, j, i) = tpixel;
						p += 3;
					}
				}
			}	break;
			case QB_PIXEL_F32_1: {
				float32_t *p = ARRAY_IN(storage, F32, address);
				for(i = 0; i < (uint32_t) image->sy; i++) {
					for(j = 0; j < (uint32_t) image->sx; j++) {
						r = qb_clamp_float32(p[0], gdRedMax);
						tpixel = gdTrueColorAlpha(r, r, r, gdAlphaOpaque);
						gdImageTrueColorPixel(image, j, i) = tpixel;
						p += 1;
					}
				}
			}	break;
			case QB_PIXEL_F64_4: {
				float64_t *p = ARRAY_IN(storage, F64, address);
				for(i = 0; i < (uint32_t) image->sy; i++) {
					for(j = 0; j < (uint32_t) image->sx; j++) {
						r = qb_clamp_float64(p[0], gdRedMax);
						g = qb_clamp_float64(p[1], gdGreenMax);
						b = qb_clamp_float64(p[2], gdBlueMax);
						a = gdAlphaTransparent - qb_clamp_float64(p[3], gdAlphaMax);
						tpixel = gdTrueColorAlpha(r, g, b, a);
						gdImageTrueColorPixel(image, j, i) = tpixel;
						p += 4;
					}
				}
			}	break;
			case QB_PIXEL_F64_3: {
				float64_t *p = ARRAY_IN(storage, F64, address);
				for(i = 0; i < (uint32_t) image->sy; i++) {
					for(j = 0; j < (uint32_t) image->sx; j++) {
						r = qb_clamp_float64(p[0], gdRedMax);
						g = qb_clamp_float64(p[1], gdGreenMax);
						b = qb_clamp_float64(p[2], gdBlueMax);
						tpixel = gdTrueColorAlpha(r, g, b, gdAlphaOpaque);
						gdImageTrueColorPixel(image, j, i) = tpixel;
						p += 3;
					}
				}
			}	break;
			case QB_PIXEL_F64_1: {
				float64_t *p = ARRAY_IN(storage, F64, address);
				for(i = 0; i < (uint32_t) image->sy; i++) {
					for(j = 0; j < (uint32_t) image->sx; j++) {
						r = qb_clamp_float64(p[0], gdRedMax);
						tpixel = gdTrueColorAlpha(r, r, r, gdAlphaOpaque);
						gdImageTrueColorPixel(image, j, i) = tpixel;
						p += 1;
					}
				}
			}	break;
			default: {
			}	break;
		}
	} else {
		unsigned char pixel;

		switch(pixel_type) {
			case QB_PIXEL_I08_1: {
				int8_t *p = ARRAY_IN(storage, I08, address);

				for(i = 0; i < (uint32_t) image->sy; i++) {
					for(j = 0; j < (uint32_t) image->sx; j++) {
						pixel = p[0];
						gdImagePalettePixel(image, j, i) = pixel;
						p += 1;
					}
				}

			}	break;
			default: {
			}	break;
		}
	}
}
