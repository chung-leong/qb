<?php

class QBNormalizeHandler extends QBHandler {

	public function getOperandAddressMode($i) {
		return "ARR";
	}

	public function getHelperFunctions() {
		$type = $this->getOperandType(1);
		$cType = $this->getOperandCType(1);
		$functions = array(
			array(
				"static void ZEND_FASTCALL qb_normalize_array_$type(qb_interpreter_context *__restrict cxt, $cType *v, uint32_t count, $cType *res_ptr) {",
					"uint32_t i;",
					"$cType len = qb_calculate_array_length_$type(cxt, v, count);",
					"for(i = 0; i < count; i++) {",
						"res_ptr[i] = v[i] / len;",
					"}",
				"}",
			),
		);
		return $functions;
	}
	
	public function getAction() {
		$type = $this->getOperandType(1);
		return "qb_normalize_array_$type(cxt, op1_ptr, op1_count, res_ptr);";
	}

}

?>