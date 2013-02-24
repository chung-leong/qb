<?php

class QBAnyHandler extends QBHandler {

	public function getOperandType($i) {
		return ($i == 1) ? $this->operandType : "I32";
	}

	public function getOperandAddressMode($i) {
		return ($i == 1) ? "ARR" : $this->addressMode;
	}

	protected function getScalarExpression() {
		$lines[] = array();
		$lines[] = "res = 0;";
		$lines[] = "while(op1_ptr != op1_end) {";
		$lines[] = 		"if(op1) {";
		$lines[] =			"res = 1;";
		$lines[] =			"break;";
		$lines[] = 		"}";
		$lines[] =		"op1_ptr++;";
		$lines[] = "}";
		return $lines;
	}
}

?>