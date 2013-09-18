<?php

class BooleanCast extends Handler {

	public function getOperandType($i) {
		return ($i == 2) ? "I32" : $this->operandType;
	}

	protected function getActionOnUnitData() {
		return "res = !(!(op1));";
	}
}

?>