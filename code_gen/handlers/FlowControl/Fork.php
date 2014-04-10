<?php

class Fork extends Handler {

	use ScalarAddressMode, AlwaysReturn;
	
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
		$lines = array();
		// use (void *) instead of instruction struct so we don't have to define it
		$lines[] = "cxt->instruction_pointer = ip + sizeof(void *);";
		$lines[] = "cxt->fork_count = op1;";
		$lines[] = "cxt->exit_type = QB_VM_FORK;";
		$lines[] = "return;";
		return $lines;
	}
}

?>
