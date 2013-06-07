<?php

class QBApplyPremultiplicationHandler extends QBHandler {

	public function getOperandAddressMode($i) {
		return "ARR";
	}

	public function getHelperFunctions() {
		$cType = $this->getOperandCType(1);
		$type = $this->getOperandType(1);
		$functions = array(
			array(
				"static void ZEND_FASTCALL qb_apply_premultiplication_$type($cType *op1_start, $cType *op1_end, $cType *res_start, $cType *res_end) {",
					"$cType *__restrict res_ptr = res_start;",
					"$cType *__restrict op1_ptr = op1_start;",
					"while(res_ptr < res_end) {",
						"$cType a = op1_ptr[3];",
						"if(a != 1) {",
							"res_ptr[0] = op1_ptr[0] * a;",
							"res_ptr[1] = op1_ptr[1] * a;",
							"res_ptr[2] = op1_ptr[2] * a;",
							"res_ptr[3] = op1_ptr[3];",
						"} else {",
							"res_ptr[0] = op1_ptr[0];",
							"res_ptr[1] = op1_ptr[1];",
							"res_ptr[2] = op1_ptr[2];",
							"res_ptr[3] = op1_ptr[3];",
						"}",
						"op1_ptr += 4;",
						"if(op1_ptr >= op1_end) {",
							"op1_ptr = op1_start;",
						"}",
						"res_ptr += 4;",
					"}",
				"}",
			),
		);
		return $functions;
	}
		
	public function getAction() {
		$type = $this->getOperandType(1);
		return "qb_apply_premultiplication_$type(op1_start, op1_end, res_start, res_end);";
	}
}

?>