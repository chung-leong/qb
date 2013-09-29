<?php

class ConcatVariable extends Handler {

	use MultipleAddressMode, ArrayResult, BinaryOperator, UseSprintf, ResizeResult {
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
			case 1: return "ARR";
			case 2: return "CON";
			case 3: return "ARR";
		}
	}
	
	public function getHelperFunctions() {
		$type = $this->getOperandType(1);
		$cType = $this->getOperandCType(1);
		$sprintf_scalar = $this->getSprintf("op1");
		$sprintf_array = $this->getSprintf("*op1_ptr");
		$functions = array(
			array(
				"uint32_t ZEND_FASTCALL qb_get_scalar_sprintf_length_$type(qb_interpreter_context *cxt, $cType op1) {",
					"char sprintf_buffer[64];",
					"uint32_t len = $sprintf_scalar;",
					"return len;",
				"}",
			),
			array(
				"uint32_t ZEND_FASTCALL qb_get_array_sprintf_length_$type(qb_interpreter_context *cxt, $cType *op1_ptr, uint32_t op1_count) {",
					"uint32_t total = 0;",
					"if(op1_count) {",
						"$cType *op1_end = op1_ptr + op1_count;",
						"while(op1_ptr < op1_end) {",
							"char sprintf_buffer[64];",
							"uint32_t len = $sprintf_array;",
							"total += len;",
							"op1_ptr++;",
						"}",
						"total += op1_count * 2;",
					"} else {",
						"total = 2;",
					"}",
					"return total;",
				"}",
			),
		);
		return $functions;
	}	

	public function getActionOnUnitData() {
		$sprintf = $this->getSprintf("op1");
		$lines = array();
		$lines[] = "char sprintf_buffer[64];";
		$lines[] = "uint32_t len = $sprintf;";
		$lines[] = "res_ptr += qb_resize_array(cxt, local_storage, op2, *res_count_ptr + len);";
		$lines[] = "memcpy(res_ptr + *res_count_ptr, sprintf_buffer, len);";
		$lines[] = "*res_count_ptr += len;";
		return $lines;	
	}
	
	public function getActionOnMultipleData() {
	/*
		$sprintf = $this->getSprintf("*op1_ptr");
		$cType = $this->getOperandCType(1);
		$lines = array();
		$lines[] = "uint32_t pos = res_count_before;";
		$lines[] = "$cType *op1_end = op1_ptr + op1_count;";
		$lines[] = "res_ptr[pos++] = '[';";
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
		return $lines;
	*/
		return "// do nothing for now";
	}
}

?>