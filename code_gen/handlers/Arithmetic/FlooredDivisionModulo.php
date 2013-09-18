<?php

class FlooredDivisionModulo extends Handler {

	use MultipleAddressMode, BinaryOperator;

	protected function getActionOnUnitData() {
		$cType = $this->getOperandCType(1);
		return "res = ($cType) (op1 - op2 * floor(op1 / op2));";
	}
}

?>