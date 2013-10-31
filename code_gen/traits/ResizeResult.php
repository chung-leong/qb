<?php

trait ResizeResult {

	use MainThreadExecution;

	public function changesOperandSize($i) {
		$srcCount = $this->getInputOperandCount();
		return ($i > $srcCount);
	}
	
	public function getHandlerFunctionType() {
		// always do it inside a function
		return "extern";
	}
}

?>