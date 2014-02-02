<?php

class Sign extends Handler {

	use MultipleAddressMode, UnaryOperator, FloatingPointOnly, Multithreaded;
	
	protected function getActionOnUnitData() {
		$lines = array();
		$lines[] = "if(op1 < 0) {";
		$lines[] =		"res = -1;";
		$lines[] = "} else if(op1 > 0) {";
		$lines[] =		"res = 1;";
		$lines[] = "} else {";
		$lines[] = 		"res = 0;";
		$lines[] = "}";
		return $lines;;
	}
}

?>