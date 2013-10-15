<?php

class SetEqual extends Handler {

	use ArrayAddressMode, BinaryOperator, BooleanResult;
		
	protected function getActionOnUnitData() {
		return $this->getIterationCode("res = (op1 == op2);");
	}
}

?>