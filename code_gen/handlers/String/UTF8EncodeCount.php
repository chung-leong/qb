<?php

class UTF8EncodeCount extends Handler {

	use ScalarAddressMode, UnaryOperator;
	
	public function getOperandType($i) {
		switch($i) {
			case 1: return $this->operandType;
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
		$lines[] = "uint32_t length = 0, i;";
		$lines[] = "for(i = 0; i < op1_count; i++) {";
		$lines[] = 		"uint8_t buffer[4];";
		$lines[] = 		"length += encode(op1_ptr[i], buffer);";
		$lines[] = "}";
		$lines[] = "res = length;";
		return $lines;
	}
}

?>