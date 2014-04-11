<?php

class PrintString extends Handler {

	use ArrayAddressMode, UnaryOperatorNoResult, MainThreadExecution;
	
	public function getOperandAddressMode($i) {
		return "ARR";
	}
	
	public function getOperandSize($i) {
		switch($i) {
			case 1: return "op1_count";
		}
	}

	public function getActionOnUnitData() {
		$type = $this->getOperandType(1);
		$lines = array();
		$lines[] = "USE_TSRM";
		if($type == "U08") {
			$lines[] = "php_write(op1_ptr, op1_count TSRMLS_CC);";
		} else {
			$lines[] = "char buffer[256];";
			$lines[] = "uint32_t length = 0, i;";
			$lines[] = "for(i = 0; i < op1_count; i++) {";
			$lines[] =		"if(i == op1_count - 1 || length >= 250) {";
			$lines[] =			"php_write(buffer, op1_count TSRMLS_CC);";
			$lines[] =		"}";
			$lines[] = 		"length += encode(op1_ptr[i], buffer + length);";
			$lines[] = "}";
		}
		return $lines;
	}
}

?>