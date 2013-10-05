<?php

class GuardIndex extends Handler {

	use ScalarAddressMode, BinaryOperator, MayEmitError;
	
	public function getOutputOperandCount() {
		return 0;
	}

	protected function getActionOnUnitData() {
		$lines = array();
		$lines[] = "if(UNEXPECTED(!(op1 < op2))) {";
		$lines[] =		"USE_TSRM";
		$lines[] =		"QB_G(current_line_number) = line_number;";
		$lines[] =		"qb_abort(\"Array index out of bounds (%d >= %d)\", op1, op2);";
		$lines[] = "}";
		return $lines;
	}
}

?>