<?php

class BranchOnTrue extends Handler {

	use MultipleAddressMode, UnaryOperatorNoResult, BranchInstruction;

	public function getAction() {
		return "condition = op1;";
	}
}

?>