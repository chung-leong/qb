<?php

class Terminate extends Handler {
	
	use ScalarAddressMode, ExitFunction, UnaryOperator, IntegerOnly;
	
	public function getAction() {
		$lines = array();
		$lines[] = "EG(exit_status) = op1;"; 
		$lines[] = "zend_bailout();";
		return $lines;
	}
}

?>