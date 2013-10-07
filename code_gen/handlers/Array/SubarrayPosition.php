<?php

class SubarrayPosition extends Handler {

	use MultipleAddressMode, TernaryOperator, NoUnsigned;
	
	public function getOperandAddressMode($i) {
		switch($i) {
			case 1: return "ARR";
			case 2: return "ARR";
			case 3: return "SCA";
			case 4: return "SCA";
		}
	}
	
	public function getOperandType($i) {
		switch($i) {
			case 1: return $this->operandType;
			case 2: return $this->operandType;
			case 3: return "I32";
			case 4: return "I32";
		}
	}

	public function getActionOnUnitData() {
		$type = $this->getOperandType(1);
		$cType = $this->getOperandCType(1);
		$lines = array();
		$lines[] = "int32_t index = -1;";		
		$lines[] = "if(op2_count > 0) {";
		$lines[] = 		"uint32_t i, j;";
		$lines[] = 		"for(i = op3; i < op1_count - op2_count; i++) {";
		$lines[] = 			"if(op1_ptr[i] == op2_ptr[0]) {";
		$lines[] = 				"for(j = 1; j < op2_count; j++) {";
		$lines[] = 					"if(op1_ptr[i + j] != op2_ptr[j]) {";
		$lines[] = 						"break;";
		$lines[] = 					"}";
		$lines[] = 				"}";
		$lines[] = 				"if(j == op2_count) {";
		$lines[] = 					"index = i;";
		$lines[] = 					"break;";
		$lines[] = 				"}";
		$lines[] = 			"}";
		$lines[] = 		"}";
		$lines[] = "}";
		$lines[] = "res = index;";
		return $lines;
	}
}

?>