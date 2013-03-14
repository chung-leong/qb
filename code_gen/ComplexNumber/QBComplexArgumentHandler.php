<?php

class QBComplexArgumentHandler extends QBComplexNumberHandler {

	public function getOperandSize($i) {
		return ($i == 2) ? "1" : "2";
	}

	protected function getComplexNumberExpression() {
		$cType = $this->getOperandCType(1);
		return "res = ($cType) atan2(op1_r, op1_i);";
	}
}

?>