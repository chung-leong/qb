<?php

class ComplexLog extends Handler {

	use ArrayAddressMode, UnaryOperator, FloatingPointOnly, FixedOperandSize, Slow;
	
	protected function getActionOnUnitData() {
		$type = $this->getOperandType(2);
		$cType = $this->getOperandCType(2);
		$f = ($type == 'F32') ? 'f' : '';
		$lines = array();
		$lines[] = "$cType w = sqrt$f(op1_ptr[0] * op1_ptr[0] + op1_ptr[1] * op1_ptr[1]);";
		$lines[] = "$cType r = log$f(w);";
		$lines[] = "$cType i = atan2$f(op1_ptr[1], op1_ptr[0]);";
		$lines[] = "res_ptr[0] = r;";
		$lines[] = "res_ptr[1] = i;";
		return $lines;
	}
}

?>