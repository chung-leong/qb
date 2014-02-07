<?php

class BitwiseXor extends Handler {

	use MultipleAddressMode, BinaryOperator, IntegerOnly, ExpressionReplication, NoUnsigned, Multithreaded;

	protected function getActionOnUnitData() {
		return "res = op1 ^ op2;";
	}
}

?>