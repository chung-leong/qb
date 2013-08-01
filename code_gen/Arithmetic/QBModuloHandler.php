<?php

class QBModuloHandler extends QBHandler {

	public function getInputOperandCount() {
		return 2;
	}

	protected function getActionForUnitData() {
		$type = $this->getOperandType(1);
		if($type == "F32") {
			return "res = fmodf(op1, op2);";
		} else if($type == "F64") {
			return "res = fmod(op1, op2);";
		} else {
			return "res = op1 % op2;";
		}
	}
}

?>