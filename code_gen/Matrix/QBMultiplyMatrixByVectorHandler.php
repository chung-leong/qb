<?php

class QBMultiplyMatrixByVectorHandler extends QBMultiplyMatrixByMatrixHandler {

	public function getHelperFunctions() {
		$type = $this->getOperandType(1);
		$cType = $this->getOperandCType(1);
		$functions = array(
			array(
				"static void ZEND_FASTCALL qb_multiply_matrix_by_vector_2x2_$type($cType *m, $cType *v, $cType *res_ptr) {",
					"$cType dot_product0 = (m[0 + 0] * v[0]) + (m[2 + 0] * v[1]);",
					"$cType dot_product1 = (m[0 + 1] * v[0]) + (m[2 + 1] * v[1]);",
					"res_ptr[0] = dot_product0;",
					"res_ptr[1] = dot_product1;",
				"}",
			),
			array(
				"static void ZEND_FASTCALL qb_multiply_matrix_by_vector_3x3_$type($cType *m, $cType *v, $cType *res_ptr) {",
					"$cType dot_product0 = (m[0 + 0] * v[0]) + (m[3 + 0] * v[1]) + (m[6 + 0] * v[2]);",
					"$cType dot_product1 = (m[0 + 1] * v[0]) + (m[3 + 1] * v[1]) + (m[6 + 1] * v[2]);",
					"$cType dot_product2 = (m[0 + 2] * v[0]) + (m[3 + 2] * v[1]) + (m[6 + 2] * v[2]);",
					"res_ptr[0] = dot_product0;",
					"res_ptr[1] = dot_product1;",
					"res_ptr[2] = dot_product2;",
				"}",
			),
			array(
				"static void ZEND_FASTCALL qb_multiply_matrix_by_vector_3x3_padded_$type($cType *m, $cType *v, $cType *res_ptr) {",
					"$cType dot_product0 = (m[0 + 0] * v[0]) + (m[4 + 0] * v[1]) + (m[8 + 0] * v[2]);",
					"$cType dot_product1 = (m[0 + 1] * v[0]) + (m[4 + 1] * v[1]) + (m[8 + 1] * v[2]);",
					"$cType dot_product2 = (m[0 + 2] * v[0]) + (m[4 + 2] * v[1]) + (m[8 + 2] * v[2]);",
					"res_ptr[0] = dot_product0;",
					"res_ptr[1] = dot_product1;",
					"res_ptr[2] = dot_product2;",
				"}",
			),
			array(
				"static void ZEND_FASTCALL qb_multiply_matrix_by_vector_4x4_$type($cType *m, $cType *v, $cType *res_ptr) {",
					"$cType dot_product0 = (m[0 + 0] * v[0]) + (m[4 + 0] * v[1]) + (m[8 + 0] * v[2]) + (m[12 + 0] * v[3]);",
					"$cType dot_product1 = (m[0 + 1] * v[0]) + (m[4 + 1] * v[1]) + (m[8 + 1] * v[2]) + (m[12 + 1] * v[3]);",
					"$cType dot_product2 = (m[0 + 2] * v[0]) + (m[4 + 2] * v[1]) + (m[8 + 2] * v[2]) + (m[12 + 2] * v[3]);",
					"$cType dot_product3 = (m[0 + 3] * v[0]) + (m[4 + 3] * v[1]) + (m[8 + 3] * v[2]) + (m[12 + 3] * v[3]);",
					"res_ptr[0] = dot_product0;",
					"res_ptr[1] = dot_product1;",
					"res_ptr[2] = dot_product2;",
					"res_ptr[3] = dot_product3;",
				"}",
			),
			array(
				"static void ZEND_FASTCALL qb_multiply_matrix_by_vector_$type($cType *m, uint32_t m_row, uint32_t m_col, $cType *v, uint32_t v_row, $cType *res_ptr) {",
					"uint32_t i, j, k;",
					"$cType buffer[4096];",
					"for(i = 0, k = 0; i < m_row; ++i) {",
						"$cType dot_product = 0;",
						"for(j = 0; j < m_col; ++j) {",
							"dot_product += m[k++] * v[j];",
						"}",
						"buffer[i] = dot_product;",
					"}",
					"memcpy(res_ptr, buffer, m_row * sizeof($cType));",
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
	
	public function getSIMDExpression() {
		$type = $this->getOperandType(1);
		if($this->operandSize == "variable") {
			return "qb_multiply_matrix_by_vector_$type(op1_ptr, MATRIX1_ROWS, MATRIX1_COLS, op2_ptr, MATRIX2_ROWS, res_ptr);";
		} else {
			if($this->operandPadding) {
				return "qb_multiply_matrix_by_vector_{$this->operandSize}x{$this->operandSize}_padded_$type(op1_ptr, op2_ptr, res_ptr);";
			} else {
				return "qb_multiply_matrix_by_vector_{$this->operandSize}x{$this->operandSize}_$type(op1_ptr, op2_ptr, res_ptr);";
			}
		}
	}
}

?>