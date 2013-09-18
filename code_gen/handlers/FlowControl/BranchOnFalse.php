<?php

class BranchOnFalse extends Handler {

	use MultipleAddressMode, UnaryOperator, BranchInstruction {
        BranchInstruction::getOutputOperandCount insteadof UnaryOperator;
	}
	
	public function getAction() {
		return "condition = !op1;";
	}
}

?>