<?php

class QBNormalizeHandler extends QBSIMDHandler {

	public function getOperandAddressMode($i) {
		return "ARR";
	}

	public function getHelperFunctions() {
		$type = $this->getOperandType(1);
		$cType = $this->getOperandCType(1);
		$functions = array(
			array(
				"static void ZEND_FASTCALL qb_normalize_array_1x_$type($cType *v, $cType *res_ptr) {",
					"$cType len = qb_calculate_array_length_1x_$type(v);",
					"res_ptr[0] = v[0] / len;",
				"}",
			),
			array(
				"static void ZEND_FASTCALL qb_normalize_array_2x_$type($cType *v, $cType *res_ptr) {",
					"$cType len = qb_calculate_array_length_2x_$type(v);",
					"res_ptr[0] = v[0] / len;",
					"res_ptr[1] = v[1] / len;",
				"}",
			),
			array(
				"static void ZEND_FASTCALL qb_normalize_array_3x_$type($cType *v, $cType *res_ptr) {",
					"$cType len = qb_calculate_array_length_3x_$type(v);",
					"res_ptr[0] = v[0] / len;",
					"res_ptr[1] = v[1] / len;",
					"res_ptr[2] = v[2] / len;",
				"}",
			),
			array(
				"static void ZEND_FASTCALL qb_normalize_array_4x_$type($cType *v, $cType *res_ptr) {",
					"$cType len = qb_calculate_array_length_4x_$type(v);",
					"res_ptr[0] = v[0] / len;",
					"res_ptr[1] = v[1] / len;",
					"res_ptr[2] = v[2] / len;",
					"res_ptr[3] = v[3] / len;",
				"}",
			),
			array(
				"static void ZEND_FASTCALL qb_normalize_array_$type($cType *v, $cType *res_ptr, uint32_t size) {",
					"uint32_t i;",
					"$cType len = qb_calculate_array_length_$type(v, size);",
					"for(i = 0; i < size; i++) {",
						"res_ptr[i] = v[i] / len;",
					"}",
				"}",
			),
		);
		return $functions;
	}
	
	public function getSIMDExpression() {
		$type = $this->getOperandType(1);
		if($this->operandSize == "variable") {
			return "qb_normalize_array_$type(op1_ptr, res_ptr, MATRIX2_ROWS);";
		} else {
			return "qb_normalize_array_{$this->operandSize}x_$type(op1_ptr, res_ptr);";
		}
	}
}

?>