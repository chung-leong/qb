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
		$lines[] = "php_write(\"[\", 1 TSRMLS_CC);";
		$lines[] = "while(op1_ptr < op1_end || depth > 0) {";
		$lines[] = 		"if(counts[depth] < op2_ptr[depth]) {";
		$lines[] = 			"if(counts[depth] > 0) {";
		$lines[] = 				"php_write(\", \", 2 TSRMLS_CC);";
		$lines[] = 			"}";
		$lines[] = 			"if(depth + 1 == op2_count) {";
		$lines[] = 				"char sprintf_buffer[64];";
		$lines[] = 				"uint32_t len = $sprintf;";
		$lines[] = 				"php_write(sprintf_buffer, len TSRMLS_CC);";
		$lines[] = 				"op1_ptr++;";
		$lines[] = 				"counts[depth]++;";
		$lines[] = 			"} else {";
		$lines[] = 				"php_write(\"[\", 1 TSRMLS_CC);";
		$lines[] = 				"depth++;";
		$lines[] = 			"}";
		$lines[] = 		"} else {";
		$lines[] = 			"php_write(\"]\", 1 TSRMLS_CC);";
		$lines[] = 			"counts[depth] = 0;";
		$lines[] = 			"depth--;";
		$lines[] = 			"counts[depth]++;";
		$lines[] = 		"}";
		$lines[] = "}";
		$lines[] = "php_write(\"]\", 1 TSRMLS_CC);";
		return $lines;
	}
}

?>