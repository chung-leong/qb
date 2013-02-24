<?php

class QBATan2Handler extends QBHandler {

	protected function getScalarExpression() {
		$cType = $this->getOperandCType($this->srcCount + 1);
		return "res = ($cType) atan2(op1, op2);";
	}
}

?>