<?php

class GetTime extends Handler {

	use MultipleAddressMode, NullaryOperator, FloatingPointOnly, Slow;
	
	protected function getActionOnUnitData() {
		$lines = array();
		$lines[] = "struct timeval tp;";
		$lines[] = "if(gettimeofday(&tp, NULL) != -1) {";
		$lines[] =		"res = tp.tv_sec + tp.tv_usec / 1000000.0;";
		$lines[] = "} else {";
		$lines[] =		"res = 0;";
		$lines[] = "}";
		return $lines;
	}

	public function runsInMainThread() {
		if(self::$compiler == "MSVC") {
			// the win32 implementation of gettimeofday() uses TSRM
			return true;
		} else {
			return false;
		}
	}
}

?>
