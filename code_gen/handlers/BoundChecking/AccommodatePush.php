<?php

class AccommodatePush extends Handler {

	use ScalarAddressMode, TernaryOperator;
	
	public function changesOperand($i) {
		return ($i == 1 || $i == 4);
	}
	
	public function needsInterpreterContext() {
		return true;
	}
	
	public function getOperandType($i) {
		switch($i) {
			case 1: return "U32";		// dimension
			case 2: return "U32";		// segment selector
			case 3: return "U32";		// element byte-count
			case 4: return "U32";		// result (the dimension prior to the expansion)
		}
	}
	
	public function getOperandAddressMode($i) {
		switch($i) {
			case 1: return "SCA";
			case 2: return "CON";
			case 3: return "CON";
			case 4: return "SCA";
		}
	}
	
	protected function getActionOnUnitData() {
		$lines = array();
		$lines[] = "uint32_t new_dim = op1 + 1;";
		$lines[] = "res = op1;";
		$lines[] = "op1 = new_dim;";
		$lines[] = "qb_resize_segment(&cxt->function->local_storage->segments[op2], new_dim * op3);";
		return $lines;
	}
}

?>
