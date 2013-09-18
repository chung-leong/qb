<?php

class ArraySearch extends Handler {

	use ArrayAddressMode, BinaryOperator, NoUnsigned, UnitResult, IndexResult;

	public function getOperandAddressMode($i) {
		switch($i) {
			case 1: return "ARR";
			case 2: return $this->addressMode;
			case 3: return "SCA";
		}
	}
	
	protected function getActionOnUnitData() {
		$type = $this->getOperandType(1);
		$cType = $this->getOperandCType(1);
		$addressMode = $this->getOperandAddressMode(2);
		$lines = array();
		$lines[] = "int32_t index = -1;";
		if($addressMode == "ARR") {
			$lines[] = "uint32_t i, j, k;";
			$lines[] = "for(i = 0, j = 0; j < op1_count; i++, j += op2_count) {";
			$lines[] = 		"if(op1_ptr[j] == op2_ptr[0]) {";
			$lines[] = 			"for(k = 1; k < op2_count; k++) {";
			$lines[] = 				"if(op1_ptr[j + k] != op2_ptr[k]) {";
			$lines[] = 					"break;";
			$lines[] = 				"}";
			$lines[] = 			"}";
			$lines[] = 			"if(k == op2_count) {";
			$lines[] = 				"index = i;";
			$lines[] = 				"break;";
			$lines[] = 			"}";
			$lines[] = 		"}";
			$lines[] = "}";
		} else {
			$lines[] = "uint32_t i;";
			$lines[] = "for(i = 0; i < op1_count; i++) {";
			$lines[] = 		"if(op1_ptr[i] == op2) {";
			$lines[] = 			"index = i;";
			$lines[] =			"break;";
			$lines[] = 		"}";
			$lines[] = "}";
		}
		$lines[] = "res = index;";
		return $lines;
	}
}

?>