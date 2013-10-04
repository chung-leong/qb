<?php

class ChooseSizeLargerOfTwo extends Handler {

	use ScalarAddressMode, QuaternaryOperator;
	
	protected function getActionOnUnitData() {
		$lines = array();
		$lines[] = "if(op1 >= op3) {";
		$lines[] =  	"res = op2;";
		$lines[] = "} else {";
		$lines[] =  	"res = op4;";
		$lines[] = "}";
		return $lines;
	}
}

?>