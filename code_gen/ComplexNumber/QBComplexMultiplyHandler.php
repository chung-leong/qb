<?php

class QBComplexMultiplyHandler extends QBComplexNumberHandler {

	public function getInputOperandCount() {
		return 2;
	}

	public function getHelperFunctions() {
		$type = $this->getOperandType($this->srcCount + 1);
		$cType = $this->getOperandCType($this->srcCount + 1);
		$f = ($type == 'F32') ? 'f' : '';
		$functions = array(
			array(
				"static void ZEND_FASTCALL qb_multiply_complex_$type(qb_complex_$type *a, qb_complex_$type *b, qb_complex_$type *res) {",
					"$cType r = (a->r * b->r) - (a->i * b->i);",
					"$cType i = (a->r * b->i) + (a->i * b->r);",
					"res->r = r; res->i = i;",
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