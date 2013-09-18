<?php

class ArrayMin extends Handler {

	use MultipleAddressMode, UnaryOperator, UnitResult;

	public function getActionOnUnitData() {
		$cType = $this->getOperandCType(1);
		$lines[] = "if(op1_count > 0) {";
		$lines[] = 		"uint32_t i;";
		$lines[] = 		"$cType smallest = op1_ptr[0];";
		$lines[] = 		"for(i = 1; i < op1_count; i++) {";
		$lines[] = 			"if(op1_ptr[i] < smallest) {";
		$lines[] = 				"smallest = op1_ptr[i];";
		$lines[] = 			"}";
		$lines[] = 		"}";
		$lines[] = 		"res = smallest;";
		$lines[] = "} else {";
		$lines[] = 		"res = 0;";
		$lines[] = "}";
		return $lines;
	}

}

?>