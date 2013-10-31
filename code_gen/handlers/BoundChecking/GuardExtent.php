<?php

class GuardExtent extends Handler {

	use ScalarAddressMode, TernaryOperator, MayEmitError;

	public function getOutputOperandCount() {
		return 0;
	}

	public function getOperandType($i) {
		switch($i) {
			case 1: return "U32";		// index
			case 2: return "U32";		// length
			case 3: return "U32";		// dimension (i.e. the limit)
		}
	}
	
	protected function getActionOnUnitData() {
		$lines = array();
		$lines[] = "if(UNEXPECTED(!(op1 + op2 <= op3))) {";
		$lines[] =		"USE_TSRM";
		$lines[] =		"QB_G(current_line_number) = line_number;";
		$lines[] =		"qb_abort(\"Array index out of bounds (%d + %d > %d)\", op1, op2, op3);";
		$lines[] = "}";
		return $lines;
	}
}

?>