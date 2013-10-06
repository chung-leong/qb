<?php

class PredicateClearElement extends Handler {

	use ArrayAddressMode, TernaryOperator;

	public function getOperandType($i) {
		switch($i) {
			case 1: return "U32";				// element index
			case 2: return "U32";				// element count
			case 3: return "I32";				// predicate
			case 4: return $this->operandType;	// array containing element
		}
	}
	
	public function getOperandAddressMode($i) {
		switch($i) {
			case 1: return "SCA";
			case 2: return "SCA";
			case 3: return "SCA";
			case 4: return "ARR";
		}
	}
	
	protected function getActionOnUnitData() {
		$lines = array();
		$lines[] = "if(op3) {";
		$lines[] = 		"uint32_t i, shift = op2, start = op1 * op2, end = res_count - shift;";
		$lines[] =		"if(start < end) {";
		$lines[] = 			"for(i = start; i < end; i++) {";
		$lines[] =				"res_ptr[i] = res_ptr[i + shift];";
		$lines[] = 			"}";
		$lines[] = 			"for(i = end; i < res_count; i++) {";
		$lines[] =				"res_ptr[i] = 0;";
		$lines[] = 			"}";
		$lines[] =		"}";
		$lines[] = "}";
		return $lines;
	}
}

?>