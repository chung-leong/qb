<?php

class QBComplexSquareRootHandler extends QBComplexNumberHandler {

	public function getHelperFunctions() {
		$functions = array(
			array(
				"static void ZEND_FASTCALL qb_calculate_complex_sqrt_F64(qb_complex_F64 *z, qb_complex_F64 *res) {",
					"float64_t s = (z->i > 0 ? 1.0 : ((z->i < 0 ? -1.0 : 0)));",
					"float64_t w = sqrt(z->r * z->r + z->i * z->i);",
					"float64_t r = sqrt(0.5 * (z->r + w));",
					"float64_t i = sqrt(0.5 * (-z->r + w));",
					"res->r = r; res->i = i;",
				"}",
			),
			array(
				"static void ZEND_FASTCALL qb_calculate_complex_sqrt_F32(qb_complex_F32 *z, qb_complex_F32 *res) {",
					"qb_complex_F64 z64, res64;",
					"z64.r = z->r; z64.i = z->i;",
					"qb_calculate_complex_sqrt_F64(&z64, &res64);",
					"res->r = (float32_t) res64.r; res->i = (float32_t) res64.i;",
				"}",
			),
		);
		return $functions;
	}
				
	protected function getComplexNumberExpression() {
		$type = $this->getOperandType(1);
		return "qb_calculate_complex_sqrt_$type((qb_complex_$type *) op1_ptr, (qb_complex_$type *) res_ptr);";
	}
}

?>