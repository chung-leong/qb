<?php

class Leave extends Handler {

	use NoOperands, ExitVM;
	
	public function getAction() {
		$lines = array();
		$lines[] = "cxt->exit_type = QB_VM_RETURN;";
		$lines[] = "return;";
		return $lines;
	}
}

?>