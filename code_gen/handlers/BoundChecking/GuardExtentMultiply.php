<?php

class GuardExtentMultiply extends Handler {

	use ScalarAddressMode, QuaternaryOperator, MayEmitError;

	public function getOperandType($i) {
		switch($i) {
			case 1: return "U32";		// index
			case 2: return "U32";		// length
			case 3: return "U32";		// dimension (i.e. the limit)
			case 4: return "U32";		// sub-array size
			case 5: return "U32";		// result (index * sub-array-size)
		}
	}
	
	protected function getActionOnUnitData() {
		$lines = array();
		$lines[] = "res = op2 * op4;";
		$lines[] = "if(UNEXPECTED(!(op1 + op2 < op3))) {";
		$lines[] =		"USE_TSRM";
		$lines[] =		"QB_G(current_line_number) = line_number;";
		$lines[] =		"qb_abort(\"Array index out of bounds (%d + %d > %d)\", op1, op2, op3);";
		$lines[] = "}";
		return $lines;
	}
}

?>