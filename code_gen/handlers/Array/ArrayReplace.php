<?php

class ArrayReplace extends Handler {

	use MultipleAddressMode, SenaryOperator, ResizeResult, NotThreadSafe;
	
	public function getOperandType($i) {
		switch($i) {
			case 1: return $this->operandType;	// source array
			case 2: return "S32";				// index
			case 3: return "S32";				// length
			case 4: return "U32";				// destination dimenesion
			case 5: return "U32";				// destination sub-array size
			case 6: return "U32";				// segment selector
			case 7: return $this->operandType;	// destination array
		}
	}
	
	public function changesOperand($i) {
		switch($i) {
			case 4:
			case 7: return true;
			default: return FALSE;
		}
	}
	
	public function changesOperandSize($i) {
		switch($i) {
			case 7: return true;
			default: return FALSE;
		}
	}
	
	public function getOperandAddressMode($i) {
		switch($i) {
			case 1: return "ARR";		// source array
			case 2: return "SCA";		// index
			case 3: return "SCA";		// length
			case 4: return "SCA";		// destination dimenesion
			case 5: return "SCA";		// destination sub-array size
			case 6: return "CON";		// segment selector
			case 7: return "ARR";		// destination array
		}
	}
		
	public function getOperandSize($i) {
		switch($i) {
			case 1: return "op1_count";
			case 2: return 1;
			case 3: return 1;
			case 4: return 1;
			case 5: return 1;
			case 6: return 1;
			case 7: return "res_count";
		}
	}
	
	public function getActionOnUnitData() {
		$type = $this->getOperandType(1);
		$cType = $this->getOperandCType(1);
		$lines = array();
		$lines[] = "$cType *op1_end = op1_ptr + op1_count;";
		$lines[] = "uint32_t replacement_len = op1_count, extraction_len;";
		$lines[] = "uint32_t start_index, end_index;";
		$lines[] = "if(op2 >= 0) {";
		$lines[] =		"start_index = op2;";
		$lines[] = "} else {";
		$lines[] = 		"start_index = op4 + op2;";
		$lines[] = 		"if((int32_t) start_index < 0) {";
		$lines[] =			"start_index = 0;";
		$lines[] = 		"}";
		$lines[] = "}";
		$lines[] = "if(op3 >= 0) {";
		$lines[] =		"end_index = start_index + op3;";
		$lines[] =		"if(end_index > op4) {";
		$lines[] =			"end_index = op4;";
		$lines[] =		"}";
		$lines[] = "} else {";
		$lines[] =		"end_index = op4 + op3;";
		$lines[] = "}";
		$lines[] = "extraction_len = (end_index - start_index) * op5;";
		$lines[] = "if(extraction_len != replacement_len) {";
		$lines[] =		"uint32_t remaining_len = (op4 - end_index) * op5;";
		$lines[] =		"int32_t change_len = replacement_len - extraction_len;";
		$lines[] = 		"if(remaining_len != 0 && change_len < 0) {";
		$lines[] =			"$cType *end_ptr = res_ptr + end_index * op5, *new_end_ptr = end_ptr + change_len;";
		$lines[] =			"memmove(new_end_ptr, end_ptr, remaining_len * sizeof($cType));";
		$lines[] = 		"}";
		$lines[] =		"res_ptr = ($cType *) (((int8_t *) res_ptr) + qb_resize_segment(&cxt->function->local_storage->segments[op6], (res_count + change_len) * sizeof($cType)));";
		$lines[] =		"if(remaining_len != 0 && change_len > 0) {";
		$lines[] =			"$cType *end_ptr = res_ptr + end_index * op5, *new_end_ptr = end_ptr + change_len;";
		$lines[] =			"memmove(new_end_ptr, end_ptr, remaining_len * sizeof($cType));";
		$lines[] =		"}";
		$lines[] =		"if(op4_ptr != res_count_ptr) {";
		$lines[] =			"*op4_ptr += change_len / ((int32_t) op5);";
		$lines[] =		"}";
		$lines[] =		"res_count += change_len;";
		$lines[] = "}";
		$lines[] = "if(replacement_len > 0) {";
		$lines[] =		"$cType *ptr = res_ptr + start_index * op5;";
		$lines[] = 		"while(op1_ptr < op1_end) {";
		$lines[] =			"*ptr = *op1_ptr;";
		$lines[] =			"op1_ptr++;";
		$lines[] =			"ptr++;";
		$lines[] =		"}";
		$lines[] = "}";
		return $lines;
	}
}

?>
