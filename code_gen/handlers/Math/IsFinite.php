<?php

class IsFinite extends Handler {

	use MultipleAddressMode, UnaryOperator, FloatingPointOnly;
	
	public function getOperandType($i) {
		return ($i == 2) ? "I32" : $this->operandType;
	}

	protected function getActionOnUnitData() {
		return "res = zend_finite(op1);";
	}
}

?>