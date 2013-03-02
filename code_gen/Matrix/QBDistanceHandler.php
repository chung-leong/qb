<?php

class QBDistanceHandler extends QBSIMDHandler {

	public function getOperandAddressMode($i) {
		switch($i) {
			case 1:
			case 2: return "ARR";
			case 3: return $this->addressMode;
		}
	}

	public function getHelperFunctions() {
		$type = $this->getOperandType(1);
		$cType = $this->getOperandCType(1);
		$functions = array(
			array(
				"static $cType ZEND_FASTCALL qb_calculate_array_distance_1x_$type($cType *v1, $cType *v2) {",
					"return ($cType) fabs(v1[0] - v2[0]);",
				"}",
			),
			array(
				"static $cType ZEND_FASTCALL qb_calculate_array_distance_2x_$type($cType *v1, $cType *v2) {",
					"$cType sum = (v1[0] - v2[0]) * (v1[0] - v2[0]) + (v1[1] - v2[1]) * (v1[1] - v2[1]);",
					"return ($cType) sqrt(sum);",
				"}",
			),
			array(
				"static $cType ZEND_FASTCALL qb_calculate_array_distance_3x_$type($cType *v1, $cType *v2) {",
					"$cType sum = (v1[0] - v2[0]) * (v1[0] - v2[0]) + (v1[1] - v2[1]) * (v1[1] - v2[1]) + (v1[2] - v2[2]) * (v1[2] - v2[2]);",
					"return ($cType) sqrt(sum);",
				"}",
			),
			array(
				"static $cType ZEND_FASTCALL qb_calculate_array_distance_4x_$type($cType *v1, $cType *v2) {",
					"$cType sum = (v1[0] - v2[0]) * (v1[0] - v2[0]) + (v1[1] - v2[1]) * (v1[1] - v2[1]) + (v1[2] - v2[2]) * (v1[2] - v2[2]) + (v1[3] - v2[3]) * (v1[3] - v2[3]);",
					"return ($cType) sqrt(sum);",
				"}",
			),
			array(
				"static $cType ZEND_FASTCALL qb_calculate_array_distance_$type($cType *v1, $cType *v2, uint32_t count) {",
					"uint32_t i;",
					"$cType sum = 0;",
					"for(i = 0; i < count; i++) {",
						"sum += (v1[i] - v2[i]) * (v1[i] - v2[i]);",
					"}",
					"return ($cType) sqrt(sum);",
				"}",
			),
		);
		return $functions;
	}
	
	public function getResultSizePossibilities() {
		if($this->addressMode == "ARR") {
			return "vector_count";
		}
	}
	
	public function getResultSizeCalculation() {
		if($this->addressMode == "ARR") {
			$vectorSize = $this->getOperandSize(1);
			return "vector_count = ((op1_count > op2_count) ? op1_count : op2_count) / $vectorSize;";
		}
	}
	
	public function getOperandSize($i) {
		if($i == 3) {
			return 1;
		} else {
			return parent::getOperandSize($i);
		}
	}
	
	protected function getSIMDExpression() {
		$type = $this->getOperandType(1);
		if($this->operandSize == "variable") {
			return "res = qb_calculate_array_distance_$type(op1_ptr, op2_ptr, MATRIX2_ROWS);";
		} else {
			return "res = qb_calculate_array_distance_{$this->operandSize}x_$type(op1_ptr, op2_ptr);";
		}
	}
}

?>