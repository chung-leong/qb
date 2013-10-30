<?php

class PrintMultidimensionalVariable extends Handler {

	use ArrayAddressMode, UseSprintf;
	
	public function getInputOperandCount() {
		return 2;
	}

	public function getOperandType($i) {
		switch($i) {
			case 1: return $this->operandType;
			case 2: return "U32";
		}
	}

	public function getOperandAddressMode($i) {
		switch($i) {
			case 1: return "ARR";
			case 2: return "ARR";
		}
	}

	public function getOperandSize($i) {
		switch($i) {
			case 1: return "op1_count";
			case 2: return "op2_count";
		}
	}
	
	public function getActionOnUnitData() {
		$sprintf = $this->getSprintf("*op1_ptr");
		$cType = $this->getOperandCType(1);
		$lines = array();
		$lines[] = "USE_TSRM";
		$lines[] = "$cType *op1_end = op1_ptr + op1_count;";
		$lines[] = "uint32_t depth = 0, i;";
		$lines[] = "uint32_t counts[MAX_DIMENSION];";
		$lines[] = "for(i = 0; i < op2_count; i++) {";
		$lines[] = 		"counts[i] = 0;";
		$lines[] = "}";
		$lines[] = "qb_write_output(cxt, \"[\", 1);";
		$lines[] = "while(op1_ptr < op1_end || depth > 0) {";
		$lines[] = 		"if(counts[depth] < op2_ptr[depth]) {";
		$lines[] = 			"if(counts[depth] > 0) {";
		$lines[] = 				"qb_write_output(cxt, \", \", 2);";
		$lines[] = 			"}";
		$lines[] = 			"if(depth + 1 == op2_count) {";
		$lines[] = 				"char sprintf_buffer[64];";
		$lines[] = 				"uint32_t len = $sprintf;";
		$lines[] = 				"qb_write_output(cxt, sprintf_buffer, len);";
		$lines[] = 				"op1_ptr++;";
		$lines[] = 				"counts[depth]++;";
		$lines[] = 			"} else {";
		$lines[] = 				"qb_write_output(cxt, \"[\", 1);";
		$lines[] = 				"depth++;";
		$lines[] = 			"}";
		$lines[] = 		"} else {";
		$lines[] = 			"qb_write_output(cxt, \"]\", 1);";
		$lines[] = 			"counts[depth] = 0;";
		$lines[] = 			"depth--;";
		$lines[] = 			"counts[depth]++;";
		$lines[] = 		"}";
		$lines[] = "}";
		$lines[] = "qb_write_output(cxt, \"]\", 1);";
		return $lines;
	}
}

?>