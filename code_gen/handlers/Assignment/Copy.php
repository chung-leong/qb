<?php

class Copy extends Handler {

	use MultipleAddressMode, UnaryOperator, NoUnsigned;

	protected function getActionOnUnitData() {
		return "res = op1;";
	}
}

?>