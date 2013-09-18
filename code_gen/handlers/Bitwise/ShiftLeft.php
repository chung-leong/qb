<?php

class ShiftLeft extends Handler {

	use MultipleAddressMode, BinaryOperator, IntegerOnly;

	protected function getActionOnUnitData() {
		return "res = op1 << op2;";
	}
}

?>