<?php

class StaticInitializationEnd extends Handler {

	use NoOperands;

	public function getAction() {
		return "function->instruction_start = function->instruction_non_static_start;";
	}
}

?>