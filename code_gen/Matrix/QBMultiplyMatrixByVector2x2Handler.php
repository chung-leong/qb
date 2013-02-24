<?php

class QBMultiplyMatrixByVector2x2Handler extends QBHandler {

	public function getOperandAddressMode($i) {
		return "ARR";
	}

	public function getHelperFunctions() {
		$type = $this->getOperandType(1);
		$cType = $this->getOperandCType(1);
		$functions = array(
			array(
				"static void ZEND_FASTCALL qb_multiply_matrix_by_vector_2x2_$type(qb_interpreter_context *cxt, $cType *m, $cType *v, $cType *res_ptr) {",
					"$cType dot_product0 = (m[0 + 0] * v[0]) + (m[2 + 0] * v[1]);",
					"$cType dot_product1 = (m[0 + 1] * v[0]) + (m[2 + 1] * v[1]);",
					"res_ptr[0] = dot_product0;",
					"res_ptr[1] = dot_product1;",
				"}",
			),
		);
		return $functions;
	}

	public function getResultSizePossibilities() {
		return 2;
	}
	
	public function getAction() {
		$type = $this->getOperandType(1);
		return "qb_multiply_matrix_by_vector_2x2_$type(cxt, op1_ptr, op2_ptr, res_ptr);";
	}
}

?>