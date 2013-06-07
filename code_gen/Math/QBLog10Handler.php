<?php

class QBLog10Handler extends QBHandler {

	protected function getScalarExpression() {
		$type = $this->getOperandType($this->srcCount + 1);
		$cType = $this->getOperandCType($this->srcCount + 1);
		$f = ($type == 'F32') ? 'f' : '';
		return "res = log10$f(op1);";
	}
}

?>