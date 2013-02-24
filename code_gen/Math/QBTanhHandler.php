<?php

class QBTanhHandler extends QBHandler {

	protected function getScalarExpression() {
		$cType = $this->getOperandCType($this->srcCount + 1);
		return "res = ($cType) tanh(op1);";

	}
}

?>