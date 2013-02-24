<?php

class QBMultiplyMatrixByMatrix4x4Handler extends QBHandler {

	public function getOperandAddressMode($i) {
		return "ARR";
	}

	public function getHelperFunctions() {
		$type = $this->getOperandType(1);
		$cType = $this->getOperandCType(1);
		$functions = array(
			array(
				"static void ZEND_FASTCALL qb_multiply_matrix_by_matrix_4x4_$type(qb_interpreter_context *cxt, $cType *m1, $cType *m2, $cType *res_ptr) {",
					"uint32_t q;",
					"for(q = 0; q < 12; q += 4) {",
						"$cType dot_product0 = (m1[0 + q] * m2[0 + 0]) + (m1[1 + q] * m2[4 + 0]) + (m1[2 + q] * m2[8 + 0]) + (m1[3 + q] * m2[12 + 0]);",
						"$cType dot_product1 = (m1[0 + q] * m2[0 + 1]) + (m1[1 + q] * m2[4 + 1]) + (m1[2 + q] * m2[8 + 1]) + (m1[3 + q] * m2[12 + 1]);",
						"$cType dot_product2 = (m1[0 + q] * m2[0 + 2]) + (m1[1 + q] * m2[4 + 2]) + (m1[2 + q] * m2[8 + 2]) + (m1[3 + q] * m2[12 + 2]);",
						"$cType dot_product3 = (m1[0 + q] * m2[0 + 3]) + (m1[1 + q] * m2[4 + 3]) + (m1[2 + q] * m2[8 + 3]) + (m1[3 + q] * m2[12 + 3]);",
						"res_ptr[q + 0] = dot_product0;",
						"res_ptr[q + 1] = dot_product1;",
						"res_ptr[q + 2] = dot_product2;",
						"res_ptr[q + 3] = dot_product3;",
					"}",
				"}",
			),
		);
		return $functions;
	}
	
	public function getResultSizePossibilities() {
		return 16;
	}
	
	public function getAction() {
		$type = $this->getOperandType(1);
		return "qb_multiply_matrix_by_matrix_4x4_$type(cxt, op1_ptr, op2_ptr, res_ptr);";
	}
}

?>