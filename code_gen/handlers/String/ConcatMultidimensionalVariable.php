<?php

class ConcatMultidimensionalVariable extends Handler {

	use ArrayAddressMode, ArrayResult, TernaryOperator, UseSprintf, ResizeResult {
        ResizeResult::needsInterpreterContext insteadof UseSprintf;
	}
	
	public function getOperandType($i) {
		switch($i) {
			case 1: return $this->operandType;
			case 2: return "U32";
			case 3: return "U32";
			case 4: return "U08";
		}
	}
	
	public function getOperandAddressMode($i) {
		switch($i) {
			case 1: return "ARR";
			case 2: return "ARR";
			case 3: return "CON";
			case 4: return "ARR";
		}
	}

	public function getActionOnUnitData() {
		$sprintf = $this->getSprintf("*op1_ptr");
		$cType = $this->getOperandCType(1);
		$lines = array();
		$lines[] = "$cType *op1_start = op1_ptr;";
		$lines[] = "$cType *op1_end = op1_ptr + op1_count;";
		$lines[] = "uint32_t *op2_start = op2_ptr;";
		$lines[] = "uint32_t *op2_end = op2_ptr + op2_count;";
		$lines[] = "uint32_t depth = 0, pos = 0;";
		$lines[] = "uint32_t counts[64];";
		$lines[] = "uint32_t total = 0, multiplier = 1;";
		$lines[] = "while(op1_ptr < op1_end) {";
		$lines[] = 		"char sprintf_buffer[64];";
		$lines[] = 		"uint32_t len = $sprintf;";
		$lines[] = 		"total += len;";
		$lines[] = 		"op1_ptr++;";
		$lines[] = "}";
		$lines[] = "while(op2_ptr < op2_end) {";
		$lines[] = 		"total += multiplier * *op2_ptr * 2;";
		$lines[] = 		"multiplier *= *op2_ptr;";
		$lines[] = 		"op2_ptr++;";
		$lines[] = "}";
		$lines[] = "if(total == 0) {";
		$lines[] = 		"total = 2;";
		$lines[] = "}";
		$lines[] = "res_ptr += qb_resize_array(cxt, local_storage, op3, res_count + total);";
		$lines[] = "memset(counts, 0, sizeof(uint32_t) * op2_count);";
		$lines[] = "res_ptr[pos++] = '[';";
		$lines[] = "op1_ptr = op1_start;";
		$lines[] = "op2_ptr = op2_start;";
		$lines[] = "while(op1_ptr < op1_end || depth > 0) {";
		$lines[] = 		"if(counts[depth] < op2_ptr[depth]) {";
		$lines[] = 			"if(counts[depth] > 0) {";
		$lines[] = 				"res_ptr[pos++] = ',';";
		$lines[] = 				"res_ptr[pos++] = ' ';";
		$lines[] = 			"}";
		$lines[] = 			"if(depth + 1 == op2_count) {";
		$lines[] = 				"char sprintf_buffer[64];";
		$lines[] = 				"uint32_t len = $sprintf;";
		$lines[] = 				"memcpy(res_ptr + pos, sprintf_buffer, len);";
		$lines[] = 				"pos += len;";
		$lines[] = 				"op1_ptr++;";
		$lines[] = 				"counts[depth]++;";
		$lines[] = 			"} else {";
		$lines[] = 				"res_ptr[pos++] = '[';";
		$lines[] = 				"depth++;";
		$lines[] = 			"}";
		$lines[] = 		"} else {";
		$lines[] = 			"res_ptr[pos++] = ']';";
		$lines[] = 			"counts[depth] = 0;";
		$lines[] = 			"depth--;";
		$lines[] = 			"counts[depth]++;";
		$lines[] = 		"}";
		$lines[] = "}";
		$lines[] = "res_ptr[pos++] = ']';";
		$lines[] = "res_count += total;";
		return $lines;
	}
}

?>