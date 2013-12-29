<?php

class IncrementBranchOnGreaterThan extends Handler {

	use MultipleAddressMode, UnaryOperator, BranchInstruction;

	protected function getActionOnUnitData() {
		return "condition = (op1 > ++res);";
	}
}

?>