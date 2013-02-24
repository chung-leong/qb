<?php

class QBFloorHandler extends QBHandler {

	protected function getScalarExpression() {
		$cType = $this->getOperandCType($this->srcCount + 1);
		return "res = ($cType) floor(op1);";

	}
}

?>