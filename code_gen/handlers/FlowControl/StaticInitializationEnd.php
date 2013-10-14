<?php

class StaticInitializationEnd extends Handler {

	use NoOperands;

	public function getAction() {
		return "cxt->function->instruction_start = ip;";
	}
}

?>