<?php

class SetAny extends Handler {

	use MultipleAddressMode, UnaryOperator, BooleanResult;
	
	public function getOperandAddressMode($i) {
		switch($i) {
			case 1: return "ARR";
			case 2: return $this->addressMode;
		}
	}
	
	public function getOperandSize($i) {
		switch($i) {
			case 1: return "op1_count";
			case 2: return 1;
		}
	}
	
	protected function getActionOnUnitData() {
		$cType = $this->getOperandCType(1);
		$lines[] = array();
		$lines[] = "$cType *op1_end = op1_ptr + op1_count;";
		$lines[] = "res = 0;";
		$lines[] = "while(op1_ptr != op1_end) {";
		$lines[] = 		"if(op1) {";
		$lines[] =			"res = 1;";
		$lines[] =			"break;";
		$lines[] = 		"}";
		$lines[] =		"op1_ptr++;";
		$lines[] = "}";
		return $lines;
	}
}

?>