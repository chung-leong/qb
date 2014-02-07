<?php

class Decrement extends Handler {

	use MultipleAddressMode, NullaryOperator, ExpressionReplication, NoUnsigned, Multithreaded;

	protected function getActionOnUnitData() {
		return "--res;";
	}
}

?>