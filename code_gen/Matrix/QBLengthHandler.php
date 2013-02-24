<?php

class QBLengthHandler extends QBHandler {

	public function getOperandAddressMode($i) {
		switch($i) {
			case 1: return "ARR";
			case 2: return $this->addressMode;
		}
	}

	public function getHelperFunctions() {
		$type = $this->getOperandType(1);
		$cType = $this->getOperandCType(1);
		$sqrt = ($type == "F64") ? 'sqrt' : 'sqrtf';
		$functions = array(
			array(
				"static $cType ZEND_FASTCALL qb_calculate_array_length_$type(qb_interpreter_context *__restrict cxt, $cType *v, uint32_t count) {",
					"uint32_t i;",
					"$cType sum = 0;",
					"for(i = 0; i < count; i++) {",
						"sum += v[i] * v[i];",
					"}",
					"return $sqrt(sum);",
				"}",
			),
		);
		return $functions;
	}
	
	protected function getScalarExpression() {
		$type = $this->getOperandType(1);
		return "res = qb_calculate_array_length_$type(cxt, op1_ptr, op1_count);";
	}

}

?>