<?php

class ComplexTanh extends Handler {

	use ArrayAddressMode, UnaryOperator, FloatingPointOnly, FixedOperandSize, Multithreaded, Complex;
	
	protected function getActionOnUnitData() {
		$type = $this->getOperandType(2);
		$f = ($type == 'F32') ? 'f' : '';
		return "res = ctanh$f(op1);";
	}
}

?>
