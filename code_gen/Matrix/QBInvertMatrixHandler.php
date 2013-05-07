<?php

class QBInvertMatrixHandler extends QBSIMDHandler {

	public function getOperandAddressMode($i) {
		return "ARR";
	}
	
	public function getHelperFunctions() {
		$type = $this->getOperandType(1);
		$cType = $this->getOperandCType(1);
		$f = ($type == "F32") ? 'f' : '';
		$functions = array(
			array(
				"static void ZEND_FASTCALL qb_invert_matrix_2x2_$type($cType *m, $cType *res_ptr) {",
					"$cType p00 =  m[3];",
					"$cType p01 = -m[1];",
					"$cType p10 = -m[2];",
					"$cType p11 =  m[0];",
					"$cType det = (m[0] * p00) + (m[1] * p10);",
					"if(det != 0) {",
						"$cType rdet = 1.0$f / det;",
						"res_ptr[0] = p00 * rdet;",
						"res_ptr[1] = p01 * rdet;",
						"res_ptr[2] = p10 * rdet;",
						"res_ptr[3] = p11 * rdet;",
					"} else {",
						"uint32_t i;",
						"for(i = 0; i < 4; i++) {",
							"res_ptr[i] = ($cType) NAN;",
						"}",
					"}",
				"}",
			),
			array(
				"static void ZEND_FASTCALL qb_invert_matrix_3x3_$type($cType *m, $cType *res_ptr) {",
					"$cType p00 = (m[4] * m[8]) - (m[5] * m[7]);",
					"$cType p01 = (m[2] * m[7]) - (m[1] * m[8]);",
					"$cType p02 = (m[1] * m[5]) - (m[2] * m[4]);",
					"$cType p10 = (m[5] * m[6]) - (m[3] * m[8]);",
					"$cType p11 = (m[0] * m[8]) - (m[2] * m[6]);",
					"$cType p12 = (m[2] * m[3]) - (m[0] * m[5]);",
					"$cType p20 = (m[3] * m[7]) - (m[4] * m[6]);",
					"$cType p21 = (m[1] * m[6]) - (m[0] * m[7]);",
					"$cType p22 = (m[0] * m[4]) - (m[1] * m[3]);",
					"$cType det = (m[0] * p00) + (m[1] * p10) + (m[2] * p20);",
					"if(det != 0) {",
						"$cType rdet = 1.0$f / det;",
						"res_ptr[0] = p00 * rdet;",
						"res_ptr[1] = p01 * rdet;",
						"res_ptr[2] = p02 * rdet;",
						"res_ptr[3] = p10 * rdet;",
						"res_ptr[4] = p11 * rdet;",
						"res_ptr[5] = p12 * rdet;",
						"res_ptr[6] = p20 * rdet;",
						"res_ptr[7] = p21 * rdet;",
						"res_ptr[8] = p22 * rdet;",
					"} else {",
						"uint32_t i;",
						"for(i = 0; i < 9; i++) {",
							"res_ptr[i] = ($cType) NAN;",
						"}",
					"}",
				"}",
			),
			array(
				"static void ZEND_FASTCALL qb_invert_matrix_4x4_$type($cType *m, $cType *res_ptr) {",
					"$cType p00 = (m[ 6] * m[11] * m[13]) - (m[ 7] * m[10] * m[13]) + (m[ 7] * m[ 9] * m[14]) - (m[ 5] * m[11] * m[14]) - (m[ 6] * m[ 9] * m[15]) + (m[ 5] * m[10] * m[15]);",
					"$cType p01 = (m[ 3] * m[10] * m[13]) - (m[ 2] * m[11] * m[13]) - (m[ 3] * m[ 9] * m[14]) + (m[ 1] * m[11] * m[14]) + (m[ 2] * m[ 9] * m[15]) - (m[ 1] * m[10] * m[15]);",
					"$cType p02 = (m[ 2] * m[ 7] * m[13]) - (m[ 3] * m[ 6] * m[13]) + (m[ 3] * m[ 5] * m[14]) - (m[ 1] * m[ 7] * m[14]) - (m[ 2] * m[ 5] * m[15]) + (m[ 1] * m[ 6] * m[15]);",
					"$cType p03 = (m[ 3] * m[ 6] * m[ 9]) - (m[ 2] * m[ 7] * m[ 9]) - (m[ 3] * m[ 5] * m[10]) + (m[ 1] * m[ 7] * m[10]) + (m[ 2] * m[ 5] * m[11]) - (m[ 1] * m[ 6] * m[11]);",
					"$cType p10 = (m[ 7] * m[10] * m[12]) - (m[ 6] * m[11] * m[12]) - (m[ 7] * m[ 8] * m[14]) + (m[ 4] * m[11] * m[14]) + (m[ 6] * m[ 8] * m[15]) - (m[ 4] * m[10] * m[15]);",
					"$cType p11 = (m[ 2] * m[11] * m[12]) - (m[ 3] * m[10] * m[12]) + (m[ 3] * m[ 8] * m[14]) - (m[ 0] * m[11] * m[14]) - (m[ 2] * m[ 8] * m[15]) + (m[ 0] * m[10] * m[15]);",
					"$cType p12 = (m[ 3] * m[ 6] * m[12]) - (m[ 2] * m[ 7] * m[12]) - (m[ 3] * m[ 4] * m[14]) + (m[ 0] * m[ 7] * m[14]) + (m[ 2] * m[ 4] * m[15]) - (m[ 0] * m[ 6] * m[15]);",
					"$cType p13 = (m[ 2] * m[ 7] * m[ 8]) - (m[ 3] * m[ 6] * m[ 8]) + (m[ 3] * m[ 4] * m[10]) - (m[ 0] * m[ 7] * m[10]) - (m[ 2] * m[ 4] * m[11]) + (m[ 0] * m[ 6] * m[11]);",
					"$cType p20 = (m[ 5] * m[11] * m[12]) - (m[ 7] * m[ 9] * m[12]) + (m[ 7] * m[ 8] * m[13]) - (m[ 4] * m[11] * m[13]) - (m[ 5] * m[ 8] * m[15]) + (m[ 4] * m[ 9] * m[15]);",
					"$cType p21 = (m[ 3] * m[ 9] * m[12]) - (m[ 1] * m[11] * m[12]) - (m[ 3] * m[ 8] * m[13]) + (m[ 0] * m[11] * m[13]) + (m[ 1] * m[ 8] * m[15]) - (m[ 0] * m[ 9] * m[15]);",
					"$cType p22 = (m[ 1] * m[ 7] * m[12]) - (m[ 3] * m[ 5] * m[12]) + (m[ 3] * m[ 4] * m[13]) - (m[ 0] * m[ 7] * m[13]) - (m[ 1] * m[ 4] * m[15]) + (m[ 0] * m[ 5] * m[15]);",
					"$cType p23 = (m[ 3] * m[ 5] * m[ 8]) - (m[ 1] * m[ 7] * m[ 8]) - (m[ 3] * m[ 4] * m[ 9]) + (m[ 0] * m[ 7] * m[ 9]) + (m[ 1] * m[ 4] * m[11]) - (m[ 0] * m[ 5] * m[11]);",
					"$cType p30 = (m[ 6] * m[ 9] * m[12]) - (m[ 5] * m[10] * m[12]) - (m[ 6] * m[ 8] * m[13]) + (m[ 4] * m[10] * m[13]) + (m[ 5] * m[ 8] * m[14]) - (m[ 4] * m[ 9] * m[14]);",
					"$cType p31 = (m[ 1] * m[10] * m[12]) - (m[ 2] * m[ 9] * m[12]) + (m[ 2] * m[ 8] * m[13]) - (m[ 0] * m[10] * m[13]) - (m[ 1] * m[ 8] * m[14]) + (m[ 0] * m[ 9] * m[14]);",
					"$cType p32 = (m[ 2] * m[ 5] * m[12]) - (m[ 1] * m[ 6] * m[12]) - (m[ 2] * m[ 4] * m[13]) + (m[ 0] * m[ 6] * m[13]) + (m[ 1] * m[ 4] * m[14]) - (m[ 0] * m[ 5] * m[14]);",
					"$cType p33 = (m[ 1] * m[ 6] * m[ 8]) - (m[ 2] * m[ 5] * m[ 8]) + (m[ 2] * m[ 4] * m[ 9]) - (m[ 0] * m[ 6] * m[ 9]) - (m[ 1] * m[ 4] * m[10]) + (m[ 0] * m[ 5] * m[10]);",
					"$cType det = (m[0] * p00) + (m[1] * p10) + (m[2] * p20) + (m[3] * p30);",
					"if(det != 0) {",
						"$cType rdet = 1.0$f / det;",
						"res_ptr[ 0] = p00 * rdet;",
						"res_ptr[ 1] = p01 * rdet;",
						"res_ptr[ 2] = p02 * rdet;",
						"res_ptr[ 3] = p03 * rdet;",
						"res_ptr[ 4] = p10 * rdet;",
						"res_ptr[ 5] = p11 * rdet;",
						"res_ptr[ 6] = p12 * rdet;",
						"res_ptr[ 7] = p13 * rdet;",
						"res_ptr[ 8] = p20 * rdet;",
						"res_ptr[ 9] = p21 * rdet;",
						"res_ptr[10] = p22 * rdet;",
						"res_ptr[11] = p23 * rdet;",
						"res_ptr[12] = p30 * rdet;",
						"res_ptr[13] = p31 * rdet;",
						"res_ptr[14] = p32 * rdet;",
						"res_ptr[15] = p33 * rdet;",
					"} else {",
						"uint32_t i;",
						"for(i = 0; i < 16; i++) {",
							"res_ptr[i] = ($cType) NAN;",
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
	
	public function getSIMDExpression() {
		$type = $this->getOperandType(1);
		if($this->operandSize == "variable") {
			throw Exception("No implementation for matrices of arbituary size");
		} else {
			return "qb_invert_matrix_{$this->operandSize}x{$this->operandSize}_$type(op1_ptr, res_ptr);";
		}
	}
}

?>