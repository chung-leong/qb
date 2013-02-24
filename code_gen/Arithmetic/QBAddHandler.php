<?php

class QBAddHandler extends QBHandler {

	protected function getScalarExpression() {
		return "res = op1 + op2;";
	}
}

?>