<?php

class QBLength1xHandler extends QBHandler {

	public function getOperandAddressMode($i) {
		switch($i) {
			case 1: return "ARR";
			case 2: return $this->addressMode;
		}
	}

	protected function getScalarExpression() {
		$cType = $this->getOperandCType(1);
		$lines = array();
		$lines[] = "res = ($cType) fabs(op1);";
		return $lines;
	}

}

?>