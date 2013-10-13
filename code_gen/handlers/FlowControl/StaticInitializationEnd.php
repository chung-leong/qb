<?php

class StaticInitializationEnd extends Handler {

	use NoOperands;

	public function getAction() {
		return "cxt->active_function->instruction_start = ip;";
	}
}

?>