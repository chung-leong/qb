<?php

class QBArrayPadHandler extends QBHandler {

	public function getInputOperandCount() {
		return 3;
	}

	public function getOperandAddressMode($i) {
		switch($i) {
			case 1: return "ARR";
			case 2: return "ARR";
			case 3: return "VAR";
			case 4: return "ARR";
		}
	}
	
	public function getOperandType($i) {
		switch($i) {
			case 1: return $this->operandType;
			case 2: return $this->operandType;
			case 3: return "S32";
			case 4: return $this->operandType;
		}
	}
	
	public function getResultSizePossibilities() {
		return array("op1_count", "abs(op3) * op2_count");
	}
	
	public function getActionOnUnitData() {
		$type = $this->getOperandType(1);
		$cType = $this->getOperandCType(1);
		$lines = array();
		$lines[] = "if(res_count > op1_count) {";
		$lines[] =		"$cType *res_end;";
		$lines[] =		"$cType *op2_start = op2_ptr, *op2_end = op2_ptr + op2_count;";
		$lines[] =		"if(op3 > 0) {";
		$lines[] = 			"memcpy(res_ptr, op1_ptr, sizeof($cType) * op1_count);";
		$lines[] =			"res_end = res_ptr + res_count;";
		$lines[] =			"res_ptr += op1_count;";		
		$lines[] =		"} else {";
		$lines[] =			"res_end = res_ptr + (res_count - op1_count);";
		$lines[] = 			"memcpy(res_end, op1_ptr, sizeof($cType) * op1_count);";
		$lines[] =		"}";
		$lines[] =		"while(res_ptr < res_end) {";
		$lines[] =			"*res_ptr = *op2_ptr;";
		$lines[] =			"op2_ptr++;";
		$lines[] =			"res_ptr++;";
		$lines[] =			"if(op2_ptr >= op2_end) {";
		$lines[] = 				"op2_ptr = op2_start;";
		$lines[] =			"}";
		$lines[] =		"}";
		$lines[] = "} else {";		
		$lines[] = 		"memcpy(res_ptr, op1_ptr, sizeof($cType) * op1_count);";
		$lines[] = "}";
		return $lines;
	}
}

?>