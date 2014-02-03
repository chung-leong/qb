<?php

class Max extends Handler {

	use MultipleAddressMode, BinaryOperator, Multithreaded;

	protected function getActionOnUnitData() {
		return "res = (op1 > op2) ? op1 : op2;";
	}
}

?>