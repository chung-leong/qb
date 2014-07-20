<?php

class SampleConvolution extends Handler {

	use MultipleAddressMode, FloatingPointOnly, DoublePrecisionIsRare, Multithreaded;
	
	public function getInputOperandCount() {
		return 10;
	}

	public function getOutputOperandCount() {
		return 1;
	}
	
	public function getOperandAddressMode($i) {
		switch($i) {
			case 1: return "ARR";	// image
			case 2:	return "SCA";	// width
			case 3:	return "SCA";	// height
			case 4: return $this->addressMode;	// x
			case 5: return $this->addressMode;	// y
			case 6: return "ARR";	// cells
			case 7: return "SCA";	// row count
			case 8: return "SCA";	// column count
			case 9: return $this->addressMode;	// divider
			case 10: return $this->addressMode;	// offset
			case 11: return "ARR";	// result
		}
	}
	
	public function getOperandType($i) {
		switch($i) {
			case 1: return $this->operandType;	// image
			case 2:	return "U32";	// width
			case 3:	return "U32";	// height
			case 4: return $this->operandType;	// x
			case 5: return $this->operandType;	// y
			case 6: return $this->operandType;	// cells
			case 7: return "U32";	// row count
			case 8: return "U32";	// column count
			case 9: return $this->operandType;	// divider
			case 10: return $this->operandType;	// offset
			case 11: return $this->operandType;	// result
		}
	}

	public function getOperandSize($i) {
		switch($i) {
			case 1: return 0;	// the image remains fixed during iteration
			case 2: return 1;
			case 3: return 1;
			case 4: return 1;
			case 5: return 1;
			case 6: return 0;	// the convolution matrix also doesn't shift
			case 7: return 1;
			case 8: return 1;
			case 9: return 1;
			case 10: return 1;
			case 11: return $this->operandSize;
		}
	}
	
	protected function getPixelRetrievalExpression($offsetX, $offsetY) {
		$pixel = "p$offsetX$offsetY";
		$lines = array();
		$lines[] = "if((((uint32_t) ix + $offsetX) < op2) && (((uint32_t) iy + $offsetY) < op3)) {";
		$lines[] =		"uint32_t index = (((iy + $offsetY) * op2) + (ix + $offsetX)) * $this->operandSize;";
		$lines[] =		"{$pixel}[0] = op1_ptr[index + 0];";
		if($this->operandSize >= 2) {
			$lines[] =	"{$pixel}[1] = op1_ptr[index + 1];";
		}
		if($this->operandSize >= 3) {
			$lines[] =	"{$pixel}[2] = op1_ptr[index + 2];";
		}
		if($this->operandSize >= 4) {
			$lines[] =	"{$pixel}[3] = op1_ptr[index + 3];";
		}
		$lines[] = "} else {";
		$lines[] =		"{$pixel}[0] = 0;";
		if($this->operandSize >= 2) {
			$lines[] =	"{$pixel}[1] = 0;";
		}
		if($this->operandSize >= 3) {
			$lines[] =	"{$pixel}[2] = 0;";
		}
		if($this->operandSize >= 4) {
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
		$lines[] = "$cType x0 = op4 - ((double) (op8 - 1)) * 0.5$f;";
		$lines[] = "$cType y0 = op5 - ((double) (op7 - 1)) * 0.5$f;";
		$lines[] = "int32_t ix0 = (int32_t) floor$f(x0 - 0.5$f);";
		$lines[] = "int32_t iy0 = (int32_t) floor$f(y0 - 0.5$f);";
		$lines[] = "$cType fx = (x0 - 0.5$f) - floor$f(x0 - 0.5$f);";
		$lines[] = "$cType fy = (y0 - 0.5$f) - floor$f(y0 - 0.5$f);";
		$lines[] = "int32_t ix, iy, c, r;";
		$lines[] = "$cType sum[$this->operandSize] = {";
		$lines[] = 		implode(', ', array_fill(0, $this->operandSize, "0.0$f"));
		$lines[] = "};";
		$lines[] = "if(fx + fy == 0) {";
		$lines[] =		"for(r = 0, iy = iy0; r < op7; r++, iy++) {";
		$lines[] =			"for(c = 0, ix = ix0; c < op8; c++, ix++) {";
		$lines[] =				"$cType coefficient = *op6_ptr++;";
		$lines[] = 				"if(((uint32_t) ix < op2) && ((uint32_t) iy < op3)) {";
		$lines[] = 					"uint32_t index = ((iy * op2) + ix) * $this->operandSize;";
		$lines[] = 					"sum[0] += op1_ptr[index + 0] * coefficient;";
		if($this->operandSize >= 2) {
			$lines[] = 				"sum[1] += op1_ptr[index + 1] * coefficient;";
		}
		if($this->operandSize >= 3) {
			$lines[] = 				"sum[2] += op1_ptr[index + 2] * coefficient;";
		}
		if($this->operandSize >= 4) {
			$lines[] = 				"sum[3] += op1_ptr[index + 3] * coefficient;";
		}
		$lines[] = 				"}";
		$lines[] = 			"}";
		$lines[] = 		"}";
		$lines[] = "} else { ";
		$lines[] = 		"$cType fx1 = 1.0$f - fx;";
		$lines[] = 		"$cType fy1 = 1.0$f - fy;";
		$lines[] = 		"$cType w00 = fx1 * fy1;";
		$lines[] = 		"$cType w10 = fx * fy1;";
		$lines[] = 		"$cType w01 = fx1 * fy;";
		$lines[] = 		"$cType w11 = fx * fy;";
		$lines[] = 		"$cType p00[4], p01[4], p10[4], p11[4];";
		$lines[] =		"for(r = 0, iy = iy0; r < op7; r++, iy++) {";
		$lines[] =			"for(c = 0, ix = ix0; c < op8; c++, ix++) {";
		$lines[] =				"$cType coefficient = *op6_ptr++;";
		$lines[] =		 		$this->getPixelRetrievalExpression(0, 0);
		$lines[] = 				$this->getPixelRetrievalExpression(1, 0);
		$lines[] = 				$this->getPixelRetrievalExpression(0, 1);
		$lines[] = 				$this->getPixelRetrievalExpression(1, 1);
		$lines[] = 				"sum[0] += (p00[0] * w00 + p10[0] * w10 + p01[0] * w01 + p11[0] * w11) * coefficient;";
		if($this->operandSize >= 2) {
			$lines[] =			"sum[1] += (p00[1] * w00 + p10[1] * w10 + p01[1] * w01 + p11[1] * w11) * coefficient;";
		}
		if($this->operandSize >= 3) {
			$lines[] = 			"sum[2] += (p00[2] * w00 + p10[2] * w10 + p01[2] * w01 + p11[2] * w11) * coefficient;";
		}
		if($this->operandSize >= 4) {
			$lines[] = 			"sum[3] += (p00[3] * w00 + p10[3] * w10 + p01[3] * w01 + p11[3] * w11) * coefficient;";
		}
		$lines[] = 			"}";
		$lines[] = 		"}";
		$lines[] = "}";
		$lines[] = "if(op9 != 0.0$f) {";
		$lines[] = 		"sum[0] /= op9;";
		if($this->operandSize >= 2) {
			$lines[] = 	"sum[1] /= op9;";
		}
		if($this->operandSize >= 3) {
			$lines[] = 	"sum[2] /= op9;";
		}
		if($this->operandSize >= 4) {
			$lines[] = 	"sum[3] /= op9;";
		}
		$lines[] = "}";
		$lines[] = "if(op10 != 0.0$f) {";
		$lines[] = 		"sum[0] += op10;";
		if($this->operandSize >= 2) {
			$lines[] = 	"sum[1] += op10;";
		}
		if($this->operandSize >= 3) {
			$lines[] = 	"sum[2] += op10;";
		}
		if($this->operandSize >= 4) {
			$lines[] = 	"sum[3] += op10;";
		}
		$lines[] = "}";
		$lines[] = "res_ptr[0] = sum[0];";
		if($this->operandSize >= 2) {
			$lines[] = 	"res_ptr[1] = sum[1];";
		}
		if($this->operandSize >= 3) {
			$lines[] = 	"res_ptr[2] = sum[2];";
		}
		if($this->operandSize >= 4) {
			$lines[] = 	"res_ptr[3] = sum[3];";
		}
		return $lines;
	}
}

?>
