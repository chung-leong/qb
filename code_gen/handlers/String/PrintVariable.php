<?php

class PrintVariable extends Handler {

	use MultipleAddressMode, UnaryOperator;
	
	public function getActionOnUnitData() {
		$sprintf = $this->getSprintf("op1");
		$lines = array();
		$lines[] = "USE_TSRM";
		$lines[] = "char sprintf_buffer[64];";
		$lines[] = "uint32_t len = $sprintf;";
		$lines[] = "php_write(sprintf_buffer, len TSRMLS_CC);";
		return $lines;
	}

	public function getActionOnMultipleData() {
		$sprintf = $this->getSprintf("*op1_ptr");
		$cType = $this->getOperandCType(1);
		$lines = array();
		$lines[] = "USE_TSRM";
		$lines[] = "$cType *op1_end = op1_ptr + op1_count;";
		$lines[] = "php_write(\"[\", 1 TSRMLS_CC);";
		$lines[] = "while(op1_ptr < op1_end) {";
		$lines[] =		"char sprintf_buffer[64];";
		$lines[] =		"uint32_t len = $sprintf;";
		$lines[] =		"php_write(sprintf_buffer, len TSRMLS_CC);";
		$lines[] = 		"op1_ptr++;";
		$lines[] = 		"if(op1_ptr != op1_end) {";
		$lines[] = 			"php_write(\", \", 2 TSRMLS_CC);";
		$lines[] = 		"}";
		$lines[] = "}";
		$lines[] = "php_write(\"]\", 1 TSRMLS_CC);";
		return $lines;
	}
}

?>