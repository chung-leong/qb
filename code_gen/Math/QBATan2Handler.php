<?php

class QBATan2Handler extends QBHandler {

	public function getInputOperandCount() {
		return 2;
	}

	protected function getActionOnUnitData() {
		$type = $this->getOperandType(2);
		$cType = $this->getOperandCType(2);
		$f = ($type == 'F32') ? 'f' : '';
		return "res = atan2$f(op1, op2);";
	}
}

?>