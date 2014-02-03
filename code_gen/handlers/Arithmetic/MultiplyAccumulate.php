<?php

class MultiplyAccumulate extends Handler {

	use MultipleAddressMode, TernaryOperator, ExpressionReplication, Multithreaded;

	protected function getActionOnUnitData() {
		return "res = (op1 * op2) + op3;";
	}
}

?>