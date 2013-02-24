<?php

class QBMixHandler extends QBHandler {

	protected function getScalarExpression() {
		$cType = $this->getOperandCType($this->srcCount + 1);
		return "res = ($cType) (op1 * (1.0 - op3) + op2 * op3);";
	}
}

?>