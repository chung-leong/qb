<?php

class ShiftRight extends Handler {

	use MultipleAddressMode, BinaryOperator, ExpressionReplication, IntegerOnly, Multithreaded;

	protected function getActionOnUnitData() {
		return "res = op1 >> op2;";
	}
}

?>