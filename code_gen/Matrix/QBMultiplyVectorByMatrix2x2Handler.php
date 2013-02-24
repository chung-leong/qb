<?php

class QBMultiplyVectorByMatrix2x2Handler extends QBHandler {

	public function getOperandAddressMode($i) {
		return "ARR";
	}

	public function getHelperFunctions() {
		$type = $this->getOperandType(1);
		$cType = $this->getOperandCType(1);
		$functions = array(
			array(
				"static void ZEND_FASTCALL qb_multiply_vector_by_matrix_2x2_$type(qb_interpreter_context *cxt, $cType *v, $cType *m, $cType *res_ptr) {",
					"$cType dot_product0 = (v[0] * m[0 + 0]) + (v[1] * m[1 + 0]);",
					"$cType dot_product1 = (v[0] * m[0 + 2]) + (v[1] * m[1 + 2]);",
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
		return "qb_multiply_vector_by_matrix_2x2_$type(cxt, op1_ptr, op2_ptr, res_ptr);";
	}
}

?>