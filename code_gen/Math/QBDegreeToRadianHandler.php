<?php

class QBDegreeToRadianHandler extends QBHandler {

	protected function getScalarExpression() {
		$cType = $this->getOperandCType(2);
		return "res = ($cType) ((op1 / 180.0) * M_PI);";
	}
}

?>