<?php

class QBConcatVariableHandler extends QBPrintHandler {

	public function getOperandType($i) {
		return ($i == 2) ? "U08" : $this->operandType;
	}

	public function getOperandAddressMode($i) {
		// the result is an array
		return ($i == 2) ? "ARR" : $this->addressMode;
	}
	
	public function getHelperFunctions() {
		$type = $this->getOperandType(1);
		$cType = $this->getOperandCType(1);
		$sprintf = $this->getSprintf();
		$functions = array(
			array(
				"uint32_t ZEND_FASTCALL qb_get_scalar_sprintf_length_$type(qb_interpreter_context *cxt, $cType *op1_ptr) {",
					"char sprintf_buffer[64];",
					"uint32_t len = $sprintf;",
					"return len;",
				"}",
			),
			array(
				"uint32_t ZEND_FASTCALL qb_get_array_sprintf_length_$type(qb_interpreter_context *cxt, $cType *op1_ptr, uint32_t op1_count) {",
					"uint32_t total = 0;",
					"$cType *op1_end = op1_ptr + op1_count;",
					"while(op1_ptr < op1_end) {",
						"char sprintf_buffer[64];",
						"uint32_t len = $sprintf;",
						"total += len;",
						"op1_ptr++;",
					"}",
					"total += op1_count * 2;",
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
		if($this->addressMode == "ARR") {
			return "string_length = qb_get_array_sprintf_length_$type(cxt, op1_ptr, op1_count);";
		} else {
			return "string_length = qb_get_scalar_sprintf_length_$type(cxt, op1);";
		}		 
	}
	
	public function getActionOnUnitData() {
		$sprintf = $this->getSprintf();
		$lines = array();
		$lines[] = "char sprintf_buffer[64];";
		$lines[] = "uint32_t len = $sprintf;";
		$lines[] = "memcpy(res_ptr, sprintf_buffer, len);";
		return $lines;	
	}
	
	public function getActionOnMultipleData() {
		$sprintf = $this->getSprintf();
		$cType = $this->getOperandCType(1);
		$lines = array();
		$lines[] = "uint32_t pos = 0;";
		$lines[] = "$cType *op1_end = op1_ptr + op1_count;";
		$lines[] = "res_ptr[pos++] = '[';";
		$lines[] = "while(op1_ptr < op1_end) {";
		$lines[] = 		"char sprintf_buffer[64];";
		$lines[] = 		"uint32_t len = $sprintf;";
		$lines[] = 		"memcpy(res_ptr + pos, sprintf_buffer, len);";
		$lines[] = 		"pos += len;";
		$lines[] = 		"if(op1_ptr != op1_end) {";
		$lines[] = 			"res_ptr[pos++] = ',';";
		$lines[] = 			"res_ptr[pos++] = ' ';";
		$lines[] = 		"}";
		$lines[] = 		"op1_ptr++;";
		$lines[] = "}";
		$lines[] = "res_ptr[pos++] = ']';";
		return $lines;
	}
}

?>