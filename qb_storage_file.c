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

static php_stream * qb_get_file_stream(zval *resource) {
	if(Z_TYPE_P(resource) == IS_RESOURCE) {
		php_stream *stream;
		TSRMLS_FETCH();
		stream = (php_stream*) zend_fetch_resource(&resource TSRMLS_CC, -1, NULL, NULL, 2, php_file_le_stream(), php_file_le_pstream());
		if(stream) {
			if(strcmp(stream->wrapper->wops->label, "plainfile") == 0) {
				return stream;
			}
		}
	}
	return NULL;
}

static int32_t qb_capture_dimensions_from_file(php_stream *stream, qb_dimension_mappings *m, uint32_t dimension_index) {
	uint32_t byte_count;
	int32_t need_utf8_decoding = FALSE;
	TSRMLS_FETCH();
	if(m->dst_address_flags & QB_ADDRESS_STRING) {
		if(m->dst_element_type >= QB_TYPE_I16) {
			need_utf8_decoding = TRUE;
		}
	}
	if(need_utf8_decoding) {
		// get the Unicode codepoint count
		off_t position;
		uint32_t cp_count = 0, read = 0, i, state = 0, codepoint;
		uint8_t buffer[1024];
		position = php_stream_tell(stream);
		php_stream_seek(stream, 0, SEEK_SET);
		do {
			read = php_stream_read(stream, (char *) buffer, sizeof(buffer));
			for(i = 0; i < read; i++) {
				if(!decode(&state, &codepoint, buffer[i])) {
					cp_count++;
				}
			}
		} while(read == sizeof(buffer));
		php_stream_seek(stream, position, SEEK_SET);
		byte_count = BYTE_COUNT(cp_count, m->dst_element_type);
	} else {
		// get the file size
		php_stream_statbuf ssb;
		php_stream_stat(stream, &ssb);
		byte_count = (uint32_t) ssb.sb.st_size;
	}
	return qb_capture_dimensions_from_byte_count(byte_count, m, dimension_index);
}

static int32_t qb_copy_elements_to_file(int8_t *src_memory, zval *zstream, php_stream *stream, qb_dimension_mappings *m, uint32_t dimension_index) {
	off_t position;
	uint32_t src_element_count = (dimension_index < m->src_dimension_count) ? m->src_array_sizes[dimension_index] : 1;
	uint32_t src_byte_count;
	size_t byte_written;
	int32_t need_utf8_encoding = FALSE;
	TSRMLS_FETCH();

	if(m->src_address_flags & QB_ADDRESS_STRING) {
		if(m->src_element_type >= QB_TYPE_I16) {
			need_utf8_encoding = TRUE;
		}
	}

	position = php_stream_tell(stream);
	php_stream_seek(stream, 0, SEEK_SET);
	if(need_utf8_encoding) {
		uint32_t i = 0, j;
		uint8_t buffer[1024];
		src_byte_count = 0;
		byte_written = 0;
		do {
			if(STORAGE_TYPE_MATCH(m->src_element_type, QB_TYPE_I16)) {
				uint16_t *src_elements = (uint16_t *) src_memory;
				for(j = 0; j < sizeof(buffer) - 4 && i < src_element_count; i++) {
					j += encode(src_elements[i], buffer + j);
				}
			} else if(STORAGE_TYPE_MATCH(m->src_element_type, QB_TYPE_I32)) {
				uint32_t *src_elements = (uint32_t *) src_memory;
				for(j = 0; j < sizeof(buffer) - 4 && i < src_element_count; i++) {
					j += encode(src_elements[i], buffer + j);
				}
			} else {
				break;
			}
			src_byte_count += j;
			byte_written += php_stream_write(stream, (char *) buffer, j);
		} while(i < src_element_count);
	} else {
		src_byte_count = BYTE_COUNT(src_element_count, m->src_element_type);
		byte_written = php_stream_write(stream, (char *) src_memory, src_byte_count);
	}
	php_stream_seek(stream, position, SEEK_SET);
	php_stream_truncate_set_size(stream, src_byte_count);
	if(byte_written != src_byte_count) {
		qb_report_file_write_error(0, src_byte_count, stream);
		return FALSE;
	}
	return TRUE;
}

static int32_t qb_copy_elements_from_file(php_stream *stream, int8_t *dst_memory, qb_dimension_mappings *m, uint32_t dimension_index) {
	off_t position;
	uint32_t src_byte_count;
	uint32_t dst_element_count = (dimension_index < m->dst_dimension_count) ? m->dst_array_sizes[dimension_index] : 1;
	int32_t need_utf8_decoding = FALSE;
	TSRMLS_FETCH();

	if(m->dst_address_flags & QB_ADDRESS_STRING) {
		if(m->dst_element_type >= QB_TYPE_I16) {
			need_utf8_decoding = TRUE;
		}
	}

	position = php_stream_tell(stream);
	php_stream_seek(stream, 0, SEEK_SET);
	if((m->dst_address_flags & QB_ADDRESS_STRING) && (m->dst_element_type >= QB_TYPE_I16)) {
		uint32_t read = 0, i, j = 0, state = 0, codepoint;
		uint8_t buffer[1024];
		do {
			read = php_stream_read(stream, (char *) buffer, sizeof(buffer));
			if(STORAGE_TYPE_MATCH(m->dst_element_type, QB_TYPE_I16)) {
				uint16_t *dst_elements = (uint16_t *) dst_memory;
				for(i = 0; i < read; i++) {
					if(!decode(&state, &codepoint, buffer[i])) {
						dst_elements[j] = codepoint;
						j++;
					}
				}
			} else if(STORAGE_TYPE_MATCH(m->dst_element_type, QB_TYPE_I32)) {
				uint32_t *dst_elements = (uint32_t *) dst_memory;
				for(i = 0; i < read; i++) {
					if(!decode(&state, &codepoint, buffer[i])) {
						dst_elements[j] = codepoint;
						j++;
					}
				}
			}
		} while(read == sizeof(buffer));
	} else {
		uint32_t dst_byte_count = BYTE_COUNT(dst_element_count, m->dst_element_type);
		src_byte_count = (uint32_t) php_stream_read(stream, (char *) dst_memory, dst_byte_count);
		php_stream_seek(stream, position, SEEK_SET);
		if(src_byte_count < dst_byte_count) {
			if(dimension_index == 0) {
				qb_copy_wrap_around(dst_memory, src_byte_count, dst_byte_count);
			} else {
				memset(dst_memory + src_byte_count, 0, dst_byte_count - src_byte_count);
			}
		}
	}
	php_stream_seek(stream, position, SEEK_SET);
	return TRUE;
}

static void * qb_map_file_to_memory(php_stream *stream, uint32_t byte_count, int32_t write_access TSRMLS_DC) {
	if(QB_G(allow_memory_map)) {
		php_stream_mmap_range range;
		if(write_access) {
			// make sure the file is large enough
			php_stream_statbuf ssb;
			php_stream_stat(stream, &ssb);
			if(byte_count > (uint32_t) ssb.sb.st_size) {
				php_stream_truncate_set_size(stream, byte_count);
			}
		}
		range.length = byte_count;
		range.offset = 0;
		range.mode = (write_access) ? PHP_STREAM_MAP_MODE_SHARED_READWRITE : PHP_STREAM_MAP_MODE_SHARED_READONLY;
		range.mapped = NULL;

		if(php_stream_set_option(stream, PHP_STREAM_OPTION_MMAP_API, PHP_STREAM_MMAP_MAP_RANGE, &range) == PHP_STREAM_OPTION_RETURN_OK) {
			return (int8_t *) range.mapped;
		}
	}
	return NULL;
}

static void qb_unmap_file_from_memory(php_stream *stream TSRMLS_DC) {
	php_stream_set_option(stream, PHP_STREAM_OPTION_MMAP_API, PHP_STREAM_MMAP_UNMAP, NULL);
}
