<?php

class QBStepHandler extends QBHandler {

	protected function getScalarExpression() {
		$cType = $this->getOperandCType(3);
		return "res = ($cType) ((op2 < op1) ? 0 : 1);";
	}
}

?>