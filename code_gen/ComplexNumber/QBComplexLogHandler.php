<?php

class QBComplexLogHandler extends QBComplexNumberHandler {

	protected function getComplexNumberExpression() {
		$cType = $this->getOperandCType(1);
		return array(
			"res_r = ($cType) (log(sqrt((op1_r * op1_r) + (op1_i * op1_i))));",
			"res_i = ($cType) atan2(op1_r, op1_i);",
		);
	}
}

?>