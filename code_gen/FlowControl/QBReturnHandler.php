<?php

class QBReturnHandler extends QBHandler {

	public function getJumpTargetCount() {
		return -1;
	}
	
	public function getInputOperandCount() {
		return 0;
	}
	
	public function getOutputOperandCount() {
		return 0;
	}

	public function getCode() {
		$lines = array();
		$name = $this->getName();
		$lines[] = $this->getLabelCode($name);
		$lines[] = "{";
		$lines[] = "}";
		$lines[] = "goto label_exit;";
		return $lines;
	}
}

?>