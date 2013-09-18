<?php

class BooleanCast extends Handler {

	use MultipleAddressMode, UnaryOperator, ScalarResult, BooleanResult;

	protected function getActionOnUnitData() {
		return "res = (op1 != 0);";
	}
}

?>