<?php

class BranchOnTrue extends Handler {

	trait TraitBranchInstruction;

	public function getAction() {
		return "condition = op1;";
	}
}

?>