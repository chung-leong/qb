<?php

class UTF8Encode extends Handler {

	use ArrayAddressMode, UnaryOperator;
	
	public function getOperandType($i) {
		switch($i) {
			case 1: return $this->operandType;
			case 2: return "U08";
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
		$lines[] = "uint32_t length = 0, i;";
		$lines[] = "for(i = 0; i < op1_count; i++) {";
		$lines[] = 		"length += encode(op1_ptr[i], res_ptr + length);";
		$lines[] = "}";
		return $lines;
	}
}

?>