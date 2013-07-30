<?php

class QBShiftRightHandler extends QBHandler {

	public function getInputOperandCount() {
		return 2;
	}

	protected function getScalarExpression() {
		return "res = op1 >> op2;";
	}
}

?>