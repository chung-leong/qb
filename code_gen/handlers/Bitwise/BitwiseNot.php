<?php

class BitwiseNot extends Handler {

	use	MultipleAddressMode, UnaryOperator, IntegerOnly;
	
	protected function getActionOnUnitData() {
		return "res = ~op1;";
	}
}

?>