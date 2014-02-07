<?php

class UTF8Decode extends Handler {

	use ArrayAddressMode, UnaryOperator;

	public function getOperandType($i) {
		switch($i) {
			case 1: return "U08";
			case 2: return $this->operandType;
		}
	}
	
	public function getOperandSize($i) {
		switch($i) {
			case 1: return "op1_count";
			case 2: return "res_count";
		}
	}
	
	public function getActionOnUnitData() {
		$lines = array();
		$lines[] = "uint32_t codepoint, state = 0, i, j;";
		$lines[] = "for(i = 0, j = 0; i < op1_count; i++) {";
		$lines[] = 		"if(!decode(&state, &codepoint, op1_ptr[i])) {";
		$lines[] = 			"res_ptr[j++] = codepoint;";
		$lines[] = 		"}";
		$lines[] = "}";
		return $lines;
	}
}

?>