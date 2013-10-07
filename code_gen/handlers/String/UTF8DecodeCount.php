<?php

class UTF8DecodeCount extends Handler {

	use ScalarAddressMode, UnaryOperator;

	public function getOperandType($i) {
		switch($i) {
			case 1: return "U08";
			case 2: return "U32";
		}
	}
	
	public function getOperandAddressMode($i) {
		switch($i) {
			case 1: return "ARR";
			case 2: return "SCA";
		}
	}
	
	public function getActionOnUnitData() {
		$lines = array();
		$lines[] = "uint32_t codepoint, state = 0, count = 0, i;";
		$lines[] = "for(i = 0; i < op1_count; i++) {";
		$lines[] = 		"if(!decode(&state, &codepoint, op1_ptr[i])) {";
		$lines[] = 			"count++;";
		$lines[] = 		"}";
		$lines[] = "}";
		$lines[] = "res = count;";
		return $lines;
	}
}

?>