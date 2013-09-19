<?php

class Shuffle extends Handler {

	use ArrayAddressMode, UnaryOperator;

	public function getOperandType($i) {
		switch($i) {
			case 1: return "U32";
			case 2: return $this->operandType;
		}
	}
	
	public function getOperandAddressMode($i) {
		switch($i) {
			case 1: return "SCA";
			case 2: return "ARR";
		}
	}
	
	public function needsInterpreterContext() {
		return true;
	}

	public function getActionOnUnitData() {
		$cType = $this->getOperandCType(2);
		$lines = array();
		$lines[] = "USE_TSRM";
		$lines[] = "uint32_t n_elems, n_left, rnd_idx;";
		$lines[] = "$cType temp, *temps = NULL, *elems = res_ptr;";
		$lines[] = "ALLOCA_FLAG(use_heap);";
		$lines[] = "if(op1 == 1) {";
		$lines[] = 		"n_elems = res_count;";
		$lines[] = 		"temps = NULL;";
		$lines[] = "} else {";
		$lines[] = 		"n_elems = res_count / op1;";
		$lines[] = 		"temps = do_alloca(sizeof($cType) * op1, use_heap);";
		$lines[] = "}";
		$lines[] = "n_left = n_elems;";
		$lines[] = "while(--n_left) {";
		$lines[] = 		"rnd_idx = php_rand(TSRMLS_C);";
		$lines[] = 		"RAND_RANGE(rnd_idx, 0, n_left, PHP_RAND_MAX);";
		$lines[] = 		"if(rnd_idx != n_left) {";
		$lines[] = 			"if(op1 == 1) {";
		$lines[] = 				"temp = elems[n_left];";
		$lines[] = 				"elems[n_left] = elems[rnd_idx];";
		$lines[] = 				"elems[rnd_idx] = temp;";
		$lines[] = 			"} else {";
		$lines[] = 				"memcpy(temps, &elems[n_left * op1], sizeof($cType) * op1);";
		$lines[] = 				"memcpy(&elems[n_left * op1], &elems[rnd_idx * op1], sizeof($cType) * op1);";
		$lines[] = 				"memcpy(&elems[rnd_idx * op1], temps, sizeof($cType) * op1);";
		$lines[] = 			"}";
		$lines[] = 		"}";
		$lines[] = "}";
		$lines[] = "if(temps) {";
		$lines[] = 		"free_alloca(temps, use_heap);";
		$lines[] = "}";
		return $lines;
	}
}

?>