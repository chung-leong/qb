<?php

class QBMultiplyVectorByMatrixHandler extends QBSIMDHandler {

	public function getHelperFunctions() {
		$type = $this->getOperandType(1);
		$cType = $this->getOperandCType(1);
		$functions = array(
			array(
				"static void ZEND_FASTCALL qb_multiply_vector_by_matrix_1x1_$type($cType *v, $cType *m, $cType *res_ptr) {",
					"res_ptr[0] = v[0] * m[0];",
				"}",
			),
			array(
				"static void ZEND_FASTCALL qb_multiply_vector_by_matrix_2x2_$type($cType *v, $cType *m, $cType *res_ptr) {",
					"$cType dot_product0 = (v[0] * m[0 + 0]) + (v[1] * m[1 + 0]);",
					"$cType dot_product1 = (v[0] * m[0 + 2]) + (v[1] * m[1 + 2]);",
					"res_ptr[0] = dot_product0;",
					"res_ptr[1] = dot_product1;",
				"}",
			),
			array(
				"static void ZEND_FASTCALL qb_multiply_vector_by_matrix_3x3_$type($cType *v, $cType *m, $cType *res_ptr) {",
					"$cType dot_product0 = (v[0] * m[0 + 0]) + (v[1] * m[1 + 0]) + (v[2] * m[2 + 0]);",
					"$cType dot_product1 = (v[0] * m[0 + 3]) + (v[1] * m[1 + 3]) + (v[2] * m[2 + 3]);",
					"$cType dot_product2 = (v[0] * m[0 + 6]) + (v[1] * m[1 + 6]) + (v[2] * m[2 + 6]);",
					"res_ptr[0] = dot_product0;",
					"res_ptr[1] = dot_product1;",
					"res_ptr[2] = dot_product2;",
				"}",
			),
			array(
				"static void ZEND_FASTCALL qb_multiply_vector_by_matrix_3x3_padded_$type($cType *v, $cType *m, $cType *res_ptr) {",
					"$cType dot_product0 = (v[0] * m[0 + 0]) + (v[1] * m[1 + 0]) + (v[2] * m[2 + 0]);",
					"$cType dot_product1 = (v[0] * m[0 + 4]) + (v[1] * m[1 + 4]) + (v[2] * m[2 + 4]);",
					"$cType dot_product2 = (v[0] * m[0 + 8]) + (v[1] * m[1 + 8]) + (v[2] * m[2 + 8]);",
					"res_ptr[0] = dot_product0;",
					"res_ptr[1] = dot_product1;",
					"res_ptr[2] = dot_product2;",
				"}",
			),
			array(
				"static void ZEND_FASTCALL qb_multiply_vector_by_matrix_4x4_$type($cType *v, $cType *m, $cType *res_ptr) {",
					"$cType dot_product0 = (v[0] * m[0 + 0]) + (v[1] * m[1 + 0]) + (v[2] * m[2 + 0]) + (v[3] * m[3 + 0]);",
					"$cType dot_product1 = (v[0] * m[0 + 4]) + (v[1] * m[1 + 4]) + (v[2] * m[2 + 4]) + (v[3] * m[3 + 4]);",
					"$cType dot_product2 = (v[0] * m[0 + 8]) + (v[1] * m[1 + 8]) + (v[2] * m[2 + 8]) + (v[3] * m[3 + 8]);",
					"$cType dot_product3 = (v[0] * m[0 + 12]) + (v[1] * m[1 + 12]) + (v[2] * m[2 + 12]) + (v[3] * m[3 + 12]);",
					"res_ptr[0] = dot_product0;",
					"res_ptr[1] = dot_product1;",
					"res_ptr[2] = dot_product2;",
					"res_ptr[3] = dot_product3;",
				"}",
			),
			array(
				"static void ZEND_FASTCALL qb_multiply_vector_by_matrix_$type($cType *v, uint32_t v_col, $cType *m, uint32_t m_row, uint32_t m_col, $cType *res_ptr) {",
					"uint32_t i, j, k;",
					"$cType buffer[4096];",
					"for(i = 0; i < m_col; ++i) {",
						"$cType dot_product = 0;",
						"for(j = 0, k = 0; j < m_row; ++j) {",
							"dot_product += v[j] * m[k + i];",
							"k += m_col;",
						"}",
						"buffer[i] = dot_product;",
					"}",
					"memcpy(res_ptr, buffer, m_col * sizeof($cType));",
				"}",
			),
		);
		return $functions;
	}

	public function getOperandSize($i) {
		if($this->operandSize == "variable") {
			switch($i) {
				case 1: return "VECTOR_SIZE";
				case 2: return "MATRIX1_SIZE";
				case 3: return "MATRIX1_COLS";
			}
		} else {
			switch($i) {
				case 1: return $this->operandSize + $this->operandPadding;
				case 2: return ($this->operandSize + $this->operandPadding) * $this->operandSize;
				case 3: return $this->operandSize + $this->operandPadding;
			}
		}
	}

	public function getResultSizePossibilities() {
		if($this->addressMode == "ARR") {
			return "mmult_res_count";
		}
	}
	
	public function getResultSizeCalculation() {
		if($this->addressMode == "ARR") {
			$lines = array();
			$vectorSize = $this->getOperandSize(1);
			$matrixSize = $this->getOperandSize(2); 
			$outputSize = $this->getOperandSize(3);
			$lines[] = "vector_count = op1_count / $vectorSize;";
			$lines[] = "matrix1_count = op2_count / $matrixSize;";
			$lines[] = "mmult_res_count = ((matrix1_count > vector_count) ? matrix1_count : vector_count) * $outputSize;";
			return $lines;
		}
	}
	
	public function getSIMDExpression() {
		$type = $this->getOperandType(1);
		if($this->operandSize == "variable") {
			return "qb_multiply_vector_by_matrix_$type(op1_ptr, VECTOR_SIZE, op2_ptr, MATRIX1_ROWS, MATRIX1_COLS, res_ptr);";
		} else {
			if($this->operandPadding) {
				return "qb_multiply_vector_by_matrix_{$this->operandSize}x{$this->operandSize}_padded_$type(op1_ptr, op2_ptr, res_ptr);";
			} else {
				return "qb_multiply_vector_by_matrix_{$this->operandSize}x{$this->operandSize}_$type(op1_ptr, op2_ptr, res_ptr);";
			}
		}
	}
}

?>