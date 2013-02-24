<?php

class QBIncrementHandler extends QBHandler {

	protected function getScalarExpression() {
		return "++res;";
	}
}

?>