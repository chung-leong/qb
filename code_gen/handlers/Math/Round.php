<?php

class Round extends Handler {

	use MultipleAddressMode, TernaryOperator, FloatingPointOnly;

	public function getOperandType($i) {
		return ($i == 2 || $i == 3) ? "I32" : $this->operandType;
	}

	protected function getActionOnUnitData() {
		$cType = $this->getOperandCType(4); 	
		// TODO: use regular round() if we're rounding to integral
		return "res = ($cType) _php_math_round(op1, op2, op3);";
	}
}

?>