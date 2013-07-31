<?php

class QBSampleNearestHandler extends QBSampleHandler {

	public function getHelperFunctions() {
		$type = $this->getOperandType(1);
		$cType = $this->getOperandCType(1);
		$f = ($type == 'F32') ? 'f' : '';
		$functions = array(
			array(
				"void ZEND_FASTCALL qb_sample_nearest_3x_$type($cType *__restrict pixels, uint32_t width, uint32_t height, $cType x, $cType y, $cType *__restrict res_ptr) {",
					"int32_t ix = qb_quick_floor$f(x);",
					"int32_t iy = qb_quick_floor$f(y);",
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
				"void ZEND_FASTCALL qb_sample_nearest_4x_$type($cType *__restrict pixels, uint32_t width, uint32_t height, $cType x, $cType y, $cType *__restrict res_ptr) {",
					"int32_t ix = qb_quick_floor$f(x);",
					"int32_t iy = qb_quick_floor$f(y);",
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
			array(
				"void ZEND_FASTCALL qb_sample_multiple_nearest_3x_$type($cType *__restrict pixels, uint32_t width, uint32_t height, $cType *x_start, $cType *x_end, $cType *y_start, $cType *y_end, $cType *res_start, $cType *res_end) {",
					"$cType *__restrict res_ptr = res_start;",
					"$cType *__restrict x_ptr = x_start;",
					"$cType *__restrict y_ptr = y_start;",
				
					"for(;;) {",
						"qb_sample_nearest_3x_$type(pixels, width, height, *x_ptr, *y_ptr, res_ptr);",
						"res_ptr += 3;",
						"if(res_ptr >= res_end) {",
							"break;",
						"}",
						"x_ptr += 1;",
						"if(x_ptr >= x_end) {",
							"x_ptr = x_start;",
						"}",
						"y_ptr += 1;",
						"if(y_ptr >= y_end) {",
							"y_ptr = y_start;",
						"}",						
					"}",
				"}",
			),
			array(
				"void ZEND_FASTCALL qb_sample_multiple_nearest_4x_$type($cType *__restrict pixels, uint32_t width, uint32_t height, $cType *x_start, $cType *x_end, $cType *y_start, $cType *y_end, $cType *res_start, $cType *res_end) {",
					"$cType *__restrict res_ptr = res_start;",
					"$cType *__restrict x_ptr = x_start;",
					"$cType *__restrict y_ptr = y_start;",
				
					"for(;;) {",
						"qb_sample_nearest_4x_$type(pixels, width, height, *x_ptr, *y_ptr, res_ptr);",
						"res_ptr += 4;",
						"if(res_ptr >= res_end) {",
							"break;",
						"}",
						"x_ptr += 1;",
						"if(x_ptr >= x_end) {",
							"x_ptr = x_start;",
						"}",
						"y_ptr += 1;",
						"if(y_ptr >= y_end) {",
							"y_ptr = y_start;",
						"}",						
					"}",
				"}",
			),
		);
		return $functions;
	}
	
	public function getAction() {
		$type = $this->getOperandType(1);
		if($this->addressMode == "ARR") {
			return "qb_sample_multiple_nearest_{$this->operandSize}x_$type(op1_ptr, op2, op3, op4_ptr, op5_ptr, res_ptr);";
		} else {
			return "qb_sample_nearest_{$this->operandSize}x_$type(op1_ptr, op2, op3, op4, op5, res_ptr);";
		}
	}
}

?>