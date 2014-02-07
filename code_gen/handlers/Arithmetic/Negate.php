<?php

class Negate extends Handler {

	use MultipleAddressMode, UnaryOperator, ExpressionReplication, NoUnsigned, Multithreaded;
	
	protected function getActionOnUnitData() {
		return "res = - op1;";
	}
}

?>