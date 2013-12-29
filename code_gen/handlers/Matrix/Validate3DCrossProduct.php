<?php

class Validate3DCrossProduct extends Handler {

	use ScalarAddressMode, BinaryOperatorNoResult, MayEmitError;
	
	public function getActionOnUnitData() {
		$cType = $this->getOperandCType(1);
		$lines = array();
		$lines[] = "if(op1 != 3 || op2 != 3) {";
		$lines[] =		"qb_report_invalid_cross_product_exception(cxt->thread, line_id, op1, op2);";
		$lines[] =		"cxt->exit_type = QB_VM_ERROR;";
		$lines[] =		"return;";
		$lines[] = "}";
		return $lines;
	}
}

?>