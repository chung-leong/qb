<?php

class QBHypotHandler extends QBHandler {

	public function getInputOperandCount() {
		return 2;
	}

	protected function getScalarExpression() {
		$type = $this->getOperandType($this->srcCount + 1);
		$cType = $this->getOperandCType($this->srcCount + 1);
		$f = ($type == 'F32') ? 'f' : '';
		return "res = ($cType) hypot$f(op1, op2);";
	}
}

?>