<?php

class ForkIdentifier extends Handler {

	use MultipleAddressMode, NullaryOperator;
	
	public function needsInterpreterContext() {
		return true;
	}

	protected function getActionOnUnitData() {
		return "res = cxt->fork_id;";
	}
}

?>