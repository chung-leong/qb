<?php

class QBReflectHandler extends QBSIMDHandler {

	public function getInputOperandCount() {
		return 2;
	}

	public function getHelperFunctions() {
		$cType = $this->getOperandCType(1);
		$type = $this->getOperandType(1);
		$functions = array(
			array(
				"void ZEND_FASTCALL qb_calculate_reflect_2x_$type($cType *op1_start, $cType *op1_end, $cType *op2_start, $cType *op2_end, $cType *res_start, $cType *res_end) {",
					"$cType *__restrict res_ptr = res_start;",
					"$cType *__restrict op1_ptr = op1_start;",
					"$cType *__restrict op2_ptr = op2_start;",
					"for(;;) {",
						"$cType dot_product;",
						"qb_calculate_dot_product_2x_$type(op1_ptr, NULL, op2_ptr, NULL, &dot_product, NULL);",
						"res_ptr[0] = ($cType) (op1_ptr[0] - 2.0 * dot_product * op2_ptr[0]);",
						"res_ptr[1] = ($cType) (op1_ptr[1] - 2.0 * dot_product * op2_ptr[1]);",
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
				"void ZEND_FASTCALL qb_calculate_reflect_3x_$type($cType *op1_start, $cType *op1_end, $cType *op2_start, $cType *op2_end, $cType *res_start, $cType *res_end) {",
					"$cType *__restrict res_ptr = res_start;",
					"$cType *__restrict op1_ptr = op1_start;",
					"$cType *__restrict op2_ptr = op2_start;",
					"for(;;) {",
						"$cType dot_product;",
						"qb_calculate_dot_product_3x_$type(op1_ptr, NULL, op2_ptr, NULL, &dot_product, NULL);",
						"res_ptr[0] = ($cType) (op1_ptr[0] - 2.0 * dot_product * op2_ptr[0]);",
						"res_ptr[1] = ($cType) (op1_ptr[1] - 2.0 * dot_product * op2_ptr[1]);",
						"res_ptr[2] = ($cType) (op1_ptr[2] - 2.0 * dot_product * op2_ptr[2]);",
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
				"void ZEND_FASTCALL qb_calculate_reflect_4x_$type($cType *op1_start, $cType *op1_end, $cType *op2_start, $cType *op2_end, $cType *res_start, $cType *res_end) {",
					"$cType *__restrict res_ptr = res_start;",
					"$cType *__restrict op1_ptr = op1_start;",
					"$cType *__restrict op2_ptr = op2_start;",
					"for(;;) {",
						"$cType dot_product;",
						"qb_calculate_dot_product_4x_$type(op1_ptr, NULL, op2_ptr, NULL, &dot_product, NULL);",
						"res_ptr[0] = ($cType) (op1_ptr[0] - 2.0 * dot_product * op2_ptr[0]);",
						"res_ptr[1] = ($cType) (op1_ptr[1] - 2.0 * dot_product * op2_ptr[1]);",
						"res_ptr[2] = ($cType) (op1_ptr[2] - 2.0 * dot_product * op2_ptr[2]);",
						"res_ptr[3] = ($cType) (op1_ptr[3] - 2.0 * dot_product * op2_ptr[3]);",
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
				"void ZEND_FASTCALL qb_calculate_reflect_$type($cType *op1_start, $cType *op1_end, $cType *op2_start, $cType *op2_end, uint32_t dim, $cType *res_start, $cType *res_end) {",
					"$cType *__restrict res_ptr = res_start;",
					"$cType *__restrict op1_ptr = op1_start;",
					"$cType *__restrict op2_ptr = op2_start;",
					"for(;;) {",
						"uint32_t i;",
						"$cType dot_product;",
						"qb_calculate_dot_product_$type(op1_ptr, NULL, op2_ptr, NULL, dim, &dot_product, NULL);",
						"for(i = 0; i < dim; i++) {",
							"res_ptr[i] = ($cType) (op1_ptr[i] - 2.0 * dot_product * op2_ptr[i]);",
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
				return "qb_calculate_reflect_$type(op1_ptr, op1_ptr + op1_count, op2_ptr, op2_ptr + op2_count, MATRIX2_ROWS, res_ptr, res_ptr + res_count);";
			} else {
				return "qb_calculate_reflect_$type(op1_ptr, NULL, op2_ptr, NULL, MATRIX2_ROWS, res_ptr, NULL);";
			}
		} else {
			if($this->addressMode == "ARR") {
				return "qb_calculate_reflect_{$this->operandSize}x_$type(op1_ptr, op1_ptr + op1_count, op2_ptr, op2_ptr + op2_count, res_ptr, res_ptr + res_count);";
			} else {
				return "qb_calculate_reflect_{$this->operandSize}x_$type(op1_ptr, NULL, op2_ptr, NULL, res_ptr, NULL);";
			}
		}
	}
}

?>