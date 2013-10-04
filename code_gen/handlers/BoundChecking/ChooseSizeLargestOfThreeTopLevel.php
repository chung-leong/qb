<?php

class ChooseSizeLargestOfThreeTopLevel extends Handler {

	use ScalarAddressMode, TernaryOperator;
	
	protected function getActionOnUnitData() {
		$lines = array();
		$lines[] = "if(op1 >= op2) {";
		$lines[] =		"if(op1 >= op3) {";
		$lines[] = 			"res = op1;";
		$lines[] =		"} else {";
		$lines[] =			"res = op3;";
		$lines[] =		"}";
		$lines[] = "} else {";
		$lines[] =		"if(op2 >= op3) {";
		$lines[] = 			"res = op2;";
		$lines[] =		"} else {";
		$lines[] =			"res = op3;";
		$lines[] =		"}";
		$lines[] = "}";
		return $lines;
	}
}

?>