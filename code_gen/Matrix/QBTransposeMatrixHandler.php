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
				"static void ZEND_FASTCALL qb_transpose_matrix_2x2_$type($cType *m, $cType *res_ptr) {",
					"$cType m00 = m[0];",
					"$cType m10 = m[1];",
					"$cType m01 = m[2];",
					"$cType m11 = m[3];",
					"res_ptr[0] = m00;",
					"res_ptr[1] = m01;",
					"res_ptr[2] = m10;",
					"res_ptr[3] = m11;",
				"}",
			),
			array(
				"static void ZEND_FASTCALL qb_transpose_matrix_3x3_$type($cType *m, $cType *res_ptr) {",
					"$cType m00 = m[0];",
					"$cType m10 = m[1];",
					"$cType m20 = m[2];",
					"$cType m01 = m[3];",
					"$cType m11 = m[4];",
					"$cType m21 = m[5];",
					"$cType m02 = m[6];",
					"$cType m12 = m[7];",
					"$cType m22 = m[8];",
					"res_ptr[0] = m00;",
					"res_ptr[1] = m01;",
					"res_ptr[2] = m02;",
					"res_ptr[3] = m10;",
					"res_ptr[4] = m11;",
					"res_ptr[5] = m12;",
					"res_ptr[6] = m20;",
					"res_ptr[7] = m21;",
					"res_ptr[8] = m22;",
				"}",
			),
			array(
				"static void ZEND_FASTCALL qb_transpose_matrix_4x4_$type($cType *m, $cType *res_ptr) {",
					"$cType m00 = m[ 0];",
					"$cType m10 = m[ 1];",
					"$cType m20 = m[ 2];",
					"$cType m30 = m[ 3];",
					"$cType m01 = m[ 4];",
					"$cType m11 = m[ 5];",
					"$cType m21 = m[ 6];",
					"$cType m31 = m[ 7];",
					"$cType m02 = m[ 8];",
					"$cType m12 = m[ 9];",
					"$cType m22 = m[10];",
					"$cType m32 = m[11];",
					"$cType m03 = m[12];",
					"$cType m13 = m[13];",
					"$cType m23 = m[14];",
					"$cType m33 = m[15];",
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
			return "qb_transpose_matrix_{$this->operandSize}x{$this->operandSize}_$type(op1_ptr, res_ptr);";
		}
	}
}

?>