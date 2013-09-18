<?php

class Increment extends Handler {

	use MultipleAddressMode, NullaryOperator;

	protected function getActionOnUnitData() {
		return "++res;";
	}
}

?>