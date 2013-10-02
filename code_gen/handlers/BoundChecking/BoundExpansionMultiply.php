<?php

class BoundExpansionMultiply extends Handler {

	use ScalarAddressMode, SenaryOperator;
	
	public function changesOperand($i) {
		return ($i == 2 || $i == 4 || $i == 7);
	}
	
	public function needsInterpreterContext() {
		return true;
	}
	
	public function needsLocalStorage() {
		return true;
	}

	public function getOperandType($i) {
		switch($i) {
			case 1: return "U32";		// index
			case 2: return "U32";		// dimension (i.e. the limit)
			case 3: return "U32";		// sub-array size
			case 4: return "U32";		// array-size
			case 5: return "U32";		// segment selector
			case 6: return "U32";		// element byte-count
			case 7: return "U32";		// result (index * sub-array-size)
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
			case 7: return "SCA";
		}
	}
	
	protected function getActionOnUnitData() {
		$lines = array();
		$lines[] = "res = op1 * op3;";
		$lines[] = "if(UNEXPECTED(op1 >= op2)) {";
		$lines[] =		"uint32_t new_dim = op1 + 1;";
		$lines[] =		"uint32_t new_size = new_dim * op3;";
		$lines[] =		"op4 = new_size;";
		$lines[] =		"op2 = new_dim;";
		$lines[] = 		"qb_resize_array(cxt, local_storage, op5, new_size * op6);";
		$lines[] = "}";
		return $lines;
	}
}

?>