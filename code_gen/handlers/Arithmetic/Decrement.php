<?php

class Decrement extends Handler {

	use MultipleAddressMode, NullaryOperator;

	protected function getActionOnUnitData() {
		return "--res;";
	}
}

?>