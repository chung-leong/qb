<?php

class ArrayFill extends Handler {

	use MultipleAddressMode, BinaryOperator;
	
	public function getOperandAddressMode($i) {
		switch($i) {
			case 1: return "SCA";
			case 2: return $this->addressMode;
			case 3: return "ARR";
		}
	}
	
	public function getOperandType($i) {
		switch($i) {
			case 1: return "U32";
			case 2: return $this->operandType;
			case 3: return $this->operandType;
		}
	}
	
	public function getOperandSize($i) {
		switch($i) {
			case 1: return 1;
			case 2: return ($this->addressMode == 2) ? "op2_count" : 1;
			case 3: return "res_count";
		}
	}
	
	public function getActionOnUnitData() {
		$cType = $this->getOperandCType(2);
		$lines = array();
		$lines[] = "$cType *res_end = res_ptr + res_count, *res_start = res_ptr + op1;";
		$lines[] = "while(res_ptr < res_start) {";
		$lines[] = 		"*res_ptr = 0;";
		$lines[] =		"res_ptr++;";
		$lines[] = "}";
		$lines[] = "while(res_ptr < res_end) {";
		$lines[] = 		"*res_ptr = op2;";
		$lines[] =		"res_ptr++;";
		$lines[] = "}";
		return $lines;
	}
	
	public function getActionOnMultipleData() {
		$cType = $this->getOperandCType(2);
		$lines = array();
		$lines[] = "$cType *res_end = res_ptr + res_count, *res_start = res_ptr + (op1 * op2_count);";
		$lines[] = "$cType *op2_end = op2_ptr + op2_count, *op2_start = op2_ptr;";
		$lines[] = "while(res_ptr < res_start) {";
		$lines[] = 		"*res_ptr = 0;";
		$lines[] =		"res_ptr++;";
		$lines[] = "}";
		$lines[] = "while(res_ptr < res_end) {";
		$lines[] =		"*res_ptr = *op2_ptr;";
		$lines[] =		"op2_ptr++;";
		$lines[] =		"res_ptr++;";
		$lines[] =		"if(op2_ptr >= op2_end) {";
		$lines[] = 			"op2_ptr = op2_start;";
		$lines[] =		"}";
		$lines[] = "}";
		return $lines;
	}
}

?>