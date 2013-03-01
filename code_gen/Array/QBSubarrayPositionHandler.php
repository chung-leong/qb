<?php

class QBSubarrayPositionHandler extends QBHandler {

	public function getOperandAddressMode($i) {
		switch($i) {
			case 1: 
			case 2: return "ARR";
			case 3:
			case 4: return "VAR";
		}
	}
	
	public function getOperandType($i) {
		switch($i) {
			case 3:
			case 4: return "I32";
			default: return $this->operandType;
		}
	}


	public function getHelperFunctions() {
		$type = $this->getOperandType(1);
		$cType = $this->getOperandCType(1);
		$functions = array(
			array(
				"static int32_t ZEND_FASTCALL qb_find_subarray_$type($cType *elements, uint32_t count, $cType *needle, uint32_t needle_width, uint32_t start_index) {",
					"if(needle_width > 0) {",
						"uint32_t i, j;",
						"for(i = start_index; i < count - needle_width; i++) {",
							"if(elements[i] == needle[0]) {",
								"for(j = 1; j < needle_width; j++) {",
									"if(elements[i + j] != needle[j]) {",
										"break;",
									"}",
								"}",
								"if(j == needle_width) {",
									"return i;",
								"}",
							"}",
						"}",
					"}",
					"return -1;",
				"}",
			),
		);
		return $functions;
	}
	
	public function getAction() {
		$type = $this->getOperandType(1);
		return "res = qb_find_subarray_$type(op1_ptr, op1_count, op2_ptr, op2_count, (uint32_t) op3);";
	}
}

?>