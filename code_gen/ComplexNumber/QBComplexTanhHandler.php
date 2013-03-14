<?php

class QBComplexTanhHandler extends QBComplexNumberHandler {

	protected function getComplexNumberExpression() {
		$cType = $this->getOperandCType(1);
		return array(
			"res_r = ($cType) (sinh(2.0 * op1_r) / (cosh(2.0 * op1_r) + cos(2.0 * op1_i)));",
			"res_i = ($cType) (sin(2.0 * op1_i) / (cosh(2.0 * op1_r) + cos(2.0 * op1_i)));",
		);
	}
}

?>