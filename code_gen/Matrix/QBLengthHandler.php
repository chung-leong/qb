<?php

class QBLengthHandler extends QBSIMDHandler {

	public function getOperandAddressMode($i) {
		switch($i) {
			case 1: return "ARR";
			case 2: return $this->addressMode;
		}
	}

	public function getHelperFunctions() {
		$type = $this->getOperandType(1);
		$cType = $this->getOperandCType(1);
		$f = ($type == 'F32') ? 'f' : '';
		$functions = array(
			array(
				"void ZEND_FASTCALL qb_calculate_array_length_2x_$type($cType *op1_start, $cType *op1_end, $cType *res_start, $cType *res_end) {",
					"$cType *__restrict res_ptr = res_start;",
					"$cType *__restrict op1_ptr = op1_start;",
					"for(;;) {",
						"$cType sum = (op1_ptr[0] * op1_ptr[0]) + (op1_ptr[1] * op1_ptr[1]);",
						"res_ptr[0] = sqrt$f(sum);",
						"res_ptr += 1;",
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
				"void ZEND_FASTCALL qb_calculate_array_length_3x_$type($cType *op1_start, $cType *op1_end, $cType *res_start, $cType *res_end) {",
					"$cType *__restrict res_ptr = res_start;",
					"$cType *__restrict op1_ptr = op1_start;",
					"for(;;) {",
						"$cType sum = (op1_ptr[0] * op1_ptr[0]) + (op1_ptr[1] * op1_ptr[1]) + (op1_ptr[2] * op1_ptr[2]);",
						"res_ptr[0] = sqrt$f(sum);",
						"res_ptr += 1;",
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
				"void ZEND_FASTCALL qb_calculate_array_length_4x_$type($cType *op1_start, $cType *op1_end, $cType *res_start, $cType *res_end) {",
					"$cType *__restrict res_ptr = res_start;",
					"$cType *__restrict op1_ptr = op1_start;",
					"for(;;) {",
						"$cType sum = (op1_ptr[0] * op1_ptr[0]) + (op1_ptr[1] * op1_ptr[1]) + (op1_ptr[2] * op1_ptr[2]) + (op1_ptr[3] * op1_ptr[3]);",
						"res_ptr[0] = sqrt$f(sum);",
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
				"void ZEND_FASTCALL qb_calculate_array_length_$type($cType *op1_start, $cType *op1_end, uint32_t dim, $cType *res_start, $cType *res_end) {",
					"$cType *__restrict res_ptr = res_start;",
					"$cType *__restrict op1_ptr = op1_start;",
					"for(;;) {",
						"uint32_t i;",
						"$cType sum = 0;",
						"for(i = 0; i < dim; i++) {",
							"sum += op1_ptr[i] * op1_ptr[i];",
						"}",
						"res_ptr[0] = sqrt$f(sum);",
						"res_ptr += 1;",
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
	
	public function getResultSizePossibilities() {
		if($this->addressMode == "ARR") {
			return "vector_count";
		}
	}
	
	public function getResultSizeCalculation() {
		if($this->addressMode == "ARR") {
			$vectorSize = $this->getOperandSize(1);
			return "vector_count = op1_count / $vectorSize;";
		}
	}
	
	public function getOperandSize($i) {
		if($i == 2) {
			return 1;
		} else {
			return parent::getOperandSize($i);
		}
	}
	
	public function getAction() {
		$type = $this->getOperandType(1);
		if($this->operandSize == "variable") {
			if($this->addressMode == "ARR") {
				return "qb_calculate_array_length_$type(op1_ptr, op1_ptr + op1_count, MATRIX2_ROWS, res_ptr, res_ptr + res_count);";
			} else {
				return "qb_calculate_array_length_$type(op1_ptr, NULL, MATRIX2_ROWS, res_ptr, NULL);";
			}
		} else {
			if($this->addressMode == "ARR") {
				return "qb_calculate_array_length_{$this->operandSize}x_$type(op1_ptr, op1_ptr + op1_count, res_ptr, res_ptr + res_count);";
			} else {
				return "qb_calculate_array_length_{$this->operandSize}x_$type(op1_ptr, NULL, res_ptr, NULL);";
			}
		}
	}
}

?>