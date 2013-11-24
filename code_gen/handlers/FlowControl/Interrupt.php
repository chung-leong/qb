<?php

class Interrupt extends Handler {

	use NoOperands, ExitVM;
	
	public function getAction() {
		$lines = array();
		$lines[] = "cxt->function->instruction_start = ip;";
		$lines[] = "cxt->exit_type = QB_VM_YIELD;";
		$lines[] = "return;";
		return $lines;
	}
}

?>