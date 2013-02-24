<?php

class QBASinhHandler extends QBHandler {

	protected function getScalarExpression() {
		$cType = $this->getOperandCType($this->srcCount + 1);
		return "res = ($cType) asinh(op1);";
	}
}

?>