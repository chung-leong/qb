<?php

class QBSampleNearestHandler extends QBSampleHandler {

	public function getHelperFunctions() {
		$type = $this->getOperandType(1);
		$cType = $this->getOperandCType(1);
		$functions = array(
			array(
				"static void ZEND_FASTCALL qb_sample_nearest_3x_$type($cType *__restrict pixels, uint32_t width, uint32_t height, $cType x, $cType y, $cType *__restrict res_ptr) {",
					"int32_t ix = qb_quick_floor(x);",
					"int32_t iy = qb_quick_floor(y);",
					"if(((uint32_t) ix < width) && ((uint32_t) iy < height)) {",
						"uint32_t index = ((iy * width) + ix) * 3;",
						"res_ptr[0] = pixels[index + 0];",
						"res_ptr[1] = pixels[index + 1];",
						"res_ptr[2] = pixels[index + 2];",
					"} else {",
						"res_ptr[0] = res_ptr[1] = res_ptr[2] = 0;",
					"}",
				"}",
			),
			array(
				"static void ZEND_FASTCALL qb_sample_nearest_4x_$type($cType *__restrict pixels, uint32_t width, uint32_t height, $cType x, $cType y, $cType *__restrict res_ptr) {",
					"int32_t ix = qb_quick_floor(x);",
					"int32_t iy = qb_quick_floor(y);",
					"if(((uint32_t) ix < width) && ((uint32_t) iy < height)) {",
						"uint32_t index = ((iy * width) + ix) * 4;",
						"res_ptr[0] = pixels[index + 0];",
						"res_ptr[1] = pixels[index + 1];",
						"res_ptr[2] = pixels[index + 2];",
						"res_ptr[3] = pixels[index + 3];",
					"} else {",
						"res_ptr[0] = res_ptr[1] = res_ptr[2] = res_ptr[3] = 0;",
					"}",
				"}",
			),
		);
		return $functions;
	}
	
	public function getAction() {
		$type = $this->getOperandType(1);
		$expr = "qb_sample_nearest_{$this->operandSize}x_$type(op1_ptr, op2, op3, op4, op5, res_ptr);";
		return ($this->addressMode == "ARR") ? $this->getIterationCode($expr) : $expr;
	}
}

?>