<?php

// note: using column-major order

class QBMultiplyMatrixByVectorHandler extends QBMultiplyMatrixByMatrixHandler {

	public function getInputOperandCount() {
		return 2;
	}

	public function getHelperFunctions() {
		$type = $this->getOperandType(1);
		$cType = $this->getOperandCType(1);
		$functions = array(
			array(
				"void ZEND_FASTCALL qb_multiply_cm_matrix_by_vector_2x2_$type($cType *op1_start, $cType *op1_end, $cType *op2_start, $cType *op2_end, $cType *res_start, $cType *res_end) {",
					"$cType *__restrict res_ptr = res_start;",
					"$cType *__restrict op1_ptr = op1_start;",
					"$cType *__restrict op2_ptr = op2_start;",
					"for(;;) {",
						"$cType dot_product0 = (op1_ptr[0 * 2 + 0] * op2_ptr[0]) + (op1_ptr[1 * 2 + 0] * op2_ptr[1]);",
						"$cType dot_product1 = (op1_ptr[0 * 2 + 1] * op2_ptr[0]) + (op1_ptr[1 * 2 + 1] * op2_ptr[1]);",
						"res_ptr[0] = dot_product0;",
						"res_ptr[1] = dot_product1;",
						"res_ptr += 2;",
						"if(res_ptr >= res_end) {",
							"break;",
						"}",
						"op1_ptr += 4;",
						"if(op1_ptr >= op1_end) {",
							"op1_ptr = op1_start;",
						"}",
						"op2_ptr += 2;",
						"if(op2_ptr >= op2_end) {",
							"op2_ptr = op2_start;",
						"}",
					"}",
				"}",
			),
			array(
				"void ZEND_FASTCALL qb_multiply_cm_matrix_by_vector_3x3_$type($cType *op1_start, $cType *op1_end, $cType *op2_start, $cType *op2_end, $cType *res_start, $cType *res_end) {",
					"$cType *__restrict res_ptr = res_start;",
					"$cType *__restrict op1_ptr = op1_start;",
					"$cType *__restrict op2_ptr = op2_start;",
					"for(;;) {",
						"$cType dot_product0 = (op1_ptr[0 * 3 + 0] * op2_ptr[0]) + (op1_ptr[1 * 3 + 0] * op2_ptr[1]) + (op1_ptr[2 * 3 + 0] * op2_ptr[2]);",
						"$cType dot_product1 = (op1_ptr[0 * 3 + 1] * op2_ptr[0]) + (op1_ptr[1 * 3 + 1] * op2_ptr[1]) + (op1_ptr[2 * 3 + 1] * op2_ptr[2]);",
						"$cType dot_product2 = (op1_ptr[0 * 3 + 2] * op2_ptr[0]) + (op1_ptr[1 * 3 + 2] * op2_ptr[1]) + (op1_ptr[2 * 3 + 2] * op2_ptr[2]);",
						"res_ptr[0] = dot_product0;",
						"res_ptr[1] = dot_product1;",
						"res_ptr[2] = dot_product2;",
						"res_ptr += 3;",
						"if(res_ptr >= res_end) {",
							"break;",
						"}",
						"op1_ptr += 9;",
						"if(op1_ptr >= op1_end) {",
							"op1_ptr = op1_start;",
						"}",
						"op2_ptr += 3;",
						"if(op2_ptr >= op2_end) {",
							"op2_ptr = op2_start;",
						"}",
					"}",
				"}",
			),
			array(
				"void ZEND_FASTCALL qb_multiply_cm_matrix_by_vector_3x3_padded_$type($cType *op1_start, $cType *op1_end, $cType *op2_start, $cType *op2_end, $cType *res_start, $cType *res_end) {",
					"$cType *__restrict res_ptr = res_start;",
					"$cType *__restrict op1_ptr = op1_start;",
					"$cType *__restrict op2_ptr = op2_start;",
					"for(;;) {",
						"$cType dot_product0 = (op1_ptr[0 * 4 + 0] * op2_ptr[0]) + (op1_ptr[1 * 4 + 0] * op2_ptr[1]) + (op1_ptr[2 * 4 + 0] * op2_ptr[2]);",
						"$cType dot_product1 = (op1_ptr[0 * 4 + 1] * op2_ptr[0]) + (op1_ptr[1 * 4 + 1] * op2_ptr[1]) + (op1_ptr[2 * 4 + 1] * op2_ptr[2]);",
						"$cType dot_product2 = (op1_ptr[0 * 4 + 2] * op2_ptr[0]) + (op1_ptr[1 * 4 + 2] * op2_ptr[1]) + (op1_ptr[2 * 4 + 2] * op2_ptr[2]);",
						"res_ptr[0] = dot_product0;",
						"res_ptr[1] = dot_product1;",
						"res_ptr[2] = dot_product2;",
						"res_ptr += 4;",
						"if(res_ptr >= res_end) {",
							"break;",
						"}",
						"op1_ptr += 12;",
						"if(op1_ptr >= op1_end) {",
							"op1_ptr = op1_start;",
						"}",
						"op2_ptr += 4;",
						"if(op2_ptr >= op2_end) {",
							"op2_ptr = op2_start;",
						"}",
					"}",
				"}",
			),
			array(
				"void ZEND_FASTCALL qb_multiply_cm_matrix_by_vector_4x4_$type($cType *op1_start, $cType *op1_end, $cType *op2_start, $cType *op2_end, $cType *res_start, $cType *res_end) {",
					"$cType *__restrict res_ptr = res_start;",
					"$cType *__restrict op1_ptr = op1_start;",
					"$cType *__restrict op2_ptr = op2_start;",
					"for(;;) {",
						"$cType dot_product0 = (op1_ptr[0 * 4 + 0] * op2_ptr[0]) + (op1_ptr[1 * 4 + 0] * op2_ptr[1]) + (op1_ptr[2 * 4 + 0] * op2_ptr[2]) + (op1_ptr[3 * 4 + 0] * op2_ptr[3]);",
						"$cType dot_product1 = (op1_ptr[0 * 4 + 1] * op2_ptr[0]) + (op1_ptr[1 * 4 + 1] * op2_ptr[1]) + (op1_ptr[2 * 4 + 1] * op2_ptr[2]) + (op1_ptr[3 * 4 + 1] * op2_ptr[3]);",
						"$cType dot_product2 = (op1_ptr[0 * 4 + 2] * op2_ptr[0]) + (op1_ptr[1 * 4 + 2] * op2_ptr[1]) + (op1_ptr[2 * 4 + 2] * op2_ptr[2]) + (op1_ptr[3 * 4 + 2] * op2_ptr[3]);",
						"$cType dot_product3 = (op1_ptr[0 * 4 + 3] * op2_ptr[0]) + (op1_ptr[1 * 4 + 3] * op2_ptr[1]) + (op1_ptr[2 * 4 + 3] * op2_ptr[2]) + (op1_ptr[3 * 4 + 3] * op2_ptr[3]);",
						"res_ptr[0] = dot_product0;",
						"res_ptr[1] = dot_product1;",
						"res_ptr[2] = dot_product2;",
						"res_ptr[3] = dot_product3;",
						"res_ptr += 4;",
						"if(res_ptr >= res_end) {",
							"break;",
						"}",
						"op1_ptr += 16;",
						"if(op1_ptr >= op1_end) {",
							"op1_ptr = op1_start;",
						"}",
						"op2_ptr += 4;",
						"if(op2_ptr >= op2_end) {",
							"op2_ptr = op2_start;",
						"}",
					"}",
				"}",
			),
			array(
				"void ZEND_FASTCALL qb_multiply_cm_matrix_by_vector_$type($cType *op1_start, $cType *op1_end, uint32_t m_row, uint32_t m_col, $cType *op2_start, $cType *op2_end, uint32_t v_row, $cType *res_start, $cType *res_end) {",
					"ALLOCA_FLAG(use_heap)",
					"$cType *buffer = do_alloca(m_row * sizeof($cType), use_heap);",
					"$cType *__restrict res_ptr = res_start;",
					"$cType *__restrict op1_ptr = op1_start;",
					"$cType *__restrict op2_ptr = op2_start;",
					"for(;;) {",
						"uint32_t i, j, k;",
						"for(i = 0; i < m_row; ++i) {",
							"$cType dot_product = 0;",
							"for(j = 0, k = i; j < m_col; ++j, k += m_col) {",
								"dot_product += op1_ptr[k] * op2_ptr[j];",
							"}",
							"buffer[i] = dot_product;",
						"}",
						"memcpy(res_ptr, buffer, m_row * sizeof($cType));",
						"res_ptr += m_row;",
						"if(res_ptr >= res_end) {",
							"break;",
						"}",
						"op1_ptr += m_row * m_col;",
						"if(op1_ptr >= op1_end) {",
							"op1_ptr = op1_start;",
						"}",
						"op2_ptr += v_row;",
						"if(op2_ptr >= op2_end) {",
							"op2_ptr = op2_start;",
						"}",
					"}",
					"free_alloca(buffer, use_heap);",
				"}",
			),
		);
		return $functions;
	}
	
	public function getOperandSize($i) {
		if($this->operandSize == "variable") {
			switch($i) {
				case 1: return "(MATRIX1_ROWS * MATRIX1_COLS)";
				case 2: return "(MATRIX2_ROWS * 1)";
				case 3: return "(MATRIX1_ROWS * 1)";
			}
		} else {
			switch($i) {
				case 1: return ($this->operandSize + $this->operandPadding) * $this->operandSize;
				case 2: return $this->operandSize;
				case 3: return $this->operandSize;
			}
		}
	}
	
	public function getAction() {
		$type = $this->getOperandType(1);
		if($this->operandSize == "variable") {
			if($this->addressMode == "ARR") {
				return "qb_multiply_cm_matrix_by_vector_$type(op1_ptr, op1_ptr + op1_count, MATRIX1_ROWS, MATRIX1_COLS, op2_ptr, op2_ptr + op2_count, MATRIX2_ROWS, res_ptr, res_ptr + res_count);";
			} else {
				return "qb_multiply_cm_matrix_by_vector_$type(op1_ptr, NULL, MATRIX1_ROWS, MATRIX1_COLS, op2_ptr, NULL, MATRIX2_ROWS, res_ptr, NULL);";
			}
		} else {
			if($this->operandPadding) {
				if($this->addressMode == "ARR") {
					return "qb_multiply_cm_matrix_by_vector_{$this->operandSize}x{$this->operandSize}_padded_$type(op1_ptr, op1_ptr + op1_count, op2_ptr, op2_ptr + op2_count, res_ptr, res_ptr + res_count);";
				} else {
					return "qb_multiply_cm_matrix_by_vector_{$this->operandSize}x{$this->operandSize}_padded_$type(op1_ptr, NULL, op2_ptr, NULL, res_ptr, NULL);";
				}
			} else {
				if($this->addressMode == "ARR") {
					return "qb_multiply_cm_matrix_by_vector_{$this->operandSize}x{$this->operandSize}_$type(op1_ptr, op1_ptr + op1_count, op2_ptr, op2_ptr + op2_count, res_ptr, res_ptr + res_count);";
				} else {
					return "qb_multiply_cm_matrix_by_vector_{$this->operandSize}x{$this->operandSize}_$type(op1_ptr, NULL, op2_ptr, NULL, res_ptr, NULL);";
				}
			}
		}
	}
}

?>