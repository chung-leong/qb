<?php

class QBCrossProductHandler extends QBHandler {

	public function getOperandAddressMode($i) {
		return "ARR";
	}
	
	public function getHelperFunctions() {
		$cType = $this->getOperandCType(1);
		$type = $this->getOperandType(1);
		$functions = array(
			array(
				"static void ZEND_FASTCALL qb_calculate_cross_product_$type(qb_interpreter_context *cxt, $cType *v1, $cType *v2, $cType *res_ptr) {",
					"$cType i = (v1[1] * v2[2]) - (v1[2] * v2[1]);",
					"$cType j = (v1[2] * v2[0]) - (v1[0] * v2[2]);",
					"$cType k = (v1[0] * v2[1]) - (v1[1] * v2[0]);",
					"res_ptr[0] = i;",
					"res_ptr[1] = j;",
					"res_ptr[2] = k;",
				"}",
			),
		);
		return $functions;
	}
	
	public function getAction() {
		// the compiler need to make sure the vectors are of the correct size
		$type = $this->getOperandType(1);
		$lines = array();
		$lines[] = "qb_calculate_cross_product_$type(cxt, op1_ptr, op2_ptr, res_ptr);";
		return $lines;
	}

}

?>