<?php

class Leave extends Handler {

	use NoOperands, AlwaysReturn;
	
	public function getJumpTargetCount() {
		return -1;
	}
	
	public function getHandlerFunctionType() {
		return null;
	}
	
	public function getAction() {
		$lines = array();
		$lines[] = "cxt->exit_type = QB_VM_RETURN;";
		$lines[] = "return;";
		return $lines;
	}
}

?>
