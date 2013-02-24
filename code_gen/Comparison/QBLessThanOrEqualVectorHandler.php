<?php

class QBLessThanOrEqualVectorHandler extends QBCompareVectorHandler {

	protected function getScalarExpression() {
		return "res = (op1 <= op2);";
	}
}

?>