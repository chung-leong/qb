<?php

class QBLengthHandler extends QBSIMDHandler {

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
				"static $cType ZEND_FASTCALL qb_calculate_array_length_1x_$type($cType *v) {",
					"return ($cType) fabs(v[0]);",
				"}",
			),
			array(
				"static $cType ZEND_FASTCALL qb_calculate_array_length_2x_$type($cType *v) {",
					"$cType sum = (v[0] * v[0]) + (v[1] * v[1]);",
					"return ($cType) sqrt(sum);",
				"}",
			),
			array(
				"static $cType ZEND_FASTCALL qb_calculate_array_length_3x_$type($cType *v) {",
					"$cType sum = (v[0] * v[0]) + (v[1] * v[1]) + (v[2] * v[2]);",
					"return ($cType) sqrt(sum);",
				"}",
			),
			array(
				"static $cType ZEND_FASTCALL qb_calculate_array_length_4x_$type($cType *v) {",
					"$cType sum = (v[0] * v[0]) + (v[1] * v[1]) + (v[2] * v[2]) + (v[3] * v[3]);",
					"return ($cType) sqrt(sum);",
				"}",
			),
			array(
				"static $cType ZEND_FASTCALL qb_calculate_array_length_$type($cType *v, uint32_t size) {",
					"uint32_t i;",
					"$cType sum = 0;",
					"for(i = 0; i < size; i++) {",
						"sum += v[i] * v[i];",
					"}",
					"return ($cType) sqrt(sum);",
				"}",
			),
		);
		return $functions;
	}
	
	public function getResultSizePossibilities() {
		return "vector_count";
	}
	
	public function getResultSizeCalculation() {
		$vectorSize = $this->getOperandSize(1);
		return "vector_count = op1_count / $vectorSize;";
	}
	
	public function getOperandSize($i) {
		if($i == 2) {
			return 1;
		} else {
			if($this->operandSize == "variable") {
				return "VECTOR_SIZE";
			} else {
				return $this->operandSize;
			}
		}
	}
	
	protected function getSIMDExpression() {
		$type = $this->getOperandType(1);
		if($this->operandSize == "variable") {
			return "res = qb_calculate_array_length_$type(op1_ptr, VECTOR_SIZE);";
		} else {
			return "res = qb_calculate_array_length_{$this->operandSize}x_$type(op1_ptr);";
		}
	}
}

?>