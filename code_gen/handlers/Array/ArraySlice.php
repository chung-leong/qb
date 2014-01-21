<?php

class ArraySlice extends Handler {

	use MultipleAddressMode, QuinaryOperator;
	
	public function getOperandType($i) {
		switch($i) {
			case 1: return "S32";				// index
			case 2: return "S32";				// length
			case 3: return "U32";				// source dimension
			case 4: return "U32";				// source sub-array size
			case 5: return $this->operandType;	// source array
			case 6: return $this->operandType;	// result
		}
	}
	
	public function getOperandAddressMode($i) {
		switch($i) {
			case 1: return $this->addressMode;	// index
			case 2: return $this->addressMode;	// length
			case 3: return "SCA";				// source dimension
			case 4: return "SCA";				// source sub-array size
			case 5: return "ARR";				// source array
			case 6: return "ARR";				// result
		}
	}
		
	public function getOperandSize($i) {
		switch($i) {
			case 1: return 1;
			case 2: return 1;
			case 3: return 1;
			case 4: return 1;
			case 5: return "op5_count";
			case 6: return "res_count";
		}
	}
	
	public function getActionOnUnitData() {
		$type = $this->getOperandType(5);
		$cType = $this->getOperandCType(5);
		$lines = array();
		$lines[] = "$cType *res_end = res_ptr + res_count;";
		$lines[] = "uint32_t start_index, end_index;";
		$lines[] = "if(op1 >= 0) {";
		$lines[] =		"start_index = op1;";
		$lines[] = "} else {";
		$lines[] = 		"start_index = (op3 > (uint32_t) -op1) ? op3 + op1 : 0;";
		$lines[] = "}";
		$lines[] = "if(op2 >= 0) {";
		$lines[] =		"end_index = start_index + op2;";
		$lines[] =		"if(end_index > op3) {";
		$lines[] =			"end_index = op3;";
		$lines[] =		"}";
		$lines[] = "} else {";
		$lines[] =		"end_index = op3 + op2;";
		$lines[] = "}";
		$lines[] = "if(start_index < end_index) {";
		$lines[] =		"$cType *ptr = op5_ptr + (start_index * op4);";
		$lines[] =		"while(res_ptr < res_end) {";
		$lines[] =			"*res_ptr = *ptr;";
		$lines[] =			"res_ptr++;";
		$lines[] =			"ptr++;";
		$lines[] =		"}";
		$lines[] = "} else {";
		$lines[] = 		"while(res_ptr < res_end) {";
		$lines[] =			"*res_ptr = 0;";
		$lines[] =			"res_ptr++;";
		$lines[] =		"}";
		$lines[] = "}";
		return $lines;
	}
}

?>