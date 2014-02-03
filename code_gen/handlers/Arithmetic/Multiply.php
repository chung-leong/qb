<?php

class Multiply extends Handler {

	use MultipleAddressMode, BinaryOperator, ExpressionReplication, Multithreaded;

	protected function getActionOnUnitData() {
		return "res = op1 * op2;";
	}
}

?>
