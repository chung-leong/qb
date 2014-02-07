<?php

class ArrayPadCount extends Handler {

	use ScalarAddressMode, BinaryOperator;
	
	public function getOperandAddressMode($i) {
		switch($i) {
			case 1: return "SCA";
			case 2: return "SCA";
			case 3: return "SCA";
		}
	}
	
	public function getOperandType($i) {
		switch($i) {
			case 1: return "U32";
			case 2: return "S32";
			case 3: return "U32";
		}
	}
	
	public function getOperandSize($i) {
		switch($i) {
			case 1: return 1;
			case 2: return 1;
			case 3: return 1;
		}
	}
	
	public function getActionOnUnitData() {
		$lines = array();
		$lines[] = "if(op2 < 0) {";
		$lines[] = 		"res = ((uint32_t) (-op2) > op1) ? (uint32_t) (-op2) : op1;";
		$lines[] = "} else {";
		$lines[] =		"res = ((uint32_t) op2 > op1) ? (uint32_t) op2 : op1;";
		$lines[] = "}";
		return $lines;
	}
}

?>