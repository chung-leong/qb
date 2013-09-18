<?php

class Min extends Handler {

	public function getInputOperandCount() {
		return 2;
	}

	protected function getActionOnUnitData() {
		return "res = (op1 < op2) ? op1 : op2;";
	}
}

?>