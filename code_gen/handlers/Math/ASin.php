<?php

class ASin extends Handler {

	use MultipleAddressMode, UnaryOperator, FloatingPointOnly, Multithreaded;
	
	protected function getActionOnUnitData() {
		$type = $this->getOperandType(2);
		$cType = $this->getOperandCType(2);
		$f = ($type == 'F32') ? 'f' : '';
		return "res = asin$f(op1);";
	}
}

?>