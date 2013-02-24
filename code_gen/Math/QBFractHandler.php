<?php

class QBFractHandler extends QBHandler {

	protected function getScalarExpression() {
		$cType = $this->getOperandCType($this->srcCount + 1);
		return "res = op1 - ($cType) floor(op1);";
	}
}

?>