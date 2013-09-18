<?php

class Range extends Handler {

	use MultipleAddressMode, UnaryOperator, ArrayResult;
	
	public function getInputOperandCount() {
		return 3;
	}

	public function getOperandType($i) {
		switch($i) {
			case 1:
			case 2:
			case 4: return $this->operandType;
			case 3:
				if($this->operandType[0] == 'U') {
					return "S" . substr($this->operandType, 1);
				} else {
					return $this->operandType;
				}
		}
	}
	
	public function getHelperFunctions() {
		$type = $this->getOperandType(1);
		$cType = $this->getOperandCType(1);
		$cType3 = $this->getOperandCType(3);
		$lines = array();
		$lines[] = "uint32_t qb_get_range_length_$type($cType op1, $cType op2, $cType3 op3) {";
		$lines[] = 		"$cType count;";
		$lines[] = 		"$cType distance;";
		$lines[] = 		"$cType interval;";
		$lines[] = 		"if(op3 > 0 && op2 >= op1) {";
		$lines[] = 			"distance = op2 - op1;";
		$lines[] = 			"interval = op3;";
		$lines[] = 		"} else if(op3 < 0 && op1 >= op2) {";
		$lines[] = 			"distance = op1 - op2;";
		$lines[] = 			"interval = -op3;";
		$lines[] = 		"} else {";
		$lines[] = 			"distance = 0;";
		$lines[] = 			"interval = 1;";
		$lines[] = 		"}";
		$lines[] = 		"distance += interval;";
		$lines[] = 		"count = (interval != 1) ? distance / interval : distance;";
		if($type == 'F32' || $type == 'F64' || $type == 'I64' || $type == 'U64') {
			$lines[] =	"if(count > 4294967295) {";
			$lines[] =		"count = 4294967295;";
			$lines[] =	"}";
		}
		$lines[] = 		"return (uint32_t) count;";
		$lines[] = "}";
		return array($lines);
	}
	
	public function getResultSizePossibilities() {
		return "string_length";
	}

	public function getResultSizeCalculation() {
		$type = $this->getOperandType(1);
		return "string_length = qb_get_range_length_$type(op1, op2, op3);";
	}		

	public function needsStringLength() {
		return true;
	}
	
	public function getActionOnUnitData() {
		$type = $this->getOperandType(1);
		$cType = $this->getOperandCType(1);
		$lines = array();
		$lines[] = "$cType *res_end = res_ptr + string_length;";
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