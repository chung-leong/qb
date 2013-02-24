<?php

class QBMultiplyMatrixByVector3x3Handler extends QBHandler {

	public function getOperandAddressMode($i) {
		return "ARR";
	}

	public function getHelperFunctions() {
		$type = $this->getOperandType(1);
		$cType = $this->getOperandCType(1);
		$functions = array(
			array(
				"static void ZEND_FASTCALL qb_multiply_matrix_by_vector_3x3_$type(qb_interpreter_context *cxt, $cType *m, $cType *v, $cType *res_ptr) {",
					"$cType dot_product0 = (m[0 + 0] * v[0]) + (m[3 + 0] * v[1]) + (m[6 + 0] * v[2]);",
					"$cType dot_product1 = (m[0 + 1] * v[0]) + (m[3 + 1] * v[1]) + (m[6 + 1] * v[2]);",
					"$cType dot_product2 = (m[0 + 2] * v[0]) + (m[3 + 2] * v[1]) + (m[6 + 2] * v[2]);",
					"res_ptr[0] = dot_product0;",
					"res_ptr[1] = dot_product1;",
					"res_ptr[2] = dot_product2;",
				"}",
			),
		);
		return $functions;
	}

	public function getResultSizePossibilities() {
		return 3;
	}
	
	public function getAction() {
		$type = $this->getOperandType(1);
		return "qb_multiply_matrix_by_vector_3x3_$type(cxt, op1_ptr, op2_ptr, res_ptr);";
	}
}

?>