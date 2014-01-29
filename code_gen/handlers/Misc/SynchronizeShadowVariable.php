<?php

class SynchronizeShadowVariable extends Handler {

	use ScalarAddressMode;
	
	public function getInputOperandCount() {
		return 1;
	}
	
	public function getOperandType($i) {
		return "U32";
	}

	public function getAction() {
		return "qb_sync_shadow_variable(cxt, op1);";
	}
}

?>
