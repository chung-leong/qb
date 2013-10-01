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
#include <math.h>
#include "zend_variables.h"

static const char * qb_get_address_name(qb_interpreter_context *cxt, qb_address *address) {
	uint32_t i, j;
	if(cxt->function) {
		for(i = 0; i < cxt->function->variable_count; i++) {
			qb_variable *qvar = cxt->function->variables[i];
			if(qvar->address == address) {
				return qvar->name;
			}
		}
	}
	for(j = cxt->call_stack_height - 1; (int32_t) j >= 0; j++) {
		qb_call_stack_item *item = &cxt->call_stack[j];
		for(i = 0; i < item->function->variable_count; i++) {
			qb_variable *qvar = item->function->variables[i];
			if(qvar->address == address) {
				return qvar->name;
			}
		}
	}
	return "(unknown)";
}

#include "qb_interpreter_gd_image.c"

static php_stream * qb_get_file_stream(qb_interpreter_context *cxt, zval *resource) {
	USE_TSRM 
	if(Z_TYPE_P(resource) == IS_RESOURCE) {
		php_stream *stream = (php_stream*) zend_fetch_resource(&resource TSRMLS_CC, -1, NULL, NULL, 2, php_file_le_stream(), php_file_le_pstream());
		if(stream) {
			if(strcmp(stream->wrapper->wops->label, "plainfile") == 0) {
				return stream;
			}
		}
	}
	return NULL;
}

static uint32_t qb_set_array_dimensions_from_bytes(qb_interpreter_context *cxt, uint32_t byte_count, qb_address *address) {
	uint32_t element_count = ELEMENT_COUNT(byte_count, address->type);
	uint32_t item_element_count, item_byte_count, dimension, dimension_expected;
	qb_address *dimension_address, *item_size_address;

	if(address->dimension_count > 1) {
		item_size_address = address->array_size_addresses[1];
		if(CONSTANT(item_size_address)) {
			item_element_count = VALUE(U32, item_size_address);
			item_byte_count = BYTE_COUNT(item_element_count, address->type);
		} else {
			// cannot determine the dimension since the lower dimensions aren't defined 
			qb_abort("Array has undefined dimensions: %s", qb_get_address_name(cxt, address));
		}
		dimension = byte_count / item_byte_count;
	} else {
		dimension = element_count;
		item_byte_count = BYTE_COUNT(1, address->type);
	}

	// make sure the number of bytes is a multiple of the entry size
	if(byte_count != dimension * item_byte_count) {
		qb_abort("Number of bytes in string (%d) is not divisible by the size of each array entry (%d): %s", byte_count, item_byte_count, qb_get_address_name(cxt, address));
	}

	dimension_address = address->dimension_addresses[0];
	dimension_expected = VALUE(U32, dimension_address);
	if(dimension > dimension_expected) {
		if(CONSTANT(dimension_address)) {
			// dimension is defined
			qb_abort("Number of entries (%d) exceeds the declared size of the array (%d): %s", dimension, dimension_expected, qb_get_address_name(cxt, address));
		}
		VALUE(U32, dimension_address) = dimension;
		if(address->dimension_count > 1) {
			// set the array size as well (since it's not the same as the dimension)
			VALUE(U32, address->array_size_address) = element_count;
		}
	} else if(dimension < dimension_expected) {
		element_count = VALUE(U32, address->array_size_address);
	}
	return element_count;
}

static uint32_t qb_get_zend_array_size(qb_interpreter_context *cxt, zval *zvalue) {
	if(Z_TYPE_P(zvalue) == IS_ARRAY) {
		HashTable *ht = Z_ARRVAL_P(zvalue);
		return ht->nNextFreeElement;
	} else {
		if(Z_TYPE_P(zvalue) == IS_NULL) {
			return 0;
		} else {
			return 1;
		}
	}
}

static uint32_t qb_set_array_dimensions_from_caller_address(qb_interpreter_context *cxt, qb_storage *caller_storage, qb_address *caller_address, qb_address *address) {
	if(caller_address->dimension_count == address->dimension_count) {
		int32_t i;
		uint32_t array_size = 1;
		for(i = address->dimension_count - 1; i >= 0; i--) {
			qb_address *dimension_address = address->dimension_addresses[i];
			qb_address *caller_dimension_address = caller_address->dimension_addresses[i];
			qb_address *array_size_address = address->array_size_addresses[i];
			uint32_t dimension = VALUE_IN(caller_storage, U32, caller_dimension_address);
			uint32_t dimension_expected = VALUE(U32, dimension_address);

			array_size *= dimension;

			if(dimension > dimension_expected) {
				if(CONSTANT(dimension_address)) {
					qb_abort("Number of elements (%d) exceeds declared size of array (%d): %s", dimension, dimension_expected, qb_get_address_name(cxt, address));
				} else {
					VALUE(U32, dimension_address) = dimension;
				}
				if(i != address->dimension_count - 1) {
					VALUE(U32, array_size_address) = array_size;
				}
			} else if(dimension < dimension_expected) {
				array_size = VALUE(U32, array_size_address);
			}
		}
		return array_size;
	} else {
		qb_abort("Dimension mismatch: %s", qb_get_address_name(cxt, address));
		return 0;
	}
}

static int32_t qb_is_linear_zval_array(qb_interpreter_context *cxt, zval *zvalue) {
	HashTable *ht = Z_ARRVAL_P(zvalue);
	Bucket *p;
	for(p = ht->pListHead; p; p = p->pListNext) {
		if(p->nKeyLength == 0 && (long) p->h >= 0) {
			zval **p_element = p->pData;
			if(Z_TYPE_PP(p_element) == IS_ARRAY || Z_TYPE_PP(p_element) == IS_OBJECT) {
				return FALSE;
			}
		}
	}
	return TRUE;
}

static uint32_t qb_set_array_dimensions_from_zval(qb_interpreter_context *cxt, zval *zvalue, qb_address *address) {
	USE_TSRM

	gdImagePtr image;
	php_stream *stream;

	if(Z_TYPE_P(zvalue) == IS_ARRAY || Z_TYPE_P(zvalue) == IS_NULL || Z_TYPE_P(zvalue) == IS_LONG || Z_TYPE_P(zvalue) == IS_DOUBLE) {
		qb_address *dimension_address = address->dimension_addresses[0];
		uint32_t dimension = qb_get_zend_array_size(cxt, zvalue);
		uint32_t dimension_expected = VALUE(U32, dimension_address);

		if(CONSTANT(dimension_address)) {
			if(dimension > dimension_expected) {
				if(address->dimension_count > 1 && FIXED_LENGTH_ARRAY(address)) {
					// maybe we're trying to initialize a multidimensional array with a linear array
					if(qb_is_linear_zval_array(cxt, zvalue)) {
						uint32_t array_size = Z_ARRVAL_P(zvalue)->nNextFreeElement;
						uint32_t array_size_expected = ARRAY_SIZE(address);
						if(array_size > array_size_expected) {
							qb_abort("Number of elements (%d) exceeds the declared size of the array (%d): %s", array_size, array_size_expected, qb_get_address_name(cxt, address));
						}
						return array_size_expected;
					}
				}
				// dimension is defined
				qb_abort("Number of elements (%d) exceeds the declared size of the array (%d): %s", dimension, dimension_expected, qb_get_address_name(cxt, address));
			}
			dimension = dimension_expected;
		} else {
			VALUE(U32, dimension_address) = dimension;
		}
		if(address->dimension_count > 1) {
			qb_address item_address, *item_dimension_address, *item_array_size_address, *array_size_address;
			uint32_t item_array_size, array_size;

			item_address = *address;
			item_address.dimension_addresses++;
			item_address.array_size_addresses++;
			item_address.dimension_count--;
			item_dimension_address = item_address.dimension_addresses[0];
			if(item_address.index_alias_schemes) {
				item_address.index_alias_schemes++;
			}

			// set the dimension to zero first
			if(!CONSTANT(item_dimension_address)) {
				VALUE(U32, item_dimension_address) = 0;
			}

			// set the dimensions of sub-arrays (order doesn't matter)
			if(Z_TYPE_P(zvalue) == IS_ARRAY) {
				HashTable *ht = Z_ARRVAL_P(zvalue);
				Bucket *p;

				for(p = ht->pListHead; p; p = p->pListNext) {
					if(p->nKeyLength == 0 && (long) p->h >= 0) {
						zval **p_element = p->pData;
						qb_set_array_dimensions_from_zval(cxt, *p_element, &item_address);
					}
				}
			} else {
				// pass the value along to set the long dimension to either one or zero (if they were not defined)
				qb_set_array_dimensions_from_zval(cxt, zvalue, &item_address);
			}

			// calculate the array size (i.e. element count)
			item_array_size_address = item_address.array_size_addresses[0];
			item_array_size = VALUE(U32, item_array_size_address);
			array_size_address = address->array_size_addresses[0];
			array_size = dimension * item_array_size;
			VALUE(U32, array_size_address) = array_size;
			return array_size;
		} else {
			// don't need to calculate the array size here, as array_size_address
			// is pointing to the same value as dimension_address
			return dimension;
		}
	} else if(Z_TYPE_P(zvalue) == IS_OBJECT) {
		if(address->index_alias_schemes && address->index_alias_schemes[0]) {
			return ARRAY_SIZE(address);
		} else {
			qb_abort("Unable to convert an object to an array: %s", qb_get_address_name(cxt, address));
		}
	} else if(Z_TYPE_P(zvalue) == IS_STRING) {
		return qb_set_array_dimensions_from_bytes(cxt, Z_STRLEN_P(zvalue), address);
	} else if((image = qb_get_gd_image(cxt, zvalue))) {
		return qb_set_array_dimensions_from_image(cxt, image, address);
	} else if((stream = qb_get_file_stream(cxt, zvalue))) {
		// get the file size
		size_t position, size;
		position = php_stream_tell(stream);
		php_stream_seek(stream, 0, SEEK_END);		
		size = php_stream_tell(stream);
		php_stream_seek(stream, position, SEEK_SET);

		return qb_set_array_dimensions_from_bytes(cxt, size, address);
	} else {
		const char *ztype_name = zend_get_type_by_const(Z_TYPE_P(zvalue));
		qb_abort("Cannot convert %s to an array: %s", ztype_name, qb_get_address_name(cxt, address));
		return 0;
	}
}

static void qb_copy_elements_from_caller_address(qb_interpreter_context *cxt, qb_storage *caller_storage, qb_address *caller_address, qb_address *address) {
	uint32_t src_element_count = SCALAR(caller_address) ? 1 : ARRAY_SIZE_IN(caller_storage, caller_address);
	uint32_t dst_element_count = SCALAR(address) ? 1 : ARRAY_SIZE(address);
	qb_copy_elements(caller_address->type, ARRAY_IN(caller_storage, I08, caller_address), src_element_count, address->type, ARRAY(I08, address), dst_element_count);
}

static void qb_copy_element_from_zval(qb_interpreter_context *cxt, zval *zvalue, qb_address *address) {
	if(Z_TYPE_P(zvalue) == IS_LONG || Z_TYPE_P(zvalue) == IS_BOOL) {
		long value = Z_LVAL_P(zvalue);
#if SIZEOF_LONG == 8
		qb_copy_element(QB_TYPE_S64, (int8_t *) &value, address->type, ARRAY(I08, address));
#else
		qb_copy_element(QB_TYPE_S32, (int8_t *) &value, address->type, ARRAY(I08, address));
#endif
	} else if(Z_TYPE_P(zvalue) == IS_DOUBLE) {
		double value = Z_DVAL_P(zvalue);
		qb_copy_element(QB_TYPE_F64, (int8_t *) &value, address->type, ARRAY(I08, address));
	} else if(Z_TYPE_P(zvalue) == IS_STRING) {
		uint32_t type_size = type_sizes[address->type];
		uint32_t string_len = Z_STRLEN_P(zvalue);
		const char *string = Z_STRVAL_P(zvalue);
		if(type_size != string_len) {
			qb_abort("Cannot convert string to %s due to size mismatch", type_names[address->type]);
		}
		qb_copy_element(address->type, (int8_t *) string, address->type, ARRAY(I08, address));
	} else if(Z_TYPE_P(zvalue) == IS_ARRAY || Z_TYPE_P(zvalue) == IS_CONSTANT_ARRAY) {
		switch(address->type) {
			case QB_TYPE_S64:
			case QB_TYPE_U64: {
				VALUE(I64, address) = qb_zval_array_to_int64(zvalue);
			}	break;
			default:
				qb_abort("Cannot convert an array to %s", type_names[address->type]);
		}
	} else if(Z_TYPE_P(zvalue) == IS_NULL) {
		memset(ARRAY(I08, address), 0, BYTE_COUNT(1, address->type));
	}
}

static void qb_copy_elements_from_zval(qb_interpreter_context *cxt, zval *zvalue, qb_address *address) {
	USE_TSRM
	uint32_t i;
	gdImagePtr image;
	php_stream *stream;

	if(Z_TYPE_P(zvalue) == IS_ARRAY || Z_TYPE_P(zvalue) == IS_OBJECT) {
		qb_address *dimension_address = address->dimension_addresses[0];
		qb_address _item_address, *item_address = &_item_address;
		uint32_t item_element_count, item_byte_count, dimension = VALUE(U32, dimension_address);
		memset(item_address, 0, sizeof(qb_address));

		item_address->dimension_count = address->dimension_count - 1;
		item_address->type = address->type;
		item_address->flags = address->flags;
		item_address->segment_selector = address->segment_selector;
		item_address->segment_offset = address->segment_offset;
		if(item_address->dimension_count > 0) {
			item_address->mode = QB_ADDRESS_MODE_ARR;
			item_address->dimension_addresses = address->dimension_addresses + 1;
			item_address->array_size_addresses = address->array_size_addresses + 1;
			item_address->array_size_address = item_address->array_size_addresses[0];
			if(address->index_alias_schemes) {
				item_address->index_alias_schemes = address->index_alias_schemes + 1;
			}
			item_element_count = ARRAY_SIZE(item_address);
		} else {
			item_address->mode = QB_ADDRESS_MODE_ELE;
			item_element_count = 1;
		}
		item_byte_count = BYTE_COUNT(item_element_count, item_address->type);

		if(Z_TYPE_P(zvalue) == IS_ARRAY) {
			HashTable *ht = Z_ARRVAL_P(zvalue);
			uint32_t array_size = ht->nNextFreeElement;

			if(array_size > dimension) {
				if(item_address->dimension_count > 0 && qb_is_linear_zval_array(cxt, zvalue)) {
					// initializing a multidimensional array with a linear array
					item_address->dimension_count = 0;
					item_address->mode = QB_ADDRESS_MODE_ELE;
					item_address->dimension_addresses =
					item_address->array_size_addresses = NULL;
					item_address->array_size_address = NULL;
					item_element_count = 1;
					item_byte_count = BYTE_COUNT(item_element_count, item_address->type);
				}
			}
			for(i = 0; i < array_size; i++) {
				zval **p_item;
				if(zend_hash_index_find(ht, i, (void **) &p_item) == SUCCESS) {
					if(item_address->dimension_count > 0) {
						qb_copy_elements_from_zval(cxt, *p_item, item_address);
					} else { 
						qb_copy_element_from_zval(cxt, *p_item, item_address);
					}
				} else {
					memset(ARRAY(I08, item_address), 0, item_byte_count);
				}
				item_address->segment_offset += item_byte_count;
			}
			if(array_size < dimension) {
				qb_copy_wrap_around(ARRAY(I08, address), array_size * item_byte_count, dimension * item_byte_count);
			}
		} else {
			qb_index_alias_scheme *scheme = address->index_alias_schemes[0];
			for(i = 0; i < dimension; i++) {
				zval **p_item, *item = NULL;
				zval *alias = qb_cstring_to_zval(scheme->aliases[i] TSRMLS_CC);
				p_item = Z_OBJ_GET_PROP_PTR_PTR(zvalue, alias);

				if(p_item) {
					item = *p_item;
				} else if(Z_OBJ_HT_P(zvalue)->read_property) {
					item = Z_OBJ_READ_PROP(zvalue, alias);
				}
				if(item && Z_TYPE_P(item) != IS_NULL) {
					if(item_address->dimension_count > 0) {
						qb_copy_elements_from_zval(cxt, item, item_address);
					} else {
						qb_copy_element_from_zval(cxt, item, item_address);
					}
				} else {
					memset(ARRAY(I08, item_address), 0, item_byte_count);
				}
				if(!p_item && item) {
					Z_ADDREF_P(item);
					zval_ptr_dtor(&item);
				}
				item_address->segment_offset += item_byte_count;
			}
		}
	} else if(Z_TYPE_P(zvalue) == IS_STRING) {
		// copy the bytes from the string
		int8_t *memory = (int8_t *) Z_STRVAL_P(zvalue);
		uint32_t element_count = VALUE(U32, address->array_size_address);
		uint32_t byte_count = BYTE_COUNT(element_count, address->type);
		memcpy(ARRAY(I08, address), memory, byte_count);
		if((uint32_t) Z_STRLEN_P(zvalue) < byte_count) {
			qb_copy_wrap_around(ARRAY(I08, address), Z_STRLEN_P(zvalue), byte_count);
		}
	} else if(Z_TYPE_P(zvalue) == IS_NULL) {
		// clear the bytes 
		uint32_t element_count = VALUE(U32, address->array_size_address);
		uint32_t byte_count = BYTE_COUNT(element_count, address->type);
		memset(ARRAY(I08, address), 0, byte_count);
	} else if(Z_TYPE_P(zvalue) == IS_LONG || Z_TYPE_P(zvalue) == IS_DOUBLE) {
		uint32_t element_count = VALUE(U32, address->array_size_address);
		// copy the one element
		qb_copy_element_from_zval(cxt, zvalue, address);
		if(element_count > 1) {
			uint32_t byte_count = BYTE_COUNT(element_count, address->type);
			qb_copy_wrap_around(ARRAY(I08, address), BYTE_COUNT(1, address->type), byte_count);
		}
	} else if((stream = qb_get_file_stream(cxt, zvalue))) {
		// copy the content from the file 
		uint32_t position = php_stream_tell(stream);
		uint32_t byte_count = VALUE(U32, address->array_size_address);
		php_stream_seek(stream, 0, SEEK_SET);
		php_stream_read(stream, (char *) ARRAY(I08, address), byte_count);
		php_stream_seek(stream, position, SEEK_SET);
	} else if((image = qb_get_gd_image(cxt, zvalue))) {
		qb_copy_elements_from_gd_image(cxt, image, address);
	}
}

static int32_t qb_map_segment_to_file(qb_interpreter_context *cxt, qb_memory_segment *segment, php_stream *stream, int32_t write_access, uint32_t byte_count) {
	USE_TSRM
	if(QB_G(allow_memory_map)) {
		php_stream_mmap_range range;
		size_t file_size;
		if(write_access) {
			// make sure the file is large enough
			off_t position;
			position = php_stream_tell(stream);
			php_stream_seek(stream, 0, SEEK_END);
			file_size = php_stream_tell(stream);
			php_stream_seek(stream, position, SEEK_SET);
			if(byte_count > file_size) {
				php_stream_truncate_set_size(stream, byte_count);
			}
		}
		range.length = byte_count;
		range.offset = 0;
		range.mode = (write_access) ? PHP_STREAM_MAP_MODE_SHARED_READWRITE : PHP_STREAM_MAP_MODE_SHARED_READONLY;
		range.mapped = NULL;

		if(php_stream_set_option(stream, PHP_STREAM_OPTION_MMAP_API, PHP_STREAM_MMAP_MAP_RANGE, &range) == PHP_STREAM_OPTION_RETURN_OK) {
			// range.mapped is the end of the 
			segment->memory = (int8_t *) range.mapped;
			segment->stream = stream;
			segment->flags |= QB_SEGMENT_MAPPED;
			return TRUE;
		}
	}
	return FALSE;
}

static void qb_unmap_segment(qb_interpreter_context *cxt, qb_memory_segment *segment) {
	USE_TSRM
	php_stream_set_option(segment->stream, PHP_STREAM_OPTION_MMAP_API, PHP_STREAM_MMAP_UNMAP, NULL);
	segment->memory = NULL;
	segment->stream = NULL;
	segment->flags &= ~QB_SEGMENT_MAPPED;
}

static void qb_free_segment(qb_interpreter_context *cxt, qb_memory_segment *segment) {
	USE_TSRM
	if(segment->flags & QB_SEGMENT_MAPPED) {
		php_stream *stream = segment->stream;
		qb_unmap_segment(cxt, segment);

		// set the file to the actual size if more bytes were allocated than needed
		if(segment->current_allocation != segment->byte_count) {
			php_stream_truncate_set_size(stream, segment->byte_count);
		}
		if(!(segment->flags & QB_SEGMENT_BORROWED)) {
			php_stream_close(stream);
			segment->flags &= ~QB_SEGMENT_BORROWED;
		}
	} else if(segment->flags & QB_SEGMENT_BORROWED) {
		segment->memory = NULL;
		segment->flags &= ~QB_SEGMENT_BORROWED;
	} else if(!(segment->flags & QB_SEGMENT_PREALLOCATED)) {
		if(segment->memory) {
			efree(segment->memory);
			segment->memory = NULL;
		}
	}
	segment->current_allocation = 0;
}

static void qb_resize_segment(qb_interpreter_context *cxt, qb_memory_segment *segment, uint32_t byte_count) {
	USE_TSRM
	if(segment->flags & QB_SEGMENT_MAPPED) {
		php_stream *stream = segment->stream;
		qb_unmap_segment(cxt, segment);
		php_stream_truncate_set_size(stream, byte_count);
		if(!qb_map_segment_to_file(cxt, segment, stream, TRUE, byte_count)) {
			qb_abort("Cannot map '%s' into memory", stream->orig_path);
		}
	} else if(segment->flags & QB_SEGMENT_PREALLOCATED) {
		qb_abort("Cannot resize preallocated memory");
	} else {
		segment->memory = erealloc(segment->memory, byte_count);
	}
}

void qb_enlarge_segment(qb_interpreter_context *cxt, qb_memory_segment *segment, uint32_t desired_size) {
	int8_t *current_data_end;

	if(desired_size > segment->current_allocation) {
		// allocate more bytes
		segment->current_allocation = ALIGN_TO(desired_size, 1024);
		qb_resize_segment(cxt, segment, segment->current_allocation);

		// clear the newly allcoated bytes
		current_data_end = segment->memory + segment->byte_count;
		memset(current_data_end, 0, desired_size - segment->byte_count);
	}
}

void qb_shrink_segment(qb_interpreter_context *restrict cxt, qb_memory_segment *segment, uint32_t start_index, uint32_t count) {
	/*
	uint32_t current_size = segment->byte_count;
	uint32_t desired_size = current_size - count;
	uint32_t bytes_to_move, bytes_to_remove;
	int8_t *gap_start, *gap_end, *data_end;

	// figure out the gap's beginning and end
	bytes_to_move = desired_size - start_index;
	bytes_to_remove = count;
	gap_start = segment->memory + start_index;
	gap_end = gap_start + bytes_to_remove;
	data_end = gap_start + bytes_to_move;

	// move data behind the gap forward
	memmove(gap_start, gap_end, bytes_to_move);

	// clear the bytes behind the actual data, now vacated
	memset(data_end, 0, bytes_to_remove);
	*/
}

NO_RETURN void qb_abort_range_error(qb_interpreter_context *restrict cxt, qb_memory_segment *segment, uint32_t index, uint32_t count, uint32_t line_number) {
	/* TODO:
	USE_TSRM
	qb_function *function = NULL;
	qb_storage *storage = NULL;
	qb_variable *problem_variable = NULL;
	uint32_t i, base_index, segment_selector = 0;

	// see if the segment is in the called function's storage
	for(i = 0; i < cxt->storage->segment_count; i++) {
		if(&cxt->storage->segments[i] == segment) {
			segment_selector = i;
			storage = cxt->storage;
			function = cxt->function;
			break;
		}
	}
	if(!storage) {
		// it's probably in the caller
		if(cxt->call_stack_height > 0) {
			qb_call_stack_item *caller = &cxt->call_stack[cxt->call_stack_height - 1];
			for(i = 0; i < caller->storage->segment_count; i++) {
				if(&caller->storage->segments[i] == segment) {
					segment_selector = i;
					storage = caller->storage;
					function = caller->function;
					line_number = cxt->function_call_line_number;
					break;
				}
			}
		}
	}
	if(function) {
		for(i = 0; i < function->variable_count; i++) {
			qb_variable *qvar = function->variables[i];
			if(qvar->address) {
				if(qvar->address->segment_selector == segment_selector) {
					if(segment_selector >= QB_SELECTOR_DYNAMIC_ARRAY_START) {
						base_index = 0;
						problem_variable = qvar;
						break;
					} else {
						uint32_t length = SCALAR(qvar->address) ? 1 : ARRAY_SIZE_IN(storage, qvar->address);
						base_index = ELEMENT_COUNT(qvar->address->segment_offset, qvar->address->type);
						if(base_index <= index && index < base_index + length) {
							problem_variable = qvar;
							break;
						}
					}
				}
			}
		}
		QB_G(current_filename) = function->filename;
		QB_G(current_line_number) = line_number;
	}
	if(problem_variable) {
		uint32_t max_index = index + count - base_index - 1;
		const char *var_name = (problem_variable->name) ? problem_variable->name : "(return value)";
		if(SCALAR(problem_variable->address)) {
			qb_abort("Array index %d is beyond the range of a scalar: %s", max_index, var_name);
		} else {
			qb_abort("Array index %d is out of range: %s", max_index, var_name);
		}
	} else {
		qb_abort("Illegal memory access on segment %d at index %d", segment_selector, index + count - 1);
	}
	*/
	qb_abort("Range error");
}

enum {
	QB_TRANSFER_CAN_BORROW_MEMORY	= 0x0001,
	QB_TRANSFER_CAN_SEIZE_MEMORY	= 0x0002,
	QB_TRANSFER_CAN_ENLARGE_SEGMENT	= 0x0004,
};

static void qb_transfer_value_from_zval(qb_interpreter_context *cxt, zval *zvalue, qb_address *address, int32_t transfer_flags) {
	USE_TSRM
	qb_memory_segment *segment = &cxt->storage->segments[address->segment_selector];
	uint32_t element_start_index = address->segment_offset;

	if(SCALAR(address)) {
		// make sure the address is valid
		/* TODO:
		if(address->segment_selector >= QB_SELECTOR_DYNAMIC_ARRAY_START) {			
			if(element_start_index + 1 > segment->current_allocation) {
				if(segment->flags & QB_SEGMENT_EXPANDABLE) {
					qb_resize_segment(cxt, segment, element_start_index + 1);
					segment->current_allocation = element_start_index + 1;
				} else {
					qb_abort_range_error(cxt, segment, element_start_index, 1, 0);
				}
			} 
		}
		*/

		// assign scalar value
		qb_copy_element_from_zval(cxt, zvalue, address);
	} else {
		// determine the array's dimensions and check for out-of-bound condition
		/* TODO
		uint32_t element_count = qb_set_array_dimensions_from_zval(cxt, zvalue, address);
		uint32_t byte_count = BYTE_COUNT(element_count, address->type);

		if(address->flags & QB_ADDRESS_SEGMENT) {
			segment->byte_count = byte_count;
			// if the segment doesn't have preallocated memory, see if it can borrow it from the zval
			if(!(segment->flags & QB_SEGMENT_PREALLOCATED) && (transfer_flags & (QB_TRANSFER_CAN_BORROW_MEMORY | QB_TRANSFER_CAN_SEIZE_MEMORY))) {
				php_stream *stream;
				if(Z_TYPE_P(zvalue) == IS_STRING) {
					// use memory from the string if it's long enough
					if((uint32_t) Z_STRLEN_P(zvalue) >= byte_count) {
						qb_free_segment(cxt, segment);
						segment->memory = (int8_t *) Z_STRVAL_P(zvalue);
						segment->current_allocation = byte_count;
						if(transfer_flags & QB_TRANSFER_CAN_BORROW_MEMORY) {
							segment->flags |= QB_SEGMENT_BORROWED;
						} else {
							Z_TYPE_P(zvalue) = IS_NULL;
						}
						return;
					}
				}
				if((stream = qb_get_file_stream(cxt, zvalue))) {
					// use memory mapped file if possible
					int32_t write_access = !READ_ONLY(address);
					uint32_t allocation = byte_count;
					if(allocation == 0) {
						// can't have a mapping that's zero
						if(write_access) {
							allocation = 1024;
						}
					}
					qb_free_segment(cxt, segment);
					if(qb_map_segment_to_file(cxt, segment, stream, write_access, allocation)) {
						segment->current_allocation = allocation;
						if(transfer_flags & QB_TRANSFER_CAN_BORROW_MEMORY) {
							segment->flags |= QB_SEGMENT_BORROWED;
						} else if(transfer_flags & QB_TRANSFER_CAN_SEIZE_MEMORY) {
							// the stream was taken from the zval
							Z_TYPE_P(zvalue) = IS_NULL;
						}
						return;
					}
				}
			}
		}
		
		if(element_start_index + byte_count > segment->current_allocation) {
			qb_resize_segment(cxt, segment, element_start_index + byte_count);
			segment->current_allocation = element_start_index + byte_count;
		}

		// copy values over
		qb_copy_elements_from_zval(cxt, zvalue, address);
		*/
	}
}

static void qb_transfer_value_from_caller_storage(qb_interpreter_context *cxt, qb_storage *caller_storage, qb_address *caller_address, qb_address *address, uint32_t transfer_flags) {
	/* TODO
	// make sure the address is in bound in the caller segment
	if(caller_address->segment_selector >= QB_SELECTOR_DYNAMIC_ARRAY_START) {
		qb_memory_segment *caller_segment = &caller_storage->segments[caller_address->segment_selector];
		uint32_t caller_element_start_index = caller_address->segment_offset;
		uint32_t caller_element_count = SCALAR(caller_address) ? 1 : ARRAY_SIZE_IN(caller_storage, caller_address);
		uint32_t caller_byte_count = BYTE_COUNT(caller_element_count, caller_address->type);

		if(caller_element_start_index + caller_byte_count > caller_segment->byte_count) {
			if((caller_segment->flags & QB_SEGMENT_EXPANDABLE) && (transfer_flags & QB_TRANSFER_CAN_ENLARGE_SEGMENT)) {
				qb_enlarge_segment(cxt, caller_segment, caller_element_start_index + caller_byte_count);
			} else {
				qb_abort_range_error(cxt, caller_segment, caller_element_start_index, caller_element_count, cxt->function_call_line_number);
			}
		}
	}

	// set up the destination segment, if necessary
	if(address->flags & QB_ADDRESS_SEGMENT) {
		// set the dimensions of the destination address (or error out)
		qb_memory_segment *segment = &cxt->storage->segments[address->segment_selector];
		uint32_t element_count = qb_set_array_dimensions_from_caller_address(cxt, caller_storage, caller_address, address); 
		uint32_t byte_count = BYTE_COUNT(element_count, address->type);

		// see if we can just point to the memory in the caller storage
		if(!(segment->flags & QB_SEGMENT_PREALLOCATED) && transfer_flags & QB_TRANSFER_CAN_BORROW_MEMORY) {
			if(STORAGE_TYPE_MATCH(address->type, caller_address->type)) {
				// use memory from the caller if it's larger enough
				if(ARRAY_SIZE_IN(caller_storage, caller_address) >= ARRAY_SIZE(address)) {
					if(segment->flags & QB_SEGMENT_EXPANDABLE) {
						if(EXPANDABLE_ARRAY(caller_address)) {
							qb_memory_segment *caller_segment = &caller_storage->segments[caller_address->segment_selector];
							segment->memory = caller_segment->memory;
							segment->stream = caller_segment->stream;
							segment->current_allocation = caller_segment->current_allocation;
							segment->byte_count = byte_count;
							segment->flags |= (caller_segment->flags & QB_SEGMENT_MAPPED) | QB_SEGMENT_BORROWED;
							return;
						}
					} else {
						segment->memory = ARRAY_IN(caller_storage, I08, caller_address);
						segment->current_allocation = byte_count;
						segment->byte_count = byte_count;
						segment->flags |= QB_SEGMENT_BORROWED;
						return;
					}
				}
			}
		}

		// allocate memory for the segment
		if(segment->byte_count != byte_count) {
			qb_resize_segment(cxt, segment, byte_count);
		}

		// update the segment length and allocation count
		segment->byte_count = segment->current_allocation = byte_count; 
	}
	qb_copy_elements_from_caller_address(cxt, caller_storage, caller_address, address);
	*/
}

static void qb_transfer_value_from_import_source(qb_interpreter_context *cxt, qb_variable_import *import) {
	USE_TSRM
	qb_variable *qvar = import->variable;

	if(import->previous_copy_index != -1) {
		// copy value from qb caller
		qb_variable_import *previous_copy = &cxt->variable_imports[import->previous_copy_index];
		qb_transfer_value_from_caller_storage(cxt, previous_copy->storage, previous_copy->variable->address, qvar->address, QB_TRANSFER_CAN_BORROW_MEMORY);
	} else {
		zval *zvalue = NULL, **p_zvalue = NULL;
		if(import->value_pointer) {
			zvalue = *import->value_pointer;
			p_zvalue = import->value_pointer;
		} else {
			// look for it
			if(qvar->flags & QB_VARIABLE_GLOBAL) {
				// copy value from global symbol table
				if(zend_hash_quick_find(&EG(symbol_table), qvar->name, qvar->name_length + 1, qvar->hash_value, (void **) &p_zvalue) == SUCCESS) {
					SEPARATE_ZVAL_TO_MAKE_IS_REF(p_zvalue);
					zvalue = *p_zvalue;
				}
			} else {
				// something from the object/class
				USE_TSRM

				if(qvar->flags & QB_VARIABLE_CLASS_CONSTANT) {
					// only static:: constants are treated like variables
					zend_class_entry *ce = EG(called_scope);
					zval **p_value;
					zend_hash_quick_find(&ce->constants_table, qvar->name, qvar->name_length + 1, qvar->hash_value, (void **) &p_value);
					zvalue = *p_value;
				} else {
					zval *name = qb_string_to_zval(qvar->name, qvar->name_length TSRMLS_CC);
					if(qvar->flags & QB_VARIABLE_CLASS) {
						// copy value from class, using the called scope if the class wasn't known beforehand (i.e. static::)
						zend_class_entry *ce = (qvar->zend_class) ? qvar->zend_class : EG(called_scope);
						p_zvalue = Z_CLASS_GET_PROP(ce, qvar->name, qvar->name_length);
						if(p_zvalue) {
							SEPARATE_ZVAL_TO_MAKE_IS_REF(p_zvalue);
							zvalue = *p_zvalue;
						}
					} else if(qvar->flags & QB_VARIABLE_CLASS_INSTANCE) {
						// copy value from class instance
						zval *container = cxt->this_object;
						p_zvalue = Z_OBJ_GET_PROP_PTR_PTR(container, name);
						if(p_zvalue) {
							SEPARATE_ZVAL_TO_MAKE_IS_REF(p_zvalue);
							zvalue = *p_zvalue;
						} else if(Z_OBJ_HT_P(container)->read_property) {
							zvalue = Z_OBJ_READ_PROP(container, name);
						}
					}
				}
			}
			import->value_pointer = p_zvalue;
		}
		if(zvalue) {
			qb_transfer_value_from_zval(cxt, zvalue, qvar->address, QB_TRANSFER_CAN_BORROW_MEMORY);
			if(!p_zvalue) {
				if(Z_REFCOUNT_P(zvalue) == 0) {
					Z_ADDREF_P(zvalue);
					zval_ptr_dtor(&zvalue);
				}
			}
		} else {
			qb_transfer_value_from_zval(cxt, &zval_used_for_init, qvar->address, 0);
		}
	}
}

static void qb_update_imported_variables(qb_interpreter_context *cxt) {
	int32_t i;

	for(i = cxt->variable_import_count - 1; i >= 0; i--) {
		qb_variable_import *import = &cxt->variable_imports[i];
		qb_transfer_value_from_import_source(cxt, import);
	}
}

static void qb_import_variable(qb_interpreter_context *cxt, qb_variable *qvar) {
	int32_t i;
	qb_variable_import *import;

	if(!cxt->variable_imports) {
		qb_create_array((void **) &cxt->variable_imports, &cxt->variable_import_count, sizeof(qb_variable_import), 8);
	}
	import = qb_enlarge_array((void**) &cxt->variable_imports, 1);
	import->variable = qvar;
	import->storage = cxt->storage;
	import->previous_copy_index = -1;
	import->value_pointer = NULL;

	if(!(qvar->flags & QB_VARIABLE_CLASS_CONSTANT)) {
		for(i = cxt->variable_import_count - 2; i >= 0; i--) {
			qb_variable_import *previous_import = &cxt->variable_imports[i];
			qb_variable *previous_qvar = previous_import->variable;

			if(previous_qvar->hash_value == qvar->hash_value && strcmp(qvar->name, previous_qvar->name) == 0) {
				if(previous_qvar->address->type != qvar->address->type) {
					qb_abort("Redeclaring %s, which was previously declared as %s, as %s", qvar->name, type_names[previous_qvar->address->type], type_names[qvar->address->type]);
				}
				import->previous_copy_index = i;
			}
		}
	}

	qb_transfer_value_from_import_source(cxt, import);
}

static void qb_transfer_arguments_from_php(qb_interpreter_context *cxt, zval *this, zval *retval, zval ***p_args, uint32_t arg_count) {
	qb_function *func = cxt->function;
	uint32_t i;

	cxt->this_object = this;
	cxt->argument_count = arg_count;
	if(arg_count > func->argument_count) {
		arg_count = func->argument_count;
	}

	for(i = 0; i < arg_count; i++) {
		qb_variable *qvar = func->variables[i];
		zval **p_zarg = p_args[i];
		uint32_t transfer_flags = 0;

		if((qvar->flags & QB_VARIABLE_PASSED_BY_REF) || READ_ONLY(qvar->address)) {
			// avoid allocating new memory and copying contents if changes will be copied back anyway (or no changes will be made)
			transfer_flags = QB_TRANSFER_CAN_BORROW_MEMORY;
		}
		qb_transfer_value_from_zval(cxt, *p_zarg, qvar->address, transfer_flags);
	}
}

static void qb_copy_element_to_zval(qb_interpreter_context *cxt, qb_address *address, zval *zvalue) {
	zval_dtor(zvalue);
	switch(address->type) {
		case QB_TYPE_S08: {
			Z_TYPE_P(zvalue) = IS_LONG;
			Z_LVAL_P(zvalue) = VALUE(S08, address);
		}	break;
		case QB_TYPE_U08: {
			Z_TYPE_P(zvalue) = IS_LONG;
			Z_LVAL_P(zvalue) = VALUE(U08, address);
		}	break;
		case QB_TYPE_S16: {
			Z_TYPE_P(zvalue) = IS_LONG;
			Z_LVAL_P(zvalue) = VALUE(S16, address);
		}	break;
		case QB_TYPE_U16: {
			Z_TYPE_P(zvalue) = IS_LONG;
			Z_LVAL_P(zvalue) = VALUE(U16, address);
		}	break;
		case QB_TYPE_S32: {
			Z_TYPE_P(zvalue) = IS_LONG;
			Z_LVAL_P(zvalue) = VALUE(S32, address);
		}	break;
		case QB_TYPE_U32: {
#if SIZEOF_LONG == 8
			Z_TYPE_P(zvalue) = IS_LONG;
			Z_LVAL_P(zvalue) = VALUE(U32, address);
#else
			CTYPE(U32) value = VALUE(U32, address);
			if(value & 0x80000000) {
				// the sign bit is used--save it to a double
				Z_TYPE_P(zvalue) = IS_DOUBLE;
				Z_DVAL_P(zvalue) = (double) value;
			} else {
				// we can safely put it in a 32-bit long
				Z_TYPE_P(zvalue) = IS_LONG;
				Z_LVAL_P(zvalue) = (long) value;
			}
#endif
		}	break;
		case QB_TYPE_S64: {
#if SIZEOF_LONG == 8
			Z_TYPE_P(zvalue) = IS_LONG;
			Z_LVAL_P(zvalue) = VALUE(S64, address);
#else
			CTYPE(S64) value = VALUE(S64, address);
			if(value & 0xFFFFFFFF00000000LL) {
				// the upper bits are used--save it to a double to preserve as much info as we can
				Z_TYPE_P(zvalue) = IS_DOUBLE;
				Z_DVAL_P(zvalue) = (double) value;
			} else {
				// we can safely put it in a 32-bit long
				Z_TYPE_P(zvalue) = IS_LONG;
				Z_LVAL_P(zvalue) = (long) value;
			}
#endif
		}	break;
		case QB_TYPE_U64: {
#if SIZEOF_LONG == 8
			Z_TYPE_P(zvalue) = IS_LONG;
			Z_LVAL_P(zvalue) = VALUE(U64, address);
#else
			CTYPE(U64) value = VALUE(U64, address);
			if(value & 0xFFFFFFFF80000000LL) {
				// the upper bits are used--save it to a double to preserve as much info as we can
				Z_TYPE_P(zvalue) = IS_DOUBLE;
				Z_DVAL_P(zvalue) = (double) value;
			} else {
				// we can safely put it in a 32-bit long
				Z_TYPE_P(zvalue) = IS_LONG;
				Z_LVAL_P(zvalue) = (long) value;
			}
#endif
		}	break;
		case QB_TYPE_F32: {
			Z_TYPE_P(zvalue) = IS_DOUBLE;
			Z_DVAL_P(zvalue) = VALUE(F32, address);
		}	break;
		case QB_TYPE_F64: {
			Z_TYPE_P(zvalue) = IS_DOUBLE;
			Z_DVAL_P(zvalue) = VALUE(F64, address);
		}	break;
		default: break;
	}
}

static void qb_initialize_zval_array(qb_interpreter_context *cxt, qb_address *item_address, zval *container, zval *item) {
	USE_TSRM
	zend_class_entry *ce = NULL;
	if(item_address->index_alias_schemes && item_address->index_alias_schemes[0]) {
		qb_index_alias_scheme *scheme = item_address->index_alias_schemes[0];
		if(scheme->class_name) {
			if(!scheme->zend_class) {
				zend_class_entry **p_ce;
				if(zend_lookup_class(scheme->class_name, scheme->class_name_length, &p_ce TSRMLS_CC) == FAILURE) {
					qb_abort("Class '%s' not found", scheme->class_name);
				} else if((*p_ce)->ce_flags & (ZEND_ACC_INTERFACE|ZEND_ACC_IMPLICIT_ABSTRACT_CLASS|ZEND_ACC_EXPLICIT_ABSTRACT_CLASS)) {
					qb_abort("Cannot instantiate interface or abstract class '%s'", scheme->class_name);
				}
				scheme->zend_class = *p_ce;
			}
		}
		ce = scheme->zend_class;

	}
	if(!ce && container) {
		// use the class of the first item in the container (if there's one)
		HashTable *ht = Z_ARRVAL_P(container);
		if(ht->pListHead) {
			zval **p_element = ht->pListHead->pData, *element = *p_element;
			if(Z_TYPE_P(element) == IS_OBJECT) {
				ce = Z_OBJCE_P(element);
			}
		}
	}
	if(!ce && item_address->index_alias_schemes && item_address->index_alias_schemes[0]) {
		// use standard class if a scheme is provided but no explicit class name given
		ce = zend_standard_class_def;
	}
	if(ce) {
		object_init_ex(item, ce);
	} else {
		uint32_t size = VALUE(U32, item_address->dimension_addresses[0]);
		array_init_size(item, size);
	}
}

static void qb_copy_elements_to_zval(qb_interpreter_context *cxt, qb_address *address, zval *zvalue) {
	USE_TSRM
	qb_address *dimension_address = address->dimension_addresses[0];
	uint32_t dimension = VALUE(U32, dimension_address);
	uint32_t i;
	gdImagePtr image;
	php_stream *stream;

	if(Z_TYPE_P(zvalue) == IS_ARRAY || Z_TYPE_P(zvalue) == IS_OBJECT) {
		qb_address _item_address, *item_address = &_item_address;
		uint32_t item_element_count, item_byte_count;

		item_address->type = address->type;
		item_address->segment_selector = address->segment_selector;
		item_address->segment_offset = address->segment_offset;
		item_address->dimension_count = address->dimension_count - 1;
		item_address->array_index_address = NULL;
		if(item_address->dimension_count > 0) {
			item_address->mode = QB_ADDRESS_MODE_ARR;
			item_address->dimension_addresses = address->dimension_addresses + 1;
			item_address->array_size_addresses = address->array_size_addresses + 1;
			item_address->array_size_address = item_address->array_size_addresses[0];
			if(address->index_alias_schemes) {
				item_address->index_alias_schemes = address->index_alias_schemes + 1;
			} else {
				item_address->index_alias_schemes = NULL;
			}
			item_element_count = VALUE(U32, item_address->array_size_address);
		} else {
			item_address->mode = QB_ADDRESS_MODE_ELE;
			item_address->dimension_addresses = NULL;
			item_address->array_size_addresses = NULL;
			item_address->array_size_address = NULL;
			item_address->array_index_address = NULL;
			item_address->index_alias_schemes = NULL;
			item_element_count = 1;
		}
		item_byte_count = BYTE_COUNT(item_element_count, address->type);

		if(Z_TYPE_P(zvalue) == IS_ARRAY) {
			HashTable *ht = Z_ARRVAL_P(zvalue);
			for(i = 0; i < dimension; i++) {
				zval **p_item, *item;

				if(zend_hash_index_find(ht, i, (void **) &p_item) == SUCCESS) {
					SEPARATE_ZVAL_TO_MAKE_IS_REF(p_item);
					item = *p_item;
				} else {
					ALLOC_INIT_ZVAL(item);
					if(item_address->dimension_count > 0) {
						qb_initialize_zval_array(cxt, item_address, zvalue, item);
					}
					zend_hash_index_update(ht, i, (void **) &item, sizeof(zval *), NULL);
				}
				if(item_address->dimension_count > 0) {
					qb_copy_elements_to_zval(cxt, item_address, item);
				} else {
					qb_copy_element_to_zval(cxt, item_address, item);
				}
				item_address->segment_offset += item_byte_count;
			}
		} else {
			qb_index_alias_scheme *scheme = address->index_alias_schemes[0];
			for(i = 0; i < dimension; i++) {
				zval **p_item, *item = NULL;
				zval *alias = qb_cstring_to_zval(scheme->aliases[i] TSRMLS_CC);
				p_item = Z_OBJ_GET_PROP_PTR_PTR(zvalue, alias);
				if(p_item) {
					SEPARATE_ZVAL_TO_MAKE_IS_REF(p_item);
					item = *p_item;
				} else {
					ALLOC_INIT_ZVAL(item);
					if(item_address->dimension_count > 0) {
						qb_initialize_zval_array(cxt, item_address, zvalue, item);
					}
				}
				if(item_address->dimension_count > 0) {
					qb_copy_elements_to_zval(cxt, item_address, item);
				} else {
					qb_copy_element_to_zval(cxt, item_address, item);
				}
				if(!p_item) {
					Z_OBJ_WRITE_PROP(zvalue, alias, item);
					zval_ptr_dtor(&item);
				}
				item_address->segment_offset += item_byte_count;
			}
		}
	} else if(Z_TYPE_P(zvalue) == IS_STRING) {
		int8_t *memory = (int8_t *) Z_STRVAL_P(zvalue);
		uint32_t element_count = VALUE(U32, address->array_size_address);
		uint32_t byte_count = BYTE_COUNT(element_count, address->type);
		memcpy(memory, ARRAY(I08, address), byte_count);
	} else if((stream = qb_get_file_stream(cxt, zvalue))) {
		// copy the content from the file 
		uint32_t position = php_stream_tell(stream);
		uint32_t byte_count = VALUE(U32, address->array_size_address);
		php_stream_seek(stream, 0, SEEK_SET);
		php_stream_write(stream, (char *) ARRAY(I08, address), byte_count);
		php_stream_seek(stream, position, SEEK_SET);
	} else if((image = qb_get_gd_image(cxt, zvalue))) {
		qb_copy_elements_to_gd_image(cxt, address, image);
	}
}

static void qb_transfer_value_to_zval(qb_interpreter_context *cxt, qb_address *address, zval *zvalue) {
	if(SCALAR(address)) {
		qb_copy_element_to_zval(cxt, address, zvalue);
	} else {
		qb_memory_segment *segment = &cxt->storage->segments[address->segment_selector];
		uint32_t element_count = VALUE(U32, address->array_size_address);
		uint32_t byte_count = BYTE_COUNT(element_count, address->type);

		if(Z_TYPE_P(zvalue) == IS_NULL) {
			// create a string if the string flag is set, otherwise create an array
			if(address->flags & QB_ADDRESS_STRING) {
				ZVAL_EMPTY_STRING(zvalue);
			} else {
				qb_initialize_zval_array(cxt, address, NULL, zvalue);
			}
		}

		if(segment->flags & QB_SEGMENT_BORROWED) {
			// modifications would have been made directly to the string/file
			if(Z_TYPE_P(zvalue) == IS_STRING) {
				// make sure that we haven't allocated a new memory block
				if(Z_STRVAL_P(zvalue) != (char *) segment->memory) {
					uint32_t allocated_byte_count = BYTE_COUNT(segment->current_allocation, address->type);

					// reallocate if there's no room for the string terminator or if there're lots of extra bytes 
					if(byte_count >= allocated_byte_count || (allocated_byte_count - byte_count) > 256) {
						segment->memory = erealloc(segment->memory, byte_count + 1);
					}
					segment->memory[byte_count] = '\0';
					efree(Z_STRVAL_P(zvalue));
					Z_STRVAL_P(zvalue) = (char *) segment->memory;
					Z_STRLEN_P(zvalue) = byte_count;
				}
			}
			return;
		} 
		if(Z_TYPE_P(zvalue) == IS_STRING) {
			if(Z_STRLEN_P(zvalue) != byte_count) {
				int8_t *memory = emalloc(byte_count + 1);
				memory[byte_count] = '\0';
				efree(Z_STRVAL_P(zvalue));
				Z_STRVAL_P(zvalue) = (char *) memory;
				Z_STRLEN_P(zvalue) = byte_count;
			}
		} 
		qb_copy_elements_to_zval(cxt, address, zvalue);
	}
}

static void qb_copy_elements_to_caller_address(qb_interpreter_context *cxt, qb_address *address, qb_storage *caller_storage, qb_address *caller_address) {
	uint32_t src_element_count = SCALAR(address) ? 1 : ARRAY_SIZE(address);
	uint32_t dst_element_count = SCALAR(caller_address) ? 1 : ARRAY_SIZE_IN(caller_storage, caller_address);
	qb_copy_elements(address->type, ARRAY(I08, address), src_element_count, caller_address->type, ARRAY_IN(caller_storage, I08, caller_address), dst_element_count);
}

static void qb_transfer_value_to_caller_storage(qb_interpreter_context *cxt, qb_address *address, qb_storage *caller_storage, qb_address *caller_address) {
	/* TODO
	if(caller_address->segment_selector >= QB_SELECTOR_DYNAMIC_ARRAY_START) {
		qb_memory_segment *segment = &cxt->storage->segments[address->segment_selector];
		qb_memory_segment *caller_segment = &caller_storage->segments[caller_address->segment_selector];

		if(segment->flags & QB_SEGMENT_BORROWED) {
			// as memory was borrowed from the caller, content changes are there already
			if(segment->flags & QB_SEGMENT_EXPANDABLE) {
				// see if the size needs to be updated
				if(caller_segment->byte_count != segment->byte_count) {
					caller_segment->byte_count = segment->byte_count;
					if(caller_address->dimension_count > 1) {
						// TODO: update the dimension as well
					}
				}
				if(caller_segment->memory != segment->memory) {
					// update the memory pointer as well in case it has changed
					caller_segment->memory = segment->memory;
				}
			}
			segment->flags &= ~QB_SEGMENT_BORROWED;
			segment->memory = NULL;
			segment->stream = NULL;
			return;
		} else {
			uint32_t element_start_index = caller_address->segment_offset;
			uint32_t element_count = SCALAR(address) ? 1 : ARRAY_SIZE(address);
			uint32_t byte_count = BYTE_COUNT(element_count, address->type);

			if(element_start_index + byte_count > caller_segment->byte_count) {
				if(caller_segment->flags & QB_SEGMENT_EXPANDABLE) {
					// expand the caller segment to accommodate
					qb_enlarge_segment(cxt, caller_segment, element_start_index + element_count);
				} else {
					qb_abort_range_error(cxt, caller_segment, element_start_index, element_count, cxt->function_call_line_number);
				}
			}
		}
	}
	qb_copy_elements_to_caller_address(cxt, address, caller_storage, caller_address);
	*/
}

static void qb_transfer_value_to_import_source(qb_interpreter_context *cxt, qb_variable_import *import) {
	USE_TSRM
	qb_variable *qvar = import->variable;

	// do the transfer only if it the variable could have been changed
	if(!READ_ONLY(qvar->address)) {
		if(import->previous_copy_index != -1) {
			// copy value to caller storage
			qb_variable_import *previous_copy = &cxt->variable_imports[import->previous_copy_index];
			qb_transfer_value_to_caller_storage(cxt, qvar->address, previous_copy->storage, previous_copy->variable->address);
		} else {
			zval *zvalue = NULL;
			if(import->value_pointer) {
				zvalue = *import->value_pointer;
			}
			if(!zvalue) {
				ALLOC_INIT_ZVAL(zvalue);
				if(!SCALAR(qvar->address)) {
					if(qvar->address->flags & QB_ADDRESS_STRING) {
						ZVAL_EMPTY_STRING(zvalue);
					} else {
						qb_initialize_zval_array(cxt, qvar->address, NULL, zvalue);
					}
				}
			}
			qb_transfer_value_to_zval(cxt, qvar->address, zvalue);

			if(!import->value_pointer) {
				if(qvar->flags & QB_VARIABLE_GLOBAL) {
					zend_hash_quick_update(&EG(symbol_table), qvar->name, qvar->name_length + 1, qvar->hash_value, (void **) &zvalue, sizeof(zval *), NULL);
				} else if(qvar->flags & QB_VARIABLE_CLASS_INSTANCE) {
					zval *container = cxt->this_object;
					zval *name = qb_string_to_zval(qvar->name, qvar->name_length TSRMLS_CC);
					Z_OBJ_WRITE_PROP(container, name, zvalue);
				}
				zval_ptr_dtor(&zvalue);
			}
		}
	}
}

static void qb_sync_imported_variables(qb_interpreter_context *cxt) {
	uint32_t i;
	for(i = 0; i < cxt->variable_import_count; i++) {
		qb_variable_import *import = &cxt->variable_imports[i];
		qb_transfer_value_to_import_source(cxt, import);
	}
}

static void qb_retire_imported_variable(qb_interpreter_context *cxt, qb_variable *qvar) {
	// assume the function will be called in the correct order
	qb_variable_import *import = &cxt->variable_imports[--cxt->variable_import_count];
	qb_transfer_value_to_import_source(cxt, import);
}

static void qb_transfer_arguments_to_php(qb_interpreter_context *cxt, zval *retval, zval ***p_args, int arg_count) {
	qb_function *func = cxt->function;
	uint32_t i;

	// copy value into return variable
	if(func->return_variable->address) {
		if(!SCALAR(func->return_variable->address)) {
			qb_initialize_zval_array(cxt, func->return_variable->address, NULL, retval);
		}
		qb_transfer_value_to_zval(cxt, func->return_variable->address, retval);
	}

	// copy values into arguments passed by reference and return variable
	for(i = 0; i < func->argument_count; i++) {
		qb_variable *qvar = func->variables[i];

		if(qvar->flags & QB_VARIABLE_ARGUMENT && qvar->flags & QB_VARIABLE_PASSED_BY_REF) {
			zval *zarg = *p_args[i];
			qb_transfer_value_to_zval(cxt, qvar->address, zarg);
		}
	}
}

static void qb_free_interpreter_context(qb_interpreter_context *cxt) {
	if(cxt->zend_arguments) {
		efree(cxt->zend_arguments);
		efree(cxt->zend_argument_pointers);
	}
	qb_destroy_array((void **) &cxt->variable_imports);
	qb_destroy_array((void **) &cxt->call_stack);
}

void qb_main(qb_interpreter_context *__restrict cxt, qb_function *__restrict function);

static void qb_initialize_interpreter_context(qb_interpreter_context *cxt TSRMLS_DC) {
	uint32_t i;

	memset(cxt, 0, sizeof(qb_interpreter_context));

	// use shadow variables for debugging purpose by default
	cxt->flags = QB_EMPLOY_SHADOW_VARIABLES;

	// set up addresses structure used during function calls
	cxt->array_address.mode = QB_ADDRESS_MODE_ARR;
	cxt->array_address.dimension_addresses = cxt->dimension_addresse_pointers;
	cxt->array_address.array_size_addresses = cxt->array_size_address_pointers;
	for(i = 0; i < MAX_DIMENSION; i++) {
		cxt->array_address.dimension_addresses[i] = &cxt->dimension_addresses[i];
		cxt->array_address.array_size_addresses[i] = &cxt->array_size_addresses[i];
	}
	cxt->array_address.array_size_address = cxt->array_address.array_size_addresses[0];

	cxt->floating_point_precision = EG(precision);
	cxt->line_number_pointer = &QB_G(current_line_number);
	//cxt->thread_pool = &QB_G(thread_pool);
#ifdef ZEND_WIN32
	cxt->windows_timed_out_pointer = &EG(timed_out);
#endif
	SAVE_TSRMLS
}

zend_class_entry *value_type_debug_base_class = NULL;
zend_class_entry *value_type_debug_classes[QB_TYPE_COUNT] = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, };

static zend_class_entry *qb_get_value_type_debug_class(qb_interpreter_context *cxt, uint32_t type) {
	USE_TSRM
	zend_class_entry ce;
	if(!value_type_debug_base_class) {
		INIT_CLASS_ENTRY(ce, "%QB%", NULL);
		value_type_debug_base_class = zend_register_internal_class_ex(&ce, zend_standard_class_def, NULL TSRMLS_CC);
	}
	if(!value_type_debug_classes[type]) {
		const char *type_name = type_names[type];
		INIT_CLASS_ENTRY_EX(ce, type_name, strlen(type_name), NULL);
		value_type_debug_classes[type] = zend_register_internal_class_ex(&ce, value_type_debug_base_class, NULL TSRMLS_CC);
	}
	return value_type_debug_classes[type];
}

static void qb_transfer_value_to_debug_zval(qb_interpreter_context *cxt, qb_address *address, zval *zvalue) {
	USE_TSRM
	if(SCALAR(address)) {
		if(address->flags & QB_ADDRESS_STRING) {
			zval_dtor(zvalue);
			ZVAL_STRINGL(zvalue, (char *) ARRAY(I08, address), 1, TRUE);
		} else {
			zval **p_value;
			int32_t store_in_object;
			if(QB_G(debug_with_exact_type)) {
				store_in_object = TRUE;
			} else {
				store_in_object = FALSE;
				switch(address->type) {
					case QB_TYPE_S08: {
						Z_TYPE_P(zvalue) = IS_LONG;
						Z_LVAL_P(zvalue) = VALUE(S08, address);
					}	break;
					case QB_TYPE_U08: {
						Z_TYPE_P(zvalue) = IS_LONG;
						Z_LVAL_P(zvalue) = VALUE(U08, address);
					}	break;
					case QB_TYPE_S16: {
						Z_TYPE_P(zvalue) = IS_LONG;
						Z_LVAL_P(zvalue) = VALUE(S16, address);
					}	break;
					case QB_TYPE_U16: {
						Z_TYPE_P(zvalue) = IS_LONG;
						Z_LVAL_P(zvalue) = VALUE(U16, address);
					}	break;
					case QB_TYPE_S32: {
						Z_TYPE_P(zvalue) = IS_LONG;
						Z_LVAL_P(zvalue) = VALUE(S32, address);
					}	break;
#if SIZEOF_LONG == 8
					case QB_TYPE_U32: {
						Z_TYPE_P(zvalue) = IS_LONG;
						Z_LVAL_P(zvalue) = VALUE(U32, address);
					}	break;
					case QB_TYPE_S64: {
						Z_TYPE_P(zvalue) = IS_LONG;
						Z_LVAL_P(zvalue) = VALUE(S64, address);
					}	break;
#endif
					case QB_TYPE_F32: {
						Z_TYPE_P(zvalue) = IS_DOUBLE;
						Z_DVAL_P(zvalue) = VALUE(F32, address);
					}	break;
					case QB_TYPE_F64: {
						Z_TYPE_P(zvalue) = IS_DOUBLE;
						Z_DVAL_P(zvalue) = VALUE(F64, address);
					}	break;
					default: {
						// the range is too large for a PHP int
						// create a object and store the value as a string instead
						store_in_object = TRUE;
					}
				}

			}
			if(store_in_object) {
				USE_TSRM
				if(Z_TYPE_P(zvalue) != IS_OBJECT) {
					// create an object and add a property by the name of "value"
					zval *name = qb_string_to_zval("value", 5 TSRMLS_CC);
					zend_class_entry *ce = qb_get_value_type_debug_class(cxt, address->type);
					object_init_ex(zvalue, ce);
					p_value = Z_OBJ_GET_PROP_PTR_PTR(zvalue, name);

					// create a string with a buffer large enough for any number
					SEPARATE_ZVAL(p_value);
					Z_TYPE_PP(p_value) = IS_STRING;
					Z_STRVAL_PP(p_value) = emalloc(MAX_LENGTH_OF_DOUBLE + 1);
				} else {
					// we know the object has only one property
					HashTable *ht = Z_OBJ_HT_P(zvalue)->get_properties(zvalue TSRMLS_CC);
					Bucket *p = ht->pListHead;
					p_value = (zval **) p->pData;
				}
				Z_STRLEN_PP(p_value) = qb_element_to_string(Z_STRVAL_PP(p_value), MAX_LENGTH_OF_DOUBLE, ARRAY(I08, address), address->type);
			}
		}
	} else {
		if(address->flags & QB_ADDRESS_STRING) {
			zval_dtor(zvalue);
			ZVAL_STRINGL(zvalue, (char *) ARRAY(I08, address), ARRAY_SIZE(address), TRUE);
		} else {
			uint32_t element_count = ARRAY_SIZE(address);
			if(element_count > 1024) {
				// too many elements--just show the number
				USE_TSRM
				zval **p_value;
				char buffer[64];
				uint32_t len = snprintf(buffer, sizeof(buffer), "(%d elements)", element_count);

				if(Z_TYPE_P(zvalue) != IS_OBJECT) {
					// create an object and add a property by the name of "value"
					zval *name = qb_string_to_zval("value", 5 TSRMLS_CC);
					zend_class_entry *ce = qb_get_value_type_debug_class(cxt, address->type);
					object_init_ex(zvalue, ce);
					p_value = Z_OBJ_GET_PROP_PTR_PTR(zvalue, name);

					SEPARATE_ZVAL(p_value);
				} else {
					// we know the object has only one property
					HashTable *ht = Z_OBJ_HT_P(zvalue)->get_properties(zvalue TSRMLS_CC);
					Bucket *p = ht->pListHead;
					p_value = (zval **) p->pData;
				}
				zval_dtor(*p_value);
				ZVAL_STRINGL(*p_value, buffer, len, TRUE);
			} else {
				uint32_t item_element_count, item_byte_count, i;
				uint32_t dimension = VALUE(U32, address->dimension_addresses[0]);
				qb_address _item_address, *item_address = &_item_address;

				item_address->flags = address->flags;
				item_address->type = address->type;
				item_address->segment_selector = address->segment_selector;
				item_address->segment_offset = address->segment_offset;
				item_address->dimension_count = address->dimension_count - 1;
				item_address->array_index_address = NULL;
				if(item_address->dimension_count > 0) {
					item_address->mode = QB_ADDRESS_MODE_ARR;
					item_address->dimension_addresses = address->dimension_addresses + 1;
					item_address->array_size_addresses = address->array_size_addresses + 1;
					item_address->array_size_address = item_address->array_size_addresses[0];
					if(address->index_alias_schemes) {
						item_address->index_alias_schemes = address->index_alias_schemes + 1;
					} else {
						item_address->index_alias_schemes = NULL;
					}
					item_element_count = VALUE(U32, item_address->array_size_address);
				} else {
					item_address->mode = QB_ADDRESS_MODE_ELE;
					item_address->dimension_addresses = NULL;
					item_address->array_size_addresses = NULL;
					item_address->array_size_address = NULL;
					item_address->array_index_address = NULL;
					item_address->index_alias_schemes = NULL;
					item_element_count = 1;
				}
				item_byte_count = BYTE_COUNT(item_element_count, address->type);

				if(Z_TYPE_P(zvalue) != IS_ARRAY) {
					array_init_size(zvalue, dimension);
				}
				for(i = 0; i < dimension; i++) {
					zval *item, **p_item;
					if(zend_hash_index_find(Z_ARRVAL_P(zvalue), i, (void **) &p_item) == SUCCESS) {
						item = *p_item;
					} else {
						ALLOC_INIT_ZVAL(item);
						zend_hash_index_update(Z_ARRVAL_P(zvalue), i, (void **) &item, sizeof(zval *), NULL);
					}
					qb_transfer_value_to_debug_zval(cxt, item_address, item);
					item_address->segment_offset += item_byte_count;
				}
			}
		}
	}
}

static void qb_create_shadow_variables(qb_interpreter_context *cxt) {
	USE_TSRM
	uint32_t i, j;
	zend_execute_data *ex = EG(current_execute_data);
	for(i = 0, j = 0; i < cxt->function->variable_count; i++) {
		qb_variable *qvar = cxt->function->variables[i];
		if(!(qvar->flags & (QB_VARIABLE_CLASS | QB_VARIABLE_CLASS_INSTANCE | QB_VARIABLE_RETURN_VALUE))) {
			zval **var, *value;
			ALLOC_INIT_ZVAL(value);
			qb_transfer_value_to_debug_zval(cxt, qvar->address, value);
			zend_hash_quick_update(ex->symbol_table, qvar->name, qvar->name_length + 1, qvar->hash_value, &value, sizeof(zval *), (void **) &var);
#if !ZEND_ENGINE_2_4 && !ZEND_ENGINE_2_3 && !ZEND_ENGINE_2_2 && !ZEND_ENGINE_2_1
			*EX_CV_NUM(ex, j) = var;
#else
			ex->CVs[j] = var;
#endif
			j++;

			if(qvar->flags & QB_VARIABLE_ARGUMENT) {
				// push argument onto Zend stack
				Z_ADDREF_P(value);
#if !ZEND_ENGINE_2_2 && !ZEND_ENGINE_2_1
				zend_vm_stack_push(value TSRMLS_CC);
#else
				zend_ptr_stack_push(&EG(argument_stack), value);
#endif
			}
		}
	}
	// push the argument count
#if !ZEND_ENGINE_2_4 && !ZEND_ENGINE_2_3 && !ZEND_ENGINE_2_2 && !ZEND_ENGINE_2_1
	ex->function_state.arguments = zend_vm_stack_top(TSRMLS_C);
	zend_vm_stack_push((void *) (zend_uintptr_t) cxt->function->argument_count TSRMLS_CC);
#elif !ZEND_ENGINE_2_2 && !ZEND_ENGINE_2_1
	ex->function_state.arguments = zend_vm_stack_push_args(cxt->function->argument_count TSRMLS_CC);
#else
	zend_ptr_stack_2_push(&EG(argument_stack), (void *) (zend_uintptr_t) cxt->function->argument_count, NULL);
#endif
}

static void qb_sync_shadow_variables(qb_interpreter_context *cxt) {
	USE_TSRM
	uint32_t i, j;
	zend_execute_data *ex = EG(current_execute_data);
	for(i = 0, j = 0; i < cxt->function->variable_count; i++) {
		qb_variable *qvar = cxt->function->variables[i];
		if(!(qvar->flags & (QB_VARIABLE_CLASS | QB_VARIABLE_CLASS_INSTANCE | QB_VARIABLE_RETURN_VALUE))) {
#if !ZEND_ENGINE_2_4 && !ZEND_ENGINE_2_3 && !ZEND_ENGINE_2_2 && !ZEND_ENGINE_2_1
			zval *zvalue = **EX_CV_NUM(ex, j);
#else
			zval *zvalue = *ex->CVs[j];
#endif
			qb_transfer_value_to_debug_zval(cxt, qvar->address, zvalue);
			j++;
		}
	}
}

static void qb_destroy_shadow_variables(qb_interpreter_context *cxt) {
	// Zend will actually perform the clean-up of the CVs
	// the only thing we need to do here is to put arguments that were passed by reference
	// back into the symbol table and pop them off the stack
	USE_TSRM
	uint32_t i, j, arg_count;
	zend_execute_data *ex = EG(current_execute_data);

	// pop the argument count
#if !ZEND_ENGINE_2_2 && !ZEND_ENGINE_2_1
	arg_count = (uint32_t) (zend_uintptr_t) zend_vm_stack_pop(TSRMLS_C);
#else
	zend_ptr_stack_pop(&EG(argument_stack));
	arg_count = (uint32_t) (zend_uintptr_t) zend_ptr_stack_pop(&EG(argument_stack));
#endif

	for(i = 0, j = 0; i < arg_count; i++) {
		qb_variable *qvar = cxt->function->variables[i];

		// pop argument off Zend stack
#if !ZEND_ENGINE_2_2 && !ZEND_ENGINE_2_1
		zval *argument = zend_vm_stack_pop(TSRMLS_C);
#else
		zval *argument = zend_ptr_stack_pop(&EG(argument_stack));
#endif
		if(qvar->flags & QB_VARIABLE_PASSED_BY_REF) {
			zval **var;
			Z_ADDREF_P(argument);
			zend_hash_quick_update(ex->symbol_table, qvar->name, qvar->name_length + 1, qvar->hash_value, &argument, sizeof(zval *), (void **) &var);

			// updating the CVs for consistency sake
#if !ZEND_ENGINE_2_4 && !ZEND_ENGINE_2_3 && !ZEND_ENGINE_2_2 && !ZEND_ENGINE_2_1
			*EX_CV_NUM(ex, j) = var;
#else
			ex->CVs[j] = var;
#endif
		}
		zval_ptr_dtor(&argument);
		j++;
	}
}

static qb_storage * qb_find_previous_storage(qb_interpreter_context *cxt, qb_function *qfunc) {
	int32_t i;
	for(i = cxt->call_stack_height - 1; i >= 0; i++) {
		qb_call_stack_item *call_stack_item = &cxt->call_stack[i];
		if(call_stack_item->function == qfunc) {
			return call_stack_item->storage;
		}
	}
	return qfunc->local_storage;
}

void qb_initialize_function_call(qb_interpreter_context *cxt, zend_function *zfunc, uint32_t argument_count, uint32_t line_number) {
	qb_call_stack_item *caller = NULL;
	if(cxt->function) {
		if(!cxt->call_stack) {
			qb_create_array((void **) &cxt->call_stack, &cxt->call_stack_height, sizeof(qb_call_stack_item), 8);
		}
		caller = qb_enlarge_array((void **) &cxt->call_stack, 1);
		caller->function = cxt->function;
		caller->storage = cxt->storage;
		caller->function_call_line_number = cxt->function_call_line_number;
	}
	cxt->zend_function = zfunc;
	cxt->argument_count = argument_count;
	cxt->function_call_line_number = line_number;
	if(qb_is_compiled_function(zfunc)) {
		qb_function *qfunc = zfunc->op_array.reserved[0];
		cxt->storage = qfunc->local_storage;
		cxt->function = qfunc;
	} else {
		// allocate space for arguments
		if(argument_count + 1 > cxt->zend_argument_buffer_size) {
			uint32_t new_buffer_size = argument_count + 1, i;
			cxt->zend_arguments = erealloc(cxt->zend_arguments, new_buffer_size * sizeof(zval *));
			cxt->zend_argument_pointers = erealloc(cxt->zend_argument_pointers, new_buffer_size * sizeof(zval **));
			for(i = cxt->zend_argument_buffer_size; i < new_buffer_size; i++) {
				cxt->zend_arguments[i] = NULL;
				cxt->zend_argument_pointers[i] = &cxt->zend_arguments[i];
			}
			cxt->zend_argument_buffer_size = new_buffer_size;
		}
		cxt->function = NULL;
	}
}

static zend_always_inline void qb_enter_vm(qb_interpreter_context *cxt) {
#ifdef NATIVE_COMPILE_ENABLED
	if(cxt->function->native_proc) {
		qb_native_proc proc = cxt->function->native_proc;
		if(proc(cxt) == FAILURE) {
			USE_TSRM
			if(QB_G(allow_bytecode_interpretation)) {
				qb_main(cxt);
			} else {
				qb_abort("Unable to run compiled procedure");
			}
		}
	} else {
		qb_main(cxt);
	}
#else
	qb_main(cxt, cxt->function);
#endif
}

extern zend_module_entry qb_module_entry;

#include "zend_extensions.h"
#include "zend_vm.h"

void qb_run_zend_extension_op(qb_interpreter_context *cxt, uint32_t zend_opcode, uint32_t line_number) {
	// see if a handler for the op exists before doing any of the hard work
	USE_TSRM
	int32_t handler_exists = FALSE;
	if(!EG(no_extensions)) {
		zend_llist_element *element;
		for(element = zend_extensions.head; element; element = element->next) {
			zend_extension *extension = (zend_extension *) element->data;
			switch(zend_opcode) {
				case ZEND_EXT_FCALL_BEGIN: {
					if(extension->fcall_begin_handler != NULL) {
						handler_exists = TRUE;
					}
				}	break;
				case ZEND_EXT_FCALL_END: {
					if(extension->fcall_end_handler != NULL) {
						handler_exists = TRUE;
					}
				}	break;
				case ZEND_EXT_STMT: {
					if(extension->statement_handler != NULL) {
						handler_exists = TRUE;
					}
				}	break;
			}
		}
	}

	if(handler_exists) {
		USE_TSRM
		zend_llist_element *element;
		zend_function *zfunc = cxt->function->zend_function;
		zend_op_array *op_array = EG(current_execute_data)->op_array;
		zend_op *opline = EG(current_execute_data)->opline;
		opline->opcode = zend_opcode;
		opline->lineno = line_number;

		zfunc->type = ZEND_USER_FUNCTION;
		zfunc->op_array.opcodes = opline;
		zfunc->op_array.last = 1;
		zfunc->op_array.T = 0;

		if(cxt->flags & QB_EMPLOY_SHADOW_VARIABLES) {
			qb_sync_imported_variables(cxt);
			qb_sync_shadow_variables(cxt);
		}

		for(element = zend_extensions.head; element; element = element->next) {
			zend_extension *extension = (zend_extension *) element->data;
			switch(zend_opcode) {
				case ZEND_EXT_FCALL_BEGIN: {
					if(extension->fcall_begin_handler != NULL) {
						extension->fcall_begin_handler(op_array);
					}
				}	break;
				case ZEND_EXT_FCALL_END: {
					if(extension->fcall_end_handler != NULL) {
						extension->fcall_end_handler(op_array);
					}
				}	break;
				case ZEND_EXT_STMT: {
					if(extension->statement_handler != NULL) {
						extension->statement_handler(op_array);
					}
				}	break;
			}
		}

		zfunc->type = ZEND_INTERNAL_FUNCTION;
		zfunc->internal_function.handler = PHP_FN(qb_execute);
#if !ZEND_ENGINE_2_1
		zfunc->internal_function.module = &qb_module_entry;
#endif
	}
}

int qb_user_opcode_handler(ZEND_OPCODE_HANDLER_ARGS) {
	zend_op *zop = execute_data->opline;
	qb_interpreter_context *cxt = (qb_interpreter_context *) Z_OPERAND_INFO(zop->op2, jmp_addr);
	zend_function *zfunc = cxt->function->zend_function;
	zend_uchar original_opcode;

	if(cxt->flags & QB_EMPLOY_SHADOW_VARIABLES) {
		qb_create_shadow_variables(cxt);
	}

	// turn it back into internal function
	zfunc->type = ZEND_INTERNAL_FUNCTION;
	zfunc->internal_function.handler = PHP_FN(qb_execute);
#if !ZEND_ENGINE_2_1
	zfunc->internal_function.module = &qb_module_entry;
#endif

	// change the opcode so it's FCALL
	original_opcode = zop->opcode;
	zop->opcode = ZEND_DO_FCALL;
	zop->lineno = cxt->function_call_line_number;

	// enter the QB virtual machine
	qb_enter_vm(cxt);

	if(cxt->flags & QB_EMPLOY_SHADOW_VARIABLES) {
		qb_destroy_shadow_variables(cxt);
	}

	// make it look like a user function before exiting, just in case
	zfunc->type = ZEND_USER_FUNCTION;
	zfunc->op_array.opcodes = zop;

	zop->opcode = original_opcode;

	return ZEND_USER_OPCODE_RETURN;
}

int32_t qb_user_opcode = -1;

static void qb_enter_vm_thru_zend(qb_interpreter_context *cxt) {
	USE_TSRM
	qb_function *qfunc = cxt->function;
	zend_function *zfunc = qfunc->zend_function;
	zend_op user_op, **opline_ptr;
	zval name;
	if(qb_user_opcode == -1) {
		// choose a user opcode that isn't in use
		uint32_t i;
		for(i = 255; i >= 200; i--) {
			if(!zend_get_user_opcode_handler(i)) {
				qb_user_opcode = i;
				break;
			}
		}
		// set the opcode handler
		zend_set_user_opcode_handler(qb_user_opcode, qb_user_opcode_handler);
	}
	if(qb_user_opcode != -1) {
		zend_execute_data *original_execute_data;

		memset(&user_op, 0, sizeof(zend_op));
		user_op.opcode = qb_user_opcode;

		// attach a name to the op so others can figure out what it is
		Z_OPERAND_TYPE(user_op.op1) = Z_OPERAND_CONST;
		Z_TYPE(name) = IS_STRING;
		Z_STRVAL(name) = "QBVM";
		Z_STRLEN(name) = 4;
#if !ZEND_ENGINE_2_3 && !ZEND_ENGINE_2_2 && !ZEND_ENGINE_2_1
		Z_OPERAND_ZV(user_op.op1) = &name;
#else
		memcpy(Z_OPERAND_ZV(user_op.op1), &name, sizeof(zval));
#endif

		Z_OPERAND_TYPE(user_op.op2) = Z_OPERAND_UNUSED;
		Z_OPERAND_INFO(user_op.op2, jmp_addr) = (zend_op *) cxt;
		Z_OPERAND_TYPE(user_op.result) = Z_OPERAND_UNUSED;
		zend_vm_set_opcode_handler(&user_op);

		// make the funcion look like a user function temporarily
		zfunc->type = ZEND_USER_FUNCTION;
		zfunc->op_array.opcodes = &user_op;
		zfunc->op_array.last = 1;
		zfunc->op_array.T = 0;
		zfunc->op_array.filename = cxt->function->filename;
		if(qfunc->variable_count) {
			uint32_t i, local_var_count = 0;
			zfunc->op_array.vars = emalloc(sizeof(zend_compiled_variable) * qfunc->variable_count);
			for(i = 0; i < qfunc->variable_count; i++) {
				qb_variable *qvar = qfunc->variables[i];
				if(!(qvar->flags & (QB_VARIABLE_CLASS | QB_VARIABLE_CLASS_INSTANCE | QB_VARIABLE_RETURN_VALUE))) {
					zend_compiled_variable *zvar = &zfunc->op_array.vars[local_var_count++];
					zvar->name = qvar->name;
					zvar->name_len = qvar->name_length;
					zvar->hash_value = qvar->hash_value;
				}
			}
			zfunc->op_array.last_var = local_var_count;
		}

		if(cxt->call_stack_height == 0) {
			original_execute_data = EG(current_execute_data);
			EG(current_execute_data) = original_execute_data->prev_execute_data;
			cxt->function_call_line_number = original_execute_data->opline->lineno;
		}

		// run it through zend_execute
		opline_ptr = EG(opline_ptr);
		zend_execute(&zfunc->op_array TSRMLS_CC);
		EG(opline_ptr) = opline_ptr;

		if(cxt->call_stack_height == 0) {
			EG(current_execute_data) = original_execute_data;
		}

		// revert back to internal function
		zfunc->type = ZEND_INTERNAL_FUNCTION;
		zfunc->internal_function.handler = PHP_FN(qb_execute);
#if !ZEND_ENGINE_2_1
		zfunc->internal_function.module = &qb_module_entry;
#endif
		if(qfunc->variable_count) {
			efree(zfunc->op_array.vars);
			zfunc->op_array.vars = NULL;
			zfunc->op_array.last_var = 0;
		}
	} else {
		qb_enter_vm(cxt);
	}
}

void qb_execute_function_call(qb_interpreter_context *cxt) {
	if(cxt->function) {
		int8_t *memory;
		uint32_t combined_byte_count, i;
		
		// the following optimization depends very much on how the segments are laid out
		memory = cxt->storage->segments[QB_SELECTOR_SHARED_SCALAR].memory;
		combined_byte_count = cxt->storage->segments[QB_SELECTOR_SHARED_SCALAR].byte_count + cxt->storage->segments[QB_SELECTOR_LOCAL_SCALAR].byte_count;
		memset(memory, 0, combined_byte_count);
		memory = cxt->storage->segments[QB_SELECTOR_LOCAL_ARRAY].memory;
		combined_byte_count = cxt->storage->segments[QB_SELECTOR_LOCAL_ARRAY].byte_count + cxt->storage->segments[QB_SELECTOR_SHARED_ARRAY].byte_count;
		memset(memory, 0, combined_byte_count);

		// process the other segments
		for(i = QB_SELECTOR_ARRAY_START; i < cxt->storage->segment_count; i++) {
			qb_memory_segment *segment = &cxt->storage->segments[i];
			if(segment->byte_count) {
				if(segment->byte_count < segment->current_allocation) {
					qb_resize_segment(cxt, segment, segment->byte_count);
					segment->current_allocation = segment->byte_count;
				}
				if(segment->flags & QB_SEGMENT_CLEAR_ON_CALL) {
					memset(segment->memory, 0, segment->byte_count);
				}
			}
		}

		for(i = cxt->argument_count; i < cxt->function->argument_count; i++) {
			qb_variable *qvar = cxt->function->variables[i];
			if(qvar->default_value_address) {
				uint32_t transfer_flags;
				if((qvar->flags & QB_VARIABLE_PASSED_BY_REF) || !READ_ONLY(qvar->address)) {
					transfer_flags = 0;
				} else {
					transfer_flags = QB_TRANSFER_CAN_BORROW_MEMORY;
				}
				// copying from the function's own storage, actually
				qb_transfer_value_from_caller_storage(cxt, cxt->storage, qvar->default_value_address, qvar->address, transfer_flags);
			}
		}
		for(i = cxt->function->argument_count; i < cxt->function->variable_count; i++) {
			qb_variable *qvar = cxt->function->variables[i];
			if(qvar->flags & (QB_VARIABLE_GLOBAL | QB_VARIABLE_CLASS_INSTANCE | QB_VARIABLE_CLASS | QB_VARIABLE_CLASS_CONSTANT)) {
				// import external variables
				qb_import_variable(cxt, qvar);
			}
		}

		if(cxt->function->flags & QB_ENGINE_GO_THRU_ZEND) {
			// use a Zend op to activate the function, so a user-function environment is created
			qb_enter_vm_thru_zend(cxt);
		} else {
			// run the function normally
			qb_enter_vm(cxt);
		}

		// copy values to imported variables and remove them
		for(i = cxt->function->argument_count; i < cxt->function->variable_count; i++) {
			qb_variable *qvar = cxt->function->variables[i];
			if(qvar->flags & (QB_VARIABLE_GLOBAL | QB_VARIABLE_CLASS | QB_VARIABLE_CLASS_INSTANCE)) {
				qb_retire_imported_variable(cxt, qvar);
			}
		}
	} else {
		USE_TSRM
		zval *retval;
		zend_fcall_info fci;
		zend_fcall_info_cache fcc;

		// copy global and class variables back to PHP first
		qb_sync_imported_variables(cxt);

		fcc.calling_scope = (cxt->this_object) ? Z_OBJCE_P(cxt->this_object) : NULL;
		fcc.function_handler = cxt->zend_function;
		fcc.initialized = 1;
		fci.size = sizeof(zend_fcall_info);
#if !ZEND_ENGINE_2_2 && !ZEND_ENGINE_2_1
		if(cxt->zend_function->common.scope) {
			fci.function_table = &cxt->zend_function->common.scope->function_table;
			if((cxt->zend_function->common.fn_flags & ZEND_ACC_STATIC) || !cxt->this_object) {
				fci.object_ptr = fcc.object_ptr = NULL;
			} else {
				fci.object_ptr = fcc.object_ptr = cxt->this_object;
			}
			fcc.called_scope = cxt->zend_function->common.scope;
		} else {
			fci.function_table = EG(function_table);
			fci.object_ptr = fcc.object_ptr = NULL;
		}
#else 
		if(cxt->zend_function->common.scope) {
			fci.function_table = &cxt->zend_function->common.scope->function_table;
			if((cxt->zend_function->common.fn_flags & ZEND_ACC_STATIC) || !cxt->this_object) {
				fci.object_pp = fcc.object_pp = NULL;
			} else {
				fci.object_pp = fcc.object_pp = &cxt->this_object;
			}
		} else {
			fci.function_table = EG(function_table);
			fci.object_pp = fcc.object_pp = NULL;
		}
#endif
		fci.function_name = qb_cstring_to_zval(cxt->zend_function->common.function_name TSRMLS_CC);
		fci.retval_ptr_ptr = &retval;
		fci.param_count = cxt->argument_count;
		fci.params = cxt->zend_argument_pointers;
		fci.no_separation = 0;
		fci.symbol_table = NULL;

		if(zend_call_function(&fci, &fcc TSRMLS_CC) == SUCCESS) {
			if(EG(exception)) {
				cxt->exception_encountered = TRUE;
			}
			cxt->zend_arguments[cxt->argument_count] = retval;
		} else {
			qb_abort("Internal error");
		}

		// copy values of global and class variables from PHP again
		qb_update_imported_variables(cxt);
	}
}

void qb_finalize_function_call(qb_interpreter_context *cxt) {
	if(cxt->function) {
		uint32_t i;
		for(i = QB_SELECTOR_ARRAY_START; i < cxt->storage->segment_count; i++) {
			qb_memory_segment *segment = &cxt->storage->segments[i];
			if(segment->flags & QB_SEGMENT_FREE_ON_RETURN) {
				qb_free_segment(cxt, segment);
				if(segment->flags & QB_SEGMENT_EMPTY_ON_RETURN) {
					segment->byte_count = 0;
				}
			}
		}
	} else {
		uint32_t i;
		for(i = 0; i < cxt->argument_count + 1; i++) {
			if(cxt->zend_arguments[i]) {
				zval_ptr_dtor(&cxt->zend_arguments[i]);
				cxt->zend_arguments[i] = NULL;
			}
		}
	}
	if(cxt->call_stack_height) {
		qb_call_stack_item *caller = &cxt->call_stack[--cxt->call_stack_height];
		cxt->function = caller->function;
		cxt->storage = caller->storage;
		cxt->function_call_line_number = caller->function_call_line_number;
	}
}

void qb_copy_argument(qb_interpreter_context *cxt, uint32_t argument_index) {
	if(cxt->function) {
		if(argument_index < cxt->function->argument_count) {
			qb_variable *qvar = cxt->function->variables[argument_index];
			qb_call_stack_item *caller = &cxt->call_stack[cxt->call_stack_height - 1];
			uint32_t transfer_flags = 0;
			if(qvar->flags & QB_VARIABLE_PASSED_BY_REF) {
				transfer_flags = QB_TRANSFER_CAN_BORROW_MEMORY | QB_TRANSFER_CAN_ENLARGE_SEGMENT;
			} else if(READ_ONLY(qvar->address)) {
				transfer_flags = QB_TRANSFER_CAN_BORROW_MEMORY;
			} else {
				transfer_flags = 0;
			}
			qb_transfer_value_from_caller_storage(cxt, caller->storage, cxt->argument_address, qvar->address, transfer_flags);
		}
	} else {
		zval **p_zvalue = &cxt->zend_arguments[argument_index];
		ALLOC_INIT_ZVAL(*p_zvalue);
		qb_transfer_value_to_zval(cxt, cxt->argument_address, *p_zvalue);
	}
}

void qb_resync_argument(qb_interpreter_context *cxt, uint32_t argument_index) {
	if(cxt->function) {
		qb_variable *qvar = cxt->function->variables[argument_index];
		qb_call_stack_item *caller = &cxt->call_stack[cxt->call_stack_height - 1];
		if((qvar->flags & (QB_VARIABLE_PASSED_BY_REF | QB_VARIABLE_RETURN_VALUE)) && qvar->address) {
			qb_transfer_value_to_caller_storage(cxt, qvar->address, caller->storage, cxt->argument_address);
		}
	} else {
		if((cxt->zend_function->common.arg_info && argument_index < cxt->zend_function->common.num_args && cxt->zend_function->common.arg_info[argument_index].pass_by_reference)
		|| (argument_index == cxt->argument_count && cxt->zend_arguments[argument_index])) {
			zval *zvalue = cxt->zend_arguments[argument_index];
			uint32_t transfer_flags = 0;
			if(Z_TYPE_P(zvalue) == IS_STRING) {
				if(Z_REFCOUNT_P(zvalue) == 1) {
#ifdef IS_INTERNED
					if(!IS_INTERNED(Z_STRVAL_P(zvalue))) {
						transfer_flags = QB_TRANSFER_CAN_SEIZE_MEMORY;
					}
#else
					transfer_flags = QB_TRANSFER_CAN_SEIZE_MEMORY;
#endif

				}
			}
			qb_transfer_value_from_zval(cxt, zvalue, cxt->argument_address, transfer_flags);
		}
	}
}

int qb_execute(zend_function *zfunc, zval *this, zval ***arguments, int argument_count, zval *return_value TSRMLS_DC) {
	int result = SUCCESS;
	qb_interpreter_context _cxt, *cxt = &_cxt;
	qb_initialize_interpreter_context(cxt TSRMLS_CC);

	EG(scope) = zfunc->common.scope;

	// set up the local storage (pass zero as argument count, as
	// arguments handling actually happens below
	qb_initialize_function_call(cxt, zfunc, 0, (uint32_t) -1);

	// copy values from arguments and global symbol table
	qb_transfer_arguments_from_php(cxt, this, return_value, arguments, argument_count);

	// execute the call
	qb_execute_function_call(cxt);

	// copy values back to pass-by-ref arguments and global variables
	qb_transfer_arguments_to_php(cxt, return_value, arguments, argument_count);

	// clean up the local storage
	qb_finalize_function_call(cxt);

	qb_free_interpreter_context(cxt);
	return result;
}

void qb_execute_internal(qb_function *qfunc TSRMLS_DC) {
	qb_interpreter_context _cxt, *cxt = &_cxt;
	unsigned char windows_time_out = 0;
	SAVE_TSRMLS
	cxt->windows_timed_out_pointer = &windows_time_out;
	cxt->function = qfunc;
	cxt->storage = qfunc->local_storage;
	cxt->argument_count = 0;
	qb_execute_function_call(cxt);
}

int qb_initialize_interpreter(TSRMLS_D) {
#ifdef NATIVE_COMPILE_ENABLED
	uint32_t i;
	// calculate hash for faster lookup
	for(i = 0; i < global_native_symbol_count; i++) {
		qb_native_symbol *symbol = &global_native_symbols[i];
		symbol->hash_value = zend_inline_hash_func(symbol->name, strlen(symbol->name) + 1);
	}
#endif
#ifndef _MSC_VER
	qb_main(NULL, NULL);
#endif
	return SUCCESS;
}

intptr_t qb_resize_array(qb_interpreter_context *__restrict cxt, qb_storage *__restrict storage, uint32_t segment_selector, uint32_t new_size) {
	qb_memory_segment *segment = &storage->segments[segment_selector];
	segment->byte_count = new_size;
	if(new_size > segment->current_allocation) {
		int8_t *original_location = segment->memory, *new_location;
		segment->current_allocation = ALIGN_TO(new_size, 1024);
		qb_resize_segment(cxt, segment, new_size);
		new_location = segment->memory;
		if(new_location != original_location) {
			uint32_t i;
			uint32_t diff = new_location - original_location;
			for(i = 0; i < segment->reference_count; i++) {
				uintptr_t *p_ref = segment->references[i];
				*p_ref += diff;
			}
			return diff;
		}
	}
	return 0;
}

void qb_dispatch_instruction_to_threads(qb_interpreter_context *cxt, void *control_func, int8_t **instruction_pointers) {
	uint32_t i;
	uint32_t count = cxt->thread_count_for_next_op;
	cxt->thread_count_for_next_op = 0;

	if(!cxt->thread_pool->thread_count) {
		//qb_initialize_thread_pool(cxt->thread_pool);
	}
	for(i = 0; i < count; i++)  {
		int8_t *ip = instruction_pointers[i];
		//qb_schedule_task(cxt->thread_pool, control_func, cxt, ip);
	}
	//qb_run_tasks(cxt->thread_pool);
}
