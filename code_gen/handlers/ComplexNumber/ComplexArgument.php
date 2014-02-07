<?php

class ComplexArgument extends Handler {

	use ArrayAddressMode, UnaryOperator, UnitResult, FloatingPointOnly, FixedOperandSize, Slow, Multithreaded;
	
	protected function getActionOnUnitData() {
		$cType = $this->getOperandCType(1);
		return "res_ptr[0] = ($cType) atan2(op1_ptr[1], op1_ptr[0]);";
	}
}

?>