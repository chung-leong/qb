<?php

class QBAddHandler extends QBHandler {

	public function getInputOperandCount() {
		return 2;
	}

	protected function getActionOnUnitData() {
		return "res = op1 + op2;";
	}
}

?>