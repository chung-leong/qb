<?php

class Validate4DCrossProduct extends Handler {

	use ScalarAddressMode, TernaryOperatorNoResult, MayEmitError;
	
	public function getActionOnUnitData() {
		$cType = $this->getOperandCType(1);
		$lines = array();
		$lines[] = "if(op1 != 4 || op2 != 4 || op3 != 4) {";
		$lines[] =		"qb_report_invalid_4d_cross_product_exception(cxt->thread, line_id, op1, op2, op3);";
		$lines[] =		"cxt->exit_type = QB_VM_ERROR;";
		$lines[] =		"return;";
		$lines[] = "}";
		return $lines;
	}
}

?>