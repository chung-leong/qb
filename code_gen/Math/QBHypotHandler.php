<?php

class QBHypotHandler extends QBHandler {

	protected function getScalarExpression() {
		$cType = $this->getOperandCType($this->srcCount + 1);
		return "res = ($cType) hypot(op1, op2);";
	}
}

?>