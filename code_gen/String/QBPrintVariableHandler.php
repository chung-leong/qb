<?php

class QBPrintVariableHandler extends QBHandler {

	public function getHelperFunctions() {
		$type = $this->getOperandType(1);
		$cType = $this->getOperandCType(1);

		if($type == "F32") {		
			$format = '"%.*G"';
			$sprintf = "snprintf(sprintf_buffer, sizeof(sprintf_buffer), $format, cxt->floating_point_precision / 2, v)";
		} else if($type == "F64") {
			$format = '"%.*G"';
			$sprintf = "snprintf(sprintf_buffer, sizeof(sprintf_buffer), $format, cxt->floating_point_precision, v)";
		} else {
			// use macros in inttypes.h
			$size = intval(substr($type, 1));
			$sign = ($type[0] == 'U') ? 'u' : 'd';
			$format = '"%" PRI' . $sign . $size;
			$sprintf = "snprintf(sprintf_buffer, sizeof(sprintf_buffer), $format, v)";
		}
		
		$functions = array(
			array(
				"static void ZEND_FASTCALL qb_print_scalar_$type(qb_interpreter_context *cxt, $cType v) {",
					"USE_TSRM",
					"char sprintf_buffer[64];",
					"uint32_t len = $sprintf;",
					"php_write(sprintf_buffer, len TSRMLS_CC);",
				"}",
			),
			array(
				"static void ZEND_FASTCALL qb_print_array_$type(qb_interpreter_context *cxt, $cType *ptr, $cType *end) {",
					"USE_TSRM",
					"php_write(\"[\", 1 TSRMLS_CC);",
					"while(ptr != end) {",
						"qb_print_scalar_$type(cxt, *ptr);",
						"ptr++;",
						"if(ptr != end) {",
							"php_write(\", \", 2 TSRMLS_CC);",
						"}",
					"}",
					"php_write(\"]\", 1 TSRMLS_CC);",
				"}",
			),
		);
		return $functions;
	}	

	public function getAction() {
		$type = $this->getOperandType(1);
		if($this->addressMode == "ARR") {
			return "qb_print_array_$type(cxt, op1_ptr, op1_end);";
		} else {
			return "qb_print_scalar_$type(cxt, op1);";
		}		 
	}
}

?>