<?php

class QBColumnMajorTransposeMatrixHandler extends QBSIMDHandler {

	public function getOperandAddressMode($i) {
		return "ARR";
	}
	
	public function getHelperFunctions() {
		$type = $this->getOperandType(1);
		$cType = $this->getOperandCType(1);
		$functions = array(
			array(
				"static void ZEND_FASTCALL qb_transpose_cm_matrix_2x2_$type($cType *op1_start, $cType *op1_end, $cType *res_start, $cType *res_end) {",
					"$cType *__restrict res_ptr = res_start;",
					"$cType *__restrict op1_ptr = op1_start;",
					"for(;;) {",
						"$cType m00 = op1_ptr[0 * 2 + 0];",
						"$cType m10 = op1_ptr[0 * 2 + 1];",
						"$cType m01 = op1_ptr[1 * 2 + 0];",
						"$cType m11 = op1_ptr[1 * 2 + 1];",
						"res_ptr[0 * 2 + 0] = m00;",
						"res_ptr[0 * 2 + 1] = m01;",
						"res_ptr[1 * 2 + 0] = m10;",
						"res_ptr[1 * 2 + 1] = m11;",
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
				"static void ZEND_FASTCALL qb_transpose_cm_matrix_3x3_$type($cType *op1_start, $cType *op1_end, $cType *res_start, $cType *res_end) {",
					"$cType *__restrict res_ptr = res_start;",
					"$cType *__restrict op1_ptr = op1_start;",
					"for(;;) {",
						"$cType m00 = op1_ptr[0 * 3 + 0];",
						"$cType m10 = op1_ptr[0 * 3 + 1];",
						"$cType m20 = op1_ptr[0 * 3 + 2];",
						"$cType m01 = op1_ptr[1 * 3 + 0];",
						"$cType m11 = op1_ptr[1 * 3 + 1];",
						"$cType m21 = op1_ptr[1 * 3 + 2];",
						"$cType m02 = op1_ptr[2 * 3 + 0];",
						"$cType m12 = op1_ptr[2 * 3 + 1];",
						"$cType m22 = op1_ptr[2 * 3 + 2];",
						"res_ptr[0 * 3 + 0] = m00;",
						"res_ptr[0 * 3 + 1] = m01;",
						"res_ptr[0 * 3 + 2] = m02;",
						"res_ptr[1 * 3 + 0] = m10;",
						"res_ptr[1 * 3 + 1] = m11;",
						"res_ptr[1 * 3 + 2] = m12;",
						"res_ptr[2 * 3 + 0] = m20;",
						"res_ptr[2 * 3 + 1] = m21;",
						"res_ptr[2 * 3 + 2] = m22;",
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
				"static void ZEND_FASTCALL qb_transpose_cm_matrix_4x4_$type($cType *op1_start, $cType *op1_end, $cType *res_start, $cType *res_end) {",
					"$cType *__restrict res_ptr = res_start;",
					"$cType *__restrict op1_ptr = op1_start;",
					"for(;;) {",
						"$cType m00 = op1_ptr[0 * 4 + 0];",
						"$cType m10 = op1_ptr[0 * 4 + 1];",
						"$cType m20 = op1_ptr[0 * 4 + 2];",
						"$cType m30 = op1_ptr[0 * 4 + 3];",
						"$cType m01 = op1_ptr[1 * 4 + 0];",
						"$cType m11 = op1_ptr[1 * 4 + 1];",
						"$cType m21 = op1_ptr[1 * 4 + 2];",
						"$cType m31 = op1_ptr[1 * 4 + 3];",
						"$cType m02 = op1_ptr[2 * 4 + 0];",
						"$cType m12 = op1_ptr[2 * 4 + 1];",
						"$cType m22 = op1_ptr[2 * 4 + 2];",
						"$cType m32 = op1_ptr[2 * 4 + 3];",
						"$cType m03 = op1_ptr[3 * 4 + 0];",
						"$cType m13 = op1_ptr[3 * 4 + 1];",
						"$cType m23 = op1_ptr[3 * 4 + 2];",
						"$cType m33 = op1_ptr[3 * 4 + 3];",
						"res_ptr[0 * 4 + 0] = m00;",
						"res_ptr[0 * 4 + 1] = m01;",
						"res_ptr[0 * 4 + 2] = m02;",
						"res_ptr[0 * 4 + 3] = m03;",
						"res_ptr[1 * 4 + 0] = m10;",
						"res_ptr[1 * 4 + 1] = m11;",
						"res_ptr[1 * 4 + 2] = m12;",
						"res_ptr[1 * 4 + 3] = m13;",
						"res_ptr[2 * 4 + 0] = m20;",
						"res_ptr[2 * 4 + 1] = m21;",
						"res_ptr[2 * 4 + 2] = m22;",
						"res_ptr[2 * 4 + 3] = m23;",
						"res_ptr[3 * 4 + 0] = m30;",
						"res_ptr[3 * 4 + 1] = m31;",
						"res_ptr[3 * 4 + 2] = m32;",
						"res_ptr[3 * 4 + 3] = m33;",
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
				return "qb_transpose_cm_matrix_{$this->operandSize}x{$this->operandSize}_$type(op1_start, op1_end, res_start, res_end);";
			} else {
				return "qb_transpose_cm_matrix_{$this->operandSize}x{$this->operandSize}_$type(op1_ptr, NULL, res_ptr, NULL);";
			}
		}
	}
}

?>