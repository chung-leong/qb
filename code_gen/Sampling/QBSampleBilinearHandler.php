<?php

class QBSampleBilinearHandler extends QBSampleHandler {

	protected function getPixelRetrievalExpression($offsetX, $offsetY) {
		$pixel = "p$offsetX$offsetY";
		$lines = array();
		$lines[] = "if((((uint32_t) ix + $offsetX) < width) && (((uint32_t) iy + $offsetY) < height)) {";
		$lines[] =		"uint32_t index = (((iy + $offsetY) * width) + (ix + $offsetX)) * $this->operandSize;";
		$lines[] =		"{$pixel}[0] = pixels[index + 0];";
		$lines[] =		"{$pixel}[1] = pixels[index + 1];";
		$lines[] =		"{$pixel}[2] = pixels[index + 2];";
		if($this->operandSize == 4) {
			$lines[] =	"{$pixel}[3] = pixels[index + 3];";
		}
		$lines[] = "} else {";
		$lines[] =		"{$pixel}[0] = 0;";
		$lines[] =		"{$pixel}[1] = 0;";
		$lines[] =		"{$pixel}[2] = 0;";
		if($this->operandSize == 4) {
			$lines[] =	"{$pixel}[3] = 0;";
		}
		$lines[] = "}";
		return $lines;
	}
	
	public function getActionOnUnitData() {
		$type = $this->getOperandType(6);
		$cType = $this->getOperandCType(6);
		$f = ($type == 'F32') ? 'f' : '';
		$lines = array();
		$lines[] = "int32_t ix = qb_quick_floor$f(op4 - 0.5$f);";
		$lines[] = "int32_t iy = qb_quick_floor$f(op5 - 0.5$f);";
		$lines[] = "$cType fx = (x - 0.5$f) - floor$f(op4 - 0.5$f);";
		$lines[] = "$cType fy = (y - 0.5$f) - floor$f(op5 - 0.5$f);";
		$lines[] = "if(fx + fy == 0) {";
		$lines[] = 		"if(((uint32_t) ix < op2) && ((uint32_t) iy < op3)) {";
		$lines[] = 			"uint32_t index = ((iy * width) + ix) * 4;";
		$lines[] = 			"res_ptr[0]	= pixels[index + 0];";
		$lines[] = 			"res_ptr[1]	= pixels[index + 1];";
		$lines[] = 			"res_ptr[2]	= pixels[index + 2];";
		if($this->operandSize == 4) {
			$lines[] = 		"res_ptr[3]	= pixels[index + 3];";
		}
		$lines[] = 		"} else {";
		$lines[] = 			"res_ptr[0] = 0;";
		$lines[] =			"res_ptr[1] = 0;";
		$lines[] = 			"res_ptr[2] = 0;";
		if($this->operandSize == 4) {
			$lines[] =		"res_ptr[3] = 0;";
		}
		$lines[] = 		"}";
		$lines[] = "} else { ";
		$lines[] = 		"$cType fx1 = 1.0$f - fx;";
		$lines[] = 		"$cType fy1 = 1.0$f - fy;";
		$lines[] = 		"$cType w00 = fx1 * fy1;";
		$lines[] = 		"$cType w10 = fx * fy1;";
		$lines[] = 		"$cType w01 = fx1 * fy;";
		$lines[] = 		"$cType w11 = fx * fy;";
		$lines[] = 		"$cType p00[4], p01[4], p10[4], p11[4];";
		$lines[] = 		$this->getPixelRetrievalExpression(0, 0);
		$lines[] = 		$this->getPixelRetrievalExpression(1, 0);
		$lines[] = 		$this->getPixelRetrievalExpression(0, 1);
		$lines[] = 		$this->getPixelRetrievalExpression(1, 1);
		$lines[] = 		"res_ptr[0] = p00[0] * w00 + p10[0] * w10 + p01[0] * w01 + p11[0] * w11;";
		$lines[] = 		"res_ptr[1] = p00[1] * w00 + p10[1] * w10 + p01[1] * w01 + p11[1] * w11;";
		$lines[] = 		"res_ptr[2] = p00[2] * w00 + p10[2] * w10 + p01[2] * w01 + p11[2] * w11;";
		if($this->operandSize == 4) {
			$lines[] = 	"res_ptr[3] = p00[3] * w00 + p10[3] * w10 + p01[3] * w01 + p11[3] * w11;";
		}
		$lines[] = "}";
		return $lines;
	}
}

?>