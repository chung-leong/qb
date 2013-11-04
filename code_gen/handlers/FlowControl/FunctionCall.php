<?php

class FunctionCall extends Handler {

	public function __construct($baseName) {
		$this->baseName = $baseName;
		$this->operandType = "U32";
		$this->operandSize = 1;
		$this->addressMode = "ARR";
		$this->multipleData = false;
	}
	
	public function getInputOperandCount() {
		return 3;
	}
	
	public function getOperandAddressMode($i) {
		switch($i) {
			case 1: return "SCA";
			case 2: return "ARR";
			case 3: return "SCA";
		}
	}
	
	public function getHandlerFunctionType() {
		return null;
	}
	
	public function needsInterpreterContext() {
		return true;
	}
	
	public function needsLineNumber() {
		return true;
	}
	
	public function getActionOnUnitData() {
		$lines = array();
		$lines[] = "qb_dispatch_function_call(cxt, op1, op2_ptr, op2_count, op3, line_number);";
		$lines[] = "if(UNEXPECTED(cxt->exception_encountered)) {";
		$lines[] =		"cxt->exit_type = QB_VM_EXCEPTION;";
		$lines[] =		"return;";
		$lines[] = "}";
		return $lines;
	}
	
}

?>