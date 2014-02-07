<?php

class Subtract extends Handler {

	use MultipleAddressMode, BinaryOperator, ExpressionReplication, NoUnsigned, Multithreaded;
	
	protected function getActionOnUnitData() {
		return "res = op1 - op2;";
	}
}

?>