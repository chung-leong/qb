<?php

trait MayEmitError {

	public function needsInterpreterContext() {
		return true;
	}
	
	public function needsLineNumber($where = null) {
		return true;
	}
}

?>