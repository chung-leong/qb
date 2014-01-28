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

static intptr_t qb_relocate_segment_memory(qb_memory_segment *segment, int8_t *new_location) {
	if(segment->memory != new_location) {
		// adjust references in code
		uint32_t i;
		intptr_t diff = new_location - segment->memory;
		for(i = 0; i < segment->reference_count; i++) {
			uintptr_t *p_ref = segment->references[i];
			*p_ref += diff;
		}
		segment->memory = new_location;

		// relocate other segments that point to the same location
		if(segment->next_dependent) {
			qb_relocate_segment_memory(segment->next_dependent, new_location);
		}

		// return the shift in location
		return diff;
	}
	return 0;
}

void qb_import_segment(qb_memory_segment *segment, qb_memory_segment *other_segment) {
	if(other_segment->imported_segment) {
		// if the target segment has imported another segment, import the same one instead
		qb_import_segment(segment, other_segment->imported_segment);
	} else {
		qb_relocate_segment_memory(segment, other_segment->memory);

		segment->flags |= QB_SEGMENT_IMPORTED;
		segment->imported_segment = other_segment;
		segment->next_dependent = other_segment->next_dependent;
		other_segment->next_dependent = segment;
	}
}

static void qb_allocate_segment_memory_in_main_thread(void *param1, void *param2, int param3) {
	qb_allocate_segment_memory(param1, param3);
}

void qb_allocate_segment_memory(qb_memory_segment *segment, uint32_t byte_count) {
	if(segment->flags & QB_SEGMENT_IMPORTED) {
		qb_allocate_segment_memory(segment->imported_segment, byte_count);
	} else {
		if(byte_count > segment->current_allocation) {
			if(qb_in_main_thread()) {
				uint32_t new_allocation = ALIGN_TO(byte_count, 1024);
				uint32_t extra = new_allocation - byte_count;
				int8_t *memory = (segment->current_allocation) ? erealloc(segment->memory, new_allocation): emalloc(new_allocation);
				int8_t *data_end = memory + byte_count;
				segment->current_allocation = new_allocation;
				memset(data_end, 0, extra);
				qb_relocate_segment_memory(segment, memory);
				segment->byte_count = byte_count;
			} else {
				qb_run_in_main_thread(qb_allocate_segment_memory_in_main_thread, segment, NULL, byte_count);
			}
		}
	}
}

static int32_t qb_connect_segment_to_memory(qb_memory_segment *segment, int8_t *memory, uint32_t byte_count, uint32_t bytes_available, int32_t ownership) {	
	if(segment->flags & QB_SEGMENT_IMPORTED) {
		return qb_connect_segment_to_memory(segment->imported_segment, memory, byte_count, bytes_available, ownership);
	} else {
		if(byte_count <= bytes_available) {
			segment->byte_count = byte_count;
			segment->current_allocation = bytes_available;
			if(!ownership) {
				segment->flags |= QB_SEGMENT_BORROWED;
			}
			if(bytes_available) {
				qb_relocate_segment_memory(segment, memory);
			}
			return TRUE;
		} else {
			return FALSE;
		}
	}
}

static void * qb_map_file_to_memory(php_stream *stream, uint32_t byte_count, int32_t write_access TSRMLS_DC);
static void qb_unmap_file_from_memory(php_stream *stream TSRMLS_DC);

static int32_t qb_connect_segment_to_file(qb_memory_segment *segment, php_stream *stream, uint32_t byte_count, int32_t write_access) {
	if(segment->flags & QB_SEGMENT_IMPORTED) {
		return qb_connect_segment_to_file(segment->imported_segment, stream, byte_count, write_access);
	} else {
		TSRMLS_FETCH();
		uint32_t bytes_available = byte_count;
		int8_t *memory;
		if(!bytes_available) {
			bytes_available = 1024;
		}
		memory = qb_map_file_to_memory(stream, bytes_available, write_access TSRMLS_CC);
		if(memory) {
			if(!qb_connect_segment_to_memory(segment, memory, byte_count, bytes_available, FALSE)) {
				qb_unmap_file_from_memory(stream TSRMLS_CC);
				return FALSE;
			}
			segment->flags |= QB_SEGMENT_MAPPED;
			segment->stream = stream;
			return TRUE;
		} else {
			return FALSE;
		}
	}
}

static void qb_release_segment_in_main_thread(void *param1, void *param2, int param3) {
	qb_release_segment(param1);
}

void qb_release_segment(qb_memory_segment *segment) {
	if(segment->flags & QB_SEGMENT_IMPORTED) {
		qb_memory_segment *other_segment = segment->imported_segment;
		segment->flags &= ~QB_SEGMENT_IMPORTED;
		other_segment->next_dependent = segment->next_dependent;
		segment->next_dependent = NULL;
		segment->imported_segment = NULL;
	} else if((segment->flags & QB_SEGMENT_BORROWED) && !(segment->flags & QB_SEGMENT_MAPPED)) {
		// the memory was borrowed--nothing needs to be done
		segment->flags &= ~QB_SEGMENT_BORROWED;
	} else {
		if(qb_in_main_thread()) {
			if(segment->flags & QB_SEGMENT_MAPPED) {
				TSRMLS_FETCH();
				qb_unmap_file_from_memory(segment->stream TSRMLS_CC);

				// set the file to the actual size if more bytes were allocated than needed
				if(segment->current_allocation != segment->byte_count) {
					php_stream_truncate_set_size(segment->stream, segment->byte_count);
				}
				segment->flags &= ~(QB_SEGMENT_BORROWED | QB_SEGMENT_MAPPED);
				segment->stream = NULL;
			} else {
				if(segment->current_allocation > 0) {
					efree(segment->memory);
				}
			}
		} else {
			if((segment->flags & QB_SEGMENT_MAPPED) || segment->current_allocation > 0) {
				qb_run_in_main_thread(qb_release_segment_in_main_thread, segment, NULL, 0);
				return;
			}
		}
	} 
	// note: segment->memory is not set to NULL
	// because we need the value for relocation
	segment->current_allocation = 0;
}

void qb_resize_segment_in_main_thread(void *param1, void *param2, int param3) {
	intptr_t *p_offset = param2;
	*p_offset = qb_resize_segment(param1, param3);
}

intptr_t qb_resize_segment(qb_memory_segment *segment, uint32_t new_size) {
	if(segment->flags & QB_SEGMENT_IMPORTED) {
		return qb_resize_segment(segment->imported_segment, new_size);
	}
	if(new_size > segment->current_allocation) {
		if(qb_in_main_thread()) {
			int8_t *current_data_end;
			int8_t *memory;
			uint32_t new_allocation = ALIGN_TO(new_size, 4);
			uint32_t addition = new_allocation - segment->current_allocation;

			if(segment->flags & QB_SEGMENT_MAPPED) {
				// unmap the file, enlarge it, then map it again
				TSRMLS_FETCH();
				qb_unmap_file_from_memory(segment->stream TSRMLS_CC);
				php_stream_truncate_set_size(segment->stream, new_allocation);
				memory = qb_map_file_to_memory(segment->stream, new_allocation, TRUE TSRMLS_CC);
				if(!memory) {
					// shouldn't really happen--we had managed to map it successfully after all
					// can use NULL for the thread since this always run in the main thread
					qb_report_memory_map_exception(0, segment->stream->orig_path);
					qb_dispatch_exceptions(TSRMLS_C);
				}
			} else {
				int8_t *allocated_memory;
				// segment->memory is valid only when current_allocation > 0
				if(segment->current_allocation > 0) {
					allocated_memory = segment->memory;
				} else {
					allocated_memory = NULL;
				}
				memory = erealloc(allocated_memory, new_allocation);
			}

			// clear the newly allcoated bytes
			current_data_end = memory + segment->byte_count;
			memset(current_data_end, 0, addition);
			segment->byte_count = new_size;
			segment->current_allocation = new_allocation;
			return qb_relocate_segment_memory(segment, memory);
		} else {
			intptr_t offset;
			qb_run_in_main_thread(qb_resize_segment_in_main_thread, segment, &offset, new_size);
			return offset;
		}
	} else {
		segment->byte_count = new_size;
	}
	return 0;
}

qb_storage * qb_create_storage_copy(qb_storage *base, intptr_t instruction_shift, int32_t reentrance) {
	qb_storage *storage;
	intptr_t shift;
	uint32_t i;

	storage = emalloc(base->size);
	memcpy(storage, base, base->size);

	// adjust pointers
	shift = (uintptr_t) storage - (uintptr_t) base;
	SHIFT_POINTER(storage->segments, shift);
	for(i = 0; i < storage->segment_count; i++) {
		qb_memory_segment *src = &base->segments[i];
		qb_memory_segment *dst = &storage->segments[i];
		int separation;

		if(dst->flags & QB_SEGMENT_SEPARATE_ON_FORK && !reentrance) {
			separation = TRUE;
		} else if((dst->flags & QB_SEGMENT_SEPARATE_ON_REENTRY) && reentrance) {
			separation = TRUE;
		} else {
			separation = FALSE;
		}

		if(dst->flags & QB_SEGMENT_PREALLOCATED) {
			if(separation) {
				SHIFT_POINTER(dst->memory, shift);
			}
		} else {
			if(dst->references) {
				// only adjust the pointer--the references themselves will get shifted during relocation
				SHIFT_POINTER(dst->references, shift);
			}

			if(separation) {
				if(dst->flags & QB_SEGMENT_IMPORTED) {
					// unattach it
					dst->flags &= ~QB_SEGMENT_IMPORTED;
					dst->imported_segment = NULL;
					dst->next_dependent = NULL;
				} else {
					if((dst->flags & QB_SEGMENT_EMPTY_ON_RETURN) && reentrance) {
						dst->byte_count = 0;
					}
					if((dst->flags & QB_SEGMENT_REALLOCATE_ON_CALL) && reentrance) {
						// the memory will be alloc'ed when the copy is called
						dst->current_allocation = 0;
					} else {
						if(dst->byte_count) {
							// allocate new memory for the segment
							int8_t *new_memory = emalloc(dst->byte_count);
							if(!reentrance) {
								// forking--need to copy the contents over
								memcpy(new_memory, dst->memory, dst->byte_count);
							}
							qb_relocate_segment_memory(dst, new_memory);
						}
						dst->current_allocation = dst->byte_count;
					}
				}
			} else {
				dst->byte_count = 0;
				dst->current_allocation = 0;
				qb_import_segment(dst, src);
			}
		}
	}
	return storage;
}

void qb_copy_storage_contents(qb_storage *src_storage, qb_storage *dst_storage) {
	qb_memory_segment *src_segment_start, *src_segment_end, *dst_segment_start;
	uint32_t i, byte_count;

	// copy the preallocated segments
	src_segment_start = &src_storage->segments[QB_SELECTOR_LOCAL_SCALAR];
	src_segment_end = &src_storage->segments[QB_SELECTOR_LOCAL_ARRAY];
	dst_segment_start = &dst_storage->segments[QB_SELECTOR_LOCAL_SCALAR];
	byte_count = (uint32_t) ((src_segment_end->memory + src_segment_end->byte_count) - src_segment_start->memory);
	memcpy(dst_segment_start->memory, src_segment_start->memory, byte_count);

	for(i = QB_SELECTOR_ARRAY_START; i < src_storage->segment_count; i++) {
		qb_memory_segment *src = &src_storage->segments[i];
		qb_memory_segment *dst = &dst_storage->segments[i];
		if(dst->memory != src->memory) {
			// assume that the destination buffer is large enough
#ifdef ZEND_DEBUG
			if(dst->current_allocation < src->byte_count) {
				qb_debug_abort("Error");
			}
#endif
			memcpy(dst->memory, src->memory, src->byte_count);
			dst->byte_count = src->byte_count;
		}
	}
}

static uint32_t qb_get_zval_array_size(zval *zvalue) {
	HashTable *ht = Z_ARRVAL_P(zvalue);
	return ht->nNextFreeElement;
}

static void qb_initialize_element_address(qb_address *address, qb_address *container_address) {
	*address = *container_address;
	if(--address->dimension_count > 0) {
		address->dimension_addresses++;
		address->array_size_addresses++;
		address->array_size_address = address->array_size_addresses[0];
		if(address->index_alias_schemes) {
			address->index_alias_schemes++;
		}
	} else {
		address->array_size_address = NULL;
		address->dimension_addresses = address->array_size_addresses = NULL;
	}
}

static int32_t qb_capture_dimensions_from_byte_count(uint32_t byte_count, qb_dimension_mappings *m, uint32_t dimension_index) {
	uint32_t element_count = ELEMENT_COUNT(byte_count, m->dst_element_type);
	uint32_t correct_byte_count = BYTE_COUNT(element_count, m->dst_element_type);

	if(dimension_index + 1 > MAX_DIMENSION) {
		// TODO: report error
		return FALSE;
	}
	// make sure the number of bytes is a multiple of the element size
	if(byte_count != correct_byte_count) {
		qb_report_binary_string_size_mismatch_exception(0, byte_count, m->dst_element_type);
	}
	if(m->src_dimensions[dimension_index] < element_count) {
		m->src_dimensions[dimension_index] = element_count;
	}
	m->src_dimension_count = dimension_index + 1;
	return TRUE;
}

static int32_t qb_capture_dimensions_from_string(zval *zvalue, qb_dimension_mappings *m, uint32_t dimension_index) {
	return qb_capture_dimensions_from_byte_count(Z_STRLEN_P(zvalue), m, dimension_index);
}

static int32_t qb_capture_dimensions_from_zval(zval *zvalue, qb_dimension_mappings *m, uint32_t dimension_index);

static int32_t qb_capture_dimensions_from_array(zval *zarray, qb_dimension_mappings *m, uint32_t dimension_index) {
	HashTable *ht = Z_ARRVAL_P(zarray);
	Bucket *p;
	uint32_t dimension = ht->nNextFreeElement;
	if(dimension_index + 1 > MAX_DIMENSION) {
		// TODO: report error
		return FALSE;
	}
	if(m->src_dimensions[dimension_index] < dimension) {
		m->src_dimensions[dimension_index] = dimension;
	}
	if(m->src_dimension_count < dimension_index + 1) {
		m->src_dimension_count = dimension_index + 1;
	}
	for(p = ht->pListHead; p; p = p->pListNext) {
		if(p->h >= 0 && !p->arKey) {
			zval **p_element = p->pData;
			qb_capture_dimensions_from_zval(*p_element, m, dimension_index + 1);
		}
	}
	return TRUE;
}

static int32_t qb_capture_dimensions_from_object(zval *zobject, qb_dimension_mappings *m, uint32_t dimension_index) {
	qb_index_alias_scheme *scheme = m->dst_index_alias_schemes[dimension_index];
	uint32_t i;
	TSRMLS_FETCH();
	if(dimension_index + 1 > MAX_DIMENSION) {
		// TODO: report error
		return FALSE;
	}
	if(!scheme) {
		qb_report_illegal_conversion_to_array_exception(0, "object");
	}
	if(m->src_dimensions[dimension_index] < scheme->dimension) {
		m->src_dimensions[dimension_index] = scheme->dimension;
	}
	m->src_dimension_count = dimension_index + 1;
	for(i = 0; i < scheme->dimension; i++) {
		zval **p_element, *element = NULL;
		zval *alias = qb_cstring_to_zval(scheme->aliases[i] TSRMLS_CC);
		p_element = Z_OBJ_GET_PROP_PTR_PTR(zobject, alias);

		if(p_element) {
			element = *p_element;
		} else if(Z_OBJ_HT_P(zobject)->read_property) {
			element = Z_OBJ_READ_PROP(zobject, alias);
		}
		if(element) {
			qb_capture_dimensions_from_zval(element, m, dimension_index + 1);
		}
		if(!p_element && element) {
			// value from __get()
			Z_ADDREF_P(element);
			zval_ptr_dtor(&element);
		}
	}
	return TRUE;
}

void qb_copy_wrap_around(int8_t *memory, uint32_t filled_byte_count, uint32_t required_byte_count) {
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

void qb_fill_array_gap(int8_t *memory, uint32_t filled_byte_count, uint32_t required_byte_count, uint32_t dimension_index) {
	if(dimension_index == 0) {
		qb_copy_wrap_around(memory, filled_byte_count, required_byte_count);
	} else {
		memset(memory + filled_byte_count, 0, required_byte_count - filled_byte_count);
	}
}

#include "qb_storage_file.c"
#include "qb_storage_gd_image.c"

static int32_t qb_capture_dimensions_from_zval(zval *zvalue, qb_dimension_mappings *m, uint32_t dimension_index) {
	switch(Z_TYPE_P(zvalue)) {
		case IS_CONSTANT_ARRAY:
		case IS_ARRAY: {
			return qb_capture_dimensions_from_array(zvalue, m, dimension_index);
		}
		case IS_OBJECT: {
			return qb_capture_dimensions_from_object(zvalue, m, dimension_index);
		}	
		case IS_STRING: {
			return qb_capture_dimensions_from_string(zvalue, m, dimension_index);
		}	
		case IS_NULL:
		case IS_LONG:
		case IS_BOOL:
		case IS_DOUBLE: {
			return TRUE;
		}
		case IS_RESOURCE: {
			gdImagePtr image;
			php_stream *stream;
			
			if((image = qb_get_gd_image(zvalue))) {
				return qb_capture_dimensions_from_image(image, m, dimension_index);
			} else if((stream = qb_get_file_stream(zvalue))) {
				return qb_capture_dimensions_from_file(stream, m, dimension_index);
			} else {
				qb_report_illegal_conversion_to_array_exception(0, "resource");
				return FALSE;
			}
		}	break;
		default: {
			return FALSE;
		}
	} 
}

static int32_t qb_add_destination_dimensions(qb_storage *storage, qb_address *address, qb_dimension_mappings *m) {
	uint32_t i;
	m->dst_dimension_count = address->dimension_count;
	m->dst_element_type = address->type;
	for(i = 0; i < address->dimension_count; i++) {
		qb_address *dimension_address = address->dimension_addresses[i];
		qb_address *array_size_address = address->array_size_addresses[i];
		if(CONSTANT(dimension_address)) {
			m->dst_dimensions[i] = VALUE_IN(storage, U32, dimension_address);
		} else {
			m->dst_dimensions[i] = 0;
		}
		if(CONSTANT(array_size_address)) {
			m->dst_array_sizes[i] = VALUE_IN(storage, U32, dimension_address);
		} else {
			m->dst_array_sizes[i] = 0;
		}
		if(address->index_alias_schemes) {
			m->dst_index_alias_schemes[i] = address->index_alias_schemes[i];
		} else {
			m->dst_index_alias_schemes[i] = NULL;
		}
	}
	return TRUE;
}

static int32_t qb_add_source_dimensions_from_zval(zval *zvalue, qb_dimension_mappings *m) {
	uint32_t i;
	m->src_dimension_count = 0;
	for(i = 0; i < MAX_DIMENSION; i++) {
		m->src_dimensions[i] = 0;
	}
	if(qb_capture_dimensions_from_zval(zvalue, m, 0)) {
		uint32_t element_size = 1;
		for(i = m->src_dimension_count - 1; (int32_t) i >= 0; i--) {
			uint32_t array_size = element_size * m->src_dimensions[i];
			m->src_array_sizes[i] = array_size;
			element_size = array_size;
		}
		return TRUE;
	}
	return FALSE;
}

static int32_t qb_apply_dimension_mappings(qb_storage *storage, qb_address *address, qb_dimension_mappings *m) {
	uint32_t src_array_size = (m->src_dimension_count > 0) ? m->src_dimensions[0] : 1;
	if(m->dst_dimension_count > 0) {
		uint32_t dst_array_size = m->dst_array_sizes[0];
		if(src_array_size != dst_array_size) {
			if(!dst_array_size) {
				// some dimensions aren't defined
				uint32_t unknown_dimension_count = 0, known_dimension_count = m->src_dimension_count;
				uint32_t i;
				for(i = 0; i < m->dst_dimension_count; i++) {
					if(m->dst_dimensions[i] != 0) {
						// the subarray size of the destination needs to match the source
						if(i < m->src_dimension_count && m->dst_array_sizes[i] == m->src_array_sizes[i]) {
							known_dimension_count = i;
						} else {
							known_dimension_count = 0;
						}
						break;
					} else {
						unknown_dimension_count++;
					}
				}
				if(unknown_dimension_count == known_dimension_count) {
					// copy the dimensions over
					for(i = 0; i < unknown_dimension_count; i++) {
						m->dst_dimensions[i] = m->src_dimensions[i];
						m->dst_array_sizes[i] = m->src_array_sizes[i];
					}
				} else if(unknown_dimension_count == 1) {
					uint32_t src_array_size = m->src_array_sizes[0];
					if(m->dst_dimension_count == 1) {
						// a multidimensional array copied to a linear array
						m->dst_dimensions[0] = src_array_size;
						m->dst_array_sizes[0] = src_array_size;
					} else {
						uint32_t dst_sub_array_size = m->dst_array_sizes[1];
						uint32_t dst_dimension = src_array_size / dst_sub_array_size;
						uint32_t dst_array_size = dst_dimension * dst_sub_array_size;
						m->dst_dimensions[0] = dst_dimension;
						m->dst_array_sizes[0] = dst_array_size;
						if(dst_array_size != src_array_size) {
							// TODO: warning
						}
					}
				} else {
					// TODO: error msg
					return FALSE;
				}
				for(i = 0; i < unknown_dimension_count; i++) {
					qb_address *dimension_address = address->dimension_addresses[i];
					qb_address *array_size_address = address->array_size_addresses[i];
					VALUE_IN(storage, U32, dimension_address) = m->dst_dimensions[i];
					VALUE_IN(storage, U32, array_size_address) = m->dst_array_sizes[i];
				}
			} else if(dst_array_size < src_array_size) {
				// the source is too large
				// TODO: error msg
				return FALSE;
			}
		}
	} else {
		// scalar
		if(src_array_size != 1) {
			// TODO: error msg
			return FALSE;
		}
	}
	return TRUE;
}

static int32_t qb_copy_elements_from_zval(zval *zvalue, int8_t *dst_memory, qb_dimension_mappings *m, uint32_t dimension_index);

static int32_t qb_copy_elements_from_array(zval *zarray, int8_t *dst_memory, qb_dimension_mappings *m, uint32_t dimension_index) {
	int8_t *dst_pointer = dst_memory;
	uint32_t dst_dimension = (dimension_index < m->dst_dimension_count) ? m->dst_dimensions[dimension_index] : 1;
	uint32_t dst_element_element_count = (dimension_index + 1 < m->dst_dimension_count) ? m->dst_array_sizes[dimension_index + 1] : 1;
	uint32_t dst_element_byte_count = BYTE_COUNT(dst_element_element_count, m->dst_element_type);
	uint32_t src_index = 0;
	HashTable *ht = Z_ARRVAL_P(zarray);
	Bucket *p;

	// assume the elements are stored in order in the array
	for(p = ht->pListHead; p && src_index < dst_dimension; p = p->pListNext) {
		if((uint32_t) p->h == src_index && !p->arKey) {
			zval **p_element = p->pData;
			qb_copy_elements_from_zval(*p_element, dst_pointer, m, dimension_index + 1);
			src_index++;
			dst_pointer += dst_element_byte_count;
		} else {
			break;
		}
	}
	if(src_index < dst_dimension) {
		// the elements are not sequential: copy the rest
		while(src_index < dst_dimension && src_index < ht->nNextFreeElement) {
			zval **p_element;
			if(zend_hash_index_find(ht, src_index, (void **) &p_element) == SUCCESS) {
				qb_copy_elements_from_zval(*p_element, dst_pointer, m, dimension_index + 1);
			} else {
				// there's a gap
				memset(dst_pointer, 0, dst_element_byte_count);
			}
			src_index++;
			dst_pointer += dst_element_byte_count;
		}
		if(src_index < dst_dimension) {
			qb_fill_array_gap(dst_memory, src_index * dst_element_byte_count, dst_dimension * dst_element_byte_count, dimension_index);
		}
	}
	return TRUE;
}

static int32_t qb_copy_elements_from_object(zval *zobject, int8_t *dst_memory, qb_dimension_mappings *m, uint32_t dimension_index) {
	int8_t *dst_pointer = dst_memory;
	uint32_t dst_dimension = (dimension_index < m->dst_dimension_count) ? m->dst_dimensions[dimension_index] : 1;
	uint32_t dst_element_element_count = (dimension_index + 1 < m->dst_dimension_count) ? m->dst_array_sizes[dimension_index + 1] : 1;
	uint32_t dst_element_byte_count = BYTE_COUNT(dst_element_element_count, m->dst_element_type);
	uint32_t src_index = 0;
	qb_index_alias_scheme *scheme = m->dst_index_alias_schemes[dimension_index];
	TSRMLS_FETCH();
	while(src_index < scheme->dimension) {
		zval **p_element, *element = NULL;
		zval *alias = qb_cstring_to_zval(scheme->aliases[src_index] TSRMLS_CC);
		p_element = Z_OBJ_GET_PROP_PTR_PTR(zobject, alias);

		if(p_element) {
			element = *p_element;
		} else if(Z_OBJ_HT_P(zobject)->read_property) {
			element = Z_OBJ_READ_PROP(zobject, alias);
		}
		if(element && Z_TYPE_P(element) != IS_NULL) {
			qb_copy_elements_from_zval(element, dst_pointer, m, dimension_index + 1);
		} else {
			memset(dst_pointer, 0, dst_element_byte_count);
		}
		if(!p_element && element) {
			Z_ADDREF_P(element);
			zval_ptr_dtor(&element);
		}
		src_index++;
		dst_pointer += dst_element_byte_count;
	}
	if(src_index < dst_dimension) {
		qb_fill_array_gap(dst_memory, src_index * dst_element_byte_count, dst_dimension * dst_element_byte_count, dimension_index);
	}
	return TRUE;
}

static int32_t qb_copy_elements_from_string(zval *zstring, int8_t *dst_memory, qb_dimension_mappings *m, uint32_t dimension_index) {
	int8_t *src_memory = (int8_t *) Z_STRVAL_P(zstring);
	uint32_t src_byte_count = Z_STRLEN_P(zstring);
	uint32_t dst_element_count = (dimension_index < m->dst_dimension_count) ? m->dst_array_sizes[dimension_index] : 1;
	uint32_t dst_byte_count = BYTE_COUNT(dst_element_count, m->dst_element_type);
	memcpy(dst_memory, src_memory, src_byte_count);
	if(src_byte_count < dst_byte_count) {
		qb_fill_array_gap(dst_memory, src_byte_count, dst_byte_count, dimension_index);
	}
	return TRUE;
}

static int32_t qb_copy_elements_from_long(zval *zlong, int8_t *dst_memory, qb_dimension_mappings *m, uint32_t dimension_index) {
	long src_value = Z_LVAL_P(zlong);
#if SIZEOF_LONG == 8
	qb_copy_element(QB_TYPE_S64, (int8_t *) &src_value, m->dst_element_type, dst_memory);
#else
	qb_copy_element(QB_TYPE_S32, (int8_t *) &src_value, m->dst_element_type, dst_memory);
#endif
	if(dimension_index < m->dst_dimension_count) {
		uint32_t dst_element_count = m->dst_array_sizes[dimension_index];
		uint32_t dst_byte_count = BYTE_COUNT(dst_element_count, m->dst_element_type);
		uint32_t src_byte_count = BYTE_COUNT(1, m->dst_element_type);
		qb_fill_array_gap(dst_memory, src_byte_count, dst_byte_count, dimension_index);
	}
	return TRUE;
}

static int32_t qb_copy_elements_from_double(zval *zdouble, int8_t *dst_memory, qb_dimension_mappings *m, uint32_t dimension_index) {
	double src_value = Z_DVAL_P(zdouble);
	qb_copy_element(QB_TYPE_F64, (int8_t *) &src_value, m->dst_element_type, dst_memory);
	if(dimension_index < m->dst_dimension_count) {
		uint32_t dst_element_count = m->dst_array_sizes[dimension_index];
		uint32_t dst_byte_count = BYTE_COUNT(dst_element_count, m->dst_element_type);
		uint32_t src_byte_count = BYTE_COUNT(1, m->dst_element_type);
		qb_fill_array_gap(dst_memory, src_byte_count, dst_byte_count, dimension_index);
	}
	return TRUE;
}

static int32_t qb_copy_elements_from_null(zval *znull, int8_t *dst_memory, qb_dimension_mappings *m, uint32_t dimension_index) {
	uint32_t dst_element_count = (dimension_index < m->dst_dimension_count) ? m->dst_array_sizes[dimension_index] : 1;
	uint32_t dst_byte_count = BYTE_COUNT(dst_element_count, m->dst_element_type);
	memset(dst_memory, 0, dst_byte_count);
	return TRUE;
}

static int32_t qb_copy_elements_from_zval(zval *zvalue, int8_t *dst_memory, qb_dimension_mappings *m, uint32_t dimension_index) {
	switch(Z_TYPE_P(zvalue)) {
		case IS_CONSTANT_ARRAY:
		case IS_ARRAY:	return qb_copy_elements_from_array(zvalue, dst_memory, m, dimension_index);
		case IS_OBJECT:	return qb_copy_elements_from_object(zvalue, dst_memory, m, dimension_index);
		case IS_STRING:	return qb_copy_elements_from_string(zvalue, dst_memory, m, dimension_index);
		case IS_NULL:	return qb_copy_elements_from_null(zvalue, dst_memory, m, dimension_index);
		case IS_BOOL:
		case IS_LONG:	return qb_copy_elements_from_long(zvalue, dst_memory, m, dimension_index);
		case IS_DOUBLE:	return qb_copy_elements_from_double(zvalue, dst_memory, m, dimension_index);
		case IS_RESOURCE: {
			gdImagePtr image;
			php_stream *stream;
			if((image = qb_get_gd_image(zvalue))) {
				// copy the content from the image
				return qb_copy_elements_from_gd_image(image, dst_memory, m, dimension_index);
			} else if((stream = qb_get_file_stream(zvalue))) {
				// copy the content from the file 
				return qb_copy_elements_from_file(stream, dst_memory, m, dimension_index);
			}
		}	break;
	}
	return FALSE;
}

static int32_t qb_copy_element_to_zval(qb_storage *storage, qb_address *address, zval *zvalue) {
	zval_dtor(zvalue);
	switch(address->type) {
		case QB_TYPE_S08: {
			Z_TYPE_P(zvalue) = IS_LONG;
			Z_LVAL_P(zvalue) = VALUE_IN(storage, S08, address);
		}	break;
		case QB_TYPE_U08: {
			Z_TYPE_P(zvalue) = IS_LONG;
			Z_LVAL_P(zvalue) = VALUE_IN(storage, U08, address);
		}	break;
		case QB_TYPE_S16: {
			Z_TYPE_P(zvalue) = IS_LONG;
			Z_LVAL_P(zvalue) = VALUE_IN(storage, S16, address);
		}	break;
		case QB_TYPE_U16: {
			Z_TYPE_P(zvalue) = IS_LONG;
			Z_LVAL_P(zvalue) = VALUE_IN(storage, U16, address);
		}	break;
		case QB_TYPE_S32: {
			Z_TYPE_P(zvalue) = IS_LONG;
			Z_LVAL_P(zvalue) = VALUE_IN(storage, S32, address);
		}	break;
		case QB_TYPE_U32: {
#if SIZEOF_LONG == 8
			Z_TYPE_P(zvalue) = IS_LONG;
			Z_LVAL_P(zvalue) = VALUE_IN(storage, U32, address);
#else
			CTYPE(U32) value = VALUE_IN(storage, U32, address);
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
			Z_LVAL_P(zvalue) = VALUE_IN(storage, S64, address);
#else
			CTYPE(S64) value = VALUE_IN(storage, S64, address);
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
			Z_LVAL_P(zvalue) = VALUE_IN(storage, U64, address);
#else
			CTYPE(U64) value = VALUE_IN(storage, U64, address);
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
			Z_DVAL_P(zvalue) = VALUE_IN(storage, F32, address);
		}	break;
		case QB_TYPE_F64: {
			Z_TYPE_P(zvalue) = IS_DOUBLE;
			Z_DVAL_P(zvalue) = VALUE_IN(storage, F64, address);
		}	break;
	}
	return TRUE;
}

static int32_t qb_initialize_zval_array(qb_storage *storage, qb_address *element_address, zval *container, zval *element) {	
	zend_class_entry *ce = NULL;
	TSRMLS_FETCH();
	if(element_address->index_alias_schemes && element_address->index_alias_schemes[0]) {
		qb_index_alias_scheme *scheme = element_address->index_alias_schemes[0];
		if(scheme->class_name) {
			if(!scheme->zend_class) {
				zend_class_entry **p_ce;
				if(zend_lookup_class(scheme->class_name, scheme->class_name_length, &p_ce TSRMLS_CC) == FAILURE) {
					qb_report_missing_class_exception(0, scheme->class_name);
					return FALSE;
				} else if((*p_ce)->ce_flags & (ZEND_ACC_INTERFACE|ZEND_ACC_IMPLICIT_ABSTRACT_CLASS|ZEND_ACC_EXPLICIT_ABSTRACT_CLASS)) {
					qb_report_abstract_class_exception(0, scheme->class_name);
					return FALSE;
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
	if(!ce && element_address->index_alias_schemes && element_address->index_alias_schemes[0]) {
		// use standard class if a scheme is provided but no explicit class name given
		ce = zend_standard_class_def;
	}
	if(ce) {
		object_init_ex(element, ce);
	} else {
		uint32_t size = VALUE_IN(storage, U32, element_address->dimension_addresses[0]);
		array_init_size(element, size);
	}
	return TRUE;
}

static int32_t qb_copy_elements_to_zval(qb_storage *storage, qb_address *address, zval *container, zval *zvalue);

static int32_t qb_copy_elements_to_array(qb_storage *storage, qb_address *address, zval *zarray) {
	qb_address _element_address, *element_address = &_element_address;
	HashTable *ht = Z_ARRVAL_P(zarray);
	uint32_t dimension = VALUE_IN(storage, U32, address->dimension_addresses[0]);
	uint32_t element_count = (address->dimension_count > 1) ? VALUE_IN(storage, U32, address->array_size_addresses[1]) : 1;
	uint32_t element_byte_count = BYTE_COUNT(element_count, address->type);
	uint32_t index = 0;
	Bucket *p;

	qb_initialize_element_address(element_address, address);

	// assume the elements are stored in order in the array
	for(p = ht->pListHead; p && index < dimension; p = p->pListNext) {
		if((uint32_t) p->h == index && !p->arKey) {
			zval **p_element = p->pData, *element;
			SEPARATE_ZVAL_TO_MAKE_IS_REF(p_element);
			element = *p_element;
			if(element_address->dimension_count > 0) {
				if(!qb_copy_elements_to_zval(storage, element_address, zarray, element)) {
					return FALSE;
				}
			} else {
				if(!qb_copy_element_to_zval(storage, element_address, element)) {
					return FALSE;
				}
			}
			index++;
			element_address->segment_offset += element_byte_count;
		} else {
			break;
		}
	}
	while(index < dimension) {
		zval **p_element, *element;
		if(zend_hash_index_find(ht, index, (void **) &p_element) == SUCCESS) {
			SEPARATE_ZVAL_TO_MAKE_IS_REF(p_element);
			element = *p_element;
		} else {
			ALLOC_INIT_ZVAL(element);
			zend_hash_index_update(ht, index, (void **) &element, sizeof(zval *), NULL);
		}
		if(element_address->dimension_count > 0) {
			if(!qb_copy_elements_to_zval(storage, element_address, zarray, element)) {
				return FALSE;
			}
		} else {
			if(!qb_copy_element_to_zval(storage, element_address, element)) {
				return FALSE;
			}
		}
		index++;
		element_address->segment_offset += element_byte_count;
	}
	// remove items with indices larger than the dimension
	while(p) {
		Bucket *next = p->pListNext;
		if((uint32_t) p->h >= dimension && !p->arKey) {
			zend_hash_index_del(ht, p->h);
		}
		p = next;
	}
	return TRUE;
}

static int32_t qb_copy_elements_to_object(qb_storage *storage, qb_address *address, zval *zobject) {
	qb_address _element_address, *element_address = &_element_address;
	qb_index_alias_scheme *scheme = address->index_alias_schemes[0];
	uint32_t dimension = VALUE_IN(storage, U32, address->dimension_addresses[0]);
	uint32_t element_byte_count = BYTE_COUNT(1, address->type);
	uint32_t i = 0;
	TSRMLS_FETCH();

	qb_initialize_element_address(element_address, address);

	for(i = 0; i < dimension; i++) {
		zval **p_element, *element = NULL;
		zval *alias = qb_cstring_to_zval(scheme->aliases[i] TSRMLS_CC);
		p_element = Z_OBJ_GET_PROP_PTR_PTR(zobject, alias);
		if(p_element) {
			SEPARATE_ZVAL_TO_MAKE_IS_REF(p_element);
			element = *p_element;
		} else {
			ALLOC_INIT_ZVAL(element);
		}
		if(element_address->dimension_count > 0) {
			if(!qb_copy_elements_to_zval(storage, element_address, NULL, element)) {
				return FALSE;
			}
		} else {
			if(!qb_copy_element_to_zval(storage, element_address, element)) {
				return FALSE;
			}
		}
		if(!p_element) {
			Z_OBJ_WRITE_PROP(zobject, alias, element);
			zval_ptr_dtor(&element);
		}
		element_address->segment_offset += element_byte_count;
	}
	return TRUE;
}

static int32_t qb_copy_elements_to_string(qb_storage *storage, qb_address *address, zval *zvalue) {
	// make sure the string is the right size
	uint32_t element_count = VALUE_IN(storage, U32, address->array_size_address);
	uint32_t byte_count = BYTE_COUNT(element_count, address->type);
	int8_t *memory;
	if(Z_STRLEN_P(zvalue) == byte_count) {
		memory = (int8_t *) Z_STRVAL_P(zvalue);
	} else {
		memory = emalloc(byte_count + 1);
		memory[byte_count] = '\0';
		efree(Z_STRVAL_P(zvalue));
		Z_STRVAL_P(zvalue) = (char *) memory;
		Z_STRLEN_P(zvalue) = byte_count;
	}
	memcpy(memory, ARRAY_IN(storage, I08, address), byte_count);
	return TRUE;
}

static int32_t qb_copy_elements_to_zval(qb_storage *storage, qb_address *address, zval *container, zval *zvalue) {
	switch(Z_TYPE_P(zvalue)) {
		case IS_ARRAY:	{
			return qb_copy_elements_to_array(storage, address, zvalue);
		}	break;
		case IS_OBJECT:	{
			return qb_copy_elements_to_object(storage, address, zvalue);
		}	break;
		case IS_STRING:	{
			return qb_copy_elements_to_string(storage, address, zvalue);
		}	break;
		case IS_RESOURCE:	{
			gdImagePtr image;
			php_stream *stream;
			if((stream = qb_get_file_stream(zvalue))) {
				// copy the content from the file 
				return qb_copy_elements_to_file(storage, address, stream);
			} else if((image = qb_get_gd_image(zvalue))) {
				return qb_copy_elements_to_gd_image(storage, address, image);
			}
		}	break;
		default:	{
			// create a string if the string flag is set, otherwise create an array (or possibly an object)
			if(address->flags & QB_ADDRESS_STRING) {
				ZVAL_EMPTY_STRING(zvalue);
			} else if(address->flags & QB_ADDRESS_IMAGE) {
				if(!qb_initialize_zval_image(storage, address, zvalue)) {
					return FALSE;
				}
			} else {
				if(!qb_initialize_zval_array(storage, address, container, zvalue)) {
					return FALSE;
				}
			}
			return qb_copy_elements_to_zval(storage, address, container, zvalue);
		}	break;
	}
	return TRUE;
}

static int32_t qb_set_array_dimensions_from_storage_location(qb_storage *storage, qb_address *address, qb_storage *src_storage, qb_address *src_address, uint32_t *p_array_size) {
	if(src_address->dimension_count == address->dimension_count) {
		int32_t i;
		uint32_t array_size = 1;
		for(i = address->dimension_count - 1; i >= 0; i--) {
			qb_address *dimension_address = address->dimension_addresses[i];
			qb_address *caller_dimension_address = src_address->dimension_addresses[i];
			qb_address *array_size_address = address->array_size_addresses[i];
			uint32_t dimension = VALUE_IN(src_storage, U32, caller_dimension_address);
			uint32_t dimension_expected = VALUE_IN(storage, U32, dimension_address);

			array_size *= dimension;

			if(dimension > dimension_expected) {
				if(CONSTANT(dimension_address)) {
					qb_report_argument_size_mismatch_exception(0, dimension, dimension_expected);
					return FALSE;
				} else {
					VALUE_IN(storage, U32, dimension_address) = dimension;
				}
				if(i != address->dimension_count - 1) {
					VALUE_IN(storage, U32, array_size_address) = array_size;
				}
			} else if(dimension < dimension_expected) {
				array_size = VALUE_IN(storage, U32, array_size_address);
			}
		}
		*p_array_size = array_size;
		return TRUE;
	} else {
		qb_report_dimension_count_mismatch_exception(0, address->dimension_count, src_address->dimension_count);
		return FALSE;
	}
}

static int32_t qb_set_array_dimensions_at_storage_location(qb_storage *storage, qb_address *address, qb_storage *src_storage, qb_address *src_address, uint32_t *p_array_size) {
	return qb_set_array_dimensions_from_storage_location(src_storage, src_address, storage, address, p_array_size);
}

void qb_copy_elements(uint32_t source_type, int8_t *restrict source_memory, uint32_t source_count, uint32_t dest_type, int8_t *restrict dest_memory, uint32_t dest_count) {
	uint32_t i, count = min(source_count, dest_count);
	if(EXPECTED(dest_type == source_type)) {
		switch(dest_type) {
			case QB_TYPE_S08:
			case QB_TYPE_U08: for(i = 0; i < count; i++) ((CTYPE(U08) *) dest_memory)[i] = ((CTYPE(U08) *) source_memory)[i]; break;
			case QB_TYPE_S16: 
			case QB_TYPE_U16: for(i = 0; i < count; i++) ((CTYPE(U16) *) dest_memory)[i] = ((CTYPE(U16) *) source_memory)[i]; break;
			case QB_TYPE_S32:
			case QB_TYPE_U32: for(i = 0; i < count; i++) ((CTYPE(U32) *) dest_memory)[i] = ((CTYPE(U32) *) source_memory)[i]; break;
			case QB_TYPE_S64:
			case QB_TYPE_U64: for(i = 0; i < count; i++) ((CTYPE(U64) *) dest_memory)[i] = ((CTYPE(U64) *) source_memory)[i]; break;
			case QB_TYPE_F32: for(i = 0; i < count; i++) ((CTYPE(F32) *) dest_memory)[i] = ((CTYPE(F32) *) source_memory)[i]; break;
			case QB_TYPE_F64: for(i = 0; i < count; i++) ((CTYPE(F64) *) dest_memory)[i] = ((CTYPE(F64) *) source_memory)[i]; break;
		}
	} else {
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
	}
	if(UNEXPECTED(source_count < dest_count)) {
		qb_copy_wrap_around(dest_memory, BYTE_COUNT(source_count, dest_type), BYTE_COUNT(dest_count, dest_type));
	}
}

void qb_copy_element(uint32_t source_type, int8_t *restrict source_memory, uint32_t dest_type, int8_t *restrict dest_memory) {
	if(EXPECTED(dest_type == source_type)) {
		switch(dest_type) {
			case QB_TYPE_S08:
			case QB_TYPE_U08: ((CTYPE(U08) *) dest_memory)[0] = ((CTYPE(U08) *) source_memory)[0]; break;
			case QB_TYPE_S16: 
			case QB_TYPE_U16: ((CTYPE(U16) *) dest_memory)[0] = ((CTYPE(U16) *) source_memory)[0]; break;
			case QB_TYPE_S32:
			case QB_TYPE_U32: ((CTYPE(U32) *) dest_memory)[0] = ((CTYPE(U32) *) source_memory)[0]; break;
			case QB_TYPE_S64:
			case QB_TYPE_U64: ((CTYPE(U64) *) dest_memory)[0] = ((CTYPE(U64) *) source_memory)[0]; break;
			case QB_TYPE_F32: ((CTYPE(F32) *) dest_memory)[0] = ((CTYPE(F32) *) source_memory)[0]; break;
			case QB_TYPE_F64: ((CTYPE(F64) *) dest_memory)[0] = ((CTYPE(F64) *) source_memory)[0]; break;
		}
	} else {
		switch(dest_type) {
			case QB_TYPE_S08: {
				switch(source_type) {
					case QB_TYPE_S08:
					case QB_TYPE_U08: ((CTYPE(S08) *) dest_memory)[0] = (CTYPE(S08)) ((CTYPE(S08) *) source_memory)[0]; break;
					case QB_TYPE_S16:
					case QB_TYPE_U16: ((CTYPE(S08) *) dest_memory)[0] = (CTYPE(S08)) ((CTYPE(S16) *) source_memory)[0]; break;
					case QB_TYPE_S32:
					case QB_TYPE_U32: ((CTYPE(S08) *) dest_memory)[0] = (CTYPE(S08)) ((CTYPE(S32) *) source_memory)[0]; break;
					case QB_TYPE_S64:
					case QB_TYPE_U64: ((CTYPE(S08) *) dest_memory)[0] = (CTYPE(S08)) ((CTYPE(S64) *) source_memory)[0]; break;
					case QB_TYPE_F32: ((CTYPE(S08) *) dest_memory)[0] = (CTYPE(S08)) (CTYPE(S64)) ((CTYPE(F32) *) source_memory)[0]; break;
					case QB_TYPE_F64: ((CTYPE(S08) *) dest_memory)[0] = (CTYPE(S08)) (CTYPE(S64)) ((CTYPE(F64) *) source_memory)[0]; break;
				}
			}	break;
			case QB_TYPE_U08: {
				switch(source_type) {
					case QB_TYPE_S08:
					case QB_TYPE_U08: ((CTYPE(U08) *) dest_memory)[0] = (CTYPE(U08)) ((CTYPE(U08) *) source_memory)[0]; break;
					case QB_TYPE_S16:
					case QB_TYPE_U16: ((CTYPE(U08) *) dest_memory)[0] = (CTYPE(U08)) ((CTYPE(S16) *) source_memory)[0]; break;
					case QB_TYPE_S32:
					case QB_TYPE_U32: ((CTYPE(U08) *) dest_memory)[0] = (CTYPE(U08)) ((CTYPE(S32) *) source_memory)[0]; break;
					case QB_TYPE_S64:
					case QB_TYPE_U64: ((CTYPE(U08) *) dest_memory)[0] = (CTYPE(U08)) ((CTYPE(S64) *) source_memory)[0]; break;
					case QB_TYPE_F32: ((CTYPE(U08) *) dest_memory)[0] = (CTYPE(U08)) (CTYPE(S64)) ((CTYPE(F32) *) source_memory)[0]; break;
					case QB_TYPE_F64: ((CTYPE(U08) *) dest_memory)[0] = (CTYPE(U08)) (CTYPE(S64)) ((CTYPE(F64) *) source_memory)[0]; break;
				}
			}	break;
			case QB_TYPE_S16: {
				switch(source_type) {
					case QB_TYPE_S08: ((CTYPE(S16) *) dest_memory)[0] = (CTYPE(S16)) ((CTYPE(S08) *) source_memory)[0]; break;
					case QB_TYPE_U08: ((CTYPE(S16) *) dest_memory)[0] = (CTYPE(S16)) ((CTYPE(U08) *) source_memory)[0]; break;
					case QB_TYPE_S16:
					case QB_TYPE_U16: ((CTYPE(S16) *) dest_memory)[0] = (CTYPE(S16)) ((CTYPE(S16) *) source_memory)[0]; break;
					case QB_TYPE_S32:
					case QB_TYPE_U32: ((CTYPE(S16) *) dest_memory)[0] = (CTYPE(S16)) ((CTYPE(S32) *) source_memory)[0]; break;
					case QB_TYPE_S64:
					case QB_TYPE_U64: ((CTYPE(S16) *) dest_memory)[0] = (CTYPE(S16)) ((CTYPE(S64) *) source_memory)[0]; break;
					case QB_TYPE_F32: ((CTYPE(S16) *) dest_memory)[0] = (CTYPE(S16)) (CTYPE(S64)) ((CTYPE(F32) *) source_memory)[0]; break;
					case QB_TYPE_F64: ((CTYPE(S16) *) dest_memory)[0] = (CTYPE(S16)) (CTYPE(S64)) ((CTYPE(F64) *) source_memory)[0]; break;
				}
			}	break;
			case QB_TYPE_U16: {
				switch(source_type) {
					case QB_TYPE_S08: ((CTYPE(U16) *) dest_memory)[0] = (CTYPE(U16)) ((CTYPE(S08) *) source_memory)[0]; break;
					case QB_TYPE_U08: ((CTYPE(U16) *) dest_memory)[0] = (CTYPE(U16)) ((CTYPE(U08) *) source_memory)[0]; break;
					case QB_TYPE_S16:
					case QB_TYPE_U16: ((CTYPE(U16) *) dest_memory)[0] = (CTYPE(U16)) ((CTYPE(U16) *) source_memory)[0]; break;
					case QB_TYPE_S32:
					case QB_TYPE_U32: ((CTYPE(U16) *) dest_memory)[0] = (CTYPE(U16)) ((CTYPE(S32) *) source_memory)[0]; break;
					case QB_TYPE_S64:
					case QB_TYPE_U64: ((CTYPE(U16) *) dest_memory)[0] = (CTYPE(U16)) ((CTYPE(S64) *) source_memory)[0]; break;
					case QB_TYPE_F32: ((CTYPE(U16) *) dest_memory)[0] = (CTYPE(U16)) (CTYPE(S64)) ((CTYPE(F32) *) source_memory)[0]; break;
					case QB_TYPE_F64: ((CTYPE(U16) *) dest_memory)[0] = (CTYPE(U16)) (CTYPE(S64)) ((CTYPE(F64) *) source_memory)[0]; break;
				}
			}	break;
			case QB_TYPE_S32: {
				switch(source_type) {
					case QB_TYPE_S08: ((CTYPE(S32) *) dest_memory)[0] = (CTYPE(S32)) ((CTYPE(S08) *) source_memory)[0]; break;
					case QB_TYPE_U08: ((CTYPE(S32) *) dest_memory)[0] = (CTYPE(S32)) ((CTYPE(U08) *) source_memory)[0]; break;
					case QB_TYPE_S16: ((CTYPE(S32) *) dest_memory)[0] = (CTYPE(S32)) ((CTYPE(S16) *) source_memory)[0]; break;
					case QB_TYPE_U16: ((CTYPE(S32) *) dest_memory)[0] = (CTYPE(S32)) ((CTYPE(U16) *) source_memory)[0]; break;
					case QB_TYPE_S32:
					case QB_TYPE_U32: ((CTYPE(S32) *) dest_memory)[0] = (CTYPE(S32)) ((CTYPE(S32) *) source_memory)[0]; break;
					case QB_TYPE_S64:
					case QB_TYPE_U64: ((CTYPE(S32) *) dest_memory)[0] = (CTYPE(S32)) ((CTYPE(S64) *) source_memory)[0]; break;
					case QB_TYPE_F32: ((CTYPE(S32) *) dest_memory)[0] = (CTYPE(S32)) (CTYPE(S64)) ((CTYPE(F32) *) source_memory)[0]; break;
					case QB_TYPE_F64: ((CTYPE(S32) *) dest_memory)[0] = (CTYPE(S32)) (CTYPE(S64)) ((CTYPE(F64) *) source_memory)[0]; break;
				}
			}	break;
			case QB_TYPE_U32: {
				switch(source_type) {
					case QB_TYPE_S08: ((CTYPE(U32) *) dest_memory)[0] = (CTYPE(U32)) ((CTYPE(S08) *) source_memory)[0]; break;
					case QB_TYPE_U08: ((CTYPE(U32) *) dest_memory)[0] = (CTYPE(U32)) ((CTYPE(U08) *) source_memory)[0]; break;
					case QB_TYPE_S16: ((CTYPE(U32) *) dest_memory)[0] = (CTYPE(U32)) ((CTYPE(S16) *) source_memory)[0]; break;
					case QB_TYPE_U16: ((CTYPE(U32) *) dest_memory)[0] = (CTYPE(U32)) ((CTYPE(U16) *) source_memory)[0]; break;
					case QB_TYPE_S32:
					case QB_TYPE_U32: ((CTYPE(U32) *) dest_memory)[0] = (CTYPE(U32)) ((CTYPE(U32) *) source_memory)[0]; break;
					case QB_TYPE_S64:
					case QB_TYPE_U64: ((CTYPE(U32) *) dest_memory)[0] = (CTYPE(U32)) ((CTYPE(S64) *) source_memory)[0]; break;
					case QB_TYPE_F32: ((CTYPE(U32) *) dest_memory)[0] = (CTYPE(U32)) (CTYPE(S64)) ((CTYPE(F32) *) source_memory)[0]; break;
					case QB_TYPE_F64: ((CTYPE(U32) *) dest_memory)[0] = (CTYPE(U32)) (CTYPE(S64)) ((CTYPE(F64) *) source_memory)[0]; break;
				}
			}	break;
			case QB_TYPE_S64: {
				switch(source_type) {
					case QB_TYPE_S08: ((CTYPE(S64) *) dest_memory)[0] = (CTYPE(S64)) ((CTYPE(S08) *) source_memory)[0]; break;
					case QB_TYPE_U08: ((CTYPE(S64) *) dest_memory)[0] = (CTYPE(S64)) ((CTYPE(U08) *) source_memory)[0]; break;
					case QB_TYPE_S16: ((CTYPE(S64) *) dest_memory)[0] = (CTYPE(S64)) ((CTYPE(S16) *) source_memory)[0]; break;
					case QB_TYPE_U16: ((CTYPE(S64) *) dest_memory)[0] = (CTYPE(S64)) ((CTYPE(U16) *) source_memory)[0]; break;
					case QB_TYPE_S32: ((CTYPE(S64) *) dest_memory)[0] = (CTYPE(S64)) ((CTYPE(S32) *) source_memory)[0]; break;
					case QB_TYPE_U32: ((CTYPE(S64) *) dest_memory)[0] = (CTYPE(S64)) ((CTYPE(U32) *) source_memory)[0]; break;
					case QB_TYPE_S64:
					case QB_TYPE_U64: ((CTYPE(S64) *) dest_memory)[0] = (CTYPE(S64)) ((CTYPE(S64) *) source_memory)[0]; break;
					case QB_TYPE_F32: ((CTYPE(S64) *) dest_memory)[0] = (CTYPE(S64)) ((CTYPE(F32) *) source_memory)[0]; break;
					case QB_TYPE_F64: ((CTYPE(S64) *) dest_memory)[0] = (CTYPE(S64)) ((CTYPE(F64) *) source_memory)[0]; break;
				}
			}	break;
			case QB_TYPE_U64: {
				switch(source_type) {
					case QB_TYPE_S08: ((CTYPE(U64) *) dest_memory)[0] = (CTYPE(U64)) ((CTYPE(S08) *) source_memory)[0]; break;
					case QB_TYPE_U08: ((CTYPE(U64) *) dest_memory)[0] = (CTYPE(U64)) ((CTYPE(U08) *) source_memory)[0]; break;
					case QB_TYPE_S16: ((CTYPE(U64) *) dest_memory)[0] = (CTYPE(U64)) ((CTYPE(S16) *) source_memory)[0]; break;
					case QB_TYPE_U16: ((CTYPE(U64) *) dest_memory)[0] = (CTYPE(U64)) ((CTYPE(U16) *) source_memory)[0]; break;
					case QB_TYPE_S32: ((CTYPE(U64) *) dest_memory)[0] = (CTYPE(U64)) ((CTYPE(S32) *) source_memory)[0]; break;
					case QB_TYPE_U32: ((CTYPE(U64) *) dest_memory)[0] = (CTYPE(U64)) ((CTYPE(U32) *) source_memory)[0]; break;
					case QB_TYPE_S64:
					case QB_TYPE_U64: ((CTYPE(U64) *) dest_memory)[0] = (CTYPE(U64)) ((CTYPE(U64) *) source_memory)[0]; break;
					case QB_TYPE_F32: ((CTYPE(U64) *) dest_memory)[0] = (CTYPE(U64)) ((CTYPE(F32) *) source_memory)[0]; break;
					case QB_TYPE_F64: ((CTYPE(U64) *) dest_memory)[0] = (CTYPE(U64)) ((CTYPE(F64) *) source_memory)[0]; break;
				}
			}	break;
			case QB_TYPE_F32: {
				switch(source_type) {
					case QB_TYPE_S08: ((CTYPE(F32) *) dest_memory)[0] = (CTYPE(F32)) ((CTYPE(S08) *) source_memory)[0]; break;
					case QB_TYPE_U08: ((CTYPE(F32) *) dest_memory)[0] = (CTYPE(F32)) ((CTYPE(U08) *) source_memory)[0]; break;
					case QB_TYPE_S16: ((CTYPE(F32) *) dest_memory)[0] = (CTYPE(F32)) ((CTYPE(S16) *) source_memory)[0]; break;
					case QB_TYPE_U16: ((CTYPE(F32) *) dest_memory)[0] = (CTYPE(F32)) ((CTYPE(U16) *) source_memory)[0]; break;
					case QB_TYPE_S32: ((CTYPE(F32) *) dest_memory)[0] = (CTYPE(F32)) ((CTYPE(S32) *) source_memory)[0]; break;
					case QB_TYPE_U32: ((CTYPE(F32) *) dest_memory)[0] = (CTYPE(F32)) ((CTYPE(U32) *) source_memory)[0]; break;
					case QB_TYPE_S64: ((CTYPE(F32) *) dest_memory)[0] = (CTYPE(F32)) ((CTYPE(S64) *) source_memory)[0]; break;
					case QB_TYPE_U64: ((CTYPE(F32) *) dest_memory)[0] = (CTYPE(F32)) ((CTYPE(U64) *) source_memory)[0]; break;
					case QB_TYPE_F32: ((CTYPE(F32) *) dest_memory)[0] = (CTYPE(F32)) ((CTYPE(F32) *) source_memory)[0]; break;
					case QB_TYPE_F64: ((CTYPE(F32) *) dest_memory)[0] = (CTYPE(F32)) ((CTYPE(F64) *) source_memory)[0]; break;
				}
			}	break;
			case QB_TYPE_F64: {
				switch(source_type) {
					case QB_TYPE_S08: ((CTYPE(F64) *) dest_memory)[0] = (CTYPE(F64)) ((CTYPE(S08) *) source_memory)[0]; break;
					case QB_TYPE_U08: ((CTYPE(F64) *) dest_memory)[0] = (CTYPE(F64)) ((CTYPE(U08) *) source_memory)[0]; break;
					case QB_TYPE_S16: ((CTYPE(F64) *) dest_memory)[0] = (CTYPE(F64)) ((CTYPE(S16) *) source_memory)[0]; break;
					case QB_TYPE_U16: ((CTYPE(F64) *) dest_memory)[0] = (CTYPE(F64)) ((CTYPE(U16) *) source_memory)[0]; break;
					case QB_TYPE_S32: ((CTYPE(F64) *) dest_memory)[0] = (CTYPE(F64)) ((CTYPE(S32) *) source_memory)[0]; break;
					case QB_TYPE_U32: ((CTYPE(F64) *) dest_memory)[0] = (CTYPE(F64)) ((CTYPE(U32) *) source_memory)[0]; break;
					case QB_TYPE_S64: ((CTYPE(F64) *) dest_memory)[0] = (CTYPE(F64)) ((CTYPE(S64) *) source_memory)[0]; break;
					case QB_TYPE_U64: ((CTYPE(F64) *) dest_memory)[0] = (CTYPE(F64)) ((CTYPE(U64) *) source_memory)[0]; break;
					case QB_TYPE_F32: ((CTYPE(F64) *) dest_memory)[0] = (CTYPE(F64)) ((CTYPE(F32) *) source_memory)[0]; break;
					case QB_TYPE_F64: ((CTYPE(F64) *) dest_memory)[0] = (CTYPE(F64)) ((CTYPE(F64) *) source_memory)[0]; break;
				}
			}	break;
		}
	}
}

static int32_t qb_copy_elements_from_storage_location(qb_storage *storage, qb_address *address, qb_storage *src_storage, qb_address *src_address) {
	uint32_t src_element_count = SCALAR(src_address) ? 1 : ARRAY_SIZE_IN(src_storage, src_address);
	uint32_t dst_element_count = SCALAR(address) ? 1 : ARRAY_SIZE_IN(storage, address);
	qb_copy_elements(src_address->type, ARRAY_IN(src_storage, I08, src_address), src_element_count, address->type, ARRAY_IN(storage, I08, address), dst_element_count);
	return TRUE;
}

static int32_t qb_copy_element_from_storage_location(qb_storage *storage, qb_address *address, qb_storage *src_storage, qb_address *src_address) {
	qb_copy_element(src_address->type, ARRAY_IN(src_storage, I08, src_address), address->type, ARRAY_IN(storage, I08, address));
	return TRUE;
}

static int32_t qb_copy_elements_to_storage_location(qb_storage *storage, qb_address *address, qb_storage *dst_storage, qb_address *dst_address) {
	uint32_t src_element_count = SCALAR(address) ? 1 : ARRAY_SIZE_IN(storage, address);
	uint32_t dst_element_count = SCALAR(dst_address) ? 1 : ARRAY_SIZE_IN(dst_storage, dst_address);
	qb_copy_elements(address->type, ARRAY_IN(storage, I08, address), src_element_count, dst_address->type, ARRAY_IN(dst_storage, I08, dst_address), dst_element_count);
	return TRUE;
}

static int32_t qb_copy_element_to_storage_location(qb_storage *storage, qb_address *address, qb_storage *dst_storage, qb_address *dst_address) {
	qb_copy_element(address->type, ARRAY_IN(storage, I08, address), dst_address->type, ARRAY_IN(dst_storage, I08, dst_address));
	return TRUE;
}

int32_t qb_transfer_value_from_zval(qb_storage *storage, qb_address *address, zval *zvalue, int32_t transfer_flags) {
	// determine the array's dimensions and check for out-of-bound condition
	qb_dimension_mappings _mappings, *mappings = &_mappings;
	int8_t *pointer;
	if(!qb_add_destination_dimensions(storage, address, mappings) || !qb_add_source_dimensions_from_zval(zvalue, mappings)) {
		return FALSE;
	}
	if(!qb_apply_dimension_mappings(storage, address, mappings)) {
		return FALSE;
	}

	// if it's a variable-length array, adjust the size 
	if(address->segment_selector >= QB_SELECTOR_ARRAY_START) {
		uint32_t element_count = mappings->dst_array_sizes[0];
		qb_memory_segment *segment = &storage->segments[address->segment_selector];
		uint32_t byte_count = BYTE_COUNT(element_count, address->type);

		// use memory from the source if possible
		if(transfer_flags & (QB_TRANSFER_CAN_BORROW_MEMORY | QB_TRANSFER_CAN_SEIZE_MEMORY)) {
			if(Z_TYPE_P(zvalue) == IS_STRING) {
				if(!IS_INTERNED(Z_STRVAL_P(zvalue))) {
					int8_t *memory = (int8_t *) Z_STRVAL_P(zvalue);
					uint32_t bytes_available = Z_STRLEN_P(zvalue) + 1;
					if(qb_connect_segment_to_memory(segment, memory, byte_count, bytes_available, (transfer_flags & QB_TRANSFER_CAN_SEIZE_MEMORY))) {
						if(transfer_flags & QB_TRANSFER_CAN_SEIZE_MEMORY) {
							ZVAL_NULL(zvalue);
						}
						return TRUE;
					}
				}
			} else if(Z_TYPE_P(zvalue) == IS_RESOURCE) {
				php_stream *stream = qb_get_file_stream(zvalue);
				if(stream) {
					if(qb_connect_segment_to_file(segment, stream, byte_count, !READ_ONLY(address))) {
						return TRUE;
					}
				}
			}
		}
			
		// make sure there's enough bytes in the segment
		qb_allocate_segment_memory(segment, byte_count);
		pointer = segment->memory;
	} else {
		pointer = ARRAY_IN(storage, I08, address);
	}
	return qb_copy_elements_from_zval(zvalue, pointer, mappings, 0);
}

int32_t qb_transfer_value_from_storage_location(qb_storage *storage, qb_address *address, qb_storage *src_storage, qb_address *src_address, uint32_t transfer_flags) {
	if(SCALAR(address)) {
		qb_copy_element_from_storage_location(storage, address, src_storage, src_address);
		return TRUE;
	} else {
		uint32_t element_count;
		if(!qb_set_array_dimensions_from_storage_location(storage, address, src_storage, src_address, &element_count)) {
			return FALSE;
		}

		if(address->segment_selector >= QB_SELECTOR_ARRAY_START) {
			qb_memory_segment *segment = &storage->segments[address->segment_selector];
			uint32_t byte_count = BYTE_COUNT(element_count, address->type);
			if(transfer_flags & QB_TRANSFER_CAN_BORROW_MEMORY) {
				if(src_address->segment_selector < QB_SELECTOR_ARRAY_START) {
					// the incoming value is a fixed length array or scalar
					// use the memory there unless the function resizes the argument
					if(READ_ONLY(address->array_size_address)) {
						int8_t *memory = ARRAY_IN(src_storage, I08, src_address);
						uint32_t bytes_available = ARRAY_SIZE_IN(src_storage, src_address);
						if(qb_connect_segment_to_memory(segment, memory, byte_count, bytes_available, FALSE)) {
							return TRUE;
						}
					}
				} else {
					if(VARIABLE_LENGTH(src_address)) {
						qb_memory_segment *src_segment = &src_storage->segments[src_address->segment_selector];
						if(src_segment->flags & QB_SEGMENT_IMPORTED) {
							// the incoming value is an imported segment--import it 
							qb_import_segment(segment, src_segment->imported_segment);
						} else {
							qb_import_segment(segment, src_segment);
						}
						if(segment->imported_segment->byte_count != byte_count) {
							qb_resize_segment(segment->imported_segment, byte_count);
							// TODO: wrap-around
						}
						return TRUE;
					} else if(FIXED_LENGTH(address) && ARRAY_SIZE_IN(src_storage, src_address) >= byte_count)  {
						int8_t *memory = ARRAY_IN(src_storage, I08, src_address);
						uint32_t bytes_available = ARRAY_SIZE_IN(src_storage, src_address);
						if(qb_connect_segment_to_memory(segment, memory, byte_count, bytes_available, FALSE)) {
							return TRUE;
						}
					}
				}
			}
			qb_allocate_segment_memory(segment, byte_count);
		}
		qb_copy_elements_from_storage_location(storage, address, src_storage, src_address);
		return TRUE;
	}
}

int32_t qb_transfer_value_to_zval(qb_storage *storage, qb_address *address, zval *zvalue) {
	if(SCALAR(address)) {
		qb_copy_element_to_zval(storage, address, zvalue);
		return TRUE;
	} else {
		if(address->segment_selector >= QB_SELECTOR_ARRAY_START) {
			qb_memory_segment *segment = &storage->segments[address->segment_selector];
			if(segment->flags & QB_SEGMENT_MAPPED) {
				// the contents are in the file so we don't need to do anything
				return TRUE;
			} else if(segment->flags & QB_SEGMENT_BORROWED) {
				int8_t *memory;
				if(segment->byte_count == segment->current_allocation || (segment->byte_count - segment->current_allocation) > 1024) {
					// allocate there's no room for null terminator or there's a lot of unused space
					memory = erealloc(segment->memory, segment->byte_count + 1);
					memory[segment->byte_count] = '\0';
				} else {
					// unused space are always zeroed out so the string is terminated already
					memory = segment->memory;
				}
				if(Z_STRVAL_P(zvalue) != (char *) memory) {
					efree(Z_STRVAL_P(zvalue));
					Z_STRVAL_P(zvalue) = (char *) memory;
				}
				Z_STRLEN_P(zvalue) = segment->byte_count;
				return TRUE;
			}
		}
		qb_copy_elements_to_zval(storage, address, NULL, zvalue);
		return TRUE;
	}
}

int32_t qb_transfer_value_to_storage_location(qb_storage *storage, qb_address *address, qb_storage *dst_storage, qb_address *dst_address) {
	if(SCALAR(address)) {
		qb_copy_element_to_storage_location(storage, address, dst_storage, dst_address);
		return TRUE;
	} else {
		uint32_t element_count;
		if(!qb_set_array_dimensions_at_storage_location(storage, address, dst_storage, dst_address, &element_count)) {
			return FALSE;
		}

		if(address->segment_selector >= QB_SELECTOR_ARRAY_START) {
			qb_memory_segment *segment = &storage->segments[address->segment_selector];
			
			uint32_t byte_count = BYTE_COUNT(element_count, address->type);
			if(segment->flags & QB_SEGMENT_BORROWED) {
				// nothing needs to happen
				return TRUE;
			} else if(segment->flags & QB_SEGMENT_IMPORTED) {
				qb_memory_segment *dst_segment = &dst_storage->segments[dst_address->segment_selector];
				int8_t *memory = ARRAY_IN(dst_storage, I08, dst_address);

				// make sure the segment is large enough
				if(((dst_segment->flags & QB_SEGMENT_IMPORTED) && dst_segment->imported_segment->byte_count != byte_count) || dst_segment->byte_count != byte_count) {
					qb_resize_segment(dst_segment, byte_count);
					// TODO: wrap-around
				}
				return TRUE;
			}

			if(dst_address->segment_selector >= QB_SELECTOR_ARRAY_START) {
				qb_memory_segment *dst_segment = &dst_storage->segments[dst_address->segment_selector];
				qb_allocate_segment_memory(dst_segment, byte_count);
			}
		}
		qb_copy_elements_to_storage_location(storage, address, dst_storage, dst_address);
		return TRUE;
	}
}

