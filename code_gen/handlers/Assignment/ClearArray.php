<?php

class ClearArray extends Handler {

	use ArrayAddressMode, NullaryOperator;
	
	public function changesOperandSize($i) {
		return ($i == 3);
	}
	
	public function getOperandType($i) {
		switch($i) {
			case 1: return $this->operandType;
		}
	}
	
	public function getOperandAddressMode($i) {
		switch($i) {
			case 1: return "ARR";
		}
	}
	
	protected function getActionOnUnitData() {
		$lines = array();
		$lines[] = "uint32_t i;";
		$lines[] = "for(i = 0; i < res_count; i++) {";
		$lines[] =		"res_ptr[i] = 0;";
		$lines[] = "}";
		$lines[] = "res_count = 0;";
		return $lines;
	}
}

?>