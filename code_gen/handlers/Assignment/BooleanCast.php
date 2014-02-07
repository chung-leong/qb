<?php

class BooleanCast extends Handler {

	use MultipleAddressMode, UnaryOperator, ExpressionReplication, BooleanResult;

	protected function getActionOnUnitData() {
		return "res = (op1 != 0);";
	}
}

?>