<?php

class QBCrossProductHandler extends QBSIMDHandler {

	public function __construct($name, $operandType, $addressMode) {
		parent::__construct($name, $operandType, $addressMode, 3);
	}
	
	public function getHelperFunctions() {
		$cType = $this->getOperandCType(1);
		$type = $this->getOperandType(1);
		$functions = array(
			array(
				"static void ZEND_FASTCALL qb_calculate_cross_product_$type($cType *v1, $cType *v2, $cType *res_ptr) {",
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
	
	public function getSIMDExpression() {
		$type = $this->getOperandType(1);
		return "qb_calculate_cross_product_$type(op1_ptr, op2_ptr, res_ptr);";
	}
}

?>