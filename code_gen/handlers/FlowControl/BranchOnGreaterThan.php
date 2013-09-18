<?php

class BranchOnGreaterThan extends Handler {

	use MultipleAddressMode, BinaryOperator, BranchInstruction {
        BranchInstruction::getOutputOperandCount insteadof BinaryOperator;
	}

	protected function getActionOnUnitData() {
		return "condition = (op1 > op2);";
	}
}

?>