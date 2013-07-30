<?php

class QBBranchOnGreaterThanHandler extends QBBranchHandler {

	public function getInputOperandCount() {
		return 2;
	}

	protected function getScalarExpression() {
		return "condition = (op1 > op2);";
	}
}

?>