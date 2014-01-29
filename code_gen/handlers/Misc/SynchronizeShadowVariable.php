<?php

class SynchronizeShadowVariable extends Handler {

	use ScalarAddressMode, UnaryOperatorNoResult, MainThreadExecution;
	
	public function getAction() {
		return "qb_sync_shadow_variable(cxt, op1);";
	}
}

?>
