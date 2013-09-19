<?php

class SampleNearest extends Handler {

	use MultipleAddressMode, PixelSampler, FloatingPointOnly, DoublePrecisionIsRare, UseQuickRoundingFunction;

	public function getActionOnUnitData() {
		$type = $this->getOperandType(1);
		$cType = $this->getOperandCType(1);
		$f = ($type == 'F32') ? 'f' : '';
		$lines = array();
		$lines[] = "int32_t ix = qb_quick_floor$f(op4);";
		$lines[] = "int32_t iy = qb_quick_floor$f(op5);";
		$lines[] = "if(((uint32_t) ix < op2) && ((uint32_t) iy < op3)) {";
		$lines[] = 		"uint32_t index = ((iy * op2) + ix) * $this->operandSize;";
		$lines[] = 		"res_ptr[0] = op1_ptr[index + 0];";
		$lines[] = 		"res_ptr[1] = op1_ptr[index + 1];";
		$lines[] = 		"res_ptr[2] = op1_ptr[index + 2];";
		if($this->operandSize == 4) {
			$lines[] = 	"res_ptr[3] = op1_ptr[index + 3];";
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