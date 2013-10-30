<?php

class SetNot extends Handler {

	use ArrayAddressMode, UnaryOperator, BooleanResult;

	public function getOperandSize($i) {
		switch($i) {
			case 1: return "op1_count";
			case 2: return "res_count";
		}
	}
	
	protected function getActionOnUnitData() {
		return $this->getIterationCode("res = (!op1);", 1);
	}
}

?>