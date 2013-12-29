<?php

class BranchOnLessThan extends Handler {

	use MultipleAddressMode, BinaryOperatorNoResult, BranchInstruction;

	protected function getActionOnUnitData() {
		return "condition = (op1 < op2);";
	}
}

?>