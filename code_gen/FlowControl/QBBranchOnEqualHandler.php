<?php

class QBBranchOnEqualHandler extends QBBranchHandler {

	public function getAction() {
		return "condition = (op1 == op2);";
	}
}

?>