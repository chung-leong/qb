<?php

class ExtensionOp extends Handler {

	use ScalarAddressMode, UnaryOperator, needLineNumber;
	
	public function getOperandType($i) {
		return "U32";
	}

	public function getAction() {
		return "qb_run_zend_extension_op(cxt, op1, PHP_LINE_NUMBER);";
	}
}

?>