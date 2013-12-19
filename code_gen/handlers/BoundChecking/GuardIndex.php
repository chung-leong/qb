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
		$lines[] =		"qb_record_out_of_bound_exception(op1, op2, FALSE, line_id TSRMLS_CC);";
		$lines[] =		"cxt->exit_type = QB_VM_BAILOUT;";
		$lines[] =		"return;";
		$lines[] = "}";
		return $lines;
	}
}

?>