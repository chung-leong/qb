<?php

class QBComplexExpHandler extends QBComplexNumberHandler {

	public function getHelperFunctions() {
		$type = $this->getOperandType($this->srcCount + 1);
		$cType = $this->getOperandCType($this->srcCount + 1);
		$f = ($type == 'F32') ? 'f' : '';
		$functions = array(
			array(
				"static void ZEND_FASTCALL qb_calculate_complex_exp_$type(qb_complex_$type *z, qb_complex_$type *res) {",
					"$cType w = exp$f(z->r);",
					"$cType r = w * cos$f(z->i);",
					"$cType i = w * sin$f(z->i);",
					"res->r = r; res->i = i;",
				"}",
			),
		);
		return $functions;
	}

	protected function getComplexNumberExpression() {
		$type = $this->getOperandType(1);
		return "qb_calculate_complex_exp_$type((qb_complex_$type *) op1_ptr, (qb_complex_$type *) res_ptr);";
	}
}

?>