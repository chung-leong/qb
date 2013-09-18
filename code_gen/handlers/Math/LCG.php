<?php

class LCG extends Handler {

	use MultipleAddressMode, NullaryOperator, FloatingPointOnly, Slow;

	public function needsInterpreterContext() {
		return true;
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