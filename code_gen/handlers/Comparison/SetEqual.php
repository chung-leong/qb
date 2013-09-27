<?php

class SetEqual extends Handler {

	use ArrayAddressMode, BinaryOperator, SetOperator, BooleanResult;
		
	protected function getActionOnUnitData() {
		return "res = (op1 == op2);";
	}
}

?>