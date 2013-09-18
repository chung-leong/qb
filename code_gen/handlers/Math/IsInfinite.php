<?php

class IsInfinite extends Handler {

	public function getOperandType($i) {
		return ($i == 2) ? "I32" : $this->operandType;
	}

	protected function getActionOnUnitData() {
		// zend_isinf() may return -1 for -INF
		return "res = (zend_isinf(op1) != 0);";
	}
}

?>