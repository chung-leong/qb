<?php

class QBComplexCosHandler extends QBComplexNumberHandler {

	public function getHelperFunctions() {
		$functions = array(
			array(
				"static void ZEND_FASTCALL qb_calculate_complex_cos_F64(qb_complex_F64 *z, qb_complex_F64 *res) {",
					"res->r = cos(z->r) * cosh(z->i);",
					"res->i = -sin(z->r) * sinh(z->i);",
				"}",
			),
			array(
				"static void ZEND_FASTCALL qb_calculate_complex_cos_F32(qb_complex_F32 *z, qb_complex_F32 *res) {",
					"qb_complex_F64 z64, res64;",
					"z64.r = z->r; z64.i = z->i;",
					"qb_calculate_complex_cos_F64(&z64, &res64);",
					"res->r = (float32_t) res64.r; res->i = (float32_t) res64.i;",
				"}",
			),
		);
		return $functions;
	}

	protected function getComplexNumberExpression() {
		$type = $this->getOperandType(1);
		return "qb_calculate_complex_cos_$type((qb_complex_$type *) op1_ptr, (qb_complex_$type *) res_ptr);";
	}
}

?>