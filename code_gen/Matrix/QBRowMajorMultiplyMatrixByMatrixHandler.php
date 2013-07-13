<?php

class QBRowMajorMultiplyMatrixByMatrixHandler extends QBSIMDHandler {

	public function getHelperFunctions() {
		$type = $this->getOperandType(1);
		$cType = $this->getOperandCType(1);
		$functions = array(
			array(
				"static void ZEND_FASTCALL qb_multiply_rm_matrix_by_matrix_2x2_$type($cType *op1_start, $cType *op1_end, $cType *op2_start, $cType *op2_end, $cType *res_start, $cType *res_end) {",
					"$cType *__restrict res_ptr = res_start;",
					"$cType *__restrict op1_ptr = op1_start;",
					"$cType *__restrict op2_ptr = op2_start;",
					"for(;;) {",
						"$cType dot_product0 = (op1_ptr[0 * 2 + 0] * op2_ptr[0 * 2 + 0]) + (op1_ptr[1 * 2 + 0] * op2_ptr[0 * 2 + 1]);",
						"$cType dot_product1 = (op1_ptr[0 * 2 + 0] * op2_ptr[1 * 2 + 0]) + (op1_ptr[1 * 2 + 0] * op2_ptr[1 * 2 + 1]);",
						"$cType dot_product2 = (op1_ptr[0 * 2 + 1] * op2_ptr[0 * 2 + 0]) + (op1_ptr[1 * 2 + 1] * op2_ptr[0 * 2 + 1]);",
						"$cType dot_product3 = (op1_ptr[0 * 2 + 1] * op2_ptr[1 * 2 + 0]) + (op1_ptr[1 * 2 + 1] * op2_ptr[1 * 2 + 1]);",
						"res_ptr[0 * 2 + 0] = dot_product0;",
						"res_ptr[1 * 2 + 0] = dot_product1;",
						"res_ptr[0 * 2 + 1] = dot_product2;",
						"res_ptr[1 * 2 + 1] = dot_product3;",
						"res_ptr += 4;",
						"if(res_ptr >= res_end) {",
							"break;",
						"}",
						"op1_ptr += 4;",
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
				"static void ZEND_FASTCALL qb_multiply_rm_matrix_by_matrix_3x3_$type($cType *op1_start, $cType *op1_end, $cType *op2_start, $cType *op2_end, $cType *res_start, $cType *res_end) {",
					"$cType *__restrict res_ptr = res_start;",
					"$cType *__restrict op1_ptr = op1_start;",
					"$cType *__restrict op2_ptr = op2_start;",
					"for(;;) {",
						"$cType dot_product0 = (op1_ptr[0 * 3 + 0] * op2_ptr[0 * 3 + 0]) + (op1_ptr[1 * 3 + 0] * op2_ptr[0 * 3 + 1]) + (op1_ptr[2 * 3 + 0] * op2_ptr[0 * 3 + 2]);",
						"$cType dot_product1 = (op1_ptr[0 * 3 + 0] * op2_ptr[1 * 3 + 0]) + (op1_ptr[1 * 3 + 0] * op2_ptr[1 * 3 + 1]) + (op1_ptr[2 * 3 + 0] * op2_ptr[1 * 3 + 2]);",
						"$cType dot_product2 = (op1_ptr[0 * 3 + 0] * op2_ptr[2 * 3 + 0]) + (op1_ptr[1 * 3 + 0] * op2_ptr[2 * 3 + 1]) + (op1_ptr[2 * 3 + 0] * op2_ptr[2 * 3 + 2]);",
						"$cType dot_product3 = (op1_ptr[0 * 3 + 1] * op2_ptr[0 * 3 + 0]) + (op1_ptr[1 * 3 + 1] * op2_ptr[0 * 3 + 1]) + (op1_ptr[2 * 3 + 1] * op2_ptr[0 * 3 + 2]);",
						"$cType dot_product4 = (op1_ptr[0 * 3 + 1] * op2_ptr[1 * 3 + 0]) + (op1_ptr[1 * 3 + 1] * op2_ptr[1 * 3 + 1]) + (op1_ptr[2 * 3 + 1] * op2_ptr[1 * 3 + 2]);",
						"$cType dot_product5 = (op1_ptr[0 * 3 + 1] * op2_ptr[2 * 3 + 0]) + (op1_ptr[1 * 3 + 1] * op2_ptr[2 * 3 + 1]) + (op1_ptr[2 * 3 + 1] * op2_ptr[2 * 3 + 2]);",
						"$cType dot_product6 = (op1_ptr[0 * 3 + 2] * op2_ptr[0 * 3 + 0]) + (op1_ptr[1 * 3 + 2] * op2_ptr[0 * 3 + 1]) + (op1_ptr[2 * 3 + 2] * op2_ptr[0 * 3 + 2]);",
						"$cType dot_product7 = (op1_ptr[0 * 3 + 2] * op2_ptr[1 * 3 + 0]) + (op1_ptr[1 * 3 + 2] * op2_ptr[1 * 3 + 1]) + (op1_ptr[2 * 3 + 2] * op2_ptr[1 * 3 + 2]);",
						"$cType dot_product8 = (op1_ptr[0 * 3 + 2] * op2_ptr[2 * 3 + 0]) + (op1_ptr[1 * 3 + 2] * op2_ptr[2 * 3 + 1]) + (op1_ptr[2 * 3 + 2] * op2_ptr[2 * 3 + 2]);",
						"res_ptr[0 * 3 + 0] = dot_product0;",
						"res_ptr[1 * 3 + 0] = dot_product1;",
						"res_ptr[2 * 3 + 0] = dot_product2;",
						"res_ptr[0 * 3 + 1] = dot_product3;",
						"res_ptr[1 * 3 + 1] = dot_product4;",
						"res_ptr[2 * 3 + 1] = dot_product5;",
						"res_ptr[0 * 3 + 2] = dot_product6;",
						"res_ptr[1 * 3 + 2] = dot_product7;",
						"res_ptr[2 * 3 + 2] = dot_product8;",
						"res_ptr += 9;",
						"if(res_ptr >= res_end) {",
							"break;",
						"}",
						"op1_ptr += 9;",
						"if(op1_ptr >= op1_end) {",
							"op1_ptr = op1_start;",
						"}",
						"op2_ptr += 9;",
						"if(op2_ptr >= op2_end) {",
							"op2_ptr = op2_start;",
						"}",
					"}",
				"}",
			),
			array(
				"static void ZEND_FASTCALL qb_multiply_rm_matrix_by_matrix_3x3_padded_$type($cType *op1_start, $cType *op1_end, $cType *op2_start, $cType *op2_end, $cType *res_start, $cType *res_end) {",
					"$cType *__restrict res_ptr = res_start;",
					"$cType *__restrict op1_ptr = op1_start;",
					"$cType *__restrict op2_ptr = op2_start;",
					"for(;;) {",
						"$cType dot_product0 = (op1_ptr[0 * 4 + 0] * op2_ptr[0 * 4 + 0]) + (op1_ptr[1 * 4 + 0] * op2_ptr[0 * 4 + 1]) + (op1_ptr[2 * 4 + 0] * op2_ptr[0 * 4 + 2]);",
						"$cType dot_product1 = (op1_ptr[0 * 4 + 0] * op2_ptr[1 * 4 + 0]) + (op1_ptr[1 * 4 + 0] * op2_ptr[1 * 4 + 1]) + (op1_ptr[2 * 4 + 0] * op2_ptr[1 * 4 + 2]);",
						"$cType dot_product2 = (op1_ptr[0 * 4 + 0] * op2_ptr[2 * 4 + 0]) + (op1_ptr[1 * 4 + 0] * op2_ptr[2 * 4 + 1]) + (op1_ptr[2 * 4 + 0] * op2_ptr[2 * 4 + 2]);",
						"$cType dot_product3 = (op1_ptr[0 * 4 + 1] * op2_ptr[0 * 4 + 0]) + (op1_ptr[1 * 4 + 1] * op2_ptr[0 * 4 + 1]) + (op1_ptr[2 * 4 + 1] * op2_ptr[0 * 4 + 2]);",
						"$cType dot_product4 = (op1_ptr[0 * 4 + 1] * op2_ptr[1 * 4 + 0]) + (op1_ptr[1 * 4 + 1] * op2_ptr[1 * 4 + 1]) + (op1_ptr[2 * 4 + 1] * op2_ptr[1 * 4 + 2]);",
						"$cType dot_product5 = (op1_ptr[0 * 4 + 1] * op2_ptr[2 * 4 + 0]) + (op1_ptr[1 * 4 + 1] * op2_ptr[2 * 4 + 1]) + (op1_ptr[2 * 4 + 1] * op2_ptr[2 * 4 + 2]);",
						"$cType dot_product6 = (op1_ptr[0 * 4 + 2] * op2_ptr[0 * 4 + 0]) + (op1_ptr[1 * 4 + 2] * op2_ptr[0 * 4 + 1]) + (op1_ptr[2 * 4 + 2] * op2_ptr[0 * 4 + 2]);",
						"$cType dot_product7 = (op1_ptr[0 * 4 + 2] * op2_ptr[1 * 4 + 0]) + (op1_ptr[1 * 4 + 2] * op2_ptr[1 * 4 + 1]) + (op1_ptr[2 * 4 + 2] * op2_ptr[1 * 4 + 2]);",
						"$cType dot_product8 = (op1_ptr[0 * 4 + 2] * op2_ptr[2 * 4 + 0]) + (op1_ptr[1 * 4 + 2] * op2_ptr[2 * 4 + 1]) + (op1_ptr[2 * 4 + 2] * op2_ptr[2 * 4 + 2]);",
						"res_ptr[0 * 4 + 0] = dot_product0;",
						"res_ptr[1 * 4 + 0] = dot_product1;",
						"res_ptr[2 * 4 + 0] = dot_product2;",
						"res_ptr[0 * 4 + 1] = dot_product3;",
						"res_ptr[1 * 4 + 1] = dot_product4;",
						"res_ptr[2 * 4 + 1] = dot_product5;",
						"res_ptr[0 * 4 + 2] = dot_product6;",
						"res_ptr[1 * 4 + 2] = dot_product7;",
						"res_ptr[2 * 4 + 2] = dot_product8;",
						"res_ptr += 12;",
						"if(res_ptr >= res_end) {",
							"break;",
						"}",
						"op1_ptr += 12;",
						"if(op1_ptr >= op1_end) {",
							"op1_ptr = op1_start;",
						"}",
						"op2_ptr += 12;",
						"if(op2_ptr >= op2_end) {",
							"op2_ptr = op2_start;",
						"}",
					"}",
				"}",
			),
			array(
				"static void ZEND_FASTCALL qb_multiply_rm_matrix_by_matrix_4x4_$type($cType *op1_start, $cType *op1_end, $cType *op2_start, $cType *op2_end, $cType *res_start, $cType *res_end) {",
					"$cType *__restrict res_ptr = res_start;",
					"$cType *__restrict op1_ptr = op1_start;",
					"$cType *__restrict op2_ptr = op2_start;",
					"for(;;) {",
						"$cType dot_product0 = (op1_ptr[0 * 4 + 0] * op2_ptr[0 * 4 + 0]) + (op1_ptr[1 * 4 + 0] * op2_ptr[0 * 4 + 1]) + (op1_ptr[2 * 4 + 0] * op2_ptr[0 * 4 + 2]) + (op1_ptr[3 * 4 + 0] * op2_ptr[0 * 4 + 3]);",
						"$cType dot_product1 = (op1_ptr[0 * 4 + 0] * op2_ptr[1 * 4 + 0]) + (op1_ptr[1 * 4 + 0] * op2_ptr[1 * 4 + 1]) + (op1_ptr[2 * 4 + 0] * op2_ptr[1 * 4 + 2]) + (op1_ptr[3 * 4 + 0] * op2_ptr[1 * 4 + 3]);",
						"$cType dot_product2 = (op1_ptr[0 * 4 + 0] * op2_ptr[2 * 4 + 0]) + (op1_ptr[1 * 4 + 0] * op2_ptr[2 * 4 + 1]) + (op1_ptr[2 * 4 + 0] * op2_ptr[2 * 4 + 2]) + (op1_ptr[3 * 4 + 0] * op2_ptr[2 * 4 + 3]);",
						"$cType dot_product3 = (op1_ptr[0 * 4 + 0] * op2_ptr[3 * 4 + 0]) + (op1_ptr[1 * 4 + 0] * op2_ptr[3 * 4 + 1]) + (op1_ptr[2 * 4 + 0] * op2_ptr[3 * 4 + 2]) + (op1_ptr[3 * 4 + 0] * op2_ptr[3 * 4 + 3]);",
						"$cType dot_product4 = (op1_ptr[0 * 4 + 1] * op2_ptr[0 * 4 + 0]) + (op1_ptr[1 * 4 + 1] * op2_ptr[0 * 4 + 1]) + (op1_ptr[2 * 4 + 1] * op2_ptr[0 * 4 + 2]) + (op1_ptr[3 * 4 + 1] * op2_ptr[0 * 4 + 3]);",
						"$cType dot_product5 = (op1_ptr[0 * 4 + 1] * op2_ptr[1 * 4 + 0]) + (op1_ptr[1 * 4 + 1] * op2_ptr[1 * 4 + 1]) + (op1_ptr[2 * 4 + 1] * op2_ptr[1 * 4 + 2]) + (op1_ptr[3 * 4 + 1] * op2_ptr[1 * 4 + 3]);",
						"$cType dot_product6 = (op1_ptr[0 * 4 + 1] * op2_ptr[2 * 4 + 0]) + (op1_ptr[1 * 4 + 1] * op2_ptr[2 * 4 + 1]) + (op1_ptr[2 * 4 + 1] * op2_ptr[2 * 4 + 2]) + (op1_ptr[3 * 4 + 1] * op2_ptr[2 * 4 + 3]);",
						"$cType dot_product7 = (op1_ptr[0 * 4 + 1] * op2_ptr[3 * 4 + 0]) + (op1_ptr[1 * 4 + 1] * op2_ptr[3 * 4 + 1]) + (op1_ptr[2 * 4 + 1] * op2_ptr[3 * 4 + 2]) + (op1_ptr[3 * 4 + 1] * op2_ptr[3 * 4 + 3]);",
						"$cType dot_product8 = (op1_ptr[0 * 4 + 2] * op2_ptr[0 * 4 + 0]) + (op1_ptr[1 * 4 + 2] * op2_ptr[0 * 4 + 1]) + (op1_ptr[2 * 4 + 2] * op2_ptr[0 * 4 + 2]) + (op1_ptr[3 * 4 + 2] * op2_ptr[0 * 4 + 3]);",
						"$cType dot_product9 = (op1_ptr[0 * 4 + 2] * op2_ptr[1 * 4 + 0]) + (op1_ptr[1 * 4 + 2] * op2_ptr[1 * 4 + 1]) + (op1_ptr[2 * 4 + 2] * op2_ptr[1 * 4 + 2]) + (op1_ptr[3 * 4 + 2] * op2_ptr[1 * 4 + 3]);",
						"$cType dot_product10 = (op1_ptr[0 * 4 + 2] * op2_ptr[2 * 4 + 0]) + (op1_ptr[1 * 4 + 2] * op2_ptr[2 * 4 + 1]) + (op1_ptr[2 * 4 + 2] * op2_ptr[2 * 4 + 2]) + (op1_ptr[3 * 4 + 2] * op2_ptr[2 * 4 + 3]);",
						"$cType dot_product11 = (op1_ptr[0 * 4 + 2] * op2_ptr[3 * 4 + 0]) + (op1_ptr[1 * 4 + 2] * op2_ptr[3 * 4 + 1]) + (op1_ptr[2 * 4 + 2] * op2_ptr[3 * 4 + 2]) + (op1_ptr[3 * 4 + 2] * op2_ptr[3 * 4 + 3]);",
						"$cType dot_product12 = (op1_ptr[0 * 4 + 3] * op2_ptr[0 * 4 + 0]) + (op1_ptr[1 * 4 + 3] * op2_ptr[0 * 4 + 1]) + (op1_ptr[2 * 4 + 3] * op2_ptr[0 * 4 + 2]) + (op1_ptr[3 * 4 + 3] * op2_ptr[0 * 4 + 3]);",
						"$cType dot_product13 = (op1_ptr[0 * 4 + 3] * op2_ptr[1 * 4 + 0]) + (op1_ptr[1 * 4 + 3] * op2_ptr[1 * 4 + 1]) + (op1_ptr[2 * 4 + 3] * op2_ptr[1 * 4 + 2]) + (op1_ptr[3 * 4 + 3] * op2_ptr[1 * 4 + 3]);",
						"$cType dot_product14 = (op1_ptr[0 * 4 + 3] * op2_ptr[2 * 4 + 0]) + (op1_ptr[1 * 4 + 3] * op2_ptr[2 * 4 + 1]) + (op1_ptr[2 * 4 + 3] * op2_ptr[2 * 4 + 2]) + (op1_ptr[3 * 4 + 3] * op2_ptr[2 * 4 + 3]);",
						"$cType dot_product15 = (op1_ptr[0 * 4 + 3] * op2_ptr[3 * 4 + 0]) + (op1_ptr[1 * 4 + 3] * op2_ptr[3 * 4 + 1]) + (op1_ptr[2 * 4 + 3] * op2_ptr[3 * 4 + 2]) + (op1_ptr[3 * 4 + 3] * op2_ptr[3 * 4 + 3]);",
						"res_ptr[0 * 4 + 0] = dot_product0;",
						"res_ptr[1 * 4 + 0] = dot_product1;",
						"res_ptr[2 * 4 + 0] = dot_product2;",
						"res_ptr[3 * 4 + 0] = dot_product3;",
						"res_ptr[0 * 4 + 1] = dot_product4;",
						"res_ptr[1 * 4 + 1] = dot_product5;",
						"res_ptr[2 * 4 + 1] = dot_product6;",
						"res_ptr[3 * 4 + 1] = dot_product7;",
						"res_ptr[0 * 4 + 2] = dot_product8;",
						"res_ptr[1 * 4 + 2] = dot_product9;",
						"res_ptr[2 * 4 + 2] = dot_product10;",
						"res_ptr[3 * 4 + 2] = dot_product11;",
						"res_ptr[0 * 4 + 3] = dot_product12;",
						"res_ptr[1 * 4 + 3] = dot_product13;",
						"res_ptr[2 * 4 + 3] = dot_product14;",
						"res_ptr[3 * 4 + 3] = dot_product15;",
						"res_ptr += 16;",
						"if(res_ptr >= res_end) {",
							"break;",
						"}",
						"op1_ptr += 16;",
						"if(op1_ptr >= op1_end) {",
							"op1_ptr = op1_start;",
						"}",
						"op2_ptr += 16;",
						"if(op2_ptr >= op2_end) {",
							"op2_ptr = op2_start;",
						"}",
					"}",
				"}",
			),
			array(
				"static void ZEND_FASTCALL qb_multiply_rm_matrix_by_matrix_$type($cType *op1_start, $cType *op1_end, uint32_t m1_row, uint32_t m1_col, $cType *op2_start, $cType *op2_end, uint32_t m2_row, uint32_t m2_col, $cType *res_start, $cType *res_end) {",
					"ALLOCA_FLAG(use_heap)",
					"$cType *buffer = do_alloca(m1_row * m2_col * sizeof($cType), use_heap);",
					"$cType *__restrict res_ptr = res_start;",
					"$cType *__restrict op1_ptr = op1_start;",
					"$cType *__restrict op2_ptr = op2_start;",
					"for(;;) {",
						"uint32_t i, j, k, p, q, res_index = 0;",
						"for(i = 0, q = 0; i < m1_row; ++i) {",
							"for(j = 0; j < m2_col; ++j) {",
								"$cType dot_product = 0;",
								"for(p = 0, k = j; p < m2_row; ++p, k += m2_col) {",
									"dot_product += op1_ptr[p + q] * op2_ptr[k];",
								"}",
								"buffer[res_index++] = dot_product;",
							"}",
							"q += m1_col;",
						"}",
						"memcpy(res_ptr, buffer, m1_row * m2_col * sizeof($cType));",
						"res_ptr += m1_row * m2_col;",
						"if(res_ptr >= res_end) {",
							"break;",
						"}",
						"op1_ptr += m1_row * m1_col;",
						"if(op1_ptr >= op1_end) {",
							"op1_ptr = op1_start;",
						"}",
						"op2_ptr += m2_row * m2_col;",
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
	
	public function getAction() {
		$type = $this->getOperandType(1);
		if($this->operandSize == "variable") {
			if($this->addressMode == "ARR") {
				return "qb_multiply_rm_matrix_by_matrix_$type(op1_start, op1_end, MATRIX1_ROWS, MATRIX1_COLS, op2_start, op2_end, MATRIX2_ROWS, MATRIX2_COLS, res_start, res_end);";
			} else {
				return "qb_multiply_rm_matrix_by_matrix_$type(op1_ptr, NULL, MATRIX1_ROWS, MATRIX1_COLS, op2_ptr, NULL, MATRIX2_ROWS, MATRIX2_COLS, res_ptr, NULL);";
			}
		} else {
			if($this->operandPadding) {
				if($this->addressMode == "ARR") {
					return "qb_multiply_rm_matrix_by_matrix_{$this->operandSize}x{$this->operandSize}_padded_$type(op1_start, op1_end, op2_start, op2_end, res_start, res_end);";
				} else {
					return "qb_multiply_rm_matrix_by_matrix_{$this->operandSize}x{$this->operandSize}_padded_$type(op1_ptr, NULL, op2_ptr, NULL, res_ptr, NULL);";
				}
			} else {
				if($this->addressMode == "ARR") {
					return "qb_multiply_rm_matrix_by_matrix_{$this->operandSize}x{$this->operandSize}_$type(op1_start, op1_end, op2_start, op2_end, res_start, res_end);";
				} else {
					return "qb_multiply_rm_matrix_by_matrix_{$this->operandSize}x{$this->operandSize}_$type(op1_ptr, NULL, op2_ptr, NULL, res_ptr, NULL);";
				}
			}
		}
	}
}

?>