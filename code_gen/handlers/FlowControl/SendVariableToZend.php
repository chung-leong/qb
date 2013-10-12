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
		$lines[] = "qb_push_zend_argument(cxt, local_storage, var, &zend_argument_stack);";
		return $lines;
	}
}

?>