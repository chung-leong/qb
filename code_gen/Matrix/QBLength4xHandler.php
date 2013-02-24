<?php

class QBLength4xHandler extends QBHandler {

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
				"static $cType ZEND_FASTCALL qb_calculate_array_length_4x_$type(qb_interpreter_context *__restrict cxt, $cType *v) {",
					"$cType sum = (v[0] * v[0]) + (v[1] * v[1]) + (v[2] * v[2]) + (v[3] * v[3]);",
					"return $sqrt(sum);",
				"}",
			),
		);
		return $functions;
	}
	
	protected function getScalarExpression() {
		$type = $this->getOperandType(1);
		return "res = qb_calculate_array_length_4x_$type(cxt, op1_ptr);";
	}

}

?>