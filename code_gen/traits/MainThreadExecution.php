<?php

trait MainThreadExecution {

	public function needsInterpreterContext() {
		return true;
	}
	
	public function runsInMainThread() {
		return true;
	}
}

?>