<?php

class ATan2 extends Handler {

	use MultipleAddressMode, BinaryOperator, FloatingPointOnly;
	
	protected function getActionOnUnitData() {
		$type = $this->getOperandType(2);
		$cType = $this->getOperandCType(2);
		$f = ($type == 'F32') ? 'f' : '';
		return "res = atan2$f(op1, op2);";
	}
}

?>