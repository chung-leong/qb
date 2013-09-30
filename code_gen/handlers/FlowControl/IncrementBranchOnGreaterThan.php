<?php

class IncrementBranchOnGreaterThan extends Handler {

	use MultipleAddressMode, UnaryOperator, BranchInstruction {
        UnaryOperator::getOutputOperandCount insteadof BranchInstruction;
	}

	protected function getActionOnUnitData() {
		return "condition = (op1 > ++res);";
	}
}

?>