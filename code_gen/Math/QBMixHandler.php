<?php

class QBMixHandler extends QBHandler {

	public function getInputOperandCount() {
		return 3;
	}

	protected function getActionOnUnitData() {
		return "res = op1 * (1 - op3) + op2 * op3;";
	}
}

?>