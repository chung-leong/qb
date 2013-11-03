<?php

class GuardSize extends Handler {

	use ScalarAddressMode, BinaryOperator, MayEmitError;
	
	public function getOutputOperandCount() {
		return 0;
	}

	protected function getActionOnUnitData() {
		$lines = array();
		$lines[] = "if(UNEXPECTED(!(op1 <= op2))) {";
		$lines[] =		"qb_trigger_out_of_bound_exception(cxt, op1, op2, TRUE, line_number);";
		$lines[] = "}";
		return $lines;
	}
}

?>