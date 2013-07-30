<?php

class QBExitHandler extends QBHandler {
	
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
			for($i = 1; $i <= $this->opCount; $i++) {
			$lines[] = $this->getOperandDeclaration($i);
		}
		for($i = 1; $i <= $this->opCount; $i++) {
			$lines[] = $this->getOperandRetrievalCode($i);
		}
		$lines[] =		"EG(exit_status) = op1;"; 
		$lines[] =		"zend_bailout();";
		$lines[] = "}";
		return $lines;
	}
	
	protected function initialize() {
		$this->flags |= self::WILL_JUMP;
	}
}

?>