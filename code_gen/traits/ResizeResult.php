<?php

trait ResizeResult {

	public function adjustsResultSize() {
		return true;
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