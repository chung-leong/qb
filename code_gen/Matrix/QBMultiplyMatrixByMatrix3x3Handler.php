<?php

class QBMultiplyMatrixByMatrix3x3Handler extends QBHandler {

	public function getOperandAddressMode($i) {
		return "ARR";
	}

	public function getHelperFunctions() {
		$type = $this->getOperandType(1);
		$cType = $this->getOperandCType(1);
		$functions = array(
			array(
				"static void ZEND_FASTCALL qb_multiply_matrix_by_matrix_3x3_$type(qb_interpreter_context *cxt, $cType *m1, $cType *m2, $cType *res_ptr) {",
					"uint32_t q;",
					"for(q = 0; q < 9; q += 3) {",
						"$cType dot_product0 = (m1[0 + q] * m2[0 + 0]) + (m1[1 + q] * m2[3 + 0]) + (m1[2 + q] * m2[6 + 0]);",
						"$cType dot_product1 = (m1[0 + q] * m2[0 + 1]) + (m1[1 + q] * m2[3 + 1]) + (m1[2 + q] * m2[6 + 1]);",
						"$cType dot_product2 = (m1[0 + q] * m2[0 + 2]) + (m1[1 + q] * m2[3 + 2]) + (m1[2 + q] * m2[6 + 2]);",
						"res_ptr[q + 0] = dot_product0;",
						"res_ptr[q + 1] = dot_product1;",
						"res_ptr[q + 2] = dot_product2;",
					"}",
				"}",
			),
		);
		return $functions;
	}
	
	public function getResultSizePossibilities() {
		return 9;
	}
	
	public function getAction() {
		$type = $this->getOperandType(1);
		return "qb_multiply_matrix_by_matrix_3x3_$type(cxt, op1_ptr, op2_ptr, res_ptr);";
	}
}

?>