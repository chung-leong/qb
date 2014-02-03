<?php

class Increment extends Handler {

	use MultipleAddressMode, NullaryOperator, ExpressionReplication, NoUnsigned, Multithreaded;

	protected function getActionOnUnitData() {
		return "++res;";
	}
}

?>