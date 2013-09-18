<?php

class Negate extends Handler {

	use MultipleAddressMode, UnaryOperator;
	
	protected function getActionOnUnitData() {
		return "res = - op1;";
	}
}

?>