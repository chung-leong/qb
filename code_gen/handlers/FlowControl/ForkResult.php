<?php

class ForkResult extends Handler {

	use ArrayAddressMode, NullaryOperator;
	
	public function needsInterpreterContext() {
		return true;
	}

	protected function getActionOnUnitData() {
		$lines = array();
		$lines[] = "res_ptr[0] = cxt->fork_id;";
		$lines[] = "res_ptr[1] = cxt->fork_count;";
		return $lines;
	}
}

?>