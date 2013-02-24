<?php

class QBIsNANHandler extends QBHandler {

	public function getOperandType($i) {
		return ($i == 2) ? "I32" : $this->operandType;
	}

	protected function getScalarExpression() {
		return "res = zend_isnan(op1);";
	}
}

?>