<?php

class Spoon extends Handler {

	use NoOperands, ExitVM;
	
	public function needsInterpreterContext() {
		return true;
	}
	
	public function getAction() {
		$instr = $this->getInstructionStructure();
		$lines = array();
		$lines[] = "cxt->instruction_pointer = ip;";
		$lines[] = "cxt->exit_type = QB_VM_SPOON;";
		$lines[] = "return;";
		return $lines;
	}
}

?>