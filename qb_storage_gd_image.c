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
	QB_PIXEL_F32_2,
	QB_PIXEL_F32_3,
	QB_PIXEL_F32_4,
	QB_PIXEL_F64_1,
	QB_PIXEL_F64_2,
	QB_PIXEL_F64_3,
	QB_PIXEL_F64_4,

	QB_PIXEL_1D_I08_1 = QB_PIXEL_I08_1 | QB_PIXEL_ARRANGEMENT_1D,
	QB_PIXEL_1D_I08_4 = QB_PIXEL_I08_4 | QB_PIXEL_ARRANGEMENT_1D,
	QB_PIXEL_1D_I32_1 = QB_PIXEL_I32_1 | QB_PIXEL_ARRANGEMENT_1D,
	QB_PIXEL_1D_F32_1 = QB_PIXEL_F32_1 | QB_PIXEL_ARRANGEMENT_1D,
	QB_PIXEL_1D_F32_2 = QB_PIXEL_F32_2 | QB_PIXEL_ARRANGEMENT_1D,
	QB_PIXEL_1D_F32_3 = QB_PIXEL_F32_3 | QB_PIXEL_ARRANGEMENT_1D,
	QB_PIXEL_1D_F32_4 = QB_PIXEL_F32_4 | QB_PIXEL_ARRANGEMENT_1D,
	QB_PIXEL_1D_F64_1 = QB_PIXEL_F64_1 | QB_PIXEL_ARRANGEMENT_1D,
	QB_PIXEL_1D_F64_2 = QB_PIXEL_F64_2 | QB_PIXEL_ARRANGEMENT_1D,
	QB_PIXEL_1D_F64_3 = QB_PIXEL_F64_3 | QB_PIXEL_ARRANGEMENT_1D,
	QB_PIXEL_1D_F64_4 = QB_PIXEL_F64_4 | QB_PIXEL_ARRANGEMENT_1D,

	QB_PIXEL_2D_I08_1 = QB_PIXEL_I08_1 | QB_PIXEL_ARRANGEMENT_2D,
	QB_PIXEL_2D_I08_4 = QB_PIXEL_I08_4 | QB_PIXEL_ARRANGEMENT_2D,
	QB_PIXEL_2D_I32_1 = QB_PIXEL_I32_1 | QB_PIXEL_ARRANGEMENT_2D,
	QB_PIXEL_2D_F32_1 = QB_PIXEL_F32_1 | QB_PIXEL_ARRANGEMENT_2D,
	QB_PIXEL_2D_F32_2 = QB_PIXEL_F32_2 | QB_PIXEL_ARRANGEMENT_2D,
	QB_PIXEL_2D_F32_3 = QB_PIXEL_F32_3 | QB_PIXEL_ARRANGEMENT_2D,
	QB_PIXEL_2D_F32_4 = QB_PIXEL_F32_4 | QB_PIXEL_ARRANGEMENT_2D,
	QB_PIXEL_2D_F64_1 = QB_PIXEL_F64_1 | QB_PIXEL_ARRANGEMENT_2D,
	QB_PIXEL_2D_F64_2 = QB_PIXEL_F64_2 | QB_PIXEL_ARRANGEMENT_2D,
	QB_PIXEL_2D_F64_3 = QB_PIXEL_F64_3 | QB_PIXEL_ARRANGEMENT_2D,
	QB_PIXEL_2D_F64_4 = QB_PIXEL_F64_4 | QB_PIXEL_ARRANGEMENT_2D,
};

static int qb_get_gd_id(void) {
	static int le_gd = -1;
	if(le_gd == -1) {
		le_gd = zend_fetch_list_dtor_id("gd");
	}
	return le_gd;
}

gdImagePtr qb_get_gd_image(zval *resource) {
	if(Z_TYPE_P(resource) == IS_RESOURCE) {
		int le_gd = qb_get_gd_id();
		TSRMLS_FETCH();
		if(le_gd) {
			gdImagePtr image = (gdImagePtr) zend_fetch_resource(&resource TSRMLS_CC, -1, NULL, NULL, 1, le_gd);
			return image;
		}
	}
	return NULL;
}

static qb_pixel_format qb_get_compatible_pixel_format(uint32_t dimension_count, uint32_t last_dimension, qb_primitive_type element_type, int32_t true_color) {
	qb_pixel_format pixel_format = QB_PIXEL_INVALID;

	if(dimension_count == 3) {
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
				} else if(last_dimension == 2) {
					pixel_format = QB_PIXEL_2D_F32_2;
				} else if(last_dimension == 3) {
					pixel_format = QB_PIXEL_2D_F32_3;
				} else if(last_dimension == 4) {
					pixel_format = QB_PIXEL_2D_F32_4;
				}
			} else if(element_type == QB_TYPE_F64) {
				if(last_dimension == 1) {
					pixel_format = QB_PIXEL_2D_F64_1;
				} else if(last_dimension == 2) {
					pixel_format = QB_PIXEL_2D_F64_2;
				} else if(last_dimension == 3) {
					pixel_format = QB_PIXEL_2D_F64_3;
				} else if(last_dimension == 4) {
					pixel_format = QB_PIXEL_2D_F64_4;
				}
			}
		}
	} else if(dimension_count == 2) {
		// allow either int32, representing each pixel (if the image is true color)
		// or int8, representing representing each pixel (if the image uses a palette )
		// or int8, float32, and float64, with the last dimension being one, three, or four (if the image is true color)
		if(true_color) {
			if(STORAGE_TYPE_MATCH(element_type, QB_TYPE_I32)) {
				pixel_format = QB_PIXEL_2D_I32_1;
			} else if(element_type == QB_TYPE_F32) {
				if(last_dimension == 1) {
					pixel_format = QB_PIXEL_1D_F32_1;
				} else if(last_dimension == 2) {
					pixel_format = QB_PIXEL_1D_F32_2;
				} else if(last_dimension == 3) {
					pixel_format = QB_PIXEL_1D_F32_3;
				} else if(last_dimension == 4) {
					pixel_format = QB_PIXEL_1D_F32_4;
				}
			} else if(element_type == QB_TYPE_F64) {
				if(last_dimension == 1) {
					pixel_format = QB_PIXEL_1D_F64_1;
				} else if(last_dimension == 2) {
					pixel_format = QB_PIXEL_1D_F64_2;
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
	} else if(dimension_count == 1) {
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

static int32_t qb_capture_dimensions_from_image(gdImagePtr image, qb_dimension_mappings *m, uint32_t dimension_index) {
	qb_pixel_format pixel_format = QB_PIXEL_INVALID;
	if(dimension_index < m->dst_dimension_count) {
		pixel_format = qb_get_compatible_pixel_format(m->dst_dimension_count - dimension_index, m->dst_dimensions[m->dst_dimension_count - 1], m->dst_element_type, image->trueColor);
	}
	if(pixel_format == QB_PIXEL_INVALID) {
		qb_report_invalid_variable_for_image_exception(0, m->dst_dimension_count, image->trueColor);
		return FALSE;
	}
	if(pixel_format & QB_PIXEL_ARRANGEMENT_2D) {
		uint32_t dimension1 = image->sy;
		uint32_t dimension2 = image->sx;
		if(dimension_index + 2 > MAX_DIMENSION) {
			qb_report_too_man_dimension_exception(0);
			return FALSE;
		}
		if(m->src_dimensions[dimension_index] < dimension1) {
			m->src_dimensions[dimension_index] = dimension1;
		}
		if(m->src_dimensions[dimension_index + 1] < dimension2) {
			m->src_dimensions[dimension_index + 1] = dimension2;
		}
		m->src_dimension_count = dimension_index + 2;
	} else {
		uint32_t dimension = image->sx * image->sy;
		if(dimension_index + 1 > MAX_DIMENSION) {
			qb_report_too_man_dimension_exception(0);
			return FALSE;
		}
		if(m->src_dimensions[dimension_index] < dimension) {
			m->src_dimensions[dimension_index] = dimension;
		}
		m->src_dimension_count = dimension_index + 1;
	}
	if(!STORAGE_TYPE_MATCH(m->dst_element_type, QB_TYPE_I32)) {
		// add the final dimension
		m->src_dimensions[m->src_dimension_count] = m->dst_dimensions[m->dst_dimension_count - 1];
		m->src_dimension_count++;
	}
	return TRUE;
}

static int32_t qb_reallocate_gd_image(gdImagePtr image, int width, int height) {
	int i, scanline_size, pixel_size;
	unsigned char ***p_scanlines;

	if(width <= 0 || height <= 0 || width > INT_MAX / sizeof(int) || height > INT_MAX / sizeof(void *)) {
		qb_report_gd_image_exception(0, width, height);
		return FALSE;
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
	return TRUE;
}

static void qb_copy_rgba_pixel_from_gd_image_scanline_F32(void *param1, void *param2, int param3) {
	float32_t *p = param1;
	int *tpixels = param2, tpixel;
	uint32_t width = (uint32_t) param3, i;

	for(i = 0; i < width; i++) {
		tpixel = tpixels[i];
		p[0] = ((float32_t) gdTrueColorGetRed(tpixel) * (1.0f / gdRedMax));
		p[1] = ((float32_t) gdTrueColorGetGreen(tpixel) * (1.0f / gdGreenMax));
		p[2] = ((float32_t) gdTrueColorGetBlue(tpixel) * (1.0f / gdBlueMax));
		p[3] = ((float32_t) (gdAlphaTransparent - gdTrueColorGetAlpha(tpixel)) * (1.0f / gdAlphaMax));
		p += 4;
	}
}

static void qb_copy_rgb_pixel_from_gd_image_scanline_F32(void *param1, void *param2, int param3) {
	float32_t *p = param1;
	int *tpixels = param2, tpixel;
	uint32_t width = (uint32_t) param3, i;

	for(i = 0; i < width; i++) {
		tpixel = tpixels[i];
		p[0] = ((float32_t) gdTrueColorGetRed(tpixel) * (1.0f / gdRedMax));
		p[1] = ((float32_t) gdTrueColorGetGreen(tpixel) * (1.0f / gdGreenMax));
		p[2] = ((float32_t) gdTrueColorGetBlue(tpixel) * (1.0f / gdBlueMax));
		p += 3;
	}
}

static void qb_copy_ya_pixel_from_gd_image_scanline_F32(void *param1, void *param2, int param3) {
	float32_t *p = param1;
	int *tpixels = param2, tpixel;
	uint32_t width = (uint32_t) param3, i;

	for(i = 0; i < width; i++) {
		tpixel = tpixels[i];
		p[0] = ((float32_t) gdTrueColorGetRed(tpixel) * (1.0f / gdRedMax)) * 0.299f
			 + ((float32_t) gdTrueColorGetGreen(tpixel) * (1.0f / gdGreenMax)) * 0.587f
			 + ((float32_t) gdTrueColorGetBlue(tpixel) * (1.0f / gdBlueMax)) * 0.114f;
		p[1] = ((float32_t) (gdAlphaTransparent - gdTrueColorGetAlpha(tpixel)) * (1.0f / gdAlphaMax));
		p += 2;
	}
}

static void qb_copy_y_pixel_from_gd_image_scanline_F32(void *param1, void *param2, int param3) {
	float32_t *p = param1;
	int *tpixels = param2, tpixel;
	uint32_t width = (uint32_t) param3, i;

	for(i = 0; i < width; i++) {
		tpixel = tpixels[i];
		p[0] = ((float32_t) gdTrueColorGetRed(tpixel) * (1.0f / gdRedMax)) * 0.299f
			 + ((float32_t) gdTrueColorGetGreen(tpixel) * (1.0f / gdGreenMax)) * 0.587f
			 + ((float32_t) gdTrueColorGetBlue(tpixel) * (1.0f / gdBlueMax)) * 0.114f;
		p += 1;
	}
}

static void qb_copy_rgba_pixel_from_gd_image_scanline_F64(void *param1, void *param2, int param3) {
	float64_t *p = param1;
	int *tpixels = param2, tpixel;
	uint32_t width = (uint32_t) param3, i;

	for(i = 0; i < width; i++) {
		tpixel = tpixels[i];
		p[0] = ((float64_t) gdTrueColorGetRed(tpixel) * (1.0 / gdRedMax));
		p[1] = ((float64_t) gdTrueColorGetGreen(tpixel) * (1.0 / gdGreenMax));
		p[2] = ((float64_t) gdTrueColorGetBlue(tpixel) * (1.0 / gdBlueMax));
		p[3] = ((float64_t) (gdAlphaTransparent - gdTrueColorGetAlpha(tpixel)) * (1.0 / gdAlphaMax));
		p += 4;
	}
}

static void qb_copy_rgb_pixel_from_gd_image_scanline_F64(void *param1, void *param2, int param3) {
	float64_t *p = param1;
	int *tpixels = param2, tpixel;
	uint32_t width = (uint32_t) param3, i;

	for(i = 0; i < width; i++) {
		tpixel = tpixels[i];
		p[0] = ((float64_t) gdTrueColorGetRed(tpixel) * (1.0 / gdRedMax));
		p[1] = ((float64_t) gdTrueColorGetGreen(tpixel) * (1.0 / gdGreenMax));
		p[2] = ((float64_t) gdTrueColorGetBlue(tpixel) * (1.0 / gdBlueMax));
		p += 3;
	}
}

static void qb_copy_ya_pixel_from_gd_image_scanline_F64(void *param1, void *param2, int param3) {
	float64_t *p = param1;
	int *tpixels = param2, tpixel;
	uint32_t width = (uint32_t) param3, i;

	for(i = 0; i < width; i++) {
		tpixel = tpixels[i];
		p[0] = ((float64_t) gdTrueColorGetRed(tpixel) * (1.0 / gdRedMax)) * 0.299
			 + ((float64_t) gdTrueColorGetGreen(tpixel) * (1.0 / gdGreenMax)) * 0.587
			 + ((float64_t) gdTrueColorGetBlue(tpixel) * (1.0 / gdBlueMax)) * 0.114;
		p[1] = ((float64_t) (gdAlphaTransparent - gdTrueColorGetAlpha(tpixel)) * (1.0 / gdAlphaMax));
		p += 2;
	}
}

static void qb_copy_y_pixel_from_gd_image_scanline_F64(void *param1, void *param2, int param3) {
	float64_t *p = param1;
	int *tpixels = param2, tpixel;
	uint32_t width = (uint32_t) param3, i;

	for(i = 0; i < width; i++) {
		tpixel = tpixels[i];
		p[0] = ((float64_t) gdTrueColorGetRed(tpixel) * (1.0 / gdRedMax)) * 0.299
			 + ((float64_t) gdTrueColorGetGreen(tpixel) * (1.0 / gdGreenMax)) * 0.587
			 + ((float64_t) gdTrueColorGetBlue(tpixel) * (1.0 / gdBlueMax)) * 0.114;
		p += 1;
	}
}

static int32_t qb_copy_elements_from_gd_image(gdImagePtr image, int8_t *dst_memory, qb_dimension_mappings *m, uint32_t dimension_index) {
	uint32_t i, j;
	qb_pixel_format pixel_format = qb_get_compatible_pixel_format(m->dst_dimension_count - dimension_index, m->dst_dimensions[m->dst_dimension_count - 1], m->dst_element_type, image->trueColor);
	qb_pixel_format pixel_type = pixel_format & ~QB_PIXEL_ARRANGEMENT_FLAGS;
	qb_pixel_format pixel_arrangement = pixel_format & QB_PIXEL_ARRANGEMENT_FLAGS;
	uint32_t dst_height = (pixel_arrangement == QB_PIXEL_ARRANGEMENT_2D) ? m->dst_dimensions[dimension_index] : image->sy;
	uint32_t dst_width = (pixel_arrangement == QB_PIXEL_ARRANGEMENT_2D) ? m->dst_dimensions[dimension_index + 1] : image->sx;
	uint32_t dst_pixel_count = dst_height * dst_width;
	uint32_t src_pixel_count = image->sy * image->sx;

	if(image->trueColor) {
		qb_thread_proc proc = NULL;
		uint32_t pixel_size;

		switch(pixel_type) {
			case QB_PIXEL_I08_4:
			case QB_PIXEL_I32_1: {
				int32_t *p = (int32_t *) dst_memory;

				for(i = 0; i < (uint32_t) image->sy; i++) {
					for(j = 0; j < (uint32_t) image->sx; j++) {
						p[0] = gdImageTrueColorPixel(image, j, i);
						p += 1;
					}
					if((uint32_t) image->sx < dst_width) {
						memset(p, 0, (dst_width - image->sx) * sizeof(int32_t));
						p += (dst_width - image->sx);
					}
				}
				if(src_pixel_count < dst_pixel_count) {
					memset(p, 0, (dst_pixel_count - src_pixel_count) * sizeof(int32_t));
				}
			}	break;
			case QB_PIXEL_F32_4: {
				proc = qb_copy_rgba_pixel_from_gd_image_scanline_F32;
				pixel_size = sizeof(float32_t) * 4;
			}	break;
			case QB_PIXEL_F32_3: {
				proc = qb_copy_rgb_pixel_from_gd_image_scanline_F32;
				pixel_size = sizeof(float32_t) * 3;
			}	break;
			case QB_PIXEL_F32_2: {
				proc = qb_copy_ya_pixel_from_gd_image_scanline_F32;
				pixel_size = sizeof(float32_t) * 2;
			}	break;
			case QB_PIXEL_F32_1: {
				proc = qb_copy_y_pixel_from_gd_image_scanline_F32;
				pixel_size = sizeof(float32_t) * 1;
			}	break;
			case QB_PIXEL_F64_4: {
				proc = qb_copy_rgba_pixel_from_gd_image_scanline_F64;
				pixel_size = sizeof(float64_t) * 4;
			}	break;
			case QB_PIXEL_F64_3: {
				proc = qb_copy_rgb_pixel_from_gd_image_scanline_F64;
				pixel_size = sizeof(float64_t) * 3;
			}	break;
			case QB_PIXEL_F64_2: {
				proc = qb_copy_ya_pixel_from_gd_image_scanline_F64;
				pixel_size = sizeof(float64_t) * 2;
			}	break;
			case QB_PIXEL_F64_1: {
				proc = qb_copy_y_pixel_from_gd_image_scanline_F64;
				pixel_size = sizeof(float64_t) * 1;
			}	break;
			default: {
			}	break;
		}

		if(proc) {
			int8_t *p = dst_memory;
			qb_task_group _group, *group = &_group;
			qb_initialize_task_group(group, image->sy, 0);
			for(i = 0; i < (uint32_t) image->sy; i++) {
				qb_add_task(group, proc, p, image->tpixels[i], image->sx);
				p += image->sx * pixel_size;
				if((uint32_t) image->sx < dst_width) {
					memset(p, 0, (dst_width - image->sx) * pixel_size);
					p += (dst_width - image->sx) * pixel_size;
				}
			}
			if(src_pixel_count < dst_pixel_count) {
				memset(p, 0, (dst_pixel_count - src_pixel_count) * pixel_size);
			}
			qb_run_task_group(group, FALSE);
			qb_free_task_group(group);
		}
	} else {
		switch(pixel_type) {
			case QB_PIXEL_I08_1: {
				int8_t *p = dst_memory;

				for(i = 0; i < (uint32_t) image->sy; i++) {
					for(j = 0; j < (uint32_t) image->sx; j++) {
						p[0] = gdImagePalettePixel(image, j, i);
						p += 1;
					}
					if((uint32_t) image->sx < dst_width) {
						memset(p, 0, (dst_width - image->sx) * sizeof(int8_t));
						p += (dst_width - image->sx);
					}
				}
				if(src_pixel_count < dst_pixel_count) {
					memset(p, 0, (dst_pixel_count - src_pixel_count) * sizeof(int8_t));
				}
			}	break;
			default: {
			}	break;
		}
	}
	return TRUE;
}

static void qb_copy_rgba_pixel_to_gd_image_scanline_F32(void *param1, void *param2, int param3) {
	float32_t *p = param1;
	int *tpixels = param2;
	uint32_t width = (uint32_t) param3, i;
	int r, g, b, a;

	for(i = 0; i < width; i++) {
		r = qb_clamp_float32(p[0], gdRedMax);
		g = qb_clamp_float32(p[1], gdGreenMax);
		b = qb_clamp_float32(p[2], gdBlueMax);
		a = gdAlphaTransparent - qb_clamp_float32(p[3], gdAlphaMax);
		tpixels[i] = gdTrueColorAlpha(r, g, b, a);
		p += 4;
	}
}

static void qb_copy_rgb_pixel_to_gd_image_scanline_F32(void *param1, void *param2, int param3) {
	float32_t *p = param1;
	int *tpixels = param2;
	uint32_t width = (uint32_t) param3, i;
	int r, g, b;

	for(i = 0; i < width; i++) {
		r = qb_clamp_float32(p[0], gdRedMax);
		g = qb_clamp_float32(p[1], gdGreenMax);
		b = qb_clamp_float32(p[2], gdBlueMax);
		tpixels[i] = gdTrueColorAlpha(r, g, b, gdAlphaOpaque);
		p += 3;
	}
}

static void qb_copy_ya_pixel_to_gd_image_scanline_F32(void *param1, void *param2, int param3) {
	float32_t *p = param1;
	int *tpixels = param2;
	uint32_t width = (uint32_t) param3, i;
	int r, a;

	for(i = 0; i < width; i++) {
		r = qb_clamp_float32(p[0], gdRedMax);
		a = gdAlphaTransparent - qb_clamp_float32(p[1], gdAlphaMax);
		tpixels[i] = gdTrueColorAlpha(r, r, r, a);
		p += 2;
	}
}

static void qb_copy_y_pixel_to_gd_image_scanline_F32(void *param1, void *param2, int param3) {
	float32_t *p = param1;
	int *tpixels = param2;
	uint32_t width = (uint32_t) param3, i;
	int r;

	for(i = 0; i < width; i++) {
		r = qb_clamp_float32(p[0], gdRedMax);
		tpixels[i] = gdTrueColorAlpha(r, r, r, gdAlphaOpaque);
		p += 1;
	}
}

static void qb_copy_rgba_pixel_to_gd_image_scanline_F64(void *param1, void *param2, int param3) {
	float64_t *p = param1;
	int *tpixels = param2;
	uint32_t width = (uint32_t) param3, i;
	int r, g, b, a;

	for(i = 0; i < width; i++) {
		r = qb_clamp_float64(p[0], gdRedMax);
		g = qb_clamp_float64(p[1], gdGreenMax);
		b = qb_clamp_float64(p[2], gdBlueMax);
		a = gdAlphaTransparent - qb_clamp_float64(p[3], gdAlphaMax);
		tpixels[i] = gdTrueColorAlpha(r, g, b, a);
		p += 4;
	}
}

static void qb_copy_rgb_pixel_to_gd_image_scanline_F64(void *param1, void *param2, int param3) {
	float64_t *p = param1;
	int *tpixels = param2;
	uint32_t width = (uint32_t) param3, i;
	int r, g, b;

	for(i = 0; i < width; i++) {
		r = qb_clamp_float64(p[0], gdRedMax);
		g = qb_clamp_float64(p[1], gdGreenMax);
		b = qb_clamp_float64(p[2], gdBlueMax);
		tpixels[i] = gdTrueColorAlpha(r, g, b, gdAlphaOpaque);
		p += 3;
	}
}

static void qb_copy_ya_pixel_to_gd_image_scanline_F64(void *param1, void *param2, int param3) {
	float64_t *p = param1;
	int *tpixels = param2;
	uint32_t width = (uint32_t) param3, i;
	int r, a;

	for(i = 0; i < width; i++) {
		r = qb_clamp_float64(p[0], gdRedMax);
		a = gdAlphaTransparent - qb_clamp_float64(p[1], gdAlphaMax);
		tpixels[i] = gdTrueColorAlpha(r, r, r, a);
		p += 2;
	}
}

static void qb_copy_y_pixel_to_gd_image_scanline_F64(void *param1, void *param2, int param3) {
	float64_t *p = param1;
	int *tpixels = param2;
	uint32_t width = (uint32_t) param3, i;
	int r;

	for(i = 0; i < width; i++) {
		r = qb_clamp_float64(p[0], gdRedMax);
		tpixels[i] = gdTrueColorAlpha(r, r, r, gdAlphaOpaque);
		p += 1;
	}
}

static int32_t qb_copy_elements_to_gd_image(int8_t *src_memory, gdImagePtr image, qb_dimension_mappings *m, uint32_t dimension_index) {
	uint32_t i, j;
	qb_pixel_format pixel_format = qb_get_compatible_pixel_format(m->src_dimension_count - dimension_index, m->src_dimensions[m->src_dimension_count - 1], m->src_element_type, image->trueColor);
	qb_pixel_format pixel_type = pixel_format & ~QB_PIXEL_ARRANGEMENT_FLAGS;
	qb_pixel_format pixel_arrangement = pixel_format & QB_PIXEL_ARRANGEMENT_FLAGS;
	uint32_t src_pixel_count;
	uint32_t dst_width, dst_height;

	if(pixel_arrangement == QB_PIXEL_ARRANGEMENT_1D) {
		src_pixel_count = m->src_dimensions[dimension_index];
		if(src_pixel_count >= (uint32_t) image->sx) {
			dst_width = image->sx;
			dst_height = src_pixel_count / dst_width;
		} else {
			dst_width = 0;
			dst_height = 0;
		}
	} else {
		dst_height = m->src_dimensions[dimension_index];
		dst_width = m->src_dimensions[dimension_index + 1];
	}
	if(image->sy != dst_height || image->sx != dst_width) {
		if(!qb_reallocate_gd_image(image, dst_width, dst_height)) {
			return FALSE;
		}
	}

	if(image->trueColor) {
		qb_thread_proc proc = NULL;
		uint32_t pixel_size;

		switch(pixel_type) {
			case QB_PIXEL_I08_4:
			case QB_PIXEL_I32_1: {
				int32_t *p = (int32_t *) src_memory;
				int tpixel;
				for(i = 0; i < (uint32_t) image->sy; i++) {
					for(j = 0; j < (uint32_t) image->sx; j++) {
						tpixel = *p++;
						gdImageTrueColorPixel(image, j, i) = tpixel;
					}
				}
			}	break;
			case QB_PIXEL_F32_4: {
				proc = qb_copy_rgba_pixel_to_gd_image_scanline_F32;
				pixel_size = sizeof(float32_t) * 4;
			}	break;
			case QB_PIXEL_F32_3: {
				proc = qb_copy_rgb_pixel_to_gd_image_scanline_F32;
				pixel_size = sizeof(float32_t) * 3;
			}	break;
			case QB_PIXEL_F32_2: {
				proc = qb_copy_ya_pixel_to_gd_image_scanline_F32;
				pixel_size = sizeof(float32_t) * 2;
			}	break;
			case QB_PIXEL_F32_1: {
				proc = qb_copy_y_pixel_to_gd_image_scanline_F32;
				pixel_size = sizeof(float32_t) * 1;
			}	break;
			case QB_PIXEL_F64_4: {
				proc = qb_copy_rgba_pixel_to_gd_image_scanline_F64;
				pixel_size = sizeof(float64_t) * 4;
			}	break;
			case QB_PIXEL_F64_3: {
				proc = qb_copy_rgb_pixel_to_gd_image_scanline_F64;
				pixel_size = sizeof(float64_t) * 3;
			}	break;
			case QB_PIXEL_F64_2: {
				proc = qb_copy_ya_pixel_to_gd_image_scanline_F64;
				pixel_size = sizeof(float64_t) * 2;
			}	break;
			case QB_PIXEL_F64_1: {
				proc = qb_copy_y_pixel_to_gd_image_scanline_F64;
				pixel_size = sizeof(float64_t) * 1;
			}	break;
			default: {
			}	break;
		}

		if(proc) {
			TSRMLS_FETCH();
			int8_t *p = src_memory;
			qb_task_group _group, *group = &_group;
			qb_initialize_task_group(group, image->sy, 0);
			for(i = 0; i < (uint32_t) image->sy; i++) {
				qb_add_task(group, proc, p, image->tpixels[i], image->sx);
				p += image->sx * pixel_size;
			}
			qb_run_task_group(group, FALSE);
			qb_free_task_group(group);
		}
	} else {
		switch(pixel_type) {
			case QB_PIXEL_I08_1: {
				int8_t *p = src_memory;
				unsigned char pixel;
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
	return TRUE;
}

static gdImagePtr gdImageCreateTrueColor(uint32_t sx, uint32_t sy) {
	uint32_t i;
	gdImagePtr im;

	if(sx * sy > INT32_MAX || sizeof(unsigned char*) * sy > INT32_MAX || sizeof(int) * sx > INT32_MAX) {
		return NULL;
	}

	im = (gdImage *) emalloc(sizeof(gdImage));
	memset(im, 0, sizeof(gdImage));
	im->tpixels = (int **) emalloc(sizeof(int *) * sy);
	im->AA_opacity = (unsigned char **) emalloc(sizeof(unsigned char *) * sy);
	for(i = 0; i < sy; i++) {
		im->tpixels[i] = (int *) ecalloc(sx, sizeof(int));
		im->AA_opacity[i] = (unsigned char *) ecalloc(sx, sizeof(unsigned char));
	}
	im->sx = sx;
	im->sy = sy;
	im->transparent = (-1);
	im->trueColor = 1;
	im->saveAlphaFlag = 0;
	im->alphaBlendingFlag = 1;
	im->thick = 1;
	im->cx2 = im->sx - 1;
	im->cy2 = im->sy - 1;
	im->interpolation_id = GD_BILINEAR_FIXED;
	return im;
}

static int32_t qb_initialize_zval_image(zval *zimage, qb_dimension_mappings *m, uint32_t dimension_index) {	
	uint32_t height = m->src_dimensions[dimension_index];
	uint32_t width = m->src_dimensions[dimension_index + 1];
	int le_gd = qb_get_gd_id();
	gdImagePtr im = (le_gd != 0) ? gdImageCreateTrueColor(width, height) : NULL;
	if(!im) {
		qb_report_gd_image_exception(0, width, height);
		return FALSE;
	}
	ZEND_REGISTER_RESOURCE(zimage, im, le_gd);
	return TRUE;
}
