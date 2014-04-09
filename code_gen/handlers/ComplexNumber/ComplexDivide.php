<?php

class ComplexDivide extends Handler {

	use ArrayAddressMode, BinaryOperator, FloatingPointOnly, FixedOperandSize, Slow, Multithreaded, Complex;

	protected function getActionOnUnitData() {
		$type = $this->getOperandType(3);
		$f = ($type == 'F32') ? 'f' : '';
		return "res = cdiv$f(op1, op2);";
	}
}

?>
