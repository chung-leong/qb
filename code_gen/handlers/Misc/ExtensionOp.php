<?php

class ExtensionOp extends Handler {

	use ScalarAddressMode, NeedLineNumber;
	
	public function getInputOperandCount() {
		return 1;
	}
	
	public function getOperandType($i) {
		return "U32";
	}

	public function getAction() {
		return "qb_run_zend_extension_op(cxt, op1, line_id);";
	}
}

?>