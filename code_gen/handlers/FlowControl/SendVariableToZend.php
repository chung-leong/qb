<?php

class SendVariableToZend extends Handler {

	public function __construct($baseName) {
		$this->baseName = $baseName;
		$this->operandType = "U32";
		$this->operandSize = 1;
		$this->addressMode = "CON";
		$this->multipleData = false;
	}

	public function getInputOperandCount() {
		return 1;
	}
	
	public function needsLocalStorage() {
		return true;
	}
	
	public function needsFunctionObject() {
		return true;
	}
	
	public function getHandlerFunctionType() {
		return null;
	}
	
	public function getActionOnUnitData() {
		$lines = array();
		$lines[] = "qb_variable *var = function->variables[op1];";
		$lines[] = "zval ***pp_zarg, **p_zarg, *zarg;";
		$lines[] = "if(!zend_arguments) {";
		$lines[] =		"qb_create_array((void **) &zend_arguments, &zend_argument_count, sizeof(zval *), 4);";
		$lines[] =		"qb_create_array((void **) &zend_argument_pointers, &zend_argument_pointer_count, sizeof(zval **), 4);";
		$lines[] = "}";
		$lines[] = "ALLOC_INIT_ZVAL(zarg);";
		$lines[] = "qb_transfer_value_to_zval(local_storage, var->address, zarg);";
		$lines[] = "p_zarg = qb_enlarge_array((void **) &zend_arguments, 1);";
		$lines[] = "pp_zarg = qb_enlarge_array((void **) &zend_argument_pointers, 1);";
		$lines[] = "*pp_zarg = p_zarg;";
		$lines[] = "*p_zarg = zarg;";
		return $lines;
	}
}

?>