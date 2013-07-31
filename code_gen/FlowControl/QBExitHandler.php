<?php

class QBExitHandler extends QBHandler {
	
	public function getJumpTargetCount() {
		return -1;
	}
	
	public function getInputOperandCount() {
		return 0;
	}

	public function getOutputOperandCount() {
		return 0;
	}

	public function getOperandType($i) {
		return "I32";
	}
	
	public function getOperandAddressMode($i) {
		return "VAR";
	}
	
	public function getCode() {
		$lines = array();
		$name = $this->getName();
		$lines[] = $this->getLabelCode($name);
		$lines[] = "{";
		$lines[] =		"EG(exit_status) = op1;"; 
		$lines[] =		"zend_bailout();";
		$lines[] = "}";
		return $lines;
	}
}

?>