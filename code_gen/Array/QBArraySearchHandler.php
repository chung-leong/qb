<?php

class QBArraySearchHandler extends QBHandler {

	public function getInputOperandCount() {
		return 2;
	}

	public function getOperandAddressMode($i) {
		switch($i) {
			case 1: return "ARR";
			case 3: return "VAR";
			default: return $this->addressMode;
		}
	}
	
	public function getOperandType($i) {
		switch($i) {
			case 3: return "I32";
			default: return $this->operandType;
		}
	}

	public function getHelperFunctions() {
		$type = $this->getOperandType(1);
		$cType = $this->getOperandCType(1);
		$functions = array(
			array(
				"int32_t ZEND_FASTCALL qb_find_element_$type($cType *elements, uint32_t count, $cType needle) {",
					"uint32_t i;",
					"for(i = 0; i < count; i++) {",
						"if(elements[i] == needle) {",
							"return i;",
						"}",
					"}",
					"return -1;",
				"}",
			),
			array(
				"int32_t ZEND_FASTCALL qb_find_elements_$type($cType *elements, uint32_t count, $cType *needle, uint32_t needle_width) {",
					"uint32_t i, j, k;",
					"for(i = 0, j = 0; j < count; i++, j += needle_width) {",
						"if(elements[j] == needle[0]) {",
							"for(k = 1; k < needle_width; k++) {",
								"if(elements[j + k] != needle[k]) {",
									"break;",
								"}",
							"}",
							"if(k == needle_width) {",
								"return i;",
							"}",
						"}",
					"}",
					"return -1;",
				"}",
			),
		);
		return $functions;
	}
	
	protected function getArrayExpression() {
		$type = $this->getOperandType(1);
		return "res = qb_find_elements_$type(op1_ptr, op1_count, op2_ptr, op2_count);";
	}
	
	protected function getScalarExpression() {
		$type = $this->getOperandType(1);
		return "res = qb_find_element_$type(op1_ptr, op1_count, op2);";
	}
}

?>