<?php

class BranchOnNotEqual extends Handler {

	use ScalarAddressMode, BinaryOperator, BranchInstruction, NoUnsigned {
        BranchInstruction::getOutputOperandCount insteadof BinaryOperator;
	}

	protected function getActionOnUnitData() {
		return "condition = (op1 != op2);";
	}
}

?>