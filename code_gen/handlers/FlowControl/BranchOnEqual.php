<?php

class BranchOnEqual extends Handler {

	use MultipleAddressMode, BinaryOperatorNoResult, BranchInstruction, NoUnsigned;

	public function getAction() {
		return "condition = (op1 == op2);";
	}
}

?>