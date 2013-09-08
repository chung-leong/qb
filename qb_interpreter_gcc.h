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


#pragma pack(push,1)

typedef struct qb_instruction_0 {
	void *next_handler;
} qb_instruction_0;

typedef struct qb_instruction_1 {
	void *next_handler;
	uint32_t operand1;
} qb_instruction_1;

typedef struct qb_instruction_1_lineno {
	void *next_handler;
	uint32_t operand1;
	uint32_t line_number;
} qb_instruction_1_lineno;

typedef struct qb_instruction_2 {
	void *next_handler;
	uint32_t operand1;
	uint32_t operand2;
} qb_instruction_2;

typedef struct qb_instruction_2_lineno {
	void *next_handler;
	uint32_t operand1;
	uint32_t operand2;
	uint32_t line_number;
} qb_instruction_2_lineno;

typedef struct qb_instruction_2_matrix_lineno {
	void *next_handler;
	uint32_t operand1;
	uint32_t operand2;
	uint32_t matrix_dimensions;
	uint32_t line_number;
} qb_instruction_2_matrix_lineno;

typedef struct qb_instruction_3 {
	void *next_handler;
	uint32_t operand1;
	uint32_t operand2;
	uint32_t operand3;
} qb_instruction_3;

typedef struct qb_instruction_3_lineno {
	void *next_handler;
	uint32_t operand1;
	uint32_t operand2;
	uint32_t operand3;
	uint32_t line_number;
} qb_instruction_3_lineno;

typedef struct qb_instruction_3_matrix_lineno {
	void *next_handler;
	uint32_t operand1;
	uint32_t operand2;
	uint32_t operand3;
	uint32_t matrix_dimensions;
	uint32_t line_number;
} qb_instruction_3_matrix_lineno;

typedef struct qb_instruction_4 {
	void *next_handler;
	uint32_t operand1;
	uint32_t operand2;
	uint32_t operand3;
	uint32_t operand4;
} qb_instruction_4;

typedef struct qb_instruction_4_lineno {
	void *next_handler;
	uint32_t operand1;
	uint32_t operand2;
	uint32_t operand3;
	uint32_t operand4;
	uint32_t line_number;
} qb_instruction_4_lineno;

typedef struct qb_instruction_4_matrix_lineno {
	void *next_handler;
	uint32_t operand1;
	uint32_t operand2;
	uint32_t operand3;
	uint32_t operand4;
	uint32_t matrix_dimensions;
	uint32_t line_number;
} qb_instruction_4_matrix_lineno;

typedef struct qb_instruction_5_lineno {
	void *next_handler;
	uint32_t operand1;
	uint32_t operand2;
	uint32_t operand3;
	uint32_t operand4;
	uint32_t operand5;
	uint32_t line_number;
} qb_instruction_5_lineno;

typedef struct qb_instruction_6_lineno {
	void *next_handler;
	uint32_t operand1;
	uint32_t operand2;
	uint32_t operand3;
	uint32_t operand4;
	uint32_t operand5;
	uint32_t operand6;
	uint32_t line_number;
} qb_instruction_6_lineno;

typedef struct qb_instruction_array_resize {
	void *next_handler;
	uint16_t operand_size;
	uint16_t argument_count;
	uint32_t operand1;
	uint32_t operands[1];
} qb_instruction_array_resize;

typedef struct qb_instruction_branch_0 {
	void *next_handler1;
	int8_t *instruction_pointer1;
	void *next_handler2;
	int8_t *instruction_pointer2;
} qb_instruction_branch_0;

typedef struct qb_instruction_branch_1 {
	void *next_handler1;
	int8_t *instruction_pointer1;
	void *next_handler2;
	int8_t *instruction_pointer2;
	uint32_t operand1;
} qb_instruction_branch_1;

typedef struct qb_instruction_branch_1_lineno {
	void *next_handler1;
	int8_t *instruction_pointer1;
	void *next_handler2;
	int8_t *instruction_pointer2;
	uint32_t operand1;
	uint32_t line_number;
} qb_instruction_branch_1_lineno;

typedef struct qb_instruction_branch_2 {
	void *next_handler1;
	int8_t *instruction_pointer1;
	void *next_handler2;
	int8_t *instruction_pointer2;
	uint32_t operand1;
	uint32_t operand2;
} qb_instruction_branch_2;

typedef struct qb_instruction_branch_2_lineno {
	void *next_handler1;
	int8_t *instruction_pointer1;
	void *next_handler2;
	int8_t *instruction_pointer2;
	uint32_t operand1;
	uint32_t operand2;
	uint32_t line_number;
} qb_instruction_branch_2_lineno;

typedef struct qb_instruction_fcall {
	void *next_handler;
	uint16_t operand_size;
	uint16_t argument_count;
	uint32_t line_number;
	uint32_t symbol_index;
	uint32_t operands[1];
} qb_instruction_fcall;

typedef struct qb_instruction_jump_0 {
	void *next_handler;
	int8_t *instruction_pointer;
} qb_instruction_jump_0;

#pragma pack(pop)

extern void *op_handlers[];

NO_RETURN void qb_abort_divide_by_zero_error(qb_interpreter_context *restrict cxt, uint32_t line_number);
NO_RETURN void qb_abort_missing_column(qb_interpreter_context *restrict cxt, uint32_t column_offset, uint32_t line_number);
int qb_compare_ascending_F32(const void *p1, const void *p2);
int qb_compare_ascending_F32_array(const void *p1, const void *p2, const void *p3);
int qb_compare_ascending_F64(const void *p1, const void *p2);
int qb_compare_ascending_F64_array(const void *p1, const void *p2, const void *p3);
int qb_compare_ascending_S08(const void *p1, const void *p2);
int qb_compare_ascending_S08_array(const void *p1, const void *p2, const void *p3);
int qb_compare_ascending_S16(const void *p1, const void *p2);
int qb_compare_ascending_S16_array(const void *p1, const void *p2, const void *p3);
int qb_compare_ascending_S32(const void *p1, const void *p2);
int qb_compare_ascending_S32_array(const void *p1, const void *p2, const void *p3);
int qb_compare_ascending_S64(const void *p1, const void *p2);
int qb_compare_ascending_S64_array(const void *p1, const void *p2, const void *p3);
int qb_compare_ascending_U08(const void *p1, const void *p2);
int qb_compare_ascending_U08_array(const void *p1, const void *p2, const void *p3);
int qb_compare_ascending_U16(const void *p1, const void *p2);
int qb_compare_ascending_U16_array(const void *p1, const void *p2, const void *p3);
int qb_compare_ascending_U32(const void *p1, const void *p2);
int qb_compare_ascending_U32_array(const void *p1, const void *p2, const void *p3);
int qb_compare_ascending_U64(const void *p1, const void *p2);
int qb_compare_ascending_U64_array(const void *p1, const void *p2, const void *p3);
int qb_compare_descending_F32(const void *p1, const void *p2);
int qb_compare_descending_F32_array(const void *p1, const void *p2, const void *p3);
int qb_compare_descending_F64(const void *p1, const void *p2);
int qb_compare_descending_F64_array(const void *p1, const void *p2, const void *p3);
int qb_compare_descending_S08(const void *p1, const void *p2);
int qb_compare_descending_S08_array(const void *p1, const void *p2, const void *p3);
int qb_compare_descending_S16(const void *p1, const void *p2);
int qb_compare_descending_S16_array(const void *p1, const void *p2, const void *p3);
int qb_compare_descending_S32(const void *p1, const void *p2);
int qb_compare_descending_S32_array(const void *p1, const void *p2, const void *p3);
int qb_compare_descending_S64(const void *p1, const void *p2);
int qb_compare_descending_S64_array(const void *p1, const void *p2, const void *p3);
int qb_compare_descending_U08(const void *p1, const void *p2);
int qb_compare_descending_U08_array(const void *p1, const void *p2, const void *p3);
int qb_compare_descending_U16(const void *p1, const void *p2);
int qb_compare_descending_U16_array(const void *p1, const void *p2, const void *p3);
int qb_compare_descending_U32(const void *p1, const void *p2);
int qb_compare_descending_U32_array(const void *p1, const void *p2, const void *p3);
int qb_compare_descending_U64(const void *p1, const void *p2);
int qb_compare_descending_U64_array(const void *p1, const void *p2, const void *p3);
int32_t ZEND_FASTCALL qb_compare_array_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count);
int32_t ZEND_FASTCALL qb_compare_array_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count);
int32_t ZEND_FASTCALL qb_compare_array_S08(int8_t *op1_ptr, uint32_t op1_count, int8_t *op2_ptr, uint32_t op2_count);
int32_t ZEND_FASTCALL qb_compare_array_S16(int16_t *op1_ptr, uint32_t op1_count, int16_t *op2_ptr, uint32_t op2_count);
int32_t ZEND_FASTCALL qb_compare_array_S32(int32_t *op1_ptr, uint32_t op1_count, int32_t *op2_ptr, uint32_t op2_count);
int32_t ZEND_FASTCALL qb_compare_array_S64(int64_t *op1_ptr, uint32_t op1_count, int64_t *op2_ptr, uint32_t op2_count);
int32_t ZEND_FASTCALL qb_compare_array_U08(uint8_t *op1_ptr, uint32_t op1_count, uint8_t *op2_ptr, uint32_t op2_count);
int32_t ZEND_FASTCALL qb_compare_array_U16(uint16_t *op1_ptr, uint32_t op1_count, uint16_t *op2_ptr, uint32_t op2_count);
int32_t ZEND_FASTCALL qb_compare_array_U32(uint32_t *op1_ptr, uint32_t op1_count, uint32_t *op2_ptr, uint32_t op2_count);
int32_t ZEND_FASTCALL qb_compare_array_U64(uint64_t *op1_ptr, uint32_t op1_count, uint64_t *op2_ptr, uint32_t op2_count);
uint32_t ZEND_FASTCALL qb_decode_fcall_mix_operand(qb_interpreter_context *__restrict cxt, int8_t *__restrict *segments, uint32_t *__restrict operands);
uint32_t ZEND_FASTCALL qb_decode_fcall_variable_operand(qb_interpreter_context *__restrict cxt, int8_t *__restrict *segments, uint32_t *__restrict operands);
uint32_t ZEND_FASTCALL qb_get_array_sprintf_length_F32(qb_interpreter_context *cxt, float32_t *op1_ptr, uint32_t op1_count);
uint32_t ZEND_FASTCALL qb_get_array_sprintf_length_F64(qb_interpreter_context *cxt, float64_t *op1_ptr, uint32_t op1_count);
uint32_t ZEND_FASTCALL qb_get_array_sprintf_length_S08(qb_interpreter_context *cxt, int8_t *op1_ptr, uint32_t op1_count);
uint32_t ZEND_FASTCALL qb_get_array_sprintf_length_S16(qb_interpreter_context *cxt, int16_t *op1_ptr, uint32_t op1_count);
uint32_t ZEND_FASTCALL qb_get_array_sprintf_length_S32(qb_interpreter_context *cxt, int32_t *op1_ptr, uint32_t op1_count);
uint32_t ZEND_FASTCALL qb_get_array_sprintf_length_S64(qb_interpreter_context *cxt, int64_t *op1_ptr, uint32_t op1_count);
uint32_t ZEND_FASTCALL qb_get_array_sprintf_length_U08(qb_interpreter_context *cxt, uint8_t *op1_ptr, uint32_t op1_count);
uint32_t ZEND_FASTCALL qb_get_array_sprintf_length_U16(qb_interpreter_context *cxt, uint16_t *op1_ptr, uint32_t op1_count);
uint32_t ZEND_FASTCALL qb_get_array_sprintf_length_U32(qb_interpreter_context *cxt, uint32_t *op1_ptr, uint32_t op1_count);
uint32_t ZEND_FASTCALL qb_get_array_sprintf_length_U64(qb_interpreter_context *cxt, uint64_t *op1_ptr, uint32_t op1_count);
uint32_t ZEND_FASTCALL qb_get_scalar_sprintf_length_F32(qb_interpreter_context *cxt, float32_t op1);
uint32_t ZEND_FASTCALL qb_get_scalar_sprintf_length_F64(qb_interpreter_context *cxt, float64_t op1);
uint32_t ZEND_FASTCALL qb_get_scalar_sprintf_length_S08(qb_interpreter_context *cxt, int8_t op1);
uint32_t ZEND_FASTCALL qb_get_scalar_sprintf_length_S16(qb_interpreter_context *cxt, int16_t op1);
uint32_t ZEND_FASTCALL qb_get_scalar_sprintf_length_S32(qb_interpreter_context *cxt, int32_t op1);
uint32_t ZEND_FASTCALL qb_get_scalar_sprintf_length_S64(qb_interpreter_context *cxt, int64_t op1);
uint32_t ZEND_FASTCALL qb_get_scalar_sprintf_length_U08(qb_interpreter_context *cxt, uint8_t op1);
uint32_t ZEND_FASTCALL qb_get_scalar_sprintf_length_U16(qb_interpreter_context *cxt, uint16_t op1);
uint32_t ZEND_FASTCALL qb_get_scalar_sprintf_length_U32(qb_interpreter_context *cxt, uint32_t op1);
uint32_t ZEND_FASTCALL qb_get_scalar_sprintf_length_U64(qb_interpreter_context *cxt, uint64_t op1);
uint32_t ZEND_FASTCALL qb_get_utf8_codepoint_count(uint8_t *op1_ptr, uint32_t op1_count);
uint32_t ZEND_FASTCALL qb_get_utf8_encoded_length_U16(uint16_t *op1_ptr, uint32_t op1_count);
uint32_t ZEND_FASTCALL qb_get_utf8_encoded_length_U32(uint32_t *op1_ptr, uint32_t op1_count);
uint32_t qb_get_difference_count_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, uint32_t op3);
uint32_t qb_get_difference_count_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, uint32_t op3);
uint32_t qb_get_difference_count_I08(int8_t *op1_ptr, uint32_t op1_count, int8_t *op2_ptr, uint32_t op2_count, uint32_t op3);
uint32_t qb_get_difference_count_I16(int16_t *op1_ptr, uint32_t op1_count, int16_t *op2_ptr, uint32_t op2_count, uint32_t op3);
uint32_t qb_get_difference_count_I32(int32_t *op1_ptr, uint32_t op1_count, int32_t *op2_ptr, uint32_t op2_count, uint32_t op3);
uint32_t qb_get_difference_count_I64(int64_t *op1_ptr, uint32_t op1_count, int64_t *op2_ptr, uint32_t op2_count, uint32_t op3);
uint32_t qb_get_intersect_count_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, uint32_t op3);
uint32_t qb_get_intersect_count_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, uint32_t op3);
uint32_t qb_get_intersect_count_I08(int8_t *op1_ptr, uint32_t op1_count, int8_t *op2_ptr, uint32_t op2_count, uint32_t op3);
uint32_t qb_get_intersect_count_I16(int16_t *op1_ptr, uint32_t op1_count, int16_t *op2_ptr, uint32_t op2_count, uint32_t op3);
uint32_t qb_get_intersect_count_I32(int32_t *op1_ptr, uint32_t op1_count, int32_t *op2_ptr, uint32_t op2_count, uint32_t op3);
uint32_t qb_get_intersect_count_I64(int64_t *op1_ptr, uint32_t op1_count, int64_t *op2_ptr, uint32_t op2_count, uint32_t op3);
uint32_t qb_get_multidimensional_array_sprintf_length_F32(qb_interpreter_context *cxt, float32_t *op1_ptr, uint32_t op1_count, uint32_t *op2_ptr, uint32_t op2_count);
uint32_t qb_get_multidimensional_array_sprintf_length_F64(qb_interpreter_context *cxt, float64_t *op1_ptr, uint32_t op1_count, uint32_t *op2_ptr, uint32_t op2_count);
uint32_t qb_get_multidimensional_array_sprintf_length_S08(qb_interpreter_context *cxt, int8_t *op1_ptr, uint32_t op1_count, uint32_t *op2_ptr, uint32_t op2_count);
uint32_t qb_get_multidimensional_array_sprintf_length_S16(qb_interpreter_context *cxt, int16_t *op1_ptr, uint32_t op1_count, uint32_t *op2_ptr, uint32_t op2_count);
uint32_t qb_get_multidimensional_array_sprintf_length_S32(qb_interpreter_context *cxt, int32_t *op1_ptr, uint32_t op1_count, uint32_t *op2_ptr, uint32_t op2_count);
uint32_t qb_get_multidimensional_array_sprintf_length_S64(qb_interpreter_context *cxt, int64_t *op1_ptr, uint32_t op1_count, uint32_t *op2_ptr, uint32_t op2_count);
uint32_t qb_get_multidimensional_array_sprintf_length_U08(qb_interpreter_context *cxt, uint8_t *op1_ptr, uint32_t op1_count, uint32_t *op2_ptr, uint32_t op2_count);
uint32_t qb_get_multidimensional_array_sprintf_length_U16(qb_interpreter_context *cxt, uint16_t *op1_ptr, uint32_t op1_count, uint32_t *op2_ptr, uint32_t op2_count);
uint32_t qb_get_multidimensional_array_sprintf_length_U32(qb_interpreter_context *cxt, uint32_t *op1_ptr, uint32_t op1_count, uint32_t *op2_ptr, uint32_t op2_count);
uint32_t qb_get_multidimensional_array_sprintf_length_U64(qb_interpreter_context *cxt, uint64_t *op1_ptr, uint32_t op1_count, uint32_t *op2_ptr, uint32_t op2_count);
uint32_t qb_get_range_length_F32(float32_t op1, float32_t op2, float32_t op3);
uint32_t qb_get_range_length_F64(float64_t op1, float64_t op2, float64_t op3);
uint32_t qb_get_range_length_S08(int8_t op1, int8_t op2, int8_t op3);
uint32_t qb_get_range_length_S16(int16_t op1, int16_t op2, int16_t op3);
uint32_t qb_get_range_length_S32(int32_t op1, int32_t op2, int32_t op3);
uint32_t qb_get_range_length_S64(int64_t op1, int64_t op2, int64_t op3);
uint32_t qb_get_range_length_U08(uint8_t op1, uint8_t op2, int8_t op3);
uint32_t qb_get_range_length_U16(uint16_t op1, uint16_t op2, int16_t op3);
uint32_t qb_get_range_length_U32(uint32_t op1, uint32_t op2, int32_t op3);
uint32_t qb_get_range_length_U64(uint64_t op1, uint64_t op2, int64_t op3);
uint32_t qb_get_unique_element_count_F32(float32_t *op1_ptr, uint32_t op1_count, uint32_t op2);
uint32_t qb_get_unique_element_count_F64(float64_t *op1_ptr, uint32_t op1_count, uint32_t op2);
uint32_t qb_get_unique_element_count_I08(int8_t *op1_ptr, uint32_t op1_count, uint32_t op2);
uint32_t qb_get_unique_element_count_I16(int16_t *op1_ptr, uint32_t op1_count, uint32_t op2);
uint32_t qb_get_unique_element_count_I32(int32_t *op1_ptr, uint32_t op1_count, uint32_t op2);
uint32_t qb_get_unique_element_count_I64(int64_t *op1_ptr, uint32_t op1_count, uint32_t op2);
void ZEND_FASTCALL qb_convert_hsl_to_rgb_F32(float32_t *op1_ptr, float32_t *res_ptr);
void ZEND_FASTCALL qb_convert_hsl_to_rgb_F64(float64_t *op1_ptr, float64_t *res_ptr);
void ZEND_FASTCALL qb_convert_hsv_to_rgb_F32(float32_t *op1_ptr, float32_t *res_ptr);
void ZEND_FASTCALL qb_convert_hsv_to_rgb_F64(float64_t *op1_ptr, float64_t *res_ptr);
void ZEND_FASTCALL qb_convert_rgb_to_hsl_F32(float32_t *op1_ptr, float32_t *res_ptr);
void ZEND_FASTCALL qb_convert_rgb_to_hsl_F64(float64_t *op1_ptr, float64_t *res_ptr);
void ZEND_FASTCALL qb_convert_rgb_to_hsv_F32(float32_t *op1_ptr, float32_t *res_ptr);
void ZEND_FASTCALL qb_convert_rgb_to_hsv_F64(float64_t *op1_ptr, float64_t *res_ptr);
void ZEND_FASTCALL qb_relocate_elements_F32(float32_t *elements, uint32_t *old_dims, uint32_t *new_dims, uint32_t dimension_count);
void ZEND_FASTCALL qb_relocate_elements_F64(float64_t *elements, uint32_t *old_dims, uint32_t *new_dims, uint32_t dimension_count);
void ZEND_FASTCALL qb_relocate_elements_I08(int8_t *elements, uint32_t *old_dims, uint32_t *new_dims, uint32_t dimension_count);
void ZEND_FASTCALL qb_relocate_elements_I16(int16_t *elements, uint32_t *old_dims, uint32_t *new_dims, uint32_t dimension_count);
void ZEND_FASTCALL qb_relocate_elements_I32(int32_t *elements, uint32_t *old_dims, uint32_t *new_dims, uint32_t dimension_count);
void ZEND_FASTCALL qb_relocate_elements_I64(int64_t *elements, uint32_t *old_dims, uint32_t *new_dims, uint32_t dimension_count);
void ZEND_FASTCALL qb_do_abs_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_abs_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_abs_multiple_times_S08(int8_t *op1_ptr, uint32_t op1_count, int8_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_abs_multiple_times_S16(int16_t *op1_ptr, uint32_t op1_count, int16_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_abs_multiple_times_S32(int32_t *op1_ptr, uint32_t op1_count, int32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_abs_multiple_times_S64(int64_t *op1_ptr, uint32_t op1_count, int64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_acos_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_acos_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_acosh_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_acosh_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_add_2x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_add_2x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_add_3x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_add_3x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_add_4x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_add_4x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_add_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_add_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_add_multiple_times_I08(int8_t *op1_ptr, uint32_t op1_count, int8_t *op2_ptr, uint32_t op2_count, int8_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_add_multiple_times_I16(int16_t *op1_ptr, uint32_t op1_count, int16_t *op2_ptr, uint32_t op2_count, int16_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_add_multiple_times_I32(int32_t *op1_ptr, uint32_t op1_count, int32_t *op2_ptr, uint32_t op2_count, int32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_add_multiple_times_I64(int64_t *op1_ptr, uint32_t op1_count, int64_t *op2_ptr, uint32_t op2_count, int64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_all_I32(int32_t *op1_ptr, uint32_t op1_count, int32_t *res_ptr);
void ZEND_FASTCALL qb_do_alpha_blend_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_alpha_blend_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_any_I32(int32_t *op1_ptr, uint32_t op1_count, int32_t *res_ptr);
void ZEND_FASTCALL qb_do_apply_premultiplication_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_apply_premultiplication_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_array_column_F32(qb_interpreter_context *cxt, float32_t *op1_ptr, uint32_t op1_count, uint32_t op2, uint32_t op3, uint32_t op4, float32_t *res_ptr, uint32_t res_count, uint32_t PHP_LINE_NUMBER);
void ZEND_FASTCALL qb_do_array_column_F64(qb_interpreter_context *cxt, float64_t *op1_ptr, uint32_t op1_count, uint32_t op2, uint32_t op3, uint32_t op4, float64_t *res_ptr, uint32_t res_count, uint32_t PHP_LINE_NUMBER);
void ZEND_FASTCALL qb_do_array_column_I08(qb_interpreter_context *cxt, int8_t *op1_ptr, uint32_t op1_count, uint32_t op2, uint32_t op3, uint32_t op4, int8_t *res_ptr, uint32_t res_count, uint32_t PHP_LINE_NUMBER);
void ZEND_FASTCALL qb_do_array_column_I16(qb_interpreter_context *cxt, int16_t *op1_ptr, uint32_t op1_count, uint32_t op2, uint32_t op3, uint32_t op4, int16_t *res_ptr, uint32_t res_count, uint32_t PHP_LINE_NUMBER);
void ZEND_FASTCALL qb_do_array_column_I32(qb_interpreter_context *cxt, int32_t *op1_ptr, uint32_t op1_count, uint32_t op2, uint32_t op3, uint32_t op4, int32_t *res_ptr, uint32_t res_count, uint32_t PHP_LINE_NUMBER);
void ZEND_FASTCALL qb_do_array_column_I64(qb_interpreter_context *cxt, int64_t *op1_ptr, uint32_t op1_count, uint32_t op2, uint32_t op3, uint32_t op4, int64_t *res_ptr, uint32_t res_count, uint32_t PHP_LINE_NUMBER);
void ZEND_FASTCALL qb_do_array_difference_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, uint32_t op3, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_array_difference_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, uint32_t op3, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_array_difference_I08(int8_t *op1_ptr, uint32_t op1_count, int8_t *op2_ptr, uint32_t op2_count, uint32_t op3, int8_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_array_difference_I16(int16_t *op1_ptr, uint32_t op1_count, int16_t *op2_ptr, uint32_t op2_count, uint32_t op3, int16_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_array_difference_I32(int32_t *op1_ptr, uint32_t op1_count, int32_t *op2_ptr, uint32_t op2_count, uint32_t op3, int32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_array_difference_I64(int64_t *op1_ptr, uint32_t op1_count, int64_t *op2_ptr, uint32_t op2_count, uint32_t op3, int64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_array_intersect_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, uint32_t op3, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_array_intersect_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, uint32_t op3, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_array_intersect_I08(int8_t *op1_ptr, uint32_t op1_count, int8_t *op2_ptr, uint32_t op2_count, uint32_t op3, int8_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_array_intersect_I16(int16_t *op1_ptr, uint32_t op1_count, int16_t *op2_ptr, uint32_t op2_count, uint32_t op3, int16_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_array_intersect_I32(int32_t *op1_ptr, uint32_t op1_count, int32_t *op2_ptr, uint32_t op2_count, uint32_t op3, int32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_array_intersect_I64(int64_t *op1_ptr, uint32_t op1_count, int64_t *op2_ptr, uint32_t op2_count, uint32_t op3, int64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_array_max_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr);
void ZEND_FASTCALL qb_do_array_max_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr);
void ZEND_FASTCALL qb_do_array_max_S08(int8_t *op1_ptr, uint32_t op1_count, int8_t *res_ptr);
void ZEND_FASTCALL qb_do_array_max_S16(int16_t *op1_ptr, uint32_t op1_count, int16_t *res_ptr);
void ZEND_FASTCALL qb_do_array_max_S32(int32_t *op1_ptr, uint32_t op1_count, int32_t *res_ptr);
void ZEND_FASTCALL qb_do_array_max_S64(int64_t *op1_ptr, uint32_t op1_count, int64_t *res_ptr);
void ZEND_FASTCALL qb_do_array_max_U08(uint8_t *op1_ptr, uint32_t op1_count, uint8_t *res_ptr);
void ZEND_FASTCALL qb_do_array_max_U16(uint16_t *op1_ptr, uint32_t op1_count, uint16_t *res_ptr);
void ZEND_FASTCALL qb_do_array_max_U32(uint32_t *op1_ptr, uint32_t op1_count, uint32_t *res_ptr);
void ZEND_FASTCALL qb_do_array_max_U64(uint64_t *op1_ptr, uint32_t op1_count, uint64_t *res_ptr);
void ZEND_FASTCALL qb_do_array_min_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr);
void ZEND_FASTCALL qb_do_array_min_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr);
void ZEND_FASTCALL qb_do_array_min_S08(int8_t *op1_ptr, uint32_t op1_count, int8_t *res_ptr);
void ZEND_FASTCALL qb_do_array_min_S16(int16_t *op1_ptr, uint32_t op1_count, int16_t *res_ptr);
void ZEND_FASTCALL qb_do_array_min_S32(int32_t *op1_ptr, uint32_t op1_count, int32_t *res_ptr);
void ZEND_FASTCALL qb_do_array_min_S64(int64_t *op1_ptr, uint32_t op1_count, int64_t *res_ptr);
void ZEND_FASTCALL qb_do_array_min_U08(uint8_t *op1_ptr, uint32_t op1_count, uint8_t *res_ptr);
void ZEND_FASTCALL qb_do_array_min_U16(uint16_t *op1_ptr, uint32_t op1_count, uint16_t *res_ptr);
void ZEND_FASTCALL qb_do_array_min_U32(uint32_t *op1_ptr, uint32_t op1_count, uint32_t *res_ptr);
void ZEND_FASTCALL qb_do_array_min_U64(uint64_t *op1_ptr, uint32_t op1_count, uint64_t *res_ptr);
void ZEND_FASTCALL qb_do_array_pad_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, int32_t op3, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_array_pad_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, int32_t op3, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_array_pad_I08(int8_t *op1_ptr, uint32_t op1_count, int8_t *op2_ptr, uint32_t op2_count, int32_t op3, int8_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_array_pad_I16(int16_t *op1_ptr, uint32_t op1_count, int16_t *op2_ptr, uint32_t op2_count, int32_t op3, int16_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_array_pad_I32(int32_t *op1_ptr, uint32_t op1_count, int32_t *op2_ptr, uint32_t op2_count, int32_t op3, int32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_array_pad_I64(int64_t *op1_ptr, uint32_t op1_count, int64_t *op2_ptr, uint32_t op2_count, int32_t op3, int64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_array_product_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr);
void ZEND_FASTCALL qb_do_array_product_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr);
void ZEND_FASTCALL qb_do_array_product_S08(int8_t *op1_ptr, uint32_t op1_count, int8_t *res_ptr);
void ZEND_FASTCALL qb_do_array_product_S16(int16_t *op1_ptr, uint32_t op1_count, int16_t *res_ptr);
void ZEND_FASTCALL qb_do_array_product_S32(int32_t *op1_ptr, uint32_t op1_count, int32_t *res_ptr);
void ZEND_FASTCALL qb_do_array_product_S64(int64_t *op1_ptr, uint32_t op1_count, int64_t *res_ptr);
void ZEND_FASTCALL qb_do_array_product_U08(uint8_t *op1_ptr, uint32_t op1_count, uint8_t *res_ptr);
void ZEND_FASTCALL qb_do_array_product_U16(uint16_t *op1_ptr, uint32_t op1_count, uint16_t *res_ptr);
void ZEND_FASTCALL qb_do_array_product_U32(uint32_t *op1_ptr, uint32_t op1_count, uint32_t *res_ptr);
void ZEND_FASTCALL qb_do_array_product_U64(uint64_t *op1_ptr, uint32_t op1_count, uint64_t *res_ptr);
void ZEND_FASTCALL qb_do_array_random(qb_interpreter_context *cxt, uint32_t op1, uint32_t op2, uint32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_array_reverse_F32(float32_t *op1_ptr, uint32_t op1_count, uint32_t op2, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_array_reverse_F64(float64_t *op1_ptr, uint32_t op1_count, uint32_t op2, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_array_reverse_I08(int8_t *op1_ptr, uint32_t op1_count, uint32_t op2, int8_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_array_reverse_I16(int16_t *op1_ptr, uint32_t op1_count, uint32_t op2, int16_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_array_reverse_I32(int32_t *op1_ptr, uint32_t op1_count, uint32_t op2, int32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_array_reverse_I64(int64_t *op1_ptr, uint32_t op1_count, uint32_t op2, int64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_array_search_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t op2, int32_t *res_ptr);
void ZEND_FASTCALL qb_do_array_search_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t op2, int32_t *res_ptr);
void ZEND_FASTCALL qb_do_array_search_I08(int8_t *op1_ptr, uint32_t op1_count, int8_t op2, int32_t *res_ptr);
void ZEND_FASTCALL qb_do_array_search_I16(int16_t *op1_ptr, uint32_t op1_count, int16_t op2, int32_t *res_ptr);
void ZEND_FASTCALL qb_do_array_search_I32(int32_t *op1_ptr, uint32_t op1_count, int32_t op2, int32_t *res_ptr);
void ZEND_FASTCALL qb_do_array_search_I64(int64_t *op1_ptr, uint32_t op1_count, int64_t op2, int32_t *res_ptr);
void ZEND_FASTCALL qb_do_array_search_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, int32_t *res_ptr);
void ZEND_FASTCALL qb_do_array_search_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, int32_t *res_ptr);
void ZEND_FASTCALL qb_do_array_search_multiple_times_I08(int8_t *op1_ptr, uint32_t op1_count, int8_t *op2_ptr, uint32_t op2_count, int32_t *res_ptr);
void ZEND_FASTCALL qb_do_array_search_multiple_times_I16(int16_t *op1_ptr, uint32_t op1_count, int16_t *op2_ptr, uint32_t op2_count, int32_t *res_ptr);
void ZEND_FASTCALL qb_do_array_search_multiple_times_I32(int32_t *op1_ptr, uint32_t op1_count, int32_t *op2_ptr, uint32_t op2_count, int32_t *res_ptr);
void ZEND_FASTCALL qb_do_array_search_multiple_times_I64(int64_t *op1_ptr, uint32_t op1_count, int64_t *op2_ptr, uint32_t op2_count, int32_t *res_ptr);
void ZEND_FASTCALL qb_do_array_sum_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr);
void ZEND_FASTCALL qb_do_array_sum_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr);
void ZEND_FASTCALL qb_do_array_sum_S08(int8_t *op1_ptr, uint32_t op1_count, int8_t *res_ptr);
void ZEND_FASTCALL qb_do_array_sum_S16(int16_t *op1_ptr, uint32_t op1_count, int16_t *res_ptr);
void ZEND_FASTCALL qb_do_array_sum_S32(int32_t *op1_ptr, uint32_t op1_count, int32_t *res_ptr);
void ZEND_FASTCALL qb_do_array_sum_S64(int64_t *op1_ptr, uint32_t op1_count, int64_t *res_ptr);
void ZEND_FASTCALL qb_do_array_sum_U08(uint8_t *op1_ptr, uint32_t op1_count, uint8_t *res_ptr);
void ZEND_FASTCALL qb_do_array_sum_U16(uint16_t *op1_ptr, uint32_t op1_count, uint16_t *res_ptr);
void ZEND_FASTCALL qb_do_array_sum_U32(uint32_t *op1_ptr, uint32_t op1_count, uint32_t *res_ptr);
void ZEND_FASTCALL qb_do_array_sum_U64(uint64_t *op1_ptr, uint32_t op1_count, uint64_t *res_ptr);
void ZEND_FASTCALL qb_do_array_unique_F32(float32_t *op1_ptr, uint32_t op1_count, uint32_t op2, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_array_unique_F64(float64_t *op1_ptr, uint32_t op1_count, uint32_t op2, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_array_unique_I08(int8_t *op1_ptr, uint32_t op1_count, uint32_t op2, int8_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_array_unique_I16(int16_t *op1_ptr, uint32_t op1_count, uint32_t op2, int16_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_array_unique_I32(int32_t *op1_ptr, uint32_t op1_count, uint32_t op2, int32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_array_unique_I64(int64_t *op1_ptr, uint32_t op1_count, uint32_t op2, int64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_asin_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_asin_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_asinh_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_asinh_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_atan2_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_atan2_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_atan_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_atan_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_atanh_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_atanh_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_bitwise_and_multiple_times_I08(int8_t *op1_ptr, uint32_t op1_count, int8_t *op2_ptr, uint32_t op2_count, int8_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_bitwise_and_multiple_times_I16(int16_t *op1_ptr, uint32_t op1_count, int16_t *op2_ptr, uint32_t op2_count, int16_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_bitwise_and_multiple_times_I32(int32_t *op1_ptr, uint32_t op1_count, int32_t *op2_ptr, uint32_t op2_count, int32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_bitwise_and_multiple_times_I64(int64_t *op1_ptr, uint32_t op1_count, int64_t *op2_ptr, uint32_t op2_count, int64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_bitwise_not_multiple_times_I08(int8_t *op1_ptr, uint32_t op1_count, int8_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_bitwise_not_multiple_times_I16(int16_t *op1_ptr, uint32_t op1_count, int16_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_bitwise_not_multiple_times_I32(int32_t *op1_ptr, uint32_t op1_count, int32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_bitwise_not_multiple_times_I64(int64_t *op1_ptr, uint32_t op1_count, int64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_bitwise_or_multiple_times_I08(int8_t *op1_ptr, uint32_t op1_count, int8_t *op2_ptr, uint32_t op2_count, int8_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_bitwise_or_multiple_times_I16(int16_t *op1_ptr, uint32_t op1_count, int16_t *op2_ptr, uint32_t op2_count, int16_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_bitwise_or_multiple_times_I32(int32_t *op1_ptr, uint32_t op1_count, int32_t *op2_ptr, uint32_t op2_count, int32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_bitwise_or_multiple_times_I64(int64_t *op1_ptr, uint32_t op1_count, int64_t *op2_ptr, uint32_t op2_count, int64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_bitwise_xor_multiple_times_I08(int8_t *op1_ptr, uint32_t op1_count, int8_t *op2_ptr, uint32_t op2_count, int8_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_bitwise_xor_multiple_times_I16(int16_t *op1_ptr, uint32_t op1_count, int16_t *op2_ptr, uint32_t op2_count, int16_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_bitwise_xor_multiple_times_I32(int32_t *op1_ptr, uint32_t op1_count, int32_t *op2_ptr, uint32_t op2_count, int32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_bitwise_xor_multiple_times_I64(int64_t *op1_ptr, uint32_t op1_count, int64_t *op2_ptr, uint32_t op2_count, int64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_boolean_cast_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, int32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_boolean_cast_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, int32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_boolean_cast_multiple_times_I08(int8_t *op1_ptr, uint32_t op1_count, int32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_boolean_cast_multiple_times_I16(int16_t *op1_ptr, uint32_t op1_count, int32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_boolean_cast_multiple_times_I32(int32_t *op1_ptr, uint32_t op1_count, int32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_boolean_cast_multiple_times_I64(int64_t *op1_ptr, uint32_t op1_count, int32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_cast_multiple_times_F32_F64(float32_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_cast_multiple_times_F32_S08(float32_t *op1_ptr, uint32_t op1_count, int8_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_cast_multiple_times_F32_S16(float32_t *op1_ptr, uint32_t op1_count, int16_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_cast_multiple_times_F32_S32(float32_t *op1_ptr, uint32_t op1_count, int32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_cast_multiple_times_F32_S64(float32_t *op1_ptr, uint32_t op1_count, int64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_cast_multiple_times_F32_U08(float32_t *op1_ptr, uint32_t op1_count, uint8_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_cast_multiple_times_F32_U16(float32_t *op1_ptr, uint32_t op1_count, uint16_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_cast_multiple_times_F32_U32(float32_t *op1_ptr, uint32_t op1_count, uint32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_cast_multiple_times_F32_U64(float32_t *op1_ptr, uint32_t op1_count, uint64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_cast_multiple_times_F64_F32(float64_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_cast_multiple_times_F64_S08(float64_t *op1_ptr, uint32_t op1_count, int8_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_cast_multiple_times_F64_S16(float64_t *op1_ptr, uint32_t op1_count, int16_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_cast_multiple_times_F64_S32(float64_t *op1_ptr, uint32_t op1_count, int32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_cast_multiple_times_F64_S64(float64_t *op1_ptr, uint32_t op1_count, int64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_cast_multiple_times_F64_U08(float64_t *op1_ptr, uint32_t op1_count, uint8_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_cast_multiple_times_F64_U16(float64_t *op1_ptr, uint32_t op1_count, uint16_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_cast_multiple_times_F64_U32(float64_t *op1_ptr, uint32_t op1_count, uint32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_cast_multiple_times_F64_U64(float64_t *op1_ptr, uint32_t op1_count, uint64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_cast_multiple_times_I16_I08(int16_t *op1_ptr, uint32_t op1_count, int8_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_cast_multiple_times_I32_I08(int32_t *op1_ptr, uint32_t op1_count, int8_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_cast_multiple_times_I32_I16(int32_t *op1_ptr, uint32_t op1_count, int16_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_cast_multiple_times_I64_I08(int64_t *op1_ptr, uint32_t op1_count, int8_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_cast_multiple_times_I64_I16(int64_t *op1_ptr, uint32_t op1_count, int16_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_cast_multiple_times_I64_I32(int64_t *op1_ptr, uint32_t op1_count, int32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_cast_multiple_times_S08_F32(int8_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_cast_multiple_times_S08_F64(int8_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_cast_multiple_times_S08_I16(int8_t *op1_ptr, uint32_t op1_count, int16_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_cast_multiple_times_S08_I32(int8_t *op1_ptr, uint32_t op1_count, int32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_cast_multiple_times_S08_I64(int8_t *op1_ptr, uint32_t op1_count, int64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_cast_multiple_times_S16_F32(int16_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_cast_multiple_times_S16_F64(int16_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_cast_multiple_times_S16_I32(int16_t *op1_ptr, uint32_t op1_count, int32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_cast_multiple_times_S16_I64(int16_t *op1_ptr, uint32_t op1_count, int64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_cast_multiple_times_S32_F32(int32_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_cast_multiple_times_S32_F64(int32_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_cast_multiple_times_S32_I64(int32_t *op1_ptr, uint32_t op1_count, int64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_cast_multiple_times_S64_F32(int64_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_cast_multiple_times_S64_F64(int64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_cast_multiple_times_U08_F32(uint8_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_cast_multiple_times_U08_F64(uint8_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_cast_multiple_times_U08_I16(uint8_t *op1_ptr, uint32_t op1_count, int16_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_cast_multiple_times_U08_I32(uint8_t *op1_ptr, uint32_t op1_count, int32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_cast_multiple_times_U08_I64(uint8_t *op1_ptr, uint32_t op1_count, int64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_cast_multiple_times_U16_F32(uint16_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_cast_multiple_times_U16_F64(uint16_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_cast_multiple_times_U16_I32(uint16_t *op1_ptr, uint32_t op1_count, int32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_cast_multiple_times_U16_I64(uint16_t *op1_ptr, uint32_t op1_count, int64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_cast_multiple_times_U32_F32(uint32_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_cast_multiple_times_U32_F64(uint32_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_cast_multiple_times_U32_I64(uint32_t *op1_ptr, uint32_t op1_count, int64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_cast_multiple_times_U64_F32(uint64_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_cast_multiple_times_U64_F64(uint64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_ceil_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_ceil_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_clamp_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, float32_t *op3_ptr, uint32_t op3_count, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_clamp_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, float64_t *op3_ptr, uint32_t op3_count, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_complex_abs_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_complex_abs_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_complex_abs_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_complex_argument_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_complex_argument_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_complex_argument_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_complex_cos_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_complex_cos_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_complex_cos_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_complex_cosh_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_complex_cosh_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_complex_cosh_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_complex_divide_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_complex_divide_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_complex_divide_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_complex_exp_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_complex_exp_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_complex_exp_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_complex_log_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_complex_log_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_complex_log_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_complex_multiply_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_complex_multiply_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_complex_multiply_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_complex_pow_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_complex_pow_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_complex_pow_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_complex_sin_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_complex_sin_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_complex_sin_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_complex_sinh_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_complex_sinh_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_complex_sinh_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_complex_square_root_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_complex_square_root_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_complex_square_root_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_complex_tan_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_complex_tan_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_complex_tan_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_complex_tanh_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_complex_tanh_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_complex_tanh_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_concat_multidimensional_variable_F32(qb_interpreter_context *cxt, float32_t *op1_ptr, uint32_t op1_count, uint32_t *op2_ptr, uint32_t op2_count, uint8_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_concat_multidimensional_variable_F64(qb_interpreter_context *cxt, float64_t *op1_ptr, uint32_t op1_count, uint32_t *op2_ptr, uint32_t op2_count, uint8_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_concat_multidimensional_variable_S08(qb_interpreter_context *cxt, int8_t *op1_ptr, uint32_t op1_count, uint32_t *op2_ptr, uint32_t op2_count, uint8_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_concat_multidimensional_variable_S16(qb_interpreter_context *cxt, int16_t *op1_ptr, uint32_t op1_count, uint32_t *op2_ptr, uint32_t op2_count, uint8_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_concat_multidimensional_variable_S32(qb_interpreter_context *cxt, int32_t *op1_ptr, uint32_t op1_count, uint32_t *op2_ptr, uint32_t op2_count, uint8_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_concat_multidimensional_variable_S64(qb_interpreter_context *cxt, int64_t *op1_ptr, uint32_t op1_count, uint32_t *op2_ptr, uint32_t op2_count, uint8_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_concat_multidimensional_variable_U08(qb_interpreter_context *cxt, uint8_t *op1_ptr, uint32_t op1_count, uint32_t *op2_ptr, uint32_t op2_count, uint8_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_concat_multidimensional_variable_U16(qb_interpreter_context *cxt, uint16_t *op1_ptr, uint32_t op1_count, uint32_t *op2_ptr, uint32_t op2_count, uint8_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_concat_multidimensional_variable_U32(qb_interpreter_context *cxt, uint32_t *op1_ptr, uint32_t op1_count, uint32_t *op2_ptr, uint32_t op2_count, uint8_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_concat_multidimensional_variable_U64(qb_interpreter_context *cxt, uint64_t *op1_ptr, uint32_t op1_count, uint32_t *op2_ptr, uint32_t op2_count, uint8_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_concat_variable_F32(qb_interpreter_context *cxt, float32_t op1, uint8_t *res_ptr, uint32_t res_count, uint32_t res_count_before);
void ZEND_FASTCALL qb_do_concat_variable_F64(qb_interpreter_context *cxt, float64_t op1, uint8_t *res_ptr, uint32_t res_count, uint32_t res_count_before);
void ZEND_FASTCALL qb_do_concat_variable_S08(qb_interpreter_context *cxt, int8_t op1, uint8_t *res_ptr, uint32_t res_count, uint32_t res_count_before);
void ZEND_FASTCALL qb_do_concat_variable_S16(qb_interpreter_context *cxt, int16_t op1, uint8_t *res_ptr, uint32_t res_count, uint32_t res_count_before);
void ZEND_FASTCALL qb_do_concat_variable_S32(qb_interpreter_context *cxt, int32_t op1, uint8_t *res_ptr, uint32_t res_count, uint32_t res_count_before);
void ZEND_FASTCALL qb_do_concat_variable_S64(qb_interpreter_context *cxt, int64_t op1, uint8_t *res_ptr, uint32_t res_count, uint32_t res_count_before);
void ZEND_FASTCALL qb_do_concat_variable_U08(qb_interpreter_context *cxt, uint8_t op1, uint8_t *res_ptr, uint32_t res_count, uint32_t res_count_before);
void ZEND_FASTCALL qb_do_concat_variable_U16(qb_interpreter_context *cxt, uint16_t op1, uint8_t *res_ptr, uint32_t res_count, uint32_t res_count_before);
void ZEND_FASTCALL qb_do_concat_variable_U32(qb_interpreter_context *cxt, uint32_t op1, uint8_t *res_ptr, uint32_t res_count, uint32_t res_count_before);
void ZEND_FASTCALL qb_do_concat_variable_U64(qb_interpreter_context *cxt, uint64_t op1, uint8_t *res_ptr, uint32_t res_count, uint32_t res_count_before);
void ZEND_FASTCALL qb_do_concat_variable_multiple_times_F32(qb_interpreter_context *cxt, float32_t *op1_ptr, uint32_t op1_count, uint8_t *res_ptr, uint32_t res_count, uint32_t res_count_before);
void ZEND_FASTCALL qb_do_concat_variable_multiple_times_F64(qb_interpreter_context *cxt, float64_t *op1_ptr, uint32_t op1_count, uint8_t *res_ptr, uint32_t res_count, uint32_t res_count_before);
void ZEND_FASTCALL qb_do_concat_variable_multiple_times_S08(qb_interpreter_context *cxt, int8_t *op1_ptr, uint32_t op1_count, uint8_t *res_ptr, uint32_t res_count, uint32_t res_count_before);
void ZEND_FASTCALL qb_do_concat_variable_multiple_times_S16(qb_interpreter_context *cxt, int16_t *op1_ptr, uint32_t op1_count, uint8_t *res_ptr, uint32_t res_count, uint32_t res_count_before);
void ZEND_FASTCALL qb_do_concat_variable_multiple_times_S32(qb_interpreter_context *cxt, int32_t *op1_ptr, uint32_t op1_count, uint8_t *res_ptr, uint32_t res_count, uint32_t res_count_before);
void ZEND_FASTCALL qb_do_concat_variable_multiple_times_S64(qb_interpreter_context *cxt, int64_t *op1_ptr, uint32_t op1_count, uint8_t *res_ptr, uint32_t res_count, uint32_t res_count_before);
void ZEND_FASTCALL qb_do_concat_variable_multiple_times_U08(qb_interpreter_context *cxt, uint8_t *op1_ptr, uint32_t op1_count, uint8_t *res_ptr, uint32_t res_count, uint32_t res_count_before);
void ZEND_FASTCALL qb_do_concat_variable_multiple_times_U16(qb_interpreter_context *cxt, uint16_t *op1_ptr, uint32_t op1_count, uint8_t *res_ptr, uint32_t res_count, uint32_t res_count_before);
void ZEND_FASTCALL qb_do_concat_variable_multiple_times_U32(qb_interpreter_context *cxt, uint32_t *op1_ptr, uint32_t op1_count, uint8_t *res_ptr, uint32_t res_count, uint32_t res_count_before);
void ZEND_FASTCALL qb_do_concat_variable_multiple_times_U64(qb_interpreter_context *cxt, uint64_t *op1_ptr, uint32_t op1_count, uint8_t *res_ptr, uint32_t res_count, uint32_t res_count_before);
void ZEND_FASTCALL qb_do_copy_2x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_copy_2x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_copy_3x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_copy_3x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_copy_4x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_copy_4x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_copy_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_copy_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_copy_multiple_times_I08(int8_t *op1_ptr, uint32_t op1_count, int8_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_copy_multiple_times_I16(int16_t *op1_ptr, uint32_t op1_count, int16_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_copy_multiple_times_I32(int32_t *op1_ptr, uint32_t op1_count, int32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_copy_multiple_times_I64(int64_t *op1_ptr, uint32_t op1_count, int64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_cos_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_cos_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_cosh_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_cosh_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_cross_product_2x_F64(float64_t *op1_ptr, float64_t *op2_ptr, float64_t *res_ptr);
void ZEND_FASTCALL qb_do_cross_product_2x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_cross_product_2x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_cross_product_3x_F64(float64_t *op1_ptr, float64_t *op2_ptr, float64_t *res_ptr);
void ZEND_FASTCALL qb_do_cross_product_3x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_cross_product_3x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_cross_product_4x_F32(float32_t *op1_ptr, float32_t *op2_ptr, float32_t *op3_ptr, float32_t *res_ptr);
void ZEND_FASTCALL qb_do_cross_product_4x_F64(float64_t *op1_ptr, float64_t *op2_ptr, float64_t *op3_ptr, float64_t *res_ptr);
void ZEND_FASTCALL qb_do_cross_product_4x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, float32_t *op3_ptr, uint32_t op3_count, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_cross_product_4x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, float64_t *op3_ptr, uint32_t op3_count, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_decrement_2x_multiple_times_F32(float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_decrement_2x_multiple_times_F64(float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_decrement_3x_multiple_times_F32(float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_decrement_3x_multiple_times_F64(float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_decrement_4x_multiple_times_F32(float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_decrement_4x_multiple_times_F64(float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_decrement_multiple_times_F32(float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_decrement_multiple_times_F64(float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_decrement_multiple_times_I08(int8_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_decrement_multiple_times_I16(int16_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_decrement_multiple_times_I32(int32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_decrement_multiple_times_I64(int64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_degree_to_radian_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_degree_to_radian_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_determinant_2x_F64(float64_t *op1_ptr, float64_t *res_ptr);
void ZEND_FASTCALL qb_do_determinant_2x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_determinant_2x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_determinant_3x_F64(float64_t *op1_ptr, float64_t *res_ptr);
void ZEND_FASTCALL qb_do_determinant_3x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_determinant_3x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_determinant_4x_F64(float64_t *op1_ptr, float64_t *res_ptr);
void ZEND_FASTCALL qb_do_determinant_4x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_determinant_4x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_determinant_F32(float32_t *op1_ptr, uint32_t MATRIX1_ROWS, uint32_t MATRIX1_COLS, float32_t *res_ptr);
void ZEND_FASTCALL qb_do_determinant_F64(float64_t *op1_ptr, uint32_t MATRIX1_ROWS, uint32_t MATRIX1_COLS, float64_t *res_ptr);
void ZEND_FASTCALL qb_do_determinant_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, uint32_t MATRIX1_ROWS, uint32_t MATRIX1_COLS, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_determinant_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, uint32_t MATRIX1_ROWS, uint32_t MATRIX1_COLS, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_distance_2x_F64(float64_t *op1_ptr, float64_t *op2_ptr, float64_t *res_ptr);
void ZEND_FASTCALL qb_do_distance_2x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_distance_2x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_distance_3x_F64(float64_t *op1_ptr, float64_t *op2_ptr, float64_t *res_ptr);
void ZEND_FASTCALL qb_do_distance_3x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_distance_3x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_distance_4x_F64(float64_t *op1_ptr, float64_t *op2_ptr, float64_t *res_ptr);
void ZEND_FASTCALL qb_do_distance_4x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_distance_4x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_distance_F32(float32_t *op1_ptr, float32_t *op2_ptr, uint32_t MATRIX1_ROWS, uint32_t MATRIX1_COLS, uint32_t MATRIX2_ROWS, uint32_t MATRIX2_COLS, float32_t *res_ptr);
void ZEND_FASTCALL qb_do_distance_F64(float64_t *op1_ptr, float64_t *op2_ptr, uint32_t MATRIX1_ROWS, uint32_t MATRIX1_COLS, uint32_t MATRIX2_ROWS, uint32_t MATRIX2_COLS, float64_t *res_ptr);
void ZEND_FASTCALL qb_do_distance_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, uint32_t MATRIX1_ROWS, uint32_t MATRIX1_COLS, uint32_t MATRIX2_ROWS, uint32_t MATRIX2_COLS, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_distance_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, uint32_t MATRIX1_ROWS, uint32_t MATRIX1_COLS, uint32_t MATRIX2_ROWS, uint32_t MATRIX2_COLS, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_divide_2x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_divide_2x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_divide_3x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_divide_3x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_divide_4x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_divide_4x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_divide_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_divide_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_divide_multiple_times_S08(qb_interpreter_context *cxt, int8_t *op1_ptr, uint32_t op1_count, int8_t *op2_ptr, uint32_t op2_count, int8_t *res_ptr, uint32_t res_count, uint32_t PHP_LINE_NUMBER);
void ZEND_FASTCALL qb_do_divide_multiple_times_S16(qb_interpreter_context *cxt, int16_t *op1_ptr, uint32_t op1_count, int16_t *op2_ptr, uint32_t op2_count, int16_t *res_ptr, uint32_t res_count, uint32_t PHP_LINE_NUMBER);
void ZEND_FASTCALL qb_do_divide_multiple_times_S32(qb_interpreter_context *cxt, int32_t *op1_ptr, uint32_t op1_count, int32_t *op2_ptr, uint32_t op2_count, int32_t *res_ptr, uint32_t res_count, uint32_t PHP_LINE_NUMBER);
void ZEND_FASTCALL qb_do_divide_multiple_times_S64(qb_interpreter_context *cxt, int64_t *op1_ptr, uint32_t op1_count, int64_t *op2_ptr, uint32_t op2_count, int64_t *res_ptr, uint32_t res_count, uint32_t PHP_LINE_NUMBER);
void ZEND_FASTCALL qb_do_divide_multiple_times_U08(qb_interpreter_context *cxt, uint8_t *op1_ptr, uint32_t op1_count, uint8_t *op2_ptr, uint32_t op2_count, uint8_t *res_ptr, uint32_t res_count, uint32_t PHP_LINE_NUMBER);
void ZEND_FASTCALL qb_do_divide_multiple_times_U16(qb_interpreter_context *cxt, uint16_t *op1_ptr, uint32_t op1_count, uint16_t *op2_ptr, uint32_t op2_count, uint16_t *res_ptr, uint32_t res_count, uint32_t PHP_LINE_NUMBER);
void ZEND_FASTCALL qb_do_divide_multiple_times_U32(qb_interpreter_context *cxt, uint32_t *op1_ptr, uint32_t op1_count, uint32_t *op2_ptr, uint32_t op2_count, uint32_t *res_ptr, uint32_t res_count, uint32_t PHP_LINE_NUMBER);
void ZEND_FASTCALL qb_do_divide_multiple_times_U64(qb_interpreter_context *cxt, uint64_t *op1_ptr, uint32_t op1_count, uint64_t *op2_ptr, uint32_t op2_count, uint64_t *res_ptr, uint32_t res_count, uint32_t PHP_LINE_NUMBER);
void ZEND_FASTCALL qb_do_dot_product_2x_F64(float64_t *op1_ptr, float64_t *op2_ptr, float64_t *res_ptr);
void ZEND_FASTCALL qb_do_dot_product_2x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_dot_product_2x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_dot_product_3x_F64(float64_t *op1_ptr, float64_t *op2_ptr, float64_t *res_ptr);
void ZEND_FASTCALL qb_do_dot_product_3x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_dot_product_3x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_dot_product_4x_F64(float64_t *op1_ptr, float64_t *op2_ptr, float64_t *res_ptr);
void ZEND_FASTCALL qb_do_dot_product_4x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_dot_product_4x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_dot_product_F32(float32_t *op1_ptr, float32_t *op2_ptr, uint32_t MATRIX1_ROWS, uint32_t MATRIX1_COLS, float32_t *res_ptr);
void ZEND_FASTCALL qb_do_dot_product_F64(float64_t *op1_ptr, float64_t *op2_ptr, uint32_t MATRIX1_ROWS, uint32_t MATRIX1_COLS, float64_t *res_ptr);
void ZEND_FASTCALL qb_do_dot_product_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, uint32_t MATRIX1_ROWS, uint32_t MATRIX1_COLS, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_dot_product_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, uint32_t MATRIX1_ROWS, uint32_t MATRIX1_COLS, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_equal_vector_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, int32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_equal_vector_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, int32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_equal_vector_multiple_times_I08(int8_t *op1_ptr, uint32_t op1_count, int8_t *op2_ptr, uint32_t op2_count, int32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_equal_vector_multiple_times_I16(int16_t *op1_ptr, uint32_t op1_count, int16_t *op2_ptr, uint32_t op2_count, int32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_equal_vector_multiple_times_I32(int32_t *op1_ptr, uint32_t op1_count, int32_t *op2_ptr, uint32_t op2_count, int32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_equal_vector_multiple_times_I64(int64_t *op1_ptr, uint32_t op1_count, int64_t *op2_ptr, uint32_t op2_count, int32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_exp2_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_exp2_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_exp_m1_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_exp_m1_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_exp_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_exp_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_face_forward_2x_F64(float64_t *op1_ptr, float64_t *op2_ptr, float64_t *res_ptr);
void ZEND_FASTCALL qb_do_face_forward_2x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_face_forward_2x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_face_forward_3x_F64(float64_t *op1_ptr, float64_t *op2_ptr, float64_t *res_ptr);
void ZEND_FASTCALL qb_do_face_forward_3x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_face_forward_3x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_face_forward_4x_F64(float64_t *op1_ptr, float64_t *op2_ptr, float64_t *res_ptr);
void ZEND_FASTCALL qb_do_face_forward_4x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_face_forward_4x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_face_forward_F32(float32_t *op1_ptr, float32_t *op2_ptr, uint32_t MATRIX1_ROWS, uint32_t MATRIX1_COLS, uint32_t MATRIX2_ROWS, uint32_t MATRIX2_COLS, float32_t *res_ptr);
void ZEND_FASTCALL qb_do_face_forward_F64(float64_t *op1_ptr, float64_t *op2_ptr, uint32_t MATRIX1_ROWS, uint32_t MATRIX1_COLS, uint32_t MATRIX2_ROWS, uint32_t MATRIX2_COLS, float64_t *res_ptr);
void ZEND_FASTCALL qb_do_face_forward_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, uint32_t MATRIX1_ROWS, uint32_t MATRIX1_COLS, uint32_t MATRIX2_ROWS, uint32_t MATRIX2_COLS, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_face_forward_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, uint32_t MATRIX1_ROWS, uint32_t MATRIX1_COLS, uint32_t MATRIX2_ROWS, uint32_t MATRIX2_COLS, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_floor_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_floor_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_floored_division_modulo_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_floored_division_modulo_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_fract_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_fract_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_hsl2rgb_3x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_hsl2rgb_3x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_hsl2rgb_4x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_hsl2rgb_4x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_hsv2rgb_3x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_hsv2rgb_3x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_hsv2rgb_4x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_hsv2rgb_4x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_hypot_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_hypot_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_in_array_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, int32_t *res_ptr);
void ZEND_FASTCALL qb_do_in_array_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, int32_t *res_ptr);
void ZEND_FASTCALL qb_do_in_array_multiple_times_I08(int8_t *op1_ptr, uint32_t op1_count, int8_t *op2_ptr, uint32_t op2_count, int32_t *res_ptr);
void ZEND_FASTCALL qb_do_in_array_multiple_times_I16(int16_t *op1_ptr, uint32_t op1_count, int16_t *op2_ptr, uint32_t op2_count, int32_t *res_ptr);
void ZEND_FASTCALL qb_do_in_array_multiple_times_I32(int32_t *op1_ptr, uint32_t op1_count, int32_t *op2_ptr, uint32_t op2_count, int32_t *res_ptr);
void ZEND_FASTCALL qb_do_in_array_multiple_times_I64(int64_t *op1_ptr, uint32_t op1_count, int64_t *op2_ptr, uint32_t op2_count, int32_t *res_ptr);
void ZEND_FASTCALL qb_do_increment_2x_multiple_times_F32(float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_increment_2x_multiple_times_F64(float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_increment_3x_multiple_times_F32(float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_increment_3x_multiple_times_F64(float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_increment_4x_multiple_times_F32(float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_increment_4x_multiple_times_F64(float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_increment_multiple_times_F32(float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_increment_multiple_times_F64(float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_increment_multiple_times_I08(int8_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_increment_multiple_times_I16(int16_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_increment_multiple_times_I32(int32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_increment_multiple_times_I64(int64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_invert_matrix_2x_F32(float32_t *op1_ptr, float32_t *res_ptr);
void ZEND_FASTCALL qb_do_invert_matrix_2x_F64(float64_t *op1_ptr, float64_t *res_ptr);
void ZEND_FASTCALL qb_do_invert_matrix_2x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_invert_matrix_2x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_invert_matrix_3x_F32(float32_t *op1_ptr, float32_t *res_ptr);
void ZEND_FASTCALL qb_do_invert_matrix_3x_F64(float64_t *op1_ptr, float64_t *res_ptr);
void ZEND_FASTCALL qb_do_invert_matrix_3x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_invert_matrix_3x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_invert_matrix_4x_F32(float32_t *op1_ptr, float32_t *res_ptr);
void ZEND_FASTCALL qb_do_invert_matrix_4x_F64(float64_t *op1_ptr, float64_t *res_ptr);
void ZEND_FASTCALL qb_do_invert_matrix_4x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_invert_matrix_4x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_invert_matrix_F32(float32_t *op1_ptr, uint32_t MATRIX1_ROWS, uint32_t MATRIX1_COLS, float32_t *res_ptr);
void ZEND_FASTCALL qb_do_invert_matrix_F64(float64_t *op1_ptr, uint32_t MATRIX1_ROWS, uint32_t MATRIX1_COLS, float64_t *res_ptr);
void ZEND_FASTCALL qb_do_invert_matrix_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, uint32_t MATRIX1_ROWS, uint32_t MATRIX1_COLS, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_invert_matrix_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, uint32_t MATRIX1_ROWS, uint32_t MATRIX1_COLS, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_is_finite_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, int32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_is_finite_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, int32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_is_infinite_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, int32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_is_infinite_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, int32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_is_nan_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, int32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_is_nan_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, int32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_lcg_F32(qb_interpreter_context *cxt, float32_t *res_ptr);
void ZEND_FASTCALL qb_do_lcg_F64(qb_interpreter_context *cxt, float64_t *res_ptr);
void ZEND_FASTCALL qb_do_lcg_multiple_times_F32(qb_interpreter_context *cxt, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_lcg_multiple_times_F64(qb_interpreter_context *cxt, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_length_2x_F64(float64_t *op1_ptr, float64_t *res_ptr);
void ZEND_FASTCALL qb_do_length_2x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_length_2x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_length_3x_F64(float64_t *op1_ptr, float64_t *res_ptr);
void ZEND_FASTCALL qb_do_length_3x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_length_3x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_length_4x_F64(float64_t *op1_ptr, float64_t *res_ptr);
void ZEND_FASTCALL qb_do_length_4x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_length_4x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_length_F32(float32_t *op1_ptr, uint32_t MATRIX1_ROWS, uint32_t MATRIX1_COLS, float32_t *res_ptr);
void ZEND_FASTCALL qb_do_length_F64(float64_t *op1_ptr, uint32_t MATRIX1_ROWS, uint32_t MATRIX1_COLS, float64_t *res_ptr);
void ZEND_FASTCALL qb_do_length_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, uint32_t MATRIX1_ROWS, uint32_t MATRIX1_COLS, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_length_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, uint32_t MATRIX1_ROWS, uint32_t MATRIX1_COLS, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_less_than_or_equal_vector_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, int32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_less_than_or_equal_vector_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, int32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_less_than_or_equal_vector_multiple_times_S08(int8_t *op1_ptr, uint32_t op1_count, int8_t *op2_ptr, uint32_t op2_count, int32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_less_than_or_equal_vector_multiple_times_S16(int16_t *op1_ptr, uint32_t op1_count, int16_t *op2_ptr, uint32_t op2_count, int32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_less_than_or_equal_vector_multiple_times_S32(int32_t *op1_ptr, uint32_t op1_count, int32_t *op2_ptr, uint32_t op2_count, int32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_less_than_or_equal_vector_multiple_times_S64(int64_t *op1_ptr, uint32_t op1_count, int64_t *op2_ptr, uint32_t op2_count, int32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_less_than_or_equal_vector_multiple_times_U08(uint8_t *op1_ptr, uint32_t op1_count, uint8_t *op2_ptr, uint32_t op2_count, int32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_less_than_or_equal_vector_multiple_times_U16(uint16_t *op1_ptr, uint32_t op1_count, uint16_t *op2_ptr, uint32_t op2_count, int32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_less_than_or_equal_vector_multiple_times_U32(uint32_t *op1_ptr, uint32_t op1_count, uint32_t *op2_ptr, uint32_t op2_count, int32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_less_than_or_equal_vector_multiple_times_U64(uint64_t *op1_ptr, uint32_t op1_count, uint64_t *op2_ptr, uint32_t op2_count, int32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_less_than_vector_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, int32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_less_than_vector_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, int32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_less_than_vector_multiple_times_S08(int8_t *op1_ptr, uint32_t op1_count, int8_t *op2_ptr, uint32_t op2_count, int32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_less_than_vector_multiple_times_S16(int16_t *op1_ptr, uint32_t op1_count, int16_t *op2_ptr, uint32_t op2_count, int32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_less_than_vector_multiple_times_S32(int32_t *op1_ptr, uint32_t op1_count, int32_t *op2_ptr, uint32_t op2_count, int32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_less_than_vector_multiple_times_S64(int64_t *op1_ptr, uint32_t op1_count, int64_t *op2_ptr, uint32_t op2_count, int32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_less_than_vector_multiple_times_U08(uint8_t *op1_ptr, uint32_t op1_count, uint8_t *op2_ptr, uint32_t op2_count, int32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_less_than_vector_multiple_times_U16(uint16_t *op1_ptr, uint32_t op1_count, uint16_t *op2_ptr, uint32_t op2_count, int32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_less_than_vector_multiple_times_U32(uint32_t *op1_ptr, uint32_t op1_count, uint32_t *op2_ptr, uint32_t op2_count, int32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_less_than_vector_multiple_times_U64(uint64_t *op1_ptr, uint32_t op1_count, uint64_t *op2_ptr, uint32_t op2_count, int32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_log10_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_log10_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_log1p_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_log1p_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_log2_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_log2_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_log_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_log_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_max_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_max_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_max_multiple_times_S08(int8_t *op1_ptr, uint32_t op1_count, int8_t *op2_ptr, uint32_t op2_count, int8_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_max_multiple_times_S16(int16_t *op1_ptr, uint32_t op1_count, int16_t *op2_ptr, uint32_t op2_count, int16_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_max_multiple_times_S32(int32_t *op1_ptr, uint32_t op1_count, int32_t *op2_ptr, uint32_t op2_count, int32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_max_multiple_times_S64(int64_t *op1_ptr, uint32_t op1_count, int64_t *op2_ptr, uint32_t op2_count, int64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_max_multiple_times_U08(uint8_t *op1_ptr, uint32_t op1_count, uint8_t *op2_ptr, uint32_t op2_count, uint8_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_max_multiple_times_U16(uint16_t *op1_ptr, uint32_t op1_count, uint16_t *op2_ptr, uint32_t op2_count, uint16_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_max_multiple_times_U32(uint32_t *op1_ptr, uint32_t op1_count, uint32_t *op2_ptr, uint32_t op2_count, uint32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_max_multiple_times_U64(uint64_t *op1_ptr, uint32_t op1_count, uint64_t *op2_ptr, uint32_t op2_count, uint64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_min_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_min_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_min_multiple_times_S08(int8_t *op1_ptr, uint32_t op1_count, int8_t *op2_ptr, uint32_t op2_count, int8_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_min_multiple_times_S16(int16_t *op1_ptr, uint32_t op1_count, int16_t *op2_ptr, uint32_t op2_count, int16_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_min_multiple_times_S32(int32_t *op1_ptr, uint32_t op1_count, int32_t *op2_ptr, uint32_t op2_count, int32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_min_multiple_times_S64(int64_t *op1_ptr, uint32_t op1_count, int64_t *op2_ptr, uint32_t op2_count, int64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_min_multiple_times_U08(uint8_t *op1_ptr, uint32_t op1_count, uint8_t *op2_ptr, uint32_t op2_count, uint8_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_min_multiple_times_U16(uint16_t *op1_ptr, uint32_t op1_count, uint16_t *op2_ptr, uint32_t op2_count, uint16_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_min_multiple_times_U32(uint32_t *op1_ptr, uint32_t op1_count, uint32_t *op2_ptr, uint32_t op2_count, uint32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_min_multiple_times_U64(uint64_t *op1_ptr, uint32_t op1_count, uint64_t *op2_ptr, uint32_t op2_count, uint64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_mix_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, float32_t *op3_ptr, uint32_t op3_count, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_mix_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, float64_t *op3_ptr, uint32_t op3_count, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_modulo_2x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_modulo_2x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_modulo_3x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_modulo_3x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_modulo_4x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_modulo_4x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_modulo_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_modulo_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_modulo_multiple_times_S08(int8_t *op1_ptr, uint32_t op1_count, int8_t *op2_ptr, uint32_t op2_count, int8_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_modulo_multiple_times_S16(int16_t *op1_ptr, uint32_t op1_count, int16_t *op2_ptr, uint32_t op2_count, int16_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_modulo_multiple_times_S32(int32_t *op1_ptr, uint32_t op1_count, int32_t *op2_ptr, uint32_t op2_count, int32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_modulo_multiple_times_S64(int64_t *op1_ptr, uint32_t op1_count, int64_t *op2_ptr, uint32_t op2_count, int64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_modulo_multiple_times_U08(uint8_t *op1_ptr, uint32_t op1_count, uint8_t *op2_ptr, uint32_t op2_count, uint8_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_modulo_multiple_times_U16(uint16_t *op1_ptr, uint32_t op1_count, uint16_t *op2_ptr, uint32_t op2_count, uint16_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_modulo_multiple_times_U32(uint32_t *op1_ptr, uint32_t op1_count, uint32_t *op2_ptr, uint32_t op2_count, uint32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_modulo_multiple_times_U64(uint64_t *op1_ptr, uint32_t op1_count, uint64_t *op2_ptr, uint32_t op2_count, uint64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_multiply_2x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_multiply_2x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_multiply_3x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_multiply_3x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_multiply_4x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_multiply_4x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_multiply_accumulate_2x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, float32_t *op3_ptr, uint32_t op3_count, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_multiply_accumulate_2x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, float64_t *op3_ptr, uint32_t op3_count, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_multiply_accumulate_3x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, float32_t *op3_ptr, uint32_t op3_count, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_multiply_accumulate_3x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, float64_t *op3_ptr, uint32_t op3_count, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_multiply_accumulate_4x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, float32_t *op3_ptr, uint32_t op3_count, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_multiply_accumulate_4x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, float64_t *op3_ptr, uint32_t op3_count, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_multiply_accumulate_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, float32_t *op3_ptr, uint32_t op3_count, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_multiply_accumulate_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, float64_t *op3_ptr, uint32_t op3_count, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_multiply_accumulate_multiple_times_S32(int32_t *op1_ptr, uint32_t op1_count, int32_t *op2_ptr, uint32_t op2_count, int32_t *op3_ptr, uint32_t op3_count, int32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_multiply_accumulate_multiple_times_S64(int64_t *op1_ptr, uint32_t op1_count, int64_t *op2_ptr, uint32_t op2_count, int64_t *op3_ptr, uint32_t op3_count, int64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_multiply_accumulate_multiple_times_U32(uint32_t *op1_ptr, uint32_t op1_count, uint32_t *op2_ptr, uint32_t op2_count, uint32_t *op3_ptr, uint32_t op3_count, uint32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_multiply_accumulate_multiple_times_U64(uint64_t *op1_ptr, uint32_t op1_count, uint64_t *op2_ptr, uint32_t op2_count, uint64_t *op3_ptr, uint32_t op3_count, uint64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_multiply_matrix_by_matrix_cm_2x_F64(float64_t *op1_ptr, float64_t *op2_ptr, float64_t *res_ptr);
void ZEND_FASTCALL qb_do_multiply_matrix_by_matrix_cm_2x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_multiply_matrix_by_matrix_cm_2x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_multiply_matrix_by_matrix_cm_3x_F32(float32_t *op1_ptr, float32_t *op2_ptr, float32_t *res_ptr);
void ZEND_FASTCALL qb_do_multiply_matrix_by_matrix_cm_3x_F64(float64_t *op1_ptr, float64_t *op2_ptr, float64_t *res_ptr);
void ZEND_FASTCALL qb_do_multiply_matrix_by_matrix_cm_3x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_multiply_matrix_by_matrix_cm_3x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_multiply_matrix_by_matrix_cm_3x_padded_F32(float32_t *op1_ptr, float32_t *op2_ptr, float32_t *res_ptr);
void ZEND_FASTCALL qb_do_multiply_matrix_by_matrix_cm_4x_F32(float32_t *op1_ptr, float32_t *op2_ptr, float32_t *res_ptr);
void ZEND_FASTCALL qb_do_multiply_matrix_by_matrix_cm_4x_F64(float64_t *op1_ptr, float64_t *op2_ptr, float64_t *res_ptr);
void ZEND_FASTCALL qb_do_multiply_matrix_by_matrix_cm_4x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_multiply_matrix_by_matrix_cm_4x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_multiply_matrix_by_matrix_cm_F32(float32_t *op1_ptr, float32_t *op2_ptr, uint32_t MATRIX1_ROWS, uint32_t MATRIX1_COLS, uint32_t MATRIX2_ROWS, uint32_t MATRIX2_COLS, float32_t *res_ptr);
void ZEND_FASTCALL qb_do_multiply_matrix_by_matrix_cm_F64(float64_t *op1_ptr, float64_t *op2_ptr, uint32_t MATRIX1_ROWS, uint32_t MATRIX1_COLS, uint32_t MATRIX2_ROWS, uint32_t MATRIX2_COLS, float64_t *res_ptr);
void ZEND_FASTCALL qb_do_multiply_matrix_by_matrix_cm_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, uint32_t MATRIX1_ROWS, uint32_t MATRIX1_COLS, uint32_t MATRIX2_ROWS, uint32_t MATRIX2_COLS, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_multiply_matrix_by_matrix_cm_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, uint32_t MATRIX1_ROWS, uint32_t MATRIX1_COLS, uint32_t MATRIX2_ROWS, uint32_t MATRIX2_COLS, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_multiply_matrix_by_vector_cm_2x_F64(float64_t *op1_ptr, float64_t *op2_ptr, float64_t *res_ptr);
void ZEND_FASTCALL qb_do_multiply_matrix_by_vector_cm_2x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_multiply_matrix_by_vector_cm_2x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_multiply_matrix_by_vector_cm_3x_F64(float64_t *op1_ptr, float64_t *op2_ptr, float64_t *res_ptr);
void ZEND_FASTCALL qb_do_multiply_matrix_by_vector_cm_3x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_multiply_matrix_by_vector_cm_3x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_multiply_matrix_by_vector_cm_4x_F64(float64_t *op1_ptr, float64_t *op2_ptr, float64_t *res_ptr);
void ZEND_FASTCALL qb_do_multiply_matrix_by_vector_cm_4x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_multiply_matrix_by_vector_cm_4x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_multiply_matrix_by_vector_cm_F32(float32_t *op1_ptr, float32_t *op2_ptr, uint32_t MATRIX1_ROWS, uint32_t MATRIX1_COLS, uint32_t MATRIX2_ROWS, uint32_t MATRIX2_COLS, float32_t *res_ptr);
void ZEND_FASTCALL qb_do_multiply_matrix_by_vector_cm_F64(float64_t *op1_ptr, float64_t *op2_ptr, uint32_t MATRIX1_ROWS, uint32_t MATRIX1_COLS, uint32_t MATRIX2_ROWS, uint32_t MATRIX2_COLS, float64_t *res_ptr);
void ZEND_FASTCALL qb_do_multiply_matrix_by_vector_cm_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, uint32_t MATRIX1_ROWS, uint32_t MATRIX1_COLS, uint32_t MATRIX2_ROWS, uint32_t MATRIX2_COLS, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_multiply_matrix_by_vector_cm_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, uint32_t MATRIX1_ROWS, uint32_t MATRIX1_COLS, uint32_t MATRIX2_ROWS, uint32_t MATRIX2_COLS, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_multiply_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_multiply_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_multiply_multiple_times_S08(int8_t *op1_ptr, uint32_t op1_count, int8_t *op2_ptr, uint32_t op2_count, int8_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_multiply_multiple_times_S16(int16_t *op1_ptr, uint32_t op1_count, int16_t *op2_ptr, uint32_t op2_count, int16_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_multiply_multiple_times_S32(int32_t *op1_ptr, uint32_t op1_count, int32_t *op2_ptr, uint32_t op2_count, int32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_multiply_multiple_times_S64(int64_t *op1_ptr, uint32_t op1_count, int64_t *op2_ptr, uint32_t op2_count, int64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_multiply_multiple_times_U08(uint8_t *op1_ptr, uint32_t op1_count, uint8_t *op2_ptr, uint32_t op2_count, uint8_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_multiply_multiple_times_U16(uint16_t *op1_ptr, uint32_t op1_count, uint16_t *op2_ptr, uint32_t op2_count, uint16_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_multiply_multiple_times_U32(uint32_t *op1_ptr, uint32_t op1_count, uint32_t *op2_ptr, uint32_t op2_count, uint32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_multiply_multiple_times_U64(uint64_t *op1_ptr, uint32_t op1_count, uint64_t *op2_ptr, uint32_t op2_count, uint64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_multiply_vector_by_matrix_cm_2x_F64(float64_t *op1_ptr, float64_t *op2_ptr, float64_t *res_ptr);
void ZEND_FASTCALL qb_do_multiply_vector_by_matrix_cm_2x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_multiply_vector_by_matrix_cm_2x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_multiply_vector_by_matrix_cm_3x_F64(float64_t *op1_ptr, float64_t *op2_ptr, float64_t *res_ptr);
void ZEND_FASTCALL qb_do_multiply_vector_by_matrix_cm_3x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_multiply_vector_by_matrix_cm_3x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_multiply_vector_by_matrix_cm_4x_F64(float64_t *op1_ptr, float64_t *op2_ptr, float64_t *res_ptr);
void ZEND_FASTCALL qb_do_multiply_vector_by_matrix_cm_4x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_multiply_vector_by_matrix_cm_4x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_multiply_vector_by_matrix_cm_F32(float32_t *op1_ptr, float32_t *op2_ptr, uint32_t MATRIX1_ROWS, uint32_t MATRIX1_COLS, uint32_t MATRIX2_ROWS, uint32_t MATRIX2_COLS, float32_t *res_ptr);
void ZEND_FASTCALL qb_do_multiply_vector_by_matrix_cm_F64(float64_t *op1_ptr, float64_t *op2_ptr, uint32_t MATRIX1_ROWS, uint32_t MATRIX1_COLS, uint32_t MATRIX2_ROWS, uint32_t MATRIX2_COLS, float64_t *res_ptr);
void ZEND_FASTCALL qb_do_multiply_vector_by_matrix_cm_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, uint32_t MATRIX1_ROWS, uint32_t MATRIX1_COLS, uint32_t MATRIX2_ROWS, uint32_t MATRIX2_COLS, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_multiply_vector_by_matrix_cm_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, uint32_t MATRIX1_ROWS, uint32_t MATRIX1_COLS, uint32_t MATRIX2_ROWS, uint32_t MATRIX2_COLS, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_negate_2x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_negate_2x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_negate_3x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_negate_3x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_negate_4x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_negate_4x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_negate_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_negate_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_negate_multiple_times_I08(int8_t *op1_ptr, uint32_t op1_count, int8_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_negate_multiple_times_I16(int16_t *op1_ptr, uint32_t op1_count, int16_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_negate_multiple_times_I32(int32_t *op1_ptr, uint32_t op1_count, int32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_negate_multiple_times_I64(int64_t *op1_ptr, uint32_t op1_count, int64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_normalize_2x_F64(float64_t *op1_ptr, float64_t *res_ptr);
void ZEND_FASTCALL qb_do_normalize_2x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_normalize_2x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_normalize_3x_F64(float64_t *op1_ptr, float64_t *res_ptr);
void ZEND_FASTCALL qb_do_normalize_3x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_normalize_3x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_normalize_4x_F64(float64_t *op1_ptr, float64_t *res_ptr);
void ZEND_FASTCALL qb_do_normalize_4x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_normalize_4x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_normalize_F32(float32_t *op1_ptr, uint32_t MATRIX1_ROWS, uint32_t MATRIX1_COLS, float32_t *res_ptr);
void ZEND_FASTCALL qb_do_normalize_F64(float64_t *op1_ptr, uint32_t MATRIX1_ROWS, uint32_t MATRIX1_COLS, float64_t *res_ptr);
void ZEND_FASTCALL qb_do_normalize_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, uint32_t MATRIX1_ROWS, uint32_t MATRIX1_COLS, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_normalize_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, uint32_t MATRIX1_ROWS, uint32_t MATRIX1_COLS, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_not_equal_vector_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, int32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_not_equal_vector_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, int32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_not_equal_vector_multiple_times_I08(int8_t *op1_ptr, uint32_t op1_count, int8_t *op2_ptr, uint32_t op2_count, int32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_not_equal_vector_multiple_times_I16(int16_t *op1_ptr, uint32_t op1_count, int16_t *op2_ptr, uint32_t op2_count, int32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_not_equal_vector_multiple_times_I32(int32_t *op1_ptr, uint32_t op1_count, int32_t *op2_ptr, uint32_t op2_count, int32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_not_equal_vector_multiple_times_I64(int64_t *op1_ptr, uint32_t op1_count, int64_t *op2_ptr, uint32_t op2_count, int32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_not_vector_multiple_times_I32(int32_t *op1_ptr, uint32_t op1_count, int32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_pow_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_pow_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_print_multidimensional_variable_F32(qb_interpreter_context *cxt, float32_t *op1_ptr, uint32_t op1_count, uint32_t *op2_ptr, uint32_t op2_count);
void ZEND_FASTCALL qb_do_print_multidimensional_variable_F64(qb_interpreter_context *cxt, float64_t *op1_ptr, uint32_t op1_count, uint32_t *op2_ptr, uint32_t op2_count);
void ZEND_FASTCALL qb_do_print_multidimensional_variable_S08(qb_interpreter_context *cxt, int8_t *op1_ptr, uint32_t op1_count, uint32_t *op2_ptr, uint32_t op2_count);
void ZEND_FASTCALL qb_do_print_multidimensional_variable_S16(qb_interpreter_context *cxt, int16_t *op1_ptr, uint32_t op1_count, uint32_t *op2_ptr, uint32_t op2_count);
void ZEND_FASTCALL qb_do_print_multidimensional_variable_S32(qb_interpreter_context *cxt, int32_t *op1_ptr, uint32_t op1_count, uint32_t *op2_ptr, uint32_t op2_count);
void ZEND_FASTCALL qb_do_print_multidimensional_variable_S64(qb_interpreter_context *cxt, int64_t *op1_ptr, uint32_t op1_count, uint32_t *op2_ptr, uint32_t op2_count);
void ZEND_FASTCALL qb_do_print_multidimensional_variable_U08(qb_interpreter_context *cxt, uint8_t *op1_ptr, uint32_t op1_count, uint32_t *op2_ptr, uint32_t op2_count);
void ZEND_FASTCALL qb_do_print_multidimensional_variable_U16(qb_interpreter_context *cxt, uint16_t *op1_ptr, uint32_t op1_count, uint32_t *op2_ptr, uint32_t op2_count);
void ZEND_FASTCALL qb_do_print_multidimensional_variable_U32(qb_interpreter_context *cxt, uint32_t *op1_ptr, uint32_t op1_count, uint32_t *op2_ptr, uint32_t op2_count);
void ZEND_FASTCALL qb_do_print_multidimensional_variable_U64(qb_interpreter_context *cxt, uint64_t *op1_ptr, uint32_t op1_count, uint32_t *op2_ptr, uint32_t op2_count);
void ZEND_FASTCALL qb_do_print_variable_F32(qb_interpreter_context *cxt, float32_t op1);
void ZEND_FASTCALL qb_do_print_variable_F64(qb_interpreter_context *cxt, float64_t op1);
void ZEND_FASTCALL qb_do_print_variable_S08(qb_interpreter_context *cxt, int8_t op1);
void ZEND_FASTCALL qb_do_print_variable_S16(qb_interpreter_context *cxt, int16_t op1);
void ZEND_FASTCALL qb_do_print_variable_S32(qb_interpreter_context *cxt, int32_t op1);
void ZEND_FASTCALL qb_do_print_variable_S64(qb_interpreter_context *cxt, int64_t op1);
void ZEND_FASTCALL qb_do_print_variable_U08(qb_interpreter_context *cxt, uint8_t op1);
void ZEND_FASTCALL qb_do_print_variable_U16(qb_interpreter_context *cxt, uint16_t op1);
void ZEND_FASTCALL qb_do_print_variable_U32(qb_interpreter_context *cxt, uint32_t op1);
void ZEND_FASTCALL qb_do_print_variable_U64(qb_interpreter_context *cxt, uint64_t op1);
void ZEND_FASTCALL qb_do_print_variable_multiple_times_F32(qb_interpreter_context *cxt, float32_t *op1_ptr, uint32_t op1_count);
void ZEND_FASTCALL qb_do_print_variable_multiple_times_F64(qb_interpreter_context *cxt, float64_t *op1_ptr, uint32_t op1_count);
void ZEND_FASTCALL qb_do_print_variable_multiple_times_S08(qb_interpreter_context *cxt, int8_t *op1_ptr, uint32_t op1_count);
void ZEND_FASTCALL qb_do_print_variable_multiple_times_S16(qb_interpreter_context *cxt, int16_t *op1_ptr, uint32_t op1_count);
void ZEND_FASTCALL qb_do_print_variable_multiple_times_S32(qb_interpreter_context *cxt, int32_t *op1_ptr, uint32_t op1_count);
void ZEND_FASTCALL qb_do_print_variable_multiple_times_S64(qb_interpreter_context *cxt, int64_t *op1_ptr, uint32_t op1_count);
void ZEND_FASTCALL qb_do_print_variable_multiple_times_U08(qb_interpreter_context *cxt, uint8_t *op1_ptr, uint32_t op1_count);
void ZEND_FASTCALL qb_do_print_variable_multiple_times_U16(qb_interpreter_context *cxt, uint16_t *op1_ptr, uint32_t op1_count);
void ZEND_FASTCALL qb_do_print_variable_multiple_times_U32(qb_interpreter_context *cxt, uint32_t *op1_ptr, uint32_t op1_count);
void ZEND_FASTCALL qb_do_print_variable_multiple_times_U64(qb_interpreter_context *cxt, uint64_t *op1_ptr, uint32_t op1_count);
void ZEND_FASTCALL qb_do_radian_to_degree_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_radian_to_degree_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_random_S08(qb_interpreter_context *cxt, int8_t op1, int8_t op2, int8_t *res_ptr);
void ZEND_FASTCALL qb_do_random_S16(qb_interpreter_context *cxt, int16_t op1, int16_t op2, int16_t *res_ptr);
void ZEND_FASTCALL qb_do_random_S32(qb_interpreter_context *cxt, int32_t op1, int32_t op2, int32_t *res_ptr);
void ZEND_FASTCALL qb_do_random_S64(qb_interpreter_context *cxt, int64_t op1, int64_t op2, int64_t *res_ptr);
void ZEND_FASTCALL qb_do_random_U08(qb_interpreter_context *cxt, uint8_t op1, uint8_t op2, uint8_t *res_ptr);
void ZEND_FASTCALL qb_do_random_U16(qb_interpreter_context *cxt, uint16_t op1, uint16_t op2, uint16_t *res_ptr);
void ZEND_FASTCALL qb_do_random_U32(qb_interpreter_context *cxt, uint32_t op1, uint32_t op2, uint32_t *res_ptr);
void ZEND_FASTCALL qb_do_random_U64(qb_interpreter_context *cxt, uint64_t op1, uint64_t op2, uint64_t *res_ptr);
void ZEND_FASTCALL qb_do_random_mt_S08(qb_interpreter_context *cxt, int8_t op1, int8_t op2, int8_t *res_ptr);
void ZEND_FASTCALL qb_do_random_mt_S16(qb_interpreter_context *cxt, int16_t op1, int16_t op2, int16_t *res_ptr);
void ZEND_FASTCALL qb_do_random_mt_S32(qb_interpreter_context *cxt, int32_t op1, int32_t op2, int32_t *res_ptr);
void ZEND_FASTCALL qb_do_random_mt_S64(qb_interpreter_context *cxt, int64_t op1, int64_t op2, int64_t *res_ptr);
void ZEND_FASTCALL qb_do_random_mt_U08(qb_interpreter_context *cxt, uint8_t op1, uint8_t op2, uint8_t *res_ptr);
void ZEND_FASTCALL qb_do_random_mt_U16(qb_interpreter_context *cxt, uint16_t op1, uint16_t op2, uint16_t *res_ptr);
void ZEND_FASTCALL qb_do_random_mt_U32(qb_interpreter_context *cxt, uint32_t op1, uint32_t op2, uint32_t *res_ptr);
void ZEND_FASTCALL qb_do_random_mt_U64(qb_interpreter_context *cxt, uint64_t op1, uint64_t op2, uint64_t *res_ptr);
void ZEND_FASTCALL qb_do_random_mt_multiple_times_S08(qb_interpreter_context *cxt, int8_t *op1_ptr, uint32_t op1_count, int8_t *op2_ptr, uint32_t op2_count, int8_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_random_mt_multiple_times_S16(qb_interpreter_context *cxt, int16_t *op1_ptr, uint32_t op1_count, int16_t *op2_ptr, uint32_t op2_count, int16_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_random_mt_multiple_times_S32(qb_interpreter_context *cxt, int32_t *op1_ptr, uint32_t op1_count, int32_t *op2_ptr, uint32_t op2_count, int32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_random_mt_multiple_times_S64(qb_interpreter_context *cxt, int64_t *op1_ptr, uint32_t op1_count, int64_t *op2_ptr, uint32_t op2_count, int64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_random_mt_multiple_times_U08(qb_interpreter_context *cxt, uint8_t *op1_ptr, uint32_t op1_count, uint8_t *op2_ptr, uint32_t op2_count, uint8_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_random_mt_multiple_times_U16(qb_interpreter_context *cxt, uint16_t *op1_ptr, uint32_t op1_count, uint16_t *op2_ptr, uint32_t op2_count, uint16_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_random_mt_multiple_times_U32(qb_interpreter_context *cxt, uint32_t *op1_ptr, uint32_t op1_count, uint32_t *op2_ptr, uint32_t op2_count, uint32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_random_mt_multiple_times_U64(qb_interpreter_context *cxt, uint64_t *op1_ptr, uint32_t op1_count, uint64_t *op2_ptr, uint32_t op2_count, uint64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_random_multiple_times_S08(qb_interpreter_context *cxt, int8_t *op1_ptr, uint32_t op1_count, int8_t *op2_ptr, uint32_t op2_count, int8_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_random_multiple_times_S16(qb_interpreter_context *cxt, int16_t *op1_ptr, uint32_t op1_count, int16_t *op2_ptr, uint32_t op2_count, int16_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_random_multiple_times_S32(qb_interpreter_context *cxt, int32_t *op1_ptr, uint32_t op1_count, int32_t *op2_ptr, uint32_t op2_count, int32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_random_multiple_times_S64(qb_interpreter_context *cxt, int64_t *op1_ptr, uint32_t op1_count, int64_t *op2_ptr, uint32_t op2_count, int64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_random_multiple_times_U08(qb_interpreter_context *cxt, uint8_t *op1_ptr, uint32_t op1_count, uint8_t *op2_ptr, uint32_t op2_count, uint8_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_random_multiple_times_U16(qb_interpreter_context *cxt, uint16_t *op1_ptr, uint32_t op1_count, uint16_t *op2_ptr, uint32_t op2_count, uint16_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_random_multiple_times_U32(qb_interpreter_context *cxt, uint32_t *op1_ptr, uint32_t op1_count, uint32_t *op2_ptr, uint32_t op2_count, uint32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_random_multiple_times_U64(qb_interpreter_context *cxt, uint64_t *op1_ptr, uint32_t op1_count, uint64_t *op2_ptr, uint32_t op2_count, uint64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_range_F32(float32_t op1, float32_t op2, float32_t op3, uint32_t string_length, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_range_F64(float64_t op1, float64_t op2, float64_t op3, uint32_t string_length, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_range_S08(int8_t op1, int8_t op2, int8_t op3, uint32_t string_length, int8_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_range_S16(int16_t op1, int16_t op2, int16_t op3, uint32_t string_length, int16_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_range_S32(int32_t op1, int32_t op2, int32_t op3, uint32_t string_length, int32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_range_S64(int64_t op1, int64_t op2, int64_t op3, uint32_t string_length, int64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_range_U08(uint8_t op1, uint8_t op2, int8_t op3, uint32_t string_length, uint8_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_range_U16(uint16_t op1, uint16_t op2, int16_t op3, uint32_t string_length, uint16_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_range_U32(uint32_t op1, uint32_t op2, int32_t op3, uint32_t string_length, uint32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_range_U64(uint64_t op1, uint64_t op2, int64_t op3, uint32_t string_length, uint64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_reflect_2x_F64(float64_t *op1_ptr, float64_t *op2_ptr, float64_t *res_ptr);
void ZEND_FASTCALL qb_do_reflect_2x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_reflect_2x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_reflect_3x_F64(float64_t *op1_ptr, float64_t *op2_ptr, float64_t *res_ptr);
void ZEND_FASTCALL qb_do_reflect_3x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_reflect_3x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_reflect_4x_F64(float64_t *op1_ptr, float64_t *op2_ptr, float64_t *res_ptr);
void ZEND_FASTCALL qb_do_reflect_4x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_reflect_4x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_reflect_F32(float32_t *op1_ptr, float32_t *op2_ptr, uint32_t MATRIX1_ROWS, uint32_t MATRIX1_COLS, float32_t *res_ptr);
void ZEND_FASTCALL qb_do_reflect_F64(float64_t *op1_ptr, float64_t *op2_ptr, uint32_t MATRIX1_ROWS, uint32_t MATRIX1_COLS, float64_t *res_ptr);
void ZEND_FASTCALL qb_do_reflect_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, uint32_t MATRIX1_ROWS, uint32_t MATRIX1_COLS, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_reflect_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, uint32_t MATRIX1_ROWS, uint32_t MATRIX1_COLS, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_refract_2x_F64(float64_t *op1_ptr, float64_t *op2_ptr, float64_t op3, float64_t *res_ptr);
void ZEND_FASTCALL qb_do_refract_2x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, float32_t op3, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_refract_2x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, float64_t op3, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_refract_3x_F64(float64_t *op1_ptr, float64_t *op2_ptr, float64_t op3, float64_t *res_ptr);
void ZEND_FASTCALL qb_do_refract_3x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, float32_t op3, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_refract_3x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, float64_t op3, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_refract_4x_F64(float64_t *op1_ptr, float64_t *op2_ptr, float64_t op3, float64_t *res_ptr);
void ZEND_FASTCALL qb_do_refract_4x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, float32_t op3, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_refract_4x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, float64_t op3, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_refract_F32(float32_t *op1_ptr, float32_t *op2_ptr, float32_t op3, uint32_t MATRIX1_ROWS, uint32_t MATRIX1_COLS, uint32_t MATRIX2_ROWS, uint32_t MATRIX2_COLS, float32_t *res_ptr);
void ZEND_FASTCALL qb_do_refract_F64(float64_t *op1_ptr, float64_t *op2_ptr, float64_t op3, uint32_t MATRIX1_ROWS, uint32_t MATRIX1_COLS, uint32_t MATRIX2_ROWS, uint32_t MATRIX2_COLS, float64_t *res_ptr);
void ZEND_FASTCALL qb_do_refract_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, float32_t op3, uint32_t MATRIX1_ROWS, uint32_t MATRIX1_COLS, uint32_t MATRIX2_ROWS, uint32_t MATRIX2_COLS, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_refract_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, float64_t op3, uint32_t MATRIX1_ROWS, uint32_t MATRIX1_COLS, uint32_t MATRIX2_ROWS, uint32_t MATRIX2_COLS, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_remove_premultiplication_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_remove_premultiplication_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_reverse_sort_F32(uint32_t op1, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_reverse_sort_F64(uint32_t op1, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_reverse_sort_S08(uint32_t op1, int8_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_reverse_sort_S16(uint32_t op1, int16_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_reverse_sort_S32(uint32_t op1, int32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_reverse_sort_S64(uint32_t op1, int64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_reverse_sort_U08(uint32_t op1, uint8_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_reverse_sort_U16(uint32_t op1, uint16_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_reverse_sort_U32(uint32_t op1, uint32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_reverse_sort_U64(uint32_t op1, uint64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_rgb2hsl_3x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_rgb2hsl_3x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_rgb2hsl_4x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_rgb2hsl_4x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_rgb2hsv_3x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_rgb2hsv_3x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_rgb2hsv_4x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_rgb2hsv_4x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_round_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, int32_t *op2_ptr, uint32_t op2_count, int32_t *op3_ptr, uint32_t op3_count, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_round_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, int32_t *op2_ptr, uint32_t op2_count, int32_t *op3_ptr, uint32_t op3_count, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_rsqrt_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_rsqrt_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_sample_bilinear_3x_F32(float32_t *op1_ptr, uint32_t op2, uint32_t op3, float32_t op4, float32_t op5, float32_t *res_ptr);
void ZEND_FASTCALL qb_do_sample_bilinear_3x_F64(float64_t *op1_ptr, uint32_t op2, uint32_t op3, float64_t op4, float64_t op5, float64_t *res_ptr);
void ZEND_FASTCALL qb_do_sample_bilinear_3x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, uint32_t op2, uint32_t op3, float32_t *op4_ptr, uint32_t op4_count, float32_t *op5_ptr, uint32_t op5_count, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_sample_bilinear_3x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, uint32_t op2, uint32_t op3, float64_t *op4_ptr, uint32_t op4_count, float64_t *op5_ptr, uint32_t op5_count, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_sample_bilinear_4x_F32(float32_t *op1_ptr, uint32_t op2, uint32_t op3, float32_t op4, float32_t op5, float32_t *res_ptr);
void ZEND_FASTCALL qb_do_sample_bilinear_4x_F64(float64_t *op1_ptr, uint32_t op2, uint32_t op3, float64_t op4, float64_t op5, float64_t *res_ptr);
void ZEND_FASTCALL qb_do_sample_bilinear_4x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, uint32_t op2, uint32_t op3, float32_t *op4_ptr, uint32_t op4_count, float32_t *op5_ptr, uint32_t op5_count, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_sample_bilinear_4x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, uint32_t op2, uint32_t op3, float64_t *op4_ptr, uint32_t op4_count, float64_t *op5_ptr, uint32_t op5_count, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_sample_nearest_3x_F64(float64_t *op1_ptr, uint32_t op2, uint32_t op3, float64_t op4, float64_t op5, float64_t *res_ptr);
void ZEND_FASTCALL qb_do_sample_nearest_3x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, uint32_t op2, uint32_t op3, float32_t *op4_ptr, uint32_t op4_count, float32_t *op5_ptr, uint32_t op5_count, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_sample_nearest_3x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, uint32_t op2, uint32_t op3, float64_t *op4_ptr, uint32_t op4_count, float64_t *op5_ptr, uint32_t op5_count, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_sample_nearest_4x_F64(float64_t *op1_ptr, uint32_t op2, uint32_t op3, float64_t op4, float64_t op5, float64_t *res_ptr);
void ZEND_FASTCALL qb_do_sample_nearest_4x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, uint32_t op2, uint32_t op3, float32_t *op4_ptr, uint32_t op4_count, float32_t *op5_ptr, uint32_t op5_count, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_sample_nearest_4x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, uint32_t op2, uint32_t op3, float64_t *op4_ptr, uint32_t op4_count, float64_t *op5_ptr, uint32_t op5_count, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_shift_left_multiple_times_S08(int8_t *op1_ptr, uint32_t op1_count, int8_t *op2_ptr, uint32_t op2_count, int8_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_shift_left_multiple_times_S16(int16_t *op1_ptr, uint32_t op1_count, int16_t *op2_ptr, uint32_t op2_count, int16_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_shift_left_multiple_times_S32(int32_t *op1_ptr, uint32_t op1_count, int32_t *op2_ptr, uint32_t op2_count, int32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_shift_left_multiple_times_S64(int64_t *op1_ptr, uint32_t op1_count, int64_t *op2_ptr, uint32_t op2_count, int64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_shift_left_multiple_times_U08(uint8_t *op1_ptr, uint32_t op1_count, uint8_t *op2_ptr, uint32_t op2_count, uint8_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_shift_left_multiple_times_U16(uint16_t *op1_ptr, uint32_t op1_count, uint16_t *op2_ptr, uint32_t op2_count, uint16_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_shift_left_multiple_times_U32(uint32_t *op1_ptr, uint32_t op1_count, uint32_t *op2_ptr, uint32_t op2_count, uint32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_shift_left_multiple_times_U64(uint64_t *op1_ptr, uint32_t op1_count, uint64_t *op2_ptr, uint32_t op2_count, uint64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_shift_right_multiple_times_S08(int8_t *op1_ptr, uint32_t op1_count, int8_t *op2_ptr, uint32_t op2_count, int8_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_shift_right_multiple_times_S16(int16_t *op1_ptr, uint32_t op1_count, int16_t *op2_ptr, uint32_t op2_count, int16_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_shift_right_multiple_times_S32(int32_t *op1_ptr, uint32_t op1_count, int32_t *op2_ptr, uint32_t op2_count, int32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_shift_right_multiple_times_S64(int64_t *op1_ptr, uint32_t op1_count, int64_t *op2_ptr, uint32_t op2_count, int64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_shift_right_multiple_times_U08(uint8_t *op1_ptr, uint32_t op1_count, uint8_t *op2_ptr, uint32_t op2_count, uint8_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_shift_right_multiple_times_U16(uint16_t *op1_ptr, uint32_t op1_count, uint16_t *op2_ptr, uint32_t op2_count, uint16_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_shift_right_multiple_times_U32(uint32_t *op1_ptr, uint32_t op1_count, uint32_t *op2_ptr, uint32_t op2_count, uint32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_shift_right_multiple_times_U64(uint64_t *op1_ptr, uint32_t op1_count, uint64_t *op2_ptr, uint32_t op2_count, uint64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_shuffle_F32(qb_interpreter_context *cxt, uint32_t op1, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_shuffle_F64(qb_interpreter_context *cxt, uint32_t op1, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_shuffle_I08(qb_interpreter_context *cxt, uint32_t op1, int8_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_shuffle_I16(qb_interpreter_context *cxt, uint32_t op1, int16_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_shuffle_I32(qb_interpreter_context *cxt, uint32_t op1, int32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_shuffle_I64(qb_interpreter_context *cxt, uint32_t op1, int64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_sign_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_sign_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_sin_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_sin_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_sinh_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_sinh_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_smooth_step_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, float32_t *op3_ptr, uint32_t op3_count, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_smooth_step_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, float64_t *op3_ptr, uint32_t op3_count, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_sort_F32(uint32_t op1, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_sort_F64(uint32_t op1, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_sort_S08(uint32_t op1, int8_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_sort_S16(uint32_t op1, int16_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_sort_S32(uint32_t op1, int32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_sort_S64(uint32_t op1, int64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_sort_U08(uint32_t op1, uint8_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_sort_U16(uint32_t op1, uint16_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_sort_U32(uint32_t op1, uint32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_sort_U64(uint32_t op1, uint64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_sqrt_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_sqrt_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_step_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_step_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_subarray_position_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, int32_t op3, int32_t *res_ptr);
void ZEND_FASTCALL qb_do_subarray_position_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, int32_t op3, int32_t *res_ptr);
void ZEND_FASTCALL qb_do_subarray_position_I08(int8_t *op1_ptr, uint32_t op1_count, int8_t *op2_ptr, uint32_t op2_count, int32_t op3, int32_t *res_ptr);
void ZEND_FASTCALL qb_do_subarray_position_I16(int16_t *op1_ptr, uint32_t op1_count, int16_t *op2_ptr, uint32_t op2_count, int32_t op3, int32_t *res_ptr);
void ZEND_FASTCALL qb_do_subarray_position_I32(int32_t *op1_ptr, uint32_t op1_count, int32_t *op2_ptr, uint32_t op2_count, int32_t op3, int32_t *res_ptr);
void ZEND_FASTCALL qb_do_subarray_position_I64(int64_t *op1_ptr, uint32_t op1_count, int64_t *op2_ptr, uint32_t op2_count, int32_t op3, int32_t *res_ptr);
void ZEND_FASTCALL qb_do_subarray_position_from_end_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, int32_t op3, int32_t *res_ptr);
void ZEND_FASTCALL qb_do_subarray_position_from_end_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, int32_t op3, int32_t *res_ptr);
void ZEND_FASTCALL qb_do_subarray_position_from_end_I08(int8_t *op1_ptr, uint32_t op1_count, int8_t *op2_ptr, uint32_t op2_count, int32_t op3, int32_t *res_ptr);
void ZEND_FASTCALL qb_do_subarray_position_from_end_I16(int16_t *op1_ptr, uint32_t op1_count, int16_t *op2_ptr, uint32_t op2_count, int32_t op3, int32_t *res_ptr);
void ZEND_FASTCALL qb_do_subarray_position_from_end_I32(int32_t *op1_ptr, uint32_t op1_count, int32_t *op2_ptr, uint32_t op2_count, int32_t op3, int32_t *res_ptr);
void ZEND_FASTCALL qb_do_subarray_position_from_end_I64(int64_t *op1_ptr, uint32_t op1_count, int64_t *op2_ptr, uint32_t op2_count, int32_t op3, int32_t *res_ptr);
void ZEND_FASTCALL qb_do_subtract_2x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_subtract_2x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_subtract_3x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_subtract_3x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_subtract_4x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_subtract_4x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_subtract_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_subtract_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_subtract_multiple_times_I08(int8_t *op1_ptr, uint32_t op1_count, int8_t *op2_ptr, uint32_t op2_count, int8_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_subtract_multiple_times_I16(int16_t *op1_ptr, uint32_t op1_count, int16_t *op2_ptr, uint32_t op2_count, int16_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_subtract_multiple_times_I32(int32_t *op1_ptr, uint32_t op1_count, int32_t *op2_ptr, uint32_t op2_count, int32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_subtract_multiple_times_I64(int64_t *op1_ptr, uint32_t op1_count, int64_t *op2_ptr, uint32_t op2_count, int64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_tan_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_tan_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_tanh_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_tanh_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_transform_vector_cm_2x_F64(float64_t *op1_ptr, float64_t *op2_ptr, float64_t *res_ptr);
void ZEND_FASTCALL qb_do_transform_vector_cm_2x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_transform_vector_cm_2x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_transform_vector_cm_3x_F64(float64_t *op1_ptr, float64_t *op2_ptr, float64_t *res_ptr);
void ZEND_FASTCALL qb_do_transform_vector_cm_3x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_transform_vector_cm_3x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_transform_vector_cm_4x_F64(float64_t *op1_ptr, float64_t *op2_ptr, float64_t *res_ptr);
void ZEND_FASTCALL qb_do_transform_vector_cm_4x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_transform_vector_cm_4x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_transform_vector_rm_2x_F64(float64_t *op1_ptr, float64_t *op2_ptr, float64_t *res_ptr);
void ZEND_FASTCALL qb_do_transform_vector_rm_2x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_transform_vector_rm_2x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_transform_vector_rm_3x_F64(float64_t *op1_ptr, float64_t *op2_ptr, float64_t *res_ptr);
void ZEND_FASTCALL qb_do_transform_vector_rm_3x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_transform_vector_rm_3x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_transform_vector_rm_4x_F64(float64_t *op1_ptr, float64_t *op2_ptr, float64_t *res_ptr);
void ZEND_FASTCALL qb_do_transform_vector_rm_4x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_transform_vector_rm_4x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *op2_ptr, uint32_t op2_count, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_transpose_matrix_2x_F64(float64_t *op1_ptr, float64_t *res_ptr);
void ZEND_FASTCALL qb_do_transpose_matrix_2x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_transpose_matrix_2x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_transpose_matrix_3x_F32(float32_t *op1_ptr, float32_t *res_ptr);
void ZEND_FASTCALL qb_do_transpose_matrix_3x_F64(float64_t *op1_ptr, float64_t *res_ptr);
void ZEND_FASTCALL qb_do_transpose_matrix_3x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_transpose_matrix_3x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_transpose_matrix_4x_F32(float32_t *op1_ptr, float32_t *res_ptr);
void ZEND_FASTCALL qb_do_transpose_matrix_4x_F64(float64_t *op1_ptr, float64_t *res_ptr);
void ZEND_FASTCALL qb_do_transpose_matrix_4x_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_transpose_matrix_4x_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_transpose_matrix_F32(float32_t *op1_ptr, uint32_t MATRIX1_ROWS, uint32_t MATRIX1_COLS, float32_t *res_ptr);
void ZEND_FASTCALL qb_do_transpose_matrix_F64(float64_t *op1_ptr, uint32_t MATRIX1_ROWS, uint32_t MATRIX1_COLS, float64_t *res_ptr);
void ZEND_FASTCALL qb_do_transpose_matrix_multiple_times_F32(float32_t *op1_ptr, uint32_t op1_count, uint32_t MATRIX1_ROWS, uint32_t MATRIX1_COLS, float32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_transpose_matrix_multiple_times_F64(float64_t *op1_ptr, uint32_t op1_count, uint32_t MATRIX1_ROWS, uint32_t MATRIX1_COLS, float64_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_utf8decode_U16(uint8_t *op1_ptr, uint32_t op1_count, uint16_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_utf8decode_U32(uint8_t *op1_ptr, uint32_t op1_count, uint32_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_utf8encode_U16(uint16_t *op1_ptr, uint32_t op1_count, uint8_t *res_ptr, uint32_t res_count);
void ZEND_FASTCALL qb_do_utf8encode_U32(uint32_t *op1_ptr, uint32_t op1_count, uint8_t *res_ptr, uint32_t res_count);

static zend_always_inline int32_t qb_quick_floor(double f) {
#ifdef FAST_FLOAT_TO_INT
	return (int32_t) floor(f);
#else
	int32_t n;
	if(f < -32768) {
		return -32768;
	} else if(f > 32767) {
		return 32767;
	}
	f += 103079215104.0;
	n = ((long *) &f)[0];
	return n >> 16;
#endif
}

static zend_always_inline int32_t qb_quick_floorf(float f) {
#ifdef FAST_FLOAT_TO_INT
	return (int32_t) floorf(f);
#else
	return qb_quick_floor(f);
#endif
}

static zend_always_inline int32_t qb_quick_round(double f) {
#ifdef FAST_FLOAT_TO_INT
	return (int32_t) round(f);
#else
	int32_t n;
	if(f < -32768) {
		return -32768;
	} else if(f > 32767) {
		return 32767;
	}
	f += 103079215103.5;
	n = ((long *) &f)[0];
	return n >> 16;
#endif
}

static zend_always_inline int32_t qb_quick_roundf(float f) {
#ifdef FAST_FLOAT_TO_INT
	return (int32_t) roundf(f);
#else
	return qb_quick_round(f);
#endif
}

static zend_always_inline void qb_do_apply_premultiplication_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count) {
	float32_t a = op1_ptr[3];
	if(a != 1) {
		res_ptr[0] = op1_ptr[0] * a;
		res_ptr[1] = op1_ptr[1] * a;
		res_ptr[2] = op1_ptr[2] * a;
		res_ptr[3] = op1_ptr[3];
	} else {
		res_ptr[0] = op1_ptr[0];
		res_ptr[1] = op1_ptr[1];
		res_ptr[2] = op1_ptr[2];
		res_ptr[3] = op1_ptr[3];
	}
}

static zend_always_inline void qb_do_apply_premultiplication_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count) {
	float64_t a = op1_ptr[3];
	if(a != 1) {
		res_ptr[0] = op1_ptr[0] * a;
		res_ptr[1] = op1_ptr[1] * a;
		res_ptr[2] = op1_ptr[2] * a;
		res_ptr[3] = op1_ptr[3];
	} else {
		res_ptr[0] = op1_ptr[0];
		res_ptr[1] = op1_ptr[1];
		res_ptr[2] = op1_ptr[2];
		res_ptr[3] = op1_ptr[3];
	}
}

static zend_always_inline void qb_do_array_insert_F32(float32_t *op1_ptr, uint32_t op1_count, uint32_t op2, float32_t *res_ptr, uint32_t res_count, uint32_t res_count_before) {
	memmove(res_ptr + op2 + op1_count, res_ptr + op2, sizeof(float32_t) * (res_count_before - op2));
	memcpy(res_ptr + op2, op1_ptr, sizeof(float32_t) * op1_count);
}

static zend_always_inline void qb_do_array_insert_F64(float64_t *op1_ptr, uint32_t op1_count, uint32_t op2, float64_t *res_ptr, uint32_t res_count, uint32_t res_count_before) {
	memmove(res_ptr + op2 + op1_count, res_ptr + op2, sizeof(float64_t) * (res_count_before - op2));
	memcpy(res_ptr + op2, op1_ptr, sizeof(float64_t) * op1_count);
}

static zend_always_inline void qb_do_array_insert_I08(int8_t *op1_ptr, uint32_t op1_count, uint32_t op2, int8_t *res_ptr, uint32_t res_count, uint32_t res_count_before) {
	memmove(res_ptr + op2 + op1_count, res_ptr + op2, sizeof(int8_t) * (res_count_before - op2));
	memcpy(res_ptr + op2, op1_ptr, sizeof(int8_t) * op1_count);
}

static zend_always_inline void qb_do_array_insert_I16(int16_t *op1_ptr, uint32_t op1_count, uint32_t op2, int16_t *res_ptr, uint32_t res_count, uint32_t res_count_before) {
	memmove(res_ptr + op2 + op1_count, res_ptr + op2, sizeof(int16_t) * (res_count_before - op2));
	memcpy(res_ptr + op2, op1_ptr, sizeof(int16_t) * op1_count);
}

static zend_always_inline void qb_do_array_insert_I32(int32_t *op1_ptr, uint32_t op1_count, uint32_t op2, int32_t *res_ptr, uint32_t res_count, uint32_t res_count_before) {
	memmove(res_ptr + op2 + op1_count, res_ptr + op2, sizeof(int32_t) * (res_count_before - op2));
	memcpy(res_ptr + op2, op1_ptr, sizeof(int32_t) * op1_count);
}

static zend_always_inline void qb_do_array_insert_I64(int64_t *op1_ptr, uint32_t op1_count, uint32_t op2, int64_t *res_ptr, uint32_t res_count, uint32_t res_count_before) {
	memmove(res_ptr + op2 + op1_count, res_ptr + op2, sizeof(int64_t) * (res_count_before - op2));
	memcpy(res_ptr + op2, op1_ptr, sizeof(int64_t) * op1_count);
}

static zend_always_inline void qb_do_clamp_F32(float32_t op1, float32_t op2, float32_t op3, float32_t *res_ptr) {
	if(op1 < op2) {
		(*res_ptr) = op2;
	} else if(op1 > op3) {
		(*res_ptr) = op3;
	} else {
		(*res_ptr) = op1;
	}
}

static zend_always_inline void qb_do_clamp_F64(float64_t op1, float64_t op2, float64_t op3, float64_t *res_ptr) {
	if(op1 < op2) {
		(*res_ptr) = op2;
	} else if(op1 > op3) {
		(*res_ptr) = op3;
	} else {
		(*res_ptr) = op1;
	}
}

static zend_always_inline void qb_do_complex_cos_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count) {
	float32_t r = cosf(op1_ptr[0]) * coshf(op1_ptr[1]);
	float32_t i = -sinf(op1_ptr[0]) * sinhf(op1_ptr[1]);
	res_ptr[0] = r;
	res_ptr[1] = i;
}

static zend_always_inline void qb_do_complex_cosh_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count) {
	float32_t r = coshf(op1_ptr[0]) * cosf(op1_ptr[1]);
	float32_t i = sinhf(op1_ptr[0]) * sinf(op1_ptr[1]);
	res_ptr[0] = r;
	res_ptr[1] = i;
}

static zend_always_inline void qb_do_complex_divide_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, float32_t *res_ptr, uint32_t res_count) {
	float32_t w = op2_ptr[0] * op2_ptr[0] + op2_ptr[1] * op2_ptr[1];
	float32_t r = ((op1_ptr[0] * op2_ptr[0]) + (op1_ptr[1] * op2_ptr[1])) / w;
	float32_t i = ((op1_ptr[1] * op2_ptr[0]) - (op1_ptr[0] * op2_ptr[1])) / w;
	res_ptr[0] = r;
	res_ptr[1] = i;
}

static zend_always_inline void qb_do_complex_exp_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count) {
	float32_t w = expf(op1_ptr[0]);
	float32_t r = w * cosf(op1_ptr[1]);
	float32_t i = w * sinf(op1_ptr[1]);
	res_ptr[0] = r;
	res_ptr[1] = i;
}

static zend_always_inline void qb_do_complex_log_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count) {
	float32_t w = sqrtf(op1_ptr[0] * op1_ptr[0] + op1_ptr[1] * op1_ptr[1]);
	float32_t r = logf(w);
	float32_t i = atan2f(op1_ptr[1], op1_ptr[0]);
	res_ptr[0] = r;
	res_ptr[1] = i;
}

static zend_always_inline void qb_do_complex_multiply_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, float32_t *res_ptr, uint32_t res_count) {
	float32_t r = (op1_ptr[0] * op2_ptr[0]) - (op1_ptr[1] * op2_ptr[1]);
	float32_t i = (op1_ptr[0] * op2_ptr[1]) + (op1_ptr[1] * op2_ptr[0]);
	res_ptr[0] = r;
	res_ptr[1] = i;
}

static zend_always_inline void qb_do_complex_pow_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *op2_ptr, uint32_t op2_count, float32_t *res_ptr, uint32_t res_count) {
	float32_t u = atan2f(op1_ptr[1], op1_ptr[0]);
	float32_t v = op1_ptr[0] * op1_ptr[0] + op1_ptr[1] * op1_ptr[1];
	float32_t w = 1;
	float32_t x = powf(v, 0.5f * op2_ptr[0]);
	float32_t y = op2_ptr[0] * u;
	float32_t z = 0;
	if(op2_ptr[1] != 0) {
		z = 0.5f * op2_ptr[1] * logf(v);
		w = expf(-op2_ptr[1] * u);
	}
	res_ptr[0] = x * w * cosf(y + z);
	res_ptr[1] = x * w * sinf(y + z);
}

static zend_always_inline void qb_do_complex_sin_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count) {
	float32_t r = sinf(op1_ptr[0]) * coshf(op1_ptr[1]);
	float32_t i = cosf(op1_ptr[0]) * sinhf(op1_ptr[1]);
	res_ptr[0] = r;
	res_ptr[1] = i;
}

static zend_always_inline void qb_do_complex_sinh_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count) {
	float32_t r = sinhf(op1_ptr[0]) * cosf(op1_ptr[1]);
	float32_t i = coshf(op1_ptr[0]) * sinf(op1_ptr[1]);
	res_ptr[0] = r;
	res_ptr[1] = i;
}

static zend_always_inline void qb_do_complex_square_root_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count) {
	float32_t s = (op1_ptr[1] > 0 ? 1.0f : ((op1_ptr[1] < 0 ? -1.0f : 0.0f)));
	float32_t w = sqrtf(op1_ptr[0] * op1_ptr[0] + op1_ptr[1] * op1_ptr[1]);
	float32_t r = sqrtf(0.5f * (op1_ptr[0] + w));
	float32_t i = (s) ? s * sqrtf(0.5f * (-op1_ptr[0] + w)) : 0;
	res_ptr[0] = r;
	res_ptr[1] = i;
}

static zend_always_inline void qb_do_complex_tan_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count) {
	float32_t w = 1 / (cosf(2.0 * op1_ptr[0]) + coshf(2.0 * op1_ptr[1]));
	float32_t r = w * sinf(2.0 * op1_ptr[0]);
	float32_t i = w * sinhf(2.0 * op1_ptr[1]);
	res_ptr[0] = r;
	res_ptr[1] = i;
}

static zend_always_inline void qb_do_complex_tanh_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count) {
	float32_t w = 1 / (coshf(2.0 * op1_ptr[0]) + cosf(2.0 * op1_ptr[1]));
	float32_t r = w * sinhf(2.0 * op1_ptr[0]);
	float32_t i = w * sinf(2.0 * op1_ptr[1]);
	res_ptr[0] = r;
	res_ptr[1] = i;
}

static zend_always_inline void qb_do_cross_product_2x_F32(float32_t *op1_ptr, float32_t *op2_ptr, float32_t *res_ptr) {
	float32_t k = (op1_ptr[0] * op2_ptr[1]) - (op1_ptr[1] * op2_ptr[0]);
	res_ptr[0] = k;
	res_ptr[1] = k;
}

static zend_always_inline void qb_do_cross_product_3x_F32(float32_t *op1_ptr, float32_t *op2_ptr, float32_t *res_ptr) {
	float32_t i = (op1_ptr[1] * op2_ptr[2]) - (op1_ptr[2] * op2_ptr[1]);
	float32_t j = (op1_ptr[2] * op2_ptr[0]) - (op1_ptr[0] * op2_ptr[2]);
	float32_t k = (op1_ptr[0] * op2_ptr[1]) - (op1_ptr[1] * op2_ptr[0]);
	res_ptr[0] = i;
	res_ptr[1] = j;
	res_ptr[2] = k;
}

static zend_always_inline void qb_do_determinant_3x_F32(float32_t *op1_ptr, float32_t *res_ptr) {
	(*res_ptr) =	 (op1_ptr[0 * 3 + 0] * op1_ptr[1 * 3 + 1] * op1_ptr[2 * 3 + 2]) -
	(op1_ptr[0 * 3 + 0] * op1_ptr[1 * 3 + 2] * op1_ptr[2 * 3 + 1]) +
	(op1_ptr[0 * 3 + 1] * op1_ptr[1 * 3 + 2] * op1_ptr[2 * 3 + 0]) -
	(op1_ptr[0 * 3 + 1] * op1_ptr[1 * 3 + 0] * op1_ptr[2 * 3 + 2]) +
	(op1_ptr[0 * 3 + 2] * op1_ptr[1 * 3 + 0] * op1_ptr[2 * 3 + 1]) -
	(op1_ptr[0 * 3 + 2] * op1_ptr[1 * 3 + 1] * op1_ptr[2 * 3 + 0]);
}

static zend_always_inline void qb_do_determinant_4x_F32(float32_t *op1_ptr, float32_t *res_ptr) {
	(*res_ptr) = 	 ((op1_ptr[0 * 4 + 3] * op1_ptr[1 * 4 + 2]) * (op1_ptr[2 * 4 + 1] * op1_ptr[3 * 4 + 0])) - ((op1_ptr[0 * 4 + 2] * op1_ptr[1 * 4 + 3]) * (op1_ptr[2 * 4 + 1] * op1_ptr[3 * 4 + 0])) -
	((op1_ptr[0 * 4 + 3] * op1_ptr[1 * 4 + 1]) * (op1_ptr[2 * 4 + 2] * op1_ptr[3 * 4 + 0])) + ((op1_ptr[0 * 4 + 1] * op1_ptr[1 * 4 + 3]) * (op1_ptr[2 * 4 + 2] * op1_ptr[3 * 4 + 0])) +
	((op1_ptr[0 * 4 + 2] * op1_ptr[1 * 4 + 1]) * (op1_ptr[2 * 4 + 3] * op1_ptr[3 * 4 + 0])) - ((op1_ptr[0 * 4 + 1] * op1_ptr[1 * 4 + 2]) * (op1_ptr[2 * 4 + 3] * op1_ptr[3 * 4 + 0])) -
	((op1_ptr[0 * 4 + 3] * op1_ptr[1 * 4 + 2]) * (op1_ptr[2 * 4 + 0] * op1_ptr[3 * 4 + 1])) + ((op1_ptr[0 * 4 + 2] * op1_ptr[1 * 4 + 3]) * (op1_ptr[2 * 4 + 0] * op1_ptr[3 * 4 + 1])) +
	((op1_ptr[0 * 4 + 3] * op1_ptr[1 * 4 + 0]) * (op1_ptr[2 * 4 + 2] * op1_ptr[3 * 4 + 1])) - ((op1_ptr[0 * 4 + 0] * op1_ptr[1 * 4 + 3]) * (op1_ptr[2 * 4 + 2] * op1_ptr[3 * 4 + 1])) -
	((op1_ptr[0 * 4 + 2] * op1_ptr[1 * 4 + 0]) * (op1_ptr[2 * 4 + 3] * op1_ptr[3 * 4 + 1])) + ((op1_ptr[0 * 4 + 0] * op1_ptr[1 * 4 + 2]) * (op1_ptr[2 * 4 + 3] * op1_ptr[3 * 4 + 1])) +
	((op1_ptr[0 * 4 + 3] * op1_ptr[1 * 4 + 1]) * (op1_ptr[2 * 4 + 0] * op1_ptr[3 * 4 + 2])) - ((op1_ptr[0 * 4 + 1] * op1_ptr[1 * 4 + 3]) * (op1_ptr[2 * 4 + 0] * op1_ptr[3 * 4 + 2])) -
	((op1_ptr[0 * 4 + 3] * op1_ptr[1 * 4 + 0]) * (op1_ptr[2 * 4 + 1] * op1_ptr[3 * 4 + 2])) + ((op1_ptr[0 * 4 + 0] * op1_ptr[1 * 4 + 3]) * (op1_ptr[2 * 4 + 1] * op1_ptr[3 * 4 + 2])) +
	((op1_ptr[0 * 4 + 1] * op1_ptr[1 * 4 + 0]) * (op1_ptr[2 * 4 + 3] * op1_ptr[3 * 4 + 2])) - ((op1_ptr[0 * 4 + 0] * op1_ptr[1 * 4 + 1]) * (op1_ptr[2 * 4 + 3] * op1_ptr[3 * 4 + 2])) -
	((op1_ptr[0 * 4 + 2] * op1_ptr[1 * 4 + 1]) * (op1_ptr[2 * 4 + 0] * op1_ptr[3 * 4 + 3])) + ((op1_ptr[0 * 4 + 1] * op1_ptr[1 * 4 + 2]) * (op1_ptr[2 * 4 + 0] * op1_ptr[3 * 4 + 3])) +
	((op1_ptr[0 * 4 + 2] * op1_ptr[1 * 4 + 0]) * (op1_ptr[2 * 4 + 1] * op1_ptr[3 * 4 + 3])) - ((op1_ptr[0 * 4 + 0] * op1_ptr[1 * 4 + 2]) * (op1_ptr[2 * 4 + 1] * op1_ptr[3 * 4 + 3])) -
	((op1_ptr[0 * 4 + 1] * op1_ptr[1 * 4 + 0]) * (op1_ptr[2 * 4 + 2] * op1_ptr[3 * 4 + 3])) + ((op1_ptr[0 * 4 + 0] * op1_ptr[1 * 4 + 1]) * (op1_ptr[2 * 4 + 2] * op1_ptr[3 * 4 + 3]));
}

static zend_always_inline void qb_do_distance_2x_F32(float32_t *op1_ptr, float32_t *op2_ptr, float32_t *res_ptr) {
	float32_t sum = (op1_ptr[0] - op2_ptr[0]) * (op1_ptr[0] - op2_ptr[0]) + (op1_ptr[1] - op2_ptr[1]) * (op1_ptr[1] - op2_ptr[1]);
	(*res_ptr) = qb_fast_sqrtf(sum);
}

static zend_always_inline void qb_do_distance_3x_F32(float32_t *op1_ptr, float32_t *op2_ptr, float32_t *res_ptr) {
	float32_t sum = (op1_ptr[0] - op2_ptr[0]) * (op1_ptr[0] - op2_ptr[0]) + (op1_ptr[1] - op2_ptr[1]) * (op1_ptr[1] - op2_ptr[1]) + (op1_ptr[2] - op2_ptr[2]) * (op1_ptr[2] - op2_ptr[2]);
	(*res_ptr) = qb_fast_sqrtf(sum);
}

static zend_always_inline void qb_do_distance_4x_F32(float32_t *op1_ptr, float32_t *op2_ptr, float32_t *res_ptr) {
	float32_t sum = (op1_ptr[0] - op2_ptr[0]) * (op1_ptr[0] - op2_ptr[0]) + (op1_ptr[1] - op2_ptr[1]) * (op1_ptr[1] - op2_ptr[1]) + (op1_ptr[2] - op2_ptr[2]) * (op1_ptr[2] - op2_ptr[2]) + (op1_ptr[3] - op2_ptr[3]) * (op1_ptr[3] - op2_ptr[3]);
	(*res_ptr) = qb_fast_sqrtf(sum);
}

static zend_always_inline void qb_do_divide_S08(qb_interpreter_context *cxt, int8_t op1, int8_t op2, int8_t *res_ptr, uint32_t PHP_LINE_NUMBER) {
	if(UNEXPECTED(op2 == 0)) {
		qb_abort_divide_by_zero_error(cxt, PHP_LINE_NUMBER);
	}
	(*res_ptr) = op1 / op2;
}

static zend_always_inline void qb_do_divide_S16(qb_interpreter_context *cxt, int16_t op1, int16_t op2, int16_t *res_ptr, uint32_t PHP_LINE_NUMBER) {
	if(UNEXPECTED(op2 == 0)) {
		qb_abort_divide_by_zero_error(cxt, PHP_LINE_NUMBER);
	}
	(*res_ptr) = op1 / op2;
}

static zend_always_inline void qb_do_divide_S32(qb_interpreter_context *cxt, int32_t op1, int32_t op2, int32_t *res_ptr, uint32_t PHP_LINE_NUMBER) {
	if(UNEXPECTED(op2 == 0)) {
		qb_abort_divide_by_zero_error(cxt, PHP_LINE_NUMBER);
	}
	(*res_ptr) = op1 / op2;
}

static zend_always_inline void qb_do_divide_S64(qb_interpreter_context *cxt, int64_t op1, int64_t op2, int64_t *res_ptr, uint32_t PHP_LINE_NUMBER) {
	if(UNEXPECTED(op2 == 0)) {
		qb_abort_divide_by_zero_error(cxt, PHP_LINE_NUMBER);
	}
	(*res_ptr) = op1 / op2;
}

static zend_always_inline void qb_do_divide_U08(qb_interpreter_context *cxt, uint8_t op1, uint8_t op2, uint8_t *res_ptr, uint32_t PHP_LINE_NUMBER) {
	if(UNEXPECTED(op2 == 0)) {
		qb_abort_divide_by_zero_error(cxt, PHP_LINE_NUMBER);
	}
	(*res_ptr) = op1 / op2;
}

static zend_always_inline void qb_do_divide_U16(qb_interpreter_context *cxt, uint16_t op1, uint16_t op2, uint16_t *res_ptr, uint32_t PHP_LINE_NUMBER) {
	if(UNEXPECTED(op2 == 0)) {
		qb_abort_divide_by_zero_error(cxt, PHP_LINE_NUMBER);
	}
	(*res_ptr) = op1 / op2;
}

static zend_always_inline void qb_do_divide_U32(qb_interpreter_context *cxt, uint32_t op1, uint32_t op2, uint32_t *res_ptr, uint32_t PHP_LINE_NUMBER) {
	if(UNEXPECTED(op2 == 0)) {
		qb_abort_divide_by_zero_error(cxt, PHP_LINE_NUMBER);
	}
	(*res_ptr) = op1 / op2;
}

static zend_always_inline void qb_do_divide_U64(qb_interpreter_context *cxt, uint64_t op1, uint64_t op2, uint64_t *res_ptr, uint32_t PHP_LINE_NUMBER) {
	if(UNEXPECTED(op2 == 0)) {
		qb_abort_divide_by_zero_error(cxt, PHP_LINE_NUMBER);
	}
	(*res_ptr) = op1 / op2;
}

static zend_always_inline void qb_do_face_forward_2x_F32(float32_t *op1_ptr, float32_t *op2_ptr, float32_t *res_ptr) {
	float32_t dot_product = (op1_ptr[0] * op2_ptr[0]) + (op1_ptr[1] * op2_ptr[1]);
	if(dot_product > 0) {
		res_ptr[0] = op1_ptr[0];
		res_ptr[1] = op1_ptr[1];
	} else {
		res_ptr[0] = -op1_ptr[0];
		res_ptr[1] = -op1_ptr[1];
	}
}

static zend_always_inline void qb_do_face_forward_3x_F32(float32_t *op1_ptr, float32_t *op2_ptr, float32_t *res_ptr) {
	float32_t dot_product = (op1_ptr[0] * op2_ptr[0]) + (op1_ptr[1] * op2_ptr[1]) + (op1_ptr[2] * op2_ptr[2]);
	if(dot_product > 0) {
		res_ptr[0] = op1_ptr[0];
		res_ptr[1] = op1_ptr[1];
		res_ptr[2] = op1_ptr[2];
	} else {
		res_ptr[0] = -op1_ptr[0];
		res_ptr[1] = -op1_ptr[1];
		res_ptr[2] = -op1_ptr[2];
	}
}

static zend_always_inline void qb_do_face_forward_4x_F32(float32_t *op1_ptr, float32_t *op2_ptr, float32_t *res_ptr) {
	float32_t dot_product = (op1_ptr[0] * op2_ptr[0]) + (op1_ptr[1] * op2_ptr[1]) + (op1_ptr[2] * op2_ptr[2]) + (op1_ptr[3] * op2_ptr[3]);
	if(dot_product > 0) {
		res_ptr[0] = op1_ptr[0];
		res_ptr[1] = op1_ptr[1];
		res_ptr[2] = op1_ptr[2];
		res_ptr[3] = op1_ptr[3];
	} else {
		res_ptr[0] = -op1_ptr[0];
		res_ptr[1] = -op1_ptr[1];
		res_ptr[2] = -op1_ptr[2];
		res_ptr[3] = -op1_ptr[3];
	}
}

static zend_always_inline void qb_do_hsl2rgb_4x_F32(float32_t *op1_ptr, float32_t *res_ptr) {
	qb_convert_hsl_to_rgb_F32(op1_ptr, res_ptr);
	res_ptr[3] = op1_ptr[3];
}

static zend_always_inline void qb_do_hsl2rgb_4x_F64(float64_t *op1_ptr, float64_t *res_ptr) {
	qb_convert_hsl_to_rgb_F64(op1_ptr, res_ptr);
	res_ptr[3] = op1_ptr[3];
}

static zend_always_inline void qb_do_hsv2rgb_4x_F32(float32_t *op1_ptr, float32_t *res_ptr) {
	qb_convert_hsv_to_rgb_F32(op1_ptr, res_ptr);
	res_ptr[3] = op1_ptr[3];
}

static zend_always_inline void qb_do_hsv2rgb_4x_F64(float64_t *op1_ptr, float64_t *res_ptr) {
	qb_convert_hsv_to_rgb_F64(op1_ptr, res_ptr);
	res_ptr[3] = op1_ptr[3];
}

static zend_always_inline void qb_do_in_array_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t op2, int32_t *res_ptr) {
	int32_t index;
	qb_do_array_search_F32(op1_ptr, op1_count, op2, &index);
	(*res_ptr) = (index != -1);
}

static zend_always_inline void qb_do_in_array_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t op2, int32_t *res_ptr) {
	int32_t index;
	qb_do_array_search_F64(op1_ptr, op1_count, op2, &index);
	(*res_ptr) = (index != -1);
}

static zend_always_inline void qb_do_in_array_I08(int8_t *op1_ptr, uint32_t op1_count, int8_t op2, int32_t *res_ptr) {
	int32_t index;
	qb_do_array_search_I08(op1_ptr, op1_count, op2, &index);
	(*res_ptr) = (index != -1);
}

static zend_always_inline void qb_do_in_array_I16(int16_t *op1_ptr, uint32_t op1_count, int16_t op2, int32_t *res_ptr) {
	int32_t index;
	qb_do_array_search_I16(op1_ptr, op1_count, op2, &index);
	(*res_ptr) = (index != -1);
}

static zend_always_inline void qb_do_in_array_I32(int32_t *op1_ptr, uint32_t op1_count, int32_t op2, int32_t *res_ptr) {
	int32_t index;
	qb_do_array_search_I32(op1_ptr, op1_count, op2, &index);
	(*res_ptr) = (index != -1);
}

static zend_always_inline void qb_do_in_array_I64(int64_t *op1_ptr, uint32_t op1_count, int64_t op2, int32_t *res_ptr) {
	int32_t index;
	qb_do_array_search_I64(op1_ptr, op1_count, op2, &index);
	(*res_ptr) = (index != -1);
}

static zend_always_inline void qb_do_length_2x_F32(float32_t *op1_ptr, float32_t *res_ptr) {
	float32_t sum = (op1_ptr[0] * op1_ptr[0]) + (op1_ptr[1] * op1_ptr[1]);
	(*res_ptr) = qb_fast_sqrtf(sum);
}

static zend_always_inline void qb_do_length_3x_F32(float32_t *op1_ptr, float32_t *res_ptr) {
	float32_t sum = (op1_ptr[0] * op1_ptr[0]) + (op1_ptr[1] * op1_ptr[1]) + (op1_ptr[2] * op1_ptr[2]);
	(*res_ptr) = qb_fast_sqrtf(sum);
}

static zend_always_inline void qb_do_length_4x_F32(float32_t *op1_ptr, float32_t *res_ptr) {
	float32_t sum = (op1_ptr[0] * op1_ptr[0]) + (op1_ptr[1] * op1_ptr[1]) + (op1_ptr[2] * op1_ptr[2]) + (op1_ptr[3] * op1_ptr[3]);
	(*res_ptr) = qb_fast_sqrtf(sum);
}

static zend_always_inline void qb_do_multiply_matrix_by_matrix_cm_2x_F32(float32_t *op1_ptr, float32_t *op2_ptr, float32_t *res_ptr) {
	float32_t dot_product0 = (op1_ptr[0 * 2 + 0] * op2_ptr[0 * 2 + 0]) + (op1_ptr[0 * 2 + 1] * op2_ptr[1 * 2 + 0]);
	float32_t dot_product1 = (op1_ptr[0 * 2 + 0] * op2_ptr[0 * 2 + 1]) + (op1_ptr[0 * 2 + 1] * op2_ptr[1 * 2 + 1]);
	float32_t dot_product2 = (op1_ptr[1 * 2 + 0] * op2_ptr[0 * 2 + 0]) + (op1_ptr[1 * 2 + 1] * op2_ptr[1 * 2 + 0]);
	float32_t dot_product3 = (op1_ptr[1 * 2 + 0] * op2_ptr[0 * 2 + 1]) + (op1_ptr[1 * 2 + 1] * op2_ptr[1 * 2 + 1]);
	res_ptr[0 * 2 + 0] = dot_product0;
	res_ptr[0 * 2 + 1] = dot_product1;
	res_ptr[1 * 2 + 0] = dot_product2;
	res_ptr[1 * 2 + 1] = dot_product3;
}

static zend_always_inline void qb_do_multiply_matrix_by_vector_cm_2x_F32(float32_t *op1_ptr, float32_t *op2_ptr, float32_t *res_ptr) {
	float32_t dot_product0 = (op1_ptr[0 * 2 + 0] * op2_ptr[0]) + (op1_ptr[1 * 2 + 0] * op2_ptr[1]);
	float32_t dot_product1 = (op1_ptr[0 * 2 + 1] * op2_ptr[0]) + (op1_ptr[1 * 2 + 1] * op2_ptr[1]);
	res_ptr[0] = dot_product0;
	res_ptr[1] = dot_product1;
}

static zend_always_inline void qb_do_multiply_matrix_by_vector_cm_3x_F32(float32_t *op1_ptr, float32_t *op2_ptr, float32_t *res_ptr) {
	float32_t dot_product0 = (op1_ptr[0 * 3 + 0] * op2_ptr[0]) + (op1_ptr[1 * 3 + 0] * op2_ptr[1]) + (op1_ptr[2 * 3 + 0] * op2_ptr[2]);
	float32_t dot_product1 = (op1_ptr[0 * 3 + 1] * op2_ptr[0]) + (op1_ptr[1 * 3 + 1] * op2_ptr[1]) + (op1_ptr[2 * 3 + 1] * op2_ptr[2]);
	float32_t dot_product2 = (op1_ptr[0 * 3 + 2] * op2_ptr[0]) + (op1_ptr[1 * 3 + 2] * op2_ptr[1]) + (op1_ptr[2 * 3 + 2] * op2_ptr[2]);
	res_ptr[0] = dot_product0;
	res_ptr[1] = dot_product1;
	res_ptr[2] = dot_product2;
}

static zend_always_inline void qb_do_multiply_matrix_by_vector_cm_3x_padded_F32(float32_t *op1_ptr, float32_t *op2_ptr, float32_t *res_ptr) {
	float32_t dot_product0 = (op1_ptr[0 * 4 + 0] * op2_ptr[0]) + (op1_ptr[1 * 4 + 0] * op2_ptr[1]) + (op1_ptr[2 * 4 + 0] * op2_ptr[2]);
	float32_t dot_product1 = (op1_ptr[0 * 4 + 1] * op2_ptr[0]) + (op1_ptr[1 * 4 + 1] * op2_ptr[1]) + (op1_ptr[2 * 4 + 1] * op2_ptr[2]);
	float32_t dot_product2 = (op1_ptr[0 * 4 + 2] * op2_ptr[0]) + (op1_ptr[1 * 4 + 2] * op2_ptr[1]) + (op1_ptr[2 * 4 + 2] * op2_ptr[2]);
	res_ptr[0] = dot_product0;
	res_ptr[1] = dot_product1;
	res_ptr[2] = dot_product2;
}

static zend_always_inline void qb_do_multiply_matrix_by_vector_cm_4x_F32(float32_t *op1_ptr, float32_t *op2_ptr, float32_t *res_ptr) {
	float32_t dot_product0 = (op1_ptr[0 * 4 + 0] * op2_ptr[0]) + (op1_ptr[1 * 4 + 0] * op2_ptr[1]) + (op1_ptr[2 * 4 + 0] * op2_ptr[2]) + (op1_ptr[3 * 4 + 0] * op2_ptr[3]);
	float32_t dot_product1 = (op1_ptr[0 * 4 + 1] * op2_ptr[0]) + (op1_ptr[1 * 4 + 1] * op2_ptr[1]) + (op1_ptr[2 * 4 + 1] * op2_ptr[2]) + (op1_ptr[3 * 4 + 1] * op2_ptr[3]);
	float32_t dot_product2 = (op1_ptr[0 * 4 + 2] * op2_ptr[0]) + (op1_ptr[1 * 4 + 2] * op2_ptr[1]) + (op1_ptr[2 * 4 + 2] * op2_ptr[2]) + (op1_ptr[3 * 4 + 2] * op2_ptr[3]);
	float32_t dot_product3 = (op1_ptr[0 * 4 + 3] * op2_ptr[0]) + (op1_ptr[1 * 4 + 3] * op2_ptr[1]) + (op1_ptr[2 * 4 + 3] * op2_ptr[2]) + (op1_ptr[3 * 4 + 3] * op2_ptr[3]);
	res_ptr[0] = dot_product0;
	res_ptr[1] = dot_product1;
	res_ptr[2] = dot_product2;
	res_ptr[3] = dot_product3;
}

static zend_always_inline void qb_do_multiply_vector_by_matrix_cm_2x_F32(float32_t *op1_ptr, float32_t *op2_ptr, float32_t *res_ptr) {
	float32_t dot_product0 = (op1_ptr[0] * op2_ptr[0 * 2 + 0]) + (op1_ptr[1] * op2_ptr[0 * 2 + 1]);
	float32_t dot_product1 = (op1_ptr[0] * op2_ptr[1 * 2 + 0]) + (op1_ptr[1] * op2_ptr[1 * 2 + 1]);
	res_ptr[0] = dot_product0;
	res_ptr[1] = dot_product1;
}

static zend_always_inline void qb_do_multiply_vector_by_matrix_cm_3x_F32(float32_t *op1_ptr, float32_t *op2_ptr, float32_t *res_ptr) {
	float32_t dot_product0 = (op1_ptr[0] * op2_ptr[0 * 3 + 0]) + (op1_ptr[1] * op2_ptr[0 * 3 + 1]) + (op1_ptr[2] * op2_ptr[0 * 3 + 2]);
	float32_t dot_product1 = (op1_ptr[0] * op2_ptr[1 * 3 + 0]) + (op1_ptr[1] * op2_ptr[1 * 3 + 1]) + (op1_ptr[2] * op2_ptr[1 * 3 + 2]);
	float32_t dot_product2 = (op1_ptr[0] * op2_ptr[2 * 3 + 0]) + (op1_ptr[1] * op2_ptr[2 * 3 + 1]) + (op1_ptr[2] * op2_ptr[2 * 3 + 2]);
	res_ptr[0] = dot_product0;
	res_ptr[1] = dot_product1;
	res_ptr[2] = dot_product2;
}

static zend_always_inline void qb_do_multiply_vector_by_matrix_cm_3x_padded_F32(float32_t *op1_ptr, float32_t *op2_ptr, float32_t *res_ptr) {
	float32_t dot_product0 = (op1_ptr[0] * op2_ptr[0 * 4 + 0]) + (op1_ptr[1] * op2_ptr[0 * 4 + 1]) + (op1_ptr[2] * op2_ptr[0 * 4 + 2]);
	float32_t dot_product1 = (op1_ptr[0] * op2_ptr[1 * 4 + 0]) + (op1_ptr[1] * op2_ptr[1 * 4 + 1]) + (op1_ptr[2] * op2_ptr[1 * 4 + 2]);
	float32_t dot_product2 = (op1_ptr[0] * op2_ptr[2 * 4 + 0]) + (op1_ptr[1] * op2_ptr[2 * 4 + 1]) + (op1_ptr[2] * op2_ptr[2 * 4 + 2]);
	res_ptr[0] = dot_product0;
	res_ptr[1] = dot_product1;
	res_ptr[2] = dot_product2;
}

static zend_always_inline void qb_do_multiply_vector_by_matrix_cm_4x_F32(float32_t *op1_ptr, float32_t *op2_ptr, float32_t *res_ptr) {
	float32_t dot_product0 = (op1_ptr[0] * op2_ptr[0 * 4 + 0]) + (op1_ptr[1] * op2_ptr[0 * 4 + 1]) + (op1_ptr[2] * op2_ptr[0 * 4 + 2]) + (op1_ptr[3] * op2_ptr[0 * 4 + 3]);
	float32_t dot_product1 = (op1_ptr[0] * op2_ptr[1 * 4 + 0]) + (op1_ptr[1] * op2_ptr[1 * 4 + 1]) + (op1_ptr[2] * op2_ptr[1 * 4 + 2]) + (op1_ptr[3] * op2_ptr[1 * 4 + 3]);
	float32_t dot_product2 = (op1_ptr[0] * op2_ptr[2 * 4 + 0]) + (op1_ptr[1] * op2_ptr[2 * 4 + 1]) + (op1_ptr[2] * op2_ptr[2 * 4 + 2]) + (op1_ptr[3] * op2_ptr[2 * 4 + 3]);
	float32_t dot_product3 = (op1_ptr[0] * op2_ptr[3 * 4 + 0]) + (op1_ptr[1] * op2_ptr[3 * 4 + 1]) + (op1_ptr[2] * op2_ptr[3 * 4 + 2]) + (op1_ptr[3] * op2_ptr[3 * 4 + 3]);
	res_ptr[0] = dot_product0;
	res_ptr[1] = dot_product1;
	res_ptr[2] = dot_product2;
	res_ptr[3] = dot_product3;
}

static zend_always_inline void qb_do_normalize_2x_F32(float32_t *op1_ptr, float32_t *res_ptr) {
	float32_t sum = (op1_ptr[0] * op1_ptr[0]) + (op1_ptr[1] * op1_ptr[1]);
	float32_t r = qb_fast_rsqrtf(sum);
	res_ptr[0] = op1_ptr[0] * r;
	res_ptr[1] = op1_ptr[1] * r;
}

static zend_always_inline void qb_do_normalize_3x_F32(float32_t *op1_ptr, float32_t *res_ptr) {
	float32_t sum = (op1_ptr[0] * op1_ptr[0]) + (op1_ptr[1] * op1_ptr[1]) + (op1_ptr[2] * op1_ptr[2]);
	float32_t r = qb_fast_rsqrtf(sum);
	res_ptr[0] = op1_ptr[0] * r;
	res_ptr[1] = op1_ptr[1] * r;
	res_ptr[2] = op1_ptr[2] * r;
}

static zend_always_inline void qb_do_normalize_4x_F32(float32_t *op1_ptr, float32_t *res_ptr) {
	float32_t sum = (op1_ptr[0] * op1_ptr[0]) + (op1_ptr[1] * op1_ptr[1]) + (op1_ptr[2] * op1_ptr[2]) + (op1_ptr[3] * op1_ptr[3]);
	float32_t r = qb_fast_rsqrtf(sum);
	res_ptr[0] = op1_ptr[0] * r;
	res_ptr[1] = op1_ptr[1] * r;
	res_ptr[2] = op1_ptr[2] * r;
	res_ptr[3] = op1_ptr[3] * r;
}

static zend_always_inline void qb_do_pack_F32(float32_t op1, uint8_t *res_ptr, uint32_t res_count) {
	float32_t v = op1;
	*((uint32_t *) res_ptr) = SWAP_LE_I32(*((uint32_t *) &v));
}

static zend_always_inline void qb_do_pack_F64(float64_t op1, uint8_t *res_ptr, uint32_t res_count) {
	float64_t v = op1;
	*((uint64_t *) res_ptr) = SWAP_LE_I64(*((uint64_t *) &v));
}

static zend_always_inline void qb_do_reflect_2x_F32(float32_t *op1_ptr, float32_t *op2_ptr, float32_t *res_ptr) {
	float32_t dot_product = (op1_ptr[0] * op2_ptr[0]) + (op1_ptr[1] * op2_ptr[1]);
	res_ptr[0] = (float32_t) (op1_ptr[0] - 2.0 * dot_product * op2_ptr[0]);
	res_ptr[1] = (float32_t) (op1_ptr[1] - 2.0 * dot_product * op2_ptr[1]);
}

static zend_always_inline void qb_do_reflect_3x_F32(float32_t *op1_ptr, float32_t *op2_ptr, float32_t *res_ptr) {
	float32_t dot_product = (op1_ptr[0] * op2_ptr[0]) + (op1_ptr[1] * op2_ptr[1]) + (op1_ptr[2] * op2_ptr[2]);
	res_ptr[0] = (float32_t) (op1_ptr[0] - 2.0 * dot_product * op2_ptr[0]);
	res_ptr[1] = (float32_t) (op1_ptr[1] - 2.0 * dot_product * op2_ptr[1]);
	res_ptr[2] = (float32_t) (op1_ptr[2] - 2.0 * dot_product * op2_ptr[2]);
}

static zend_always_inline void qb_do_reflect_4x_F32(float32_t *op1_ptr, float32_t *op2_ptr, float32_t *res_ptr) {
	float32_t dot_product = (op1_ptr[0] * op2_ptr[0]) + (op1_ptr[1] * op2_ptr[1]) + (op1_ptr[2] * op2_ptr[2]) + (op1_ptr[3] * op2_ptr[3]);
	res_ptr[0] = (float32_t) (op1_ptr[0] - 2.0 * dot_product * op2_ptr[0]);
	res_ptr[1] = (float32_t) (op1_ptr[1] - 2.0 * dot_product * op2_ptr[1]);
	res_ptr[2] = (float32_t) (op1_ptr[2] - 2.0 * dot_product * op2_ptr[2]);
	res_ptr[3] = (float32_t) (op1_ptr[3] - 2.0 * dot_product * op2_ptr[3]);
}

static zend_always_inline void qb_do_refract_2x_F32(float32_t *op1_ptr, float32_t *op2_ptr, float32_t op3, float32_t *res_ptr) {
	float32_t dot_product = (op1_ptr[0] * op2_ptr[0]) + (op1_ptr[1] * op2_ptr[1]);
	float32_t k = (float32_t) (1.0 - (op3 * op3) * (1.0 - dot_product * dot_product));
	if(k < 0.0) {
		res_ptr[0] = 
		res_ptr[1] = 0.0;
	} else {
		float32_t m = op3 * dot_product + qb_fast_sqrtf(k);
		res_ptr[0] = op3 * op1_ptr[0] - m * op2_ptr[0];
		res_ptr[1] = op3 * op1_ptr[1] - m * op2_ptr[1];
	}
}

static zend_always_inline void qb_do_refract_3x_F32(float32_t *op1_ptr, float32_t *op2_ptr, float32_t op3, float32_t *res_ptr) {
	float32_t dot_product = (op1_ptr[0] * op2_ptr[0]) + (op1_ptr[1] * op2_ptr[1]) + (op1_ptr[2] * op2_ptr[2]);
	float32_t k = (float32_t) (1.0 - (op3 * op3) * (1.0 - dot_product * dot_product));
	if(k < 0.0) {
		res_ptr[0] = 
		res_ptr[1] = 
		res_ptr[2] = 0.0;
	} else {
		float32_t m = op3 * dot_product + qb_fast_sqrtf(k);
		res_ptr[0] = op3 * op1_ptr[0] - m * op2_ptr[0];
		res_ptr[1] = op3 * op1_ptr[1] - m * op2_ptr[1];
		res_ptr[2] = op3 * op1_ptr[2] - m * op2_ptr[2];
	}
}

static zend_always_inline void qb_do_refract_4x_F32(float32_t *op1_ptr, float32_t *op2_ptr, float32_t op3, float32_t *res_ptr) {
	float32_t dot_product = (op1_ptr[0] * op2_ptr[0]) + (op1_ptr[1] * op2_ptr[1]) + (op1_ptr[2] * op2_ptr[2]) + (op1_ptr[3] * op2_ptr[3]);
	float32_t k = (float32_t) (1.0 - (op3 * op3) * (1.0 - dot_product * dot_product));
	if(k < 0.0) {
		res_ptr[0] = 
		res_ptr[1] = 
		res_ptr[2] = 
		res_ptr[3] = 0.0;
	} else {
		float32_t m = op3 * dot_product + qb_fast_sqrtf(k);
		res_ptr[0] = op3 * op1_ptr[0] - m * op2_ptr[0];
		res_ptr[1] = op3 * op1_ptr[1] - m * op2_ptr[1];
		res_ptr[2] = op3 * op1_ptr[2] - m * op2_ptr[2];
		res_ptr[3] = op3 * op1_ptr[3] - m * op2_ptr[3];
	}
}

static zend_always_inline void qb_do_remove_premultiplication_F32(float32_t *op1_ptr, uint32_t op1_count, float32_t *res_ptr, uint32_t res_count) {
	float32_t a = op1_ptr[3];
	if(a != 1) { 
		float32_t ia = (a != 0) ? 1 / a : 0;
		res_ptr[0] = op1_ptr[0] * ia;
		res_ptr[1] = op1_ptr[1] * ia;
		res_ptr[2] = op1_ptr[2] * ia;
		res_ptr[3] = op1_ptr[3];
	} else {
		res_ptr[0] = op1_ptr[0];
		res_ptr[1] = op1_ptr[1];
		res_ptr[2] = op1_ptr[2];
		res_ptr[3] = op1_ptr[3];
	}
}

static zend_always_inline void qb_do_remove_premultiplication_F64(float64_t *op1_ptr, uint32_t op1_count, float64_t *res_ptr, uint32_t res_count) {
	float64_t a = op1_ptr[3];
	if(a != 1) { 
		float64_t ia = (a != 0) ? 1 / a : 0;
		res_ptr[0] = op1_ptr[0] * ia;
		res_ptr[1] = op1_ptr[1] * ia;
		res_ptr[2] = op1_ptr[2] * ia;
		res_ptr[3] = op1_ptr[3];
	} else {
		res_ptr[0] = op1_ptr[0];
		res_ptr[1] = op1_ptr[1];
		res_ptr[2] = op1_ptr[2];
		res_ptr[3] = op1_ptr[3];
	}
}

static zend_always_inline void qb_do_rgb2hsl_4x_F32(float32_t *op1_ptr, float32_t *res_ptr) {
	qb_convert_rgb_to_hsl_F32(op1_ptr, res_ptr);
	res_ptr[3] = op1_ptr[3];
}

static zend_always_inline void qb_do_rgb2hsl_4x_F64(float64_t *op1_ptr, float64_t *res_ptr) {
	qb_convert_rgb_to_hsl_F64(op1_ptr, res_ptr);
	res_ptr[3] = op1_ptr[3];
}

static zend_always_inline void qb_do_rgb2hsv_4x_F32(float32_t *op1_ptr, float32_t *res_ptr) {
	qb_convert_rgb_to_hsv_F32(op1_ptr, res_ptr);
	res_ptr[3] = op1_ptr[3];
}

static zend_always_inline void qb_do_rgb2hsv_4x_F64(float64_t *op1_ptr, float64_t *res_ptr) {
	qb_convert_rgb_to_hsv_F64(op1_ptr, res_ptr);
	res_ptr[3] = op1_ptr[3];
}

static zend_always_inline void qb_do_sample_nearest_3x_F32(float32_t *op1_ptr, uint32_t op2, uint32_t op3, float32_t op4, float32_t op5, float32_t *res_ptr) {
	int32_t ix = qb_quick_floorf(op4);
	int32_t iy = qb_quick_floorf(op5);
	if(((uint32_t) ix < op2) && ((uint32_t) iy < op3)) {
		uint32_t index = ((iy * op2) + ix) * 3;
		res_ptr[0] = op1_ptr[index + 0];
		res_ptr[1] = op1_ptr[index + 1];
		res_ptr[2] = op1_ptr[index + 2];
	} else {
		res_ptr[0] = 0;
		res_ptr[1] = 0;
		res_ptr[2] = 0;
	}
}

static zend_always_inline void qb_do_sample_nearest_4x_F32(float32_t *op1_ptr, uint32_t op2, uint32_t op3, float32_t op4, float32_t op5, float32_t *res_ptr) {
	int32_t ix = qb_quick_floorf(op4);
	int32_t iy = qb_quick_floorf(op5);
	if(((uint32_t) ix < op2) && ((uint32_t) iy < op3)) {
		uint32_t index = ((iy * op2) + ix) * 4;
		res_ptr[0] = op1_ptr[index + 0];
		res_ptr[1] = op1_ptr[index + 1];
		res_ptr[2] = op1_ptr[index + 2];
		res_ptr[3] = op1_ptr[index + 3];
	} else {
		res_ptr[0] = 0;
		res_ptr[1] = 0;
		res_ptr[2] = 0;
		res_ptr[3] = 0;
	}
}

static zend_always_inline void qb_do_sign_F32(float32_t op1, float32_t *res_ptr) {
	if(op1 < 0) {
		(*res_ptr) = -1;
	} else if(op1 > 0) {
		(*res_ptr) = 1;
	} else {
		(*res_ptr) = 0;
	}
}

static zend_always_inline void qb_do_sign_F64(float64_t op1, float64_t *res_ptr) {
	if(op1 < 0) {
		(*res_ptr) = -1;
	} else if(op1 > 0) {
		(*res_ptr) = 1;
	} else {
		(*res_ptr) = 0;
	}
}

static zend_always_inline void qb_do_smooth_step_F32(float32_t op1, float32_t op2, float32_t op3, float32_t *res_ptr) {
	if(op3 <= op1) {
		(*res_ptr) = 0;
	} else if(op3 >= op2) {
		(*res_ptr) = 1;
	} else {
		float32_t value = (op3 - op1) / (op2 - op1);
		(*res_ptr) = value * value * (3 - 2 * value);
	}
}

static zend_always_inline void qb_do_smooth_step_F64(float64_t op1, float64_t op2, float64_t op3, float64_t *res_ptr) {
	if(op3 <= op1) {
		(*res_ptr) = 0;
	} else if(op3 >= op2) {
		(*res_ptr) = 1;
	} else {
		float64_t value = (op3 - op1) / (op2 - op1);
		(*res_ptr) = value * value * (3 - 2 * value);
	}
}

static zend_always_inline void qb_do_transform_vector_cm_2x_F32(float32_t *op1_ptr, float32_t *op2_ptr, float32_t *res_ptr) {
	float32_t dot_product0 = (op1_ptr[0 * 2 + 0] * op2_ptr[0]) + (op1_ptr[1 * 2 + 0] * op2_ptr[1]) + op1_ptr[2 * 2 + 0];
	float32_t dot_product1 = (op1_ptr[0 * 2 + 1] * op2_ptr[0]) + (op1_ptr[1 * 2 + 1] * op2_ptr[1]) + op1_ptr[2 * 2 + 1];
	res_ptr[0] = dot_product0;
	res_ptr[1] = dot_product1;
}

static zend_always_inline void qb_do_transform_vector_cm_3x_F32(float32_t *op1_ptr, float32_t *op2_ptr, float32_t *res_ptr) {
	float32_t dot_product0 = (op1_ptr[0 * 3 + 0] * op2_ptr[0]) + (op1_ptr[1 * 3 + 0] * op2_ptr[1]) + (op1_ptr[2 * 3 + 0] * op2_ptr[2]) + op1_ptr[3 * 3 + 0];
	float32_t dot_product1 = (op1_ptr[0 * 3 + 1] * op2_ptr[0]) + (op1_ptr[1 * 3 + 1] * op2_ptr[1]) + (op1_ptr[2 * 3 + 1] * op2_ptr[2]) + op1_ptr[3 * 3 + 1];
	float32_t dot_product2 = (op1_ptr[0 * 3 + 2] * op2_ptr[0]) + (op1_ptr[1 * 3 + 2] * op2_ptr[1]) + (op1_ptr[2 * 3 + 2] * op2_ptr[2]) + op1_ptr[3 * 3 + 2];
	res_ptr[0] = dot_product0;
	res_ptr[1] = dot_product1;
	res_ptr[2] = dot_product2;
}

static zend_always_inline void qb_do_transform_vector_cm_4x_F32(float32_t *op1_ptr, float32_t *op2_ptr, float32_t *res_ptr) {
	float32_t dot_product0 = (op1_ptr[0 * 4 + 0] * op2_ptr[0]) + (op1_ptr[1 * 4 + 0] * op2_ptr[1]) + (op1_ptr[2 * 4 + 0] * op2_ptr[2]) + (op1_ptr[3 * 4 + 0] * op2_ptr[3]) + op1_ptr[4 * 4 + 0];
	float32_t dot_product1 = (op1_ptr[0 * 4 + 1] * op2_ptr[0]) + (op1_ptr[1 * 4 + 1] * op2_ptr[1]) + (op1_ptr[2 * 4 + 1] * op2_ptr[2]) + (op1_ptr[3 * 4 + 1] * op2_ptr[3]) + op1_ptr[4 * 4 + 1];
	float32_t dot_product2 = (op1_ptr[0 * 4 + 2] * op2_ptr[0]) + (op1_ptr[1 * 4 + 2] * op2_ptr[1]) + (op1_ptr[2 * 4 + 2] * op2_ptr[2]) + (op1_ptr[3 * 4 + 2] * op2_ptr[3]) + op1_ptr[4 * 4 + 2];
	float32_t dot_product3 = (op1_ptr[0 * 4 + 3] * op2_ptr[0]) + (op1_ptr[1 * 4 + 3] * op2_ptr[1]) + (op1_ptr[2 * 4 + 3] * op2_ptr[2]) + (op1_ptr[3 * 4 + 3] * op2_ptr[3]) + op1_ptr[4 * 4 + 3];
	res_ptr[0] = dot_product0;
	res_ptr[1] = dot_product1;
	res_ptr[2] = dot_product2;
	res_ptr[3] = dot_product3;
}

static zend_always_inline void qb_do_transform_vector_rm_2x_F32(float32_t *op1_ptr, float32_t *op2_ptr, float32_t *res_ptr) {
	float32_t dot_product0 = (op1_ptr[0 * 3 + 0] * op2_ptr[0]) + (op1_ptr[0 * 3 + 1] * op2_ptr[1]) + op1_ptr[0 * 3 + 2];
	float32_t dot_product1 = (op1_ptr[1 * 3 + 0] * op2_ptr[0]) + (op1_ptr[1 * 3 + 1] * op2_ptr[1]) + op1_ptr[1 * 3 + 2];
	res_ptr[0] = dot_product0;
	res_ptr[1] = dot_product1;
}

static zend_always_inline void qb_do_transform_vector_rm_3x_F32(float32_t *op1_ptr, float32_t *op2_ptr, float32_t *res_ptr) {
	float32_t dot_product0 = (op1_ptr[0 * 4 + 0] * op2_ptr[0]) + (op1_ptr[0 * 4 + 1] * op2_ptr[1]) + (op1_ptr[0 * 4 + 2] * op2_ptr[2]) + op1_ptr[0 * 4 + 3];
	float32_t dot_product1 = (op1_ptr[1 * 4 + 0] * op2_ptr[0]) + (op1_ptr[1 * 4 + 1] * op2_ptr[1]) + (op1_ptr[1 * 4 + 2] * op2_ptr[2]) + op1_ptr[1 * 4 + 3];
	float32_t dot_product2 = (op1_ptr[2 * 4 + 0] * op2_ptr[0]) + (op1_ptr[2 * 4 + 1] * op2_ptr[1]) + (op1_ptr[2 * 4 + 2] * op2_ptr[2]) + op1_ptr[2 * 4 + 3];
	res_ptr[0] = dot_product0;
	res_ptr[1] = dot_product1;
	res_ptr[2] = dot_product2;
}

static zend_always_inline void qb_do_transform_vector_rm_4x_F32(float32_t *op1_ptr, float32_t *op2_ptr, float32_t *res_ptr) {
	float32_t dot_product0 = (op1_ptr[0 * 5 + 0] * op2_ptr[0]) + (op1_ptr[0 * 5 + 1] * op2_ptr[1]) + (op1_ptr[0 * 5 + 2] * op2_ptr[2]) + (op1_ptr[0 * 5 + 3] * op2_ptr[3]) + op1_ptr[0 * 5 + 4];
	float32_t dot_product1 = (op1_ptr[1 * 5 + 0] * op2_ptr[0]) + (op1_ptr[1 * 5 + 1] * op2_ptr[1]) + (op1_ptr[1 * 5 + 2] * op2_ptr[2]) + (op1_ptr[1 * 5 + 3] * op2_ptr[3]) + op1_ptr[1 * 5 + 4];
	float32_t dot_product2 = (op1_ptr[2 * 5 + 0] * op2_ptr[0]) + (op1_ptr[2 * 5 + 1] * op2_ptr[1]) + (op1_ptr[2 * 5 + 2] * op2_ptr[2]) + (op1_ptr[2 * 5 + 3] * op2_ptr[3]) + op1_ptr[2 * 5 + 4];
	float32_t dot_product3 = (op1_ptr[3 * 5 + 0] * op2_ptr[0]) + (op1_ptr[3 * 5 + 1] * op2_ptr[1]) + (op1_ptr[3 * 5 + 2] * op2_ptr[2]) + (op1_ptr[3 * 5 + 3] * op2_ptr[3]) + op1_ptr[3 * 5 + 4];
	res_ptr[0] = dot_product0;
	res_ptr[1] = dot_product1;
	res_ptr[2] = dot_product2;
	res_ptr[3] = dot_product3;
}

static zend_always_inline void qb_do_transpose_matrix_2x_F32(float32_t *op1_ptr, float32_t *res_ptr) {
	float32_t m00 = op1_ptr[0 * 2 + 0];
	float32_t m10 = op1_ptr[0 * 2 + 1];
	float32_t m01 = op1_ptr[1 * 2 + 0];
	float32_t m11 = op1_ptr[1 * 2 + 1];
	res_ptr[0 * 2 + 0] = m00;
	res_ptr[0 * 2 + 1] = m01;
	res_ptr[1 * 2 + 0] = m10;
	res_ptr[1 * 2 + 1] = m11;
}

