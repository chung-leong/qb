<?php

class QBMixHandler extends QBHandler {

	protected function getScalarExpression() {
		return "res = op1 * (1 - op3) + op2 * op3;";
	}
}

?>