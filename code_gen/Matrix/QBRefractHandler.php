<?php

class QBRefractHandler extends QBSIMDHandler {

	public function getOperandAddressMode($i) {
		switch($i) {
			case 3: return "VAR";
			default: return "ARR";
		}
	}

	public function getHelperFunctions() {
		$cType = $this->getOperandCType(1);
		$type = $this->getOperandType(1);
		$functions = array(
			array(
				"static void ZEND_FASTCALL qb_calculate_refract_2x_$type($cType *v1, $cType *v2, $cType eta, $cType *res_ptr) {",
					"$cType dot_product = qb_calculate_dot_product_2x_$type(v1, v2);",
					"$cType k = ($cType) (1.0 - (eta * eta) * (1.0 - dot_product * dot_product));",
					"if(k < 0.0) {",
						"res_ptr[0] = ",
						"res_ptr[1] = 0.0;",
					"} else {",
						"$cType m = eta * dot_product + ($cType) sqrt(k);",
						"res_ptr[0] = eta * v1[0] - m * v2[0];",
						"res_ptr[1] = eta * v1[1] - m * v2[1];",
					"}",
				"}",
			),
			array(
				"static void ZEND_FASTCALL qb_calculate_refract_3x_$type($cType *v1, $cType *v2, $cType eta, $cType *res_ptr) {",
					"$cType dot_product = qb_calculate_dot_product_3x_$type(v1, v2);",
					"$cType k = ($cType) (1.0 - (eta * eta) * (1.0 - dot_product * dot_product));",
					"if(k < 0.0) {",
						"res_ptr[0] = ",
						"res_ptr[1] = ",
						"res_ptr[2] = 0.0;",
					"} else {",
						"$cType m = eta * dot_product + ($cType) sqrt(k);",
						"res_ptr[0] = eta * v1[0] - m * v2[0];",
						"res_ptr[1] = eta * v1[1] - m * v2[1];",
						"res_ptr[2] = eta * v1[2] - m * v2[2];",
					"}",
				"}",
			),
			array(
				"static void ZEND_FASTCALL qb_calculate_refract_4x_$type($cType *v1, $cType *v2, $cType eta, $cType *res_ptr) {",
					"$cType dot_product = qb_calculate_dot_product_4x_$type(v1, v2);",
					"$cType k = ($cType) (1.0 - (eta * eta) * (1.0 - dot_product * dot_product));",
					"if(k < 0.0) {",
						"res_ptr[0] = ",
						"res_ptr[1] = ",
						"res_ptr[2] = ",
						"res_ptr[3] = 0.0;",
					"} else {",
						"$cType m = eta * dot_product + ($cType) sqrt(k);",
						"res_ptr[0] = eta * v1[0] - m * v2[0];",
						"res_ptr[1] = eta * v1[1] - m * v2[1];",
						"res_ptr[2] = eta * v1[2] - m * v2[2];",
						"res_ptr[3] = eta * v1[3] - m * v2[3];",
					"}",
				"}",
			),
			array(
				"static void ZEND_FASTCALL qb_calculate_refract_$type($cType *v1, $cType *v2, $cType eta, $cType *res_ptr, uint32_t size) {",
					"$cType dot_product = qb_calculate_dot_product_$type(v1, v2, size);",
					"$cType k = ($cType) (1.0 - (eta * eta) * (1.0 - dot_product * dot_product));",
					"uint32_t i;",
					"if(k < 0.0) {",
						"for(i = 0; i < size; i++) {",
							"res_ptr[0] = 0.0;",
						"}",
					"} else {",
						"$cType m = eta * dot_product + ($cType) sqrt(k);",
						"for(i = 0; i < size; i++) {",
							"res_ptr[i] = eta * v1[i] - m * v2[i];",
						"}",
					"}",
				"}",
			),
		);
		return $functions;
	}
	
	public function getSIMDExpression() {
		$type = $this->getOperandType(1);
		if($this->operandSize == "variable") {
			return "qb_calculate_refract_$type(op1_ptr, op2_ptr, op3, res_ptr, MATRIX2_ROWS);";
		} else {
			return "qb_calculate_refract_{$this->operandSize}x_$type(op1_ptr, op2_ptr, op3, res_ptr);";
		}
	}
}

?>