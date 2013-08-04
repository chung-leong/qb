<?php

class QBArrayReverseHandler extends QBHandler {

	public function getInputOperandCount() {
		return 2;
	}

	public function getOperandAddressMode($i) {
		switch($i) {
			case 1: return "ARR";
			case 2: return "VAR";
			case 3: return "ARR";
		}
	}
	
	public function getOperandType($i) {
		switch($i) {
			case 1: return $this->operandType;
			case 2: return "U32";
			case 3: return $this->operandType;
		}
	}
	
	public function getActionOnUnitData() {
		$type = $this->getOperandType(1);
		$cType = $this->getOperandCType(1);
		$lines = array();
		$lines[] = "uint32_t i, j;";
		$lines[] = "if(op2 == 1) {";
		$lines[] = 		"for(i = 0, j = op1_count - 1; i <= j; i++, j--) {";
		$lines[] = 			"$cType t = op1_ptr[i];";
		$lines[] = 			"res_ptr[i] = op1_ptr[j];";
		$lines[] = 			"res_ptr[j] = t;";
		$lines[] = 		"}";
		$lines[] = "} else {";
		$lines[] = 		"ALLOCA_FLAG(use_heap)";
		$lines[] = 		"$cType *t = do_alloca(op2 * sizeof($cType), use_heap);";
		$lines[] = 		"for(i = 0, j = op1_count - op2; i <= j; i += op2, j -= op2) {";
		$lines[] = 			"memcpy(t, op1_ptr + i, op2 * sizeof($cType));";
		$lines[] = 			"memcpy(res_ptr + i, op1_ptr + j, op2 * sizeof($cType));";
		$lines[] = 			"memcpy(res_ptr + j, t, op2 * sizeof($cType));";
		$lines[] = 		"}";
		$lines[] = 		"free_alloca(t, use_heap);";
		$lines[] = "}";
		return $lines;
	}	
}

?>