<?php

class Copy extends Handler {

	use MultipleAddressMode, BinaryOperator, NoUnsigned;

	protected function getActionOnUnitData() {
		return "res = op1;";
	}
}

?>