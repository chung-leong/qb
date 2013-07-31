<?php

class QBComplexCosHandler extends QBComplexNumberHandler {

	public function getHelperFunctions() {
		$type = $this->getOperandType(2);
		$cType = $this->getOperandCType(2);
		$f = ($type == 'F32') ? 'f' : '';
		$functions = array(
			array(
				"void ZEND_FASTCALL qb_calculate_complex_cos_$type(qb_complex_$type *z, qb_complex_$type *res) {",
					"$cType r = cos$f(z->r) * cosh$f(z->i);",
					"$cType i = -sin$f(z->r) * sinh$f(z->i);",
					"res->r = r; res->i = i;",
				"}",
			),
		);
		return $functions;
	}

	protected function getScalarExpression() {
		$type = $this->getOperandType(1);
		return "qb_calculate_complex_cos_$type((qb_complex_$type *) op1_ptr, (qb_complex_$type *) res_ptr);";
	}
}

?>