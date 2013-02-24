<?php

class QBSqrtHandler extends QBHandler {

	protected function getScalarExpression() {
		$cType = $this->getOperandCType($this->srcCount + 1);
		return "res = ($cType) sqrt(op1);";

	}
}

?>