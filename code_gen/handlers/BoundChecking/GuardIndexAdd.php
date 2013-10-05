<?php

class GuardIndexAdd extends Handler {

	use ScalarAddressMode, TernaryOperator, MayEmitError;

	public function getOperandType($i) {
		switch($i) {
			case 1: return "U32";		// index
			case 2: return "U32";		// dimension (i.e. the limit)
			case 3: return "U32";		// offset
			case 4: return "U32";		// result (index + offset)
		}
	}
	
	protected function getActionOnUnitData() {
		$lines = array();
		$lines[] = "res = op1 + op3;";
		$lines[] = "if(UNEXPECTED(!(op1 < op2))) {";
		$lines[] =		"USE_TSRM";
		$lines[] =		"QB_G(current_line_number) = line_number;";
		$lines[] =		"qb_abort(\"Array index out of bounds (%d >= %d)\", op1, op2);";
		$lines[] = "}";
		return $lines;
	}
}

?>