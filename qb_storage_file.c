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
	// get the file size
	off_t position, size;
	TSRMLS_FETCH();
	position = php_stream_tell(stream);
	php_stream_seek(stream, 0, SEEK_END);		
	size = php_stream_tell(stream);
	php_stream_seek(stream, position, SEEK_SET);
	return qb_capture_dimensions_from_byte_count(size, m, dimension_index);
}

static int32_t qb_copy_elements_to_file(int8_t *src_memory, php_stream *stream, qb_dimension_mappings *m, uint32_t dimension_index) {
	off_t position;
	uint32_t src_element_count = (dimension_index < m->src_dimension_count) ? m->src_array_sizes[dimension_index] : 1;
	uint32_t src_byte_count = BYTE_COUNT(src_element_count, m->src_element_type);
	size_t byte_written;
	TSRMLS_FETCH();
	position = php_stream_tell(stream);
	php_stream_seek(stream, 0, SEEK_SET);
	byte_written = php_stream_write(stream, (char *) src_memory, src_byte_count);
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
	uint32_t dst_byte_count = BYTE_COUNT(dst_element_count, m->dst_element_type);
	TSRMLS_FETCH();
	position = php_stream_tell(stream);
	php_stream_seek(stream, 0, SEEK_SET);
	src_byte_count = php_stream_read(stream, (char *) dst_memory, dst_byte_count);
	php_stream_seek(stream, position, SEEK_SET);
	if(src_byte_count < dst_byte_count) {
		if(dimension_index == 0) {
			qb_copy_wrap_around(dst_memory, src_byte_count, dst_byte_count);
		} else {
			memset(dst_memory + src_byte_count, 0, dst_byte_count - src_byte_count);
		}
	}
	return TRUE;
}

static void * qb_map_file_to_memory(php_stream *stream, uint32_t byte_count, int32_t write_access TSRMLS_DC) {
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
			return (int8_t *) range.mapped;
		}
	}
	return NULL;
}

static void qb_unmap_file_from_memory(php_stream *stream TSRMLS_DC) {
	php_stream_set_option(stream, PHP_STREAM_OPTION_MMAP_API, PHP_STREAM_MMAP_UNMAP, NULL);
}
