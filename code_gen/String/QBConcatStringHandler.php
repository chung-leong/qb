<?php

class QBConcatStringHandler extends QBHandler {

	public function getOperandType($i) {
		// for now, $this->operandType should be going to be U08 as well
		return ($i == 2) ? "U08" : $this->operandType;
	}

	public function getOperandAddressMode($i) {
		// string concatenation always involves arrays
		return "ARR";
	}
	
	public function getResultSizePossibilities() {
		return "res_count + op1_count";
	}
	
	public function needsOriginalSize() {
		return true;
	}
	
	public function getActionOnUnitData() {
		return "memcpy(res_ptr + res_count_before, op1_ptr, op1_count);";
	}
}

?>