<?php

class QBFaceForwardHandler extends QBSIMDHandler {

	public function getHelperFunctions() {
		$cType = $this->getOperandCType(1);
		$type = $this->getOperandType(1);
		$functions = array(
			array(
				"static $cType ZEND_FASTCALL qb_calculate_face_forward_1x_$type($cType *v1, $cType *v2, $cType *res_ptr) {",
					"$cType dot_product = qb_calculate_dot_product_1x_$type(op1_ptr, op2_ptr);",
					"if(dot_product > 0) {",
						"res_ptr[0] = op1_ptr[0];",
					"} else {",
						"res_ptr[0] = -op1_ptr[0];",
					"}",
				"}",
			),
			array(
				"static $cType ZEND_FASTCALL qb_calculate_face_forward_2x_$type($cType *v1, $cType *v2, $cType *res_ptr) {",
					"$cType dot_product = qb_calculate_dot_product_2x_$type(op1_ptr, op2_ptr);",
					"if(dot_product > 0) {",
						"res_ptr[0] = op1_ptr[0];",
						"res_ptr[1] = op1_ptr[1];",
					"} else {",
						"res_ptr[0] = -op1_ptr[0];",
						"res_ptr[1] = -op1_ptr[1];",
					"}",
				"}",
			),
			array(
				"static $cType ZEND_FASTCALL qb_calculate_face_forward_3x_$type($cType *v1, $cType *v2, $cType *res_ptr) {",
					"$cType dot_product = qb_calculate_dot_product_3x_$type(op1_ptr, op2_ptr);",
					"if(dot_product > 0) {",
						"res_ptr[0] = op1_ptr[0];",
						"res_ptr[1] = op1_ptr[1];",
						"res_ptr[2] = op1_ptr[2];",
					"} else {",
						"res_ptr[0] = -op1_ptr[0];",
						"res_ptr[1] = -op1_ptr[1];",
						"res_ptr[2] = -op1_ptr[2];",
					"}",
				"}",
			),
			array(
				"static $cType ZEND_FASTCALL qb_calculate_face_forward_4x_$type($cType *v1, $cType *v2, $cType *res_ptr) {",
					"$cType dot_product = qb_calculate_dot_product_4x_$type(op1_ptr, op2_ptr);",
					"if(dot_product > 0) {",
						"res_ptr[0] = op1_ptr[0];",
						"res_ptr[1] = op1_ptr[1];",
						"res_ptr[2] = op1_ptr[2];",
						"res_ptr[3] = op1_ptr[3];",
					"} else {",
						"res_ptr[0] = -op1_ptr[0];",
						"res_ptr[1] = -op1_ptr[1];",
						"res_ptr[2] = -op1_ptr[2];",
						"res_ptr[3] = -op1_ptr[3];",
					"}",
				"}",
			),
			array(
				"static $cType ZEND_FASTCALL qb_calculate_face_forward_$type($cType *v1, $cType *v2, $cType *res_ptr, uint32_t size) {",
					"$cType dot_product = qb_calculate_dot_product_$type(op1_ptr, op2_ptr, size);",
					"uint32_t i;",
					"if(dot_product > 0) {",
						"for(i = 0; i < size; i++) {",
							"res_ptr[i] = op1_ptr[i];",
						"}",
					"} else {",
						"for(i = 0; i < size; i++) {",
							"res_ptr[i] = -op1_ptr[i];",
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
			return "qb_calculate_face_forward_$type(op1_ptr, op2_ptr, res_ptr, VECTOR_SIZE);";
		} else {
			return "qb_calculate_face_forward_{$this->operandSize}x_$type(op1_ptr, op2_ptr, res_ptr);";
		}
	}
}

?>