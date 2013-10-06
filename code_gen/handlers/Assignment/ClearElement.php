<?php

class ClearElement extends Handler {

	use ArrayAddressMode, BinaryOperator;

	public function getOperandType($i) {
		switch($i) {
			case 1: return "U32";				// element index
			case 2: return "U32";				// element count
			case 3: return $this->operandType;	// array containing element
		}
	}
	
	public function getOperandAddressMode($i) {
		switch($i) {
			case 1: return "SCA";
			case 2: return "SCA";
			case 3: return "ARR";
		}
	}
	
	protected function getActionOnUnitData() {
		$lines = array();
		$lines[] = "uint32_t i, shift = op2, start = op1 * op2, end = res_count - shift;";
		$lines[] = "for(i = start; i < end; i++) {";
		$lines[] =		"res_ptr[i] = res_ptr[i + shift];";
		$lines[] = "}";
		$lines[] = "for(i = end; i < res_count; i++) {";
		$lines[] =		"res_ptr[i] = 0;";
		$lines[] = "}";
		return $lines;
	}
}

?>