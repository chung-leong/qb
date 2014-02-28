<?php

class RangeCount extends Handler {

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
			case 4: return "U32";
		}
	}
	
	public function getOperandAddressMode($i) {
		switch($i) {
			case 1: return $this->addressMode;
			case 2: return $this->addressMode;
			case 3: return $this->addressMode;
			case 4: return "SCA";
		}
	}
	
	public function getOperandSize($i) {
		switch($i) {
			case 1: return 1;
			case 2: return 1;
			case 3: return 1;
			case 4: return 1;
		}
	}
	
	public function getActionOnUnitData() {
		$type = $this->getOperandType(1);
		$cType = $this->getOperandCType(1);
		$lines = array();
		$lines[] = "$cType count;";
		$lines[] = "$cType distance;";
		$lines[] = "$cType interval;";
		$lines[] = "if(op3 > 0 && op2 >= op1) {";
		$lines[] = 	"distance = op2 - op1;";
		$lines[] = 	"interval = op3;";
		$lines[] = "} else if(op3 < 0 && op1 >= op2) {";
		$lines[] = 	"distance = op1 - op2;";
		$lines[] = 	"interval = -op3;";
		$lines[] = "} else {";
		$lines[] = 	"distance = 0;";
		$lines[] = 	"interval = 1;";
		$lines[] = "}";
		$lines[] = "distance += interval;";
		$lines[] = "count = (interval != 1) ? distance / interval : distance;";
		if($type == 'F32' || $type == 'F64' || $type == 'I64' || $type == 'U64') {
			$lines[] =	"res = ((uint64_t) count > UINT32_MAX) ? UINT32_MAX : (uint32_t) count;";
		} else {
			$lines[] =	"res = (uint32_t) count;";
		}
		return $lines;
	}
}

?>
