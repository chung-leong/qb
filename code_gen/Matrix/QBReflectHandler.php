<?php

class QBReflectHandler extends QBSIMDHandler {

	public function getHelperFunctions() {
		$cType = $this->getOperandCType(1);
		$type = $this->getOperandType(1);
		$functions = array(
			array(
				"static void ZEND_FASTCALL qb_calculate_reflect_1x_$type($cType *v1, $cType *v2, $cType *res_ptr) {",
					"$cType dot_product = qb_calculate_dot_product_1x_$type(v1, v2);",
					"res_ptr[0] = ($cType) (v1[0] - 2.0 * dot_product * v2[0]);",
				"}",
			),
			array(
				"static void ZEND_FASTCALL qb_calculate_reflect_2x_$type($cType *v1, $cType *v2, $cType *res_ptr) {",
					"$cType dot_product = qb_calculate_dot_product_2x_$type(v1, v2);",
					"res_ptr[0] = ($cType) (v1[0] - 2.0 * dot_product * v2[0]);",
					"res_ptr[1] = ($cType) (v1[1] - 2.0 * dot_product * v2[1]);",
				"}",
			),
			array(
				"static void ZEND_FASTCALL qb_calculate_reflect_3x_$type($cType *v1, $cType *v2, $cType *res_ptr) {",
					"$cType dot_product = qb_calculate_dot_product_3x_$type(v1, v2);",
					"res_ptr[0] = ($cType) (v1[0] - 2.0 * dot_product * v2[0]);",
					"res_ptr[1] = ($cType) (v1[1] - 2.0 * dot_product * v2[1]);",
					"res_ptr[2] = ($cType) (v1[2] - 2.0 * dot_product * v2[2]);",
				"}",
			),
			array(
				"static void ZEND_FASTCALL qb_calculate_reflect_4x_$type($cType *v1, $cType *v2, $cType *res_ptr) {",
					"$cType dot_product = qb_calculate_dot_product_4x_$type(v1, v2);",
					"res_ptr[0] = ($cType) (v1[0] - 2.0 * dot_product * v2[0]);",
					"res_ptr[1] = ($cType) (v1[1] - 2.0 * dot_product * v2[1]);",
					"res_ptr[2] = ($cType) (v1[2] - 2.0 * dot_product * v2[2]);",
					"res_ptr[3] = ($cType) (v1[3] - 2.0 * dot_product * v2[3]);",
				"}",
			),
			array(
				"static void ZEND_FASTCALL qb_calculate_reflect_$type($cType *v1, $cType *v2, $cType *res_ptr, uint32_t size) {",
					"$cType dot_product = qb_calculate_dot_product_$type(v1, v2, size);",
					"uint32_t i;",
					"for(i = 0; i < size; i++) {",
						"res_ptr[i] = ($cType) (v1[i] - 2.0 * dot_product * v2[i]);",
					"}",
				"}",
			),
		);
		return $functions;
	}
	
	public function getSIMDExpression() {
		$type = $this->getOperandType(1);
		if($this->operandSize == "variable") {
			return "qb_calculate_face_forward_$type(op1_ptr, op2_ptr, res_ptr, VECTOR_SIZE);";
		} else {
			return "qb_calculate_face_forward_{$this->operandSize}x_$type(op1_ptr, op2_ptr, res_ptr);";
		}
	}
}

?>