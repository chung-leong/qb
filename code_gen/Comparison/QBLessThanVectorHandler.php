<?php

class QBLessThanVectorHandler extends QBCompareVectorHandler {

	protected function getScalarExpression() {
		return "res = (op1 < op2);";
	}
}

?>