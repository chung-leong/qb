<?php

class QBLog10Handler extends QBHandler {

	protected function getScalarExpression() {
		$cType = $this->getOperandCType($this->srcCount + 1);
		return "res = ($cType) log10(op1);";
	}
}

?>