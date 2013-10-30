<?php

class SetLessThan extends Handler {

	use ArrayAddressMode, BinaryOperator, BooleanResult;
	
	public function getOperandSize($i) {
		switch($i) {
			case 1: return "op1_count";
			case 2: return "op2_count";
			case 3: return "res_count";
		}
	}
	
	protected function getActionOnUnitData() {
		return $this->getIterationCode("res = (op1 < op2);");
	}
}

?>