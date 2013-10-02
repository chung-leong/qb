<?php

class BoundCheckBooleanCast extends Handler {

	use MultipleAddressMode, TernaryOperator;

	public function getOperandType($i) {
		switch($i) {
			case 1: return "U32";				// index
			case 2: return "U32";				// dimension (i.e. the limit)
			case 3: return $this->operandType;	// the element
			case 4: return "I32";				// result (boolean)
		}
	}
	
	public function getOperandAddressMode($i) {
		switch($i) {
			case 1: return "SCA";
			case 2: return "SCA";
			case 3: return $this->addressMode;
			case 4: return "SCA";
		}
	}
	
	protected function getActionOnUnitData() {
		$lines = array();
		$lines[] = "if(op1 < op2) {";
		$lines[] = 		"res = (op3 != 0);";
		$lines[] = "} else {";
		$lines[] = 		"res = FALSE;";
		$lines[] = "}";
		return $lines;
	}
}

?>