<?php

class ComplexMultiply extends Handler {

	use ArrayAddressMode, BinaryOperator, FloatingPointOnly, FixedOperandSize, Slow, Multithreaded, Complex;

	protected function getActionOnUnitData() {
		$type = $this->getOperandType(3);
		$f = ($type == 'F32') ? 'f' : '';
		return "res = cmult$f(op1, op2);";
	}
}

?>
