<?php

class Return extends Handler {

	public function getJumpTargetCount() {
		return -1;
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