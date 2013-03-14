<?php

class QBComplexExpHandler extends QBComplexNumberHandler {

	protected function getComplexNumberExpression() {
		$cType = $this->getOperandCType(1);
		return array(
			"res_r = ($cType) (exp(op1_r) * cos(op1_i));",
			"res_i = ($cType) (exp(op1_r) * sin(op1_i));",
		);
	}
}

?>