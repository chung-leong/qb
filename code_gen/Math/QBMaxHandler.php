<?php

class QBMaxHandler extends QBHandler {

	public function getInputOperandCount() {
		return 2;
	}

	protected function getActionForUnitData() {
		return "res = (op1 > op2) ? op1 : op2;";
	}
}

?>