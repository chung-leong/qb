<?php

class ClearArrayResize extends Handler {

	use ArrayAddressMode, UnaryOperator, MainThreadExecution;
	
	public function changesOperandSize($i) {
		return ($i == 2);
	}
	
	public function needsInterpreterContext() {
		return true;
	}
	
	public function getOperandType($i) {
		switch($i) {
			case 1: return "U32";					// segment selector
			case 2: return $this->operandType;
		}
	}
	
	public function getOperandAddressMode($i) {
		switch($i) {
			case 1: return "CON";
			case 2: return "ARR";
		}
	}
	
	protected function getActionOnUnitData() {
		$lines = array();
		$lines[] = "uint32_t i;";
		$lines[] = "for(i = 0; i < res_count; i++) {";
		$lines[] =		"res_ptr[i] = 0;";
		$lines[] = "}";
		$lines[] = "res_count = 0;";
		$lines[] = "qb_resize_segment(&cxt->function->local_storage->segments[op1], 0);";
		return $lines;
	}
}

?>