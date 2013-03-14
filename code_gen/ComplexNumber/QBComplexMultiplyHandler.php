<?php

class QBComplexMultiplyHandler extends QBComplexNumberHandler {

	public function getHelperFunctions() {
		$functions = array(
			array(
				"static void ZEND_FASTCALL qb_multiply_complex_F64(qb_complex_F64 *a, qb_complex_F64 *b, qb_complex_F64 *res) {",
					"res->r = (a->r * b->r) - (a->i * b->i);",
					"res->i = (a->r * b->i) + (a->i * b->r);",
				"}",
			),
			array(
				"static void ZEND_FASTCALL qb_multiply_complex_F32(qb_complex_F32 *a, qb_complex_F32 *b, qb_complex_F32 *res) {",
					"res->r = (a->r * b->r) - (a->i * b->i);",
					"res->i = (a->r * b->i) + (a->i * b->r);",
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