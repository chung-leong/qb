<?php

trait ResizeResult {

	public function changesOperandSize($i) {
		$srcCount = $this->getInputOperandCount();
		return ($i > $srcCount);
	}
	
	public function needsInterpreterContext() {
		return true;
	}
	
	public function getHandlerFunctionType() {
		// always do it inside a function
		return "extern";
	}
}

?>