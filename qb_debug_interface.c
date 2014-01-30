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

int qb_debug_set_compatibility_mode(int enable) {
	debug_compatibility_mode = enable;
	return TRUE;
}

static qb_function *qb_get_active_function(qb_function *qfunc) {
	qb_function *active = qfunc, *f;
	for(f = qfunc; f; f = f->next_reentrance_copy) {
		if(f->in_use) {
			active = f;
		}
	}
	for(f = active; f; f = f->next_forked_copy) {
		if(f->in_use) {
			TSRMLS_FETCH();
			if(f->current_fork_id == QB_G(debug_fork_id)) {
				active = f;
				break;
			}
		}
	}
	return active;
}

static qb_variable * qb_find_local_variable(qb_function *qfunc, const char *name) {
	uint32_t i;
	if(qfunc && name) {
		for(i = 0; i < qfunc->variable_count; i++) {
			qb_variable *qvar = qfunc->variables[i];
			if(!(qvar->flags & (QB_VARIABLE_CLASS | QB_VARIABLE_CLASS_INSTANCE | QB_VARIABLE_CLASS_CONSTANT | QB_VARIABLE_GLOBAL | QB_VARIABLE_RETURN_VALUE))) {
				if(strcmp(qvar->name, name) == 0) {
					return qvar;
				}
			}
		}
	}
	return NULL;
}

void qb_debug_free_variable_details(qb_debug_variable_details *details) {
	if(details) {
		free(details);
	}
}

static int qb_get_varaible_details(qb_storage *storage, qb_variable *qvar, qb_debug_variable_details **p_details) {
	qb_debug_variable_details *details = NULL;
	if(qvar) {
		details = malloc(sizeof(qb_debug_variable_details));
		if(details) {
			uint32_t i, element_count;
			qb_address *address = qvar->address;
			details->bitness = BYTE_COUNT(8, address->type);
			details->flags = 0;
			if(address->type & QB_TYPE_UNSIGNED) {
				details->flags |= QB_VAR_IS_UNSIGNED;
			}
			if(address->type > QB_TYPE_F32) {
				details->flags |= QB_VAR_IS_FLOAT;
			} else {
				details->flags |= QB_VAR_IS_INTEGER;
			}
			if(address->flags & QB_ADDRESS_STRING) {
				details->flags |= QB_VAR_IS_STRING;
			} else if(address->flags & QB_ADDRESS_BOOLEAN) {
				details->flags |= QB_VAR_IS_BOOL;
			} else if(address->flags & QB_ADDRESS_IMAGE) {
				details->flags |= QB_VAR_IS_IMAGE;
			} else {
				if(qvar->value) {
					gdImagePtr image = qb_get_gd_image(qvar->value);
					if(image && image->trueColor) {
						details->flags |= QB_VAR_IS_IMAGE;
					}
				}
			}
			details->dimension_count = address->dimension_count;
			for(i = 0; i < QB_MAXIMUM_DIMENSION_COUNT; i++) {
				if(i < details->dimension_count) {
					details->dimensions[i] = VALUE_IN(storage, U32, address->dimension_addresses[i]);
				} else {
					details->dimensions[i] = 0;
				}
			}
			if(details->dimension_count > 0) {
				element_count = VALUE_IN(storage, U32, address->array_size_address);
			} else {
				element_count = 0;
			}
			details->byte_count = BYTE_COUNT(element_count, address->type);
			details->zend_variable = qvar->value;
			details->data = ARRAY_IN(storage, I08, address);
		}
	}
	if(p_details) {
		*p_details = details;
	} else {
		qb_debug_free_variable_details(details);
	}
	return (details != NULL);
}

int qb_debug_get_local_variable_details(void *func_ptr, const char *var_name, qb_debug_variable_details **p_details) {
	qb_function *qfunc = qb_get_active_function(func_ptr);
	qb_variable *qvar = qb_find_local_variable(qfunc, var_name);
	return qb_get_varaible_details(qfunc->local_storage, qvar, p_details);
}

static qb_variable * qb_find_variable_in_scope(qb_import_scope *scope, const char *var_name) {
	if(scope && var_name) {
		uint32_t i;
		for(i = 0; i < scope->variable_count; i++) {
			qb_variable *ivar = scope->variables[i];
			if(strcmp(ivar->name, var_name) == 0) {
				return ivar;
			}
		}
	}
	return NULL;
}

int qb_debug_get_instance_variable_details(zval *obj, const char *var_name, qb_debug_variable_details **p_details) {
	TSRMLS_FETCH();
	if(obj) {
		qb_import_scope *scope = qb_find_import_scope(QB_IMPORT_SCOPE_OBJECT, obj TSRMLS_CC);
		if(scope) {
			qb_variable *ivar = qb_find_variable_in_scope(scope, var_name);
			return qb_get_varaible_details(scope->storage, ivar, p_details);
		}
	}
	return FALSE;

}

int qb_debug_get_class_variable_details(zend_class_entry *ce, const char *var_name, qb_debug_variable_details **p_details) {
	TSRMLS_FETCH();
	if(ce) {
		qb_import_scope *scope = qb_find_import_scope(QB_IMPORT_SCOPE_CLASS, ce TSRMLS_CC);
		if(scope) {
			qb_variable *ivar = qb_find_variable_in_scope(scope, var_name);
			return qb_get_varaible_details(scope->storage, ivar, p_details);
		}
	}
	return FALSE;
}

int qb_debug_get_global_variable_details(const char *var_name, qb_debug_variable_details **p_details) {
	qb_import_scope *scope;
	TSRMLS_FETCH();
	scope = qb_find_import_scope(QB_IMPORT_SCOPE_GLOBAL, &EG(symbol_table) TSRMLS_CC);
	if(scope) {
		qb_variable *ivar = qb_find_variable_in_scope(scope, var_name);
		return qb_get_varaible_details(scope->storage, ivar, p_details);
	}
	return FALSE;
}

void qb_debug_free_bitmap(qb_debug_bitmap *bitmap) {
	if(bitmap) {
		if(bitmap->scanlines) {
			uint32_t i;
			for(i = 0; i < bitmap->height; i++) {
				if(bitmap->scanlines[i]) {
					free(bitmap->scanlines[i]);
				}
			}
			free(bitmap->scanlines);
		}
		free(bitmap);
	}
}

static void qb_copy_rgba_pixel_to_scanline_F32(void *scanline, int *tpixels, uint32_t width) {
	float32_t *p = scanline;
	uint32_t i;
	int r, g, b, a;

	for(i = 0; i < width; i++) {
		r = qb_clamp_float32(p[0], gdRedMax);
		g = qb_clamp_float32(p[1], gdGreenMax);
		b = qb_clamp_float32(p[2], gdBlueMax);
		a = qb_clamp_float32(p[3], 255);
		tpixels[i] = gdTrueColorAlpha(r, g, b, a);
		p += 4;
	}
}

static void qb_copy_rgb_pixel_to_scanline_F32(void *scanline, int *tpixels, uint32_t width) {
	float32_t *p = scanline;
	uint32_t i;
	int r, g, b;

	for(i = 0; i < width; i++) {
		r = qb_clamp_float32(p[0], gdRedMax);
		g = qb_clamp_float32(p[1], gdGreenMax);
		b = qb_clamp_float32(p[2], gdBlueMax);
		tpixels[i] = gdTrueColorAlpha(r, g, b, 255);
		p += 3;
	}
}

static void qb_copy_ya_pixel_to_scanline_F32(void *scanline, int *tpixels, uint32_t width) {
	float32_t *p = scanline;
	uint32_t i;
	int r, a;

	for(i = 0; i < width; i++) {
		r = qb_clamp_float32(p[0], gdRedMax);
		a = gdAlphaTransparent - qb_clamp_float32(p[1], 255);
		tpixels[i] = gdTrueColorAlpha(r, r, r, a);
		p += 2;
	}
}

static void qb_copy_y_pixel_to_scanline_F32(void *scanline, int *tpixels, uint32_t width) {
	float32_t *p = scanline;
	uint32_t i;
	int r;
	for(i = 0; i < width; i++) {
		r = qb_clamp_float32(p[0], gdRedMax);
		tpixels[i] = gdTrueColorAlpha(r, r, r, 255);
		p += 1;
	}
}

static void qb_copy_rgba_pixel_to_scanline_F64(void *scanline, int *tpixels, uint32_t width) {
	float64_t *p = scanline;
	uint32_t i;
	int r, g, b, a;
	for(i = 0; i < width; i++) {
		r = qb_clamp_float64(p[0], gdRedMax);
		g = qb_clamp_float64(p[1], gdGreenMax);
		b = qb_clamp_float64(p[2], gdBlueMax);
		a = qb_clamp_float64(p[3], 255);
		tpixels[i] = gdTrueColorAlpha(r, g, b, a);
		p += 4;
	}
}

static void qb_copy_rgb_pixel_to_scanline_F64(void *scanline, int *tpixels, uint32_t width) {
	float64_t *p = scanline;
	uint32_t i;
	int r, g, b;
	for(i = 0; i < width; i++) {
		r = qb_clamp_float64(p[0], gdRedMax);
		g = qb_clamp_float64(p[1], gdGreenMax);
		b = qb_clamp_float64(p[2], gdBlueMax);
		tpixels[i] = gdTrueColorAlpha(r, g, b, 255);
		p += 3;
	}
}

static void qb_copy_ya_pixel_to_scanline_F64(void *scanline, int *tpixels, uint32_t width) {
	float64_t *p = scanline;
	uint32_t i;
	int r, a;
	for(i = 0; i < width; i++) {
		r = qb_clamp_float64(p[0], gdRedMax);
		a = gdAlphaTransparent - qb_clamp_float64(p[1], 255);
		tpixels[i] = gdTrueColorAlpha(r, r, r, a);
		p += 2;
	}
}

static void qb_copy_y_pixel_to_scanline_F64(void *scanline, int *tpixels, uint32_t width) {
	float64_t *p = scanline;
	uint32_t i;
	int r;
	for(i = 0; i < width; i++) {
		r = qb_clamp_float64(p[0], gdRedMax);
		tpixels[i] = gdTrueColorAlpha(r, r, r, 255);
		p += 1;
	}
}

static void qb_copy_rgba_pixel_to_scanline_U08(void *scanline, int *tpixels, uint32_t width) {
	uint8_t *p = scanline;
	uint32_t i;
	int r, g, b, a;
	for(i = 0; i < width; i++) {
		r = p[3];
		g = p[2];
		b = p[1];
		a = (int) (((float32_t) p[0]) * (255.0f / 127.0f));
		tpixels[i] = gdTrueColorAlpha(r, g, b, a);
		p += 4;
	}
}

static void qb_copy_pixels(void *data, uint32_t width, uint32_t height, uint32_t channel_count, int32_t bitness, int32_t is_float, qb_debug_bitmap *bitmap) {
	void (*f)(void *, int *, uint32_t) = NULL;
	int8_t *p = data;
	uint32_t scanline_byte_count = width * (bitness / 8) * channel_count;
	if(is_float) {
		switch(channel_count) {
			case 4: f = (bitness == 32) ? qb_copy_rgba_pixel_to_scanline_F32 : qb_copy_rgba_pixel_to_scanline_F64; break;
			case 3: f = (bitness == 32) ? qb_copy_rgb_pixel_to_scanline_F32 : qb_copy_rgb_pixel_to_scanline_F64; break;
			case 2: f = (bitness == 32) ? qb_copy_ya_pixel_to_scanline_F32 : qb_copy_ya_pixel_to_scanline_F64; break;
			case 1: f = (bitness == 32) ? qb_copy_y_pixel_to_scanline_F32 : qb_copy_y_pixel_to_scanline_F64; break;
		}
	} else {
		f = qb_copy_rgba_pixel_to_scanline_U08;
	}
	if(f) {
		uint32_t i;
		for(i = 0; i < height; i++) {
			f(p, bitmap->scanlines[i], width);
			p += scanline_byte_count;
		}
	}
}

int qb_debug_get_bitmap(qb_debug_variable_details *details, qb_debug_bitmap **p_bitmap) {
	qb_debug_bitmap *bitmap = NULL;
	uint32_t width = 0, height = 0, pixel_count = 0, channel_count = 0;
	if(details->flags & QB_VAR_IS_INTEGER && details->bitness == 32) {
		if(details->dimension_count == 2) {
			height = details->dimensions[0];
			width = details->dimensions[1];
		} else if(details->dimension_count == 1) {
			pixel_count = details->dimensions[0];
		}
		channel_count = 1;
	} else {
		if(details->dimension_count == 3) {
			height = details->dimensions[0];
			width = details->dimensions[1];
			channel_count = details->dimensions[2];
		} else if(details->dimension_count == 2) {
			pixel_count = details->dimensions[0];
			channel_count = details->dimensions[1];
		}
	}
	if(pixel_count != 0) {
		// get the image width from the original gd image
		if(details->zend_variable) {
			gdImagePtr image = qb_get_gd_image(details->zend_variable);
			width = image->sx;
			height = pixel_count / width;
		}
	}
	if(width != 0 && height != 0) {
		// allocate memory
		qb_debug_bitmap *bitmap = calloc(1, sizeof(qb_debug_bitmap));
		if(bitmap) {
			bitmap->scanlines = calloc(height, sizeof(int *));
			if(bitmap->scanlines) {
				uint32_t i;
				for(i = 0; i < height; i++) {
					int *scanline = calloc(width, sizeof(int));
					if(scanline) {
						bitmap->scanlines[i] = scanline;
					} else {
						qb_debug_free_bitmap(bitmap);
						bitmap = NULL;
					}
				}
			} else {
				qb_debug_free_bitmap(bitmap);
				bitmap = NULL;
			}
		}
		if(bitmap) {
			// copy pixels into the bitmap
			qb_copy_pixels(details->data, width, height, channel_count, details->bitness, details->flags & QB_VAR_IS_FLOAT, bitmap);
		}
	}
	if(p_bitmap) {
		*p_bitmap = bitmap;
	} else {
		qb_debug_free_bitmap(bitmap);
	}
	return (bitmap != NULL);
}

qb_debug_interface debug_interface = {
	QB_INTERFACE_SIGNATURE,
	sizeof(qb_debug_interface),
	qb_debug_set_compatibility_mode,
	qb_debug_free_bitmap,
	qb_debug_get_bitmap,
	qb_debug_free_variable_details,
	qb_debug_get_local_variable_details,
	qb_debug_get_instance_variable_details,
	qb_debug_get_class_variable_details,
	qb_debug_get_global_variable_details,
};
