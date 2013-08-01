<?php

class QBPrintVariableHandler extends QBPrintHandler {

	public function getActionForUnitData() {
		$sprintf = $this->getSprintf();
		$lines = array();
		$lines[] = "USE_TSRM",
		$lines[] = "char sprintf_buffer[64];",
		$lines[] = "uint32_t len = $sprintf;",
		$lines[] = "php_write(sprintf_buffer, len TSRMLS_CC);",
		return $lines;
	}

	public function getActionForMultipleData() {
		$sprintf = $this->getSprintf();
		$lines = array();
		$lines[] = "USE_TSRM";
		$lines[] = "php_write(\"[\", 1 TSRMLS_CC);",
		$lines[] = "while(ptr != end) {",
		$lines[] =		"char sprintf_buffer[64];",
		$lines[] =		"uint32_t len = $sprintf;",
		$lines[] =		"php_write(sprintf_buffer, len TSRMLS_CC);",
		$lines[] = 		"ptr++;",
		$lines[] = 		"if(ptr != end) {",
		$lines[] = 			"php_write(\", \", 2 TSRMLS_CC);",
		$lines[] = 		"}",
		$lines[] = "}",
		$lines[] = "php_write(\"]\", 1 TSRMLS_CC);",
		return $lines;
	}
}

?>