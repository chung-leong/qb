<?php

class QBPowHandler extends QBHandler {

	protected function getScalarExpression() {
		$cType = $this->getOperandCType($this->srcCount + 1);
		return "res = ($cType) pow(op1, op2);";
	}
}

?>