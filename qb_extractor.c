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

#include "qb.h"

static zval * qb_add_string(zval *array, const char *name, const char *s, int32_t len) {
	HashTable *ht = Z_ARRVAL_P(array);
	zval *zvalue;
	ALLOC_INIT_ZVAL(zvalue);
	if(s) {
		if(len == -1) {
			len = (int32_t) strlen(s);
		}
		ZVAL_STRINGL(zvalue, s, len, TRUE);
	}
	if(name) {
		zend_hash_update(ht, name, (uint32_t) strlen(name) + 1, &zvalue, sizeof(zval *), NULL);
	} else {
		zend_hash_next_index_insert(ht, &zvalue, sizeof(zval *), NULL);
	}
	return zvalue;
}

static zval * qb_add_int(zval *array, const char *name, long value) {
	HashTable *ht = Z_ARRVAL_P(array);
	zval *zvalue;
	ALLOC_INIT_ZVAL(zvalue);
	ZVAL_LONG(zvalue, value);
	if(name) {
		zend_hash_update(ht, name, (uint32_t) strlen(name) + 1, &zvalue, sizeof(zval *), NULL);
	} else {
		zend_hash_next_index_insert(ht, &zvalue, sizeof(zval *), NULL);
	}
	return zvalue;
}

static zval * qb_add_float(zval *array, const char *name, double value) {
	HashTable *ht = Z_ARRVAL_P(array);
	zval *zvalue;
	ALLOC_INIT_ZVAL(zvalue);
	ZVAL_DOUBLE(zvalue, value);
	if(name) {
		zend_hash_update(ht, name, (uint32_t) strlen(name) + 1, &zvalue, sizeof(zval *), NULL);
	} else {
		zend_hash_next_index_insert(ht, &zvalue, sizeof(zval *), NULL);
	}
	return zvalue;
}

static zval * qb_add_array(zval *array, const char *name) {
	HashTable *ht = Z_ARRVAL_P(array);
	zval *zvalue;
	ALLOC_INIT_ZVAL(zvalue);
	array_init(zvalue);
	if(name) {
		zend_hash_update(ht, name, (uint32_t) strlen(name) + 1, &zvalue, sizeof(zval *), NULL);
	} else {
		zend_hash_next_index_insert(ht, &zvalue, sizeof(zval *), NULL);
	}
	return zvalue;
}

static zval * qb_add_float_array(zval *array, const char *name, float32_t *values, uint32_t count) {
	zval *container = qb_add_array(array, name);
	uint32_t i;
	for(i = 0; i < count; i++) {
		qb_add_float(container, NULL, values[i]);
	}
	return container;
}

static zval * qb_add_int_array(zval *array, const char *name, int32_t *values, uint32_t count) {
	zval *container = qb_add_array(array, name);
	uint32_t i;
	for(i = 0; i < count; i++) {
		qb_add_int(container, NULL, values[i]);
	}
	return container;
}

const char *pbj_type_names[] = {
	"",
	"float",
	"float2",
	"float3",
	"float4",
	"float2x2",
	"float3x3",
	"float4x4",
	"int",
	"int2",
	"int3",
	"int4",
	"string",
	"bool",
	"bool2",
	"bool3",
	"bool4",
};

const char *pbj_texture_type_names[] = {
	"",
	"image1",
	"image2",
	"image3",
	"image4",
};

const char *pbj_texture_qb_types[] = {
	"",
	"image1",
	"image2",
	"image3",
	"image",
};

const char *pbj_param_qb_types[] = {
	"",
	"float32",
	"float32[2]",
	"float32[3]",
	"float32[4]",
	"float32[2][2]",
	"float32[3][3]",
	"float32[4][4]",
	"int32",
	"int32[2]",
	"int32[3]",
	"int32[4]",
	"string",
	"bool",
	"bool[2]",
	"bool[3]",
	"bool[4]",
};

static zval * qb_add_pbj_value(zval *array, const char *name, qb_pbj_value *value) {
	switch(value->type) {
		case PBJ_TYPE_FLOAT:	return qb_add_float(array, name, value->float1);
		case PBJ_TYPE_FLOAT2:	return qb_add_float_array(array, name, value->float2, 2);
		case PBJ_TYPE_FLOAT3:	return qb_add_float_array(array, name, value->float3, 3);
		case PBJ_TYPE_FLOAT4:	return qb_add_float_array(array, name, value->float4, 4);
		case PBJ_TYPE_FLOAT2X2:	return qb_add_float_array(array, name, value->float2x2, 4);
		case PBJ_TYPE_FLOAT3X3:	return qb_add_float_array(array, name, value->float3x3, 9);
		case PBJ_TYPE_FLOAT4X4:	return qb_add_float_array(array, name, value->float4x4, 16);
		case PBJ_TYPE_BOOL:
		case PBJ_TYPE_INT:		return qb_add_int(array, name, value->int1);
		case PBJ_TYPE_BOOL2:
		case PBJ_TYPE_INT2:		return qb_add_int_array(array, name, value->int2, 2);
		case PBJ_TYPE_BOOL3:
		case PBJ_TYPE_INT3:		return qb_add_int_array(array, name, value->int3, 3);
		case PBJ_TYPE_BOOL4:
		case PBJ_TYPE_INT4:		return qb_add_int_array(array, name, value->int4, 4);
		case PBJ_TYPE_STRING:	return qb_add_string(array, name, value->string, -1);
		default:				return qb_add_string(array, name, NULL, 0);
	}
}

ZEND_ATTRIBUTE_FORMAT(printf, 2, 3)
static void qb_append_string(zval *string, const char *format, ...) {
	uint32_t current_len = Z_STRLEN_P(string), len;
	char *addition;
	va_list args;

	va_start(args, format);
	len = vspprintf(&addition, 0, format, args);
	va_end(args);

	Z_STRVAL_P(string) = erealloc(Z_STRVAL_P(string), current_len + len + 1);
	Z_STRLEN_P(string) = current_len + len;
	memcpy(Z_STRVAL_P(string) + current_len, addition, len + 1);
	efree(addition);
}

void qb_extract_pbj_info(qb_extractor_context *cxt, int output_type) {
	USE_TSRM
	qb_pbj_translator_context *translator_cxt = cxt->translator_context;
	zval path = *cxt->input, *parameters;
	uint32_t i;

	zval_copy_ctor(&path);
	convert_to_string(&path);

	// load the code into memory
	qb_load_external_code(cxt->compiler_context, Z_STRVAL(path));

	// decode the pbj data
	qb_decode_pbj_binary(translator_cxt);

	if(output_type == QB_PBJ_DETAILS) {
		array_init(cxt->return_value);
		qb_add_string(cxt->return_value, "vendor", translator_cxt->vendor, -1);
		qb_add_string(cxt->return_value, "name", translator_cxt->name, translator_cxt->name_length);
		qb_add_string(cxt->return_value, "displayName", translator_cxt->display_name, -1);
		qb_add_string(cxt->return_value, "description", translator_cxt->description, -1);
		qb_add_int(cxt->return_value, "version", translator_cxt->version);
		parameters = qb_add_array(cxt->return_value, "parameters");
		for(i = 0; i < translator_cxt->parameter_count; i++) {
			qb_pbj_parameter *param = &translator_cxt->parameters[i];
			if(param != translator_cxt->out_coord && !param->input_size_name) {
				zval *param_info = qb_add_array(parameters, NULL);
				qb_add_string(param_info, "direction", (param->qualifier == PBJ_PARAMETER_OUT) ? "out" : "in", -1);
				qb_add_string(param_info, "name", param->name, -1);
				qb_add_string(param_info, "displayName", param->display_name, -1);
				qb_add_string(param_info, "type", pbj_type_names[param->type], -1);
				qb_add_string(param_info, "parameterType", param->parameter_type, -1);
				qb_add_string(param_info, "description", param->description, -1);
				if(param->qualifier == PBJ_PARAMETER_IN) {
					qb_add_pbj_value(param_info, "minValue", &param->min_value);
					qb_add_pbj_value(param_info, "maxValue", &param->max_value);
					qb_add_pbj_value(param_info, "defaultValue", &param->default_value);
				}
			}
		}
		for(i = 0; i < translator_cxt->texture_count; i++) {
			qb_pbj_texture *texture = &translator_cxt->textures[i];
			zval *param_info = qb_add_array(parameters, NULL);
			qb_add_string(param_info, "direction", "in", -1);
			qb_add_string(param_info, "name", texture->name, -1);
			qb_add_string(param_info, "displayName", NULL, 0);
			qb_add_string(param_info, "type", pbj_texture_type_names[texture->channel_count], -1);
			qb_add_string(param_info, "parameterType", NULL, 0);
			qb_add_string(param_info, "description", NULL, 0);
		}
	} else if(output_type == QB_PBJ_DECLARATION) {
		const char *pbj_path = Z_STRVAL_P(cxt->input);
		uint32_t param_count = 0;
		ZVAL_STRING(cxt->return_value, "/**\n", TRUE);
		qb_append_string(cxt->return_value, " * %.*s()\t", translator_cxt->name_length, translator_cxt->name);
		if(translator_cxt->description) {
			qb_append_string(cxt->return_value, "%s", translator_cxt->description);
		}
		if(translator_cxt->vendor) {
			qb_append_string(cxt->return_value, " (%s)", translator_cxt->vendor);
		}
		qb_append_string(cxt->return_value, "\n");
		
		qb_append_string(cxt->return_value, " *\n");
		qb_append_string(cxt->return_value, " * @engine\tqb\n");
		qb_append_string(cxt->return_value, " * @import\t%s\n", pbj_path);
		qb_append_string(cxt->return_value, " *\n");

		if(translator_cxt->out_pixel) {
			qb_pbj_parameter *param = translator_cxt->out_pixel;
			uint32_t channel_count;
			switch(param->type) {
				case PBJ_TYPE_FLOAT:  channel_count = 1; break;
				case PBJ_TYPE_FLOAT2: channel_count = 2; break;
				case PBJ_TYPE_FLOAT3: channel_count = 3; break;
				case PBJ_TYPE_FLOAT4: channel_count = 4; break;
			}
			qb_append_string(cxt->return_value, " * @param\t%s\t$%s\n", pbj_texture_qb_types[channel_count], param->name);
		}
		for(i = 0; i < translator_cxt->texture_count; i++) {
			qb_pbj_texture *texture = &translator_cxt->textures[i];
			qb_append_string(cxt->return_value, " * @param\t%s\t$%s\n", pbj_texture_qb_types[texture->channel_count], texture->name);
		}
		for(i = 0; i < translator_cxt->parameter_count; i++) {
			qb_pbj_parameter *param = &translator_cxt->parameters[i];
			if(param != translator_cxt->out_coord && param != translator_cxt->out_pixel && !param->input_size_name) {
				const char *type = pbj_param_qb_types[param->type];
				if(param->parameter_type) {
					if(param->type == PBJ_TYPE_FLOAT2) {
						if(strcmp(param->parameter_type, "position") == 0) {
							type = "float32[x,y]";
						}
					} else if(param->type == PBJ_TYPE_FLOAT3) {
						if(strcmp(param->parameter_type, "colorLAB") == 0) {
							type = "float32[L,a,b]";
						} else if(strcmp(param->parameter_type, "colorRGB") == 0) {
							type = "float32[r,g,b]";
						}
					} else if(param->type == PBJ_TYPE_FLOAT4) {
						if(strcmp(param->parameter_type, "colorCMYK") == 0) {
							type = "float32[c,m,y,k]";
						} else if(strcmp(param->parameter_type, "colorRGBA") == 0) {
							type = "float32[r,g,b,a]";
						}
					}
				}
				qb_append_string(cxt->return_value, " * @param\t%s\t$%s", type, param->name);
				if(param->description) {
					qb_append_string(cxt->return_value, "\t%s", param->description);
				}
				qb_append_string(cxt->return_value, "\n");
			}
		}

		qb_append_string(cxt->return_value, " *\n");
		qb_append_string(cxt->return_value, " * @return\tvoid\n");
		qb_append_string(cxt->return_value, " */\n");
		qb_append_string(cxt->return_value, "function %.*s(", translator_cxt->name_length, translator_cxt->name);
		if(translator_cxt->out_pixel) {
			qb_pbj_parameter *param = translator_cxt->out_pixel;
			qb_append_string(cxt->return_value, "&$%s", param->name);
			param_count++;
		}
		for(i = 0; i < translator_cxt->texture_count; i++) {
			qb_pbj_texture *texture = &translator_cxt->textures[i];
			if(param_count) {
				qb_append_string(cxt->return_value, ", ");
			}
			qb_append_string(cxt->return_value, "$%s", texture->name);
			param_count++;
		}
		for(i = 0; i < translator_cxt->parameter_count; i++) {
			qb_pbj_parameter *param = &translator_cxt->parameters[i];
			if(param != translator_cxt->out_coord && param != translator_cxt->out_pixel && !param->input_size_name) {
				if(param_count) {
					qb_append_string(cxt->return_value, ", ");
				}
				qb_append_string(cxt->return_value, "$%s", param->name);
			}
		}
		qb_append_string(cxt->return_value, ") {}\n");
	}
	zval_dtor(&path);
}

void qb_initialize_extractor_context(qb_extractor_context *cxt, zval *input, zval *return_value TSRMLS_DC) {
	memset(cxt, 0, sizeof(qb_extractor_context));
	cxt->input = input;
	cxt->return_value = return_value;
	cxt->pool = emalloc(sizeof(qb_data_pool));
	cxt->compiler_context = emalloc(sizeof(qb_compiler_context));
	cxt->translator_context = emalloc(sizeof(qb_pbj_translator_context));

	qb_initialize_data_pool(cxt->pool);
	qb_initialize_compiler_context(cxt->compiler_context, cxt->pool, NULL, 0, 1 TSRMLS_CC);
	qb_initialize_pbj_translator_context(cxt->translator_context, cxt->compiler_context TSRMLS_CC);

	SAVE_TSRMLS
}

void qb_free_extractor_context(qb_extractor_context *cxt) {
	qb_free_pbj_translator_context(cxt->translator_context);
	qb_free_compiler_context(cxt->compiler_context);
	qb_free_data_pool(cxt->pool);

	efree(cxt->translator_context);
	efree(cxt->compiler_context);
	efree(cxt->pool);
}