<?php

class Spoon extends Handler {

	use NoOperands;
	
	public function getHandlerFunctionType() {
		return null;
	}
	
	public function getAction() {
		$lines = array();
		$lines[] = "cxt->instruction_pointer = ip;";
		$lines[] = "cxt->exit_type = QB_VM_SPOON;";
		$lines[] = "return;";
		return $lines;
	}
}

?>
