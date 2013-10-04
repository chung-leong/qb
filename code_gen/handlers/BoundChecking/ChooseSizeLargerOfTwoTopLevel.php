<?php

class ChooseSizeLargerOfTwoTopLevel extends Handler {

	use ScalarAddressMode, BinaryOperator;
	
	protected function getActionOnUnitData() {
		$lines = array();
		$lines[] = "if(op1 >= op2) {";
		$lines[] = 		"res = op1;";
		$lines[] = "} else {";
		$lines[] = 		"res = op2;";
		$lines[] = "}";
		return $lines;
	}
}

?>