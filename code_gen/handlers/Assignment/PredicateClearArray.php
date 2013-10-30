<?php

class PredicateClearArray extends Handler {

	use ArrayAddressMode, UnaryOperator;
	
	public function getOperandType($i) {
		switch($i) {
			case 1: return "I32";
			case 2: return $this->operandType;
		}
	}
	
	public function getOperandAddressMode($i) {
		switch($i) {
			case 1: return "SCA";
			case 2: return "ARR";
		}
	}
	
	public function getOperandSize($i) {
		switch($i) {
			case 1: return 1;
			case 2: return "res_count";
		}
	}
	
	protected function getActionOnUnitData() {
		$lines = array();
		$lines[] = "if(op1) {";
		$lines[] = 		"uint32_t i;";
		$lines[] = 		"for(i = 0; i < res_count; i++) {";
		$lines[] =			"res_ptr[i] = 0;";
		$lines[] = 		"}";
		$lines[] = "}";
		return $lines;
	}
}

?>