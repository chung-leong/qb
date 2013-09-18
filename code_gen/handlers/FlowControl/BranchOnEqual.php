<?php

class BranchOnEqual extends Handler {

	use ScalarAddressMode, BinaryOperator, BranchInstruction, NoUnsigned {
        BranchInstruction::getOutputOperandCount insteadof BinaryOperator;
	}

	public function getAction() {
		return "condition = (op1 == op2);";
	}
}

?>