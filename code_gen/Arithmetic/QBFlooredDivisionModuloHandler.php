<?php

class QBFlooredDivisionModuloHandler extends QBHandler {

	public function getInputOperandCount() {
		return 2;
	}

	protected function getScalarExpression() {
		$cType = $this->getOperandCType(1);
		return "res = ($cType) (op1 - op2 * floor(op1 / op2));";
	}
}

?>