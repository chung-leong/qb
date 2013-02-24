<?php

class QBNegateHandler extends QBHandler {

	protected function getScalarExpression() {
		return "res = - op1;";
	}
}

?>