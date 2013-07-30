<?php

class QBNotVectorHandler extends QBCompareVectorHandler {

	public function getInputOperandCount() {
		return 1;
	}

	protected function getScalarExpression() {
		return "res = (!op1);";
	}
}

?>