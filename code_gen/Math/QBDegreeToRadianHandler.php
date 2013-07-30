<?php

class QBDegreeToRadianHandler extends QBHandler {

	protected function getScalarExpression() {
		$type = $this->getOperandType(2);
		$cType = $this->getOperandCType(2);
		$f = ($type == 'F32') ? 'f' : '';
		return "res = op1 * (($cType) (M_PI / 180.0));";
	}
}

?>