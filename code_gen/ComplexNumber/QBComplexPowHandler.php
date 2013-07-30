<?php

class QBComplexPowHandler extends QBComplexNumberHandler {

	public function getInputOperandCount() {
		return 2;
	}

	public function getHelperFunctions() {
		$type = $this->getOperandType(3);
		$cType = $this->getOperandCType(3);
		$f = ($type == 'F32') ? 'f' : '';
		$functions = array(
			array(
				"void ZEND_FASTCALL qb_calculate_complex_pow_$type(qb_complex_$type *b, qb_complex_$type *e, qb_complex_$type *res) {",
					"$cType u = atan2$f(b->i, b->r);",
					"$cType v = b->r * b->r + b->i * b->i;",
					"$cType w = 1;",
					"$cType x = pow$f(v, 0.5$f * e->r);",		
					"$cType y = e->r * u;",
					"$cType z = 0;",
					"if(e->i != 0) {",
						"z = 0.5$f * e->i * log$f(v);",
						"w = exp$f(-e->i * u);",
					"}",
					"res->r = x * w * cos$f(y + z);",
					"res->i = x * w * sin$f(y + z);",
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