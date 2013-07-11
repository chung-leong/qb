<?php

class QBColumnMajorInvertMatrixHandler extends QBSIMDHandler {

	public function getOperandAddressMode($i) {
		return "ARR";
	}
	
	public function getHelperFunctions() {
		$type = $this->getOperandType(1);
		$cType = $this->getOperandCType(1);
		$f = ($type == "F32") ? 'f' : '';
		$functions = array(
			array(
				"static void ZEND_FASTCALL qb_invert_cm_matrix_2x2_$type($cType *op1_start, $cType *op1_end, $cType *res_start, $cType *res_end) {",
					"$cType *__restrict res_ptr = res_start;",
					"$cType *__restrict op1_ptr = op1_start;",
					"for(;;) {",
						"$cType p00 =  op1_ptr[1 * 2 + 1];",
						"$cType p01 = -op1_ptr[0 * 2 + 1];",
						"$cType p10 = -op1_ptr[1 * 2 + 0];",
						"$cType p11 =  op1_ptr[0 * 2 + 0];",
						"$cType det = (op1_ptr[0 * 2 + 0] * p00) + (op1_ptr[0 * 2 + 1] * p10);",
						"if(det != 0) {",
							"$cType rdet = 1.0$f / det;",
							"res_ptr[0 * 2 + 0] = p00 * rdet;",
							"res_ptr[0 * 2 + 1] = p01 * rdet;",
							"res_ptr[1 * 2 + 0] = p10 * rdet;",
							"res_ptr[1 * 2 + 1] = p11 * rdet;",
						"} else {",
							"uint32_t i;",
							"for(i = 0; i < 4; i++) {",
								"res_ptr[i] = ($cType) NAN;",
							"}",
						"}",
						"res_ptr += 4;",
						"if(res_ptr >= res_end) {",
							"break;",
						"}",
						"op1_ptr += 4;",
						"if(op1_ptr >= op1_end) {",
							"op1_ptr = op1_start;",
						"}",
					"}",
				"}",
			),
			array(
				"static void ZEND_FASTCALL qb_invert_cm_matrix_3x3_$type($cType *op1_start, $cType *op1_end, $cType *res_start, $cType *res_end) {",
					"$cType *__restrict res_ptr = res_start;",
					"$cType *__restrict op1_ptr = op1_start;",
					"for(;;) {",
						"$cType p00 = (op1_ptr[1 * 3 + 1] * op1_ptr[2 * 3 + 2]) - (op1_ptr[1 * 3 + 2] * op1_ptr[2 * 3 + 1]);",
						"$cType p01 = (op1_ptr[0 * 3 + 2] * op1_ptr[2 * 3 + 1]) - (op1_ptr[0 * 3 + 1] * op1_ptr[2 * 3 + 2]);",
						"$cType p02 = (op1_ptr[0 * 3 + 1] * op1_ptr[1 * 3 + 2]) - (op1_ptr[0 * 3 + 2] * op1_ptr[1 * 3 + 1]);",
						"$cType p10 = (op1_ptr[1 * 3 + 2] * op1_ptr[2 * 3 + 0]) - (op1_ptr[1 * 3 + 0] * op1_ptr[2 * 3 + 2]);",
						"$cType p11 = (op1_ptr[0 * 3 + 0] * op1_ptr[2 * 3 + 2]) - (op1_ptr[0 * 3 + 2] * op1_ptr[2 * 3 + 0]);",
						"$cType p12 = (op1_ptr[0 * 3 + 2] * op1_ptr[1 * 3 + 0]) - (op1_ptr[0 * 3 + 0] * op1_ptr[1 * 3 + 2]);",
						"$cType p20 = (op1_ptr[1 * 3 + 0] * op1_ptr[2 * 3 + 1]) - (op1_ptr[1 * 3 + 1] * op1_ptr[2 * 3 + 0]);",
						"$cType p21 = (op1_ptr[0 * 3 + 1] * op1_ptr[2 * 3 + 0]) - (op1_ptr[0 * 3 + 0] * op1_ptr[2 * 3 + 1]);",
						"$cType p22 = (op1_ptr[0 * 3 + 0] * op1_ptr[1 * 3 + 1]) - (op1_ptr[0 * 3 + 1] * op1_ptr[1 * 3 + 0]);",
						"$cType det = (op1_ptr[0 * 3 + 0] * p00) + (op1_ptr[0 * 3 + 1] * p10) + (op1_ptr[0 * 3 + 2] * p20);",
						"if(det != 0) {",
							"$cType rdet = 1.0$f / det;",
							"res_ptr[0 * 3 + 0] = p00 * rdet;",
							"res_ptr[0 * 3 + 1] = p01 * rdet;",
							"res_ptr[0 * 3 + 2] = p02 * rdet;",
							"res_ptr[1 * 3 + 0] = p10 * rdet;",
							"res_ptr[1 * 3 + 1] = p11 * rdet;",
							"res_ptr[1 * 3 + 2] = p12 * rdet;",
							"res_ptr[2 * 3 + 0] = p20 * rdet;",
							"res_ptr[2 * 3 + 1] = p21 * rdet;",
							"res_ptr[2 * 3 + 2] = p22 * rdet;",
						"} else {",
							"uint32_t i;",
							"for(i = 0; i < 9; i++) {",
								"res_ptr[i] = ($cType) NAN;",
							"}",
						"}",
						"res_ptr += 9;",
						"if(res_ptr >= res_end) {",
							"break;",
						"}",
						"op1_ptr += 9;",
						"if(op1_ptr >= op1_end) {",
							"op1_ptr = op1_start;",
						"}",
					"}",
				"}",
			),
			array(
				"static void ZEND_FASTCALL qb_invert_cm_matrix_4x4_$type($cType *op1_start, $cType *op1_end, $cType *res_start, $cType *res_end) {",
					"$cType *__restrict res_ptr = res_start;",
					"$cType *__restrict op1_ptr = op1_start;",
					"for(;;) {",
						"$cType p00 = (op1_ptr[1 * 4 + 2] * op1_ptr[2 * 4 + 3] * op1_ptr[3 * 4 + 1]) - (op1_ptr[1 * 4 + 3] * op1_ptr[2 * 4 + 2] * op1_ptr[3 * 4 + 1]) + (op1_ptr[1 * 4 + 3] * op1_ptr[2 * 4 + 1] * op1_ptr[3 * 4 + 2]) - (op1_ptr[1 * 4 + 1] * op1_ptr[2 * 4 + 3] * op1_ptr[3 * 4 + 2]) - (op1_ptr[1 * 4 + 2] * op1_ptr[2 * 4 + 1] * op1_ptr[3 * 4 + 3]) + (op1_ptr[1 * 4 + 1] * op1_ptr[2 * 4 + 2] * op1_ptr[3 * 4 + 3]);",
						"$cType p01 = (op1_ptr[0 * 4 + 3] * op1_ptr[2 * 4 + 2] * op1_ptr[3 * 4 + 1]) - (op1_ptr[0 * 4 + 2] * op1_ptr[2 * 4 + 3] * op1_ptr[3 * 4 + 1]) - (op1_ptr[0 * 4 + 3] * op1_ptr[2 * 4 + 1] * op1_ptr[3 * 4 + 2]) + (op1_ptr[0 * 4 + 1] * op1_ptr[2 * 4 + 3] * op1_ptr[3 * 4 + 2]) + (op1_ptr[0 * 4 + 2] * op1_ptr[2 * 4 + 1] * op1_ptr[3 * 4 + 3]) - (op1_ptr[0 * 4 + 1] * op1_ptr[2 * 4 + 2] * op1_ptr[3 * 4 + 3]);",
						"$cType p02 = (op1_ptr[0 * 4 + 2] * op1_ptr[1 * 4 + 3] * op1_ptr[3 * 4 + 1]) - (op1_ptr[0 * 4 + 3] * op1_ptr[1 * 4 + 2] * op1_ptr[3 * 4 + 1]) + (op1_ptr[0 * 4 + 3] * op1_ptr[1 * 4 + 1] * op1_ptr[3 * 4 + 2]) - (op1_ptr[0 * 4 + 1] * op1_ptr[1 * 4 + 3] * op1_ptr[3 * 4 + 2]) - (op1_ptr[0 * 4 + 2] * op1_ptr[1 * 4 + 1] * op1_ptr[3 * 4 + 3]) + (op1_ptr[0 * 4 + 1] * op1_ptr[1 * 4 + 2] * op1_ptr[3 * 4 + 3]);",
						"$cType p03 = (op1_ptr[0 * 4 + 3] * op1_ptr[1 * 4 + 2] * op1_ptr[2 * 4 + 1]) - (op1_ptr[0 * 4 + 2] * op1_ptr[1 * 4 + 3] * op1_ptr[2 * 4 + 1]) - (op1_ptr[0 * 4 + 3] * op1_ptr[1 * 4 + 1] * op1_ptr[2 * 4 + 2]) + (op1_ptr[0 * 4 + 1] * op1_ptr[1 * 4 + 3] * op1_ptr[2 * 4 + 2]) + (op1_ptr[0 * 4 + 2] * op1_ptr[1 * 4 + 1] * op1_ptr[2 * 4 + 3]) - (op1_ptr[0 * 4 + 1] * op1_ptr[1 * 4 + 2] * op1_ptr[2 * 4 + 3]);",
						"$cType p10 = (op1_ptr[1 * 4 + 3] * op1_ptr[2 * 4 + 2] * op1_ptr[3 * 4 + 0]) - (op1_ptr[1 * 4 + 2] * op1_ptr[2 * 4 + 3] * op1_ptr[3 * 4 + 0]) - (op1_ptr[1 * 4 + 3] * op1_ptr[2 * 4 + 0] * op1_ptr[3 * 4 + 2]) + (op1_ptr[1 * 4 + 0] * op1_ptr[2 * 4 + 3] * op1_ptr[3 * 4 + 2]) + (op1_ptr[1 * 4 + 2] * op1_ptr[2 * 4 + 0] * op1_ptr[3 * 4 + 3]) - (op1_ptr[1 * 4 + 0] * op1_ptr[2 * 4 + 2] * op1_ptr[3 * 4 + 3]);",
						"$cType p11 = (op1_ptr[0 * 4 + 2] * op1_ptr[2 * 4 + 3] * op1_ptr[3 * 4 + 0]) - (op1_ptr[0 * 4 + 3] * op1_ptr[2 * 4 + 2] * op1_ptr[3 * 4 + 0]) + (op1_ptr[0 * 4 + 3] * op1_ptr[2 * 4 + 0] * op1_ptr[3 * 4 + 2]) - (op1_ptr[0 * 4 + 0] * op1_ptr[2 * 4 + 3] * op1_ptr[3 * 4 + 2]) - (op1_ptr[0 * 4 + 2] * op1_ptr[2 * 4 + 0] * op1_ptr[3 * 4 + 3]) + (op1_ptr[0 * 4 + 0] * op1_ptr[2 * 4 + 2] * op1_ptr[3 * 4 + 3]);",
						"$cType p12 = (op1_ptr[0 * 4 + 3] * op1_ptr[1 * 4 + 2] * op1_ptr[3 * 4 + 0]) - (op1_ptr[0 * 4 + 2] * op1_ptr[1 * 4 + 3] * op1_ptr[3 * 4 + 0]) - (op1_ptr[0 * 4 + 3] * op1_ptr[1 * 4 + 0] * op1_ptr[3 * 4 + 2]) + (op1_ptr[0 * 4 + 0] * op1_ptr[1 * 4 + 3] * op1_ptr[3 * 4 + 2]) + (op1_ptr[0 * 4 + 2] * op1_ptr[1 * 4 + 0] * op1_ptr[3 * 4 + 3]) - (op1_ptr[0 * 4 + 0] * op1_ptr[1 * 4 + 2] * op1_ptr[3 * 4 + 3]);",
						"$cType p13 = (op1_ptr[0 * 4 + 2] * op1_ptr[1 * 4 + 3] * op1_ptr[2 * 4 + 0]) - (op1_ptr[0 * 4 + 3] * op1_ptr[1 * 4 + 2] * op1_ptr[2 * 4 + 0]) + (op1_ptr[0 * 4 + 3] * op1_ptr[1 * 4 + 0] * op1_ptr[2 * 4 + 2]) - (op1_ptr[0 * 4 + 0] * op1_ptr[1 * 4 + 3] * op1_ptr[2 * 4 + 2]) - (op1_ptr[0 * 4 + 2] * op1_ptr[1 * 4 + 0] * op1_ptr[2 * 4 + 3]) + (op1_ptr[0 * 4 + 0] * op1_ptr[1 * 4 + 2] * op1_ptr[2 * 4 + 3]);",
						"$cType p20 = (op1_ptr[1 * 4 + 1] * op1_ptr[2 * 4 + 3] * op1_ptr[3 * 4 + 0]) - (op1_ptr[1 * 4 + 3] * op1_ptr[2 * 4 + 1] * op1_ptr[3 * 4 + 0]) + (op1_ptr[1 * 4 + 3] * op1_ptr[2 * 4 + 0] * op1_ptr[3 * 4 + 1]) - (op1_ptr[1 * 4 + 0] * op1_ptr[2 * 4 + 3] * op1_ptr[3 * 4 + 1]) - (op1_ptr[1 * 4 + 1] * op1_ptr[2 * 4 + 0] * op1_ptr[3 * 4 + 3]) + (op1_ptr[1 * 4 + 0] * op1_ptr[2 * 4 + 1] * op1_ptr[3 * 4 + 3]);",
						"$cType p21 = (op1_ptr[0 * 4 + 3] * op1_ptr[2 * 4 + 1] * op1_ptr[3 * 4 + 0]) - (op1_ptr[0 * 4 + 1] * op1_ptr[2 * 4 + 3] * op1_ptr[3 * 4 + 0]) - (op1_ptr[0 * 4 + 3] * op1_ptr[2 * 4 + 0] * op1_ptr[3 * 4 + 1]) + (op1_ptr[0 * 4 + 0] * op1_ptr[2 * 4 + 3] * op1_ptr[3 * 4 + 1]) + (op1_ptr[0 * 4 + 1] * op1_ptr[2 * 4 + 0] * op1_ptr[3 * 4 + 3]) - (op1_ptr[0 * 4 + 0] * op1_ptr[2 * 4 + 1] * op1_ptr[3 * 4 + 3]);",
						"$cType p22 = (op1_ptr[0 * 4 + 1] * op1_ptr[1 * 4 + 3] * op1_ptr[3 * 4 + 0]) - (op1_ptr[0 * 4 + 3] * op1_ptr[1 * 4 + 1] * op1_ptr[3 * 4 + 0]) + (op1_ptr[0 * 4 + 3] * op1_ptr[1 * 4 + 0] * op1_ptr[3 * 4 + 1]) - (op1_ptr[0 * 4 + 0] * op1_ptr[1 * 4 + 3] * op1_ptr[3 * 4 + 1]) - (op1_ptr[0 * 4 + 1] * op1_ptr[1 * 4 + 0] * op1_ptr[3 * 4 + 3]) + (op1_ptr[0 * 4 + 0] * op1_ptr[1 * 4 + 1] * op1_ptr[3 * 4 + 3]);",
						"$cType p23 = (op1_ptr[0 * 4 + 3] * op1_ptr[1 * 4 + 1] * op1_ptr[2 * 4 + 0]) - (op1_ptr[0 * 4 + 1] * op1_ptr[1 * 4 + 3] * op1_ptr[2 * 4 + 0]) - (op1_ptr[0 * 4 + 3] * op1_ptr[1 * 4 + 0] * op1_ptr[2 * 4 + 1]) + (op1_ptr[0 * 4 + 0] * op1_ptr[1 * 4 + 3] * op1_ptr[2 * 4 + 1]) + (op1_ptr[0 * 4 + 1] * op1_ptr[1 * 4 + 0] * op1_ptr[2 * 4 + 3]) - (op1_ptr[0 * 4 + 0] * op1_ptr[1 * 4 + 1] * op1_ptr[2 * 4 + 3]);",
						"$cType p30 = (op1_ptr[1 * 4 + 2] * op1_ptr[2 * 4 + 1] * op1_ptr[3 * 4 + 0]) - (op1_ptr[1 * 4 + 1] * op1_ptr[2 * 4 + 2] * op1_ptr[3 * 4 + 0]) - (op1_ptr[1 * 4 + 2] * op1_ptr[2 * 4 + 0] * op1_ptr[3 * 4 + 1]) + (op1_ptr[1 * 4 + 0] * op1_ptr[2 * 4 + 2] * op1_ptr[3 * 4 + 1]) + (op1_ptr[1 * 4 + 1] * op1_ptr[2 * 4 + 0] * op1_ptr[3 * 4 + 2]) - (op1_ptr[1 * 4 + 0] * op1_ptr[2 * 4 + 1] * op1_ptr[3 * 4 + 2]);",
						"$cType p31 = (op1_ptr[0 * 4 + 1] * op1_ptr[2 * 4 + 2] * op1_ptr[3 * 4 + 0]) - (op1_ptr[0 * 4 + 2] * op1_ptr[2 * 4 + 1] * op1_ptr[3 * 4 + 0]) + (op1_ptr[0 * 4 + 2] * op1_ptr[2 * 4 + 0] * op1_ptr[3 * 4 + 1]) - (op1_ptr[0 * 4 + 0] * op1_ptr[2 * 4 + 2] * op1_ptr[3 * 4 + 1]) - (op1_ptr[0 * 4 + 1] * op1_ptr[2 * 4 + 0] * op1_ptr[3 * 4 + 2]) + (op1_ptr[0 * 4 + 0] * op1_ptr[2 * 4 + 1] * op1_ptr[3 * 4 + 2]);",
						"$cType p32 = (op1_ptr[0 * 4 + 2] * op1_ptr[1 * 4 + 1] * op1_ptr[3 * 4 + 0]) - (op1_ptr[0 * 4 + 1] * op1_ptr[1 * 4 + 2] * op1_ptr[3 * 4 + 0]) - (op1_ptr[0 * 4 + 2] * op1_ptr[1 * 4 + 0] * op1_ptr[3 * 4 + 1]) + (op1_ptr[0 * 4 + 0] * op1_ptr[1 * 4 + 2] * op1_ptr[3 * 4 + 1]) + (op1_ptr[0 * 4 + 1] * op1_ptr[1 * 4 + 0] * op1_ptr[3 * 4 + 2]) - (op1_ptr[0 * 4 + 0] * op1_ptr[1 * 4 + 1] * op1_ptr[3 * 4 + 2]);",
						"$cType p33 = (op1_ptr[0 * 4 + 1] * op1_ptr[1 * 4 + 2] * op1_ptr[2 * 4 + 0]) - (op1_ptr[0 * 4 + 2] * op1_ptr[1 * 4 + 1] * op1_ptr[2 * 4 + 0]) + (op1_ptr[0 * 4 + 2] * op1_ptr[1 * 4 + 0] * op1_ptr[2 * 4 + 1]) - (op1_ptr[0 * 4 + 0] * op1_ptr[1 * 4 + 2] * op1_ptr[2 * 4 + 1]) - (op1_ptr[0 * 4 + 1] * op1_ptr[1 * 4 + 0] * op1_ptr[2 * 4 + 2]) + (op1_ptr[0 * 4 + 0] * op1_ptr[1 * 4 + 1] * op1_ptr[2 * 4 + 2]);",
						"$cType det = (op1_ptr[0 * 4 + 0] * p00) + (op1_ptr[0 * 4 + 1] * p10) + (op1_ptr[0 * 4 + 2] * p20) + (op1_ptr[0 * 4 + 3] * p30);",
						"if(det != 0) {",
							"$cType rdet = 1.0$f / det;",
							"res_ptr[0 * 4 + 0] = p00 * rdet;",
							"res_ptr[0 * 4 + 1] = p01 * rdet;",
							"res_ptr[0 * 4 + 2] = p02 * rdet;",
							"res_ptr[0 * 4 + 3] = p03 * rdet;",
							"res_ptr[1 * 4 + 0] = p10 * rdet;",
							"res_ptr[1 * 4 + 1] = p11 * rdet;",
							"res_ptr[1 * 4 + 2] = p12 * rdet;",
							"res_ptr[1 * 4 + 3] = p13 * rdet;",
							"res_ptr[2 * 4 + 0] = p20 * rdet;",
							"res_ptr[2 * 4 + 1] = p21 * rdet;",
							"res_ptr[2 * 4 + 2] = p22 * rdet;",
							"res_ptr[2 * 4 + 3] = p23 * rdet;",
							"res_ptr[3 * 4 + 0] = p30 * rdet;",
							"res_ptr[3 * 4 + 1] = p31 * rdet;",
							"res_ptr[3 * 4 + 2] = p32 * rdet;",
							"res_ptr[3 * 4 + 3] = p33 * rdet;",
						"} else {",
							"uint32_t i;",
							"for(i = 0; i < 16; i++) {",
								"res_ptr[i] = ($cType) NAN;",
							"}",
						"}",
						"res_ptr += 16;",
						"if(res_ptr >= res_end) {",
							"break;",
						"}",
						"op1_ptr += 16;",
						"if(op1_ptr >= op1_end) {",
							"op1_ptr = op1_start;",
						"}",
					"}",
				"}",
			),
		);
		return $functions;
	}
	
	public function getOperandSize($i) {
		if($this->operandSize == "variable") {
			return "MATRIX1_ROWS * MATRIX1_COLS";
		} else {
			return $this->operandSize * $this->operandSize;
		}
	}
	
	public function getAction() {
		$type = $this->getOperandType(1);
		if($this->operandSize == "variable") {
			throw Exception("No implementation for matrices of arbituary size");
		} else {
			if($this->addressMode == "ARR") {
				return "qb_invert_cm_matrix_{$this->operandSize}x{$this->operandSize}_$type(op1_start, op1_end, res_start, res_end);";
			} else {
				return "qb_invert_cm_matrix_{$this->operandSize}x{$this->operandSize}_$type(op1_ptr, NULL, res_ptr, NULL);";
			}
		}
	}
}

?>