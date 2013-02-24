<?php

class QBReturnHandler extends QBHandler {

	public function getCode() {
		$lines = array();
		$name = $this->getName();
		$lines[] = $this->getLabelCode($name);
		$lines[] = "{";
		$lines[] = "}";
		$lines[] = "goto label_exit;";
		return $lines;
	}
	
	protected function initialize() {
		$this->flags |= self::WILL_JUMP;
	}
}

?>