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

static int32_t qb_set_array_dimensions_from_file(qb_storage *storage, qb_address *address, php_stream *stream, uint32_t *p_array_size) {
	// get the file size
	size_t position, size;
	TSRMLS_FETCH();
	position = php_stream_tell(stream);
	php_stream_seek(stream, 0, SEEK_END);		
	size = php_stream_tell(stream);
	php_stream_seek(stream, position, SEEK_SET);
	return qb_set_array_dimensions_from_byte_count(storage, address, (uint32_t) size, p_array_size);
}

static void qb_copy_elements_to_file(qb_storage *storage, qb_address *address, php_stream *stream) {
	size_t position, byte_count;
	TSRMLS_FETCH();
	position = php_stream_tell(stream);
	byte_count = VALUE_IN(storage, U32, address->array_size_address);
	php_stream_seek(stream, 0, SEEK_SET);
	php_stream_write(stream, (char *) ARRAY_IN(storage, I08, address), byte_count);
	php_stream_seek(stream, position, SEEK_SET);
}

static void qb_copy_elements_from_file(qb_storage *storage, qb_address *address, php_stream *stream) {
	size_t position, byte_count;
	TSRMLS_FETCH();
	position = php_stream_tell(stream);
	byte_count = VALUE_IN(storage, U32, address->array_size_address);
	php_stream_seek(stream, 0, SEEK_SET);
	php_stream_read(stream, (char *) ARRAY_IN(storage, I08, address), byte_count);
	php_stream_seek(stream, position, SEEK_SET);
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