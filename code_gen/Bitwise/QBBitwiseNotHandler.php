<?php

class QBBitwiseNotHandler extends QBHandler {

	protected function getScalarExpression() {
		return "res = ~op1;";
	}
}

?>