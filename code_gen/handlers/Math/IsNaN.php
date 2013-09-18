<?php

class IsNAN extends Handler {

	public function getOperandType($i) {
		return ($i == 2) ? "I32" : $this->operandType;
	}

	protected function getActionOnUnitData() {
		return "res = zend_isnan(op1);";
	}
}

?>