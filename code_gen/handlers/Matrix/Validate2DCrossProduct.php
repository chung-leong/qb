<?php

class Validate2DCrossProduct extends Handler {

	use ScalarAddressMode, BinaryOperatorNoResult, MayEmitError;
	
	public function getActionOnUnitData() {
		$cType = $this->getOperandCType(1);
		$lines = array();
		$lines[] = "if(op1 != 2 || op2 != 2) {";
		$lines[] =		"qb_report_invalid_cross_product_exception(cxt->thread, line_id, op1, op2);";
		$lines[] =		"cxt->exit_type = QB_VM_ERROR;";
		$lines[] =		"return;";
		$lines[] = "}";
		return $lines;
	}
}

?>