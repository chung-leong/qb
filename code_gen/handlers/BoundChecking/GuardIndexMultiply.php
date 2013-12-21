<?php

class GuardIndexMultiply extends Handler {

	use ScalarAddressMode, TernaryOperator, MayEmitError;

	public function getOperandType($i) {
		switch($i) {
			case 1: return "U32";		// index
			case 2: return "U32";		// dimension (i.e. the limit)
			case 3: return "U32";		// sub-array size
			case 4: return "U32";		// result (index * sub-array-size)
		}
	}
	
	protected function getActionOnUnitData() {
		$lines = array();
		$lines[] = "res = op1 * op3;";
		$lines[] = "if(UNEXPECTED(!(op1 < op2))) {";
		$lines[] =		"USE_TSRM";
		$lines[] =		"qb_record_out_of_bound_exception(cxt->thread, line_id, op1, op2, FALSE);";
		$lines[] =		"cxt->exit_type = QB_VM_BAILOUT;";
		$lines[] =		"return;";
		$lines[] = "}";
		return $lines;
	}
}

?>