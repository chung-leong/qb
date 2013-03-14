<?php

class QBComplexSquareRootHandler extends QBComplexNumberHandler {

	protected function getComplexNumberExpression() {
		$cType = $this->getOperandCType(1);
		return array(
			"res_r = ($cType) sqrt(0.5 * (op1_r + sqrt((op1_r * op1_r) + (op2_i * op2_i))));",
			"res_i = ($cType) ((op1_i > 0 ? 1.0 : ((op1_i < 0 ? -1.0 : 0))) * sqrt(0.5 * (-op1_r + sqrt((op1_r * op1_r) + (op2_i * op2_i)))));",
		);
	}
}

?>