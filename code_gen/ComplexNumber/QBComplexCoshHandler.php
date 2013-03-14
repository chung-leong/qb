<?php

class QBComplexCoshHandler extends QBComplexNumberHandler {

	protected function getComplexNumberExpression() {
		$cType = $this->getOperandCType(1);
		return array(
			"res_r = ($cType) (cosh(op1_r) * cos(op1_i));",
			"res_i = ($cType) (sinh(op1_r) * sin(op1_i));",
		);
	}
}

?>