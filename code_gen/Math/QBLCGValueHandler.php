<?php

class QBLCGValueHandler extends QBHandler {

	public function getInputOperandCount() {
		return 0;
	}

	protected function getActionOnUnitData() {
		$cType = $this->getOperandCType(1);
		return "res = ($cType) php_combined_lcg(TSRMLS_C);";
	}
}

?>