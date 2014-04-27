<?php

class ComplexPow extends Handler {

	use ArrayAddressMode, BinaryOperator, FloatingPointOnly, FixedOperandSize, Multithreaded, Complex;

	protected function getActionOnUnitData() {
		$type = $this->getOperandType(3);
		$f = ($type == 'F32') ? 'f' : '';
		return "res = cpow$f(op1, op2);";

	}
}

?>
