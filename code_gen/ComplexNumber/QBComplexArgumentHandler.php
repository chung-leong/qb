<?php

class QBComplexArgumentHandler extends QBComplexNumberHandler {

	public function getOperandSize($i) {
		return ($i == 2) ? "1" : "2";
	}

	protected function getComplexNumberExpression() {
		$cType = $this->getOperandCType(1);
		return "res_ptr[0] = ($cType) atan2(op1_ptr[0], op1_ptr[1]);";
	}
}

?>