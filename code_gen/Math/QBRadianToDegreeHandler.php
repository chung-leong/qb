<?php

class QBRadianToDegreeHandler extends QBHandler {

	protected function getScalarExpression() {
		$cType = $this->getOperandCType($this->srcCount + 1);
		return "res = ($cType) ((op1 / M_PI) * 180);";
	}
}

?>