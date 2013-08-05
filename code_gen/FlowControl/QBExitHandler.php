<?php

class QBExitHandler extends QBHandler {
	
	public function getJumpTargetCount() {
		return -1;
	}
	
	public function getInputOperandCount() {
		return 1;
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
		$opCount = $this->getOperandCount();
		$lines[] = $this->getLabelCode($name);
		$lines[] = "{";
		// retrieve operands (none by default)
		for($i = 1; $i <= $opCount; $i++) {
			$lines[] = $this->getOperandDeclaration($i);
		}
		for($i = 1; $i <= $opCount; $i++) {
			$lines[] = $this->getOperandRetrievalCode($i);
		}
		$lines[] =		"EG(exit_status) = op1;"; 
		$lines[] =		"zend_bailout();";
		$lines[] = "}";
		return $lines;
	}
}

?>