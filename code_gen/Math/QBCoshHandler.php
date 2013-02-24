<?php

class QBCoshHandler extends QBHandler {

	protected function getScalarExpression() {
		$cType = $this->getOperandCType($this->srcCount + 1);
		return "res = ($cType) cosh(op1);";

	}
}

?>