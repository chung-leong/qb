<?php

class QBNotEqualVectorHandler extends QBCompareVectorHandler {

	protected function getScalarExpression() {
		return "res = (op1 != op2);";
	}
}

?>