<?php

class Decrement extends Handler {

	use MultipleAddressMode, NullaryOperator, ExpressionReplication, NoUnsigned;

	protected function getActionOnUnitData() {
		return "--res;";
	}
}

?>