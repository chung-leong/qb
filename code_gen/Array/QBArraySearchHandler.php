<?php

class QBArraySearchHandler extends QBHandler {

	public function getInputOperandCount() {
		return 2;
	}

	public function getOperandAddressMode($i) {
		switch($i) {
			case 1: return "ARR";
			case 2: return $this->addressMode;
			case 3: return "VAR";
		}
	}
	
	public function getOperandType($i) {
		switch($i) {
			case 1: return $this->operandType;
			case 2: return $this->operandType;
			case 3: return "I32";
		}
	}
	
	protected function getActionOnUnitData() {
		$type = $this->getOperandType(1);
		$cType = $this->getOperandCType(1);
		$lines = array();
		$lines[] = "int32_t index = -1;";
		$lines[] = "uint32_t i;";
		$lines[] = "for(i = 0; i < op1_count; i++) {";
		$lines[] = 		"if(op1_ptr[i] == op2_ptr[0]) {";
		$lines[] = 			"index = i;";
		$lines[] =			"break;";
		$lines[] = 		"}";
		$lines[] = "}";
		$lines[] = "res = index;";
		return $lines;
	}
	
	protected function getActionOnMultipleData() {
		$type = $this->getOperandType(1);
		$cType = $this->getOperandCType(1);
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