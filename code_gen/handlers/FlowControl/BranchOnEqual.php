<?php

class BranchOnEqual extends Handler {

	use MultipleAddressMode, BinaryOperator, BranchInstruction, NoUnsigned {
        BranchInstruction::getOutputOperandCount insteadof BinaryOperator;
	}

	public function getAction() {
		return "condition = (op1 == op2);";
	}
}

?>