<?php

class Range extends Handler {

	use MultipleAddressMode, TernaryOperator;
	
	public function getOperandType($i) {
		switch($i) {
			case 1: return $this->operandType;
			case 2: return $this->operandType;
			case 3:
				if($this->operandType[0] == 'U') {
					return "S" . substr($this->operandType, 1);
				} else {
					return $this->operandType;
				}
			case 4: return $this->operandType;
		}
	}
	
	public function getOperandAddressMode($i) {
		switch($i) {
			case 1: return $this->addressMode;
			case 2: return $this->addressMode;
			case 3: return $this->addressMode;
			case 4: return "ARR";
		}
	}
	
	public function getOperandSize($i) {
		switch($i) {
			case 1: return 1;
			case 2: return 1;
			case 3: return 1;
			case 4: return "res_count";
		}
	}
	
	public function getActionOnUnitData() {
		$type = $this->getOperandType(1);
		$cType = $this->getOperandCType(1);
		$lines = array();
		$lines[] = "$cType *res_end = res_ptr + res_count;";
		$lines[] = "$cType num = op1;";
		$lines[] = "while(res_ptr < res_end) {";
		$lines[] = 		"*res_ptr = num;";
		$lines[] = 		"res_ptr++;";
		$lines[] =		"num += op3;";
		$lines[] = "}";
		return $lines;
	}
}

?>