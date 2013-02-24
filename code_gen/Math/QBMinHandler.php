<?php

class QBMinHandler extends QBHandler {

	protected function getScalarExpression() {
		return "res = (op1 < op2) ? op1 : op2;";
	}
}

?>