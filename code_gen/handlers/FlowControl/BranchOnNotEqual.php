<?php

class BranchOnNotEqual extends Handler {

	public function getInputOperandCount() {
		return 2;
	}

	protected function getActionOnUnitData() {
		return "condition = (op1 != op2);";
	}
}

?>