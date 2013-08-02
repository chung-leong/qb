<?php

class QBBranchOnGreaterThanOrEqualHandler extends QBBranchHandler {

	public function getInputOperandCount() {
		return 2;
	}

	protected function getActionOnUnitData() {
		return "condition = (op1 >= op2);";
	}
}

?>