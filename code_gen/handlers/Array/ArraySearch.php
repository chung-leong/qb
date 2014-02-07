<?php

class ArraySearch extends Handler {

	use MultipleAddressMode, BinaryOperator, NoUnsigned;

	public function getOperandAddressMode($i) {
		switch($i) {
			case 1: return "ARR";
			case 2: return $this->addressMode;
			case 3: return "SCA";
		}
	}
	
	public function getOperandType($i) {
		switch($i) {
			case 1: return $this->operandType;
			case 2: return $this->operandType;
			case 3: return "I32";
		}
	}
	
	public function getOperandSize($i) {
		switch($i) {
			case 1: return "op1_count";
			case 2: return "op2_count";
			case 3: return 1;
		}
	}
	
	protected function getActionOnUnitData() {
		$lines = array();
		$lines[] = "int32_t index = -1;";
		$lines[] = "uint32_t i;";
		$lines[] = "for(i = 0; i < op1_count; i++) {";
		$lines[] = 		"if(op1_ptr[i] == op2) {";
		$lines[] = 			"index = i;";
		$lines[] =			"break;";
		$lines[] = 		"}";
		$lines[] = "}";
		$lines[] = "res = index;";
		return $lines;
	}
	
	protected function getActionOnMultipleData() {
		$lines = array();
		$lines[] = "int32_t index = -1;";
		$lines[] = "uint32_t i, j, k;";
		$lines[] = "for(i = 0, j = 0; j < op1_count; i++, j += op2_count) {";
		$lines[] = 		"if(op1_ptr[j] == op2_ptr[0]) {";
		$lines[] = 			"for(k = 1; k < op2_count; k++) {";
		$lines[] = 				"if(op1_ptr[j + k] != op2_ptr[k]) {";
		$lines[] = 					"break;";
		$lines[] = 				"}";
		$lines[] = 			"}";
		$lines[] = 			"if(k == op2_count) {";
		$lines[] = 				"index = i;";
		$lines[] = 				"break;";
		$lines[] = 			"}";
		$lines[] = 		"}";
		$lines[] = "}";
		$lines[] = "res = index;";
		return $lines;
	}
}

?>