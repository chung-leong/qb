<?php

class QBDecrementHandler extends QBHandler {

	protected function getScalarExpression() {
		return "--res;";
	}
}

?>