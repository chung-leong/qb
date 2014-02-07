<?php

class ComplexPow extends Handler {

	use ArrayAddressMode, BinaryOperator, FloatingPointOnly, FixedOperandSize, Slow, Multithreaded;

	protected function getActionOnUnitData() {
		$type = $this->getOperandType(3);
		$cType = $this->getOperandCType(3);
		$f = ($type == 'F32') ? 'f' : '';
		$lines = array();
		$lines[] = "$cType u = atan2$f(op1_ptr[1], op1_ptr[0]);";
		$lines[] = "$cType v = op1_ptr[0] * op1_ptr[0] + op1_ptr[1] * op1_ptr[1];";
		$lines[] = "$cType w = 1;";
		$lines[] = "$cType x = pow$f(v, 0.5$f * op2_ptr[0]);";		
		$lines[] = "$cType y = op2_ptr[0] * u;";
		$lines[] = "$cType z = 0;";
		$lines[] = "if(op2_ptr[1] != 0) {";
		$lines[] = 		"z = 0.5$f * op2_ptr[1] * log$f(v);";
		$lines[] = 		"w = exp$f(-op2_ptr[1] * u);";
		$lines[] = "}";
		$lines[] = "res_ptr[0] = x * w * cos$f(y + z);";
		$lines[] = "res_ptr[1] = x * w * sin$f(y + z);";
		return $lines;
	}
}

?>