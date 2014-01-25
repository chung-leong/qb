<?php

class ValidateMatrixByVector extends Handler {

	use ScalarAddressMode, BinaryOperatorNoResult, MayEmitError;
	
	public function getActionOnUnitData() {
		$cType = $this->getOperandCType(1);
		$lines = array();
		$lines[] = "if(op1 != op2) {";
		$lines[] =		"qb_report_invalid_matrix_multiplication_exception(line_id, op1, op2, 1);";
		$lines[] =		"cxt->exit_type = QB_VM_ERROR;";
		$lines[] =		"return;";
		$lines[] = "}";
		return $lines;
	}
}

?>