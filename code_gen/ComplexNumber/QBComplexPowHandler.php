<?php

class QBComplexPowHandler extends QBComplexNumberHandler {

	public function getOperandSize($i) {
		return ($i == 2) ? "1" : "2";
	}
	
	protected function getComplexNumberExpression() {
		$cType = $this->getOperandCType(1);
		return array(		
			"res_r = ($cType) (cos(op2 * atan2(op1_r, op1_i)) * pow(sqrt((op1_r * op1_r) + (op1_i * op1_i)), op2));",
			"res_i = ($cType) (sin(op2 * atan2(op1_r, op1_i)) * pow(sqrt((op1_r * op1_r) + (op1_i * op1_i)), op2));",
		);
	}
}

?>