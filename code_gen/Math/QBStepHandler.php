<?php

class QBStepHandler extends QBHandler {

	public function getInputOperandCount() {
		return 2;
	}

	protected function getScalarExpression() {
		$type = $this->getOperandType($this->srcCount + 1);
		$f = ($type == 'F32') ? 'f' : '';
		return "res = ((op2 < op1) ? 0.0$f : 1.0$f);";
	}
}

?>