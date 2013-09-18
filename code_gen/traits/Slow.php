<?php

trait Slow {

	// don't inline handler function if the operation is going to be slow anyway
	public function getFunctionType() {
		return "extern";
	}
}

?>