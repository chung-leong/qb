<?php

class QBComplexTanhHandler extends QBComplexNumberHandler {

	public function getHelperFunctions() {
		$type = $this->getOperandType($this->srcCount + 1);
		$cType = $this->getOperandCType($this->srcCount + 1);
		$f = ($type == 'F32') ? 'f' : '';
		$functions = array(
			array(
				"static void ZEND_FASTCALL qb_calculate_complex_tanh_$type(qb_complex_$type *z, qb_complex_$type *res) {",
					"$cType w = 1 / (cosh$f(2.0 * z->r) + cos$f(2.0 * z->i));",
					"$cType r = w * sinh$f(2.0 * z->r);",
					"$cType i = w * sin$f(2.0 * z->i);",
					"res->r = r; res->i = i;",
				"}",
			),
		);
		return $functions;
	}
	
	protected function getComplexNumberExpression() {
		$type = $this->getOperandType(1);
		return "qb_calculate_complex_tanh_$type((qb_complex_$type *) op1_ptr, (qb_complex_$type *) res_ptr);";
	}
}

?>