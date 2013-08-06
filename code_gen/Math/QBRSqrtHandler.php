<?php

class QBRSqrtHandler extends QBHandler {

	protected function getActionOnUnitData() {
		$type = $this->getOperandType(2);
		$cType = $this->getOperandCType(2);
		$f = ($type == 'F32') ? 'f' : '';
		return "res = qb_fast_rsqrt$f(op1);";
	}
}

?>