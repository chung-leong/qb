<?php

class LogicalXor extends Handler {

	use ScalarAddressMode, BinaryOperator, ScalarResult, BooleanResult, BooleanOnly;

	protected function getActionOnUnitData() {
		return "res = !op1 != !op2;";
	}
}

?>