<?php

class QBArrayMinHandler extends QBHandler {

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
				"static $cType ZEND_FASTCALL qb_calculate_array_min_$type(qb_interpreter_context *__restrict cxt, $cType *elements, uint32_t count) {",
					"if(count > 0) {",
						"uint32_t i;",
						"$cType smallest = elements[0];",
						"for(i = 1; i < count; i++) {",
							"if(elements[i] < smallest) {",
								"smallest = elements[i];",
							"}",
						"}",
						"return smallest;",
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
		return "res = qb_calculate_array_min_$type(cxt, op1_ptr, op1_count);";
	}
}

?>