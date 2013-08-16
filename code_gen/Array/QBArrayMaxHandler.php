<?php

class QBArrayMaxHandler extends QBHandler {

	public function getOperandAddressMode($i) {
		if($i == 1) {
			return "ARR";
		} else {
			return $this->addressMode;
		}
	}

	public function getActionOnUnitData() {
		$cType = $this->getOperandCType(1);
		$lines[] = "if(op1_count > 0) {";
		$lines[] = 		"uint32_t i;";
		$lines[] = 		"$cType biggest = op1_ptr[0];";
		$lines[] = 		"for(i = 1; i < op1_count; i++) {";
		$lines[] = 			"if(op1_ptr[i] > biggest) {";
		$lines[] = 				"biggest = op1_ptr[i];";
		$lines[] = 			"}";
		$lines[] = 		"}";
		$lines[] = 		"res = biggest;";
		$lines[] = "} else {";
		$lines[] = 		"res = 0;";
		$lines[] = "}";
		return $lines;
	}
}

?>