<?php

class ExecuteZendFunctionCall extends Handler {

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
	
	public function needsInterpreterContext() {
		return true;
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
		$lines[] = "qb_external_symbol *symbol = function->external_symbols[op1];";
		$lines[] = "zend_function *zfunc = symbol->pointer;";
		$lines[] = "zval *retval = qb_execute_zend_function_call(cxt, zfunc, zend_argument_pointers, zend_argument_count);";
		$lines[] = "uint32_t i;";
		$lines[] = "for(i = 0; i < zend_argument_count; i++) {";
		$lines[] =		"zval *zarg = zend_arguments[i];";
		$lines[] =		"zval_dtor(zarg);";
		$lines[] = "}";
		$lines[] = "zend_argument_count = 0;";
		$lines[] = "zend_argument_pointer_count = 0;";
		return $lines;
;
	}
	
}

?>