<?php

class Hypot extends Handler {

	use MultipleAddressMode, BinaryOperator, FloatingPointOnly, Multithreaded;

	protected function getActionOnUnitData() {
		$type = $this->getOperandType(3);
		$cType = $this->getOperandCType(3);
		$f = ($type == 'F32') ? 'f' : '';
		return "res = ($cType) hypot$f(op1, op2);";
	}
}

?>