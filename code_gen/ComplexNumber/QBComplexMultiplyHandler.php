<?php

class QBComplexMultiplyHandler extends QBComplexNumberHandler {

	public function getHelperFunctions() {
		$functions = array(
			array(
				"static void ZEND_FASTCALL qb_multiply_complex_F64(qb_complex_F64 *a, qb_complex_F64 *b, qb_complex_F64 *res) {",
					"float64_t r = (a->r * b->r) - (a->i * b->i);",
					"float64_t i = (a->r * b->i) + (a->i * b->r);",
					"res->r = r; res->i = i;",
				"}",
			),
			array(
				"static void ZEND_FASTCALL qb_multiply_complex_F32(qb_complex_F32 *a, qb_complex_F32 *b, qb_complex_F32 *res) {",
					"float32_t r = (a->r * b->r) - (a->i * b->i);",
					"float32_t i = (a->r * b->i) + (a->i * b->r);",
					"res->r = r; res->i = i;",
				"}",
			),
		);
		return $functions;
	}

	protected function getComplexNumberExpression() {
		$type = $this->getOperandType(1);
		return "qb_multiply_complex_$type((qb_complex_$type *) op1_ptr, (qb_complex_$type *) op2_ptr, (qb_complex_$type *) res_ptr);";
	}
}

?>