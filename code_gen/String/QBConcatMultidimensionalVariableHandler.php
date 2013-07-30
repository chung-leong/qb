<?php

class QBConcatMultidimensionalVariableHandler extends QBHandler {

	public function getInputOperandCount() {
		return 2;
	}

	public function getOperandType($i) {
		switch($i) {
			case 1: return $this->operandType;
			case 2: return "U32";
			case 3: return "U08";
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
				"static uint32_t ZEND_FASTCALL qb_convert_multidimensional_array_to_string_$type(qb_interpreter_context *cxt, $cType *ptr, $cType *end, uint32_t *size, uint32_t *size_end, uint8_t *res_ptr) {",
					"if(size == size_end) {",
						"return qb_convert_array_to_string_$type(cxt, ptr, end, res_ptr);",
					"} else {",
						"uint32_t count = *size, len, total;",
						"total = 2;",
						"if(res_ptr) {",
							"*res_ptr = '[';",
							"res_ptr++;",
						"}",
						"while(ptr < end) {",
							"len = qb_convert_multidimensional_array_to_string_$type(cxt, ptr, ptr + count, size + 1, size_end, res_ptr);",
							"if(res_ptr) {",
								"res_ptr += len;",
							"}",
							"total += len;",
							"ptr += count;",
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
		return "string_length = qb_convert_multidimensional_array_to_string_$type(cxt, op1_ptr, op1_end, op2_ptr, op2_end, NULL);";
	}
	
	public function getAction() {
		$type = $this->getOperandType(1);
		return "qb_convert_multidimensional_array_to_string_$type(cxt, op1_ptr, op1_end, op2_ptr, op2_end, res_ptr + res_count_before);";
	}
}

?>