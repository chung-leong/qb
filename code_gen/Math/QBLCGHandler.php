<?php

class QBLCGHandler extends QBHandler {

	public function getInputOperandCount() {
		return 0;
	}

	public function needsInterpreterContext() {
		return true;
	}

	public function getFunctionType() {
		// the operation is slow--no use inlining it
		return "extern";
	}
	
	protected function getActionOnUnitData() {
		$cType = $this->getOperandCType(1);
		$lines = array();
		$lines[] = "USE_TSRM";
		$lines[] = "res = ($cType) php_combined_lcg(TSRMLS_C);";
		return $lines;
	}
}

?>