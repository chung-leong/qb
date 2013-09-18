<?php

class Multiply extends Handler {

	use MultipleAddressMode, BinaryOperator;

	protected function getActionOnUnitData() {
		return "res = op1 * op2;";
	}
}

?>