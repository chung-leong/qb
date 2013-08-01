<?php

class QBMultiplyAccumulateHandler extends QBHandler {

	public function getInputOperandCount() {
		return 3;
	}

	protected function getActionForUnitData() {
		return "res = op1 + (op2 * op3);";
	}
}

?>