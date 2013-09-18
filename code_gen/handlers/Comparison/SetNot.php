<?php

class SetNot extends Handler {

	use ArrayAddressMode, UnaryOperator, BooleanResult;

	protected function getActionOnUnitData() {
		return "res = (!op1);";
	}
}

?>