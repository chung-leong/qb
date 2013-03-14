<?php

class QBComplexDivideHandler extends QBComplexNumberHandler {

	protected function getComplexNumberExpression() {
		$cType = $this->getOperandCType(1);
		return array(
			"res_r = ($cType) ( ((op1_r * op2_r) - (op1_i * op2_i)) / ((op2_r * op2_r) + (op2_i * op2_i)) );",
			"res_i = ($cType) ( ((op1_r * op2_i) + (op1_i * op2_r)) / ((op2_r * op2_r) + (op2_i * op2_i)) );",
		);
	}
}

?>