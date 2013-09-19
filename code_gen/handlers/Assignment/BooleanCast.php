<?php

class BooleanCast extends Handler {

	use MultipleAddressMode, UnaryOperator, BooleanResult;

	protected function getActionOnUnitData() {
		return "res = (op1 != 0);";
	}
}

?>