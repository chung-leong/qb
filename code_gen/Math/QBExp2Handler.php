<?php

class QBExp2Handler extends QBHandler {

	protected function getScalarExpression() {
		$cType = $this->getOperandCType($this->srcCount + 1);
		return "res = ($cType) exp2(op1);";
	}
}

?>