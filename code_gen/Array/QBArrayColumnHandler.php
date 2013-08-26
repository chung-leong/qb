<?php

class QBArrayColumnHandler extends QBHandler {

	public function getInputOperandCount() {
		return 4;
	}

	public function getOperandType($i) {
		switch($i) {
			case 1: return "U32";
			case 2: return "U32";
			case 3: return "U32";
			case 4: return $this->operandType;
			case 5: return $this->operandType;
		}
	}
	
	public function getOperandAddressMode($i) {
		switch($i) {
			case 1: return "VAR";
			case 2: return "VAR";
			case 3: return "VAR";
			case 4: return "ARR";
			case 5: return "ARR";
		}
	}
		
	public function getResultSizePossibilities() {
		return "vector_count";
	}
	
	public function getResultSizeCalculation() {
		$lines = array();
		$lines[] = "vector_count = op4_count / op2;";
		return $lines;
	}
	
	public function getOperandRetrievalCode($i) {
		$lines = parent::getOperandRetrievalCode($i);
		if($i == 4) {
			$lines[] = "if(UNEXPECTED(op1 >= op2)) {";
			$lines[] =		"qb_abort_range_error(cxt, &cxt->storage->segments[selector], op4_count + (op1 - op2), op3, PHP_LINE_NUMBER);";
			$lines[] = "}";
		}
		return $lines;
	}

	public function getActionOnUnitData() {
		$cType = $this->getOperandCType(4);
		$lines = array();
		$lines[] = "$cType *op4_end = op4_ptr + op4_count;";
		$lines[] = "op4_ptr += op1 * op3;";
		$lines[] = "if(op3 == 1) {";
		$lines[] = 		"while(op4_ptr < op4_end) {";
		$lines[] =			"*res_ptr = *op4_ptr;";
		$lines[] =			"res_ptr += 1;";
		$lines[] =			"op4_ptr += op2;";
		$lines[] =		"}";
		$lines[] = "} else {";
		$lines[] = 		"while(op4_ptr < op4_end) {";
		$lines[] =			"memcpy(res_ptr, op4_ptr, op3 * sizeof($cType));";
		$lines[] =			"res_ptr += op3;";
		$lines[] =			"op4_ptr += op2;";
		$lines[] =		"}";
		$lines[] = "}";
		return $lines;
	}
}

?>