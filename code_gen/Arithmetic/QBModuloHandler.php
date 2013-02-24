<?php

class QBModuloHandler extends QBHandler {

	protected function getScalarExpression() {
		$type = $this->getOperandType(1);
		if($type == "F32") {
			return "res = (float32_t) fmod(op1, op2);";
		} else if($type == "F64") {
			return "res = fmod(op1, op2);";
		} else {
			return "res = op1 % op2;";
		}
	}
}

?>