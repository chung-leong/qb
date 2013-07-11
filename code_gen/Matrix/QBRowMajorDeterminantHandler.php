<?php

class QBRowMajorDeterminantHandler extends QBSIMDHandler {

	public function getOperandAddressMode($i) {
		switch($i) {
			case 1: return "ARR";
			case 2: return $this->addressMode;
		}
	}

	public function getHelperFunctions() {
		$type = $this->getOperandType(1);
		$cType = $this->getOperandCType(1);
		$functions = array(
			array(
				"static void ZEND_FASTCALL qb_calculate_rm_matrix_determinant_2x2_$type($cType *op1_start, $cType *op1_end, $cType *res_start, $cType *res_end) {",
					"$cType *__restrict res_ptr = res_start;",
					"$cType *__restrict op1_ptr = op1_start;",
					"for(;;) {",
						"res_ptr[0] = (op1_ptr[0 * 2 + 0] * op1_ptr[1 * 2 + 1]) - (op1_ptr[1 * 2 + 0] * op1_ptr[0 * 2 + 1]);",
						"res_ptr += 1;",
						"if(res_ptr >= res_end) {",
							"break;",
						"}",
						"op1_ptr += 4;",
						"if(op1_ptr >= op1_end) {",
							"op1_ptr = op1_start;",
						"}",
					"}",
				"}",
			),
			array(
				"static void ZEND_FASTCALL qb_calculate_rm_matrix_determinant_3x3_$type($cType *op1_start, $cType *op1_end, $cType *res_start, $cType *res_end) {",
					"$cType *__restrict res_ptr = res_start;",
					"$cType *__restrict op1_ptr = op1_start;",
					"for(;;) {",
						"res_ptr[0] =	 (op1_ptr[0 * 3 + 0] * op1_ptr[1 * 3 + 1] * op1_ptr[2 * 3 + 2]) -",
										"(op1_ptr[0 * 3 + 0] * op1_ptr[2 * 3 + 1] * op1_ptr[1 * 3 + 2]) +",
										"(op1_ptr[1 * 3 + 0] * op1_ptr[2 * 3 + 1] * op1_ptr[0 * 3 + 2]) -",
										"(op1_ptr[1 * 3 + 0] * op1_ptr[0 * 3 + 1] * op1_ptr[2 * 3 + 2]) +",
										"(op1_ptr[2 * 3 + 0] * op1_ptr[0 * 3 + 1] * op1_ptr[1 * 3 + 2]) -",
										"(op1_ptr[2 * 3 + 0] * op1_ptr[1 * 3 + 1] * op1_ptr[0 * 3 + 2]);",
						"res_ptr += 1;",
						"if(res_ptr >= res_end) {",
							"break;",
						"}",
						"op1_ptr += 9;",
						"if(op1_ptr >= op1_end) {",
							"op1_ptr = op1_start;",
						"}",
					"}",
				"}",
			),
			array(
				"static void ZEND_FASTCALL qb_calculate_rm_matrix_determinant_4x4_$type($cType *op1_start, $cType *op1_end, $cType *res_start, $cType *res_end) {",
					"$cType *__restrict res_ptr = res_start;",
					"$cType *__restrict op1_ptr = op1_start;",
					"for(;;) {",
						"res_ptr[0] = 	 ((op1_ptr[3 * 4 + 0] * op1_ptr[2 * 4 + 1]) * (op1_ptr[1 * 4 + 2] * op1_ptr[0 * 4 + 3])) - ((op1_ptr[2 * 4 + 0] * op1_ptr[3 * 4 + 1]) * (op1_ptr[1 * 4 + 2] * op1_ptr[0 * 4 + 3])) -",
										"((op1_ptr[3 * 4 + 0] * op1_ptr[1 * 4 + 1]) * (op1_ptr[2 * 4 + 2] * op1_ptr[0 * 4 + 3])) + ((op1_ptr[1 * 4 + 0] * op1_ptr[3 * 4 + 1]) * (op1_ptr[2 * 4 + 2] * op1_ptr[0 * 4 + 3])) +",
										"((op1_ptr[2 * 4 + 0] * op1_ptr[1 * 4 + 1]) * (op1_ptr[3 * 4 + 2] * op1_ptr[0 * 4 + 3])) - ((op1_ptr[1 * 4 + 0] * op1_ptr[2 * 4 + 1]) * (op1_ptr[3 * 4 + 2] * op1_ptr[0 * 4 + 3])) -",
										"((op1_ptr[3 * 4 + 0] * op1_ptr[2 * 4 + 1]) * (op1_ptr[0 * 4 + 2] * op1_ptr[1 * 4 + 3])) + ((op1_ptr[2 * 4 + 0] * op1_ptr[3 * 4 + 1]) * (op1_ptr[0 * 4 + 2] * op1_ptr[1 * 4 + 3])) +",
										"((op1_ptr[3 * 4 + 0] * op1_ptr[0 * 4 + 1]) * (op1_ptr[2 * 4 + 2] * op1_ptr[1 * 4 + 3])) - ((op1_ptr[0 * 4 + 0] * op1_ptr[3 * 4 + 1]) * (op1_ptr[2 * 4 + 2] * op1_ptr[1 * 4 + 3])) -",
										"((op1_ptr[2 * 4 + 0] * op1_ptr[0 * 4 + 1]) * (op1_ptr[3 * 4 + 2] * op1_ptr[1 * 4 + 3])) + ((op1_ptr[0 * 4 + 0] * op1_ptr[2 * 4 + 1]) * (op1_ptr[3 * 4 + 2] * op1_ptr[1 * 4 + 3])) +",
										"((op1_ptr[3 * 4 + 0] * op1_ptr[1 * 4 + 1]) * (op1_ptr[0 * 4 + 2] * op1_ptr[2 * 4 + 3])) - ((op1_ptr[1 * 4 + 0] * op1_ptr[3 * 4 + 1]) * (op1_ptr[0 * 4 + 2] * op1_ptr[2 * 4 + 3])) -",
										"((op1_ptr[3 * 4 + 0] * op1_ptr[0 * 4 + 1]) * (op1_ptr[1 * 4 + 2] * op1_ptr[2 * 4 + 3])) + ((op1_ptr[0 * 4 + 0] * op1_ptr[3 * 4 + 1]) * (op1_ptr[1 * 4 + 2] * op1_ptr[2 * 4 + 3])) +",
										"((op1_ptr[1 * 4 + 0] * op1_ptr[0 * 4 + 1]) * (op1_ptr[3 * 4 + 2] * op1_ptr[2 * 4 + 3])) - ((op1_ptr[0 * 4 + 0] * op1_ptr[1 * 4 + 1]) * (op1_ptr[3 * 4 + 2] * op1_ptr[2 * 4 + 3])) -",
										"((op1_ptr[2 * 4 + 0] * op1_ptr[1 * 4 + 1]) * (op1_ptr[0 * 4 + 2] * op1_ptr[3 * 4 + 3])) + ((op1_ptr[1 * 4 + 0] * op1_ptr[2 * 4 + 1]) * (op1_ptr[0 * 4 + 2] * op1_ptr[3 * 4 + 3])) +",
										"((op1_ptr[2 * 4 + 0] * op1_ptr[0 * 4 + 1]) * (op1_ptr[1 * 4 + 2] * op1_ptr[3 * 4 + 3])) - ((op1_ptr[0 * 4 + 0] * op1_ptr[2 * 4 + 1]) * (op1_ptr[1 * 4 + 2] * op1_ptr[3 * 4 + 3])) -",
										"((op1_ptr[1 * 4 + 0] * op1_ptr[0 * 4 + 1]) * (op1_ptr[2 * 4 + 2] * op1_ptr[3 * 4 + 3])) + ((op1_ptr[0 * 4 + 0] * op1_ptr[1 * 4 + 1]) * (op1_ptr[2 * 4 + 2] * op1_ptr[3 * 4 + 3]));",
						"res_ptr += 1;",
						"if(res_ptr >= res_end) {",
							"break;",
						"}",
						"op1_ptr += 16;",
						"if(op1_ptr >= op1_end) {",
							"op1_ptr = op1_start;",
						"}",
					"}",
				"}",
			),
		);
		return $functions;
	}
	
	public function getResultSizePossibilities() {
		if($this->addressMode == "ARR") {
			return "matrix1_count";
		}
	}
	
	public function getResultSizeCalculation() {
		if($this->addressMode == "ARR") {
			$matrixSize = $this->getOperandSize(1);
			return "matrix1_count = op1_count / $matrixSize;";
		}
	}
	
	public function getOperandSize($i) {
		if($i == 2) {
			return 1;
		} else {
			if($this->operandSize == "variable") {
				return "(MATRIX1_ROWS * MATRIX1_COLS)";
			} else {
				return $this->operandSize * $this->operandSize;
			}
		}
	}
	
	public function getAction() {
		$type = $this->getOperandType(1);
		if($this->operandSize == "variable") {
			throw Exception("No implementation for matrices of arbituary size");
		} else {
			if($this->addressMode == "ARR") {
				return "qb_calculate_rm_matrix_determinant_{$this->operandSize}x{$this->operandSize}_$type(op1_start, op1_end, res_start, res_end);";
			} else {
				return "qb_calculate_rm_matrix_determinant_{$this->operandSize}x{$this->operandSize}_$type(op1_ptr, NULL, res_ptr, NULL);";
			}
		}
	}
}

?>