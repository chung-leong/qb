<?php

class QBExtensionOpHandler extends QBHandler {
	
	public function getOutputOperandCount() {
		return 0;
	}

	public function needsLineNumber($where = null) {
		return true;
	}

	public function getOperandType($i) {
		return "U32";
	}

	public function getOperandAddressMode($i) {
		return "VAR";
	}
	
	public function getAction() {
		return "qb_run_zend_extension_op(cxt, op1, PHP_LINE_NUMBER);";
	}
}

?>