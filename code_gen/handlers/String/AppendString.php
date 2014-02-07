<?php

class AppendString extends Handler {

	use ArrayAddressMode, BinaryOperator, ResizeResult, NotThreadSafe;
	
	public function getOperandType($i) {
		switch($i) {
			case 1: return $this->operandType;
			case 2: return "U32";
			case 3: return "U08";
		}
	}
	
	public function getOperandAddressMode($i) {
		switch($i) {
			case 1: return "ARR";
			case 2: return "CON";
			case 3: return "ARR";
		}
	}
	
	public function getOperandSize($i) {
		switch($i) {
			case 1: return "op1_count";
			case 2: return 1;
			case 3: return "op3_count";
		}
	}

	public function getActionOnUnitData() {
		$lines[] = "res_ptr += qb_resize_segment(&cxt->function->local_storage->segments[op2], res_count + op1_count);";
		$lines[] = "memcpy(res_ptr + res_count, op1_ptr, op1_count);";
		$lines[] = "res_count += op1_count;";
		return $lines;
	}
}

?>