<?php

class ArrayUnique extends Handler {

	use ArrayAddressMode, BinaryOperator;

	public function getOperandType($i) {
		switch($i) {
			case 1: return $this->operandType;
			case 2: return "U32";
			case 3: return $this->operandType;
		}
	}
	
	public function getOperandAddressMode($i) {
		switch($i) {
			case 1: return "ARR";
			case 2: return "SCA";
			case 3: return "ARR";
		}
	}
	
	public function getActionOnUnitData() {
		$lines = array();
		$lines[] = "uint32_t i, j, k;";
		$lines[] = "if(op2 == 1) {";
		$lines[] = 		"for(i = 0; i < op1_count; i++) {";
		$lines[] = 			"int32_t repeat = FALSE;";
		$lines[] = 			"for(j = 0; j < i; j++) {";
		$lines[] = 				"if(op1_ptr[j] == op1_ptr[i]) {";
		$lines[] = 					"repeat = TRUE;";
		$lines[] = 					"break;";
		$lines[] = 				"}";
		$lines[] = 			"}";
		$lines[] = 			"if(!repeat) {";
		$lines[] =				"*res_ptr = op1_ptr[i];";
		$lines[] =				"res_ptr++;";
		$lines[] = 			"}";
		$lines[] = 		"}";
		$lines[] = "} else {";
		$lines[] = 		"for(i = 0; i < op1_count; i += op2) {";
		$lines[] = 			"int32_t repeat = FALSE;";
		$lines[] = 			"for(j = 0; j < i; j += op2) {";
		$lines[] = 				"if(op1_ptr[j] == op1_ptr[i]) {";
		$lines[] = 					"int32_t match = TRUE;";
		$lines[] = 					"for(k = 1; k < op2; k++) {";
		$lines[] = 						"if(op1_ptr[j + k] != op1_ptr[i + k]) {";
		$lines[] = 							"match = FALSE;";
		$lines[] = 							"break;";
		$lines[] = 						"}";
		$lines[] = 					"}";
		$lines[] = 					"if(match) {";
		$lines[] = 						"repeat = TRUE;";
		$lines[] = 						"break;";
		$lines[] =					"}";
		$lines[] = 				"}";
		$lines[] = 			"}";
		$lines[] = 			"if(!repeat) {";
		$lines[] =				"for(k = 0; k < op2; k++) {";
		$lines[] =					"*res_ptr = op1_ptr[i + k];";
		$lines[] =					"res_ptr++;";
		$lines[] =				"}";
		$lines[] = 			"}";
		$lines[] = 		"}";
		$lines[] = "}";
		return $lines;
	}
}

?>