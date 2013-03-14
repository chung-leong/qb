<?php

class QBComplexTanHandler extends QBComplexNumberHandler {

	protected function getComplexNumberExpression() {
		$cType = $this->getOperandCType(1);
		return array(
			"res_r = ($cType) (sin(2.0 * op1_r) / (cos(2.0 * op1_r) + cosh(2.0 * op1_i)));",
			"res_i = ($cType) (sinh(2.0 * op1_i) / (cos(2.0 * op1_r) + cosh(2.0 * op1_i)));",
		);
	}
}

?>