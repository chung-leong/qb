<?php

class BoundCheck extends Handler {

	use ScalarAddressMode, BinaryOperator, MayEmitError;

	protected function getActionOnUnitData() {
		$lines = array();
		$lines[] = "if(UNEXPECTED(op1 >= op2)) {";
		$lines[] =		"qb_abort(\"Array index out of bounds (%d >= %d)\", op1, op2);";
		$lines[] = "}";
		return $lines;
	}
}

?>