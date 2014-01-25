<?php

class ValidateTransform extends Handler {

	use ScalarAddressMode, TernaryOperatorNoResult, MayEmitError;
	
	public function getActionOnUnitData() {
		$cType = $this->getOperandCType(1);
		$lines = array();
		$lines[] = "if(!(op3 >= 2 && op3 <= 4 && op1 == op3 + 1 && op2 == op3)) {";
		$lines[] =		"qb_report_invalid_transform_exception(line_id, op1, op2, op3);";
		$lines[] =		"cxt->exit_type = QB_VM_ERROR;";
		$lines[] =		"return;";
		$lines[] = "}";
		return $lines;
	}
}

?>