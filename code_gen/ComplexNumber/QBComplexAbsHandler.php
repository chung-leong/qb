<?php

class QBComplexAbsHandler extends QBComplexNumberHandler {

	protected function getComplexNumberExpression() {
		$cType = $this->getOperandCType(1);
		return "res_ptr[0] = ($cType) sqrt(op1_ptr[0] * op1_ptr[0] + op1_ptr[1] * op1_ptr[1]);";
	}
}

?>