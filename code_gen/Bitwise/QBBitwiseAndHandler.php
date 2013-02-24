<?php

class QBBitwiseAndHandler extends QBHandler {

	protected function getScalarExpression() {
		return "res = op1 & op2;";
	}
}

?>