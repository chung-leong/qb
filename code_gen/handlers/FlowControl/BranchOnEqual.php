<?php

class BranchOnEqual extends Handler {

	use BinaryOperator, TraitBranchInstruction {
        TraitBranchInstruction::getInputOperandCount insteadof TraitBinaryOperator;
	}

	public function getAction() {
		return "condition = (op1 == op2);";
	}
}

?>