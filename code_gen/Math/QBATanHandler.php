<?php

class QBATanHandler extends QBHandler {

	protected function getActionForUnitData() {
		$type = $this->getOperandType(2);
		$cType = $this->getOperandCType(2);
		$f = ($type == 'F32') ? 'f' : '';
		return "res = atan$f(op1);";

	}
}

?>