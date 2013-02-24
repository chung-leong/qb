<?php

class QBCopyHandler extends QBHandler {

	protected function getScalarExpression() {
		return "res = op1;";
	}
}

?>