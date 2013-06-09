<?php

class QBMultiplyAccumulateHandler extends QBHandler {

	protected function getScalarExpression() {
		return "res = op1 + (op2 * op3);";
	}
}

?>