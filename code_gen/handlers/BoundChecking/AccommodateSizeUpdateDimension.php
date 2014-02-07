<?php

class AccommodateSizeUpdateDimension extends Handler {

	use ScalarAddressMode, SenaryOperator, NotThreadSafe;
	
	public function getOutputOperandCount() {
		return 0;
	}

	public function changesOperand($i) {
		return ($i == 2 || $i == 3);
	}
	
	public function needsInterpreterContext() {
		return true;
	}
	
	public function getOperandType($i) {
		switch($i) {
			case 1: return "U32";		// source size
			case 2: return "U32";		// destination size
			case 3: return "U32";		// destination dimension
			case 4: return "U32";		// destination sub-array size
			case 5: return "U32";		// segment selector
			case 6: return "U32";		// element byte-count
		}
	}
	
	public function getOperandAddressMode($i) {
		switch($i) {
			case 1: return "SCA";
			case 2: return "SCA";
			case 3: return "SCA";
			case 4: return "SCA";
			case 5: return "CON";
			case 6: return "CON";
		}
	}
	
	protected function getActionOnUnitData() {
		$lines = array();
		$lines[] = "if(UNEXPECTED(!(op1 == op2))) {";
		$lines[] =		"uint32_t new_dim = op1 / op4;";		
		$lines[] =		"uint32_t new_size = new_dim * op4;";
		$lines[] =		"if(UNEXPECTED(new_size < op1)) {";
		$lines[] =			"new_dim++;";
		$lines[] =			"new_size = new_dim * op4;";
		$lines[] =		"}";
		$lines[] =		"op2 = new_size;";
		$lines[] =		"op3 = new_dim;";
		$lines[] = 		"qb_resize_segment(&cxt->function->local_storage->segments[op5], new_size * op6);";
		$lines[] = "}";
		return $lines;
	}
}

?>
