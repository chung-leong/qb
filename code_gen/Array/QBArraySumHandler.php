<?php

class QBArraySumHandler extends QBHandler {

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
				"$cType ZEND_FASTCALL qb_calculate_array_sum_$type(qb_interpreter_context *__restrict cxt, $cType *elements, uint32_t count) {",
					"uint32_t i;",
					"$cType sum = 0;",
					"for(i = 0; i < count; i++) {",
						"sum += elements[i];",
					"}",
					"return sum;",
				"}",
			),
		);
		return $functions;
	}

	public function getAction() {
		$type = $this->getOperandType(1);
		return "res = qb_calculate_array_sum_$type(cxt, op1_ptr, op1_count);";
	}
}

?>