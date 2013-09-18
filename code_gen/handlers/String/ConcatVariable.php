<?php

class ConcatVariable extends Handler {

	use MultipleAddressMode, ArrayResult, StringResult, UseSprintf;

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
		$lines[] = "memcpy(res_ptr + res_count_before, sprintf_buffer, len);";
		return $lines;	
	}
	
	public function getActionOnMultipleData() {
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
	}
}

?>