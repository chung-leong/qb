<?php

class ArrayUniqueCount extends Handler {

	use ScalarAddressMode, BinaryOperator;

	public function getOperandType($i) {
		switch($i) {
			case 1: return $this->operandType;
			case 2: return "U32";
			case 3: return "U32";
		}
	}
	
	public function getOperandAddressMode($i) {
		switch($i) {
			case 1: return "ARR";
			case 2: return "SCA";
			case 3: return "SCA";
		}
	}
	
	public function getActionOnUnitData() {
		$lines = array();
		$lines[] = "uint32_t count = 0;";
		$lines[] = "uint32_t i, j, k;";
		$lines[] = "if(op2 == 1) {";
		$lines[] = 	"for(i = 0; i < op1_count; i++) {";
		$lines[] = 		"int32_t repeat = FALSE;";
		$lines[] = 		"for(j = 0; j < i; j++) {";
		$lines[] = 			"if(op1_ptr[j] == op1_ptr[i]) {";
		$lines[] = 				"repeat = TRUE;";
		$lines[] = 				"break;";
		$lines[] = 			"}";
		$lines[] = 		"}";
		$lines[] = 		"if(!repeat) {";
		$lines[] = 			"count++;";
		$lines[] = 		"}";
		$lines[] = 	"}";
		$lines[] = "} else {";
		$lines[] = 	"for(i = 0; i < op1_count; i += op2) {";
		$lines[] = 		"int32_t repeat = FALSE;";
		$lines[] = 		"for(j = 0; j < i; j += op2) {";
		$lines[] = 			"if(op1_ptr[j] == op1_ptr[i]) {";
		$lines[] = 				"int32_t match = TRUE;";
		$lines[] = 				"for(k = 1; k < op2; k++) {";
		$lines[] = 					"if(op1_ptr[j + k] != op1_ptr[i + k]) {";
		$lines[] = 						"match = FALSE;";
		$lines[] = 						"break;";
		$lines[] = 					"}";
		$lines[] = 				"}";
		$lines[] = 				"if(match) {";
		$lines[] = 					"repeat = TRUE;";
		$lines[] = 					"break;";
		$lines[] =						"}";
		$lines[] = 			"}";
		$lines[] = 		"}";
		$lines[] = 		"if(!repeat) {";
		$lines[] = 			"count += op2;";
		$lines[] = 		"}";
		$lines[] = 	"}";
		$lines[] = "}";
		$lines[] = "res = count;";
		return $lines;
	}
}

?>