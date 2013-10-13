<?php

class AccommodateSize extends Handler {

	use ScalarAddressMode, QuaternaryOperator;
	
	public function getOutputOperandCount() {
		return 0;
	}

	public function changesOperand($i) {
		return ($i == 2);
	}
	
	public function needsInterpreterContext() {
		return true;
	}
	
	public function needsLocalStorage() {
		return true;
	}

	public function getOperandType($i) {
		switch($i) {
			case 1: return "U32";		// source size
			case 2: return "U32";		// destination size
			case 3: return "U32";		// segment selector
			case 4: return "U32";		// element byte-count
		}
	}
	
	public function getOperandAddressMode($i) {
		switch($i) {
			case 1: return "SCA";
			case 2: return "SCA";
			case 3: return "CON";
			case 4: return "CON";
		}
	}
	
	protected function getActionOnUnitData() {
		$lines = array();
		$lines[] = "if(UNEXPECTED(!(op1 == op2))) {";
		$lines[] =		"uint32_t new_dim = op1;";
		$lines[] =		"op2 = new_dim;";
		$lines[] = 		"qb_adjust_memory_segment(cxt, op3, new_dim * op4);";
		$lines[] = "}";
		return $lines;
	}
}

?>