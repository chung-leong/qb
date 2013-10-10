<?php

class AppendVariable extends Handler {

	use MultipleAddressMode, BinaryOperator, UseSprintf, ResizeResult {
        ResizeResult::needsInterpreterContext insteadof UseSprintf;
	}
	
	public function getOperandType($i) {
		switch($i) {
			case 1: return $this->operandType;
			case 2: return "U32";
			case 3: return "U08";
		}
	}
	
	public function getOperandAddressMode($i) {
		switch($i) {
			case 1: return $this->addressMode;
			case 2: return "CON";
			case 3: return "ARR";
		}
	}
	
	public function getActionOnUnitData() {
		$sprintf = $this->getSprintf("op1");
		$lines = array();
		$lines[] = "char sprintf_buffer[64];";
		$lines[] = "uint32_t len = $sprintf;";
		$lines[] = "res_ptr += qb_adjust_memory_segment(cxt, local_storage, op2, res_count + len);";
		$lines[] = "memcpy(res_ptr + res_count, sprintf_buffer, len);";
		$lines[] = "res_count += len;";
		return $lines;	
	}
	
	public function getActionOnMultipleData() {
		$sprintf = $this->getSprintf("*op1_ptr");
		$cType = $this->getOperandCType(1);
		$lines = array();
		$lines[] = "uint32_t pos = res_count;";
		$lines[] = "uint32_t total = 0;";
		$lines[] = "$cType *op1_start = op1_ptr;";
		$lines[] = "$cType *op1_end = op1_ptr + op1_count;";
		$lines[] = "if(op1_count) {";
		$lines[] = 		"while(op1_ptr < op1_end) {";
		$lines[] = 			"char sprintf_buffer[64];";
		$lines[] = 			"uint32_t len = $sprintf;";
		$lines[] = 			"total += len;";
		$lines[] = 			"op1_ptr++;";
		$lines[] = 		"}";
		$lines[] = 		"total += op1_count * 2;";
		$lines[] = "} else {";
		$lines[] = 		"total = 2;";
		$lines[] = "}";
		$lines[] = "res_ptr += qb_adjust_memory_segment(cxt, local_storage, op2, res_count + total);";
		$lines[] = "res_ptr[pos++] = '[';";
		$lines[] = "op1_ptr = op1_start;";
		$lines[] = "while(op1_ptr < op1_end) {";
		$lines[] = 		"char sprintf_buffer[64];";
		$lines[] = 		"uint32_t len = $sprintf;";
		$lines[] = 		"memcpy(res_ptr + pos, sprintf_buffer, len);";
		$lines[] = 		"pos += len;";
		$lines[] = 		"op1_ptr++;";
		$lines[] = 		"if(op1_ptr != op1_end) {";
		$lines[] = 			"res_ptr[pos++] = ',';";
		$lines[] = 			"res_ptr[pos++] = ' ';";
		$lines[] = 		"}";
		$lines[] = "}";
		$lines[] = "res_ptr[pos++] = ']';";
		$lines[] = "res_count += total;";
		return $lines;
	}
}

?>