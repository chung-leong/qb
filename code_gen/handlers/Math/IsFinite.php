<?php

class IsFinite extends Handler {

	use MultipleAddressMode, UnaryOperator, FloatingPointOnly;
	
	public function getOperandType($i) {
		return ($i == 2) ? "I32" : $this->operandType;
	}

	public function getHandlerFunctionType() {
		// work around multilevel macros
		return "extern";
	}

	protected function getActionOnUnitData() {
		return "res = zend_finite(op1);";
	}
}	

?>
