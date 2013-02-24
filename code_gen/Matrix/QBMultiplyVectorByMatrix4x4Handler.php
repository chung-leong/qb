<?php

class QBMultiplyVectorByMatrix4x4Handler extends QBHandler {

	public function getOperandAddressMode($i) {
		return "ARR";
	}

	public function getHelperFunctions() {
		$type = $this->getOperandType(1);
		$cType = $this->getOperandCType(1);
		$functions = array(
			array(
				"static void ZEND_FASTCALL qb_multiply_vector_by_matrix_4x4_$type(qb_interpreter_context *cxt, $cType *v, $cType *m, $cType *res_ptr) {",
					"$cType dot_product0 = (v[0] * m[0 + 0]) + (v[1] * m[1 + 0]) + (v[2] * m[2 + 0]) + (v[3] * m[3 + 0]);",
					"$cType dot_product1 = (v[0] * m[0 + 4]) + (v[1] * m[1 + 4]) + (v[2] * m[2 + 4]) + (v[3] * m[3 + 4]);",
					"$cType dot_product2 = (v[0] * m[0 + 8]) + (v[1] * m[1 + 8]) + (v[2] * m[2 + 8]) + (v[3] * m[3 + 8]);",
					"$cType dot_product3 = (v[0] * m[0 + 12]) + (v[1] * m[1 + 12]) + (v[2] * m[2 + 12]) + (v[3] * m[3 + 12]);",
					"res_ptr[0] = dot_product0;",
					"res_ptr[1] = dot_product1;",
					"res_ptr[2] = dot_product2;",
					"res_ptr[3] = dot_product3;",
				"}",
			),
		);
		return $functions;
	}

	public function getResultSizePossibilities() {
		return 4;
	}
	
	public function getAction() {
		$type = $this->getOperandType(1);
		return "qb_multiply_vector_by_matrix_4x4_$type(cxt, op1_ptr, op2_ptr, res_ptr);";
	}
}

?>