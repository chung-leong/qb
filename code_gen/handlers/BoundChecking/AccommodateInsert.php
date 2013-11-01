<?php

class AccommodateInsert extends Handler {

	use ScalarAddressMode, QuinaryOperator, MainThreadExecution;
	
	public function getOutputOperandCount() {
		return 0;
	}

	public function changesOperand($i) {
		return ($i == 2);
	}
	
	public function needsInterpreterContext() {
		return true;
	}
	
	public function getOperandType($i) {
		switch($i) {
			case 1: return "U32";		// index
			case 2: return "U32";		// array size
			case 3: return "U32";		// count
			case 4: return "U32";		// segment selector
			case 5: return "U32";		// element byte-count
		}
	}
	
	public function getOperandAddressMode($i) {
		switch($i) {
			case 1: return "SCA";
			case 2: return "SCA";
			case 3: return "SCA";
			case 4: return "CON";
			case 5: return "CON";
		}
	}
	
	protected function getActionOnUnitData() {
		$lines = array();
		$lines[] = "uint32_t new_size = op2 + op3;";
		$lines[] = "qb_memory_segment *segment = &cxt->function->local_storage->segments[op4];";
		$lines[] = "qb_resize_segment(segment, new_size * op4);";
		$lines[] = "if(op1 < op2) {";
		$lines[] = 		"memmove(segment->memory + (op1 + op3) * op5, segment->memory + op1 * op5, (op2 - op1) * op5);";
		$lines[] = "}";
		$lines[] = "op2 = new_size;";
		return $lines;
	}
}

?>