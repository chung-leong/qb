<?php

class SetNot extends Handler {

	use ArrayAddressMode, UnaryOperator, BooleanResult;

	protected function getActionOnUnitData() {
		return $this->getIterationCode("res = (!op1);");
	}
}

?>