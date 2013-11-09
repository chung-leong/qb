<?php

class AccommodateInsertUpdateDimensionMultiply extends Handler {

	use ScalarAddressMode, SeptenaryOperator, MainThreadExecution;
	
	public function changesOperand($i) {
		return ($i == 2 || $i == 4 || $i == 8);
	}
	
	public function needsInterpreterContext() {
		return true;
	}
	
	public function getOperandType($i) {
		switch($i) {
			case 1: return "U32";		// index
			case 2: return "U32";		// dimension
			case 3: return "U32";		// sub-array size
			case 4: return "U32";		// array size
			case 5: return "U32";		// count
			case 6: return "U32";		// segment selector
			case 7: return "U32";		// element byte-count
			case 8: return "U32";
		}
	}
	
	public function getOperandAddressMode($i) {
		switch($i) {
			case 1: return "SCA";
			case 2: return "SCA";
			case 3: return "SCA";
			case 4: return "SCA";
			case 5: return "SCA";
			case 6: return "CON";
			case 7: return "CON";
			case 8: return "SCA";
		}
	}
	
	protected function getActionOnUnitData() {
		$lines = array();
		$lines[] = "uint32_t new_dim = op2 + op5;";
		$lines[] = "uint32_t new_size = op4 + (op5 * op3);";
		$lines[] = "uint32_t element_size = op3 * op7;";
		$lines[] = "qb_memory_segment *segment = &cxt->function->local_storage->segments[op6];";
		$lines[] = "qb_resize_segment(segment, new_size * op7);";
		$lines[] = "if(op1 < op2) {";
		$lines[] = 		"memmove(segment->memory + (op1 + op5) * element_size, segment->memory + op1 * element_size, (op2 - op1) * element_size);";
		$lines[] = "}";
		$lines[] = "op2 = new_dim;";
		$lines[] = "op4 = new_size;";
		$lines[] = "res = op5 * op3;";
		return $lines;
	}
}

?>