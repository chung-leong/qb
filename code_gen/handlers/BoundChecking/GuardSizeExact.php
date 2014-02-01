<?php

class GuardSizeExact extends Handler {

	use ScalarAddressMode, BinaryOperatorNoResult, MayEmitError;
	
	protected function getActionOnUnitData() {
		$lines = array();
		$lines[] = "if(UNEXPECTED(!(op1 == op2))) {";
		$lines[] =		"void qb_report_element_size_mismatch_exception(line_id, op1, op2);";
		$lines[] =		"cxt->exit_type = QB_VM_ERROR;";
		$lines[] =		"return;";
		$lines[] = "}";
		return $lines;
	}
}

?>
