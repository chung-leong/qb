<?php

class QBArrayUniqueHandler extends QBHandler {

	public function getInputOperandCount() {
		return 2;
	}

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
			case 2: return "VAR";
			case 3: return "ARR";
		}
	}
	
	public function getHelperFunctions() {
		$type = $this->getOperandType(1);
		$cType = $this->getOperandCType(1);
		$lines = array();
		$lines[] = "uint32_t qb_get_unique_element_count_$type($cType *op1_ptr, uint32_t op1_count, uint32_t op2) {";
		$lines[] = 		"uint32_t count = 0;";
		$lines[] = 		"uint32_t i, j, k;";
		$lines[] = 		"if(op2 == 1) {";
		$lines[] = 			"for(i = 0; i < op1_count; i++) {";
		$lines[] = 				"int32_t repeat = FALSE;";
		$lines[] = 				"for(j = 0; j < i; j++) {";
		$lines[] = 					"if(op1_ptr[j] == op1_ptr[i]) {";
		$lines[] = 						"repeat = TRUE;";
		$lines[] = 						"break;";
		$lines[] = 					"}";
		$lines[] = 				"}";
		$lines[] = 				"if(!repeat) {";
		$lines[] = 					"count++;";
		$lines[] = 				"}";
		$lines[] = 			"}";
		$lines[] = 		"} else {";
		$lines[] = 			"for(i = 0; i < op1_count; i += op2) {";
		$lines[] = 				"int32_t repeat = FALSE;";
		$lines[] = 				"for(j = 0; j < i; j += op2) {";
		$lines[] = 					"if(op1_ptr[j] == op1_ptr[i]) {";
		$lines[] = 						"int32_t match = TRUE;";
		$lines[] = 						"for(k = 1; k < op2; k++) {";
		$lines[] = 							"if(op1_ptr[j + k] == op1_ptr[i + k]) {";
		$lines[] = 								"match = FALSE;";
		$lines[] = 								"break;";
		$lines[] = 							"}";
		$lines[] = 						"}";
		$lines[] = 						"if(match) {";
		$lines[] = 							"repeat = TRUE;";
		$lines[] = 							"break;";
		$lines[] =						"}";
		$lines[] = 					"}";
		$lines[] = 				"}";
		$lines[] = 				"if(!repeat) {";
		$lines[] = 					"count += op2;";
		$lines[] = 				"}";
		$lines[] = 			"}";
		$lines[] = 		"}";
		$lines[] = 		"return count;";
		$lines[] = "}";
		return array($lines);
	}
	
	public function getResultSizePossibilities() {
		return "string_length";
	}

	public function getResultSizeCalculation() {
		$type = $this->getOperandType(1);
		return "string_length = qb_get_unique_element_count_$type(op1_ptr, op1_count, op2);";
	}		

	public function getActionOnUnitData() {
		$type = $this->getOperandType(1);
		$cType = $this->getOperandCType(1);
		$lines = array();
		$lines[] = "uint32_t i, j;";
		$lines[] = "for(i = 0; i < op1_count; i++) {";
		$lines[] = 		"int32_t repeat = FALSE;";
		$lines[] = 		"for(j = 0; j < i; j++) {";
		$lines[] = 			"if(op1_ptr[j] == op1_ptr[i]) {";
		$lines[] = 				"repeat = TRUE;";
		$lines[] = 				"break;";
		$lines[] = 			"}";
		$lines[] = 		"}";
		$lines[] = 		"if(!repeat) {";
		$lines[] =			"*res_ptr = op1_ptr[i];";
		$lines[] = 			"res_ptr++;";
		$lines[] = 		"}";
		$lines[] = "}";
		return $lines;
	}
}

?>