<?php

class QBSinhHandler extends QBHandler {

	protected function getScalarExpression() {
		$cType = $this->getOperandCType($this->srcCount + 1);
		return "res = ($cType) sinh(op1);";

	}
}

?>