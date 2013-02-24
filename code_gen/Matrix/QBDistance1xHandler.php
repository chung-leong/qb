<?php

class QBDistance1xHandler extends QBHandler {

	public function getOperandAddressMode($i) {
		switch($i) {
			case 1:
			case 2: return "ARR";
			case 3: return $this->addressMode;
		}
	}

	public function getAction() {
		$cType = $this->getOperandCType(1);
		$lines = array();
		$lines[] = "res = ($cType) fabs(op1 - op2);";
		return $lines;
	}
}

?>