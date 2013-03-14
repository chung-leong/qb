<?php

class QBComplexCosHandler extends QBComplexNumberHandler {

	protected function getComplexNumberExpression() {
		$cType = $this->getOperandCType(1);
		return array(
			"res_r = ($cType) (cos(op1_r) * cosh(op1_i));",
			"res_i = ($cType) (-sin(op1_r) * sinh(op1_i));",
		);
	}
}

?>