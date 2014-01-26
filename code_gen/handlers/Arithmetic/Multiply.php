<?php

class Multiply extends Handler {

	use MultipleAddressMode, BinaryOperator, ExpressionReplication;

	protected function getActionOnUnitData() {
		return "res = op1 * op2;";
	}
}

?>
