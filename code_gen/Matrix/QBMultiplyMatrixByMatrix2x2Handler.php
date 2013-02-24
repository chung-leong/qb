<?php

class QBMultiplyMatrixByMatrix2x2Handler extends QBHandler {

	public function getOperandAddressMode($i) {
		return "ARR";
	}

	public function getHelperFunctions() {
		$type = $this->getOperandType(1);
		$cType = $this->getOperandCType(1);
		$functions = array(
			array(
				"static void ZEND_FASTCALL qb_multiply_matrix_by_matrix_2x2_$type(qb_interpreter_context *cxt, $cType *m1, $cType *m2, $cType *res_ptr) {",
					"uint32_t q;",
					"for(q = 0; q < 4; q += 2) {",
						"$cType dot_product0 = (m1[0 + q] * m2[0 + 0]) + (m1[1 + q] * m2[2 + 0]);",
						"$cType dot_product1 = (m1[0 + q] * m2[0 + 1]) + (m1[1 + q] * m2[2 + 1]);",
						"res_ptr[q + 0] = dot_product0;",
						"res_ptr[q + 1] = dot_product1;",
					"}",
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
		return "qb_multiply_matrix_by_matrix_2x2_$type(cxt, op1_ptr, op2_ptr, res_ptr);";
	}
}

?>