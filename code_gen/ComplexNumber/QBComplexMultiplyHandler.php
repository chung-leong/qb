<?php

class QBComplexMultiplyHandler extends QBComplexNumberHandler {

	protected function getComplexNumberExpression() {
		return array(
			"res_r = (op1_r * op2_r) - (op1_i * op2_i);",
			"res_i = (op1_r * op2_i) + (op1_i * op2_r);",
		);
	}
}

?>