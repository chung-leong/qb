<?php

class Max extends Handler {

	use MultipleAddressMode, BinaryOperator;

	protected function getActionOnUnitData() {
		return "res = (op1 > op2) ? op1 : op2;";
	}
}

?>