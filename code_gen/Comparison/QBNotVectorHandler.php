<?php

class QBNotVectorHandler extends QBCompareVectorHandler {

	protected function getScalarExpression() {
		return "res = (!op1);";
	}
}

?>