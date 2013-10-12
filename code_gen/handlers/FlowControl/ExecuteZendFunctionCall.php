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
		return 2;
	}
	
	public function needsInterpreterContext() {
		return true;
	}
	
	public function needsLineNumber() {
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
		$lines[] = "qb_variable *retvar = (op2 != (uint32_t) -1) ? function->variables[op2] : NULL;";
		$lines[] = "qb_execute_zend_function_call(cxt, local_storage, retvar, zfunc, &zend_argument_stack, line_number);";
		return $lines;
	}
	
}

?>