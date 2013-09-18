<?php

class ArrayInsert extends Handler {

	public function getInputOperandCount() {
		return 2;
	}

	public function getOperandAddressMode($i) {
		return ($i == 2) ? "SCA" : "ARR";
	}
	
	public function getOperandType($i) {
		return ($i == 2) ? "U32" : $this->operandType;
	}
	
	public function getResultSizePossibilities() {
		return "res_count + op1_count";
	}
	
	public function needsOriginalSize() {
		return true;
	}
	
	public function getActionOnUnitData() {
		$type = $this->getOperandType(1);
		$cType = $this->getOperandCType(1);
		$lines = array();
		$lines[] = "memmove(res_ptr + op2 + op1_count, res_ptr + op2, sizeof($cType) * (res_count_before - op2));";
		$lines[] = "memcpy(res_ptr + op2, op1_ptr, sizeof($cType) * op1_count);";
		return $lines;
	}
}

?>