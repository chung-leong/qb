<?php

trait MayEmitError {

	public function needsInterpreterContext() {
		return true;
	}
	
	public function needsLineIdentifier($where = null) {
		return true;
	}
	
	public function mayExitLoop() {
		return true;
	}
}

?>