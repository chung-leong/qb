<?php

class SetNot extends Handler {

	use ArrayAddressMode, UnaryOperator, SetOperator, BooleanResult;

	protected function getActionOnUnitData() {
		return "res = (!op1);";
	}
}

?>