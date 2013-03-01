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

float32_t qb_calculate_array_distance_1x_F32_wrapper(float32_t *v1, float32_t *v2) {
	return qb_calculate_array_distance_1x_F32(v1, v2);
}

float64_t qb_calculate_array_distance_1x_F64_wrapper(float64_t *v1, float64_t *v2) {
	return qb_calculate_array_distance_1x_F64(v1, v2);
}

float32_t qb_calculate_array_distance_2x_F32_wrapper(float32_t *v1, float32_t *v2) {
	return qb_calculate_array_distance_2x_F32(v1, v2);
}

float64_t qb_calculate_array_distance_2x_F64_wrapper(float64_t *v1, float64_t *v2) {
	return qb_calculate_array_distance_2x_F64(v1, v2);
}

float32_t qb_calculate_array_distance_3x_F32_wrapper(float32_t *v1, float32_t *v2) {
	return qb_calculate_array_distance_3x_F32(v1, v2);
}

float64_t qb_calculate_array_distance_3x_F64_wrapper(float64_t *v1, float64_t *v2) {
	return qb_calculate_array_distance_3x_F64(v1, v2);
}

float32_t qb_calculate_array_distance_4x_F32_wrapper(float32_t *v1, float32_t *v2) {
	return qb_calculate_array_distance_4x_F32(v1, v2);
}

float64_t qb_calculate_array_distance_4x_F64_wrapper(float64_t *v1, float64_t *v2) {
	return qb_calculate_array_distance_4x_F64(v1, v2);
}

float32_t qb_calculate_array_distance_F32_wrapper(float32_t *v1, float32_t *v2, uint32_t count) {
	return qb_calculate_array_distance_F32(v1, v2, count);
}

float64_t qb_calculate_array_distance_F64_wrapper(float64_t *v1, float64_t *v2, uint32_t count) {
	return qb_calculate_array_distance_F64(v1, v2, count);
}

float32_t qb_calculate_array_length_1x_F32_wrapper(float32_t *v) {
	return qb_calculate_array_length_1x_F32(v);
}

float64_t qb_calculate_array_length_1x_F64_wrapper(float64_t *v) {
	return qb_calculate_array_length_1x_F64(v);
}

float32_t qb_calculate_array_length_2x_F32_wrapper(float32_t *v) {
	return qb_calculate_array_length_2x_F32(v);
}

float64_t qb_calculate_array_length_2x_F64_wrapper(float64_t *v) {
	return qb_calculate_array_length_2x_F64(v);
}

float32_t qb_calculate_array_length_3x_F32_wrapper(float32_t *v) {
	return qb_calculate_array_length_3x_F32(v);
}

float64_t qb_calculate_array_length_3x_F64_wrapper(float64_t *v) {
	return qb_calculate_array_length_3x_F64(v);
}

float32_t qb_calculate_array_length_4x_F32_wrapper(float32_t *v) {
	return qb_calculate_array_length_4x_F32(v);
}

float64_t qb_calculate_array_length_4x_F64_wrapper(float64_t *v) {
	return qb_calculate_array_length_4x_F64(v);
}

float32_t qb_calculate_array_length_F32_wrapper(float32_t *v, uint32_t size) {
	return qb_calculate_array_length_F32(v, size);
}

float64_t qb_calculate_array_length_F64_wrapper(float64_t *v, uint32_t size) {
	return qb_calculate_array_length_F64(v, size);
}

float32_t qb_calculate_array_max_F32_wrapper(qb_interpreter_context *__restrict cxt, float32_t *elements, uint32_t count) {
	return qb_calculate_array_max_F32(cxt, elements, count);
}

float64_t qb_calculate_array_max_F64_wrapper(qb_interpreter_context *__restrict cxt, float64_t *elements, uint32_t count) {
	return qb_calculate_array_max_F64(cxt, elements, count);
}

int8_t qb_calculate_array_max_S08_wrapper(qb_interpreter_context *__restrict cxt, int8_t *elements, uint32_t count) {
	return qb_calculate_array_max_S08(cxt, elements, count);
}

int16_t qb_calculate_array_max_S16_wrapper(qb_interpreter_context *__restrict cxt, int16_t *elements, uint32_t count) {
	return qb_calculate_array_max_S16(cxt, elements, count);
}

int32_t qb_calculate_array_max_S32_wrapper(qb_interpreter_context *__restrict cxt, int32_t *elements, uint32_t count) {
	return qb_calculate_array_max_S32(cxt, elements, count);
}

int64_t qb_calculate_array_max_S64_wrapper(qb_interpreter_context *__restrict cxt, int64_t *elements, uint32_t count) {
	return qb_calculate_array_max_S64(cxt, elements, count);
}

uint8_t qb_calculate_array_max_U08_wrapper(qb_interpreter_context *__restrict cxt, uint8_t *elements, uint32_t count) {
	return qb_calculate_array_max_U08(cxt, elements, count);
}

uint16_t qb_calculate_array_max_U16_wrapper(qb_interpreter_context *__restrict cxt, uint16_t *elements, uint32_t count) {
	return qb_calculate_array_max_U16(cxt, elements, count);
}

uint32_t qb_calculate_array_max_U32_wrapper(qb_interpreter_context *__restrict cxt, uint32_t *elements, uint32_t count) {
	return qb_calculate_array_max_U32(cxt, elements, count);
}

uint64_t qb_calculate_array_max_U64_wrapper(qb_interpreter_context *__restrict cxt, uint64_t *elements, uint32_t count) {
	return qb_calculate_array_max_U64(cxt, elements, count);
}

float32_t qb_calculate_array_min_F32_wrapper(qb_interpreter_context *__restrict cxt, float32_t *elements, uint32_t count) {
	return qb_calculate_array_min_F32(cxt, elements, count);
}

float64_t qb_calculate_array_min_F64_wrapper(qb_interpreter_context *__restrict cxt, float64_t *elements, uint32_t count) {
	return qb_calculate_array_min_F64(cxt, elements, count);
}

int8_t qb_calculate_array_min_S08_wrapper(qb_interpreter_context *__restrict cxt, int8_t *elements, uint32_t count) {
	return qb_calculate_array_min_S08(cxt, elements, count);
}

int16_t qb_calculate_array_min_S16_wrapper(qb_interpreter_context *__restrict cxt, int16_t *elements, uint32_t count) {
	return qb_calculate_array_min_S16(cxt, elements, count);
}

int32_t qb_calculate_array_min_S32_wrapper(qb_interpreter_context *__restrict cxt, int32_t *elements, uint32_t count) {
	return qb_calculate_array_min_S32(cxt, elements, count);
}

int64_t qb_calculate_array_min_S64_wrapper(qb_interpreter_context *__restrict cxt, int64_t *elements, uint32_t count) {
	return qb_calculate_array_min_S64(cxt, elements, count);
}

uint8_t qb_calculate_array_min_U08_wrapper(qb_interpreter_context *__restrict cxt, uint8_t *elements, uint32_t count) {
	return qb_calculate_array_min_U08(cxt, elements, count);
}

uint16_t qb_calculate_array_min_U16_wrapper(qb_interpreter_context *__restrict cxt, uint16_t *elements, uint32_t count) {
	return qb_calculate_array_min_U16(cxt, elements, count);
}

uint32_t qb_calculate_array_min_U32_wrapper(qb_interpreter_context *__restrict cxt, uint32_t *elements, uint32_t count) {
	return qb_calculate_array_min_U32(cxt, elements, count);
}

uint64_t qb_calculate_array_min_U64_wrapper(qb_interpreter_context *__restrict cxt, uint64_t *elements, uint32_t count) {
	return qb_calculate_array_min_U64(cxt, elements, count);
}

float32_t qb_calculate_array_product_F32_wrapper(qb_interpreter_context *__restrict cxt, float32_t *elements, uint32_t count) {
	return qb_calculate_array_product_F32(cxt, elements, count);
}

float64_t qb_calculate_array_product_F64_wrapper(qb_interpreter_context *__restrict cxt, float64_t *elements, uint32_t count) {
	return qb_calculate_array_product_F64(cxt, elements, count);
}

int8_t qb_calculate_array_product_S08_wrapper(qb_interpreter_context *__restrict cxt, int8_t *elements, uint32_t count) {
	return qb_calculate_array_product_S08(cxt, elements, count);
}

int16_t qb_calculate_array_product_S16_wrapper(qb_interpreter_context *__restrict cxt, int16_t *elements, uint32_t count) {
	return qb_calculate_array_product_S16(cxt, elements, count);
}

int32_t qb_calculate_array_product_S32_wrapper(qb_interpreter_context *__restrict cxt, int32_t *elements, uint32_t count) {
	return qb_calculate_array_product_S32(cxt, elements, count);
}

int64_t qb_calculate_array_product_S64_wrapper(qb_interpreter_context *__restrict cxt, int64_t *elements, uint32_t count) {
	return qb_calculate_array_product_S64(cxt, elements, count);
}

uint8_t qb_calculate_array_product_U08_wrapper(qb_interpreter_context *__restrict cxt, uint8_t *elements, uint32_t count) {
	return qb_calculate_array_product_U08(cxt, elements, count);
}

uint16_t qb_calculate_array_product_U16_wrapper(qb_interpreter_context *__restrict cxt, uint16_t *elements, uint32_t count) {
	return qb_calculate_array_product_U16(cxt, elements, count);
}

uint32_t qb_calculate_array_product_U32_wrapper(qb_interpreter_context *__restrict cxt, uint32_t *elements, uint32_t count) {
	return qb_calculate_array_product_U32(cxt, elements, count);
}

uint64_t qb_calculate_array_product_U64_wrapper(qb_interpreter_context *__restrict cxt, uint64_t *elements, uint32_t count) {
	return qb_calculate_array_product_U64(cxt, elements, count);
}

float32_t qb_calculate_array_sum_F32_wrapper(qb_interpreter_context *__restrict cxt, float32_t *elements, uint32_t count) {
	return qb_calculate_array_sum_F32(cxt, elements, count);
}

float64_t qb_calculate_array_sum_F64_wrapper(qb_interpreter_context *__restrict cxt, float64_t *elements, uint32_t count) {
	return qb_calculate_array_sum_F64(cxt, elements, count);
}

int8_t qb_calculate_array_sum_S08_wrapper(qb_interpreter_context *__restrict cxt, int8_t *elements, uint32_t count) {
	return qb_calculate_array_sum_S08(cxt, elements, count);
}

int16_t qb_calculate_array_sum_S16_wrapper(qb_interpreter_context *__restrict cxt, int16_t *elements, uint32_t count) {
	return qb_calculate_array_sum_S16(cxt, elements, count);
}

int32_t qb_calculate_array_sum_S32_wrapper(qb_interpreter_context *__restrict cxt, int32_t *elements, uint32_t count) {
	return qb_calculate_array_sum_S32(cxt, elements, count);
}

int64_t qb_calculate_array_sum_S64_wrapper(qb_interpreter_context *__restrict cxt, int64_t *elements, uint32_t count) {
	return qb_calculate_array_sum_S64(cxt, elements, count);
}

uint8_t qb_calculate_array_sum_U08_wrapper(qb_interpreter_context *__restrict cxt, uint8_t *elements, uint32_t count) {
	return qb_calculate_array_sum_U08(cxt, elements, count);
}

uint16_t qb_calculate_array_sum_U16_wrapper(qb_interpreter_context *__restrict cxt, uint16_t *elements, uint32_t count) {
	return qb_calculate_array_sum_U16(cxt, elements, count);
}

uint32_t qb_calculate_array_sum_U32_wrapper(qb_interpreter_context *__restrict cxt, uint32_t *elements, uint32_t count) {
	return qb_calculate_array_sum_U32(cxt, elements, count);
}

uint64_t qb_calculate_array_sum_U64_wrapper(qb_interpreter_context *__restrict cxt, uint64_t *elements, uint32_t count) {
	return qb_calculate_array_sum_U64(cxt, elements, count);
}

void qb_calculate_cross_product_F32_wrapper(float32_t *v1, float32_t *v2, float32_t *res_ptr) {
	qb_calculate_cross_product_F32(v1, v2, res_ptr);
}

void qb_calculate_cross_product_F64_wrapper(float64_t *v1, float64_t *v2, float64_t *res_ptr) {
	qb_calculate_cross_product_F64(v1, v2, res_ptr);
}

float32_t qb_calculate_dot_product_1x_F32_wrapper(float32_t *v1, float32_t *v2) {
	return qb_calculate_dot_product_1x_F32(v1, v2);
}

float64_t qb_calculate_dot_product_1x_F64_wrapper(float64_t *v1, float64_t *v2) {
	return qb_calculate_dot_product_1x_F64(v1, v2);
}

float32_t qb_calculate_dot_product_2x_F32_wrapper(float32_t *v1, float32_t *v2) {
	return qb_calculate_dot_product_2x_F32(v1, v2);
}

float64_t qb_calculate_dot_product_2x_F64_wrapper(float64_t *v1, float64_t *v2) {
	return qb_calculate_dot_product_2x_F64(v1, v2);
}

float32_t qb_calculate_dot_product_3x_F32_wrapper(float32_t *v1, float32_t *v2) {
	return qb_calculate_dot_product_3x_F32(v1, v2);
}

float64_t qb_calculate_dot_product_3x_F64_wrapper(float64_t *v1, float64_t *v2) {
	return qb_calculate_dot_product_3x_F64(v1, v2);
}

float32_t qb_calculate_dot_product_4x_F32_wrapper(float32_t *v1, float32_t *v2) {
	return qb_calculate_dot_product_4x_F32(v1, v2);
}

float64_t qb_calculate_dot_product_4x_F64_wrapper(float64_t *v1, float64_t *v2) {
	return qb_calculate_dot_product_4x_F64(v1, v2);
}

float32_t qb_calculate_dot_product_F32_wrapper(float32_t *v1, float32_t *v2, uint32_t size) {
	return qb_calculate_dot_product_F32(v1, v2, size);
}

float64_t qb_calculate_dot_product_F64_wrapper(float64_t *v1, float64_t *v2, uint32_t size) {
	return qb_calculate_dot_product_F64(v1, v2, size);
}

void qb_calculate_face_forward_1x_F32_wrapper(float32_t *v1, float32_t *v2, float32_t *res_ptr) {
	qb_calculate_face_forward_1x_F32(v1, v2, res_ptr);
}

void qb_calculate_face_forward_1x_F64_wrapper(float64_t *v1, float64_t *v2, float64_t *res_ptr) {
	qb_calculate_face_forward_1x_F64(v1, v2, res_ptr);
}

void qb_calculate_face_forward_2x_F32_wrapper(float32_t *v1, float32_t *v2, float32_t *res_ptr) {
	qb_calculate_face_forward_2x_F32(v1, v2, res_ptr);
}

void qb_calculate_face_forward_2x_F64_wrapper(float64_t *v1, float64_t *v2, float64_t *res_ptr) {
	qb_calculate_face_forward_2x_F64(v1, v2, res_ptr);
}

void qb_calculate_face_forward_3x_F32_wrapper(float32_t *v1, float32_t *v2, float32_t *res_ptr) {
	qb_calculate_face_forward_3x_F32(v1, v2, res_ptr);
}

void qb_calculate_face_forward_3x_F64_wrapper(float64_t *v1, float64_t *v2, float64_t *res_ptr) {
	qb_calculate_face_forward_3x_F64(v1, v2, res_ptr);
}

void qb_calculate_face_forward_4x_F32_wrapper(float32_t *v1, float32_t *v2, float32_t *res_ptr) {
	qb_calculate_face_forward_4x_F32(v1, v2, res_ptr);
}

void qb_calculate_face_forward_4x_F64_wrapper(float64_t *v1, float64_t *v2, float64_t *res_ptr) {
	qb_calculate_face_forward_4x_F64(v1, v2, res_ptr);
}

void qb_calculate_face_forward_F32_wrapper(float32_t *v1, float32_t *v2, float32_t *res_ptr, uint32_t size) {
	qb_calculate_face_forward_F32(v1, v2, res_ptr, size);
}

void qb_calculate_face_forward_F64_wrapper(float64_t *v1, float64_t *v2, float64_t *res_ptr, uint32_t size) {
	qb_calculate_face_forward_F64(v1, v2, res_ptr, size);
}

void qb_calculate_reflect_1x_F32_wrapper(float32_t *v1, float32_t *v2, float32_t *res_ptr) {
	qb_calculate_reflect_1x_F32(v1, v2, res_ptr);
}

void qb_calculate_reflect_1x_F64_wrapper(float64_t *v1, float64_t *v2, float64_t *res_ptr) {
	qb_calculate_reflect_1x_F64(v1, v2, res_ptr);
}

void qb_calculate_reflect_2x_F32_wrapper(float32_t *v1, float32_t *v2, float32_t *res_ptr) {
	qb_calculate_reflect_2x_F32(v1, v2, res_ptr);
}

void qb_calculate_reflect_2x_F64_wrapper(float64_t *v1, float64_t *v2, float64_t *res_ptr) {
	qb_calculate_reflect_2x_F64(v1, v2, res_ptr);
}

void qb_calculate_reflect_3x_F32_wrapper(float32_t *v1, float32_t *v2, float32_t *res_ptr) {
	qb_calculate_reflect_3x_F32(v1, v2, res_ptr);
}

void qb_calculate_reflect_3x_F64_wrapper(float64_t *v1, float64_t *v2, float64_t *res_ptr) {
	qb_calculate_reflect_3x_F64(v1, v2, res_ptr);
}

void qb_calculate_reflect_4x_F32_wrapper(float32_t *v1, float32_t *v2, float32_t *res_ptr) {
	qb_calculate_reflect_4x_F32(v1, v2, res_ptr);
}

void qb_calculate_reflect_4x_F64_wrapper(float64_t *v1, float64_t *v2, float64_t *res_ptr) {
	qb_calculate_reflect_4x_F64(v1, v2, res_ptr);
}

void qb_calculate_reflect_F32_wrapper(float32_t *v1, float32_t *v2, float32_t *res_ptr, uint32_t size) {
	qb_calculate_reflect_F32(v1, v2, res_ptr, size);
}

void qb_calculate_reflect_F64_wrapper(float64_t *v1, float64_t *v2, float64_t *res_ptr, uint32_t size) {
	qb_calculate_reflect_F64(v1, v2, res_ptr, size);
}

void qb_calculate_refract_1x_F32_wrapper(float32_t *v1, float32_t *v2, float32_t eta, float32_t *res_ptr) {
	qb_calculate_refract_1x_F32(v1, v2, eta, res_ptr);
}

void qb_calculate_refract_1x_F64_wrapper(float64_t *v1, float64_t *v2, float64_t eta, float64_t *res_ptr) {
	qb_calculate_refract_1x_F64(v1, v2, eta, res_ptr);
}

void qb_calculate_refract_2x_F32_wrapper(float32_t *v1, float32_t *v2, float32_t eta, float32_t *res_ptr) {
	qb_calculate_refract_2x_F32(v1, v2, eta, res_ptr);
}

void qb_calculate_refract_2x_F64_wrapper(float64_t *v1, float64_t *v2, float64_t eta, float64_t *res_ptr) {
	qb_calculate_refract_2x_F64(v1, v2, eta, res_ptr);
}

void qb_calculate_refract_3x_F32_wrapper(float32_t *v1, float32_t *v2, float32_t eta, float32_t *res_ptr) {
	qb_calculate_refract_3x_F32(v1, v2, eta, res_ptr);
}

void qb_calculate_refract_3x_F64_wrapper(float64_t *v1, float64_t *v2, float64_t eta, float64_t *res_ptr) {
	qb_calculate_refract_3x_F64(v1, v2, eta, res_ptr);
}

void qb_calculate_refract_4x_F32_wrapper(float32_t *v1, float32_t *v2, float32_t eta, float32_t *res_ptr) {
	qb_calculate_refract_4x_F32(v1, v2, eta, res_ptr);
}

void qb_calculate_refract_4x_F64_wrapper(float64_t *v1, float64_t *v2, float64_t eta, float64_t *res_ptr) {
	qb_calculate_refract_4x_F64(v1, v2, eta, res_ptr);
}

void qb_calculate_refract_F32_wrapper(float32_t *v1, float32_t *v2, float32_t eta, float32_t *res_ptr, uint32_t size) {
	qb_calculate_refract_F32(v1, v2, eta, res_ptr, size);
}

void qb_calculate_refract_F64_wrapper(float64_t *v1, float64_t *v2, float64_t eta, float64_t *res_ptr, uint32_t size) {
	qb_calculate_refract_F64(v1, v2, eta, res_ptr, size);
}

int32_t qb_compare_array_F32_wrapper(float32_t *op1_start, float32_t *op1_end, float32_t *op2_start, float32_t *op2_end) {
	return qb_compare_array_F32(op1_start, op1_end, op2_start, op2_end);
}

int32_t qb_compare_array_F64_wrapper(float64_t *op1_start, float64_t *op1_end, float64_t *op2_start, float64_t *op2_end) {
	return qb_compare_array_F64(op1_start, op1_end, op2_start, op2_end);
}

int32_t qb_compare_array_S08_wrapper(int8_t *op1_start, int8_t *op1_end, int8_t *op2_start, int8_t *op2_end) {
	return qb_compare_array_S08(op1_start, op1_end, op2_start, op2_end);
}

int32_t qb_compare_array_S16_wrapper(int16_t *op1_start, int16_t *op1_end, int16_t *op2_start, int16_t *op2_end) {
	return qb_compare_array_S16(op1_start, op1_end, op2_start, op2_end);
}

int32_t qb_compare_array_S32_wrapper(int32_t *op1_start, int32_t *op1_end, int32_t *op2_start, int32_t *op2_end) {
	return qb_compare_array_S32(op1_start, op1_end, op2_start, op2_end);
}

int32_t qb_compare_array_S64_wrapper(int64_t *op1_start, int64_t *op1_end, int64_t *op2_start, int64_t *op2_end) {
	return qb_compare_array_S64(op1_start, op1_end, op2_start, op2_end);
}

int32_t qb_compare_array_U08_wrapper(uint8_t *op1_start, uint8_t *op1_end, uint8_t *op2_start, uint8_t *op2_end) {
	return qb_compare_array_U08(op1_start, op1_end, op2_start, op2_end);
}

int32_t qb_compare_array_U16_wrapper(uint16_t *op1_start, uint16_t *op1_end, uint16_t *op2_start, uint16_t *op2_end) {
	return qb_compare_array_U16(op1_start, op1_end, op2_start, op2_end);
}

int32_t qb_compare_array_U32_wrapper(uint32_t *op1_start, uint32_t *op1_end, uint32_t *op2_start, uint32_t *op2_end) {
	return qb_compare_array_U32(op1_start, op1_end, op2_start, op2_end);
}

int32_t qb_compare_array_U64_wrapper(uint64_t *op1_start, uint64_t *op1_end, uint64_t *op2_start, uint64_t *op2_end) {
	return qb_compare_array_U64(op1_start, op1_end, op2_start, op2_end);
}

void qb_copy_argument_wrapper(qb_interpreter_context *cxt, uint32_t argument_index) {
	qb_copy_argument(cxt, argument_index);
}

uint32_t qb_decode_fcall_mix_operand_wrapper(qb_interpreter_context *__restrict cxt, int8_t *__restrict *segments, uint32_t *__restrict operands) {
	return qb_decode_fcall_mix_operand(cxt, segments, operands);
}

uint32_t zend_always_inline qb_decode_fcall_variable_operand_wrapper(qb_interpreter_context *__restrict cxt, int8_t *__restrict *segments, uint32_t *__restrict operands) {
	return qb_decode_fcall_variable_operand(cxt, segments, operands);
}

void qb_decode_utf8_U16_wrapper(qb_interpreter_context *cxt, uint8_t *utf8_chars, uint32_t utf8_length, uint16_t *buffer) {
	qb_decode_utf8_U16(cxt, utf8_chars, utf8_length, buffer);
}

void qb_decode_utf8_U32_wrapper(qb_interpreter_context *cxt, uint8_t *utf8_chars, uint32_t utf8_length, uint32_t *buffer) {
	qb_decode_utf8_U32(cxt, utf8_chars, utf8_length, buffer);
}

void qb_encode_utf8_U16_wrapper(qb_interpreter_context *cxt, uint16_t *codepoints, uint32_t count, uint8_t *buffer) {
	qb_encode_utf8_U16(cxt, codepoints, count, buffer);
}

void qb_encode_utf8_U32_wrapper(qb_interpreter_context *cxt, uint32_t *codepoints, uint32_t count, uint8_t *buffer) {
	qb_encode_utf8_U32(cxt, codepoints, count, buffer);
}

void qb_enlarge_segment_wrapper(qb_interpreter_context *cxt, qb_memory_segment *segment, uint32_t desired_size) {
	qb_enlarge_segment(cxt, segment, desired_size);
}

void qb_execute_function_call_wrapper(qb_interpreter_context *cxt) {
	qb_execute_function_call(cxt);
}

void qb_finalize_function_call_wrapper(qb_interpreter_context *cxt) {
	qb_finalize_function_call(cxt);
}

int32_t qb_find_element_F32_wrapper(float32_t *elements, uint32_t count, float32_t needle) {
	return qb_find_element_F32(elements, count, needle);
}

int32_t qb_find_element_F64_wrapper(float64_t *elements, uint32_t count, float64_t needle) {
	return qb_find_element_F64(elements, count, needle);
}

int32_t qb_find_element_I08_wrapper(int8_t *elements, uint32_t count, int8_t needle) {
	return qb_find_element_I08(elements, count, needle);
}

int32_t qb_find_element_I16_wrapper(int16_t *elements, uint32_t count, int16_t needle) {
	return qb_find_element_I16(elements, count, needle);
}

int32_t qb_find_element_I32_wrapper(int32_t *elements, uint32_t count, int32_t needle) {
	return qb_find_element_I32(elements, count, needle);
}

int32_t qb_find_element_I64_wrapper(int64_t *elements, uint32_t count, int64_t needle) {
	return qb_find_element_I64(elements, count, needle);
}

int32_t qb_find_elements_F32_wrapper(float32_t *elements, uint32_t count, float32_t *needle, uint32_t needle_width) {
	return qb_find_elements_F32(elements, count, needle, needle_width);
}

int32_t qb_find_elements_F64_wrapper(float64_t *elements, uint32_t count, float64_t *needle, uint32_t needle_width) {
	return qb_find_elements_F64(elements, count, needle, needle_width);
}

int32_t qb_find_elements_I08_wrapper(int8_t *elements, uint32_t count, int8_t *needle, uint32_t needle_width) {
	return qb_find_elements_I08(elements, count, needle, needle_width);
}

int32_t qb_find_elements_I16_wrapper(int16_t *elements, uint32_t count, int16_t *needle, uint32_t needle_width) {
	return qb_find_elements_I16(elements, count, needle, needle_width);
}

int32_t qb_find_elements_I32_wrapper(int32_t *elements, uint32_t count, int32_t *needle, uint32_t needle_width) {
	return qb_find_elements_I32(elements, count, needle, needle_width);
}

int32_t qb_find_elements_I64_wrapper(int64_t *elements, uint32_t count, int64_t *needle, uint32_t needle_width) {
	return qb_find_elements_I64(elements, count, needle, needle_width);
}

int32_t qb_find_subarray_F32_wrapper(float32_t *elements, uint32_t count, float32_t *needle, uint32_t needle_width, uint32_t start_index) {
	return qb_find_subarray_F32(elements, count, needle, needle_width, start_index);
}

int32_t qb_find_subarray_F64_wrapper(float64_t *elements, uint32_t count, float64_t *needle, uint32_t needle_width, uint32_t start_index) {
	return qb_find_subarray_F64(elements, count, needle, needle_width, start_index);
}

int32_t qb_find_subarray_I08_wrapper(int8_t *elements, uint32_t count, int8_t *needle, uint32_t needle_width, uint32_t start_index) {
	return qb_find_subarray_I08(elements, count, needle, needle_width, start_index);
}

int32_t qb_find_subarray_I16_wrapper(int16_t *elements, uint32_t count, int16_t *needle, uint32_t needle_width, uint32_t start_index) {
	return qb_find_subarray_I16(elements, count, needle, needle_width, start_index);
}

int32_t qb_find_subarray_I32_wrapper(int32_t *elements, uint32_t count, int32_t *needle, uint32_t needle_width, uint32_t start_index) {
	return qb_find_subarray_I32(elements, count, needle, needle_width, start_index);
}

int32_t qb_find_subarray_I64_wrapper(int64_t *elements, uint32_t count, int64_t *needle, uint32_t needle_width, uint32_t start_index) {
	return qb_find_subarray_I64(elements, count, needle, needle_width, start_index);
}

int32_t qb_find_subarray_from_end_F32_wrapper(float32_t *elements, uint32_t count, float32_t *needle, uint32_t needle_width, int32_t start_index) {
	return qb_find_subarray_from_end_F32(elements, count, needle, needle_width, start_index);
}

int32_t qb_find_subarray_from_end_F64_wrapper(float64_t *elements, uint32_t count, float64_t *needle, uint32_t needle_width, int32_t start_index) {
	return qb_find_subarray_from_end_F64(elements, count, needle, needle_width, start_index);
}

int32_t qb_find_subarray_from_end_I08_wrapper(int8_t *elements, uint32_t count, int8_t *needle, uint32_t needle_width, int32_t start_index) {
	return qb_find_subarray_from_end_I08(elements, count, needle, needle_width, start_index);
}

int32_t qb_find_subarray_from_end_I16_wrapper(int16_t *elements, uint32_t count, int16_t *needle, uint32_t needle_width, int32_t start_index) {
	return qb_find_subarray_from_end_I16(elements, count, needle, needle_width, start_index);
}

int32_t qb_find_subarray_from_end_I32_wrapper(int32_t *elements, uint32_t count, int32_t *needle, uint32_t needle_width, int32_t start_index) {
	return qb_find_subarray_from_end_I32(elements, count, needle, needle_width, start_index);
}

int32_t qb_find_subarray_from_end_I64_wrapper(int64_t *elements, uint32_t count, int64_t *needle, uint32_t needle_width, int32_t start_index) {
	return qb_find_subarray_from_end_I64(elements, count, needle, needle_width, start_index);
}

uint32_t qb_get_utf8_codepoint_count_wrapper(qb_interpreter_context *cxt, uint8_t *utf8_chars, uint32_t utf8_length) {
	return qb_get_utf8_codepoint_count(cxt, utf8_chars, utf8_length);
}

uint32_t qb_get_utf8_encoded_length_U16_wrapper(qb_interpreter_context *cxt, uint16_t *codepoints, uint32_t count) {
	return qb_get_utf8_encoded_length_U16(cxt, codepoints, count);
}

uint32_t qb_get_utf8_encoded_length_U32_wrapper(qb_interpreter_context *cxt, uint32_t *codepoints, uint32_t count) {
	return qb_get_utf8_encoded_length_U32(cxt, codepoints, count);
}

void qb_initialize_function_call_wrapper(qb_interpreter_context *cxt, zend_function *zfunc, uint32_t argument_count, uint32_t line_number) {
	qb_initialize_function_call(cxt, zfunc, argument_count, line_number);
}

int64_t qb_mt_rand_S64_wrapper(qb_interpreter_context *cxt, int64_t lower_limit, uint64_t upper_limit) {
	return qb_mt_rand_S64(cxt, lower_limit, upper_limit);
}

uint64_t qb_mt_rand_U64_wrapper(qb_interpreter_context *cxt, uint64_t lower_limit, uint64_t upper_limit) {
	return qb_mt_rand_U64(cxt, lower_limit, upper_limit);
}

void qb_multiply_matrix_by_matrix_1x1_F32_wrapper(float32_t *m1, float32_t *m2, float32_t *res_ptr) {
	qb_multiply_matrix_by_matrix_1x1_F32(m1, m2, res_ptr);
}

void qb_multiply_matrix_by_matrix_1x1_F64_wrapper(float64_t *m1, float64_t *m2, float64_t *res_ptr) {
	qb_multiply_matrix_by_matrix_1x1_F64(m1, m2, res_ptr);
}

void qb_multiply_matrix_by_matrix_2x2_F32_wrapper(float32_t *m1, float32_t *m2, float32_t *res_ptr) {
	qb_multiply_matrix_by_matrix_2x2_F32(m1, m2, res_ptr);
}

void qb_multiply_matrix_by_matrix_2x2_F64_wrapper(float64_t *m1, float64_t *m2, float64_t *res_ptr) {
	qb_multiply_matrix_by_matrix_2x2_F64(m1, m2, res_ptr);
}

void qb_multiply_matrix_by_matrix_3x3_F32_wrapper(float32_t *m1, float32_t *m2, float32_t *res_ptr) {
	qb_multiply_matrix_by_matrix_3x3_F32(m1, m2, res_ptr);
}

void qb_multiply_matrix_by_matrix_3x3_F64_wrapper(float64_t *m1, float64_t *m2, float64_t *res_ptr) {
	qb_multiply_matrix_by_matrix_3x3_F64(m1, m2, res_ptr);
}

void qb_multiply_matrix_by_matrix_3x3_padded_F32_wrapper(float32_t *m1, float32_t *m2, float32_t *res_ptr) {
	qb_multiply_matrix_by_matrix_3x3_padded_F32(m1, m2, res_ptr);
}

void qb_multiply_matrix_by_matrix_3x3_padded_F64_wrapper(float64_t *m1, float64_t *m2, float64_t *res_ptr) {
	qb_multiply_matrix_by_matrix_3x3_padded_F64(m1, m2, res_ptr);
}

void qb_multiply_matrix_by_matrix_4x4_F32_wrapper(float32_t *m1, float32_t *m2, float32_t *res_ptr) {
	qb_multiply_matrix_by_matrix_4x4_F32(m1, m2, res_ptr);
}

void qb_multiply_matrix_by_matrix_4x4_F64_wrapper(float64_t *m1, float64_t *m2, float64_t *res_ptr) {
	qb_multiply_matrix_by_matrix_4x4_F64(m1, m2, res_ptr);
}

void qb_multiply_matrix_by_matrix_F32_wrapper(float32_t *m1, uint32_t m1_row, uint32_t m1_col, float32_t *m2, uint32_t m2_row, uint32_t m2_col, float32_t *res_ptr) {
	qb_multiply_matrix_by_matrix_F32(m1, m1_row, m1_col, m2, m2_row, m2_col, res_ptr);
}

void qb_multiply_matrix_by_matrix_F64_wrapper(float64_t *m1, uint32_t m1_row, uint32_t m1_col, float64_t *m2, uint32_t m2_row, uint32_t m2_col, float64_t *res_ptr) {
	qb_multiply_matrix_by_matrix_F64(m1, m1_row, m1_col, m2, m2_row, m2_col, res_ptr);
}

void qb_multiply_matrix_by_vector_1x1_F32_wrapper(float32_t *m, float32_t *v, float32_t *res_ptr) {
	qb_multiply_matrix_by_vector_1x1_F32(m, v, res_ptr);
}

void qb_multiply_matrix_by_vector_1x1_F64_wrapper(float64_t *m, float64_t *v, float64_t *res_ptr) {
	qb_multiply_matrix_by_vector_1x1_F64(m, v, res_ptr);
}

void qb_multiply_matrix_by_vector_2x2_F32_wrapper(float32_t *m, float32_t *v, float32_t *res_ptr) {
	qb_multiply_matrix_by_vector_2x2_F32(m, v, res_ptr);
}

void qb_multiply_matrix_by_vector_2x2_F64_wrapper(float64_t *m, float64_t *v, float64_t *res_ptr) {
	qb_multiply_matrix_by_vector_2x2_F64(m, v, res_ptr);
}

void qb_multiply_matrix_by_vector_3x3_F32_wrapper(float32_t *m, float32_t *v, float32_t *res_ptr) {
	qb_multiply_matrix_by_vector_3x3_F32(m, v, res_ptr);
}

void qb_multiply_matrix_by_vector_3x3_F64_wrapper(float64_t *m, float64_t *v, float64_t *res_ptr) {
	qb_multiply_matrix_by_vector_3x3_F64(m, v, res_ptr);
}

void qb_multiply_matrix_by_vector_3x3_padded_F32_wrapper(float32_t *m, float32_t *v, float32_t *res_ptr) {
	qb_multiply_matrix_by_vector_3x3_padded_F32(m, v, res_ptr);
}

void qb_multiply_matrix_by_vector_3x3_padded_F64_wrapper(float64_t *m, float64_t *v, float64_t *res_ptr) {
	qb_multiply_matrix_by_vector_3x3_padded_F64(m, v, res_ptr);
}

void qb_multiply_matrix_by_vector_4x4_F32_wrapper(float32_t *m, float32_t *v, float32_t *res_ptr) {
	qb_multiply_matrix_by_vector_4x4_F32(m, v, res_ptr);
}

void qb_multiply_matrix_by_vector_4x4_F64_wrapper(float64_t *m, float64_t *v, float64_t *res_ptr) {
	qb_multiply_matrix_by_vector_4x4_F64(m, v, res_ptr);
}

void qb_multiply_matrix_by_vector_F32_wrapper(float32_t *m, uint32_t m_row, uint32_t m_col, float32_t *v, uint32_t v_row, float32_t *res_ptr) {
	qb_multiply_matrix_by_vector_F32(m, m_row, m_col, v, v_row, res_ptr);
}

void qb_multiply_matrix_by_vector_F64_wrapper(float64_t *m, uint32_t m_row, uint32_t m_col, float64_t *v, uint32_t v_row, float64_t *res_ptr) {
	qb_multiply_matrix_by_vector_F64(m, m_row, m_col, v, v_row, res_ptr);
}

void qb_multiply_vector_by_matrix_1x1_F32_wrapper(float32_t *v, float32_t *m, float32_t *res_ptr) {
	qb_multiply_vector_by_matrix_1x1_F32(v, m, res_ptr);
}

void qb_multiply_vector_by_matrix_1x1_F64_wrapper(float64_t *v, float64_t *m, float64_t *res_ptr) {
	qb_multiply_vector_by_matrix_1x1_F64(v, m, res_ptr);
}

void qb_multiply_vector_by_matrix_2x2_F32_wrapper(float32_t *v, float32_t *m, float32_t *res_ptr) {
	qb_multiply_vector_by_matrix_2x2_F32(v, m, res_ptr);
}

void qb_multiply_vector_by_matrix_2x2_F64_wrapper(float64_t *v, float64_t *m, float64_t *res_ptr) {
	qb_multiply_vector_by_matrix_2x2_F64(v, m, res_ptr);
}

void qb_multiply_vector_by_matrix_3x3_F32_wrapper(float32_t *v, float32_t *m, float32_t *res_ptr) {
	qb_multiply_vector_by_matrix_3x3_F32(v, m, res_ptr);
}

void qb_multiply_vector_by_matrix_3x3_F64_wrapper(float64_t *v, float64_t *m, float64_t *res_ptr) {
	qb_multiply_vector_by_matrix_3x3_F64(v, m, res_ptr);
}

void qb_multiply_vector_by_matrix_3x3_padded_F32_wrapper(float32_t *v, float32_t *m, float32_t *res_ptr) {
	qb_multiply_vector_by_matrix_3x3_padded_F32(v, m, res_ptr);
}

void qb_multiply_vector_by_matrix_3x3_padded_F64_wrapper(float64_t *v, float64_t *m, float64_t *res_ptr) {
	qb_multiply_vector_by_matrix_3x3_padded_F64(v, m, res_ptr);
}

void qb_multiply_vector_by_matrix_4x4_F32_wrapper(float32_t *v, float32_t *m, float32_t *res_ptr) {
	qb_multiply_vector_by_matrix_4x4_F32(v, m, res_ptr);
}

void qb_multiply_vector_by_matrix_4x4_F64_wrapper(float64_t *v, float64_t *m, float64_t *res_ptr) {
	qb_multiply_vector_by_matrix_4x4_F64(v, m, res_ptr);
}

void qb_multiply_vector_by_matrix_F32_wrapper(float32_t *v, uint32_t v_col, float32_t *m, uint32_t m_row, uint32_t m_col, float32_t *res_ptr) {
	qb_multiply_vector_by_matrix_F32(v, v_col, m, m_row, m_col, res_ptr);
}

void qb_multiply_vector_by_matrix_F64_wrapper(float64_t *v, uint32_t v_col, float64_t *m, uint32_t m_row, uint32_t m_col, float64_t *res_ptr) {
	qb_multiply_vector_by_matrix_F64(v, v_col, m, m_row, m_col, res_ptr);
}

void qb_normalize_array_1x_F32_wrapper(float32_t *v, float32_t *res_ptr) {
	qb_normalize_array_1x_F32(v, res_ptr);
}

void qb_normalize_array_1x_F64_wrapper(float64_t *v, float64_t *res_ptr) {
	qb_normalize_array_1x_F64(v, res_ptr);
}

void qb_normalize_array_2x_F32_wrapper(float32_t *v, float32_t *res_ptr) {
	qb_normalize_array_2x_F32(v, res_ptr);
}

void qb_normalize_array_2x_F64_wrapper(float64_t *v, float64_t *res_ptr) {
	qb_normalize_array_2x_F64(v, res_ptr);
}

void qb_normalize_array_3x_F32_wrapper(float32_t *v, float32_t *res_ptr) {
	qb_normalize_array_3x_F32(v, res_ptr);
}

void qb_normalize_array_3x_F64_wrapper(float64_t *v, float64_t *res_ptr) {
	qb_normalize_array_3x_F64(v, res_ptr);
}

void qb_normalize_array_4x_F32_wrapper(float32_t *v, float32_t *res_ptr) {
	qb_normalize_array_4x_F32(v, res_ptr);
}

void qb_normalize_array_4x_F64_wrapper(float64_t *v, float64_t *res_ptr) {
	qb_normalize_array_4x_F64(v, res_ptr);
}

void qb_normalize_array_F32_wrapper(float32_t *v, float32_t *res_ptr, uint32_t size) {
	qb_normalize_array_F32(v, res_ptr, size);
}

void qb_normalize_array_F64_wrapper(float64_t *v, float64_t *res_ptr, uint32_t size) {
	qb_normalize_array_F64(v, res_ptr, size);
}

int64_t qb_rand_S64_wrapper(qb_interpreter_context *cxt, int64_t lower_limit, uint64_t upper_limit) {
	return qb_rand_S64(cxt, lower_limit, upper_limit);
}

uint64_t qb_rand_U64_wrapper(qb_interpreter_context *cxt, uint64_t lower_limit, uint64_t upper_limit) {
	return qb_rand_U64(cxt, lower_limit, upper_limit);
}

void qb_resync_argument_wrapper(qb_interpreter_context *cxt, uint32_t argument_index) {
	qb_resync_argument(cxt, argument_index);
}

void qb_run_zend_extension_op_wrapper(qb_interpreter_context *cxt, uint32_t zend_opcode, uint32_t line_number) {
	qb_run_zend_extension_op(cxt, zend_opcode, line_number);
}

void qb_sample_bilinear_F32_wrapper(qb_interpreter_context *__restrict cxt, float32_t *__restrict pixels, uint32_t width, uint32_t height, float32_t x, float32_t y, float32_t *__restrict res_ptr) {
	qb_sample_bilinear_F32(cxt, pixels, width, height, x, y, res_ptr);
}

void qb_sample_bilinear_F64_wrapper(qb_interpreter_context *__restrict cxt, float64_t *__restrict pixels, uint32_t width, uint32_t height, float64_t x, float64_t y, float64_t *__restrict res_ptr) {
	qb_sample_bilinear_F64(cxt, pixels, width, height, x, y, res_ptr);
}

void qb_sample_nearest_F32_wrapper(qb_interpreter_context *__restrict cxt, float32_t *__restrict pixels, uint32_t width, uint32_t height, float32_t x, float32_t y, float32_t *__restrict res_ptr) {
	qb_sample_nearest_F32(cxt, pixels, width, height, x, y, res_ptr);
}

void qb_sample_nearest_F64_wrapper(qb_interpreter_context *__restrict cxt, float64_t *__restrict pixels, uint32_t width, uint32_t height, float64_t x, float64_t y, float64_t *__restrict res_ptr) {
	qb_sample_nearest_F64(cxt, pixels, width, height, x, y, res_ptr);
}

void qb_shrink_segment_wrapper(qb_interpreter_context *__restrict cxt, qb_memory_segment *segment, uint32_t start_index, uint32_t count) {
	qb_shrink_segment(cxt, segment, start_index, count);
}

void qb_sort_ascending_F32_wrapper(qb_interpreter_context *cxt, float32_t *elements, uint32_t count) {
	qb_sort_ascending_F32(cxt, elements, count);
}

void qb_sort_ascending_F64_wrapper(qb_interpreter_context *cxt, float64_t *elements, uint32_t count) {
	qb_sort_ascending_F64(cxt, elements, count);
}

void qb_sort_ascending_S08_wrapper(qb_interpreter_context *cxt, int8_t *elements, uint32_t count) {
	qb_sort_ascending_S08(cxt, elements, count);
}

void qb_sort_ascending_S16_wrapper(qb_interpreter_context *cxt, int16_t *elements, uint32_t count) {
	qb_sort_ascending_S16(cxt, elements, count);
}

void qb_sort_ascending_S32_wrapper(qb_interpreter_context *cxt, int32_t *elements, uint32_t count) {
	qb_sort_ascending_S32(cxt, elements, count);
}

void qb_sort_ascending_S64_wrapper(qb_interpreter_context *cxt, int64_t *elements, uint32_t count) {
	qb_sort_ascending_S64(cxt, elements, count);
}

void qb_sort_ascending_U08_wrapper(qb_interpreter_context *cxt, uint8_t *elements, uint32_t count) {
	qb_sort_ascending_U08(cxt, elements, count);
}

void qb_sort_ascending_U16_wrapper(qb_interpreter_context *cxt, uint16_t *elements, uint32_t count) {
	qb_sort_ascending_U16(cxt, elements, count);
}

void qb_sort_ascending_U32_wrapper(qb_interpreter_context *cxt, uint32_t *elements, uint32_t count) {
	qb_sort_ascending_U32(cxt, elements, count);
}

void qb_sort_ascending_U64_wrapper(qb_interpreter_context *cxt, uint64_t *elements, uint32_t count) {
	qb_sort_ascending_U64(cxt, elements, count);
}

void qb_sort_descending_F32_wrapper(qb_interpreter_context *cxt, float32_t *elements, uint32_t count) {
	qb_sort_descending_F32(cxt, elements, count);
}

void qb_sort_descending_F64_wrapper(qb_interpreter_context *cxt, float64_t *elements, uint32_t count) {
	qb_sort_descending_F64(cxt, elements, count);
}

void qb_sort_descending_S08_wrapper(qb_interpreter_context *cxt, int8_t *elements, uint32_t count) {
	qb_sort_descending_S08(cxt, elements, count);
}

void qb_sort_descending_S16_wrapper(qb_interpreter_context *cxt, int16_t *elements, uint32_t count) {
	qb_sort_descending_S16(cxt, elements, count);
}

void qb_sort_descending_S32_wrapper(qb_interpreter_context *cxt, int32_t *elements, uint32_t count) {
	qb_sort_descending_S32(cxt, elements, count);
}

void qb_sort_descending_S64_wrapper(qb_interpreter_context *cxt, int64_t *elements, uint32_t count) {
	qb_sort_descending_S64(cxt, elements, count);
}

void qb_sort_descending_U08_wrapper(qb_interpreter_context *cxt, uint8_t *elements, uint32_t count) {
	qb_sort_descending_U08(cxt, elements, count);
}

void qb_sort_descending_U16_wrapper(qb_interpreter_context *cxt, uint16_t *elements, uint32_t count) {
	qb_sort_descending_U16(cxt, elements, count);
}

void qb_sort_descending_U32_wrapper(qb_interpreter_context *cxt, uint32_t *elements, uint32_t count) {
	qb_sort_descending_U32(cxt, elements, count);
}

void qb_sort_descending_U64_wrapper(qb_interpreter_context *cxt, uint64_t *elements, uint32_t count) {
	qb_sort_descending_U64(cxt, elements, count);
}

void zend_bailout_wrapper(void) {
	zend_bailout();
}

int zend_finite_wrapper(double n) {
	return zend_finite(n);
}

int zend_isinf_wrapper(double n) {
	return zend_isinf(n);
}

int zend_isnan_wrapper(double n) {
	return zend_isnan(n);
}

qb_native_symbol global_native_symbols[] = {
	{	0,	"_php_math_round",	_php_math_round	},
	{	0,	"abs",	abs	},
	{	0,	"acos",	acos	},
	{	0,	"acosh",	acosh	},
	{	0,	"asin",	asin	},
	{	0,	"asinh",	asinh	},
	{	0,	"atan",	atan	},
	{	0,	"atan2",	atan2	},
	{	0,	"atanh",	atanh	},
	{	0,	"ceil",	ceil	},
	{	0,	"cos",	cos	},
	{	0,	"cosh",	cosh	},
	{	0,	"exp",	exp	},
	{	0,	"exp2",	exp2	},
	{	0,	"expm1",	expm1	},
	{	0,	"fabs",	fabs	},
	{	0,	"floor",	floor	},
	{	0,	"fmod",	fmod	},
	{	0,	"hypot",	hypot	},
	{	0,	"llabs",	llabs	},
	{	0,	"log",	log	},
	{	0,	"log10",	log10	},
	{	0,	"log1p",	log1p	},
	{	0,	"log2",	log2	},
	{	0,	"memcpy",	memcpy	},
	{	0,	"memset",	memset	},
	{	0,	"php_combined_lcg",	php_combined_lcg	},
	{	0,	"php_write",	php_write	},
	{	0,	"pow",	pow	},
	{	0,	"qb_abort",	qb_abort	},
	{	0,	"qb_abort_range_error",	qb_abort_range_error	},
	{	0,	"qb_calculate_array_distance_1x_F32",	qb_calculate_array_distance_1x_F32_wrapper	},
	{	0,	"qb_calculate_array_distance_1x_F64",	qb_calculate_array_distance_1x_F64_wrapper	},
	{	0,	"qb_calculate_array_distance_2x_F32",	qb_calculate_array_distance_2x_F32_wrapper	},
	{	0,	"qb_calculate_array_distance_2x_F64",	qb_calculate_array_distance_2x_F64_wrapper	},
	{	0,	"qb_calculate_array_distance_3x_F32",	qb_calculate_array_distance_3x_F32_wrapper	},
	{	0,	"qb_calculate_array_distance_3x_F64",	qb_calculate_array_distance_3x_F64_wrapper	},
	{	0,	"qb_calculate_array_distance_4x_F32",	qb_calculate_array_distance_4x_F32_wrapper	},
	{	0,	"qb_calculate_array_distance_4x_F64",	qb_calculate_array_distance_4x_F64_wrapper	},
	{	0,	"qb_calculate_array_distance_F32",	qb_calculate_array_distance_F32_wrapper	},
	{	0,	"qb_calculate_array_distance_F64",	qb_calculate_array_distance_F64_wrapper	},
	{	0,	"qb_calculate_array_length_1x_F32",	qb_calculate_array_length_1x_F32_wrapper	},
	{	0,	"qb_calculate_array_length_1x_F64",	qb_calculate_array_length_1x_F64_wrapper	},
	{	0,	"qb_calculate_array_length_2x_F32",	qb_calculate_array_length_2x_F32_wrapper	},
	{	0,	"qb_calculate_array_length_2x_F64",	qb_calculate_array_length_2x_F64_wrapper	},
	{	0,	"qb_calculate_array_length_3x_F32",	qb_calculate_array_length_3x_F32_wrapper	},
	{	0,	"qb_calculate_array_length_3x_F64",	qb_calculate_array_length_3x_F64_wrapper	},
	{	0,	"qb_calculate_array_length_4x_F32",	qb_calculate_array_length_4x_F32_wrapper	},
	{	0,	"qb_calculate_array_length_4x_F64",	qb_calculate_array_length_4x_F64_wrapper	},
	{	0,	"qb_calculate_array_length_F32",	qb_calculate_array_length_F32_wrapper	},
	{	0,	"qb_calculate_array_length_F64",	qb_calculate_array_length_F64_wrapper	},
	{	0,	"qb_calculate_array_max_F32",	qb_calculate_array_max_F32_wrapper	},
	{	0,	"qb_calculate_array_max_F64",	qb_calculate_array_max_F64_wrapper	},
	{	0,	"qb_calculate_array_max_S08",	qb_calculate_array_max_S08_wrapper	},
	{	0,	"qb_calculate_array_max_S16",	qb_calculate_array_max_S16_wrapper	},
	{	0,	"qb_calculate_array_max_S32",	qb_calculate_array_max_S32_wrapper	},
	{	0,	"qb_calculate_array_max_S64",	qb_calculate_array_max_S64_wrapper	},
	{	0,	"qb_calculate_array_max_U08",	qb_calculate_array_max_U08_wrapper	},
	{	0,	"qb_calculate_array_max_U16",	qb_calculate_array_max_U16_wrapper	},
	{	0,	"qb_calculate_array_max_U32",	qb_calculate_array_max_U32_wrapper	},
	{	0,	"qb_calculate_array_max_U64",	qb_calculate_array_max_U64_wrapper	},
	{	0,	"qb_calculate_array_min_F32",	qb_calculate_array_min_F32_wrapper	},
	{	0,	"qb_calculate_array_min_F64",	qb_calculate_array_min_F64_wrapper	},
	{	0,	"qb_calculate_array_min_S08",	qb_calculate_array_min_S08_wrapper	},
	{	0,	"qb_calculate_array_min_S16",	qb_calculate_array_min_S16_wrapper	},
	{	0,	"qb_calculate_array_min_S32",	qb_calculate_array_min_S32_wrapper	},
	{	0,	"qb_calculate_array_min_S64",	qb_calculate_array_min_S64_wrapper	},
	{	0,	"qb_calculate_array_min_U08",	qb_calculate_array_min_U08_wrapper	},
	{	0,	"qb_calculate_array_min_U16",	qb_calculate_array_min_U16_wrapper	},
	{	0,	"qb_calculate_array_min_U32",	qb_calculate_array_min_U32_wrapper	},
	{	0,	"qb_calculate_array_min_U64",	qb_calculate_array_min_U64_wrapper	},
	{	0,	"qb_calculate_array_product_F32",	qb_calculate_array_product_F32_wrapper	},
	{	0,	"qb_calculate_array_product_F64",	qb_calculate_array_product_F64_wrapper	},
	{	0,	"qb_calculate_array_product_S08",	qb_calculate_array_product_S08_wrapper	},
	{	0,	"qb_calculate_array_product_S16",	qb_calculate_array_product_S16_wrapper	},
	{	0,	"qb_calculate_array_product_S32",	qb_calculate_array_product_S32_wrapper	},
	{	0,	"qb_calculate_array_product_S64",	qb_calculate_array_product_S64_wrapper	},
	{	0,	"qb_calculate_array_product_U08",	qb_calculate_array_product_U08_wrapper	},
	{	0,	"qb_calculate_array_product_U16",	qb_calculate_array_product_U16_wrapper	},
	{	0,	"qb_calculate_array_product_U32",	qb_calculate_array_product_U32_wrapper	},
	{	0,	"qb_calculate_array_product_U64",	qb_calculate_array_product_U64_wrapper	},
	{	0,	"qb_calculate_array_sum_F32",	qb_calculate_array_sum_F32_wrapper	},
	{	0,	"qb_calculate_array_sum_F64",	qb_calculate_array_sum_F64_wrapper	},
	{	0,	"qb_calculate_array_sum_S08",	qb_calculate_array_sum_S08_wrapper	},
	{	0,	"qb_calculate_array_sum_S16",	qb_calculate_array_sum_S16_wrapper	},
	{	0,	"qb_calculate_array_sum_S32",	qb_calculate_array_sum_S32_wrapper	},
	{	0,	"qb_calculate_array_sum_S64",	qb_calculate_array_sum_S64_wrapper	},
	{	0,	"qb_calculate_array_sum_U08",	qb_calculate_array_sum_U08_wrapper	},
	{	0,	"qb_calculate_array_sum_U16",	qb_calculate_array_sum_U16_wrapper	},
	{	0,	"qb_calculate_array_sum_U32",	qb_calculate_array_sum_U32_wrapper	},
	{	0,	"qb_calculate_array_sum_U64",	qb_calculate_array_sum_U64_wrapper	},
	{	0,	"qb_calculate_cross_product_F32",	qb_calculate_cross_product_F32_wrapper	},
	{	0,	"qb_calculate_cross_product_F64",	qb_calculate_cross_product_F64_wrapper	},
	{	0,	"qb_calculate_dot_product_1x_F32",	qb_calculate_dot_product_1x_F32_wrapper	},
	{	0,	"qb_calculate_dot_product_1x_F64",	qb_calculate_dot_product_1x_F64_wrapper	},
	{	0,	"qb_calculate_dot_product_2x_F32",	qb_calculate_dot_product_2x_F32_wrapper	},
	{	0,	"qb_calculate_dot_product_2x_F64",	qb_calculate_dot_product_2x_F64_wrapper	},
	{	0,	"qb_calculate_dot_product_3x_F32",	qb_calculate_dot_product_3x_F32_wrapper	},
	{	0,	"qb_calculate_dot_product_3x_F64",	qb_calculate_dot_product_3x_F64_wrapper	},
	{	0,	"qb_calculate_dot_product_4x_F32",	qb_calculate_dot_product_4x_F32_wrapper	},
	{	0,	"qb_calculate_dot_product_4x_F64",	qb_calculate_dot_product_4x_F64_wrapper	},
	{	0,	"qb_calculate_dot_product_F32",	qb_calculate_dot_product_F32_wrapper	},
	{	0,	"qb_calculate_dot_product_F64",	qb_calculate_dot_product_F64_wrapper	},
	{	0,	"qb_calculate_face_forward_1x_F32",	qb_calculate_face_forward_1x_F32_wrapper	},
	{	0,	"qb_calculate_face_forward_1x_F64",	qb_calculate_face_forward_1x_F64_wrapper	},
	{	0,	"qb_calculate_face_forward_2x_F32",	qb_calculate_face_forward_2x_F32_wrapper	},
	{	0,	"qb_calculate_face_forward_2x_F64",	qb_calculate_face_forward_2x_F64_wrapper	},
	{	0,	"qb_calculate_face_forward_3x_F32",	qb_calculate_face_forward_3x_F32_wrapper	},
	{	0,	"qb_calculate_face_forward_3x_F64",	qb_calculate_face_forward_3x_F64_wrapper	},
	{	0,	"qb_calculate_face_forward_4x_F32",	qb_calculate_face_forward_4x_F32_wrapper	},
	{	0,	"qb_calculate_face_forward_4x_F64",	qb_calculate_face_forward_4x_F64_wrapper	},
	{	0,	"qb_calculate_face_forward_F32",	qb_calculate_face_forward_F32_wrapper	},
	{	0,	"qb_calculate_face_forward_F64",	qb_calculate_face_forward_F64_wrapper	},
	{	0,	"qb_calculate_reflect_1x_F32",	qb_calculate_reflect_1x_F32_wrapper	},
	{	0,	"qb_calculate_reflect_1x_F64",	qb_calculate_reflect_1x_F64_wrapper	},
	{	0,	"qb_calculate_reflect_2x_F32",	qb_calculate_reflect_2x_F32_wrapper	},
	{	0,	"qb_calculate_reflect_2x_F64",	qb_calculate_reflect_2x_F64_wrapper	},
	{	0,	"qb_calculate_reflect_3x_F32",	qb_calculate_reflect_3x_F32_wrapper	},
	{	0,	"qb_calculate_reflect_3x_F64",	qb_calculate_reflect_3x_F64_wrapper	},
	{	0,	"qb_calculate_reflect_4x_F32",	qb_calculate_reflect_4x_F32_wrapper	},
	{	0,	"qb_calculate_reflect_4x_F64",	qb_calculate_reflect_4x_F64_wrapper	},
	{	0,	"qb_calculate_reflect_F32",	qb_calculate_reflect_F32_wrapper	},
	{	0,	"qb_calculate_reflect_F64",	qb_calculate_reflect_F64_wrapper	},
	{	0,	"qb_calculate_refract_1x_F32",	qb_calculate_refract_1x_F32_wrapper	},
	{	0,	"qb_calculate_refract_1x_F64",	qb_calculate_refract_1x_F64_wrapper	},
	{	0,	"qb_calculate_refract_2x_F32",	qb_calculate_refract_2x_F32_wrapper	},
	{	0,	"qb_calculate_refract_2x_F64",	qb_calculate_refract_2x_F64_wrapper	},
	{	0,	"qb_calculate_refract_3x_F32",	qb_calculate_refract_3x_F32_wrapper	},
	{	0,	"qb_calculate_refract_3x_F64",	qb_calculate_refract_3x_F64_wrapper	},
	{	0,	"qb_calculate_refract_4x_F32",	qb_calculate_refract_4x_F32_wrapper	},
	{	0,	"qb_calculate_refract_4x_F64",	qb_calculate_refract_4x_F64_wrapper	},
	{	0,	"qb_calculate_refract_F32",	qb_calculate_refract_F32_wrapper	},
	{	0,	"qb_calculate_refract_F64",	qb_calculate_refract_F64_wrapper	},
	{	0,	"qb_compare_array_F32",	qb_compare_array_F32_wrapper	},
	{	0,	"qb_compare_array_F64",	qb_compare_array_F64_wrapper	},
	{	0,	"qb_compare_array_S08",	qb_compare_array_S08_wrapper	},
	{	0,	"qb_compare_array_S16",	qb_compare_array_S16_wrapper	},
	{	0,	"qb_compare_array_S32",	qb_compare_array_S32_wrapper	},
	{	0,	"qb_compare_array_S64",	qb_compare_array_S64_wrapper	},
	{	0,	"qb_compare_array_U08",	qb_compare_array_U08_wrapper	},
	{	0,	"qb_compare_array_U16",	qb_compare_array_U16_wrapper	},
	{	0,	"qb_compare_array_U32",	qb_compare_array_U32_wrapper	},
	{	0,	"qb_compare_array_U64",	qb_compare_array_U64_wrapper	},
	{	0,	"qb_compare_ascending_F32",	qb_compare_ascending_F32	},
	{	0,	"qb_compare_ascending_F64",	qb_compare_ascending_F64	},
	{	0,	"qb_compare_ascending_S08",	qb_compare_ascending_S08	},
	{	0,	"qb_compare_ascending_S16",	qb_compare_ascending_S16	},
	{	0,	"qb_compare_ascending_S32",	qb_compare_ascending_S32	},
	{	0,	"qb_compare_ascending_S64",	qb_compare_ascending_S64	},
	{	0,	"qb_compare_ascending_U08",	qb_compare_ascending_U08	},
	{	0,	"qb_compare_ascending_U16",	qb_compare_ascending_U16	},
	{	0,	"qb_compare_ascending_U32",	qb_compare_ascending_U32	},
	{	0,	"qb_compare_ascending_U64",	qb_compare_ascending_U64	},
	{	0,	"qb_compare_descending_F32",	qb_compare_descending_F32	},
	{	0,	"qb_compare_descending_F64",	qb_compare_descending_F64	},
	{	0,	"qb_compare_descending_S08",	qb_compare_descending_S08	},
	{	0,	"qb_compare_descending_S16",	qb_compare_descending_S16	},
	{	0,	"qb_compare_descending_S32",	qb_compare_descending_S32	},
	{	0,	"qb_compare_descending_S64",	qb_compare_descending_S64	},
	{	0,	"qb_compare_descending_U08",	qb_compare_descending_U08	},
	{	0,	"qb_compare_descending_U16",	qb_compare_descending_U16	},
	{	0,	"qb_compare_descending_U32",	qb_compare_descending_U32	},
	{	0,	"qb_compare_descending_U64",	qb_compare_descending_U64	},
	{	0,	"qb_copy_argument",	qb_copy_argument_wrapper	},
	{	0,	"qb_decode_fcall_mix_operand",	qb_decode_fcall_mix_operand_wrapper	},
	{	0,	"qb_decode_fcall_variable_operand",	qb_decode_fcall_variable_operand_wrapper	},
	{	0,	"qb_decode_utf8_U16",	qb_decode_utf8_U16_wrapper	},
	{	0,	"qb_decode_utf8_U32",	qb_decode_utf8_U32_wrapper	},
	{	0,	"qb_encode_utf8_U16",	qb_encode_utf8_U16_wrapper	},
	{	0,	"qb_encode_utf8_U32",	qb_encode_utf8_U32_wrapper	},
	{	0,	"qb_enlarge_segment",	qb_enlarge_segment_wrapper	},
	{	0,	"qb_execute_function_call",	qb_execute_function_call_wrapper	},
	{	0,	"qb_finalize_function_call",	qb_finalize_function_call_wrapper	},
	{	0,	"qb_find_element_F32",	qb_find_element_F32_wrapper	},
	{	0,	"qb_find_element_F64",	qb_find_element_F64_wrapper	},
	{	0,	"qb_find_element_I08",	qb_find_element_I08_wrapper	},
	{	0,	"qb_find_element_I16",	qb_find_element_I16_wrapper	},
	{	0,	"qb_find_element_I32",	qb_find_element_I32_wrapper	},
	{	0,	"qb_find_element_I64",	qb_find_element_I64_wrapper	},
	{	0,	"qb_find_elements_F32",	qb_find_elements_F32_wrapper	},
	{	0,	"qb_find_elements_F64",	qb_find_elements_F64_wrapper	},
	{	0,	"qb_find_elements_I08",	qb_find_elements_I08_wrapper	},
	{	0,	"qb_find_elements_I16",	qb_find_elements_I16_wrapper	},
	{	0,	"qb_find_elements_I32",	qb_find_elements_I32_wrapper	},
	{	0,	"qb_find_elements_I64",	qb_find_elements_I64_wrapper	},
	{	0,	"qb_find_subarray_F32",	qb_find_subarray_F32_wrapper	},
	{	0,	"qb_find_subarray_F64",	qb_find_subarray_F64_wrapper	},
	{	0,	"qb_find_subarray_I08",	qb_find_subarray_I08_wrapper	},
	{	0,	"qb_find_subarray_I16",	qb_find_subarray_I16_wrapper	},
	{	0,	"qb_find_subarray_I32",	qb_find_subarray_I32_wrapper	},
	{	0,	"qb_find_subarray_I64",	qb_find_subarray_I64_wrapper	},
	{	0,	"qb_find_subarray_from_end_F32",	qb_find_subarray_from_end_F32_wrapper	},
	{	0,	"qb_find_subarray_from_end_F64",	qb_find_subarray_from_end_F64_wrapper	},
	{	0,	"qb_find_subarray_from_end_I08",	qb_find_subarray_from_end_I08_wrapper	},
	{	0,	"qb_find_subarray_from_end_I16",	qb_find_subarray_from_end_I16_wrapper	},
	{	0,	"qb_find_subarray_from_end_I32",	qb_find_subarray_from_end_I32_wrapper	},
	{	0,	"qb_find_subarray_from_end_I64",	qb_find_subarray_from_end_I64_wrapper	},
	{	0,	"qb_get_utf8_codepoint_count",	qb_get_utf8_codepoint_count_wrapper	},
	{	0,	"qb_get_utf8_encoded_length_U16",	qb_get_utf8_encoded_length_U16_wrapper	},
	{	0,	"qb_get_utf8_encoded_length_U32",	qb_get_utf8_encoded_length_U32_wrapper	},
	{	0,	"qb_initialize_function_call",	qb_initialize_function_call_wrapper	},
	{	0,	"qb_mt_rand_S64",	qb_mt_rand_S64_wrapper	},
	{	0,	"qb_mt_rand_U64",	qb_mt_rand_U64_wrapper	},
	{	0,	"qb_multiply_matrix_by_matrix_1x1_F32",	qb_multiply_matrix_by_matrix_1x1_F32_wrapper	},
	{	0,	"qb_multiply_matrix_by_matrix_1x1_F64",	qb_multiply_matrix_by_matrix_1x1_F64_wrapper	},
	{	0,	"qb_multiply_matrix_by_matrix_2x2_F32",	qb_multiply_matrix_by_matrix_2x2_F32_wrapper	},
	{	0,	"qb_multiply_matrix_by_matrix_2x2_F64",	qb_multiply_matrix_by_matrix_2x2_F64_wrapper	},
	{	0,	"qb_multiply_matrix_by_matrix_3x3_F32",	qb_multiply_matrix_by_matrix_3x3_F32_wrapper	},
	{	0,	"qb_multiply_matrix_by_matrix_3x3_F64",	qb_multiply_matrix_by_matrix_3x3_F64_wrapper	},
	{	0,	"qb_multiply_matrix_by_matrix_3x3_padded_F32",	qb_multiply_matrix_by_matrix_3x3_padded_F32_wrapper	},
	{	0,	"qb_multiply_matrix_by_matrix_3x3_padded_F64",	qb_multiply_matrix_by_matrix_3x3_padded_F64_wrapper	},
	{	0,	"qb_multiply_matrix_by_matrix_4x4_F32",	qb_multiply_matrix_by_matrix_4x4_F32_wrapper	},
	{	0,	"qb_multiply_matrix_by_matrix_4x4_F64",	qb_multiply_matrix_by_matrix_4x4_F64_wrapper	},
	{	0,	"qb_multiply_matrix_by_matrix_F32",	qb_multiply_matrix_by_matrix_F32_wrapper	},
	{	0,	"qb_multiply_matrix_by_matrix_F64",	qb_multiply_matrix_by_matrix_F64_wrapper	},
	{	0,	"qb_multiply_matrix_by_vector_1x1_F32",	qb_multiply_matrix_by_vector_1x1_F32_wrapper	},
	{	0,	"qb_multiply_matrix_by_vector_1x1_F64",	qb_multiply_matrix_by_vector_1x1_F64_wrapper	},
	{	0,	"qb_multiply_matrix_by_vector_2x2_F32",	qb_multiply_matrix_by_vector_2x2_F32_wrapper	},
	{	0,	"qb_multiply_matrix_by_vector_2x2_F64",	qb_multiply_matrix_by_vector_2x2_F64_wrapper	},
	{	0,	"qb_multiply_matrix_by_vector_3x3_F32",	qb_multiply_matrix_by_vector_3x3_F32_wrapper	},
	{	0,	"qb_multiply_matrix_by_vector_3x3_F64",	qb_multiply_matrix_by_vector_3x3_F64_wrapper	},
	{	0,	"qb_multiply_matrix_by_vector_3x3_padded_F32",	qb_multiply_matrix_by_vector_3x3_padded_F32_wrapper	},
	{	0,	"qb_multiply_matrix_by_vector_3x3_padded_F64",	qb_multiply_matrix_by_vector_3x3_padded_F64_wrapper	},
	{	0,	"qb_multiply_matrix_by_vector_4x4_F32",	qb_multiply_matrix_by_vector_4x4_F32_wrapper	},
	{	0,	"qb_multiply_matrix_by_vector_4x4_F64",	qb_multiply_matrix_by_vector_4x4_F64_wrapper	},
	{	0,	"qb_multiply_matrix_by_vector_F32",	qb_multiply_matrix_by_vector_F32_wrapper	},
	{	0,	"qb_multiply_matrix_by_vector_F64",	qb_multiply_matrix_by_vector_F64_wrapper	},
	{	0,	"qb_multiply_vector_by_matrix_1x1_F32",	qb_multiply_vector_by_matrix_1x1_F32_wrapper	},
	{	0,	"qb_multiply_vector_by_matrix_1x1_F64",	qb_multiply_vector_by_matrix_1x1_F64_wrapper	},
	{	0,	"qb_multiply_vector_by_matrix_2x2_F32",	qb_multiply_vector_by_matrix_2x2_F32_wrapper	},
	{	0,	"qb_multiply_vector_by_matrix_2x2_F64",	qb_multiply_vector_by_matrix_2x2_F64_wrapper	},
	{	0,	"qb_multiply_vector_by_matrix_3x3_F32",	qb_multiply_vector_by_matrix_3x3_F32_wrapper	},
	{	0,	"qb_multiply_vector_by_matrix_3x3_F64",	qb_multiply_vector_by_matrix_3x3_F64_wrapper	},
	{	0,	"qb_multiply_vector_by_matrix_3x3_padded_F32",	qb_multiply_vector_by_matrix_3x3_padded_F32_wrapper	},
	{	0,	"qb_multiply_vector_by_matrix_3x3_padded_F64",	qb_multiply_vector_by_matrix_3x3_padded_F64_wrapper	},
	{	0,	"qb_multiply_vector_by_matrix_4x4_F32",	qb_multiply_vector_by_matrix_4x4_F32_wrapper	},
	{	0,	"qb_multiply_vector_by_matrix_4x4_F64",	qb_multiply_vector_by_matrix_4x4_F64_wrapper	},
	{	0,	"qb_multiply_vector_by_matrix_F32",	qb_multiply_vector_by_matrix_F32_wrapper	},
	{	0,	"qb_multiply_vector_by_matrix_F64",	qb_multiply_vector_by_matrix_F64_wrapper	},
	{	0,	"qb_normalize_array_1x_F32",	qb_normalize_array_1x_F32_wrapper	},
	{	0,	"qb_normalize_array_1x_F64",	qb_normalize_array_1x_F64_wrapper	},
	{	0,	"qb_normalize_array_2x_F32",	qb_normalize_array_2x_F32_wrapper	},
	{	0,	"qb_normalize_array_2x_F64",	qb_normalize_array_2x_F64_wrapper	},
	{	0,	"qb_normalize_array_3x_F32",	qb_normalize_array_3x_F32_wrapper	},
	{	0,	"qb_normalize_array_3x_F64",	qb_normalize_array_3x_F64_wrapper	},
	{	0,	"qb_normalize_array_4x_F32",	qb_normalize_array_4x_F32_wrapper	},
	{	0,	"qb_normalize_array_4x_F64",	qb_normalize_array_4x_F64_wrapper	},
	{	0,	"qb_normalize_array_F32",	qb_normalize_array_F32_wrapper	},
	{	0,	"qb_normalize_array_F64",	qb_normalize_array_F64_wrapper	},
	{	0,	"qb_rand_S64",	qb_rand_S64_wrapper	},
	{	0,	"qb_rand_U64",	qb_rand_U64_wrapper	},
	{	0,	"qb_resync_argument",	qb_resync_argument_wrapper	},
	{	0,	"qb_run_zend_extension_op",	qb_run_zend_extension_op_wrapper	},
	{	0,	"qb_sample_bilinear_F32",	qb_sample_bilinear_F32_wrapper	},
	{	0,	"qb_sample_bilinear_F64",	qb_sample_bilinear_F64_wrapper	},
	{	0,	"qb_sample_nearest_F32",	qb_sample_nearest_F32_wrapper	},
	{	0,	"qb_sample_nearest_F64",	qb_sample_nearest_F64_wrapper	},
	{	0,	"qb_shrink_segment",	qb_shrink_segment_wrapper	},
	{	0,	"qb_sort_ascending_F32",	qb_sort_ascending_F32_wrapper	},
	{	0,	"qb_sort_ascending_F64",	qb_sort_ascending_F64_wrapper	},
	{	0,	"qb_sort_ascending_S08",	qb_sort_ascending_S08_wrapper	},
	{	0,	"qb_sort_ascending_S16",	qb_sort_ascending_S16_wrapper	},
	{	0,	"qb_sort_ascending_S32",	qb_sort_ascending_S32_wrapper	},
	{	0,	"qb_sort_ascending_S64",	qb_sort_ascending_S64_wrapper	},
	{	0,	"qb_sort_ascending_U08",	qb_sort_ascending_U08_wrapper	},
	{	0,	"qb_sort_ascending_U16",	qb_sort_ascending_U16_wrapper	},
	{	0,	"qb_sort_ascending_U32",	qb_sort_ascending_U32_wrapper	},
	{	0,	"qb_sort_ascending_U64",	qb_sort_ascending_U64_wrapper	},
	{	0,	"qb_sort_descending_F32",	qb_sort_descending_F32_wrapper	},
	{	0,	"qb_sort_descending_F64",	qb_sort_descending_F64_wrapper	},
	{	0,	"qb_sort_descending_S08",	qb_sort_descending_S08_wrapper	},
	{	0,	"qb_sort_descending_S16",	qb_sort_descending_S16_wrapper	},
	{	0,	"qb_sort_descending_S32",	qb_sort_descending_S32_wrapper	},
	{	0,	"qb_sort_descending_S64",	qb_sort_descending_S64_wrapper	},
	{	0,	"qb_sort_descending_U08",	qb_sort_descending_U08_wrapper	},
	{	0,	"qb_sort_descending_U16",	qb_sort_descending_U16_wrapper	},
	{	0,	"qb_sort_descending_U32",	qb_sort_descending_U32_wrapper	},
	{	0,	"qb_sort_descending_U64",	qb_sort_descending_U64_wrapper	},
	{	0,	"round",	round	},
	{	0,	"sin",	sin	},
	{	0,	"sinh",	sinh	},
	{	0,	"snprintf",	snprintf	},
	{	0,	"sqrt",	sqrt	},
	{	0,	"strlen",	strlen	},
	{	0,	"tan",	tan	},
	{	0,	"tanh",	tanh	},
	{	0,	"zend_bailout",	zend_bailout_wrapper	},
	{	0,	"zend_finite",	zend_finite_wrapper	},
	{	0,	"zend_isinf",	zend_isinf_wrapper	},
	{	0,	"zend_isnan",	zend_isnan_wrapper	},
	{	0,	"zend_timeout",	zend_timeout	},
};
