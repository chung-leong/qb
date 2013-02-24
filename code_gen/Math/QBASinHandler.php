<?php

class QBASinHandler extends QBHandler {

	protected function getScalarExpression() {
		$cType = $this->getOperandCType($this->srcCount + 1);
		return "res = ($cType) asin(op1);";
	}
}

?>