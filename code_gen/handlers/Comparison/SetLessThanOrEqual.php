<?php

class LessThanOrEqualVector extends Handler {

	use ArrayAddressMode;
	use BinaryOperator;
	use BooleanResult;
	
	protected function getActionOnUnitData() {
		return "res = (op1 <= op2);";
	}
}

?>