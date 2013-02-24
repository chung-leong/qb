<?php

class QBNormalize4xHandler extends QBHandler {

	public function getOperandAddressMode($i) {
		return "ARR";
	}

	public function getHelperFunctions() {
		$type = $this->getOperandType(1);
		$cType = $this->getOperandCType(1);
		$functions = array(
			array(
				"static void ZEND_FASTCALL qb_normalize_array_4x_$type(qb_interpreter_context *__restrict cxt, $cType *v, $cType *res_ptr) {",
					"$cType len = qb_calculate_array_length_4x_$type(cxt, v);",
					"res_ptr[0] = v[0] / len;",
					"res_ptr[1] = v[1] / len;",
					"res_ptr[2] = v[2] / len;",
					"res_ptr[3] = v[3] / len;",
				"}",
			),
		);
		return $functions;
	}
	
	public function getAction() {
		$type = $this->getOperandType(1);
		return "qb_normalize_array_4x_$type(cxt, op1_ptr, res_ptr);";
	}

}

?>