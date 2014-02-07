<?php

class Copy extends Handler {

	use MultipleAddressMode, UnaryOperator, ExpressionReplication, NoUnsigned;

	protected function getActionOnUnitData() {
		return "res = op1;";
	}
}

?>