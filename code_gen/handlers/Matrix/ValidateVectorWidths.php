<?php

class ValidateVectorWidths extends Handler {

	use ScalarAddressMode, BinaryOperatorNoResult, MayEmitError;
	
	public function getActionOnUnitData() {
		$cType = $this->getOperandCType(1);
		$lines = array();
		$lines[] = "if(op1 != op2) {";
		$lines[] =		"qb_report_vector_width_mismatch_exception(NULL, cxt->line_id, op1, op2);";
		$lines[] =		"cxt->exit_type = QB_VM_ERROR;";
		$lines[] =		"return;";
		$lines[] = "}";
		return $lines;
	}
}

?>