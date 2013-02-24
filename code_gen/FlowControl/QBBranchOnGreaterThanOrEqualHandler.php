<?php

class QBBranchOnGreaterThanOrEqualHandler extends QBBranchHandler {

	protected function getScalarExpression() {
		return "condition = (op1 >= op2);";
	}
}

?>