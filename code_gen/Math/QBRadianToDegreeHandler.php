<?php

class QBRadianToDegreeHandler extends QBHandler {

	protected function getScalarExpression() {
		$cType = $this->getOperandCType(2);
		return "res = op1 * (($cType) (180 / M_PI));";
	}
}

?>