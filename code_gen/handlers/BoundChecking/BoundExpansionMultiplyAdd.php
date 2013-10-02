<?php

class BoundExpansionMultiplyAdd extends Handler {

	use ScalarAddressMode, SeptenaryOperator;
	
	public function changesOperand($i) {
		return ($i == 2 || $i == 5 || $i == 8);
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
			case 4: return "U32";		// offset
			case 5: return "U32";		// array-size
			case 6: return "U32";		// segment selector
			case 7: return "U32";		// element byte-count
			case 8: return "U32";		// result (index * sub-array-size + offset)
		}
	}
	
	public function getOperandAddressMode($i) {
		switch($i) {
			case 1: return "SCA";
			case 2: return "SCA";
			case 3: return "SCA";
			case 4: return "SCA";
			case 5: return "SCA";
			case 6: return "CON";
			case 7: return "CON";
			case 8: return "SCA";
		}
	}
	
	protected function getActionOnUnitData() {
		$lines = array();
		$lines[] = "res = op1 * op3 + op4;";
		$lines[] = "if(UNEXPECTED(op1 >= op2)) {";
		$lines[] =		"uint32_t new_dim = op1 + 1;";
		$lines[] =		"uint32_t new_size = new_dim * op3;";
		$lines[] =		"op5 = new_size;";
		$lines[] =		"op2 = new_dim;";
		$lines[] = 		"qb_resize_array(cxt, local_storage, op6, new_size * op7);";
		$lines[] = "}";
		return $lines;
	}
}

?>