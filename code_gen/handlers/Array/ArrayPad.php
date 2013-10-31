<?php

class ArrayPad extends Handler {

	use ArrayAddressMode, TernaryOperator;
	
	public function getOperandAddressMode($i) {
		switch($i) {
			case 1: return "ARR";
			case 2: return "SCA";
			case 3: return "ARR";
			case 4: return "ARR";
		}
	}
	
	public function getOperandType($i) {
		switch($i) {
			case 1: return $this->operandType;
			case 2: return "S32";
			case 3: return $this->operandType;
			case 4: return $this->operandType;
		}
	}
	
	public function getOperandSize($i) {
		switch($i) {
			case 1: return "op1_count";
			case 2: return 1;
			case 3: return "op3_count";
			case 4: return "res_count";
		}
	}
	
	public function getActionOnUnitData() {
		$type = $this->getOperandType(1);
		$cType = $this->getOperandCType(1);
		$lines = array();
		$lines[] = "if(res_count > op1_count) {";
		$lines[] =		"$cType *res_end;";
		$lines[] =		"$cType *op3_start = op3_ptr, *op3_end = op3_ptr + op3_count;";
		$lines[] =		"if(op2 > 0) {";
		$lines[] = 			"memcpy(res_ptr, op1_ptr, sizeof($cType) * op1_count);";
		$lines[] =			"res_end = res_ptr + res_count;";
		$lines[] =			"res_ptr += op1_count;";		
		$lines[] =		"} else {";
		$lines[] =			"res_end = res_ptr + (res_count - op1_count);";
		$lines[] = 			"memcpy(res_end, op1_ptr, sizeof($cType) * op1_count);";
		$lines[] =		"}";
		$lines[] =		"while(res_ptr < res_end) {";
		$lines[] =			"*res_ptr = *op3_ptr;";
		$lines[] =			"op3_ptr++;";
		$lines[] =			"res_ptr++;";
		$lines[] =			"if(op3_ptr >= op3_end) {";
		$lines[] = 				"op3_ptr = op3_start;";
		$lines[] =			"}";
		$lines[] =		"}";
		$lines[] = "} else {";		
		$lines[] = 		"memcpy(res_ptr, op1_ptr, sizeof($cType) * op1_count);";
		$lines[] = "}";
		return $lines;
	}
}

?>