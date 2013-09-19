<?php

class Terminate extends Handler {
	
	use ScalarAddressMode, ExitFunction, IntegerOnly;
	
	public function getInputOperandCount() {
		return 1;
	}
	
	public function getAction() {
		$lines = array();
		$lines[] = "EG(exit_status) = op1;"; 
		$lines[] = "zend_bailout();";
		return $lines;
	}
}

?>