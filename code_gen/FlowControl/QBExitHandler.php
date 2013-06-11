<?php

class QBExitHandler extends QBHandler {
	
	public function getOperandType($i) {
		return "I32";
	}
	
	public function getOperandAddressMode($i) {
		return "VAR";
	}
	
	public function getCode() {
		$lines = array();
		$lines[] = $this->getDefineCode();
		$lines[] = $this->getLabelCode();
		$lines[] = "{";
		for($i = 1; $i <= $this->opCount; $i++) {
			$lines[] = $this->getOperandRetrievalCode($i);
		}
		$lines[] =		"EG(exit_status) = op1;"; 
		$lines[] =		"zend_bailout();";
		$lines[] = "}";
		$lines[] = $this->getUndefCode();
		return $lines;
	}
	
	protected function initialize() {
		$this->flags |= self::WILL_JUMP;
	}
}

?>