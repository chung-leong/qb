<?php

class LogicalNot extends Handler {

	use ScalarAddressMode, UnaryOperator, ScalarResult, BooleanResult, BooleanOnly;

	protected function getActionOnUnitData() {
		return "res = !op1;";
	}
}

?>