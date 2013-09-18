<?php

class MultiplyAccumulate extends Handler {

	use MultipleAddressMode, TernaryOperator;

	protected function getActionOnUnitData() {
		return "res = op1 + (op2 * op3);";
	}
}

?>