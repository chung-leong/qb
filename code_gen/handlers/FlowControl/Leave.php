<?php

class Leave extends Handler {

	use NoOperands, ExitFunction;
	
	public function getAction() {
		return "goto exit_label;";
	}
}

?>