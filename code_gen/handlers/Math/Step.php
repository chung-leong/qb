<?php

class Step extends Handler {

	use MultipleAddressMode, BinaryOperator, FloatingPointOnly, Multithreaded;
	
	protected function getActionOnUnitData() {
		$type = $this->getOperandType(3);
		$f = ($type == 'F32') ? 'f' : '';
		return "res = ((op2 < op1) ? 0.0$f : 1.0$f);";
	}
}

?>