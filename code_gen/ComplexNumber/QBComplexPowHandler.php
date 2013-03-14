<?php

class QBComplexPowHandler extends QBComplexNumberHandler {

	public function getHelperFunctions() {
		$functions = array(
			array(
				"static void ZEND_FASTCALL qb_calculate_complex_pow_F64(qb_complex_F64 *b, qb_complex_F64 *e, qb_complex_F64 *res) {",
					"float64_t u = atan2(b->i, b->r);",
					"float64_t v = b->r * b->r + b->i * b->i;",
					"float64_t w = 1.0;",
					"float64_t x = pow(v, 0.5 * e->r);",		
					"float64_t y = e->r * u;",
					"float64_t z = 0.0;",
					"if(e->i != 0.0) {",
						"z = 0.5 * e->i * log(v);",
						"w = exp(-e->i * u);",
					"}",
					"res->r = x * w * cos(y + z);",
					"res->i = x * w * sin(y + z);",
				"}",
			),
			array(
				"static void ZEND_FASTCALL qb_calculate_complex_pow_F32(qb_complex_F32 *b, qb_complex_F32 *e, qb_complex_F32 *res) {",
					"qb_complex_F64 b64, e64, res64;",
					"b64.r = b->r; b64.i = b->i;",
					"e64.r = e->r; e64.i = e->i;",
					"qb_calculate_complex_pow_F64(&b64, &e64, &res64);",
					"res->r = (float32_t) res64.r; res->i = (float32_t) res64.i;",
				"}",
			),
		);
		return $functions;
	}

	protected function getComplexNumberExpression() {
		$type = $this->getOperandType(1);
		return "qb_calculate_complex_pow_$type((qb_complex_$type *) op1_ptr, (qb_complex_$type *) op2_ptr, (qb_complex_$type *) res_ptr);";
	}
}

?>