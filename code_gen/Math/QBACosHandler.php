<?php

class QBACosHandler extends QBHandler {

	protected function getScalarExpression() {
		$type = $this->getOperandType(2);
		$cType = $this->getOperandCType(2);
		$f = ($type == 'F32') ? 'f' : '';
		return "res = ($cType) acos$f(op1);";
	}
}

?>
