<?php

class IsInfinite extends Handler {

	use MultipleAddressMode, UnaryOperator, FloatingPointOnly, Multithreaded;
	
	public function getOperandType($i) {
		return ($i == 2) ? "I32" : $this->operandType;
	}

	public function getHandlerFunctionType() {
		// work around multilevel macros
		return "extern";
	}

	protected function getActionOnUnitData() {
		// zend_isinf() may return -1 for -INF
		return "res = (zend_isinf(op1) != 0);";
	}
}

?>
