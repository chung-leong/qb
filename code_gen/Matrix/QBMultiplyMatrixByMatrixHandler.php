<?php

class QBMultiplyMatrixByMatrixHandler extends QBSIMDHandler {

	public function getHelperFunctions() {
		$type = $this->getOperandType(1);
		$cType = $this->getOperandCType(1);
		$functions = array(
			array(
				"static void ZEND_FASTCALL qb_multiply_matrix_by_matrix_2x2_$type($cType *m1, $cType *m2, $cType *res_ptr) {",
					"$cType dot_product0 = (m1[0 + 0] * m2[0 + 0]) + (m1[1 + 0] * m2[2 + 0]);",
					"$cType dot_product1 = (m1[0 + 0] * m2[0 + 1]) + (m1[1 + 0] * m2[2 + 1]);",
					"$cType dot_product2 = (m1[0 + 2] * m2[0 + 0]) + (m1[1 + 2] * m2[2 + 0]);",
					"$cType dot_product3 = (m1[0 + 2] * m2[0 + 1]) + (m1[1 + 2] * m2[2 + 1]);",
					"res_ptr[0 + 0] = dot_product0;",
					"res_ptr[0 + 1] = dot_product1;",
					"res_ptr[2 + 0] = dot_product2;",
					"res_ptr[2 + 1] = dot_product3;",
				"}",
			),
			array(
				"static void ZEND_FASTCALL qb_multiply_matrix_by_matrix_3x3_$type($cType *m1, $cType *m2, $cType *res_ptr) {",
					"$cType dot_product0 = (m1[0 + 0] * m2[0 + 0]) + (m1[1 + 0] * m2[3 + 0]) + (m1[2 + 0] * m2[6 + 0]);",
					"$cType dot_product1 = (m1[0 + 0] * m2[0 + 1]) + (m1[1 + 0] * m2[3 + 1]) + (m1[2 + 0] * m2[6 + 1]);",
					"$cType dot_product2 = (m1[0 + 0] * m2[0 + 2]) + (m1[1 + 0] * m2[3 + 2]) + (m1[2 + 0] * m2[6 + 2]);",
					"$cType dot_product3 = (m1[0 + 3] * m2[0 + 0]) + (m1[1 + 3] * m2[3 + 0]) + (m1[2 + 3] * m2[6 + 0]);",
					"$cType dot_product4 = (m1[0 + 3] * m2[0 + 1]) + (m1[1 + 3] * m2[3 + 1]) + (m1[2 + 3] * m2[6 + 1]);",
					"$cType dot_product5 = (m1[0 + 3] * m2[0 + 2]) + (m1[1 + 3] * m2[3 + 2]) + (m1[2 + 3] * m2[6 + 2]);",
					"$cType dot_product6 = (m1[0 + 6] * m2[0 + 0]) + (m1[1 + 6] * m2[3 + 0]) + (m1[2 + 6] * m2[6 + 0]);",
					"$cType dot_product7 = (m1[0 + 6] * m2[0 + 1]) + (m1[1 + 6] * m2[3 + 1]) + (m1[2 + 6] * m2[6 + 1]);",
					"$cType dot_product8 = (m1[0 + 6] * m2[0 + 2]) + (m1[1 + 6] * m2[3 + 2]) + (m1[2 + 6] * m2[6 + 2]);",
					"res_ptr[0 + 0] = dot_product0;",
					"res_ptr[0 + 1] = dot_product1;",
					"res_ptr[0 + 2] = dot_product2;",
					"res_ptr[3 + 0] = dot_product3;",
					"res_ptr[3 + 1] = dot_product4;",
					"res_ptr[3 + 2] = dot_product5;",
					"res_ptr[6 + 0] = dot_product6;",
					"res_ptr[6 + 1] = dot_product7;",
					"res_ptr[6 + 2] = dot_product8;",
				"}",
			),
			array(
				"static void ZEND_FASTCALL qb_multiply_matrix_by_matrix_3x3_padded_$type($cType *m1, $cType *m2, $cType *res_ptr) {",
					"$cType dot_product0 = (m1[0 + 0] * m2[0 + 0]) + (m1[1 + 0] * m2[4 + 0]) + (m1[2 + 0] * m2[8 + 0]);",
					"$cType dot_product1 = (m1[0 + 0] * m2[0 + 1]) + (m1[1 + 0] * m2[4 + 1]) + (m1[2 + 0] * m2[8 + 1]);",
					"$cType dot_product2 = (m1[0 + 0] * m2[0 + 2]) + (m1[1 + 0] * m2[4 + 2]) + (m1[2 + 0] * m2[8 + 2]);",
					"$cType dot_product3 = (m1[0 + 4] * m2[0 + 0]) + (m1[1 + 4] * m2[4 + 0]) + (m1[2 + 4] * m2[8 + 0]);",
					"$cType dot_product4 = (m1[0 + 4] * m2[0 + 1]) + (m1[1 + 4] * m2[4 + 1]) + (m1[2 + 4] * m2[8 + 1]);",
					"$cType dot_product5 = (m1[0 + 4] * m2[0 + 2]) + (m1[1 + 4] * m2[4 + 2]) + (m1[2 + 4] * m2[8 + 2]);",
					"$cType dot_product6 = (m1[0 + 8] * m2[0 + 0]) + (m1[1 + 8] * m2[4 + 0]) + (m1[2 + 8] * m2[8 + 0]);",
					"$cType dot_product7 = (m1[0 + 8] * m2[0 + 1]) + (m1[1 + 8] * m2[4 + 1]) + (m1[2 + 8] * m2[8 + 1]);",
					"$cType dot_product8 = (m1[0 + 8] * m2[0 + 2]) + (m1[1 + 8] * m2[4 + 2]) + (m1[2 + 8] * m2[8 + 2]);",
					"res_ptr[0 + 0] = dot_product0;",
					"res_ptr[0 + 1] = dot_product1;",
					"res_ptr[0 + 2] = dot_product2;",
					"res_ptr[4 + 0] = dot_product3;",
					"res_ptr[4 + 1] = dot_product4;",
					"res_ptr[4 + 2] = dot_product5;",
					"res_ptr[8 + 0] = dot_product6;",
					"res_ptr[8 + 1] = dot_product7;",
					"res_ptr[8 + 2] = dot_product8;",
				"}",
			),
			array(
				"static void ZEND_FASTCALL qb_multiply_matrix_by_matrix_4x4_$type($cType *m1, $cType *m2, $cType *res_ptr) {",
					"$cType dot_product0 = (m1[0 + 0] * m2[0 + 0]) + (m1[1 + 0] * m2[4 + 0]) + (m1[2 + 0] * m2[8 + 0]) + (m1[3 + 0] * m2[12 + 0]);",
					"$cType dot_product1 = (m1[0 + 0] * m2[0 + 1]) + (m1[1 + 0] * m2[4 + 1]) + (m1[2 + 0] * m2[8 + 1]) + (m1[3 + 0] * m2[12 + 1]);",
					"$cType dot_product2 = (m1[0 + 0] * m2[0 + 2]) + (m1[1 + 0] * m2[4 + 2]) + (m1[2 + 0] * m2[8 + 2]) + (m1[3 + 0] * m2[12 + 2]);",
					"$cType dot_product3 = (m1[0 + 0] * m2[0 + 3]) + (m1[1 + 0] * m2[4 + 3]) + (m1[2 + 0] * m2[8 + 3]) + (m1[3 + 0] * m2[12 + 3]);",
					"$cType dot_product4 = (m1[0 + 4] * m2[0 + 0]) + (m1[1 + 4] * m2[4 + 0]) + (m1[2 + 4] * m2[8 + 0]) + (m1[3 + 4] * m2[12 + 0]);",
					"$cType dot_product5 = (m1[0 + 4] * m2[0 + 1]) + (m1[1 + 4] * m2[4 + 1]) + (m1[2 + 4] * m2[8 + 1]) + (m1[3 + 4] * m2[12 + 1]);",
					"$cType dot_product6 = (m1[0 + 4] * m2[0 + 2]) + (m1[1 + 4] * m2[4 + 2]) + (m1[2 + 4] * m2[8 + 2]) + (m1[3 + 4] * m2[12 + 2]);",
					"$cType dot_product7 = (m1[0 + 4] * m2[0 + 3]) + (m1[1 + 4] * m2[4 + 3]) + (m1[2 + 4] * m2[8 + 3]) + (m1[3 + 4] * m2[12 + 3]);",
					"$cType dot_product8 = (m1[0 + 8] * m2[0 + 0]) + (m1[1 + 8] * m2[4 + 0]) + (m1[2 + 8] * m2[8 + 0]) + (m1[3 + 8] * m2[12 + 0]);",
					"$cType dot_product9 = (m1[0 + 8] * m2[0 + 1]) + (m1[1 + 8] * m2[4 + 1]) + (m1[2 + 8] * m2[8 + 1]) + (m1[3 + 8] * m2[12 + 1]);",
					"$cType dot_product10 = (m1[0 + 8] * m2[0 + 2]) + (m1[1 + 8] * m2[4 + 2]) + (m1[2 + 8] * m2[8 + 2]) + (m1[3 + 8] * m2[12 + 2]);",
					"$cType dot_product11 = (m1[0 + 8] * m2[0 + 3]) + (m1[1 + 8] * m2[4 + 3]) + (m1[2 + 8] * m2[8 + 3]) + (m1[3 + 8] * m2[12 + 3]);",
					"$cType dot_product12 = (m1[0 + 12] * m2[0 + 0]) + (m1[1 + 12] * m2[4 + 0]) + (m1[2 + 12] * m2[8 + 0]) + (m1[3 + 12] * m2[12 + 0]);",
					"$cType dot_product13 = (m1[0 + 12] * m2[0 + 1]) + (m1[1 + 12] * m2[4 + 1]) + (m1[2 + 12] * m2[8 + 1]) + (m1[3 + 12] * m2[12 + 1]);",
					"$cType dot_product14 = (m1[0 + 12] * m2[0 + 2]) + (m1[1 + 12] * m2[4 + 2]) + (m1[2 + 12] * m2[8 + 2]) + (m1[3 + 12] * m2[12 + 2]);",
					"$cType dot_product15 = (m1[0 + 12] * m2[0 + 3]) + (m1[1 + 12] * m2[4 + 3]) + (m1[2 + 12] * m2[8 + 3]) + (m1[3 + 12] * m2[12 + 3]);",
					"res_ptr[0 + 0] = dot_product0;",
					"res_ptr[0 + 1] = dot_product1;",
					"res_ptr[0 + 2] = dot_product2;",
					"res_ptr[0 + 3] = dot_product3;",
					"res_ptr[4 + 0] = dot_product4;",
					"res_ptr[4 + 1] = dot_product5;",
					"res_ptr[4 + 2] = dot_product6;",
					"res_ptr[4 + 3] = dot_product7;",
					"res_ptr[8 + 0] = dot_product8;",
					"res_ptr[8 + 1] = dot_product9;",
					"res_ptr[8 + 2] = dot_product10;",
					"res_ptr[8 + 3] = dot_product11;",
					"res_ptr[12 + 0] = dot_product12;",
					"res_ptr[12 + 1] = dot_product13;",
					"res_ptr[12 + 2] = dot_product14;",
					"res_ptr[12 + 3] = dot_product15;",
				"}",
			),
			array(
				"static void ZEND_FASTCALL qb_multiply_matrix_by_matrix_$type($cType *m1, uint32_t m1_row, uint32_t m1_col, $cType *m2, uint32_t m2_row, uint32_t m2_col, $cType *res_ptr) {",
					"uint32_t i, j, k, p, q, res_index = 0;",
					"ALLOCA_FLAG(use_heap)",
					"$cType *buffer = do_alloca(m1_row * m2_col * sizeof($cType), use_heap);",
					"for(i = 0, q = 0; i < m1_row; ++i) {",
						"for(j = 0; j < m2_col; ++j) {",
							"$cType dot_product = 0;",
							"for(p = 0, k = 0; p < m2_row; ++p, k += m2_col) {",
								"dot_product += m1[p + q] * m2[k + j];",
							"}",
							"buffer[res_index++] = dot_product;",
						"}",
						"q += m1_col;",
					"}",
					"memcpy(res_ptr, buffer, m1_row * m2_col * sizeof($cType));",
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
				case 2: return "(MATRIX2_ROWS * MATRIX2_COLS)";
				case 3: return "(MATRIX1_ROWS * MATRIX2_COLS)";
			}
		} else {
			return ($this->operandSize + $this->operandPadding) * $this->operandSize;
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
			$matrixSize1 = $this->getOperandSize(1); 
			$matrixSize2 = $this->getOperandSize(2);
			$outputSize = $this->getOperandSize(3);
			$lines[] = "matrix1_count = op1_count / $matrixSize1;";
			$lines[] = "matrix2_count = op2_count / $matrixSize2;";
			$lines[] = "mmult_res_count = ((matrix1_count > matrix2_count) ? matrix1_count : matrix2_count) * $outputSize;";
			return $lines;
		}
	}
	
	public function getSIMDExpression() {
		$type = $this->getOperandType(1);
		if($this->operandSize == "variable") {
			return "qb_multiply_matrix_by_matrix_$type(op1_ptr, MATRIX1_ROWS, MATRIX1_COLS, op2_ptr, MATRIX2_ROWS, MATRIX2_COLS, res_ptr);";
		} else {
			if($this->operandPadding) {
				return "qb_multiply_matrix_by_matrix_{$this->operandSize}x{$this->operandSize}_padded_$type(op1_ptr, op2_ptr, res_ptr);";
			} else {
				return "qb_multiply_matrix_by_matrix_{$this->operandSize}x{$this->operandSize}_$type(op1_ptr, op2_ptr, res_ptr);";
			}
		}
	}
}

?>