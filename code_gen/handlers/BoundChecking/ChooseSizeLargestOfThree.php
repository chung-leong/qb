<?php

class ChooseSizeLargestOfThree extends Handler {

	use ScalarAddressMode, SenaryOperator;
	
	protected function getActionOnUnitData() {
		$lines = array();
		$lines[] = "if(op1 >= op3) {";
		$lines[] =  	"res = op2;";
		$lines[] = "} else {";
		$lines[] =  	"res = op4;";
		$lines[] = "}";
		
		$lines[] = "if(op1 >= op3) {";
		$lines[] =		"if(op1 >= op5) {";
		$lines[] = 			"res = op2;";
		$lines[] =		"} else {";
		$lines[] =			"res = op6;";
		$lines[] =		"}";
		$lines[] = "} else {";
		$lines[] =		"if(op3 >= op5) {";
		$lines[] = 			"res = op4;";
		$lines[] =		"} else {";
		$lines[] =			"res = op6;";
		$lines[] =		"}";
		$lines[] = "}";
		return $lines;
	}
}

?>