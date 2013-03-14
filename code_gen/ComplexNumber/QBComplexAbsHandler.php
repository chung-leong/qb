<?php

class QBComplexAbsHandler extends QBComplexNumberHandler {

	protected function getComplexNumberExpression() {
		$cType = $this->getOperandCType(1);
		return "res = ($cType) sqrt((op1_r * op1_r) + (op1_i * op1_i));";
	}
}

?>