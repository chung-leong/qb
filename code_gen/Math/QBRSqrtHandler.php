<?php

class QBRSqrtHandler extends QBHandler {

	protected function getScalarExpression() {
		$cType = $this->getOperandCType(1);
		return "res = ($cType) (1 / sqrt(op1));";
	}
}

?>