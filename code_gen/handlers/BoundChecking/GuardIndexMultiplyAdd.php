<?php

class GuardIndexMultiplyAdd extends Handler {

	use ScalarAddressMode, QuaternaryOperator, MayEmitError;

	public function getOperandType($i) {
		switch($i) {
			case 1: return "U32";		// index
			case 2: return "U32";		// dimension (i.e. the limit)
			case 3: return "U32";		// sub-array size
			case 4: return "U32";		// offset
			case 5: return "U32";		// result (index + offset)
		}
	}
	
	protected function getActionOnUnitData() {
		$lines = array();
		$lines[] = "res = op1 * op3 + op4;";
		$lines[] = "if(UNEXPECTED(!(op1 < op2))) {";
		$lines[] =		"qb_report_out_of_bound_exception(line_id, op1, op2, FALSE);";
		$lines[] =		"cxt->exit_type = QB_VM_ERROR;";
		$lines[] =		"return;";
		$lines[] = "}";
		return $lines;
	}
}

?>
