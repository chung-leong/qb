<?php

class PrintString extends Handler {

	use ArrayAddressMode, MainThreadExecution;
	
	public function getInputOperandCount() {
		return 1;
	}

	public function getOperandAddressMode($i) {
		return "ARR";
	}
	
	public function getOperandSize($i) {
		switch($i) {
			case 1: return "op1_count";
		}
	}

	public function getActionOnUnitData() {
		$lines = array();
		$lines[] = "USE_TSRM";
		$lines[] = "php_write(op1_ptr, op1_count TSRMLS_CC);";
		return $lines;
	}
}

?>