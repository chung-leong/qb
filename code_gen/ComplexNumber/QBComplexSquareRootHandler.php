<?php

class QBComplexSquareRootHandler extends QBComplexNumberHandler {

	public function getHelperFunctions() {
		$type = $this->getOperandType($this->srcCount + 1);
		$cType = $this->getOperandCType($this->srcCount + 1);
		$f = ($type == 'F32') ? 'f' : '';
		$functions = array(
			array(
				"static void ZEND_FASTCALL qb_calculate_complex_sqrt_$type(qb_complex_$type *z, qb_complex_$type *res) {",
					"$cType s = (z->i > 0 ? 1.0$f : ((z->i < 0 ? -1.0$f : 0.0$f)));",
					"$cType w = sqrt$f(z->r * z->r + z->i * z->i);",
					"$cType r = sqrt$f(0.5$f * (z->r + w));",
					"$cType i = (s) ? s * sqrt$f(0.5$f * (-z->r + w)) : 0;",
					"res->r = r; res->i = i;",
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