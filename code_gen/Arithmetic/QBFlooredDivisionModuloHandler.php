<?php

// only used by Pixel Pender code

class QBFlooredDivisionModuloHandler extends QBHandler {

	protected function getScalarExpression() {
		$cType = $this->getOperandCType(1);
		return "res = ($cType) (op1 - op2 * floor(op1 / op2));";
	}
}

?>