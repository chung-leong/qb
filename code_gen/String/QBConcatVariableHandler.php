<?php

class QBConcatVariableHandler extends QBHandler {

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

		if($type == "F32") {		
			$format = '"%.*G"';
			$sprintf = "snprintf(sprintf_buffer, sizeof(sprintf_buffer), $format, cxt->floating_point_precision / 2, *ptr)";
		} else if($type == "F64") {
			$format = '"%.*G"';
			$sprintf = "snprintf(sprintf_buffer, sizeof(sprintf_buffer), $format, cxt->floating_point_precision, *ptr)";
		} else {
			// use macros in inttypes.h
			$size = intval(substr($type, 1));
			$sign = ($type[0] == 'U') ? 'u' : 'd';
			$format = '"%" PRI' . $sign . $size;
			$sprintf = "snprintf(sprintf_buffer, sizeof(sprintf_buffer), $format, *ptr)";
		}
		
		$functions = array(
			array(
				"static uint32_t ZEND_FASTCALL qb_convert_scalar_to_string_$type(qb_interpreter_context *cxt, $cType *ptr, uint8_t *res_ptr) {",
					"char sprintf_buffer[64];",
					"uint32_t len = $sprintf;",
					"if(res_ptr) {",
						"memcpy(res_ptr, sprintf_buffer, len);",
					"}",
					"return len;",
				"}",
			),
			array(
				"static uint32_t ZEND_FASTCALL qb_convert_array_to_string_$type(qb_interpreter_context *cxt, $cType *ptr, $cType *end, uint8_t *res_ptr) {",
					"char sprintf_buffer[64];",
					"uint32_t len, total = 2;",
					"if(res_ptr) {",
						"*res_ptr = '[';",
						"res_ptr++;",
					"}",
					"while(ptr != end) {",
						"len = $sprintf;",
						"total += len;",
						"ptr++;",
						"if(res_ptr) {",
							"memcpy(res_ptr, sprintf_buffer, len);",
							"res_ptr += len;",
						"}",
						"if(ptr != end) {",
							"total += 2;",
							"if(res_ptr) {",
								"*res_ptr = ',';",
								"res_ptr++;",
								"*res_ptr = ' ';",
								"res_ptr++;",
							"}",
						"}",
					"}",
					"if(res_ptr) {",
						"*res_ptr = ']';",
					"}",
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
			return "string_length = qb_convert_array_to_string_$type(cxt, op1_ptr, op1_end, NULL);";
		} else {
			return "string_length = qb_convert_scalar_to_string_$type(cxt, op1_ptr, NULL);";
		}		 
	}
	
	public function getAction() {
		$type = $this->getOperandType(1);
		if($this->addressMode == "ARR") {
			return "qb_convert_array_to_string_$type(cxt, op1_ptr, op1_end, res_ptr + res_count_before);";
		} else {
			return "qb_convert_scalar_to_string_$type(cxt, op1_ptr, res_ptr + res_count_before);";
		}		 
	}
}

?>