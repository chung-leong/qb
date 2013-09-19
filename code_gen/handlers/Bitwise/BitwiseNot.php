<?php

class BitwiseNot extends Handler {

	use	MultipleAddressMode, UnaryOperator, IntegerOnly, ExpressionReplication, NoUnsigned;
	
	protected function getActionOnUnitData() {
		return "res = ~op1;";
	}
}

?>