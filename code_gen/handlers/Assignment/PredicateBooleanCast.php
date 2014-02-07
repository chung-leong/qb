<?php

class PredicateBooleanCast extends Handler {

	use MultipleAddressMode, BinaryOperator;
	
	public function getHandlerFunctionType() {
		// since the reference to op1 might not be valid
		// this handler can never be placed in a function
		return null;
	}

	public function getOperandType($i) {
		switch($i) {
			case 1: return $this->operandType;	// the element
			case 2: return "I32";				// predicate
			case 3: return "I32";				// result (boolean)
		}
	}
	
	public function getOperandAddressMode($i) {
		switch($i) {
			case 1: return $this->addressMode;
			case 2: return "SCA";
			case 3: return "SCA";
		}
	}
	
	protected function getActionOnUnitData() {
		$lines = array();
		$lines[] = "if(!op2) {";
		$lines[] = 		"res = FALSE;";
		$lines[] = "} else {";
		$lines[] = 		"res = (op1 != 0);";
		$lines[] = "}";
		return $lines;
	}
}

?>