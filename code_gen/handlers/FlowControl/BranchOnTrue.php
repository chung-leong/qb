<?php

class BranchOnTrue extends Handler {

	use ScalarAddressMode, UnaryOperator, BranchInstruction {
        BranchInstruction::getOutputOperandCount insteadof UnaryOperator;
	}

	public function getAction() {
		return "condition = op1;";
	}
}

?>