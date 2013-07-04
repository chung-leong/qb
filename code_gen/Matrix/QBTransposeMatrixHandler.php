<?php

class QBTransposeMatrixHandler extends QBSIMDHandler {

	public function getOperandAddressMode($i) {
		return "ARR";
	}
	
	public function getHelperFunctions() {
		$type = $this->getOperandType(1);
		$cType = $this->getOperandCType(1);
		$functions = array(
			array(
				"static void ZEND_FASTCALL qb_transpose_matrix_2x2_$type($cType *op1_start, $cType *op1_end, $cType *res_start, $cType *res_end) {",
					"$cType *__restrict res_ptr = res_start;",
					"$cType *__restrict op1_ptr = op1_start;",
					"for(;;) {",
						"$cType m00 = op1_ptr[0];",
						"$cType m10 = op1_ptr[1];",
						"$cType m01 = op1_ptr[2];",
						"$cType m11 = op1_ptr[3];",
						"res_ptr[0] = m00;",
						"res_ptr[1] = m01;",
						"res_ptr[2] = m10;",
						"res_ptr[3] = m11;",
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
				"static void ZEND_FASTCALL qb_transpose_matrix_3x3_$type($cType *op1_start, $cType *op1_end, $cType *res_start, $cType *res_end) {",
					"$cType *__restrict res_ptr = res_start;",
					"$cType *__restrict op1_ptr = op1_start;",
					"for(;;) {",
						"$cType m00 = op1_ptr[0];",
						"$cType m10 = op1_ptr[1];",
						"$cType m20 = op1_ptr[2];",
						"$cType m01 = op1_ptr[3];",
						"$cType m11 = op1_ptr[4];",
						"$cType m21 = op1_ptr[5];",
						"$cType m02 = op1_ptr[6];",
						"$cType m12 = op1_ptr[7];",
						"$cType m22 = op1_ptr[8];",
						"res_ptr[0] = m00;",
						"res_ptr[1] = m01;",
						"res_ptr[2] = m02;",
						"res_ptr[3] = m10;",
						"res_ptr[4] = m11;",
						"res_ptr[5] = m12;",
						"res_ptr[6] = m20;",
						"res_ptr[7] = m21;",
						"res_ptr[8] = m22;",
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
				"static void ZEND_FASTCALL qb_transpose_matrix_4x4_$type($cType *op1_start, $cType *op1_end, $cType *res_start, $cType *res_end) {",
					"$cType *__restrict res_ptr = res_start;",
					"$cType *__restrict op1_ptr = op1_start;",
					"for(;;) {",
						"$cType m00 = op1_ptr[ 0];",
						"$cType m10 = op1_ptr[ 1];",
						"$cType m20 = op1_ptr[ 2];",
						"$cType m30 = op1_ptr[ 3];",
						"$cType m01 = op1_ptr[ 4];",
						"$cType m11 = op1_ptr[ 5];",
						"$cType m21 = op1_ptr[ 6];",
						"$cType m31 = op1_ptr[ 7];",
						"$cType m02 = op1_ptr[ 8];",
						"$cType m12 = op1_ptr[ 9];",
						"$cType m22 = op1_ptr[10];",
						"$cType m32 = op1_ptr[11];",
						"$cType m03 = op1_ptr[12];",
						"$cType m13 = op1_ptr[13];",
						"$cType m23 = op1_ptr[14];",
						"$cType m33 = op1_ptr[15];",
						"res_ptr[ 0] = m00;",
						"res_ptr[ 1] = m01;",
						"res_ptr[ 2] = m02;",
						"res_ptr[ 3] = m03;",
						"res_ptr[ 4] = m10;",
						"res_ptr[ 5] = m11;",
						"res_ptr[ 6] = m12;",
						"res_ptr[ 7] = m13;",
						"res_ptr[ 8] = m20;",
						"res_ptr[ 9] = m21;",
						"res_ptr[10] = m22;",
						"res_ptr[11] = m23;",
						"res_ptr[12] = m30;",
						"res_ptr[13] = m31;",
						"res_ptr[14] = m32;",
						"res_ptr[15] = m33;",
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
				return "qb_transpose_matrix_{$this->operandSize}x{$this->operandSize}_$type(op1_start, op1_end, res_start, res_end);";
			} else {
				return "qb_transpose_matrix_{$this->operandSize}x{$this->operandSize}_$type(op1_ptr, NULL, res_ptr, NULL);";
			}
		}
	}
}

?>