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
		$folder = dirname(__FILE__);
		$functions = array(
			file("$folder/bh_utf8.c", FILE_IGNORE_NEW_LINES),
			array(
				"static void ZEND_FASTCALL qb_encode_utf8_$type(qb_interpreter_context *cxt, $cType *codepoints, uint32_t count, uint8_t *buffer) {",
					"uint32_t length = 0, i;",
					"for(i = 0; i < count; i++) {",
						"length += encode(codepoints[i], buffer + length);",
					"}",
				"}",
			),
			array(
				"static uint32_t ZEND_FASTCALL qb_get_utf8_encoded_length_$type(qb_interpreter_context *cxt, $cType *codepoints, uint32_t count) {",
					"uint32_t length = 0, i;",
					"uint8_t buffer[4];",
					"for(i = 0; i < count; i++) {",
						"length += encode(codepoints[i], buffer);",
					"}",
					"return length;",
				"}",
			),
		);
		return $functions;
	}

	public function getResultSizePossibilities() {
		return "string_length";
	}

	public function getResultSizeCalculation() {
		$type = $this->getOperandType(1);
		return "string_length = qb_get_utf8_encoded_length_$type(cxt, op1_ptr, op1_count);";
	}
	
	public function getAction() {
		$type = $this->getOperandType(1);
		return "qb_encode_utf8_$type(cxt, op1_ptr, op1_count, res_ptr);";
	}
}

?>