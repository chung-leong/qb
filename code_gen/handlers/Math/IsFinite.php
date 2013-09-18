<?php

class IsFinite extends Handler {

	public function getOperandType($i) {
		return ($i == 2) ? "I32" : $this->operandType;
	}

	protected function getActionOnUnitData() {
		return "res = zend_finite(op1);";
	}
}

?>