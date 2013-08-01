<?php

class QBArraySumHandler extends QBHandler {

	public function getOperandAddressMode($i) {
		if($i == 1) {
			return "ARR";
		} else {
			return $this->addressMode;
		}
	}
	
	public function getActionForUnitData() {
		$type = $this->getOperandType(1);
		$cType = $this->getOperandCType(1);
		$lines[] = "uint32_t i;";
		$lines[] = "$cType sum = 0;";
		$lines[] = "for(i = 0; i < op1_count; i++) {";
		$lines[] = 		"sum += op1_ptr[i];";
		$lines[] = "}";
		$lines[] = "res = sum;";
		return $lines;
	}
}

?>