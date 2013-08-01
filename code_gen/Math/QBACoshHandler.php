<?php

class QBACoshHandler extends QBHandler {

	protected function getActionForUnitData() {
		$type = $this->getOperandType(2);
		$cType = $this->getOperandCType(2);
		$f = ($type == 'F32') ? 'f' : '';
		return "res = acosh$f(op1);";

	}
}

?>