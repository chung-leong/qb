<?php

class QBACoshHandler extends QBHandler {

	protected function getScalarExpression() {
		$cType = $this->getOperandCType($this->srcCount + 1);
		return "res = ($cType) acosh(op1);";

	}
}

?>