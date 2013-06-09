<?php

class QBSampleBilinearHandler extends QBSampleHandler {

	protected function getPixelRetrievalExpression($offsetX, $offsetY, $channelCount) {
		$lines = array();
		if($channelCount == 3) {
			$pixel = "p$offsetX$offsetY";
			$lines[] = "if((((uint32_t) ix + $offsetX) < width) && (((uint32_t) iy + $offsetY) < height)) {";
			$lines[] =		"uint32_t index = (((iy + $offsetY) * width) + (ix + $offsetX)) * 3;";
			$lines[] =		"{$pixel}[0] = pixels[index + 0];";
			$lines[] =		"{$pixel}[1] = pixels[index + 1];";
			$lines[] =		"{$pixel}[2] = pixels[index + 2];";
			$lines[] = "} else {";
			$lines[] =		"{$pixel}[0] = {$pixel}[1] = {$pixel}[2] = 0;";
			$lines[] = "}";
		} else {
			$pixel = "p$offsetX$offsetY";
			$lines[] = "if((((uint32_t) ix + $offsetX) < width) && (((uint32_t) iy + $offsetY) < height)) {";
			$lines[] =		"uint32_t index = (((iy + $offsetY) * width) + (ix + $offsetX)) * 4;";
			$lines[] =		"{$pixel}[0] = pixels[index + 0];";
			$lines[] =		"{$pixel}[1] = pixels[index + 1];";
			$lines[] =		"{$pixel}[2] = pixels[index + 2];";
			$lines[] =		"{$pixel}[3] = pixels[index + 3];";
			$lines[] = "} else {";
			$lines[] =		"{$pixel}[0] = {$pixel}[1] = {$pixel}[2] = {$pixel}[3] = 0;";
			$lines[] = "}";
		}
		return $lines;
	}
	
	public function getHelperFunctions() {
		$type = $this->getOperandType(1);
		$cType = $this->getOperandCType(1);
		$f = ($type == 'F32') ? 'f' : '';
		$functions = array(
			array(
				"static void ZEND_FASTCALL qb_sample_bilinear_3x_$type($cType *__restrict pixels, uint32_t width, uint32_t height, $cType x, $cType y, $cType *__restrict res_ptr) {",
					"int32_t ix = qb_quick_floor$f(x - 0.5$f);",
					"int32_t iy = qb_quick_floor$f(y - 0.5$f);",
					"$cType fx = (x - 0.5$f) - floor$f(x - 0.5$f);",
					"$cType fy = (y - 0.5$f) - floor$f(y - 0.5$f);",
					"if(fx == 0 && fy == 0) {",
						"if(((uint32_t) ix < width) && ((uint32_t) iy < height)) {",
							"uint32_t index = ((iy * width) + ix) * 3;",
							"res_ptr[0]	= pixels[index + 0];",
							"res_ptr[1]	= pixels[index + 1];",
							"res_ptr[2]	= pixels[index + 2];",
						"} else {",
							"res_ptr[0] = res_ptr[1] = res_ptr[2] = 0;",
						"}",
					"} else { ",
						"$cType fx1 = 1.0$f - fx;",
						"$cType fy1 = 1.0$f - fy;",
						"$cType w00 = fx1 * fy1;",
						"$cType w10 = fx * fy1;",
						"$cType w01 = fx1 * fy;",
						"$cType w11 = fx * fy;",
						"$cType p00[3], p01[3], p10[3], p11[3];",
						$this->getPixelRetrievalExpression(0, 0, 3),
						$this->getPixelRetrievalExpression(1, 0, 3),
						$this->getPixelRetrievalExpression(0, 1, 3),
						$this->getPixelRetrievalExpression(1, 1, 3),
						"res_ptr[0] = p00[0] * w00 + p10[0] * w10 + p01[0] * w01 + p11[0] * w11;",
						"res_ptr[1] = p00[1] * w00 + p10[1] * w10 + p01[1] * w01 + p11[1] * w11;",
						"res_ptr[2] = p00[2] * w00 + p10[2] * w10 + p01[2] * w01 + p11[2] * w11;",
					"}",
				"}",
			),
			array(
				"static void ZEND_FASTCALL qb_sample_bilinear_4x_$type($cType *__restrict pixels, uint32_t width, uint32_t height, $cType x, $cType y, $cType *__restrict res_ptr) {",
					"int32_t ix = qb_quick_floor$f(x - 0.5$f);",
					"int32_t iy = qb_quick_floor$f(y - 0.5$f);",
					"$cType fx = (x - 0.5$f) - floor$f(x - 0.5$f);",
					"$cType fy = (y - 0.5$f) - floor$f(y - 0.5$f);",
					"if(fx == 0 && fy == 0) {",
						"if(((uint32_t) ix < width) && ((uint32_t) iy < height)) {",
							"uint32_t index = ((iy * width) + ix) * 4;",
							"res_ptr[0]	= pixels[index + 0];",
							"res_ptr[1]	= pixels[index + 1];",
							"res_ptr[2]	= pixels[index + 2];",
							"res_ptr[3]	= pixels[index + 3];",
						"} else {",
							"res_ptr[0] = res_ptr[1] = res_ptr[2] = res_ptr[3] = 0;",
						"}",
					"} else { ",
						"$cType fx1 = 1.0$f - fx;",
						"$cType fy1 = 1.0$f - fy;",
						"$cType w00 = fx1 * fy1;",
						"$cType w10 = fx * fy1;",
						"$cType w01 = fx1 * fy;",
						"$cType w11 = fx * fy;",
						"$cType p00[4], p01[4], p10[4], p11[4];",
						$this->getPixelRetrievalExpression(0, 0, 4),
						$this->getPixelRetrievalExpression(1, 0, 4),
						$this->getPixelRetrievalExpression(0, 1, 4),
						$this->getPixelRetrievalExpression(1, 1, 4),
						"res_ptr[0] = p00[0] * w00 + p10[0] * w10 + p01[0] * w01 + p11[0] * w11;",
						"res_ptr[1] = p00[1] * w00 + p10[1] * w10 + p01[1] * w01 + p11[1] * w11;",
						"res_ptr[2] = p00[2] * w00 + p10[2] * w10 + p01[2] * w01 + p11[2] * w11;",
						"res_ptr[3] = p00[3] * w00 + p10[3] * w10 + p01[3] * w01 + p11[3] * w11;",
					"}",
				"}",
			),
		);
		return $functions;
	}
	
	public function getAction() {
		$type = $this->getOperandType(1);
		$expr = "qb_sample_bilinear_{$this->operandSize}x_$type(op1_ptr, op2, op3, op4, op5, res_ptr);";
		return ($this->addressMode == "ARR") ? $this->getIterationCode($expr) : $expr;
	}
}

?>