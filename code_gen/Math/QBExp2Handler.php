<?php

class QBExp2Handler extends QBHandler {

	protected function getActionForUnitData() {
		$type = $this->getOperandType(2);
		$cType = $this->getOperandCType(2);
		$f = ($type == 'F32') ? 'f' : '';
		return "res = exp2$f(op1);";
	}
}

?>