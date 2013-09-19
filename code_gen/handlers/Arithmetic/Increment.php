<?php

class Increment extends Handler {

	use MultipleAddressMode, NullaryOperator, ExpressionReplication, NoUnsigned;

	protected function getActionOnUnitData() {
		return "++res;";
	}
}

?>