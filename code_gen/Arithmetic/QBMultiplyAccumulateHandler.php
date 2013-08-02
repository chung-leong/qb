<?php

class QBMultiplyAccumulateHandler extends QBHandler {

	public function getInputOperandCount() {
		return 3;
	}

	protected function getActionOnUnitData() {
		return "res = op1 + (op2 * op3);";
	}
}

?>