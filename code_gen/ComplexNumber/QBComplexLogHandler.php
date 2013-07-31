<?php

class QBComplexLogHandler extends QBComplexNumberHandler {

	public function getHelperFunctions() {
		$type = $this->getOperandType(2);
		$cType = $this->getOperandCType(2);
		$f = ($type == 'F32') ? 'f' : '';
		$functions = array(
			array(
				"void ZEND_FASTCALL qb_calculate_complex_log_$type(qb_complex_$type *z, qb_complex_$type *res) {",
					"$cType w = sqrt$f(z->r * z->r + z->i * z->i);",
					"$cType r = log$f(w);",
					"$cType i = atan2$f(z->i, z->r);",
					"res->r = r; res->i = i;",
				"}",
			),
		);
		return $functions;
	}

	protected function getScalarExpression() {
		$type = $this->getOperandType(1);
		return "qb_calculate_complex_log_$type((qb_complex_$type *) op1_ptr, (qb_complex_$type *) res_ptr);";
	}
}

?>