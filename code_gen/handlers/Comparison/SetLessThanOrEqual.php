<?php

class SetLessThanOrEqual extends Handler {

	use ArrayAddressMode, BinaryOperator, SetOperator, BooleanResult;
	
	protected function getActionOnUnitData() {
		return "res = (op1 <= op2);";
	}
}

?>