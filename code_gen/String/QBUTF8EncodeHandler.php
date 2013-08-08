<?php

class QBUTF8EncodeHandler extends QBHandler {

	public function getOperandType($i) {
		if($i == 1) {
			return $this->operandType;
		} else if($i == 2) {
			return "U08";
		}
	}

	public function getOperandAddressMode($i) {
		return "ARR";
	}

	public function getHelperFunctions() {
		$type = $this->getOperandType(1);
		$cType = $this->getOperandCType(1);
		$functions = array(
			array(
				"uint32_t ZEND_FASTCALL qb_get_utf8_encoded_length_$type($cType *op1_ptr, uint32_t op1_count) {",
					"uint32_t length = 0, i;",
					"uint8_t buffer[4];",
					"for(i = 0; i < op1_count; i++) {",
						"length += encode(op1_ptr[i], buffer);",
					"}",
					"return length;",
				"}",
			),
		);
		return $functions;
	}

	public function getResultSizePossibilities() {
		return "expr_count";
	}

	public function getResultSizeCalculation() {
		$type = $this->getOperandType(1);
		return "expr_count = qb_get_utf8_encoded_length_$type(op1_ptr, op1_count);";
	}
	
	public function getActionOnUnitData() {
		$lines = array();
		$lines[] = "uint32_t length = 0, i;";
		$lines[] = "for(i = 0; i < op1_count; i++) {";
		$lines[] = 		"length += encode(op1_ptr[i], res_ptr + length);";
		$lines[] = "}";
		return $lines;
	}
}

?>