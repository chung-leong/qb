<?php

class QBLog10Handler extends QBHandler {

	protected function getActionForUnitData() {
		$type = $this->getOperandType(2);
		$cType = $this->getOperandCType(2);
		$f = ($type == 'F32') ? 'f' : '';
		return "res = log10$f(op1);";
	}
}

?>