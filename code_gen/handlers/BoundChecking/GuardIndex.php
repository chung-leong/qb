<?php

class GuardIndex extends Handler {

	use ScalarAddressMode, BinaryOperatorNoResult, MayEmitError;
	
	protected function getActionOnUnitData() {
		$lines = array();
		$lines[] = "if(UNEXPECTED(!(op1 < op2))) {";
		$lines[] =		"qb_report_out_of_bound_exception(cxt->thread, line_id, op1, op2, FALSE);";
		$lines[] =		"cxt->exit_type = QB_VM_ERROR;";
		$lines[] =		"return;";
		$lines[] = "}";
		return $lines;
	}
}

?>
