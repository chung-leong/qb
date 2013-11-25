<?php

class Fork extends Handler {

	use ScalarAddressMode;
	
	public function getJumpTargetCount() {
		return -1;
	}
	
	public function getHandlerFunctionType() {
		return null;
	}
	
	public function getInputOperandCount() {
		return 1;
	}

	public function needsInterpreterContext() {
		return true;
	}
	
	public function getAction() {
		$instr = $this->getInstructionStructure();
		$lines = array();
		$lines[] = "cxt->instruction_pointer = ip + sizeof($instr);";
		$lines[] = "cxt->fork_count = op1;";
		$lines[] = "cxt->exit_type = QB_VM_FORK;";
		$lines[] = "return;";
		return $lines;
	}
}

?>