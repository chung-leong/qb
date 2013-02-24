<?php

class QBDotProduct3xHandler extends QBHandler {

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
				"static $cType ZEND_FASTCALL qb_calculate_dot_product_3x_$type(qb_interpreter_context *__restrict cxt, $cType *v1, $cType *v2) {",
					"$cType sum = (v1[0] * v2[0]) + (v1[1] * v2[1]) + (v1[2] * v2[2]);",
					"return sum;",
				"}",
			),
		);
		return $functions;
	}
	
	public function getAction() {
		$type = $this->getOperandType(1);
		$lines = array();
		$lines[] = "res = qb_calculate_dot_product_3x_$type(cxt, op1_ptr, op2_ptr);";
		return $lines;
	}

}

?>