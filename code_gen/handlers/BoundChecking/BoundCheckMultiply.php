<?php

class BoundCheckMultiply extends Handler {

	use ScalarAddressMode, TernaryOperator, MayEmitError;

	protected function getActionOnUnitData() {
		$lines = array();
		$lines[] = "res = op1 * op3;";
		$lines[] = "if(UNEXPECTED(op1 >= op2)) {";
		$lines[] =		"qb_abort(\"Array index out of bounds (%d >= %d)\", op1, op2);";
		$lines[] = "}";
		return $lines;
	}
}

?>