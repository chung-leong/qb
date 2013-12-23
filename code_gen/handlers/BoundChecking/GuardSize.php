<?php

class GuardSize extends Handler {

	use ScalarAddressMode, BinaryOperator, MayEmitError;
	
	public function getOutputOperandCount() {
		return 0;
	}

	protected function getActionOnUnitData() {
		$lines = array();
		$lines[] = "if(UNEXPECTED(!(op1 <= op2))) {";
		$lines[] =		"USE_TSRM";
		$lines[] =		"qb_report_out_of_bound_exception(cxt->thread, line_id, op1, op2, TRUE);";
		$lines[] =		"cxt->exit_type = QB_VM_ERROR;";
		$lines[] =		"return;";
		$lines[] = "}";
		return $lines;
	}
}

?>