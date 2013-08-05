<?php

class QBUTF8DecodeHandler extends QBHandler {

	public function getOperandType($i) {
		// type can be U16 for UCS-16 and U32 for UCS-32
		if($i == 1) {
			return "U08";
		} else if($i == 2) {
			return $this->operandType;
		}
	}

	public function getOperandAddressMode($i) {
		return "ARR";
	}

	
	public function getHelperFunctions() {
		$functions = array(
			array(
				"uint32_t ZEND_FASTCALL qb_get_utf8_codepoint_count(uint8_t *op1_ptr, uint32_t op1_count) {",
					"uint32_t codepoint, state = 0, count = 0, i;",
					"for(i = 0; i < op1_count; i++) {",
						"if(!decode(&state, &codepoint, op1_ptr[i])) {",
							"count++;",
						"}",
					"}",
					"return count;",
				"}",
			),
		);
		return $functions;
	}	
	
	public function getResultSizePossibilities() {
		return "string_length";
	}

	public function getResultSizeCalculation() {
		return "string_length = qb_get_utf8_codepoint_count(op1_ptr, op1_count);";
	}
	
	public function getActionOnUnitData() {
		$lines = array();
		$lines[] = "uint32_t codepoint, state = 0, i, j;";
		$lines[] = "for(i = 0, j = 0; i < op1_count; i++) {";
		$lines[] = 		"if(!decode(&state, &codepoint, op1_ptr[i])) {";
		$lines[] = 			"res_ptr[j++] = codepoint;";
		$lines[] = 		"}";
		$lines[] = "}";
		return $lines;
	}
}

?>