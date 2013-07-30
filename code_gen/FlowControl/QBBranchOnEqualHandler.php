<?php

class QBBranchOnEqualHandler extends QBBranchHandler {

	public function getInputOperandCount() {
		return 2;
	}

	public function getAction() {
		return "condition = (op1 == op2);";
	}
}

?>