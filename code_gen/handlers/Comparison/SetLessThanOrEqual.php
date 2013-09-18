<?php

class SetLessThanOrEqual extends Handler {

	use ArrayAddressMode, BinaryOperator, BooleanResult;
	
	protected function getActionOnUnitData() {
		return "res = (op1 <= op2);";
	}
}

?>