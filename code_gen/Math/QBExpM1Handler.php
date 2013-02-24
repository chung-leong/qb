<?php

class QBExpM1Handler extends QBHandler {

	protected function getScalarExpression() {
		$cType = $this->getOperandCType($this->srcCount + 1);
		return "res = ($cType) expm1(op1);";

	}
}

?>