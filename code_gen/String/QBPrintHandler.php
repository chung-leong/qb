<?php

class QBPrintHandler extends QBHandler {

	protected function getArrayExpression() {
		$expression = $this->getScalarExpression();
		$lines = array();
		$lines[] = "php_write(\"[\", 1  TSRMLS_CC);";
		$lines[] = "while(op1_ptr != op1_end) {";
		$lines[] = 		$expression;
		$lines[] =		"op1_ptr++;";
		$lines[] = 		"if(op1_ptr != op1_end) {";
		$lines[] = 			"php_write(\", \", 2  TSRMLS_CC);";
		$lines[] = 		"}";
		$lines[] = "}";
		$lines[] = "php_write(\"]\", 1  TSRMLS_CC);";
		return $lines;
	}

	protected function getScalarExpression() {
		// use explicit cast to ensure any literal values are correctly pushed onto call stack
		$type = $this->getOperandType(1);
		$cType = $this->getOperandCType(1);
		if($type == "F32") {		
			$format = '"%.*G"';
			$sprintf = "snprintf(sprintf_buffer, sizeof(sprintf_buffer), $format, cxt->floating_point_precision / 2, ($cType) op1)";
		} else if($type == "F64") {
			$format = '"%.*G"';
			$sprintf = "snprintf(sprintf_buffer, sizeof(sprintf_buffer), $format, cxt->floating_point_precision, ($cType) op1)";
		} else {
			// use macros in inttypes.h
			$size = intval(substr($type, 1));
			$sign = ($type[0] == 'U') ? 'u' : 'd';
			$format = '"%" PRI' . $sign . $size;
			$sprintf = "snprintf(sprintf_buffer, sizeof(sprintf_buffer), $format, ($cType) op1)";
		}
		$lines = array();
		$lines[] = "string_length = $sprintf;";
		$lines[] = "php_write(sprintf_buffer, string_length TSRMLS_CC);";
		return $lines;
	}
}

?>