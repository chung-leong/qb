<?php

class ClearElementResize extends Handler {

	use ArrayAddressMode, TernaryOperator;
	
	public function changesOperandSize($i) {
		return ($i == 4);
	}
	
	public function needsInterpreterContext() {
		return true;
	}
	
	public function getOperandType($i) {
		switch($i) {
			case 1: return "U32";				// element index
			case 2: return "U32";				// element count
			case 3: return "U32";				// segment offset
			case 4: return $this->operandType;	// array containing element
		}
	}
	
	public function getOperandAddressMode($i) {
		switch($i) {
			case 1: return "SCA";
			case 2: return "SCA";
			case 3: return "CON";
			case 4: return "ARR";
		}
	}
	
	protected function getActionOnUnitData() {
		$cType = $this->getOperandCType(4);
		$lines = array();
		$lines[] = "uint32_t i, shift = op2, start = op1 * op2, end = res_count - shift;";
		$lines[] = "if(start <= end) {";
		$lines[] = 		"for(i = start; i < end; i++) {";
		$lines[] =			"res_ptr[i] = res_ptr[i + shift];";
		$lines[] = 		"}";
		$lines[] = 		"for(i = end; i < res_count; i++) {";
		$lines[] =			"res_ptr[i] = 0;";
		$lines[] = 		"}";
		$lines[] = 		"res_count = end;";
		$lines[] = 		"qb_resize_segment(&cxt->function->local_storage->segments[op3], end * sizeof($cType));";
		$lines[] = "}";
		return $lines;
	}
}

?>