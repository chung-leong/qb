<?php

class ForkCount extends Handler {

	use MultipleAddressMode, NullaryOperator;
	
	public function needsInterpreterContext() {
		return true;
	}

	protected function getActionOnUnitData() {
		return "res = cxt->fork_count;";
	}
}

?>