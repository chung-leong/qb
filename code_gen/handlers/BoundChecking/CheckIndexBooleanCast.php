<?php

class CheckIndexBooleanCast extends Handler {

	use MultipleAddressMode, QuaternaryOperator;

	public function changesOperand($i) {
		return ($i == 4 || $i == 5);
	}
	
	public function getOperandType($i) {
		switch($i) {
			case 1: return "U32";				// index
			case 2: return "U32";				// dimension (i.e. the limit)
			case 3: return $this->operandType;	// the element
			case 4: return "I32";				// predicate
			case 5: return "I32";				// result (boolean)
		}
	}
	
	public function getOperandAddressMode($i) {
		switch($i) {
			case 1: return "SCA";
			case 2: return "SCA";
			case 3: return $this->addressMode;
			case 4: return "SCA";
			case 5: return "SCA";
		}
	}
	
	protected function getActionOnUnitData() {
		$lines = array();
		$lines[] = "if(UNEXPECTED(!op4 || !(op1 < op2))) {";
		$lines[] = 		"res = FALSE;";
		$lines[] = "} else {";
		$lines[] = 		"res = (op3 != 0);";
		$lines[] = "}";
		return $lines;
	}
}

?>