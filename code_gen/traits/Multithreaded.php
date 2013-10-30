<?php

trait Multithreaded {

	public function isMultithreaded() {
		return true;
	}
	
	public function getMultithreadingThreshold() {
		// should be optimized for each handler
		return 1024;
	}
}

?>