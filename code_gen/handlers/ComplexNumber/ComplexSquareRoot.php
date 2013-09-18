<?php

class ComplexSquareRoot extends Handler {

	use ArrayAddressMode, UnaryOperator, FloatingPointOnly, Slow;
	
	protected function getActionOnUnitData() {
		$type = $this->getOperandType(2);
		$cType = $this->getOperandCType(2);
		$f = ($type == 'F32') ? 'f' : '';
		$lines = array();
		$lines[] = "$cType s = (op1_ptr[1] > 0 ? 1.0$f : ((op1_ptr[1] < 0 ? -1.0$f : 0.0$f)));";
		$lines[] = "$cType w = sqrt$f(op1_ptr[0] * op1_ptr[0] + op1_ptr[1] * op1_ptr[1]);";
		$lines[] = "$cType r = sqrt$f(0.5$f * (op1_ptr[0] + w));";
		$lines[] = "$cType i = (s) ? s * sqrt$f(0.5$f * (-op1_ptr[0] + w)) : 0;";
		$lines[] = "res_ptr[0] = r;";
		$lines[] = "res_ptr[1] = i;";
		return $lines;
	}
}

?>