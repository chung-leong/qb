<?php

class ExtensionOp extends Handler {

	use ScalarAddressMode, UnaryOperatorNoResult, MainThreadExecution, NeedLineNumber;
	
	public function getAction() {
		return "qb_run_zend_extension_op(cxt, op1, line_id);";
	}
}

?>
