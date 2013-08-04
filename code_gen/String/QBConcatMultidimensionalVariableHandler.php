<?php

class QBConcatMultidimensionalVariableHandler extends QBPrintHandler {

	public function getInputOperandCount() {
		return 2;
	}

	public function getOperandType($i) {
		switch($i) {
			case 1: return $this->operandType;
			case 2: return "U32";
			case 3: return "U08";
		}
	}

	public function getOperandAddressMode($i) {
		return "ARR";
	}

	public function getHelperFunctions() {
		$type = $this->getOperandType(1);
		$cType = $this->getOperandCType(1);
		$sprintf = $this->getSprintf();
		$functions = array(
			array(
				"uint32_t qb_get_multidimensional_array_sprintf_length_$type(qb_interpreter_context *cxt, $cType *op1_ptr, uint32_t op1_count, uint32_t *op2_ptr, uint32_t op2_count) {",
					"$cType *op1_end = op1_ptr + op1_count;",
					"uint32_t *op2_end = op2_ptr + op2_count;",
					"uint32_t total = 0, multiplier = 1;",
					"while(op1_ptr < op1_end) {",
						"char sprintf_buffer[64];",
						"uint32_t len = sprintf(sprintf_buffer, \"%d\", *op1_ptr);",
						"total += len;",
						"op1_ptr++;",
					"}",
					"while(op2_ptr < op2_end) {",
						"total += multiplier * *op2_ptr * 2;",
						"multiplier *= *op2_ptr;",
						"op2_ptr++;",
					"}",
					"return total;",
				"}",
			),
		);
		return $functions;
	}
			
	
	public function getResultSizePossibilities() {
		return "res_count + string_length";
	}

	public function getResultSizeCalculation() {
		$type = $this->getOperandType(1);
		return "string_length = qb_get_multidimensional_array_sprintf_length_$type(cxt, op1_ptr, op1_end, op2_ptr, op2_end, NULL);";
	}
	
	public function getActionOnUnitData() {
		$sprintf = $this->getSprintf();
		$cType = $this->getOperandCType(1);
		$lines = array();
		$lines[] = "USE_TSRM";
		$lines[] = "$cType *op1_end = op1_ptr + op1_count;";
		$lines[] = "uint32_t depth = 0, pos = 0;";
		$lines[] = "uint32_t counts[64];";
		$lines[] = "memset(counts, 0, sizeof(uint32_t) * op2_count);";
		$lines[] = "res_ptr[pos++] = '[';";
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
		return $lines;
	}
}

?>