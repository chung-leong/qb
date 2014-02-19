<?php

class Flush extends Handler {

	use NoOperands, MainThreadExecution;
	
	public function getActionOnUnitData() {
		$lines = array();
		$lines[] = "USE_TSRM";
		$lines[] = "sapi_flush(TSRMLS_C);";
		return $lines;
	}
}

?>