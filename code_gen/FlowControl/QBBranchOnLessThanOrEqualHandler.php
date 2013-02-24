<?php

class QBBranchOnLessThanOrEqualHandler extends QBBranchHandler {

	protected function getScalarExpression() {
		return "condition = (op1 <= op2);";
	}
}

?>