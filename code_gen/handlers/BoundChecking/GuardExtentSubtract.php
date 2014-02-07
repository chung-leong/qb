<?php

class GuardExtentSubtract extends Handler {

	use ScalarAddressMode, BinaryOperator, MayEmitError;

	public function getOperandType($i) {
		switch($i) {
			case 1: return "U32";		// dimension
			case 2: return "U32";		// index
			case 3: return "U32";		// result (index * sub-array-size)
		}
	}
	
	protected function getActionOnUnitData() {
		$lines = array();
		$lines[] = "res = op1 - op2;";
		$lines[] = "if(UNEXPECTED(!(op2 <= op1))) {";
		$lines[] = "}";
		return $lines;
	}
}

?>