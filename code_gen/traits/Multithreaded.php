<?php

trait Multithreaded {

	public function isMultithreaded() {
		if($this->mayExitLoop()) {
			return false;
		} else {
			return true;
		}
	}
}


?>