<?php

class ComplexTan extends Handler {

	use ArrayAddressMode, UnaryOperator, FloatingPointOnly, FixedOperandSize, Slow, Multithreaded;
	
	protected function getActionOnUnitData() {
		$type = $this->getOperandType(2);
		$cType = $this->getOperandCType(2);
		$f = ($type == 'F32') ? 'f' : '';
		$lines = array();
		$lines[] = "$cType w = 1.0$f / (cos$f(2.0$f * op1_ptr[0]) + cosh$f(2.0$f * op1_ptr[1]));";
		$lines[] = "$cType r = w * sin$f(2.0$f * op1_ptr[0]);";
		$lines[] = "$cType i = w * sinh$f(2.0$f * op1_ptr[1]);";
		$lines[] = "res_ptr[0] = r;";
		$lines[] = "res_ptr[1] = i;";
		return $lines;
	}
}

?>