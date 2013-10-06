<?php

class ClearScalar extends Handler {

	use ScalarAddressMode, NullaryOperator;

	public function getOperandType($i) {
		switch($i) {
			case 1: return $this->operandType;
		}
	}
	
	public function getOperandAddressMode($i) {
		switch($i) {
			case 1: return "SCA";
		}
	}
	
	protected function getActionOnUnitData() {
		$lines = array();
		$lines[] = "res = 0;";
		return $lines;
	}
}

?>