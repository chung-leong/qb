<?php

class QBIsInfiniteHandler extends QBHandler {

	public function getOperandType($i) {
		return ($i == 2) ? "I32" : $this->operandType;
	}

	protected function getScalarExpression() {
		// zend_isinf() may return -1 for -INF
		return "res = (zend_isinf(op1) != 0);";
	}
}

?>