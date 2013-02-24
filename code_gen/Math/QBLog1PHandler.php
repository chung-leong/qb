<?php

class QBLog1PHandler extends QBHandler {

	protected function getScalarExpression() {
		$cType = $this->getOperandCType($this->srcCount + 1);
		return "res = ($cType) log1p(op1);";

	}
}

?>