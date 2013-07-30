<?php

class QBNormalizeHandler extends QBSIMDHandler {

	public function getOperandAddressMode($i) {
		return "ARR";
	}

	public function getHelperFunctions() {
		$type = $this->getOperandType(1);
		$cType = $this->getOperandCType(1);
		$f = ($type == 'F32') ? 'f' : '';
		$functions = array(
			array(
				"void ZEND_FASTCALL qb_normalize_array_2x_$type($cType *op1_start, $cType *op1_end, $cType *res_start, $cType *res_end) {",
					"$cType *__restrict res_ptr = res_start;",
					"$cType *__restrict op1_ptr = op1_start;",
					"for(;;) {",
						"$cType sum = (op1_ptr[0] * op1_ptr[0]) + (op1_ptr[1] * op1_ptr[1]);",
						"$cType r = rsqrt$f(sum);",
						"res_ptr[0] = op1_ptr[0] * r;",
						"res_ptr[1] = op1_ptr[1] * r;",
						"res_ptr += 2;",
						"if(res_ptr >= res_end) {",
							"break;",
						"}",
						"op1_ptr += 2;",
						"if(op1_ptr >= op1_end) {",
							"op1_ptr = op1_start;",
						"}",
					"}",
				"}",
			),
			array(
				"void ZEND_FASTCALL qb_normalize_array_3x_$type($cType *op1_start, $cType *op1_end, $cType *res_start, $cType *res_end) {",
					"$cType *__restrict res_ptr = res_start;",
					"$cType *__restrict op1_ptr = op1_start;",
					"for(;;) {",
						"$cType sum = (op1_ptr[0] * op1_ptr[0]) + (op1_ptr[1] * op1_ptr[1]) + (op1_ptr[2] * op1_ptr[2]);",
						"$cType r = rsqrt$f(sum);",
						"res_ptr[0] = op1_ptr[0] * r;",
						"res_ptr[1] = op1_ptr[1] * r;",
						"res_ptr[2] = op1_ptr[2] * r;",
						"res_ptr += 3;",
						"if(res_ptr >= res_end) {",
							"break;",
						"}",
						"op1_ptr += 3;",
						"if(op1_ptr >= op1_end) {",
							"op1_ptr = op1_start;",
						"}",
					"}",
				"}",
			),
			array(
				"void ZEND_FASTCALL qb_normalize_array_4x_$type($cType *op1_start, $cType *op1_end, $cType *res_start, $cType *res_end) {",
					"$cType *__restrict res_ptr = res_start;",
					"$cType *__restrict op1_ptr = op1_start;",
					"for(;;) {",
						"$cType sum = (op1_ptr[0] * op1_ptr[0]) + (op1_ptr[1] * op1_ptr[1]) + (op1_ptr[2] * op1_ptr[2]) + (op1_ptr[3] * op1_ptr[3]);",
						"$cType r = rsqrt$f(sum);",
						"res_ptr[0] = op1_ptr[0] * r;",
						"res_ptr[1] = op1_ptr[1] * r;",
						"res_ptr[2] = op1_ptr[2] * r;",
						"res_ptr[3] = op1_ptr[3] * r;",
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
				"void ZEND_FASTCALL qb_normalize_array_$type($cType *op1_start, $cType *op1_end, uint32_t dim, $cType *res_start, $cType *res_end) {",
					"$cType *__restrict res_ptr = res_start;",
					"$cType *__restrict op1_ptr = op1_start;",
					"for(;;) {",
						"uint32_t i;",
						"$cType sum = 0, r;",
						"for(i = 0; i < dim; i++) {",
							"sum += op1_ptr[i] * op1_ptr[i];",
						"}",
						"r = rsqrt$f(sum);",
						"for(i = 0; i < dim; i++) {",
							"res_ptr[i] = op1_ptr[i] * r;",
						"}",
						"res_ptr += dim;",
						"if(res_ptr >= res_end) {",
							"break;",
						"}",
						"op1_ptr += dim;",
						"if(op1_ptr >= op1_end) {",
							"op1_ptr = op1_start;",
						"}",
					"}",
				"}",
			),
		);
		return $functions;
	}
	
	public function getAction() {
		$type = $this->getOperandType(1);
		if($this->operandSize == "variable") {
			if($this->addressMode == "ARR") {
				return "qb_normalize_array_$type(op1_ptr, op1_ptr + op1_count, MATRIX2_ROWS, res_ptr, res_ptr + res_count);";
			} else {
				return "qb_normalize_array_$type(op1_ptr, NULL, MATRIX2_ROWS, res_ptr, NULL);";
			}
		} else {
			if($this->addressMode == "ARR") {
				return "qb_normalize_array_{$this->operandSize}x_$type(op1_ptr, op1_ptr + op1_count, res_ptr, res_ptr + res_count);";
			} else {
				return "qb_normalize_array_{$this->operandSize}x_$type(op1_ptr, NULL, res_ptr, NULL);";
			}
		}
	}
}

?>