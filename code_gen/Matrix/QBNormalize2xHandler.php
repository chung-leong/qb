<?php

class QBNormalize2xHandler extends QBHandler {

	public function getOperandAddressMode($i) {
		return "ARR";
	}

	public function getHelperFunctions() {
		$type = $this->getOperandType(1);
		$cType = $this->getOperandCType(1);
		$functions = array(
			array(
				"static void ZEND_FASTCALL qb_normalize_array_2x_$type(qb_interpreter_context *__restrict cxt, $cType *v, $cType *res_ptr) {",
					"$cType len = qb_calculate_array_length_2x_$type(cxt, v);",
					"res_ptr[0] = v[0] / len;",
					"res_ptr[1] = v[1] / len;",
				"}",
			),
		);
		return $functions;
	}
	
	public function getAction() {
		$type = $this->getOperandType(1);
		return "qb_normalize_array_2x_$type(cxt, op1_ptr, res_ptr);";
	}

}

?>