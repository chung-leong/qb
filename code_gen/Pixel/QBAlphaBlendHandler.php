<?php

class QBAlphaBlendHandler extends QBHandler {

	public function getInputOperandCount() {
		return 2;
	}

	public function getOperandAddressMode($i) {
		return "ARR";
	}

	public function getHelperFunctions() {
		$cType = $this->getOperandCType(1);
		$type = $this->getOperandType(1);
		$functions = array(
			array(
				"static void ZEND_FASTCALL qb_alpha_blend_$type($cType *op1_start, $cType *op1_end, $cType *op2_start, $cType *op2_end, $cType *res_start, $cType *res_end) {",
					"$cType *__restrict res_ptr = res_start;",
					"$cType *__restrict op1_ptr = op1_start;",
					"$cType *__restrict op2_ptr = op2_start;",
					"while(res_ptr < res_end) {",
						"$cType a2 = op2_ptr[3];",
						"if(a2 != 0) { ",
							"$cType a1 = op1_ptr[3];",
							"if(a1 != 0) {",
								"$cType r1 = op1_ptr[0];",
								"$cType g1 = op1_ptr[1];",
								"$cType b1 = op1_ptr[2];",
								"$cType r2 = op2_ptr[0];",
								"$cType g2 = op2_ptr[1];",
								"$cType b2 = op2_ptr[2];",
								"$cType af = a2 + a1 * (1 - a2);",
								"$cType iaf = 1 / af;",
								"res_ptr[0] = (r2 * a2 * iaf) + (r1 * a1 * iaf) * (1 - a2);",
								"res_ptr[1] = (g2 * a2 * iaf) + (g1 * a1 * iaf) * (1 - a2);",
								"res_ptr[2] = (b2 * a2 * iaf) + (b1 * a1 * iaf) * (1 - a2);",
								"res_ptr[3] = af;",
							"} else {",
								"res_ptr[0] = op2_ptr[0];",
								"res_ptr[1] = op2_ptr[1];",
								"res_ptr[2] = op2_ptr[2];",
								"res_ptr[3] = op2_ptr[3];",
							"}",
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
						"op2_ptr += 4;",
						"if(op2_ptr >= op2_end) {",
							"op2_ptr = op2_start;",
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
		return "qb_alpha_blend_$type(op1_start, op1_end, op2_start, op2_end, res_start, res_end);";
	}
}

?>