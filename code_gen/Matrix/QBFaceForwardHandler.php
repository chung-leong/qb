<?php

class QBFaceForwardHandler extends QBSIMDHandler {

	public function getHelperFunctions() {
		$cType = $this->getOperandCType(1);
		$type = $this->getOperandType(1);
		$functions = array(
			array(
				"static void ZEND_FASTCALL qb_calculate_face_forward_1x_$type($cType *v1, $cType *v2, $cType *res_ptr) {",
					"$cType dot_product = qb_calculate_dot_product_1x_$type(v1, v2);",
					"if(dot_product > 0) {",
						"res_ptr[0] = v1[0];",
					"} else {",
						"res_ptr[0] = -v1[0];",
					"}",
				"}",
			),
			array(
				"static void ZEND_FASTCALL qb_calculate_face_forward_2x_$type($cType *v1, $cType *v2, $cType *res_ptr) {",
					"$cType dot_product = qb_calculate_dot_product_2x_$type(v1, v2);",
					"if(dot_product > 0) {",
						"res_ptr[0] = v1[0];",
						"res_ptr[1] = v1[1];",
					"} else {",
						"res_ptr[0] = -v1[0];",
						"res_ptr[1] = -v1[1];",
					"}",
				"}",
			),
			array(
				"static void ZEND_FASTCALL qb_calculate_face_forward_3x_$type($cType *v1, $cType *v2, $cType *res_ptr) {",
					"$cType dot_product = qb_calculate_dot_product_3x_$type(v1, v2);",
					"if(dot_product > 0) {",
						"res_ptr[0] = v1[0];",
						"res_ptr[1] = v1[1];",
						"res_ptr[2] = v1[2];",
					"} else {",
						"res_ptr[0] = -v1[0];",
						"res_ptr[1] = -v1[1];",
						"res_ptr[2] = -v1[2];",
					"}",
				"}",
			),
			array(
				"static void ZEND_FASTCALL qb_calculate_face_forward_4x_$type($cType *v1, $cType *v2, $cType *res_ptr) {",
					"$cType dot_product = qb_calculate_dot_product_4x_$type(v1, v2);",
					"if(dot_product > 0) {",
						"res_ptr[0] = v1[0];",
						"res_ptr[1] = v1[1];",
						"res_ptr[2] = v1[2];",
						"res_ptr[3] = v1[3];",
					"} else {",
						"res_ptr[0] = -v1[0];",
						"res_ptr[1] = -v1[1];",
						"res_ptr[2] = -v1[2];",
						"res_ptr[3] = -v1[3];",
					"}",
				"}",
			),
			array(
				"static void ZEND_FASTCALL qb_calculate_face_forward_$type($cType *v1, $cType *v2, $cType *res_ptr, uint32_t size) {",
					"$cType dot_product = qb_calculate_dot_product_$type(v1, v2, size);",
					"uint32_t i;",
					"if(dot_product > 0) {",
						"for(i = 0; i < size; i++) {",
							"res_ptr[i] = v1[i];",
						"}",
					"} else {",
						"for(i = 0; i < size; i++) {",
							"res_ptr[i] = -v1[i];",
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
			return "qb_calculate_face_forward_$type(op1_ptr, op2_ptr, res_ptr, MATRIX2_ROWS);";
		} else {
			return "qb_calculate_face_forward_{$this->operandSize}x_$type(op1_ptr, op2_ptr, res_ptr);";
		}
	}
}

?>