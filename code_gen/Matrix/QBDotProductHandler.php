<?php

class QBDotProductHandler extends QBSIMDHandler {

	public function getOperandAddressMode($i) {
		switch($i) {
			case 1: 
			case 2: return "ARR";
			case 3: return $this->addressMode;
		}
	}

	public function getHelperFunctions() {
		$cType = $this->getOperandCType(1);
		$type = $this->getOperandType(1);
		$functions = array(
			array(
				"static $cType ZEND_FASTCALL qb_calculate_dot_product_1x_$type($cType *v1, $cType *v2) {",
					"$cType sum = (v1[0] * v2[0]);",
					"return sum;",
				"}",
			),
			array(
				"static $cType ZEND_FASTCALL qb_calculate_dot_product_2x_$type($cType *v1, $cType *v2) {",
					"$cType sum = (v1[0] * v2[0]) + (v1[1] * v2[1]);",
					"return sum;",
				"}",
			),
			array(
				"static $cType ZEND_FASTCALL qb_calculate_dot_product_3x_$type($cType *v1, $cType *v2) {",
					"$cType sum = (v1[0] * v2[0]) + (v1[1] * v2[1]) + (v1[2] * v2[2]);",
					"return sum;",
				"}",
			),
			array(
				"static $cType ZEND_FASTCALL qb_calculate_dot_product_4x_$type($cType *v1, $cType *v2) {",
					"$cType sum = (v1[0] * v2[0]) + (v1[1] * v2[1]) + (v1[2] * v2[2]) + (v1[3] * v2[3]);",
					"return sum;",
				"}",
			),
			array(
				"static $cType ZEND_FASTCALL qb_calculate_dot_product_$type($cType *v1, $cType *v2, uint32_t size) {",
					"uint32_t i;",
					"$cType sum = 0;",
					"for(i = 0; i < size; i++) {",
						"sum += v1[i] * v2[i];",
					"}",
					"return sum;",
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
		return "vector_count = ((op1_count > op2_count) ? op1_count : op2_count) / $vectorSize;";
	}
	
	public function getOperandSize($i) {
		if($i == 3) {
			return 1;
		} else {
			if($this->operandSize == "variable") {
				return "VECTOR_SIZE";
			} else {
				return $this->operandSize;
			}
		}
	}
	
	public function getSIMDExpression() {
		$type = $this->getOperandType(1);
		if($this->operandSize == "variable") {
			return "res = qb_calculate_dot_product_$type(op1_ptr, op2_ptr, VECTOR_SIZE);";
		} else {
			return "res = qb_calculate_dot_product_{$this->operandSize}x_$type(op1_ptr, op2_ptr);";
		}
	}
}

?>