<?php

class Terminate extends Handler {
	
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
		$lines[] = "cxt->exit_status_code = op1;"; 
		$lines[] = "cxt->exit_type = QB_VM_TERMINATE;";
		$lines[] = "return;";
		return $lines;
	}
}

?>
