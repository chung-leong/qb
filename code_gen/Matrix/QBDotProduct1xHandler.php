<?php

class QBDotProduct1xHandler extends QBHandler {

	public function getOperandAddressMode($i) {
		switch($i) {
			case 1: 
			case 2: return "ARR";
			case 3: return $this->addressMode;
		}
	}

	public function getAction() {
		$lines = array();
		$lines[] = "res = op1 * op2;";
		return $lines;
	}

}

?>