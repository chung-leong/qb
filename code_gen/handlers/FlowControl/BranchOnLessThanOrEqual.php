<?php

class BranchOnLessThanOrEqual extends Handler {

	use ScalarAddressMode, BinaryOperator, BranchInstruction {
        BranchInstruction::getOutputOperandCount insteadof BinaryOperator;
	}

	protected function getActionOnUnitData() {
		return "condition = (op1 <= op2);";
	}
}

?>