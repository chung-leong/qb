<?php

class ArrayProduct extends Handler {

	use MultipleAddressMode, UnaryOperator, UnitResult;
	
	public function getOperandSize($i) {
		switch($i) {
			case 1: return "op1_count";
			case 2: return 1;
		}
	}
	
	public function getActionOnUnitData() {
		$type = $this->getOperandType(1);
		$cType = $this->getOperandCType(1);
		$lines[] = "uint32_t i;";
		$lines[] = "$cType product = 1;";
		$lines[] = "for(i = 0; i < op1_count; i++) {";
		$lines[] = 		"product *= op1_ptr[i];";
		$lines[] = "}";
		$lines[] = "res = product;";
		return $lines;
	}
}

?>