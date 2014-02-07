<?php

class ArraySliceCount extends Handler {

	use MultipleAddressMode, TernaryOperator;
	
	public function getOperandType($i) {
		switch($i) {
			case 1: return "S32";				// index
			case 2: return "S32";				// length
			case 3: return "U32";				// source dimension
			case 4: return "U32";				// result
		}
	}
	
	public function getOperandAddressMode($i) {
		switch($i) {
			case 1: return $this->addressMode;	// index
			case 2: return $this->addressMode;	// length
			case 3: return "SCA";				// source dimension
			case 4: return "SCA";				// result
		}
	}
		
	public function getOperandSize($i) {
		switch($i) {
			case 1: return 1;
			case 2: return 1;
			case 3: return 1;
			case 4: return 1;
		}
	}
	
	public function getActionOnUnitData() {
		$lines = array();
		$lines[] = "uint32_t start_index, end_index;";
		$lines[] = "if(op1 >= 0) {";
		$lines[] =		"start_index = op1;";
		$lines[] = "} else {";
		$lines[] = 		"start_index = op3 + op1;";
		$lines[] = 		"if((int32_t) start_index < 0) {";
		$lines[] =			"start_index = 0;";
		$lines[] = 		"}";
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
		$lines[] = 		"res = (end_index - start_index);";
		$lines[] = "} else {";
		$lines[] =		"res = 0;";
		$lines[] = "}";
		return $lines;
	}
}

?>
