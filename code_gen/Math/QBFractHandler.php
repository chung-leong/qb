<?php

class QBFractHandler extends QBHandler {

	protected function getActionForUnitData() {
		$type = $this->getOperandType(2);
		$cType = $this->getOperandCType(2);
		$f = ($type == 'F32') ? 'f' : '';
		return "res = op1 - floor$f(op1);";
	}
}

?>