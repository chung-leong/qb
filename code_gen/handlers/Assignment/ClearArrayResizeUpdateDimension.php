<?php

class ClearArrayResizeUpdateDimension extends Handler {

	use ArrayAddressMode, BinaryOperator;
	
	public function changesOperandSize($i) {
		return ($i == 3);
	}
	
	public function changesOperand($i) {
		return ($i == 1 || $i == 3);
	}
	
	public function needsInterpreterContext() {
		return true;
	}
	
	public function needsLocalStorage() {
		return true;
	}

	public function getOperandType($i) {
		switch($i) {
			case 1: return "U32";					// the first dimension
			case 2: return "U32";					// segment selector
			case 3: return $this->operandType;
		}
	}
	
	public function getOperandAddressMode($i) {
		switch($i) {
			case 1: return "SCA";
			case 2: return "CON";
			case 3: return "ARR";
		}
	}
	
	protected function getActionOnUnitData() {
		$lines = array();
		$lines[] = "uint32_t i;";
		$lines[] = "for(i = 0; i < res_count; i++) {";
		$lines[] =		"res_ptr[i] = 0;";
		$lines[] = "}";
		$lines[] = "op1 = 0;";
		$lines[] = "res_count = 0;";
		$lines[] = "qb_adjust_memory_segment(cxt, op2, 0);";
		return $lines;
	}
}

?>