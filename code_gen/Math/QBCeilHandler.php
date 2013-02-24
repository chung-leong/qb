<?php

class QBCeilHandler extends QBHandler {

	protected function getScalarExpression() {
		$cType = $this->getOperandCType($this->srcCount + 1);
		return "res = ($cType) ceil(op1);";

	}
}

?>