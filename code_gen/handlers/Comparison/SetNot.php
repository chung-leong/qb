<?php

class SetNot extends Handler {

	use	MultipleAddressMode,
		UnaryOperator;
	use BooleanResult;

	protected function getActionOnUnitData() {
		return "res = (!op1);";
	}
}

?>