<?php

class RoundToPrecision extends Handler {

	use MultipleAddressMode, TernaryOperator, FloatingPointOnly, Multithreaded;

	public function getOperandType($i) {
		return ($i == 2 || $i == 3) ? "I32" : $this->operandType;
	}

	protected function getActionOnUnitData() {
		$cType = $this->getOperandCType(4); 	
		return "res = ($cType) _php_math_round(op1, op2, op3);";
	}
}

?>
