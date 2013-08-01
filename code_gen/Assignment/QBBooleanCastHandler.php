<?php

class QBBooleanCastHandler extends QBHandler {

	public function getOperandType($i) {
		return ($i == 2) ? "I32" : $this->operandType;
	}

	protected function getActionForUnitData() {
		return "res = !(!(op1));";
	}
}

?>