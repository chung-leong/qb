<?php

class CheckIndex extends Handler {

	use ScalarAddressMode, QuaternaryOperator, MayEmitError;

	public function getOutputOperandCount() {
		return 0;
	}
	
	public function changesOperand($i) {
		return ($i == 3);
	}
	
	public function getOperandType($i) {
		switch($i) {
			case 1: return "U32";		// index
			case 2: return "U32";		// dimension (i.e. the limit)
			case 3: return "I32";		// predicate
		}
	}
	
	protected function getActionOnUnitData() {
		$lines = array();
		$lines[] = "if(UNEXPECTED(!(op1 < op2))) {";
		$lines[] = 		"op3 = FALSE;";
		$lines[] = "} else {";
		$lines[] = 		"op3 = TRUE;";
		$lines[] = "}";
		return $lines;
	}
}

?>