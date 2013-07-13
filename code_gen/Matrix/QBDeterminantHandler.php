<?php

class QBDeterminantHandler extends QBSIMDHandler {

	public function getOperandAddressMode($i) {
		switch($i) {
			case 1: return "ARR";
			case 2: return $this->addressMode;
		}
	}

	public function getHelperFunctions() {
		$type = $this->getOperandType(1);
		$cType = $this->getOperandCType(1);
		$functions = array(
			array(
				"static void ZEND_FASTCALL qb_calculate_cm_matrix_determinant_2x2_$type($cType *op1_start, $cType *op1_end, $cType *res_start, $cType *res_end) {",
					"$cType *__restrict res_ptr = res_start;",
					"$cType *__restrict op1_ptr = op1_start;",
					"for(;;) {",
						"res_ptr[0] = (op1_ptr[0 * 2 + 0] * op1_ptr[1 * 2 + 1]) - (op1_ptr[0 * 2 + 1] * op1_ptr[1 * 2 + 0]);",
						"res_ptr += 1;",
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
				"static void ZEND_FASTCALL qb_calculate_cm_matrix_determinant_3x3_$type($cType *op1_start, $cType *op1_end, $cType *res_start, $cType *res_end) {",
					"$cType *__restrict res_ptr = res_start;",
					"$cType *__restrict op1_ptr = op1_start;",
					"for(;;) {",
						"res_ptr[0] =	 (op1_ptr[0 * 3 + 0] * op1_ptr[1 * 3 + 1] * op1_ptr[2 * 3 + 2]) -",
										"(op1_ptr[0 * 3 + 0] * op1_ptr[1 * 3 + 2] * op1_ptr[2 * 3 + 1]) +",
										"(op1_ptr[0 * 3 + 1] * op1_ptr[1 * 3 + 2] * op1_ptr[2 * 3 + 0]) -",
										"(op1_ptr[0 * 3 + 1] * op1_ptr[1 * 3 + 0] * op1_ptr[2 * 3 + 2]) +",
										"(op1_ptr[0 * 3 + 2] * op1_ptr[1 * 3 + 0] * op1_ptr[2 * 3 + 1]) -",
										"(op1_ptr[0 * 3 + 2] * op1_ptr[1 * 3 + 1] * op1_ptr[2 * 3 + 0]);",
						"res_ptr += 1;",
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
				"static void ZEND_FASTCALL qb_calculate_cm_matrix_determinant_4x4_$type($cType *op1_start, $cType *op1_end, $cType *res_start, $cType *res_end) {",
					"$cType *__restrict res_ptr = res_start;",
					"$cType *__restrict op1_ptr = op1_start;",
					"for(;;) {",
						"res_ptr[0] = 	 ((op1_ptr[0 * 4 + 3] * op1_ptr[1 * 4 + 2]) * (op1_ptr[2 * 4 + 1] * op1_ptr[3 * 4 + 0])) - ((op1_ptr[0 * 4 + 2] * op1_ptr[1 * 4 + 3]) * (op1_ptr[2 * 4 + 1] * op1_ptr[3 * 4 + 0])) -",
										"((op1_ptr[0 * 4 + 3] * op1_ptr[1 * 4 + 1]) * (op1_ptr[2 * 4 + 2] * op1_ptr[3 * 4 + 0])) + ((op1_ptr[0 * 4 + 1] * op1_ptr[1 * 4 + 3]) * (op1_ptr[2 * 4 + 2] * op1_ptr[3 * 4 + 0])) +",
										"((op1_ptr[0 * 4 + 2] * op1_ptr[1 * 4 + 1]) * (op1_ptr[2 * 4 + 3] * op1_ptr[3 * 4 + 0])) - ((op1_ptr[0 * 4 + 1] * op1_ptr[1 * 4 + 2]) * (op1_ptr[2 * 4 + 3] * op1_ptr[3 * 4 + 0])) -",
										"((op1_ptr[0 * 4 + 3] * op1_ptr[1 * 4 + 2]) * (op1_ptr[2 * 4 + 0] * op1_ptr[3 * 4 + 1])) + ((op1_ptr[0 * 4 + 2] * op1_ptr[1 * 4 + 3]) * (op1_ptr[2 * 4 + 0] * op1_ptr[3 * 4 + 1])) +",
										"((op1_ptr[0 * 4 + 3] * op1_ptr[1 * 4 + 0]) * (op1_ptr[2 * 4 + 2] * op1_ptr[3 * 4 + 1])) - ((op1_ptr[0 * 4 + 0] * op1_ptr[1 * 4 + 3]) * (op1_ptr[2 * 4 + 2] * op1_ptr[3 * 4 + 1])) -",
										"((op1_ptr[0 * 4 + 2] * op1_ptr[1 * 4 + 0]) * (op1_ptr[2 * 4 + 3] * op1_ptr[3 * 4 + 1])) + ((op1_ptr[0 * 4 + 0] * op1_ptr[1 * 4 + 2]) * (op1_ptr[2 * 4 + 3] * op1_ptr[3 * 4 + 1])) +",
										"((op1_ptr[0 * 4 + 3] * op1_ptr[1 * 4 + 1]) * (op1_ptr[2 * 4 + 0] * op1_ptr[3 * 4 + 2])) - ((op1_ptr[0 * 4 + 1] * op1_ptr[1 * 4 + 3]) * (op1_ptr[2 * 4 + 0] * op1_ptr[3 * 4 + 2])) -",
										"((op1_ptr[0 * 4 + 3] * op1_ptr[1 * 4 + 0]) * (op1_ptr[2 * 4 + 1] * op1_ptr[3 * 4 + 2])) + ((op1_ptr[0 * 4 + 0] * op1_ptr[1 * 4 + 3]) * (op1_ptr[2 * 4 + 1] * op1_ptr[3 * 4 + 2])) +",
										"((op1_ptr[0 * 4 + 1] * op1_ptr[1 * 4 + 0]) * (op1_ptr[2 * 4 + 3] * op1_ptr[3 * 4 + 2])) - ((op1_ptr[0 * 4 + 0] * op1_ptr[1 * 4 + 1]) * (op1_ptr[2 * 4 + 3] * op1_ptr[3 * 4 + 2])) -",
										"((op1_ptr[0 * 4 + 2] * op1_ptr[1 * 4 + 1]) * (op1_ptr[2 * 4 + 0] * op1_ptr[3 * 4 + 3])) + ((op1_ptr[0 * 4 + 1] * op1_ptr[1 * 4 + 2]) * (op1_ptr[2 * 4 + 0] * op1_ptr[3 * 4 + 3])) +",
										"((op1_ptr[0 * 4 + 2] * op1_ptr[1 * 4 + 0]) * (op1_ptr[2 * 4 + 1] * op1_ptr[3 * 4 + 3])) - ((op1_ptr[0 * 4 + 0] * op1_ptr[1 * 4 + 2]) * (op1_ptr[2 * 4 + 1] * op1_ptr[3 * 4 + 3])) -",
										"((op1_ptr[0 * 4 + 1] * op1_ptr[1 * 4 + 0]) * (op1_ptr[2 * 4 + 2] * op1_ptr[3 * 4 + 3])) + ((op1_ptr[0 * 4 + 0] * op1_ptr[1 * 4 + 1]) * (op1_ptr[2 * 4 + 2] * op1_ptr[3 * 4 + 3]));",
						"res_ptr += 1;",
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
			array(
				"static void ZEND_FASTCALL qb_calculate_cm_matrix_determinant_$type($cType *op1_start, $cType *op1_end, uint32_t dim, $cType *res_start, $cType *res_end) {",
					"if(dim == 4) {",
						"qb_calculate_cm_matrix_determinant_4x4_$type(op1_start, op1_end, res_start, res_end);",
					"} else {",
						"ALLOCA_FLAG(use_heap)",
						"$cType *__restrict res_ptr = res_start;",
						"$cType *__restrict op1_ptr = op1_start;",						
						"$cType *__restrict minor = do_alloca((dim - 1) * (dim - 1) * sizeof($cType), use_heap);",
						"for(;;) {",
							"uint32_t i, j, k, m, n;",
							"$cType sign = 1, det = 0;",
							"for(m = 0; m < dim; m++) {",
								"$cType a = op1_ptr[m];",
								"$cType minor_det;",
								"for(i = 1, n = 0, k = dim; i < dim; i++) {",
									"for(j = 0; j < dim; j++, k++) {",
										"if(j != m) {",
											"minor[n] = op1_ptr[k];",
											"n++;",
										"}",
									"}",
								"}",
								"qb_calculate_cm_matrix_determinant_$type(minor, NULL, dim - 1, &minor_det, NULL);",
								"det += a * minor_det * sign;",
								"sign = -sign;",
							"}",
							"res_ptr[0] = det;",
							"res_ptr += 1;",
							"if(res_ptr >= res_end) {",
								"break;",
							"}",
							"op1_ptr += dim * dim;",
							"if(op1_ptr >= op1_end) {",
								"op1_ptr = op1_start;",
							"}",
						"}",
						"free_alloca(minor, use_heap);",
					"}",
				"}",
			),
		);
		return $functions;
	}
	
	public function getResultSizePossibilities() {
		if($this->addressMode == "ARR") {
			return "matrix1_count";
		}
	}
	
	public function getResultSizeCalculation() {
		if($this->addressMode == "ARR") {
			$matrixSize = $this->getOperandSize(1);
			return "matrix1_count = op1_count / $matrixSize;";
		}
	}
	
	public function getOperandSize($i) {
		if($i == 2) {
			return 1;
		} else {
			if($this->operandSize == "variable") {
				return "(MATRIX1_ROWS * MATRIX1_COLS)";
			} else {
				return $this->operandSize * $this->operandSize;
			}
		}
	}
	
	public function getAction() {
		$type = $this->getOperandType(1);
		if($this->operandSize == "variable") {
			if($this->addressMode == "ARR") {
				return "qb_calculate_cm_matrix_determinant_$type(op1_start, op1_end, MATRIX2_ROWS, res_start, res_end);";
			} else {
				return "qb_calculate_cm_matrix_determinant_$type(op1_ptr, NULL, MATRIX2_ROWS, res_ptr, NULL);";
			}
		} else {
			if($this->addressMode == "ARR") {
				return "qb_calculate_cm_matrix_determinant_{$this->operandSize}x{$this->operandSize}_$type(op1_start, op1_end, res_start, res_end);";
			} else {
				return "qb_calculate_cm_matrix_determinant_{$this->operandSize}x{$this->operandSize}_$type(op1_ptr, NULL, res_ptr, NULL);";
			}
		}
	}
}

?>