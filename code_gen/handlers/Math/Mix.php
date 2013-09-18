<?php

class Mix extends Handler {

	public function getInputOperandCount() {
		return 3;
	}

	protected function getActionOnUnitData() {
		return "res = op1 * (1 - op3) + op2 * op3;";
	}
}

?>