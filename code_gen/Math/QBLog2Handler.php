<?php

class QBLog2Handler extends QBHandler {

	protected function getScalarExpression() {
		$cType = $this->getOperandCType($this->srcCount + 1);
		return "res = ($cType) log2(op1);";
	}
}

?>