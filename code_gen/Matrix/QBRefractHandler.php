<?php

class QBRefractHandler extends QBSIMDHandler {

	public function getInputOperandCount() {
		return 3;
	}

	public function getOperandAddressMode($i) {
		switch($i) {
			case 3: return "VAR";
			default: return "ARR";
		}
	}

	public function getHelperFunctions() {
		$cType = $this->getOperandCType(1);
		$type = $this->getOperandType(1);
		$f = ($type == 'F32') ? 'f' : '';
		$functions = array(
			array(
				"void ZEND_FASTCALL qb_calculate_refract_2x_$type($cType *op1_start, $cType *op1_end, $cType *op2_start, $cType *op2_end, $cType eta, $cType *res_start, $cType *res_end) {",
					"$cType *__restrict res_ptr = res_start;",
					"$cType *__restrict op1_ptr = op1_start;",
					"$cType *__restrict op2_ptr = op2_start;",
					"for(;;) {",
						"$cType dot_product, k;",
						"qb_calculate_dot_product_2x_$type(op1_ptr, NULL, op2_ptr, NULL, &dot_product, NULL);",
						"k = ($cType) (1.0 - (eta * eta) * (1.0 - dot_product * dot_product));",
						"if(k < 0.0) {",
							"res_ptr[0] = ",
							"res_ptr[1] = 0.0;",
						"} else {",
							"$cType m = eta * dot_product + sqrt$f(k);",
							"res_ptr[0] = eta * op1_ptr[0] - m * op2_ptr[0];",
							"res_ptr[1] = eta * op1_ptr[1] - m * op2_ptr[1];",
						"}",
						"res_ptr += 2;",
						"if(res_ptr >= res_end) {",
							"break;",
						"}",
						"op1_ptr += 2;",
						"if(op1_ptr >= op1_end) {",
							"op1_ptr = op1_start;",
						"}",
						"op2_ptr += 2;",
						"if(op2_ptr >= op2_end) {",
							"op2_ptr = op2_start;",
						"}",						
					"}",
				"}",
			),
			array(
				"void ZEND_FASTCALL qb_calculate_refract_3x_$type($cType *op1_start, $cType *op1_end, $cType *op2_start, $cType *op2_end, $cType eta, $cType *res_start, $cType *res_end) {",
					"$cType *__restrict res_ptr = res_start;",
					"$cType *__restrict op1_ptr = op1_start;",
					"$cType *__restrict op2_ptr = op2_start;",
					"for(;;) {",
						"$cType dot_product, k;",
						"qb_calculate_dot_product_3x_$type(op1_ptr, NULL, op2_ptr, NULL, &dot_product, NULL);",
						"k = ($cType) (1.0 - (eta * eta) * (1.0 - dot_product * dot_product));",
						"if(k < 0.0) {",
							"res_ptr[0] = ",
							"res_ptr[1] = ",
							"res_ptr[2] = 0.0;",
						"} else {",
							"$cType m = eta * dot_product + sqrt$f(k);",
							"res_ptr[0] = eta * op1_ptr[0] - m * op2_ptr[0];",
							"res_ptr[1] = eta * op1_ptr[1] - m * op2_ptr[1];",
							"res_ptr[2] = eta * op1_ptr[2] - m * op2_ptr[2];",
						"}",
						"res_ptr += 3;",
						"if(res_ptr >= res_end) {",
							"break;",
						"}",
						"op1_ptr += 3;",
						"if(op1_ptr >= op1_end) {",
							"op1_ptr = op1_start;",
						"}",
						"op2_ptr += 3;",
						"if(op2_ptr >= op2_end) {",
							"op2_ptr = op2_start;",
						"}",						
					"}",
				"}",
			),
			array(
				"void ZEND_FASTCALL qb_calculate_refract_4x_$type($cType *op1_start, $cType *op1_end, $cType *op2_start, $cType *op2_end, $cType eta, $cType *res_start, $cType *res_end) {",
					"$cType *__restrict res_ptr = res_start;",
					"$cType *__restrict op1_ptr = op1_start;",
					"$cType *__restrict op2_ptr = op2_start;",
					"for(;;) {",
						"$cType dot_product, k;",
						"qb_calculate_dot_product_4x_$type(op1_ptr, NULL, op2_ptr, NULL, &dot_product, NULL);",
						"k = ($cType) (1.0 - (eta * eta) * (1.0 - dot_product * dot_product));",
						"if(k < 0.0) {",
							"res_ptr[0] = ",
							"res_ptr[1] = ",
							"res_ptr[2] = ",
							"res_ptr[3] = 0.0;",
						"} else {",
							"$cType m = eta * dot_product + sqrt$f(k);",
							"res_ptr[0] = eta * op1_ptr[0] - m * op2_ptr[0];",
							"res_ptr[1] = eta * op1_ptr[1] - m * op2_ptr[1];",
							"res_ptr[2] = eta * op1_ptr[2] - m * op2_ptr[2];",
							"res_ptr[3] = eta * op1_ptr[3] - m * op2_ptr[3];",
						"}",
						"res_ptr += 4;",
						"if(res_ptr >= res_end) {",
							"break;",
						"}",
						"op1_ptr += 4;",
						"if(op1_ptr >= op1_end) {",
							"op1_ptr = op1_start;",
						"}",
						"op2_ptr += 4;",
						"if(op2_ptr >= op2_end) {",
							"op2_ptr = op2_start;",
						"}",						
					"}",
				"}",
			),
			array(
				"void ZEND_FASTCALL qb_calculate_refract_$type($cType *op1_start, $cType *op1_end, $cType *op2_start, $cType *op2_end, uint32_t dim, $cType eta, $cType *res_start, $cType *res_end) {",
					"$cType *__restrict res_ptr = res_start;",
					"$cType *__restrict op1_ptr = op1_start;",
					"$cType *__restrict op2_ptr = op2_start;",
					"for(;;) {",
						"uint32_t i;",
						"$cType dot_product, k;",
						"qb_calculate_dot_product_$type(op1_ptr, NULL, op2_ptr, NULL, dim, &dot_product, NULL);",
						"k = ($cType) (1.0 - (eta * eta) * (1.0 - dot_product * dot_product));",
						"if(k < 0.0) {",
							"for(i = 0; i < dim; i++) {",
								"res_ptr[0] = 0.0;",
							"}",
						"} else {",
							"$cType m = eta * dot_product + sqrt$f(k);",
							"for(i = 0; i < dim; i++) {",
								"res_ptr[i] = eta * op1_ptr[i] - m * op2_ptr[i];",
							"}",
						"}",
						"res_ptr += dim;",
						"if(res_ptr >= res_end) {",
							"break;",
						"}",
						"op1_ptr += dim;",
						"if(op1_ptr >= op1_end) {",
							"op1_ptr = op1_start;",
						"}",
						"op2_ptr += dim;",
						"if(op2_ptr >= op2_end) {",
							"op2_ptr = op2_start;",
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
				return "qb_calculate_refract_$type(op1_ptr, op1_ptr + op1_count, op2_ptr, op2_ptr + op2_count, MATRIX2_ROWS, op3, res_ptr, res_ptr + res_count);";
			} else {
				return "qb_calculate_refract_$type(op1_ptr, NULL, op2_ptr, NULL, MATRIX2_ROWS, op3, res_ptr, NULL);";
			}
		} else {
			if($this->addressMode == "ARR") {
				return "qb_calculate_refract_{$this->operandSize}x_$type(op1_ptr, op1_ptr + op1_count, op2_ptr, op2_ptr + op2_count, op3, res_ptr, res_ptr + res_count);";
			} else {
				return "qb_calculate_refract_{$this->operandSize}x_$type(op1_ptr, NULL, op2_ptr, NULL, op3, res_ptr, NULL);";
			}
		}
		
	}
}

?>