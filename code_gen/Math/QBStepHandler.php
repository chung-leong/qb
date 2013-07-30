<?php

class QBStepHandler extends QBHandler {

	public function getInputOperandCount() {
		return 2;
	}

	protected function getScalarExpression() {
		$type = $this->getOperandType(3);
		$f = ($type == 'F32') ? 'f' : '';
		return "res = ((op2 < op1) ? 0.0$f : 1.0$f);";
	}
}

?>