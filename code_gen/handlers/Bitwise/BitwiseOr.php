<?php

class BitwiseOr extends Handler {

	use MultipleAddressMode, BinaryOperator, IntegerOnly, ExpressionReplication, NoUnsigned;

	protected function getActionOnUnitData() {
		return "res = op1 | op2;";
	}
}

?>