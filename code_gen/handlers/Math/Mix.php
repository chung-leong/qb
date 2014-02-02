<?php

class Mix extends Handler {

	use MultipleAddressMode, TernaryOperator, FloatingPointOnly, Multithreaded;

	protected function getActionOnUnitData() {
		return "res = op1 * (1 - op3) + op2 * op3;";
	}
}

?>