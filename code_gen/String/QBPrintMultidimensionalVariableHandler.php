<?php

class QBPrintMultidimensionalVariableHandler extends QBHandler {

	public function getInputOperandCount() {
		return 2;
	}

	public function getOutputOperandCount() {
		return 0;
	}

	public function getOperandType($i) {
		switch($i) {
			case 1: return $this->operandType;
			case 2: return "U32";
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
				"void ZEND_FASTCALL qb_print_multidimensional_array_$type(qb_interpreter_context *cxt, $cType *ptr, $cType *end, uint32_t *size, uint32_t *size_end) {",
					"USE_TSRM",
					"if(size == size_end) {",
						"qb_print_array_$type(cxt, ptr, end);",
					"} else {",
						"uint32_t count = *size;",
						"php_write(\"[\", 1 TSRMLS_CC);",
						"while(ptr < end) {",
							"qb_print_multidimensional_array_$type(cxt, ptr, ptr + count, size + 1, size_end);",
							"ptr += count;",
							"if(ptr != end) {",
								"php_write(\", \", 2 TSRMLS_CC);",
							"}",
						"}",
						"php_write(\"]\", 1 TSRMLS_CC);",
					"}",
				"}",
			),
		);
		return $functions;
	}
			
	public function getAction() {
		$type = $this->getOperandType(1);
		return "qb_print_multidimensional_array_$type(cxt, op1_ptr, op1_end, op2_ptr, op2_end);";
	}
}

?>