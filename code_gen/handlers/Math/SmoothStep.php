<?php

class SmoothStep extends Handler {

	use MultipleAddressMode, TernaryOperator, FloatingPointOnly;
	
	protected function getActionOnUnitData() {
		$cType = $this->getOperandCType(3);
		$lines = array();
		$lines[] = "if(op3 <= op1) {";
		$lines[] =		"res = 0;";
		$lines[] = "} else if(op3 >= op2) {";
		$lines[] =		"res = 1;";
		$lines[] = "} else {";
		$lines[] =		"$cType value = (op3 - op1) / (op2 - op1);";
		$lines[] = 		"res = value * value * (3 - 2 * value);";
		$lines[] = "}";
		return $lines;;
	}
}

?>