<?php

class QBArrayMaxHandler extends QBHandler {

	public function getOperandAddressMode($i) {
		if($i == 1) {
			return "ARR";
		} else {
			return $this->addressMode;
		}
	}

	public function getHelperFunctions() {
		$type = $this->getOperandType(1);
		$cType = $this->getOperandCType(1);
		$functions = array(
			array(
				"$cType ZEND_FASTCALL qb_calculate_array_max_$type(qb_interpreter_context *__restrict cxt, $cType *elements, uint32_t count) {",
					"if(count > 0) {",
						"uint32_t i;",
						"$cType biggest = elements[0];",
						"for(i = 1; i < count; i++) {",
							"if(elements[i] > biggest) {",
								"biggest = elements[i];",
							"}",
						"}",
						"return biggest;",
					"} else {",
						"return 0;",
					"}",
				"}",
			),
		);
		return $functions;
	}

	public function getAction() {
		$type = $this->getOperandType(1);
		return "res = qb_calculate_array_max_$type(cxt, op1_ptr, op1_count);";
	}
}

?>