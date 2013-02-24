<?php

class QBExtensionOpHandler extends QBHandler {
	
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