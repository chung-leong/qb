<?php

class QBCrossProductHandler extends QBSIMDHandler {

	public function __construct($name, $operandType, $addressMode) {
		parent::__construct($name, $operandType, $addressMode, 3);
	}
	
	public function getInputOperandCount() {
		return 2;
	}

	public function getHelperFunctions() {
		$cType = $this->getOperandCType(1);
		$type = $this->getOperandType(1);
		$functions = array(
			array(
				"static void ZEND_FASTCALL qb_calculate_cross_product_$type($cType *op1_start, $cType *op1_end, $cType *op2_start, $cType *op2_end, $cType *res_start, $cType *res_end) {",
					"$cType *__restrict res_ptr = res_start;",
					"$cType *__restrict op1_ptr = op1_start;",
					"$cType *__restrict op2_ptr = op2_start;",
					"for(;;) {",
						"$cType i = (op1_ptr[1] * op2_ptr[2]) - (op1_ptr[2] * op2_ptr[1]);",
						"$cType j = (op1_ptr[2] * op2_ptr[0]) - (op1_ptr[0] * op2_ptr[2]);",
						"$cType k = (op1_ptr[0] * op2_ptr[1]) - (op1_ptr[1] * op2_ptr[0]);",
						"res_ptr[0] = i;",
						"res_ptr[1] = j;",
						"res_ptr[2] = k;",
						"res_ptr += 3;",
						"if(res_ptr >= res_end) {",
							"break;",
						"}",
						"op1_ptr += 3;",
						"if(op1_ptr >= op1_end) {",
							"op1_ptr = op1_start;",
						"}",
						"op2_ptr += 3;",
						"if(op2_ptr >= op2_end) {",
							"op2_ptr = op2_start;",
						"}",
					"}",
				"}",
			),
		);
		return $functions;
	}
	
	public function getAction() {
		$type = $this->getOperandType(1);
		if($this->addressMode == "ARR") {
			return "qb_calculate_cross_product_$type(op1_start, op1_end, op2_start, op2_end, res_start, res_end);";
		} else {
			return "qb_calculate_cross_product_$type(op1_ptr, NULL, op2_ptr, NULL, res_ptr, NULL);";
		}
	}
}

?>