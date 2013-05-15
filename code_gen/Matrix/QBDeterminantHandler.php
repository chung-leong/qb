<?php

class QBDeterminantHandler extends QBSIMDHandler {

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
				"static $cType ZEND_FASTCALL qb_calculate_matrix_determinant_2x2_$type($cType *m) {",
					"$cType det = (m[0] * m[3]) - (m[1] * m[2]);",
					"return det;",
				"}",
			),
			array(
				"static $cType ZEND_FASTCALL qb_calculate_matrix_determinant_3x3_$type($cType *m) {",
					"$cType det =	 (m[0] * m[4] * m[8]) -",
									"(m[0] * m[5] * m[7]) +",
									"(m[1] * m[5] * m[6]) -",
									"(m[1] * m[3] * m[8]) +",
									"(m[2] * m[3] * m[7]) -",
									"(m[2] * m[4] * m[6]);",
					"return det;",
				"}",
			),
			array(
				"static $cType ZEND_FASTCALL qb_calculate_matrix_determinant_4x4_$type($cType *m) {",
					"$cType det = 	 ((m[ 3] * m[ 6]) * (m[ 9] * m[12])) - ((m[ 2] * m[ 7]) * (m[ 9] * m[12])) -",
									"((m[ 3] * m[ 5]) * (m[10] * m[12])) + ((m[ 1] * m[ 7]) * (m[10] * m[12])) +",
									"((m[ 2] * m[ 5]) * (m[11] * m[12])) - ((m[ 1] * m[ 6]) * (m[11] * m[12])) -",
									"((m[ 3] * m[ 6]) * (m[ 8] * m[13])) + ((m[ 2] * m[ 7]) * (m[ 8] * m[13])) +",
									"((m[ 3] * m[ 4]) * (m[10] * m[13])) - ((m[ 0] * m[ 7]) * (m[10] * m[13])) -",
									"((m[ 2] * m[ 4]) * (m[11] * m[13])) + ((m[ 0] * m[ 6]) * (m[11] * m[13])) +",
									"((m[ 3] * m[ 5]) * (m[ 8] * m[14])) - ((m[ 1] * m[ 7]) * (m[ 8] * m[14])) -",
									"((m[ 3] * m[ 4]) * (m[ 9] * m[14])) + ((m[ 0] * m[ 7]) * (m[ 9] * m[14])) +",
									"((m[ 1] * m[ 4]) * (m[11] * m[14])) - ((m[ 0] * m[ 5]) * (m[11] * m[14])) -",
									"((m[ 2] * m[ 5]) * (m[ 8] * m[15])) + ((m[ 1] * m[ 6]) * (m[ 8] * m[15])) +",
									"((m[ 2] * m[ 4]) * (m[ 9] * m[15])) - ((m[ 0] * m[ 6]) * (m[ 9] * m[15])) -",
									"((m[ 1] * m[ 4]) * (m[10] * m[15])) + ((m[ 0] * m[ 5]) * (m[10] * m[15]));",
					"return det;",
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
	
	protected function getSIMDExpression() {
		$type = $this->getOperandType(1);
		if($this->operandSize == "variable") {
			throw Exception("No implementation for matrices of arbituary size");
		} else {
			return "res = qb_calculate_matrix_determinant_{$this->operandSize}x{$this->operandSize}_$type(op1_ptr);";
		}
	}
}

?>