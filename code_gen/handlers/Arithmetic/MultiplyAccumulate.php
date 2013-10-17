<?php

class MultiplyAccumulate extends Handler {

	use MultipleAddressMode, TernaryOperator, ExpressionReplication;

	protected function getActionOnUnitData() {
		return "res = (op1 * op2) + op3;";
	}
}

?>