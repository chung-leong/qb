<?php

class QBRemovePremultiplicationHandler extends QBHandler {

	public function getOperandAddressMode($i) {
		return "ARR";
	}

	public function getHelperFunctions() {
		$cType = $this->getOperandCType(1);
		$type = $this->getOperandType(1);
		$functions = array(
			array(
				"void ZEND_FASTCALL qb_remove_premultiplication_$type($cType *op1_start, $cType *op1_end, $cType *res_start, $cType *res_end) {",
					"$cType *__restrict res_ptr = res_start;",
					"$cType *__restrict op1_ptr = op1_start;",
					"while(res_ptr < res_end) {",
						"$cType a = op1_ptr[3];",
						"if(a != 1) { ",
							"$cType ia = (a != 0) ? 1 / a : 0;",
							"res_ptr[0] = op1_ptr[0] * ia;",
							"res_ptr[1] = op1_ptr[1] * ia;",
							"res_ptr[2] = op1_ptr[2] * ia;",
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
		return "qb_remove_premultiplication_$type(op1_ptr, op1_ptr + op1_count, res_ptr, res_ptr + res_count);";
	}
}

?>