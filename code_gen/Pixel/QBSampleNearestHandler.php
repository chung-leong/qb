<?php

class QBSampleNearestHandler extends QBSampleHandler {

	public function getActionOnUnitData() {
		$type = $this->getOperandType(1);
		$cType = $this->getOperandCType(1);
		$f = ($type == 'F32') ? 'f' : '';
		$lines = array();
		$lines[] = "int32_t ix = qb_quick_floor$f(x);";
		$lines[] = "int32_t iy = qb_quick_floor$f(y);";
		$lines[] = "if(((uint32_t) ix < width) && ((uint32_t) iy < height)) {";
		$lines[] = 		"uint32_t index = ((iy * width) + ix) * $this->operandSize;";
		$lines[] = 		"res_ptr[0] = pixels[index + 0];";
		$lines[] = 		"res_ptr[1] = pixels[index + 1];";
		$lines[] = 		"res_ptr[2] = pixels[index + 2];";
		if($this->operandSize == 4) {
			$lines[] = 	"res_ptr[3] = pixels[index + 3];";
		}
		$lines[] = "} else {";
		$lines[] = 		"res_ptr[0] = 0;";
		$lines[] = 		"res_ptr[1] = 0;";
		$lines[] = 		"res_ptr[2] = 0;";
		if($this->operandSize == 4) {
			$lines[] = 	"res_ptr[3] = 0;";
		}
		$lines[] = "}";
		return $lines;
	}
}

?>