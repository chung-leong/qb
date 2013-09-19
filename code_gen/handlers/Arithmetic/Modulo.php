<?php

class Modulo extends Handler {

	use MultipleAddressMode, BinaryOperator, ExpressionReplication;

	protected function getActionOnUnitData() {
		$type = $this->getOperandType(1);
		if($type == "F32") {
			return "res = fmodf(op1, op2);";
		} else if($type == "F64") {
			return "res = fmod(op1, op2);";
		} else {
			return "res = op1 % op2;";
		}
	}
}

?>